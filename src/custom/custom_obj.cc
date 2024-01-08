#include "custom/custom_obj.hh"
#include "base/logging.hh"
#include "base/trace.hh"

#include "debug/CustomObj.hh"

namespace gem5
{

CustomObj::CustomObj(const CustomObjParams &params) :
    SimObject(params),
    event([this]{finishInst();}, name() + ".event"),//event
    instPort(params.name + ".custominst_port", this),
    blocked(false)
{

}


Port &CustomObj::getPort(const std::string & if_name, PortID idx)
{
    panic_if(idx != InvalidPortID, "This object doesn't support vector ports\n");
    if(if_name == "custominst_port"){
        return instPort;
    } else {
        return SimObject::getPort(if_name, idx);
    }
}

AddrRangeList
CustomObj::CPUSidePort::getAddrRanges() const
{
    return owner->getAddrRanges();
}

void
CustomObj::CPUSidePort::recvFunctional(PacketPtr pkt)
{
    return owner->recvFunctional(pkt);
}

bool
CustomObj::CPUSidePort::recvTimingReq(PacketPtr pkt)
{
    if(!owner->handleRequest(pkt)){
        needRetry = true;
        return false;
    } else {
        return true;
    }
}

void
CustomObj::CPUSidePort::sendPacket(PacketPtr pkt)
{
    panic_if(blockedPacket != nullptr, "Should never try to send if blocked!\n");
    if(!sendTimingResp(pkt)){
        blockedPacket = pkt;
    }
}

void
CustomObj::CPUSidePort::recvRespRetry()
{
    assert(blockedPacket != nullptr);

    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;

    sendPacket(pkt);
}

void
CustomObj::CPUSidePort::trySendRetry()
{
    if (needRetry && blockedPacket == nullptr) {
        needRetry = false;
        DPRINTF(CustomObj, "Sending retry req  %d\n", curTick());
        sendRetryReq();
    }
}


AddrRangeList
CustomObj::getAddrRanges() const
{

}

void
CustomObj::recvFunctional(PacketPtr pkt)
{

}

bool
CustomObj::handleRequest(PacketPtr pkt)
{
    if(blocked){
        return false;
    }
    DPRINTF(CustomObj, "Got request for inst\n");
    blocked = true;

    /*
handle custome inst
    */
   handleInst();


    return true;
}

void
CustomObj::handleInst()
{
    assert(blocked);
    DPRINTF(CustomObj, "handle float inst at %d \n", curTick());
    schedule(event, curTick() + 100);//event

}

void CustomObj::finishInst()
{
    assert(blocked);
    blocked = false;
    DPRINTF(CustomObj, "finish float inst at %d \n", curTick());
}

} //End gem5 namespace
