
from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject
from m5.objects.CustomObj import CustomObj

class CustomCounter(CustomObj):
    type = 'CustomCounter'
    cxx_header = "custom/custom_counter.hh"
    cxx_class = "gem5::CustomCounter"

    counter_port = ResponsePort("CPU side port, receives custom inst")