#include "tools_mt.h"
#include "sinTestServerService.h"
#include "../Event/TestIO.h"
#include "events_sinTestServerService.hpp"

/*
Алгоритм:

1. Ждем получения AddTaskREQ, на приход его создаем имитацию работы - выставляем таймер TI_DOCUMENT_READY, по срабатыванию таймера считаем, что работа сделана.
    Отвечаем in_process.
    Eсли таск уже создан, тоже отвечаем in_process.
    Eсли таск уже сделан, то отвечаем done

2. На таймер TI_DOCUMENT_READY отвечаем клиенту done.

3. На запрос GetResultREQ отвечаем статус ок вместе с документом, если документ существует, или not_found, если документ не найден.
*/

/// logic
bool sinTestServer::Service::on_TickAlarm(const timerEvent::TickAlarm* e)
{
    if(e->tid==TI_DOCUMENT_READY)
    {
        auto taskId=e->data->asString();
        auto it=task_in_process_.find(taskId);
        if(it==task_in_process_.end())
            throw CommonError("if(it==task_in_process.end())");
        auto task=it->second;

        auto document="processed request on taskId "+taskId+ " taskDescription "+task->taskDescription;
        ready_tasks_[task->taskId]=document;

        /// reply to client
        passEvent(new sinTestEvent::AddTaskRSP(taskId,"done",poppedFrontRoute(task->route)));
        logErr2("server: pass AddTaskRSP done");
        return true;
    }

    /// false means unhandled case
    return false;
}
bool sinTestServer::Service::on_GetResultREQ(const sinTestEvent::GetResultREQ* e)
{
    logErr2("server: on_GetResultREQ %s",e->taskId.c_str());

    auto it=ready_tasks_.find(e->taskId);
    if(it!=ready_tasks_.end())
    {
        passEvent(new sinTestEvent::GetResultRSP(e->taskId,it->second,"ok",poppedFrontRoute(e->route)));
        return true;
    }
    else
    {
        passEvent(new sinTestEvent::GetResultRSP(e->taskId,"","not_found",poppedFrontRoute(e->route)));
    }
    return  true;
}

bool sinTestServer::Service::on_AddTaskREQ(const sinTestEvent::AddTaskREQ* e)
{
    logErr2("server: AddTaskREQ %s",e->taskId.c_str());
    /// check in process, if in process reply "in_process"
    if(task_in_process_.count(e->taskId))
    {
        logErr2("server: in_process on rq %s",e->taskId.c_str());
        passEvent(new sinTestEvent::AddTaskRSP(e->taskId,"in_process",poppedFrontRoute(e->route)));
        return true;
    }
    /// check if already processed
    if(ready_tasks_.count(e->taskId))
    {
        passEvent(new sinTestEvent::AddTaskRSP(e->taskId,"done",poppedFrontRoute(e->route)));
        return true;
    }


    task_in_process_.insert({e->taskId,e});

    /// we'll receive ready signal from timer when document will be ready
    sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_DOCUMENT_READY,toRef(e->taskId),NULL,rand()%100,ListenerBase::serviceId));

    passEvent(new sinTestEvent::AddTaskRSP(e->taskId,"in_process",poppedFrontRoute(e->route)));

    return true;
}



/// end logic

/// service code


bool sinTestServer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        auto& ID=e->id;

        if(timerEventEnum::TickAlarm==ID)
            return on_TickAlarm((const timerEvent::TickAlarm*)e.operator->());

        if(sinTestEventEnum::AddTaskREQ==ID)
            return on_AddTaskREQ((const sinTestEvent::AddTaskREQ*)e.operator->());
        if(sinTestEventEnum::GetResultREQ==ID)
            return on_GetResultREQ((const sinTestEvent::GetResultREQ*)e.operator->());


        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            auto E=(rpcEvent::IncomingOnAcceptor*)e.operator->();
            auto &IDA=E->e->id;
            if(sinTestEventEnum::AddTaskREQ==IDA)
                return on_AddTaskREQ((const sinTestEvent::AddTaskREQ*)E->e.operator->());
            if(sinTestEventEnum::GetResultREQ==IDA)
                return on_GetResultREQ((const sinTestEvent::GetResultREQ*)E->e.operator->());
        }

    }
    catch(std::exception &e)
    {
        logErr2("server std::exception  %s",e.what());
    }
    XPASS;
    return false;
}

sinTestServer::Service::~Service()
{
}

sinTestServer::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ins->getConfig(),id,ins),
    Broadcaster(ins)
{
}

/// register plugin service in main app
void registersinTestServerService(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::sinTestServer,"sinTestServer",getEvents_sinTestServerService());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::sinTestServer,sinTestServer::Service::construct,"sinTestServer");
        regEvents_sinTestServerService();
    }
}


