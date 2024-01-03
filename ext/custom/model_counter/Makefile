DIR_VCD =verilator_custom_vcd
CC=gcc
CXX=g++
OBJS_VERILATE='*.sv' # Modify with the rtl object files

# export VERILATOR_ROOT=/home/csx/sw/verilator/v4.100
# generate sv to cpp
verilate_vcd: $(OBJS)
	$(VERILATOR_ROOT)/bin/verilator -Wall -cc --trace Top.sv --Mdir $(DIR_VCD)


.PHONY: clean cleanall

clean:
	rm -rf $(DIR_VCD) libVerilatorCustom.so