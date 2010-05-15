
import commands
import os
import sys

def main():
    if len(sys.argv) < 3:
        raise Exception("Usage: python convert.py FOLDER FORMAT")

    dirname = sys.argv[1]
    format = sys.argv[2]
    stock = os.path.join(dirname, 'stock')
    files = os.listdir(stock)
    for f in files:
        command = 'convert ' + os.path.join(stock, f) + ' ' + \
            os.path.join(dirname, format, os.path.splitext(f)[0] + '.' + format)
        print command,
        if commands.getstatusoutput(command)[0] == 0:
            print 'OK!'
        else:
            print 'Failed'

if __name__ == '__main__':
    main()
