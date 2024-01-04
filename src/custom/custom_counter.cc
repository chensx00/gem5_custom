#include "custom/custom_counter.hh"
#include "base/logging.hh"
#include "base/trace.hh"

#include "debug/CustomObj.hh"

namespace gem5
{

CustomCounter::CustomCounter(const CustomCounterParams &params) :
    CustomObj(params),
    tickEvent([this]{tick();}, name() + ".tickEvent"),
    instPort(params.name + ".counter_port", this),
    blocked(false)
{
    initRTLModel();
}

CustomCounter::~CustomCounter()
{
    DPRINTF(CustomObj, "calling destructor counter\n");
    endRTLModel();
}

Port &CustomCounter::getPort(const std::string & if_name, PortID idx)
{
    panic_if(idx != InvalidPortID, "This object doesn't support vector ports\n");
    if(if_name == "counter_port"){
        return instPort;
    } else {
        return CustomObj::getPort(if_name, idx);
    }
}

AddrRangeList
CustomCounter::CPUSidePort::getAddrRanges() const
{
    return owner->getAddrRanges();
}

void 
CustomCounter::CPUSidePort::recvFunctional(PacketPtr pkt)
{
    return owner->recvFunctional(pkt);
}

bool
CustomCounter::CPUSidePort::recvTimingReq(PacketPtr pkt)
{
    if(!owner->handleRequest(pkt)){
        needRetry = true;
        return false;
    } else {
        return true;
    }
}

void 
CustomCounter::CPUSidePort::sendPacket(PacketPtr pkt)
{
    panic_if(blockedPacket != nullptr, "Should never try to send if blocked!\n");
    if(!sendTimingResp(pkt)){
        blockedPacket = pkt;
    }
}

void
CustomCounter::CPUSidePort::recvRespRetry()
{
    assert(blockedPacket != nullptr);

    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;

    sendPacket(pkt);
}

void
CustomCounter::CPUSidePort::trySendRetry()
{
    if (needRetry && blockedPacket == nullptr) {
        needRetry = false;
        DPRINTF(CustomObj, "Sending retry req  %d\n", curTick());
        sendRetryReq();
    }
}


AddrRangeList 
CustomCounter::getAddrRanges() const
{
    
}

void 
CustomCounter::recvFunctional(PacketPtr pkt)
{
    
}

//---------------------------//


bool
CustomCounter::handleRequest(PacketPtr pkt)
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
CustomCounter::handleInst()
{
    assert(blocked);
    DPRINTF(CustomObj, "handle float inst at %d \n", curTick());
    reset();
    
    input.ena = 1;
    input.rst = 0;
    tick();
}

void
CustomCounter::finishInst()
{
    assert(blocked);
    blocked = false;
    DPRINTF(CustomObj, "finish float inst at %d \n", curTick());
}

void
CustomCounter::tick()
{
    DPRINTF(CustomObj, "tick at %d \n", curTick());
    
    outputCounter out = wr->tick(input);

    DPRINTF(CustomObj, "out.cnt = %d \n", out.cnt);
    if(out.out == 1) {
        DPRINTF(CustomObj, "out = 1  at %d \n", curTick());
        finishInst();
    } else {
        schedule(tickEvent,curTick() + 1);
    }
}

// void
// CustomCounter::tick(inputCounter in)
// {
//     outputCounter out = wr->tick(in);
//     if(out == 1) {
//         finishInst();
//     } else {
//         schedule(tickEvent,nextCycle());
//     }
// }

void
CustomCounter::reset()
{
    wr->reset();
}

void 
CustomCounter::initRTLModel()
{
    //traceOn = true
    DPRINTF(CustomObj, "reset  at %d \n", curTick());
    wr = new Wrapper_counter(true,"trace.vcd");
    wr->reset();
    //DPRINTF(CustomObj, "out.cnt = %d \n", curTick());
}

void 
CustomCounter::endRTLModel()
{
    delete wr;
}




}