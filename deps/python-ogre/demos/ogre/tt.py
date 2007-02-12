import Ogre
                
class SectionIterator ( Ogre.SectionIterator):
    def __init__(self, iterator ):
        self.iterator = iterator
    def __iter__(self):
            return self
    def next(self):
        if self.iterator.hasMoreElements():
            return self.iterator.peekNextKey(), self.iterator.getNext()
        else:
            raise StopIteration
           
            
config = Ogre.ConfigFile()
config.load('resources.cfg' ) #, '', False )
seci = SectionIterator ( config.getSectionIterator() )

for (key, item ) in t:
    print key, item           