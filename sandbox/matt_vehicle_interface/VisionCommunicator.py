from ctypes import *
from threading import *

class DANSINT(Structure):
    _fields_ = [("a",c_int),("b",c_int),("c",c_int),("d",c_int),("e",c_int),("f",c_int),("g",c_int)]

class vision_comm:
    def __init__(self,path):
        self.path = path
    def open_library(self):
        self.lib = CDLL(self.path,RTLD_GLOBAL)

class vision_thread(Thread):
    def init(self,communicator):
        self.comm = communicator
    def run(self):
        comm.lib.main()

class accessor_thread(Thread):
    def init(self,communicator):
        self.comm = communicator
        self.prepare()
    def run(self):
        x = self.get()
        print x.a
        print x.b
        print x.c
        print x.d
        print x.e
        print x.f
        print x.g
    def prepare(self):
        self.get_safe = comm.lib.getDummy
        self.get_safe.restype = POINTER(DANSINT)
    def get(self):
        structure = self.get_safe().contents
        return structure
        
    

comm = vision_comm("/Users/matbak87/robots/ram_code/build/sandbox/matt_vehicle_interface/libmatt.dylib")
comm.open_library()
vt = vision_thread()
vt.init(comm)
at = accessor_thread()
at.init(comm)
vt.start()
at.start()