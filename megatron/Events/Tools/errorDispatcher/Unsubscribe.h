#ifndef _______________serviceErrorDispatcherEvent$h4
#define _______________serviceErrorDispatcherEvent$h4
#include "___errorDispatcherEvent.h"
namespace errorDispatcherEvent
{
    class Unsubscribe: public Event::Base
    {
        


    public:
        static Base* construct(const route_t &r)
        {
            return new Unsubscribe(r);
        }
        Unsubscribe(const route_t&r)
            :Base(errorDispatcherEventEnum::Unsubscribe,r) {}
        void unpack(inBuffer&)
        {
        }
        void pack(outBuffer&) const
        {
        }
        void jdump(Json::Value &) const
        {
        }
    };

}
#endif
