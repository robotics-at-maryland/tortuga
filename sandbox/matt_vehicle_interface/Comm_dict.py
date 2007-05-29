class comm_dict:
    messages = {"run":"start the robots control sequence","kill":"kill the robots power"}
    
    def __init__(self):
        print "Initializing a communications dictionary"
    def get_message(self,message):
        return message
    
    