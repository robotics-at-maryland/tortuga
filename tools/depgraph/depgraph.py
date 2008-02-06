import sys, pprint
from optparse import OptionParser

from depgraph2dot import pydepgraphdot 
from py2depgraph import mymf

class DepGraph(pydepgraphdot):
    def __init__(self, depgraphs, types, output = sys.stdout):
        self._depgraphs = depgraphs
        self._types = types
        self._output = output

    def main(self, options):
        self.colored = options.colored
        if 'stdout' == options.output_file:
            self._output = sys.stdout
        else:
            self._output = file(options.output_file, 'w')
 
        self.render()
        
    def use(self, s, type):
        if s.startswith('email.'):
            return 0
        if s in ('logging.handlers', 'urllib', 'httplib'):
            return 0
        return pydepgraphdot.use(self, s, type)

    def get_output_file(self):
        return self._output

    def get_data(self):
        return self._depgraphs, self._types

def main():
    # Create module finder
    path = sys.path[:]
    debug = 0
    exclude = []
    mf = mymf(path,debug,exclude)

    # Parse command line options
    parser = OptionParser()
    parser.add_option('-m', '--mono', action='store_false', dest='colored',
                      default=True, help='Turns graph output black and white')
    parser.add_option('-o', '--option', dest='output_file', default='stdout',
                      help='Filename to store output in')
    
    (options, args) = parser.parse_args()

    # Find all the modules
    mf.run_script(args[0])

    # Run the dot file builder
    dep_graph = DepGraph(mf._depgraph, mf._types)
    dep_graph.main(options)

if __name__=='__main__':
    main()
