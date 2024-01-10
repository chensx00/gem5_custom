// Use `cpp` suffix due to no rule for `cc` suffix in `verilated.mk`

#include <cassert>
#include <iostream>
#include <type_traits>
#ifdef VM_TRACE_FST
#include "verilated_fst_c.h"
#define VM_TRACE_CLASS_NAME VerilatedFstC
#else
#include "verilated_vcd_c.h"
#define VM_TRACE_CLASS_NAME VerilatedVcdC
#endif

#include "Vrvv_core.h"
#include "Vrvv_core_core_pkg.h"
#include "Vrvv_core_rvv_pkg.h"
#include "rvv_core_wrapper.hh"

namespace rvvcore {

// It seems that verilator currently don't support exporting struct to C++
// model. So we have to do this awkward copy.
// See https://github.com/verilator/verilator/issues/860
struct VecContext {
    uint8_t vlmul : Vrvv_core_rvv_pkg::VLMULWidth;
    uint8_t vsew : Vrvv_core_rvv_pkg::VSEWWidth;
    uint8_t vta : 1;
    uint8_t vma : 1;
    uint8_t vill : 1;
    uint32_t vstart : Vrvv_core_core_pkg::VLWidth;
    uint32_t vle : Vrvv_core_core_pkg::VLWidth;
    uint8_t reserved : 2;
} __attribute__((packed));

union VecContextUnion {
    VecContext vec_context;
    uint32_t vec_context_raw;
};

RVVCoreWrapper::RVVCoreWrapper(uint32_t rst_cycle) : rst_cycle(rst_cycle) {
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs argument parsing
    contextp->debug(0);

    // Randomization reset policy
    // May be overridden by commandArgs argument parsing
    contextp->randReset(2);

    rvv_core = new Vrvv_core(contextp.get());

#if VM_TRACE == 1
    /*contextp->traceEverOn(true);
    tracer = new VM_TRACE_CLASS_NAME;
    rvv_core->trace(tracer, 5);
    tracer->open("rvv_core.fst");*/
#else
    tracer = nullptr;
#endif
    tracer = nullptr; // temporarily disable tracing
}

RVVCoreWrapper::~RVVCoreWrapper() {
    if (tracer) {
        // tracer->close();
        delete tracer;
    }
    rvv_core->final();
    delete rvv_core;
}

void RVVCoreWrapper::tick() {
    for (int i = 0; i < 2; ++i) {
        rvv_core->contextp()->timeInc(1);
        rvv_core->clk_i = !rvv_core->clk_i;
        rvv_core->eval();
    }
}

bool RVVCoreWrapper::isReady() {
    assert(rvv_core->clk_i && "clk_i should be high between `tick` ?");
    return rvv_core->ready_o;
}

void RVVCoreWrapper::invalidInstReq() { rvv_core->valid_i = false; }

// TODO: proper deassert valid signal
bool RVVCoreWrapper::setNewRVVInstReq(const rvvcore::RVVInstReq &recv) {
    assert(rvv_core->clk_i && "clk_i should be high between `tick` ?");
    assert(rvv_core->ready_o &&
           "ready_o should be high before set new RVVInstReq");
    rvv_core->valid_i = true;
    rvv_core->insn_i = recv.inst;
    rvv_core->insn_id_i = recv.inst_id;

    VecContextUnion u;
    u.vec_context.vill = 0;
    u.vec_context.reserved = 0;
    u.vec_context.vlmul = recv.vlmul;
    u.vec_context.vsew = recv.vsew;
    u.vec_context.vma = recv.vma;
    u.vec_context.vta = recv.vta;
    u.vec_context.vstart = recv.vstart;
    u.vec_context.vle = recv.vle;
    rvv_core->vec_context_i = u.vec_context_raw;
    return true;
}

bool RVVCoreWrapper::getRVVInstRecv(RVVInstRecv &recv) {
    if (!rvv_core->done_o) {
        return false;
    }
    recv.inst_id = rvv_core->done_insn_id_o;
    recv.illegal = rvv_core->illegal_insn_o;
    recv.result = rvv_core->result_o;
    return true;
}

void RVVCoreWrapper::reset() {
    // Assign initial clk_i with a high value, we
    // can guarantee that clk negedge will be first
    // evaluated in each tick.
    rvv_core->valid_i = 0;
    rvv_core->flush_i = 0;
    rvv_core->store_op_gnt_i = 0;
    rvv_core->clk_i = 1;
    rvv_core->rst_ni = 1;
    for (int i = 0; i < 2 * rst_cycle; ++i) {
        rvv_core->contextp()->timeInc(1);
        rvv_core->clk_i = !rvv_core->clk_i;
        rvv_core->eval();
    }
    rvv_core->rst_ni = 0;
}

uint32_t RVVCoreWrapper::getInstIdWidth() {
    return Vrvv_core_core_pkg::InsnIDWidth;
}

uint8_t RVVCoreWrapper::EW2Value(uint32_t ew) {
    switch (ew) {
    case 8:
        return Vrvv_core_rvv_pkg::EW8;
    case 16:
        return Vrvv_core_rvv_pkg::EW16;
    case 32:
        return Vrvv_core_rvv_pkg::EW32;
    case 64:
        return Vrvv_core_rvv_pkg::EW64;
    default:
        assert(false && "Unsupport EW setting");
    }
}

uint8_t RVVCoreWrapper::LMUL2Value(uint32_t numerator, uint32_t denominator) {
    if (denominator == 1) {
        switch (numerator) {
        case 1:
            return Vrvv_core_rvv_pkg::LMUL_1;
        case 2:
            return Vrvv_core_rvv_pkg::LMUL_2;
        case 4:
            return Vrvv_core_rvv_pkg::LMUL_4;
        case 8:
            return Vrvv_core_rvv_pkg::LMUL_8;
        default:
            assert(false && "Unsupport LMUL setting");
        }
    }
    assert(numerator == 1 && "Unsupport LMUL setting");
    switch (denominator) {
    case 2:
        return Vrvv_core_rvv_pkg::LMUL_1_2;
    case 4:
        return Vrvv_core_rvv_pkg::LMUL_1_4;
    case 8:
        return Vrvv_core_rvv_pkg::LMUL_1_8;
    default:
        assert(false && "Unsupport LMUL setting");
    }
}
} // namespace rvvcore