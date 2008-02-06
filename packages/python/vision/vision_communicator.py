from ctypes import *
from threading import *
import time

'''
This is the vision communicator module. It handles all communication with the C vision library.
'''

class VISIONSTRUCT(Structure):
    _fields_ = [("frameNum",c_int),
                ("width",c_int),
                ("height",c_int),
                ("redLightx",c_int)
                ,("redLighty",c_int),
                ("distFromVertical",c_int),
                ("angle",c_double),
                ("binx",c_int),
                ("biny",c_int),
                ("lightVisible",c_int),
                ("pipeVisible",c_int),
                ("binVisible",c_int),
                ("frameNumCheck",c_int)]

class vision_comm:
    
    class vision_thread(Thread):
        def init(self,communicator):
            self.comm = communicator
            self.exit = False
        def run(self):
            try:
                self.comm.lib.visionStart()
                print "Vision thread running along"
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
            self.error = False
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
    def stop_vision_thread(self):
        pass
    def prepare(self):
        try:
            self.getData = self.lib.getData
            self.giveMeFive = self.lib.giveMeFive
            self.getDummy = self.lib.getDummy
            self.getData.restype = POINTER(VISIONSTRUCT)
            self.getDummy.restype = POINTER(VISIONSTRUCT)
        except:
            print "problem loading the vision library"
            return False
    def get_data(self):
        try:
            self.latest_data = self.getData().contents
            return self.latest_data
        except:
            print "Problem accessing the vision system"
            return False
    def get_dummy(self):
        return self.getDummy().contents
    def get_five(self,file):
        data = self.giveMeFive()
        print data
        file.write(str(data))