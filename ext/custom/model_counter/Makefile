DIR_VCD =verilator_counter_vcd
CC=gcc
CXX=g++
OBJS_VERILATE='*.sv' # Modify with the rtl object files

# export VERILATOR_ROOT=/home/csx/sw/verilator/v4.100
# generate sv to cpp
verilate_vcd: $(OBJS)
	export VERILATOR_ROOT=/home/csx/sw/verilator/v4.100
	$(VERILATOR_ROOT)/bin/verilator -Wall -cc --trace Top.sv --Mdir $(DIR_VCD)


library_vcd: verilate_vcd wrapper_counter.cc wrapper_counter.hh 
	g++ -I $(DIR_VCD) -I$(VERILATOR_ROOT)/include wrapper_counter.cc $(DIR_VCD)/*.cpp \
	$(VERILATOR_ROOT)/include/verilated.cpp $(VERILATOR_ROOT)/include/verilated_vcd_c.cpp \
	-fpic  -shared -o libVerilatorCounter.so

install: libVerilatorCounter.so
	cp libVerilatorCounter.so ../
	cp wrapper_counter.hh ../
	cp custom_packet_counter.hh ../


.PHONY: clean cleanall

clean:
	rm -rf $(DIR_VCD) libVerilatorCounter.so

cleanall:
	rm -rf $(DIR_VCD) libVerilatorCounter.so
	rm -rf ../custom_packet_counter.hh
	rm -rf ../libVerilatorCounter.so
	rm -rf ../wrapper_counter.hh