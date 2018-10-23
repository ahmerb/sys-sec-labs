import socket

mysocket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
mysocket.bind("./socket")
mysocket.listen(1)

while 1:
    conn, addr = mysocket.accept()
    while 1:
        data = conn.recv(1024)
        if data:
            # implement client command to instruct server to disconnect client
            if data.find("END") > -1:
                break
            # otherwise, just echo back what client sent
            conn.send("Echo: " + data)
    # close conn to this client
    conn.close()
mysocket.close()

