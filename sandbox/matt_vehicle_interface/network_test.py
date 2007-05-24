import Client
import Server

client = Client.SimpleClient()
server = Server.SimpleServer()
server.start()
client.init('localhost',8085)
client.start()
client.close()

