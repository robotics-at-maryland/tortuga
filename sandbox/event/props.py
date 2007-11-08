
class PropertyClass(object):
    def __init__(self):
        self._props = {}
    
    def addProperty(self, name, readFunc, writeFunc = None):
        if self._props.has_key(name):
            raise Exception("Object already has property: " + name)
        self._props[name] = Property(name, readFunc, writeFunc)

    def getProperty(self, name):
        return self._props[name]

class Property(object):
    def __init__(self, name, readFunc, writeFunc):
        self.name = name
        self.readFunc = readFunc
        self.writeFunc = writeFunc
    def value(self):
        return self.readFunc()

    def set(self, value):
        self.writeFunc(value)
