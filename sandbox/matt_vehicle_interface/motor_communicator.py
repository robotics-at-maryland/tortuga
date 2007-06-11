import serial,paths,time

"C01 500 \r\n"
"W01 28 400 \r\n"
#1024 to -1024

class communicator:
    def __init__(self):
        try:
            self.ser = serial.Serial(paths.motor_communicator,19200)
            self.ser.setTimeout(1)
        except:
            pass
    
    def set_amps_limit(self):
        try:     
            self.ser.write("W01 28 400 \r\n")
        except:
            print "W01 28 400 \r\n"
        
    def set_power(self,thruster,power):
        command = "C0" + str(thruster) + " " + str(power) + "\r\n"
        print command
        try:
            self.ser.write(command)
        except:
            print command
    def close(self):
        self.set_power(1,0)
        self.set_power(2,0)
        self.set_power(3,0)
        self.set_power(4,0)
        self.ser.close()