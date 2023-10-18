#include "tools_mt.h"
#include "sinTestClientService.h"
#include "events_sinTestClient.hpp"


std::string sinTestClient::Service::genId()
{
    std::string ret;
    for(int i=0; i<20; i++)
    {
        ret+='0'+rand()%10;
    }
    return ret;
}

bool sinTestClient::Service::on_startService(const systemEvent::startService*)
{
    sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_START,NULL,NULL,3,ListenerBase::serviceId));
    return true;
}
bool sinTestClient::Service::on_TickAlarm(const timerEvent::TickAlarm* e)
{
    if(e->tid==TI_START)
    {
        auto taskId=genId();
        /// send request to server
        REF_getter<sinTestEvent::AddTaskREQ> rq=new sinTestEvent::AddTaskREQ(taskId,"give me report on july 2023", ListenerBase::serviceId);
        requests_.insert({taskId,rq});

        sendEvent(sinTestServerAddr_,ServiceEnum::sinTestServer,rq.operator->());
        logErr2("client: TI_START sent request %s",taskId.c_str());



        /// set timer to notify to resend request
        sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_RESEND_REQUEST,toRef(taskId),NULL,20,ListenerBase::serviceId));
        return true;
    }
    else if(e->tid == TI_RESEND_REQUEST)
    {
        auto taskId=e->data->asString();
        auto it=requests_.find(taskId);
        if(it!=requests_.end())
        {
            /// resend req
            sendEvent(sinTestServerAddr_,ServiceEnum::sinTestServer,it->second.operator->());
            logErr2("client: TI_RESEND_REQUEST sent request %s",taskId.c_str());

            /// set timer to notify to resend request
            sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_RESEND_REQUEST,toRef(taskId),NULL,20,ListenerBase::serviceId));
        }
        else throw CommonError("!if(it!=requests.end())");
        return true;
    }
    return  false;
}
bool sinTestClient::Service::on_GetResultRSP(const sinTestEvent::GetResultRSP*e)
{
    if(e->status=="ok")
    {
        logErr2("client: received document id=%s body=%s",e->taskId.c_str(),e->document.c_str());
        requests_.erase(e->taskId);
        return true;
    }
    else if(e->status=="not_found")
    {
        logErr2("client: received document response id=%s body=%s status=%s",e->taskId.c_str(),e->document.c_str(),e->status.c_str());
        /// run request again
        sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_RESEND_REQUEST,toRef(e->taskId),NULL,1,ListenerBase::serviceId));

        return true;
    }
    /// false if unhandled case
    return false;
}

bool sinTestClient::Service::on_AddTaskRSP(const sinTestEvent::AddTaskRSP*e)
{

    if(e->status=="in_process")
    {
        /// clear resend alarm
        sendEvent(ServiceEnum::Timer,new timerEvent::StopAlarm(TI_RESEND_REQUEST,toRef(e->taskId),ListenerBase::serviceId));
        return true;
    }
    else if(e->status=="done")
    {
        /// request for document body
        sendEvent(sinTestServerAddr_,ServiceEnum::sinTestServer,new sinTestEvent::GetResultREQ(e->taskId,ListenerBase::serviceId));
    }
    return true;
}


bool sinTestClient::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        auto& ID=e->id;

        if(timerEventEnum::TickAlarm==ID)
            return on_TickAlarm((const timerEvent::TickAlarm*)e.operator->());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.operator->());


        if(sinTestEventEnum::AddTaskRSP==ID)
            return on_AddTaskRSP((const sinTestEvent::AddTaskRSP*)e.operator->());
        if(sinTestEventEnum::GetResultRSP==ID)
            return on_GetResultRSP((const sinTestEvent::GetResultRSP*)e.operator->());

        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.operator->();
            auto& IDC=E->e->id;
            if(sinTestEventEnum::AddTaskRSP==IDC)
                return on_AddTaskRSP((const sinTestEvent::AddTaskRSP*)E->e.operator->());
            if(sinTestEventEnum::GetResultRSP==IDC)
                return on_GetResultRSP((const sinTestEvent::GetResultRSP*)E->e.operator->());


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.operator->();
            auto& IDA=E->e->id;
            if(sinTestEventEnum::AddTaskRSP==IDA)
                return on_AddTaskRSP((const sinTestEvent::AddTaskRSP*)E->e.operator->());
            if(sinTestEventEnum::GetResultRSP==IDA)
                return on_GetResultRSP((const sinTestEvent::GetResultRSP*)E->e.operator->());


            return false;
        }


    }
    catch(CommonError& e)
    {
        logErr2("sinTestWebServer CommonError  %s",e.what());

    }
    catch(std::exception &e)
    {
        logErr2("sinTestWebServer std::exception  %s",e.what());
    }
    XPASS;
    return false;
}

sinTestClient::Service::~Service()
{
}


sinTestClient::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ins->getConfig(),id,ins),
    Broadcaster(ins)
{
    auto ba=ins->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","http listen address");
    if(ba.size()==0)
        throw CommonError("if(ba.size()==0)");

    bindAddr_=*ba.begin();
    sinTestServerAddr_=ins->getConfig()->get_string("sinTestServerAddr","local","server sinTest address");

}

void registersinTestClientService(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::sinTestClient,"sinTestClient",getEvents_sinTestClient());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::sinTestClient,sinTestClient::Service::construct,"sinTestClient");
        regEvents_sinTestClient();
    }
}
