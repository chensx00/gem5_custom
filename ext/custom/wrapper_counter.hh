#ifndef __WRAPPER_COUNTER_HH__
#define __WRAPPER_COUNTER_HH__

#include <cstdlib>
#include <iostream>
#include "VTop.h"
//#include "/home/csx/sw/verilator/v4.100/include/verilated.h"
#include "verilated.h"
//#include "/home/csx/sw/verilator/v4.100/include/verilated_fst_c.h"
//#include "verilated_fst_c.h"
//#include "/home/csx/sw/verilator/v4.100/include/verilated_vcd_c.h"
#include "verilated_vcd_c.h"

#include "custom_packet_counter.hh"


class Wrapper_counter
{
    public:
        Wrapper_counter(bool traceOn, std::string name);

        ~Wrapper_counter();

        //advance RTL's clock
        outputCounter tick();
        //advance RTL module's clk with inputdata
        outputCounter tick(inputCounter in);
        //reset RTL module
        void reset();

        void processInput(inputCounter in);
        outputCounter processOutput();

        uint64_t getTickCount();
        void enableTracing();
        void disableTracing();
        void advanceTickCount();

    private:
        VTop *top;
        uint64_t tickcount;
        VerilatedVcdC *fst;
        std::string fstname;
        bool traceOn;

};


#endif