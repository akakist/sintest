#ifndef _________dfsReferrerEvent_h123Z1
#define _________dfsReferrerEvent_h123Z1
#include "serviceEnum.h"

#include "SEQ_id.h"
#include "DATA_id.h"
namespace ServiceEnum
{
    const SERVICE_id sinTestServer("sinTestServer");
    const SERVICE_id sinTestClient("sinTestClient");
}

namespace sinTestEventEnum
{

    const EVENT_id AddTaskREQ("sin.AddTaskREQ");
    const EVENT_id AddTaskRSP("sin.AddTaskRSP");
    const EVENT_id GetResultREQ("sin.GetResultREQ");
    const EVENT_id GetResultRSP("sin.GetResultRSP");


}


#endif
