BUILD_DIR?=obj_dir
SUB_MAKE_DIR=rvv_core/tb
SHARED_LIB_TARGET=rvv_core
SHARED_LIBNAME=rvvcore
TOP_MODULE=rvv_core
CC=gcc
CXX=g++

export BUILD_DIR

# `trace`, `DUMP_VRF_ACCESS` varibles should be passed implicitly by the docs
# https://www.gnu.org/software/make/manual/html_node/Variables_002fRecursion.html
build_lib: rvv_core_wrapper.cpp rvv_core_wrapper.hh rvv_core_interface.hh
	$(MAKE) -C $(SUB_MAKE_DIR) $(SHARED_LIB_TARGET)
	python3 ../extract_verilator.py $(SUB_MAKE_DIR)/$(BUILD_DIR)/V$(TOP_MODULE).mk $(SHARED_LIBNAME) ../../../rvv_core_wrapper.o

install: build_lib
	cp $(SUB_MAKE_DIR)/$(BUILD_DIR)/lib$(SHARED_LIBNAME).so ../
	cp rvv_core_wrapper.hh ../

