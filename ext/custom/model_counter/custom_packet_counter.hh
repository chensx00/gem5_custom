#ifndef __CUSTOM_PACKET_COUNTER_HH__
#define __CUSTOM_PACKET_COUNTER_HH__


#include <cstdlib>
#include <iostream>

struct inputCounter
{
    uint64_t    ena;
    uint64_t    rst;
};

struct outputCounter 
{
    uint64_t    out;
    uint64_t    cnt;
};




#endif