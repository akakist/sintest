#ifndef __________http_EventInfo__HH
#define __________http_EventInfo__HH


inline std::set<EVENT_id> getEvents_http()
{

    std::set<EVENT_id> out;
    out.insert(httpEventEnum::DoListen);
    out.insert(httpEventEnum::GetBindPortsREQ);
    out.insert(httpEventEnum::GetBindPortsRSP);
    out.insert(httpEventEnum::RegisterProtocol);
    out.insert(httpEventEnum::RequestIncoming);
    out.insert(rpcEventEnum::IncomingOnAcceptor);
    out.insert(socketEventEnum::Accepted);
    out.insert(socketEventEnum::AddToListenTCP);
    out.insert(socketEventEnum::Connected);
    out.insert(socketEventEnum::Disaccepted);
    out.insert(socketEventEnum::Disconnected);
    out.insert(socketEventEnum::NotifyBindAddress);
    out.insert(socketEventEnum::NotifyOutBufferEmpty);
    out.insert(socketEventEnum::StreamRead);
    out.insert(systemEventEnum::startService);

    return out;
}

inline void regEvents_http()
{
    iUtils->registerEvent(httpEvent::DoListen::construct);
    iUtils->registerEvent(httpEvent::GetBindPortsREQ::construct);
    iUtils->registerEvent(httpEvent::GetBindPortsRSP::construct);
    iUtils->registerEvent(httpEvent::RegisterProtocol::construct);
    iUtils->registerEvent(httpEvent::RequestIncoming::construct);
    iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
    iUtils->registerEvent(socketEvent::Accepted::construct);
    iUtils->registerEvent(socketEvent::AddToListenTCP::construct);
    iUtils->registerEvent(socketEvent::Connected::construct);
    iUtils->registerEvent(socketEvent::Disaccepted::construct);
    iUtils->registerEvent(socketEvent::Disconnected::construct);
    iUtils->registerEvent(socketEvent::NotifyBindAddress::construct);
    iUtils->registerEvent(socketEvent::NotifyOutBufferEmpty::construct);
    iUtils->registerEvent(socketEvent::StreamRead::construct);
    iUtils->registerEvent(systemEvent::startService::construct);
}
#endif
