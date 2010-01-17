
# STD Imports
from optparse import OptionParser

# Project Imports
import top.top as top

def vprint(verbose):
    if verbose:
        def handler(text):
            print text
        return handler
    else:
        def handler(text):
            pass
        return handler

def main():
    # Parse options
    parser = OptionParser()
    parser.add_option('-f', dest='filename', default=None,
                      help='write report to FILE', metavar='FILE')
    parser.add_option('-v', dest='verbose', default=True,
                      help='verbose mode')
    parser.add_option('-s', '--size', dest='size', default=1,
                      help='size of the averaging filter SIZE', metavar='SIZE')

    (options, args) = parser.parse_args()

    # Setting print mode
    printfunc = vprint(options.verbose)
    printfunc("Setting print mode to verbose")

    file = None
    size = int(options.size)
    if options.filename is not None:
        printfunc("Opening file %s in write mode" % options.filename)
        file = open(options.filename, 'w')
        printfunc("Recording data buffer is size %d" % size)
        printfunc("Averaged data will be flushed to the disk")
    else:
        printfunc("No file specified. Data will not be saved to disc.")

    printfunc("Starting main program")
    
    top.AnalyzeComputer(file = file, size = size)

if __name__ == '__main__':
    main()
