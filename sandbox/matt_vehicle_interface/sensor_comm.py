import serial
import time
import struct
import timer
import paths

def toHex(s):
    lst = []
    for ch in s:
        hv = hex(ord(ch)).replace('0x', '')
        if len(hv) == 1:
            hv = '0'+hv
        lst.append(hv)
    return reduce(lambda x,y:x+y, lst)
#to get depth value take first number multiply 256 add second number
# check to see if sum of first three bytes = the last byte
#if first byte is a failure then sensor has failed


class communicator:
    
    def __init__(self):
        self.commands = {"lcdtop":[0x0c,0x00],"lcdbottom":[0x0c,0x01],"ping":[0x00,0x00],"check":[0x01,0x01],"depth":[0x02,0x02],"synchronize":[0xff],"status":[0x04,0x04],"hardkill":[0x06,0xde,0xad,0xbe,0xef,0x3e],"marker0":[0x07,0x00,0x07],"marker1":[0x07,0x01,0x08],"display_off":[0x08,0x00,0x08],"display_on":[0x08,0x01,0x09],"display_blink":[0x08,0x02,0x0a],"thruster0off":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x00,0x90],"thruster1off":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x01,0x91],"thruster2off":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x02,0x92],"thruster3off":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x03,0x93],"thruster0on":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x04,0x94],"thruster1on":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x05,0x95],"thruster2on":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x06,0x96],"thruster3on":[0x09,0xb1,0xd0,0x23,0x7a,0x69,0x07,0x97]}
        self.response_commands = {"success":[0xbc],"checksum_fail":[0xcc],"failure":[0xdf],"depth":[0x03],"synchronize":[0xbc],"status":[0x05]}
        self.response_lengths = {"depth":4,"check":1,"ping":1,"depth":4,"synchronize":1,"status":3,"hardkill":1,"marker":1,"display":1,"thruster":1}
        print "Initializing a sensor board communicator"
        try:
            self.ser = serial.Serial(paths.sensor_address,115200,timeout=1)
            self.init = True
        except:
            print "Error initializing the serial port connection"
            self.init = False
    def write(self,command):
        try:
		self.ser.write(command)
	except:
		pass
    def time(self):
        return self.timer.time()
    def read_byte(self):
        try:
            byte = self.ser.read(1)
            return byte
        except:
            return [-1]
    def write_to_lcd(self,position,text):
        if position == "top":
            message = self.commands["lcdtop"]
        else:
            message = self.commands["lcdbottom"]
        var = ""
        for element in message:
            var +=  struct.pack('B', element)
        var += '%-16s' % text
        checksum = self.get_checksum(var)
        var += struct.pack('B', checksum)
        self.write(var)
        response = self.read_byte_message(self.response_lengths["thruster"])
        return response
    
    def pack_byte(self,byte):
        return struct.pack('B',byte)
    def read_unpacked_byte(self):
        message = self.read_byte()
        while (message == ""):
            message = self.read_byte()
        return struct.unpack('B',message)
    def send_byte_message(self,message):
        try:
            for element in message:
                self.write(self.pack_byte(element))
        except:
            return [-1]
    def read_byte_message(self,length):
        try:
            code_string = ""
            for i in range(length):
                code_string += "B"
            message = self.ser.read(length)
            unpacked = struct.unpack(code_string,message)
            return unpacked
        except:
            return [-1]
    def ping(self):
        self.send_byte_message(self.commands["ping"])
        response = self.read_byte_message(self.response_lengths["ping"])
        return response == self.response_commands["success"]
    def synchronize(self):
        self.send_byte_message(self.commands["synchronize"])
        response = self.read_byte_message(self.response_lengths["synchronize"])
        return response == self.response_commands["synchronize"]
            
    def check_message(self,message):
        length = len(message)
        sum = 0
        for i in range(length-1):
            sum = sum + message[i]
        check_sum = (message[length-1]%256) 
        return check_sum == sum%256          #True means the checksum is alright
    
    def get_checksum(self,message):
        sum = 0
        for ch in message:
            sum = sum + ord(ch)
        return sum%256
    
    def depth(self):
        self.send_byte_message(self.commands["depth"])
        response = self.read_byte_message(self.response_lengths["depth"])
        depth = response[1]*256 + response[2]
        if self.check_message(response):
            return depth
        else:
            return "Error!"
    def status(self):
        self.send_byte_message(self.commands["status"])
        response = self.read_byte_message(self.response_lengths["status"])
        if not self.check_message(response):
            return response
    def kill(self):
        self.send_byte_message(self.commands["hardkill"])
        response = self.read_byte_message(self.response_lengths["hardkill"])
        return response
    def drop_marker(self,num):
        if num == 0:
            self.send_byte_message(self.commands["marker0"])
        else:
            self.send_byte_message(self.commands["marker1"])
        response = self.read_byte_message(self.response_lengths["marker"])
        return response
    def display_off(self):
        self.send_byte_message(self.commands["display_off"])
        response = self.read_byte_message(self.response_lengths["display"])
        return response
    def display_on(self):
        self.send_byte_message(self.commands["display_on"])
        response = self.read_byte_message(self.response_lengths["display"])
        return response
    def display_blink(self):
        self.send_byte_message(self.commands["display_blink"])
        response = self.read_byte_message(self.response_lengths["display"])
        return response
    def thruster_off(self,num):
        if num == 0:
            self.send_byte_message(self.commands["thruster0off"])
            response = self.read_byte_message(self.response_lengths["thruster"])
        if num == 1:
            self.send_byte_message(self.commands["thruster1off"])
            response = self.read_byte_message(self.response_lengths["thruster"])
        if num == 2:
            self.send_byte_message(self.commands["thruster2off"])
            response = self.read_byte_message(self.response_lengths["thruster"])
    def thruster_on(self,num):
        if num == 0:
            self.send_byte_message(self.commands["thruster0on"])
            response = self.read_byte_message(self.response_lengths["thruster"])
        if num == 1:
            self.send_byte_message(self.commands["thruster1on"])
            response = self.read_byte_message(self.response_lengths["thruster"])
        if num == 2:
            self.send_byte_message(self.commands["thruster2on"])
            response = self.read_byte_message(self.response_lengths["thruster"])
            