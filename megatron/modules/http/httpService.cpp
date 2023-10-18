#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include <stdlib.h>
//#ifndef _WIN32
#include <unistd.h>
#include <string>
#include <unknown.h>
#include <IInstance.h>
#include <tools_mt.h>
#include "httpService.h"
#include "IUtils.h"
#include <Events/System/Net/socket/Connected.h>
#include <Events/System/Net/socket/AddToListenTCP.h>
#include <Events/System/Net/rpc/IncomingOnAcceptor.h>
#include <Events/System/Net/http/RequestIncoming.h>
#include <version_mega.h>
#include <st_malloc.h>
#include <logging.h>

//#endif
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include "events_http.hpp"

extern char mime_types[];
extern int mime_types_sz;
std::string datef(const time_t &__t);



UnknownBase* HTTP::Service::construct(const SERVICE_id &id, const std::string& nm,IInstance* _if)
{
    XTRY;
    return new Service(id,nm,_if);
    XPASS;
}


HTTP::Service::Service(const SERVICE_id& id, const std::string&nm, IInstance* _if):
    UnknownBase(nm),Broadcaster(_if),
    ListenerSimple(nm,_if->getConfig(),id),
//    _stuff(new __http_stuff),
    iInstance(_if)
{
    m_maxPost= static_cast<size_t>(_if->getConfig()->get_int64_t("max_post", 1000000, ""));
    {
        {
            M_LOCK(mx);
            mx.docUrls=_if->getConfig()->get_stringset("doc_urls","/pics,/html,/css","");
            mx.documentRoot=_if->getConfig()->get_string("document_root","./www","");
        }
        try {
            std::string bod;
            bod=std::string(mime_types, static_cast<unsigned int>(mime_types_sz));

            {
                M_LOCK(mx);
                mx.mime_types.clear();
            }
            std::vector<std::string> v=iUtils->splitString("\r\n",bod);
            for (size_t i=0; i<v.size(); i++)
            {
                if (v[i].size())
                {
                    if (v[i][0]=='#') continue;
                }
                std::deque<std::string> dq=iUtils->splitStringDQ("\t ",v[i]);
                if (dq.size())
                {
                    std::string typ=dq[0];
                    dq.pop_front();
                    while (dq.size())
                    {

                        {
                            M_LOCK(mx);
                            mx.mime_types[dq[0]]=typ;
                        }
                        dq.pop_front();
                    }
                }
            }
        }
        catch(...) {}
    }
}

bool HTTP::Service::on_Connected(const socketEvent::Connected*)
{
    MUTEX_INSPECTOR;

    return true;

}
bool HTTP::Service::on_NotifyBindAddress(const socketEvent::NotifyBindAddress*e)
{
    MUTEX_INSPECTOR;

    M_LOCK(mx);
    mx.bind_addrs.insert(e->addr);
    return true;
}


bool HTTP::Service::on_DoListen(const httpEvent::DoListen* e)
{
    MUTEX_INSPECTOR;

    SOCKET_id newid=iUtils->getSocketId();
    msockaddr_in sa=e->addr;
    DBG(logErr2("on_DoListen %s",e->route.dump().c_str()));
    sendEvent(socketListener,new socketEvent::AddToListenTCP(newid,sa,"HTTP",false,e->route));

    return true;
}

bool HTTP::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;
    socketListener=dynamic_cast<ListenerBase*>(iInstance->getServiceOrCreate(ServiceEnum::Socket));
    if(!socketListener)
        throw CommonError("if(!socketListener)");
    return true;
}
bool HTTP::Service::on_GetBindPortsREQ(const httpEvent::GetBindPortsREQ *e)
{
    MUTEX_INSPECTOR;
    M_LOCK(mx);
    passEvent(new httpEvent::GetBindPortsRSP(mx.bind_addrs,poppedFrontRoute(e->route)));
    return true;
}

bool HTTP::Service::handleEvent(const REF_getter<Event::Base>& evt)
{
    MUTEX_INSPECTOR;
    auto &ID=evt->id;


    if( socketEventEnum::Disaccepted==ID)
        return on_Disaccepted((const socketEvent::Disaccepted*)evt.operator->());
    if( socketEventEnum::Disconnected==ID)
        return on_Disconnected((const socketEvent::Disconnected*)evt.operator->());

    if( socketEventEnum::Accepted==ID)
        return on_Accepted((const socketEvent::Accepted*)evt.operator->());
    if( socketEventEnum::StreamRead==ID)
        return on_StreamRead((const socketEvent::StreamRead*)evt.operator->());
    if( socketEventEnum::Connected==ID)
        return on_Connected((const socketEvent::Connected*)evt.operator->());
    if( socketEventEnum::NotifyBindAddress==ID)
        return on_NotifyBindAddress((const socketEvent::NotifyBindAddress*)evt.operator->());
    if( socketEventEnum::NotifyOutBufferEmpty==ID)
        return on_NotifyOutBufferEmpty((const socketEvent::NotifyOutBufferEmpty*)evt.operator->());
    if( httpEventEnum::DoListen==ID)
        return(this->on_DoListen((const httpEvent::DoListen*)evt.operator->()));
    if( httpEventEnum::RegisterProtocol==ID)
        return(this->on_RegisterProtocol((const httpEvent::RegisterProtocol*)evt.operator->()));
    if( httpEventEnum::GetBindPortsREQ==ID)
        return(this->on_GetBindPortsREQ((const httpEvent::GetBindPortsREQ*)evt.operator->()));
    if( systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)evt.operator->());
    if( rpcEventEnum::IncomingOnAcceptor==ID)
    {
        MUTEX_INSPECTOR;
        rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *)evt.operator ->();
        auto IDA=E->e->id;
        if(httpEventEnum::GetBindPortsREQ==IDA)
        {
            MUTEX_INSPECTOR;
            const httpEvent::GetBindPortsREQ *e=(const httpEvent::GetBindPortsREQ *)E->e.operator ->();
            M_LOCK(mx);
            passEvent(new httpEvent::GetBindPortsRSP(mx.bind_addrs,poppedFrontRoute(e->route)));
            return true;
        }
        return false;
    }

    return false;
}

bool HTTP::Service::on_Accepted(const socketEvent::Accepted* evt)
{
    MUTEX_INSPECTOR;
    return true;
}
bool HTTP::Service::on_StreamRead(const socketEvent::StreamRead* evt)
{
    MUTEX_INSPECTOR;


    REF_getter<HTTP::Request> W=getData(evt->esi.operator->());


    W->m_last_io_time=time(NULL);
    if (W->parse_state.count(1)==0)
    {
        std::string head;
        {
            M_LOCK(evt->esi->m_inBuffer);
            if (!W.___ptr->__gets$(head,"\r\n\r\n", evt->esi->m_inBuffer._mx_data))
            {
                return true;
            }
        }
        std::deque<std::string> dq=iUtils->splitStringDQ("\r\n",head);
        if (dq.size())
        {

            std::string fl=dq[0];
            dq.pop_front();
            std::string::size_type pom = fl.find(" ", 0);
            if (pom == std::string::npos)
            {
                return true;
            }
            (std::string&)W->METHOD =iUtils->strupper(fl.substr(0, pom));

            if (W->METHOD != "GET" && W->METHOD != "POST")
            {
                return true;
            }

            W->url = fl.substr(pom + 1, fl.find(" ", pom + 1) - pom - 1);
            W->url = iUtils->unescapeURL(W->url);
            if (W->url.find("..", 0) != std::string::npos)
            {
                return true;
            }
        }
        while (dq.size())
        {
            std::string	line=dq[0];
            dq.pop_front();
            std::string::size_type pop = line.find(":", 0);
            if (pop == std::string::npos)
            {
                return true;
            }
            std::string k=iUtils->strupper(line.substr(0, pop));
            std::string v=line.substr(pop + 2, line.length() - pop - 2);
            W->headers[k]=v;
        }
        if (W->headers.count("COOKIE"))
        {

            std::vector <std::string> v = iUtils->splitString("; ", W->headers["COOKIE"]);
            for (unsigned int i = 0; i < v.size(); i++)
            {
                std::string q = v[i];
                if (q == " ")
                {
                    continue;
                }
                size_t z;
                z = q.find("=", 0);
                if (z == std::string::npos)
                {
                    continue;
                }
                std::string k=q.substr(0, z);
                std::string v=q.substr(z + 1, q.size() - z - 1);
                W->in_cookies[k] = v;
            }
        }
        W->original_url=W->url;
        {

            std::string::size_type qp = W->url.find("?", 0);
            if (qp != std::string::npos)
            {
                W->original_params=W->url.substr(qp + 1, W->url.size() - qp - 1);
                W->split_params(W->original_params);
                W->url = W->url.substr(0, qp);
            }
        }

        W->parse_state.insert(1);
    }

    if (W->parse_state.count(2)==0)
    {

        if (W->METHOD == "POST")
        {

            if (W->headers["CONTENT-TYPE"].find("multipart/form", 0) == std::string::npos)
            {

                size_t clen = atoi(W->headers["CONTENT-LENGTH"].c_str());
                if (clen <= 0 || clen > m_maxPost)
                {
                    return true;
                }
                {
                    M_LOCK(evt->esi->m_inBuffer);
                    if (!W->__readbuf$(W->postContent,clen, evt->esi->m_inBuffer._mx_data)) return true;
                }
                W->split_params(W->postContent);
            }
            else
            {


                //Multipart form
                const std::string &t= W->headers["CONTENT-TYPE"];
                std::string::size_type pz = t.find("boundary=", 0);
                if (pz == std::string::npos)
                {

                    return true;
                }
                std::string bound = "--" + t.substr(pz + 9, t.length() - pz - 9);
                std::string ebound = bound + "--";
                std::string sbuf;
                {

                    {
                        M_LOCK(evt->esi->m_inBuffer);
                        if (!W->__gets$(sbuf,ebound, evt->esi->m_inBuffer._mx_data)) return true;
                    }

                    while (sbuf.size())
                    {


                        std::string s;
                        std::string::size_type pos=sbuf.find(bound);
                        if (pos==std::string::npos)
                        {
                            s=sbuf;
                            sbuf="";
                        }
                        else
                        {
                            s=sbuf.substr(0,pos);
                            sbuf=sbuf.substr(pos+bound.size(),sbuf.size()-pos-bound.size());
                        }
                        if (s.size()>1)
                        {
                            if (s[0]=='\r'&&s[1]=='\n') s=s.substr(2,s.size()-2);
                        }
                        if (s.size())
                        {

                            std::map<std::string,std::string> lparams;
                            std::string content;
                            while (1)
                            {

                                std::string sloc;
                                std::string::size_type pp=s.find("\r\n");
                                if (pp==std::string::npos)
                                {
                                    logErr2("bad multipart %s %d",__FILE__,__LINE__);
                                    return true;
                                }
                                sloc=s.substr(0,pp);
                                s=s.substr(pp+2,s.size()-pp-2);
                                if (!sloc.size())
                                {
                                    content=s.substr(0,s.size()-2);
                                    break;
                                }
                                std::string::size_type pz=sloc.find(": ");
                                if (pz==std::string::npos)
                                {
                                    logErr2("bad multipart %s %d",__FILE__,__LINE__);
                                    return  true;
                                }
                                std::string key=sloc.substr(0,pz);
                                std::string val=sloc.substr(pz+2,sloc.size()-pz-2);
                                lparams[key]=val;
                            }
                            if (lparams.count("Content-Disposition"))
                            {

                                std::string cd=lparams["Content-Disposition"];
                                std::vector<std::string> flds;
                                while (cd.size())
                                {
                                    std::string::size_type ps=cd.find("; ");
                                    if (ps==std::string::npos)
                                    {
                                        flds.push_back(cd);
                                        cd="";
                                    }
                                    else
                                    {
                                        flds.push_back(cd.substr(0,ps));
                                        cd=cd.substr(ps+2,cd.size()-2);
                                    }
                                }
                                std::string name;
                                std::string filename;
                                for (unsigned i=0; i<flds.size(); i++)
                                {
                                    std::string::size_type pzz=flds[i].find("=");
                                    if (pzz!=std::string::npos)
                                    {
                                        std::string key=flds[i].substr(0,pzz);
                                        std::string val=flds[i].substr(pzz+1,flds[i].size()-1);
                                        if (val.size()>2)
                                        {
                                            if (val[0]=='\"' && val[val.size()-1]=='\"') val=val.substr(1,val.size()-2);
                                        }
                                        if (val=="\"\"") val="";
                                        if (key=="name") name=val;
                                        if (key=="filename") filename=val;
                                    }
                                }
                                W->params[name]=content;
                                if (filename.size())
                                    W->params[name+"_FILENAME"]=filename;
                                for (auto& i:lparams)
                                {
                                    std::string q = iUtils->strupper(i.first);
                                    std::string k=name + "_" + q;
                                    W->params[k] = i.second;
                                }
                            }
                        }
                    }
                }
            }
        }
        W->parse_state.insert(2);
    }
    if(W->headers.count("CONNECTION"))
    {
        if(iUtils->strlower(W->headers["CONNECTION"])=="keep-alive")
        {
            W->isKeepAlive=true;
        }
        else
        {
            W->isKeepAlive=false;
        }
    }
    else
    {
    }


    if(!W->sendRequestIncomingIsSent)
    {
        W->sendRequestIncomingIsSent=true;
        passEvent(new httpEvent::RequestIncoming(W,evt->esi,evt->route));
        clearData(evt->esi.operator->());
    }
    return  true;
}

std::string HTTP::Service::get_mime_type(const std::string& mime) const
{
    MUTEX_INSPECTOR;
    M_LOCK(mx);
    auto i=mx.mime_types.find(mime);
    if (i==mx.mime_types.end()) return "";
    else return i->second;
}
bool HTTP::Service::on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* e)
{
    MUTEX_INSPECTOR;

    return true;
    S_LOG("on_NotifyOutBufferEmpty");

    REF_getter<HTTP::Request> W=getData(e->esi.operator->());
    if(!W.valid())
    {

        e->esi->close("HTTP::Request not exists");
        return true;
    }
    REF_getter<HTTP::Request::_fileresponse> F=W->fileresponse;
    if(!F.valid())
        return true;
    if(F->fileSize==0)
    {
        e->esi->close("HTTPService: on_NotifyOutBufferEmpty: F->fileSize==0 @1");
        return true;
    }
    if(F->fileSize==-1)
    {
        e->esi->close("HTTPService: on_NotifyOutBufferEmpty: F->fileSize==-1 @2");
        return true;
    }
    if(F->contentLength<=F->written_bytes)
    {
        e->esi->close("HTTPService: on_NotifyOutBufferEmpty: F->contentLength>=F->written_bytes");
        return true;
    }
    int64_t offset=F->startb+F->written_bytes;
    {
        size_t bufsize=0x10000;
        st_malloc buf(bufsize);
        {

            int64_t readSize=bufsize;
            if(F->written_bytes+readSize > F->contentLength)
            {
                readSize=F->contentLength-F->written_bytes;
            }

            if(offset+readSize>F->fileSize)
            {
                readSize=F->fileSize-offset;
            }

            int64_t res=-1;
            if(W->fileresponse->io_protocol.m_read)
            {

                res=F->io_protocol.m_read(F->get_fd(),(unsigned char*)buf.buf,(size_t)readSize);
            }
            else
            {

                res=read(F->get_fd(),(char*)buf.buf,(size_t)readSize);
            }

            if(res==-1)
            {

                e->esi->close("HTTPService: on_NotifyOutBufferEmpty: read returns -1");
                return true;
            }

            if(res==0)
            {

                e->esi->close("HTTPService: on_NotifyOutBufferEmpty: read returns 0 (EOF)");
                return true;
            }

            e->esi->write_(toRef((uint8_t*)buf.buf,res));
            F->written_bytes+=res;
            return true;
        }
    }
    return true;
}

void registerHTTPModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::HTTP,"HTTP",getEvents_http());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::HTTP,HTTP::Service::construct,"HTTP");
        regEvents_http();
    }
}

std::string datef(const time_t &__t)
{
    MUTEX_INSPECTOR;
    time_t t=__t-100000;
    static char const * wkday[]        = {"Sun","Mon","Tue","Wed", "Thu","Fri", "Sat"};
    static char const* month[]        = {"Jan", "Feb", "Mar", "Apr"
                                         , "May", "Jun", "Jul", "Aug"
                                         , "Sep", "Oct", "Nov", "Dec"
                                        };

    char outstr[200];
    struct tm tt=*localtime(&t);
    snprintf(outstr,sizeof(outstr),"%s, %02d %s %d %02d:%02d:%02d GMT",
             wkday[tt.tm_wday%7],tt.tm_mday,month[tt.tm_mon%12],tt.tm_year+1900,tt.tm_hour,tt.tm_min,tt.tm_sec);
    return outstr;
}

/** -1 error*/ int HTTP::Service::send_other_from_disk_ext(const REF_getter<epoll_socket_info>&esi,const REF_getter<HTTP::Request>&W,const std::string & fn,const std::string& exten)
{

    MUTEX_INSPECTOR;

    W->m_last_io_time=time(NULL);

    REF_getter<HTTP::Request::_fileresponse> F=W->fileresponse;
    if(F.valid())
    {
        W->fileresponse=new HTTP::Request::_fileresponse;
        F=W->fileresponse;
    }
    F->fileName=fn;
    F->extension=exten;

    {
        if(F->io_protocol.m_open)
        {
            F->set_fd(F->io_protocol.m_open(fn.c_str(),O_RDONLY));
        }
        else
        {
            F->set_fd(::open(fn.c_str(),O_RDONLY));
        }
        if(F->get_fd()==-1)
        {
            std::string a("HTTP/1.1 404 Not Found\r\n");
            a.append("Server: nginx/1.2.6 (Ubuntu)\r\n");
            a.append("Connection: close\r\n");
            a.append("\r\n");
            esi->write_(toRef(a));
            return -1;
        }

        if(F->io_protocol.m_seek)
        {

            F->fileSize=F->io_protocol.m_seek(F->get_fd(),0,SEEK_END);

        }
        else
        {
#if defined __MACH__ || defined __FreeBSD__
            F->fileSize=lseek(F->get_fd(),0,SEEK_END);
#else
            F->fileSize=lseek64(F->get_fd(),0,SEEK_END);
#endif

        }
        if(F->fileSize==-1)
        {

            std::string a("HTTP/1.1 500 Internal Server Error\r\n");
            a.append("Server: nginx/1.2.6 (Ubuntu)\r\n");
            a.append("Connection: close\r\n");
            a.append("\r\n");
            esi->write_(a);

            return -1;
        }
    }

    F->contentLength=0;
    if (!W->headers.count("RANGE"))
    {

        F->startb=0;
        F->endb=W->fileresponse->fileSize-1;
        F->written_bytes=0;
        F->hasRange=false;

    }
    else
    {

        F->startb=0;
        F->endb=F->fileSize-1;
        F->hasRange=true;
        std::string s = W->headers["RANGE"];
        size_t n = s.find("=", 0);
        if (n != std::string::npos)
        {
            F->hasRange= true;
            std::vector <std::string> v = iUtils->splitString("-=", s.substr(n, s.size() - n));
            if (v.size() >= 1)
            {
                F->startb = atoll(v[0].c_str());
            }
            if (v.size() == 2)
            {
                F->endb  = atoll(v[1].c_str());
            }
            else
            {
                F->endb=F->fileSize-1;
            }
        }
    }

    if (F->startb > F->fileSize || F->startb < 0)
        F->startb = 0;


    if (F->endb > F->fileSize || F->endb < 0)
        F->endb = F->fileSize-1;


    if (F->endb < F->startb)
    {
        F->endb = F->fileSize-1;
        F->startb = 0;
    }

    if(F->endb==0)
        F->endb=F->fileSize-1;

    F->contentLength=F->endb-F->startb+1;


    time_t last_modified;
    {
        M_LOCK(lastModified);
        if(lastModified.container.count(W->url))
            last_modified=lastModified.container[W->url];
        else
        {
            last_modified=time(NULL);
            lastModified.container[W->url]=last_modified;
        }

    }

    std::vector<std::string> out;
    if(F->hasRange)
        out.push_back("HTTP/1.1 206 Partial Content\r\n");
    else
        out.push_back("HTTP/1.1 200 OK\r\n");
    out.push_back("Server: nginx/1.2.6 (Ubuntu)\r\n");

    out.push_back("Date: "+datef(time(NULL))+"\r\n");

    {
        M_LOCK(mx);
        auto i=mx.mime_types.find(exten);
        if(i!=mx.mime_types.end())
            out.push_back("Content-Type: "+i->second+"\r\n");
        else
            out.push_back("Content-Type: text/plain\r\n");
    }


    out.push_back("Content-length: "+std::to_string(int64_t(F->contentLength))+"\r\n");

    out.push_back("Last-Modified: "+datef(last_modified)+"\r\n");

    if(W->isKeepAlive)
        out.push_back("Connection: keep-alive\r\n");
    else
        out.push_back("Connection: close\r\n");


    if(F->hasRange)
    {
        out.push_back("Content-Range: bytes "
                      +  std::to_string((int64_t)F->startb) + "-"
                      +  std::to_string((int64_t)F->endb) + "/"
                      +  std::to_string((int64_t)F->fileSize)+"\r\n");
    }
    else
    {
        out.push_back("Accept-Ranges: bytes\r\n");
    }


    out.push_back("\r\n");

    for(size_t i=0; i<out.size(); i++)
    {
        esi->write_(out[i]);
    }


    F->headerSent=true;

    if(F->io_protocol.m_seek)
    {

        F->io_protocol.m_seek(F->get_fd(),F->startb,SEEK_SET);

    }
    else
    {
#if defined __MACH__ || defined __FreeBSD__
        lseek(F->get_fd(),F->startb,SEEK_SET);
#else
        lseek64(F->get_fd(),F->startb,SEEK_SET);
#endif

    }

    return 0;
}



bool HTTP::Service::on_Disaccepted(const socketEvent::Disaccepted*e)
{
    MUTEX_INSPECTOR;
    clearData(e->esi.operator->());
    return true;
}
bool HTTP::Service::on_Disconnected(const socketEvent::Disconnected*e)
{
    MUTEX_INSPECTOR;
    clearData(e->esi.operator->());
    return true;
}


REF_getter<HTTP::Request> HTTP::Service::getData(epoll_socket_info* esi)
{

    auto it=esi->additions.find('http');
    if(it==esi->additions.end())
    {
        REF_getter<Refcountable> p=new HTTP::Request;
        esi->additions.insert(std::make_pair('http',p));
        it=esi->additions.find('http');
    }
    auto ret=dynamic_cast<HTTP::Request*>(it->second.operator->());
    if(ret==NULL)
        throw CommonError("if(ret==NULL)");
    return ret;

}
void HTTP::Service::setData(epoll_socket_info* esi, const REF_getter<HTTP::Request> & p)
{
    esi->additions.insert(std::make_pair('http',p.operator->()));

}
void HTTP::Service::clearData(epoll_socket_info* esi)
{
    esi->additions.erase('http');

}
