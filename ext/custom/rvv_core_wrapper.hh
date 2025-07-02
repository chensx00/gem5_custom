#ifndef __RVV_CORE_WRAPPER_HH__
#define __RVV_CORE_WRAPPER_HH__

#include <cstdint>

class Vrvv_core;

namespace rvvcore {

struct RVVInstReq {
    uint32_t inst;
    uint32_t inst_id;
    uint32_t vle;
    uint32_t vstart;
    bool vma;
    bool vta;
    uint8_t vsew;
    uint8_t vlmul;
};

struct RVVInstRecv {
    uint32_t inst_id;
    uint8_t illegal;
    uint64_t result;
};

class RVVCoreWrapper {
  public:
    RVVCoreWrapper(uint32_t rst_cycle = 10);
    ~RVVCoreWrapper();
    uint32_t getIdWidth();
    void tick();
    bool isReady();
    bool setNewRVVInstReq(const RVVInstReq &recv);
    bool getRVVInstRecv(RVVInstRecv &recv);
    void reset();
    void invalidInstReq();

    // Transfer ew, lmul setting to rvv core encoding
    static uint8_t EW2Value(uint32_t ew);
    static uint8_t LMUL2Value(uint32_t numerator, uint32_t denominator);
    static uint32_t getInstIdWidth();

  private:
    uint32_t rst_cycle;
    void *tracer;
    Vrvv_core *rvv_core;
};

} // namespace rvvcore
#endif