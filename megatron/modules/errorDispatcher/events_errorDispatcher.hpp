#ifndef __________errorDispatcher_EventInfo__HH
#define __________errorDispatcher_EventInfo__HH


inline std::set<EVENT_id> getEvents_errorDispatcher()
{

	std::set<EVENT_id> out;
	out.insert(errorDispatcherEventEnum::NotifySubscriber);
	out.insert(errorDispatcherEventEnum::SendMessage);
	out.insert(errorDispatcherEventEnum::Subscribe);
	out.insert(errorDispatcherEventEnum::Unsubscribe);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(systemEventEnum::startService);

	return out;
}

inline void regEvents_errorDispatcher()
{
	iUtils->registerEvent(errorDispatcherEvent::NotifySubscriber::construct);
	iUtils->registerEvent(errorDispatcherEvent::SendMessage::construct);
	iUtils->registerEvent(errorDispatcherEvent::Subscribe::construct);
	iUtils->registerEvent(errorDispatcherEvent::Unsubscribe::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
}
#endif
