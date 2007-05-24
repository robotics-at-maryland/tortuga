import math

class ControlInstruction:
    def __init__(self):
        print "Defining a new control instruction"
    def __str__(self):
        print "Blank Instruction"
    def get_instruction_description(self):
        return self.__str__

'''
define a turn some number of radians from current forward heading
current heading assumed at 0 radians, pi/2 is a 90 degree turn left, -pi/2 is 90 left
'''  
class AngleInstruction(ControlInstruction):
    def __init__(self,angle):
        if self.is_valid_angle(angle):
            self.angle = angle
        else:
            print "Invalid angle instruction"
            self.angle = None
        print "Created an angle instruction with angle " + angle
    def is_valid_angle(self,theta):
        if theta < math.pi and theta > (-1*math.pi):
            True 
        else:
            False
    def __str__(self):
         "Turn to angle: " + self.angle

'''
Define a drive instruction, with a percent of power from -100 to 100, with
-100 representing full speed backwards and 100 full speed forwards
'''
class ForwardInstruction(ControlIntruction):
    def __init__(self,percent):
        if percent < 100 and percent > -100:
            self.power = percent
        else:
            print "Invalid power instruction"
    def __str__(self):
        print "Go straight at power: %" + self.power
        
class StopInstructin(ControlInstruction):
    def __init__(self):
        self.instruction = "STOP"
    def __str__(self):
        print "STOP!"
        