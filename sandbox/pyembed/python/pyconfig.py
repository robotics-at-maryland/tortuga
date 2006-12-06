
class PyConfig(object):
    def __init__(self, source, mode='File', filename=None):
        """
        Create the config object either by loading the contents of a file 
        (default behaviour), or by supplying raw text
        """
        self.filename = filename
        file_contents = open(filename).read()
        
        tmplocals = {}
        self.config_module = {}   
        config_code = compile(file_contents, filename, 'exec')
        eval(config_code, self.config_module, tmplocals)

        
    def read(self, path):
        nodes = path.split('.')
        obj = getattr(self.config_module, nodes[0]) 
    
        # Go down through the class hearchy untill we at the end of get nothing
        for node in nodes[1:]:
            try:
                obj = getattr(obj, node)
            except AttributeError:
                return None
    
        return obj


    def write(path, val):
        """
        Factor out the node grabing to allow looking for things at the base
        level so we don't have tons of exception handling code try to combine with
        grabVal in some way
        """
        nodes = path.split('.')
        obj = getattr(self.config_module, nodes[0]) 
    
        
        for i in xrange(1, len(nodes)):
            try:
                obj = getattr(obj, nodes[i])
            except AttributeError:
                if i is len(nodes) - 1:
                    setattr(obj, nodes[i], val)
                else:
                    setattr(obj, nodes[i], type(nodes[i], (object,), {}))
                obj = getattr(obj, nodes[i])


    def save(self):
        """ This is current just a proof of concept it should go to a file """
    
        outfile = open(self.filename, "w")
        _serialize(self.config_module, outfile)
        outfile.close()
        
    def _serialize(obj, outfile, name = None, depth = 0):
        # Print out a value, as class or normal value
        if hasattr(obj, "__name__"):
            outfile.write('%sclass %s(object):\n' % ('\t' * depth, obj.__name__))
    
            # We are in a class so output its members and subclasses
            for item in dir(obj):
                if not item.startswith('__'):
                    do_serialize(getattr(obj,item), outfile, str(item), depth + 1)
        else:
            # Output the class member 
            outfile.write('%s%s = %s\n' % ('\t' * depth, name, repr(obj)))

