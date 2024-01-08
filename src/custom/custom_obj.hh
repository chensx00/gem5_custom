#ifndef __CUSTOME_MEM_OBJECT_HH_
#define __CUSTOME_MEM_OBJECT_HH_

#include "mem/port.hh"
#include "params/CustomObj.hh"
#include "sim/sim_object.hh"
#include "mem/packet.hh"
#include "sim/clocked_object.hh"

namespace gem5
{



class CustomObj : public SimObject
{
    private:



    EventFunctionWrapper event;

    void handleInst();

    void finishInst();

    class CPUSidePort : public ResponsePort
    {
        private:
            CustomObj *owner;

            bool needRetry;

            PacketPtr blockedPacket;

        public:
            CPUSidePort(const std::string& name, CustomObj *owner) :
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



    bool handleRequest(PacketPtr pkt);

    //bool handleResponse(PacketPtr pkt);
    void recvFunctional(PacketPtr pkt);


    bool needRetry;

    AddrRangeList getAddrRanges() const;

    CPUSidePort instPort;

    bool blocked;


    public:

        CustomObj(const CustomObjParams &params);

        Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override;

    protected:
        //interface to RTL model
        //this function will call tick() in wrapper_xxx
        //virtual void tick();

        //virtual void initRTLModel();

        //virtual void endRTLModel();


};

} // End namespace gem5

#endif
