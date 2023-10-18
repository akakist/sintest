#ifndef ________UNKNWN___H
#define ________UNKNWN___H
#include "CLASS_id.h"
#include <map>
#include "mutexable.h"
#include "CONTAINER.h"
#include "commonError.h"
#include "SERVICE_id.h"
#include "IConfigObj.h"

/// base class for interfaces with casting ability

class UnknownBase
{
public:
    const std::string classname;
    virtual ~UnknownBase();
    virtual void deinit()=0;
    UnknownBase(const std::string& nm);

};

#endif
