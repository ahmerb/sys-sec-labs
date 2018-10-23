import socket
import sys

mysocket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
mysocket.connect("./socket")
mysocket.send("hello")
response = mysocket.recv(1024)
print("Server said: " + response)
mysocket.send("END")
mysocket.close()

