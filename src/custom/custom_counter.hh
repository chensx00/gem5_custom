#ifndef __CUSTOM_COUNTER_HH__
#define __CUSTOM_COUNTER_HH__

#include "mem/port.hh"
#include "params/CustomCounter.hh"
#include "sim/sim_object.hh"
#include "mem/packet.hh"
#include "sim/clocked_object.hh"
#include "custom/custom_obj.hh"
#include "wrapper_counter.hh"


namespace gem5
{

class CustomCounter : public CustomObj
{
    private:
        EventFunctionWrapper tickEvent;

        void handleInst() ;

        void finishInst() ;

    class CPUSidePort : public ResponsePort
    {
        private:
            CustomCounter *owner;

            bool needRetry;

            PacketPtr blockedPacket;
        public:
            CPUSidePort(const std::string& name, CustomCounter *owner) :
                ResponsePort(name) ,owner(owner), needRetry(false),
                blockedPacket(nullptr)
            { }
            void sendPacket(PacketPtr pkt);
            AddrRangeList getAddrRanges() const override;
            void trySendRetry();

        protected:
            Tick recvAtomic (PacketPtr pkt) override { panic("recvAtomic unimpl") ;}
            void recvFunctional(PacketPtr pkt) override ;
            bool recvTimingReq(PacketPtr ptk) override;
            void recvRespRetry() override;

    };
    bool handleRequest(PacketPtr pkt);//override;

    //bool handleResponse(PacketPtr pkt);
    void recvFunctional(PacketPtr pkt);//override;

    bool needRetry;
    AddrRangeList getAddrRanges() const;
    CPUSidePort instPort;
    bool blocked;
    inputCounter input;


    public:
        CustomCounter(const CustomCounterParams &params);
        ~CustomCounter();
        Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override;

        //wrapper pointer
        Wrapper_counter *wr;

    protected:

        void tick();

        //void tick(inputCounter in);

        void reset();

        void initRTLModel();

        void endRTLModel();


};



}


#endif
