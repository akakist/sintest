#ifndef _______________displayzzPLAYBCNODE___H1INV
#define _______________displayzzPLAYBCNODE___H1INV
#include "broadcaster.h"



#include "logging.h"
#include "listenerBuffered1Thread.h"
#include "DBH.h"
#include "USER_id.h"
#include "IProgress.h"
#include "tools_mt.h"
#include <map>



#include "Real.h"
#include "Events/System/Run/startService.h"
#include "Events/System/timer/TickTimer.h"

#include "Events/System/Net/http/DoListen.h"
#include "Events/System/Net/http/RequestIncoming.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "../Event/Ping.h"
#include "version_mega.h"
#define SESSION_ID  "session_id"
enum TIMERS
{
    TIMER_PUSH_NOOP=1
};
namespace prodtestWebServer
{
    class Session:
            public Refcountable
    {
    public:
        std::string sessionId;
        REF_getter<epoll_socket_info> esi;
        REF_getter<HTTP::Request> req;


        Session(const std::string& sid, const REF_getter<HTTP::Request> &_req,const REF_getter<epoll_socket_info> &_esi): sessionId(sid),
            req(_req),
            esi(_esi)
        {
        }
        ~Session() {}

    };



    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster
    {
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);



    public:
        Service(const SERVICE_id&, const std::string&  nm, IInstance *ins);
        ~Service();


        bool on_RequestIncoming(const httpEvent::RequestIncoming*);
        bool on_AddTaskRSP(const prodtestEvent::AddTaskRSP*e);



    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }

        REF_getter<prodtestWebServer::Session> check_session(const REF_getter<HTTP::Request>& req, HTTP::Response& resp, const REF_getter<epoll_socket_info> &_esi);
        REF_getter<prodtestWebServer::Session> get_session( const std::string& session_id);

        std::map<std::string,REF_getter<Session> > sessions;


        msockaddr_in bindAddr;
        std::string prodtestServerAddr;

    };

}
#endif

