#ifndef __________sinTestClient_EventInfo__HH
#define __________sinTestClient_EventInfo__HH


inline std::set<EVENT_id> getEvents_sinTestClient()
{

	std::set<EVENT_id> out;
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(sinTestEventEnum::AddTaskREQ);
	out.insert(sinTestEventEnum::AddTaskRSP);
	out.insert(sinTestEventEnum::GetResultREQ);
	out.insert(sinTestEventEnum::GetResultRSP);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::SetAlarm);
	out.insert(timerEventEnum::StopAlarm);
	out.insert(timerEventEnum::TickAlarm);

	return out;
}

inline void regEvents_sinTestClient()
{
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(sinTestEvent::AddTaskREQ::construct);
	iUtils->registerEvent(sinTestEvent::AddTaskRSP::construct);
	iUtils->registerEvent(sinTestEvent::GetResultREQ::construct);
	iUtils->registerEvent(sinTestEvent::GetResultRSP::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::SetAlarm::construct);
	iUtils->registerEvent(timerEvent::StopAlarm::construct);
	iUtils->registerEvent(timerEvent::TickAlarm::construct);
}
#endif
