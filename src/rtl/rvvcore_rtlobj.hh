#ifndef __CUSTOME_MEM_OBJECT_HH_
#define __CUSTOME_MEM_OBJECT_HH_

#include <memory>
#include <vector>

#include "cpu/minor/dyn_inst.hh"
#include "params/RVVCoreRTLObject.hh"
#include "rvv_core_wrapper.hh"
#include "sim/clocked_object.hh"
#include "sim/cur_tick.hh"
#include "sim/sim_object.hh"
#include "sim/ticked_object.hh"

namespace gem5 {
class MinorCPU;

// This RTLObject communicate with gem5 simulation model by a response port
class RVVCoreRTLObject : public TickedObject
{
  private:
    uint64_t last_tick = 0;
    MinorCPU *cpu;
    rvvcore::RVVCoreWrapper *rvvCoreWrapper;
    std::vector<minor::InstId> instIds;

    bool getNewInstId(uint32_t &id, minor::InstId &instId);

  public:
    RVVCoreRTLObject(const RVVCoreRTLObjectParams &params);
    bool sendNewRVVInst(minor::MinorDynInst *rvv_inst, uint32_t vstart);
    void evaluate() override;
    // Don't use `Parent.any` to resolve parent CPU for which causes
    // circular dependency when creating CCObject.
    void setCPU(MinorCPU *_cpu) { cpu = _cpu; }
    void startup() override;
    void endRTLModel() {
        delete rvvCoreWrapper;
        // Deleting nullptr in exiting callback should be safe
        rvvCoreWrapper = nullptr;
    }

  protected:
    // interface to RTL model
    // this function will call tick() in wrapper_xxx
    // virtual void tick();

    // virtual void initRTLModel();

    // virtual void endRTLModel();
};

} // End namespace gem5

#endif
