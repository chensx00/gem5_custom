#ifndef __CUSTOM_HH__
#define __CUSTOM_HH__


#include <string>
#include <vector>

#include "base/named.hh"
#include "cpu/minor/buffers.hh"
#include "cpu/minor/cpu.hh"
#include "cpu/minor/pipe_data.hh"
#include "cpu/minor/trace.hh"
#include "mem/packet.hh"

namespace gem5
{
 
namespace minor
{

/* Forward declaration */
class Execute;

class Custom : public Named
{
    protected:
        MinorCPU &cpu;
        Execute &execute;

        class CustomInstPort : public MinorCPU::MinorCPUPort
        {
            protected:
                /* My owner */
                Custom &custom;
            public:
                CustomInstPort(std::string name, Custom &custom_, MinorCPU &cpu) :
                    MinorCPU::MinorCPUPort(name, cpu), custom(custom_)
                { }
            
            protected:
                bool recvTimingResp(PacketPtr ptk) override { panic("dont imp recvTimingResp"); }

                void recvReqRetry() override { custom.recvReqRetry(); }

                bool isSnooping() const override { return true; }

                void recvTimingSnoopReq(PacketPtr pkt) override
                    { panic("dont imp recvTimingSnoopReq"); }

                void recvFunctionalSnoop(PacketPtr pkt) override { }


        };

        CustomInstPort custmoInstPort;

        /* packet going to send to MemObject */
        PacketPtr packet;

        void recvReqRetry();

        /* CustomInstPort Send packetPtr to RTL */
        bool tryToSend(PacketPtr pkt);

    public:

        Custom(std::string name_, std::string custominst_port_name_,
        MinorCPU &cpu_, Execute &execute_);

        virtual ~Custom();

        /* Send custom inst to rtl model by Port */
        void SendToRTL(MinorDynInstPtr inst);

        Port & getPort(const std::string &if_name, PortID idx=InvalidPortID);
        



        MinorCPU::MinorCPUPort &getCustPort() { return custmoInstPort; }

};



} //End namespace minor

} //End namespace gem5




#endif 