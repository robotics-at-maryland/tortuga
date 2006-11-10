def grabVal(namespace, path):
    nodes = path.split('.')
    obj = getattr(namespace, nodes[0]) 

    # Go down through the class hearchy untill we at the end of get nothing
    for node in nodes[1:]:
        try:
            obj = getattr(obj, node)
        except AttributeError:
            return None

    return obj

def setVal(namespace, path, val):
    """
    Factor out the node grabing to allow looking for things at the base
    level so we don't have tons of exception handling code try to combine with
    grabVal in some way
    """
    nodes = path.split('.')
    obj = getattr(namespace, nodes[0]) 

    
    for i in xrange(1, len(nodes)):
        try:
            obj = getattr(obj, nodes[i])
        except AttributeError:
            if i is len(nodes) - 1:
                setattr(obj, nodes[i], val)
            else:
                setattr(obj, nodes[i], type(nodes[i], (object,), {}))
            obj = getattr(obj, nodes[i])


def serialize(obj, filename):
    """ This is current just a proof of concept it should go to a file """

    outfile = open(filename, "w")
    do_serialize(obj, outfile)
    outfile.close()
    
def do_serialize(obj, outfile, name = None, depth = 0):
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

