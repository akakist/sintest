#ifndef _______________displayzzPLAYBCNODE___H1INV
#define _______________displayzzPLAYBCNODE___H1INV
#include "broadcaster.h"
#include "listenerBuffered1Thread.h"
#include "Events/System/Run/startService.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "../Event/TestIO.h"
#include "version_mega.h"

enum TIMERS
{
    TI_START,
    TI_RESEND_ADDTASK_REQ,
    TI_GETRESULT_EXPIRED
};

namespace sinTestClient
{

    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster
    {
        bool handleEvent(const REF_getter<Event::Base>& e);

        bool on_startService(const systemEvent::startService*);
        bool on_TickAlarm(const timerEvent::TickAlarm*);
        bool on_AddTaskRSP(const sinTestEvent::AddTaskRSP*e);
        bool on_GetResultRSP(const sinTestEvent::GetResultRSP*e);



    public:
        Service(const SERVICE_id&, const std::string&  nm, IInstance *ins);
        ~Service();




    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }
        std::string genId();

        msockaddr_in bindAddr_;
        std::string sinTestServerAddr_;
        std::map<std::string, REF_getter<sinTestEvent::AddTaskREQ> > requests_;

    };

}
#endif

