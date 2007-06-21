import serial

"C01 500 \r\n"
"W01 28 400 \r\n"
#1024 to -1024

file = open("matt.txt",'w')

class communicator:
    def __init__(self,serial_device):
        try:
            self.ser = serial.Serial(serial_device,19200)
            self.ser.setTimeout(1)
        except:
            print "Init failed"
    
    def set_amps_limit(self):
        try:     
            self.ser.write("W01 28 400 \r\n")
        except:
            pass
      
    def set_power(self,address,power):
        pow_command = int(power*1024)
        if pow_command > 1024:
            pow_command = 1024
        elif pow_command < -1024:
            pow_command = -1024
        command = "C0" + str(address) + " " + str(pow_command) + "\r\n"
        try:
            self.ser.write(command)
        except:
            pass
      
    def set_thruster_power(self,thruster):
        address = thruster.address
        power = thruster.power
    	pow_command = int(power*1024)
    	if pow_command > 1024:
    	    pow_command = 1024
     	elif pow_command < -1024:
    	    pow_command = -1024
            command = "C0" + str(address) + " " + str(pow_command) + "\r\n"
        try:
            self.ser.write(command)
        except:
            pass
            
    def close(self,thrusters):
        for thruster in thrusters:
            print "Shutting off thruster " + str(thruster.address)
            thruster.power = 0
            self.set_power(thruster.address,thruster.power)
        try:
            self.ser.close()
        except:
            pass
