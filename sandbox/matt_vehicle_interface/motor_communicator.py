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
            pass
        
    def set_power(self,thruster):
        address = thruster.address
        power = thruster.power
        command = "C0" + str(address) + " " + str(power) + "\r\n"
        try:
            self.ser.write(command)
        except:
            pass
            
    def close(self,thrusters):
        for thruster in thrusters:
            print "Shutting off thruster " + str(thruster.address)
            thruster.power = 0
            self.set_power(thruster)
        try:
            self.ser.close()
        except:
            pass