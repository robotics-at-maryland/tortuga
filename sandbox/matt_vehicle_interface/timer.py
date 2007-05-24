import time

class timer:
    def __init__(self):
        self.init_time = time.time()
        
    def start(self):
        self.start_time = time.time()
        
    def time(self):
        return time.time() - self.start_time