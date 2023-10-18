#ifndef __________telnet_EventInfo__HH
#define __________telnet_EventInfo__HH


inline std::set<EVENT_id> getEvents_telnet()
{

	std::set<EVENT_id> out;
	out.insert(socketEventEnum::Accepted);
	out.insert(socketEventEnum::AddToListenTCP);
	out.insert(socketEventEnum::Disaccepted);
	out.insert(socketEventEnum::Disconnected);
	out.insert(socketEventEnum::NotifyBindAddress);
	out.insert(socketEventEnum::StreamRead);
	out.insert(systemEventEnum::startService);
	out.insert(telnetEventEnum::CommandEntered);
	out.insert(telnetEventEnum::RegisterCommand);
	out.insert(telnetEventEnum::RegisterType);
	out.insert(telnetEventEnum::Reply);

	return out;
}

inline void regEvents_telnet()
{
	iUtils->registerEvent(socketEvent::Accepted::construct);
	iUtils->registerEvent(socketEvent::AddToListenTCP::construct);
	iUtils->registerEvent(socketEvent::Disaccepted::construct);
	iUtils->registerEvent(socketEvent::Disconnected::construct);
	iUtils->registerEvent(socketEvent::NotifyBindAddress::construct);
	iUtils->registerEvent(socketEvent::StreamRead::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(telnetEvent::CommandEntered::construct);
	iUtils->registerEvent(telnetEvent::RegisterCommand::construct);
	iUtils->registerEvent(telnetEvent::RegisterType::construct);
	iUtils->registerEvent(telnetEvent::Reply::construct);
}
#endif
