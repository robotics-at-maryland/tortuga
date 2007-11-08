import Queue

class EventArgs(object):
    def __init__(self, **args):
        self.args = args

    def __eq__(self, other):
        cmpObj = other
        if isinstance(other, EventArgs):
            cmpObj = other.args
        return self.args == cmpObj
    
    def __str__(self):
        return str(self.args)
    
    def __repr(self):
        return self.__str__()
    
    def __hash__(self):
        return str(self.args).__hash__()

    def __getattr__(self, item):
        try:
            return self.args[item]
        except KeyError:
            raise AttributeError(item)

class EventManager(object):
    def __init__(self):
        self._eventMap = {}
    
    def subscribe(self, eventType, handler):
        self._eventMap.setdefault(eventType, []).append(handler)

    def publish(self, eventType, sender, **args):
        if self._eventMap.has_key(eventType):
            handlers = self._eventMap[eventType]
            [h(eventType, sender, EventArgs(**args)) for h in handlers]
            
        
class QueuedEventManager(EventManager):
    def __init__(self, eventManager, queueSize = 4096):
        EventManager.__init__(self)
        self.emgr = eventManager
        self._messageQueue = Queue.Queue(queueSize)

    def subscribe(self, eventType, handler):
        # Subscribe our queueing function to the real event
        self.emgr.subscribe(eventType, self._handler)
        
        # Now subscribe the incoming handler for out outgoing event
        EventManager.subscribe(self, eventType, handler)

    def _handler(self, eventType, sender, args):
        """
        Saves event for later on our internal queue
        """
        self._messageQueue.put((eventType, sender, args))

    def processMessages(self):
        while not self._messageQueue.empty():
            etype, sender, args = self._messageQueue.get_nowait()
            self.publish(etype, sender, **args.args)
        
