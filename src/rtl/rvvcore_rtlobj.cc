#include "rtl/rvvcore_rtlobj.hh"

#include "arch/riscv/insts/static_inst.hh"
#include "arch/riscv/types.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "cpu/minor/cpu.hh"
#include "cpu/minor/dyn_inst.hh"
#include "rvv_core_wrapper/rvv_core_wrapper.hh"
#include "sim/cur_tick.hh"
#include "sim/ticked_object.hh"

namespace gem5 {

// We set a higher priority for the RVVCoreRTLObject to make sure it will
// be scheduled after the CPU tick event.
RVVCoreRTLObject::RVVCoreRTLObject(const RVVCoreRTLObjectParams &params)
    : TickedObject(params, Event::CPU_Tick_Pri + 1),
      cpu(*dynamic_cast<MinorCPU *>(params.cpu)),
      rvvCoreWrapper(new rvvcore::RVVCoreWrapper) {
    uint32_t id_width = rvvCoreWrapper->getInstIdWidth();
    instIds.resize(1 << id_width);
}

void RVVCoreRTLObject::evaluate() {
    rvvCoreWrapper->tick();
    last_tick = curTick();
    // After the tick, we ensure that rvvcore has received the rvv inst.
    // Deassert valid signal to avoid sending the same inst again.
    rvvCoreWrapper->invalidInstReq();

    rvvcore::RVVInstRecv recv;
    if (rvvCoreWrapper->getRVVInstRecv(recv)) {
        cpu.RVVInstDone(instIds[recv.inst_id], recv.illegal, recv.result);
        instIds[recv.inst_id] = minor::InstId();
    }
}

bool RVVCoreRTLObject::getNewInstId(uint32_t &rvv_id, minor::InstId &instId) {
    for (uint32_t i = 0, j = instIds.size(); i < j; i++) {
        if (instIds[i].fetchSeqNum == 0) {
            rvv_id = i;
            instIds[i] = instId;
            return true;
        }
    }
    return false;
}

bool RVVCoreRTLObject::sendNewRVVInst(minor::MinorDynInst *rvv_inst,
                                      uint32_t vstart) {
    panic_if(curTick() <= last_tick,
             "sendNewRVVInst should be called by cpu side, which will be "
             "scheduled before rvv core.");
    panic_if(!rvv_inst->staticInst->isVector(),
             "call sendNewRVVInst with a non-vector inst");
    uint32_t rvv_id;
    if (!rvvCoreWrapper->isReady()) {
        // TODO: log this
        return false;
    } else if (getNewInstId(rvv_id, rvv_inst->id)) {
        // TODO: log this
        return false;
    }
    RiscvISA::ExtMachInst inst =
        dynamic_cast<RiscvISA::RiscvStaticInst *>(rvv_inst->staticInst.get())
            ->machInst;
    rvvcore::RVVInstReq req;
    req.inst_id = rvv_id;
    req.inst = inst.instBits;
    req.vle = inst.vl;
    req.vlmul = inst.vtype8.vlmul;
    req.vsew = inst.vtype8.vsew;
    req.vma = inst.vtype8.vma;
    req.vta = inst.vtype8.vta;
    req.vstart = vstart;
    rvvCoreWrapper->setNewRVVInstReq(req);
    return true;
}

} // namespace gem5
