class main_vehicle:
    def __init__(self,vc):
        print "Creating a vehicle"
        self.vision_comm = vc
    def start_vision_system(self):
        self.vision_comm.start_vision_thread()
        return self.vision_comm.error
    def get_vision_structure(self):
        return self.vision_comm.get_data()
    