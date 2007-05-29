import Client
import Server

client = Client.SimpleClient()
server = Server.SimpleServer()
server.init(8090)
server.start()
client.init('localhost',8090)
client.start()
client.close()

