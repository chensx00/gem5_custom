from m5.objects import TickedObject
from m5.params import *
from m5.proxy import *


class RVVCoreRTLObject(TickedObject):
    type = "RVVCoreRTLObject"
    cxx_header = "rtl/rvvcore_rtlobj.hh"
    cxx_class = "gem5::RVVCoreRTLObject"
