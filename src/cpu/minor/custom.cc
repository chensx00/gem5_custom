
#include "cpu/minor/custom.hh"

#include <iomanip>
#include <sstream>

#include "base/compiler.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "cpu/minor/exec_context.hh"
#include "cpu/minor/execute.hh"
#include "cpu/minor/pipeline.hh"
#include "cpu/utils.hh"
#include "debug/Activity.hh"
#include "debug/MinorMem.hh"

#include "debug/CustomObj.hh"

namespace gem5
{

namespace minor
{

Custom::Custom(std::string name_, std::string custominst_port_name_,
        MinorCPU &cpu_, Execute &execute_):
        Named(name_),
        cpu(cpu_),
        execute(execute_),
        custmoInstPort(custominst_port_name_, *this, cpu_)
    {
        DPRINTF(CustomObj, "Init Custom\n");
    }

Custom::~Custom()
{ }

void
Custom::SendToRTL(MinorDynInstPtr inst)
{
    /* not impl complete function, just send a empty packet */
    RequestPtr requset = std::make_shared<Request>();
    packet = Packet::createRead(requset);

    Custom::tryToSend(packet);
}

bool
Custom::tryToSend(PacketPtr pkt)
{
    DPRINTF(CustomObj, "customInstPort sendTimingReq:%d\n", curTick());
    if(custmoInstPort.sendTimingReq(pkt)){
        return true;
    }
    return false;
}

void
Custom::recvReqRetry()
{
    DPRINTF(CustomObj, "customInstPort recvReqRetry:%d\n", curTick());
    tryToSend(packet);
}


Port &
Custom::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "custominst_port")
        return custmoInstPort;
    else
        return custmoInstPort;
}

} //End namespace minor

} //End namespace gem5
