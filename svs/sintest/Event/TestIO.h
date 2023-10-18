#ifndef _________dfsReferrerEvent_h19Z1
#define _________dfsReferrerEvent_h19Z1
#include "___sinTestEvent.h"
#include <vector>


namespace sinTestEvent {




    class AddTaskREQ: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new AddTaskREQ(r);
        }
        AddTaskREQ(const std::string& _taskId,
                const std::string& _taskDescription,
                const route_t&r)
            :Base(sinTestEventEnum::AddTaskREQ,r),
              taskId(_taskId),
              taskDescription(_taskDescription)
        {}
        AddTaskREQ(const route_t&r)
            :Base(sinTestEventEnum::AddTaskREQ,r) {}
        std::string taskId;
        std::string taskDescription;
        void unpack(inBuffer& o)
        {
            o>>taskId>>taskDescription;
        }
        void pack(outBuffer&o) const
        {
            o<<taskId<<taskDescription;
        }
        void jdump(Json::Value &) const
        {
        }

    };

    class AddTaskRSP: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new AddTaskRSP(r);
        }
        AddTaskRSP(const std::string& _taskId,
                const std::string& _status,
                const route_t&r)
            :Base(sinTestEventEnum::AddTaskRSP,r),
              taskId(_taskId),
              status(_status)
        {}
        AddTaskRSP(const route_t&r)
            :Base(sinTestEventEnum::AddTaskRSP,r) {}
        std::string taskId;
        std::string status;
        void unpack(inBuffer& o)
        {
            o>>taskId>>status;
        }
        void pack(outBuffer&o) const
        {
            o<<taskId<<status;
        }
        void jdump(Json::Value &) const
        {
        }

    };


    class GetResultREQ: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new GetResultREQ(r);
        }
        GetResultREQ(const std::string& _taskId,
                const route_t&r)
            :Base(sinTestEventEnum::GetResultREQ,r),
              taskId(_taskId)
        {}
        GetResultREQ(const route_t&r)
            :Base(sinTestEventEnum::GetResultREQ,r) {}
        std::string taskId;
        void unpack(inBuffer& o)
        {
            o>>taskId;
        }
        void pack(outBuffer&o) const
        {
            o<<taskId;
        }
        void jdump(Json::Value &) const
        {
        }

    };

    class GetResultRSP: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new GetResultRSP(r);
        }
        GetResultRSP(const std::string& _taskId,
                     const std::string& _document,
                     const std::string& _status,
                const route_t&r)
            :Base(sinTestEventEnum::GetResultRSP,r),
              taskId(_taskId),
              document(_document),
            status(_status)
        {}
        GetResultRSP(const route_t&r)
            :Base(sinTestEventEnum::GetResultRSP,r) {}
        std::string taskId;
        std::string document;
        std::string status;
        void unpack(inBuffer& o)
        {
            o>>taskId>>document>>status;
        }
        void pack(outBuffer&o) const
        {
            o<<taskId<<document<<status;
        }
        void jdump(Json::Value &) const
        {
        }

    };





}
#endif
