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

enum
{
    TI_DOCUMENT_READY
};

namespace sinTestServer
{
    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread, /// single threaded handler
        public Broadcaster
    {
    public:
        Service(const SERVICE_id&, const std::string&  nm, IInstance *ins);
        ~Service();

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }

    protected:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

    private:
        bool handleEvent(const REF_getter<Event::Base>& e);
        bool on_AddTaskREQ(const sinTestEvent::AddTaskREQ* e);
        bool on_GetResultREQ(const sinTestEvent::GetResultREQ* e);
        bool on_TickAlarm(const timerEvent::TickAlarm* e);

        std::map<std::string, REF_getter<sinTestEvent::AddTaskREQ> > task_in_process_;
        std::map<std::string, std::string > ready_tasks_;
    };
}
#endif

