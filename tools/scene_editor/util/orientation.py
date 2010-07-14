
# STD Includes
import math as pmath
import traceback
import readline
import re

# Project Includes
import ext.math as math

# Convert quaternions to axis angle for the simulator
def quat2axis(quat):
    angle = pmath.degrees(2 * pmath.acos(quat.w))
    # Simulator swaps x and z for some strange reason...
    x = quat.z / pmath.sqrt(1-quat.w**2)
    y = quat.y / pmath.sqrt(1-quat.w**2)
    z = quat.x / pmath.sqrt(1-quat.w**2)
    return [x, y, z, angle]

class CmdLine(object):
    def __init__(self):
        self._quatexp = re.compile(r'[^\d]*(?P<x>\d+(.\d+)?)[^\d]+(?P<y>\d+(.\d+)?)[^\d]+(?P<z>\d+(.\d+)?)[^\d]+(?P<w>\d+(.\d+)?)[^\d]*')

    def cmdloop(self):
        try:
            stop = False
            while not stop:
                text = raw_input('>>> ')
                stop = self.interpret(text)
        except EOFError:
            # Ignore this one specifically (makes Ctrl+D end program)
            print
        except KeyboardInterrupt:
            # Ignore this one specifically (makes Ctrl+C end program)
            print
        except Exception, e:
            print traceback.print_exc()

    def interpret(self, text):
        if text.strip() == 'exit':
            return True

        m = self._quatexp.match(text.strip())
        try:
            quat = math.Quaternion(float(m.group('x')), float(m.group('y')),
                                   float(m.group('z')), float(m.group('w')))
            print 'orientation:', quat2axis(quat)
        except Exception, e:
            print traceback.print_exc()

        return False

def main():
    cmdline = CmdLine()
    cmdline.cmdloop()

if __name__ == '__main__':
    main()
