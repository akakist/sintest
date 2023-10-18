#include <string>
#include "ITests.h"
#include "IUtils.h"
#include "version_mega.h"
#include "listenerBuffered.h"
#include "broadcaster.h"
#include "main/CInstance.h"
#include "Events/System/Run/startService.h"
#include "tools_mt.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "Events/System/Net/rpc/SubscribeNotifications.h"
#include "Events/System/timer/TickAlarm.h"
#include "colorOutput.h"
#include "ISSL.h"
#include "main/configObj.h"
static bool done_test=false;
#define BUF_SIZE_MAX (8*100*1)
#define N_PONG 12000
#define TI_ACTIVITY_VALUE 2.0
//#define USE_LOCAL 1

#define REMOTE_ADDR "localhost:2001"
static int session=0;
namespace ServiceEnum {
    const SERVICE_id rpcTest("rpcTest");
    const SERVICE_id rpcTestService1("rpcTestService1");
    const SERVICE_id rpcTestService2("rpcTestService2");
}
namespace testEventEnum {
    const EVENT_id testREQ("testREQ");
    const EVENT_id testRSP("testRSP");
}
enum timers
{
    TI_START,
    TI_ACTIVITY
};
namespace testEvent {
    class testREQ: public Event::Base
    {
        public:
        static Base* construct(const route_t &r)
        {
            return new testREQ(r);
        }
        testREQ(int session_,int _seq,const std::string &_buf, const std::string& _md5, const route_t& r)
        :Base(testEventEnum::testREQ,r),
             session(session_), seq(_seq),buf(_buf),md5(_md5)
        {
        }
        testREQ(const route_t& r)
            :Base(testEventEnum::testREQ,r)
        {
        }
        int session;
        int seq;
        std::string buf;
        std::string md5;
        void unpack(inBuffer& o)
        {
            o>>session>>seq>>buf>>md5;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<seq<<buf<<md5;
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class testRSP: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
    {
            return new testRSP(r);
        }
        testRSP(int session_,int _seq,const std::string &_buf, const std::string& _md5, const route_t& r)
            :Base(testEventEnum::testRSP,r), session(session_),seq(_seq),buf(_buf),md5(_md5) {}
        testRSP(const route_t& r):Base(testEventEnum::testRSP,r) {}

        int session;
        int seq;
        std::string buf;
        std::string md5;

        void unpack(inBuffer& o)
        {
            o>>session>>seq>>buf>>md5;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<seq<<buf<<md5;
        }
        void jdump(Json::Value &) const
        {
        }

    };
}

class rpcTestService1: public UnknownBase, public ListenerBuffered, public Broadcaster
{
public:
    I_ssl *ssl;
    void deinit()
    {
        ListenerBuffered::deinit();
    }

    static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
    {
        XTRY;
        return new rpcTestService1(id,nm,ifa);
        XPASS;
    }
    rpcTestService1(const SERVICE_id &id, const std::string&  nm,IInstance* ins):
        UnknownBase(nm),
        ListenerBuffered(nm,ins->getConfig(),id,ins),
        Broadcaster(ins)
    {
        ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);

    }
    void sendRequest(int session,int seq)
    {
        if(done_test)return;
        size_t sz=rand()%BUF_SIZE_MAX;
        //char s[sz];
        std::string buf;
        while(buf.size()<sz)
        {
            buf+="klallosalkjf;laskdfj;alskdf;alksdfa;lkdfa;lksd;alksdf";//[i]=i;//rand();
        }
        //std::string buf(s,sz);
        std::string md5=ssl->md5(buf);

        if(seq%1000==0)
            logErr2("send %d:%d",session,seq+1);
#ifdef USE_LOCAL
        sendEvent(ServiceEnum::rpcTestService2,new testEvent::testREQ(session,seq+1,buf,md5,ListenerBase::serviceId));
#else
        msockaddr_in sa;
        sa.initFromUrl(REMOTE_ADDR);
//        caps.insert(sa);
        sendEvent(sa,
                  ServiceEnum::rpcTestService2,new testEvent::testREQ(session,seq+1,buf,md5,ListenerBase::serviceId));
#endif

    }
    bool handleEvent(const REF_getter<Event::Base>& e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        //logErr2("@%s",__PRETTY_FUNCTION__);
        auto& ID=e->id;
        if(timerEventEnum::TickAlarm==ID)
        {
            timerEvent::TickAlarm* ee=(timerEvent::TickAlarm*)e.operator ->();
            if(ee->tid==TI_START)
            {
                sendRequest(session,0);
                //sendRequest(session+1,0);
                //sendRequest(session+2,0);
                //session+=3;
                return true;
            }
            if(ee->tid==TI_ACTIVITY)
            {
                session++;
                sendRequest(session,0);
                return true;
            }
            return true;
        }
        if(systemEventEnum::startService==ID)
        {
            MUTEX_INSPECTOR;
            /// timeout to wait second peer to bind
            sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_START,NULL,NULL,0.3,ListenerBase::serviceId));
            sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_ACTIVITY,NULL,NULL,TI_ACTIVITY_VALUE,ListenerBase::serviceId));

            return true;
        }
        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            MUTEX_INSPECTOR;
            rpcEvent::IncomingOnConnector *ioc=(rpcEvent::IncomingOnConnector *)e.operator ->();
            auto &IDC=ioc->e->id;
            if(testEventEnum::testRSP==IDC)
                return on_testRSP((testEvent::testRSP*)ioc->e.operator ->());
        }
        if(testEventEnum::testRSP==ID)
            return on_testRSP((testEvent::testRSP*)e.operator ->());
        return false;
    }
    bool on_testRSP(const testEvent::testRSP* e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        if(e->seq%1000==0)
            logErr2("on rsp %d",e->seq);
        sendEvent(ServiceEnum::Timer,new timerEvent::ResetAlarm(TI_ACTIVITY,NULL,NULL,TI_ACTIVITY_VALUE,ListenerBase::serviceId));

        //logErr2("@%s",__PRETTY_FUNCTION__);
        if(e->seq>N_PONG)
        {
            logErr2(GREEN("RPC test OK %d"),e->seq);
            done_test=true;
            return true;
        }
        if(ssl->md5(e->buf)!=e->md5)
        {
            throw CommonError("wrong md5 of buffer");
        }

        sendRequest(e->session,e->seq);

        return true;

    }
};
class rpcTestService2: public UnknownBase, public ListenerBuffered, public Broadcaster
{
public:
    I_ssl *ssl;
    void deinit()
    {
        ListenerBuffered::deinit();
    }

    static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
    {
        XTRY;
        return new rpcTestService2(id,nm,ifa);
        XPASS;
    }
    rpcTestService2(const SERVICE_id &id, const std::string&  nm,IInstance* ins):
        UnknownBase(nm),
        ListenerBuffered(nm,ins->getConfig(),id,ins),
        Broadcaster(ins)
    {
        ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);
    }
    bool on_testREQ(const testEvent::testREQ* e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        if(e->seq%1000==0)
            logErr2("recv %d",e->seq);
        if(ssl->md5(e->buf)!=e->md5)
        {
            throw CommonError("wrong md5 of buffer");
        }
        passEvent(new testEvent::testRSP(e->session,e->seq,e->buf,e->md5,poppedFrontRoute(e->route)));
        return true;

    }
    bool handleEvent(const REF_getter<Event::Base>& e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        //logErr2("@%s",__PRETTY_FUNCTION__);
        auto& ID=e->id;
        if(systemEventEnum::startService==ID)
        {
            return true;
        }
        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            MUTEX_INSPECTOR;
            rpcEvent::IncomingOnAcceptor *ioa=(rpcEvent::IncomingOnAcceptor *)e.operator ->();
            auto &IDA=ioa->e->id;
            if(testEventEnum::testREQ==IDA)
                return on_testREQ((testEvent::testREQ*)ioa->e.operator ->());

        }
        if(testEventEnum::testREQ==ID)
        {
            MUTEX_INSPECTOR;
            testEvent::testREQ* ee=(testEvent::testREQ*) e.operator ->();
            if(ssl->md5(ee->buf)!=ee->md5)
            {
                throw CommonError("wrong md5 of buffer");
            }
            //usleep(50000);
            passEvent(new testEvent::testRSP(ee->session,ee->seq,ee->buf,ee->md5,poppedFrontRoute(e->route)));
            return true;
        }
        return false;
    }
};




class rpcTest: public ITests::Base
{
public:
    int zzz;
    int run();
    std::string getName()
    {
        return "rpcTest";
    }

    rpcTest() {
        zzz=100;
    }
    ~rpcTest() {}
    static ITests::Base* construct()
    {
        XTRY;
        return new rpcTest;
        XPASS;
    }



};

int rpcTest::run()
{
    printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

    {
        IInstance *instance1=iUtils->createNewInstance("rpcTestService1");
        ConfigObj *cnf1=new ConfigObj("rpcTestService1",
                                      "\nRPC.ConnectionActivity=600.000000"
                                      "\nRPC.IterateTimeoutSec=60.000000"
                                      "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                      "\nStart=RPC,rpcTestService1"
                                      "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
                                      "\nOscarSecure.RSA_keysize=256"
                                      "\nOscarSecure.maxPacketSize=33554432"
                                      "\nRPC.BindAddr_MAIN=INADDR_ANY:2000"
                                      "\nRPC.BindAddr_RESERVE=NONE"
                                      "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                      "\nSocketIO.listen_backlog=128"
                                      "\nSocketIO.maxOutBufferSize=8388608"
                                      "\nSocketIO.size=1024"
                                      "\nSocketIO.timeout_millisec=10"
                                      "\nWebHandler.bindAddr=NONE"
                                      "\nrpcTestService1.ListenerBuffered.MaxThreadsCount=10"
                                     );
        instance1->setConfig(cnf1);
        instance1->initServices();
    }
    {
        IInstance *instance2=iUtils->createNewInstance("rpcTestService2");
        ConfigObj *cnf2=new ConfigObj("rpcTestService2",
                                      "\nRPC.ConnectionActivity=600.000000"
                                      "\nRPC.IterateTimeoutSec=60.000000"
                                      "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                      "\nStart=RPC,rpcTestService2"
                                      "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
                                      "\nOscarSecure.RSA_keysize=256"
                                      "\nOscarSecure.maxPacketSize=33554432"
                                      "\nRPC.BindAddr_MAIN=INADDR_ANY:2001"
                                      "\nRPC.BindAddr_RESERVE=NONE"
                                      "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                      "\nSocketIO.listen_backlog=128"
                                      "\nSocketIO.maxOutBufferSize=8388608"
                                      "\nSocketIO.size=1024"
                                      "\nSocketIO.timeout_millisec=10"
                                      "\nWebHandler.bindAddr=NONE"
                                      "\nrpcTestService2.ListenerBuffered.MaxThreadsCount=10"
                                     );
        instance2->setConfig(cnf2);
        instance2->initServices();
    }

    while(!done_test)
    {
        usleep(10000);
    }
//    sleep(1);

    return 0;
}


void registerRPCTest(const char* pn)
{
    if(pn)
    {

        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_TEST,ServiceEnum::rpcTest,"rpcTest",std::set<EVENT_id>());
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::rpcTestService1,"rpcTestService1",std::set<EVENT_id>());
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::rpcTestService2,"rpcTestService2",std::set<EVENT_id>());

    }
    else
    {
        iUtils->registerITest(COREVERSION,ServiceEnum::rpcTest,rpcTest::construct);
        iUtils->registerService(COREVERSION,ServiceEnum::rpcTestService1,rpcTestService1::construct,"rpcTestService1");
        iUtils->registerService(COREVERSION,ServiceEnum::rpcTestService2,rpcTestService2::construct,"rpcTestService2");
        iUtils->registerEvent(testEvent::testREQ::construct);
        iUtils->registerEvent(testEvent::testRSP::construct);

    }
}
