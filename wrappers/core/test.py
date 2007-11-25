import ext.core as core

class MockSub(core.Subsystem):
    def __init__(self):
        print 'Created MockSub'
     
class MockSubMaker(core.SubsystemMaker):
    def __init__(self):
        core.SubsystemMaker.__init__(self, "MockSub")
    def makeObject(self, params):
        print params
        return MockSub()
