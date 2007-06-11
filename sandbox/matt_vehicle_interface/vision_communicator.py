from ctypes import *
from threading import *

class VISIONSTRUCT(Structure):
    _fields_ = [("frameNum",c_int),("width",c_int),("height",c_int),("redLightx",c_int),("redLighty",c_int),("distFromVertical",c_int),("angle",c_double),("binx",c_int),("biny",c_int),("lightVisible",c_int),("pipeVisible",c_int),("binVisible",c_int),("frameNumCheck",c_int)]

class vision_comm:
    
    class vision_thread(Thread):
        def init(self,communicator):
            self.comm = communicator
        def run(self):
            try:
                comm.lib.processVision()
            except:
                print "problem running vision library main function"
      
    def __init__(self,path):
        self.path = path
        lib_error = self.open_library()
        prep_error = self.prepare()
        if lib_error == False or prep_error == False:
            print "Vision Thread Will Not Be Started!"
            self.error = True
        else:
            self.vt = self.vision_thread()
            self.vt.init(self)
    def open_library(self):
        try:
            self.lib = CDLL(self.path,RTLD_GLOBAL)
        except:
            print "problem opening the vision library"
            return False
    def start_vision_thread(self):
        if self.error == False:
            self.vt.start()
        else:
            print "Vision thread not started because of errors"
    def prepare(self):
        try:
            self.get_safe = self.lib.getData
            self.get_safe.restype = POINTER(VISIONSTRUCT)
        except:
            print "problem loading the vision library"
            return False
    def get_data(self):
        try:
            self.latest_data = self.get_safe().contents
            return self.latest_data
        except:
            print "Problem accessing the vision system"
            return False