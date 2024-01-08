from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject


class CustomObj(SimObject):
    type = "CustomObj"
    cxx_header = "custom/custom_obj.hh"
    cxx_class = "gem5::CustomObj"

    custominst_port = ResponsePort("CPU side port, receives custom inst")
