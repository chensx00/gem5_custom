#include "VTop.h"
#include "verilated_vcd_c.h"
#include "wrapper_counter.hh"

Wrapper_counter::Wrapper_counter(bool traceOn, std::string name):
    tickcount(0),
    fst(NULL),
    fstname(name),
    traceOn(traceOn)
{
    top = new VTop;

    Verilated::traceEverOn(traceOn);
    fst = new VerilatedVcdC;
    if (!fst) {
        return;
    }
    top->trace(fst,5);

    std::cout << fstname << std::endl;
    fst->open(fstname.c_str());
}

Wrapper_counter::~Wrapper_counter() 
{
    if (fst) {
        fst->dump(tickcount);
        fst->close();
        delete fst;
    }
    top->final();
    delete top;
    exit(EXIT_SUCCESS);
}

outputCounter
Wrapper_counter::tick()
{
    top->clk = 1;
    top->eval();

    advanceTickCount();

    top->clk = 0;
    top->eval();

    advanceTickCount();

    return processOutput();

}

outputCounter
Wrapper_counter::tick(inputCounter in)
{
    processInput(in);

    top->clk = 1;
    top->eval();

    advanceTickCount();

    top->clk = 0;
    top->eval();

    advanceTickCount();

    return processOutput();
}

void 
Wrapper_counter::reset()
{
    top->rst = 1;
    top->clk = 1;
    top->eval();

    advanceTickCount();

    top->clk = 0;
    top->eval();

    advanceTickCount();
    top->rst = 0;
}

void 
Wrapper_counter::processInput(inputCounter in)
{
    top->ena = in.ena;
    top->rst = in.rst;
}

outputCounter
Wrapper_counter::processOutput()
{
    outputCounter out;
    out.out = top->out;
    out.cnt = top->cnt;
    return out;
}

uint64_t
Wrapper_counter::getTickCount()
{
    return tickcount;
}

void 
Wrapper_counter::enableTracing()
{
    traceOn = true;
}

void 
Wrapper_counter::disableTracing()
{
    traceOn = false;
    fst->dump(tickcount);
    fst->close();
}

void 
Wrapper_counter::advanceTickCount()
{
    if (fst and traceOn) {
        fst->dump(tickcount);
    }
    tickcount++;
}