import SocketServer

PORTNO = 10552

class handler(SocketServer.DatagramRequestHandler):
    def handle(self):
        newmsg = self.rfile.readline().rstrip()
        print "Client %s said ``%s''" % (self.client_address[0], newmsg)
        self.wfile.write(self.server.oldmsg)
        self.server.oldmsg = newmsg

s = SocketServer.UDPServer(('',PORTNO), handler)
print "Awaiting UDP messages on port %d" % PORTNO
s.oldmsg = "This is the starting message."
s.serve_forever()
