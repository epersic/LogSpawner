import socket 
bindIp = "127.0.0.1"
bindPort = 1234

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bindIp, bindPort))

server.listen(5)

print("[*] Listening on %s:%d" % (bindIp, bindPort))

client, addr = server.accept()
print("[*] Accepted connection from: %s:%d" % (addr[0], addr[1]))

while True:
    incomingSize = client.recv(1024)
    incomingSize = incomingSize.decode()
    
    if not incomingSize:
        print("Connection closed")
        break

    print(int(incomingSize))
    # msg = client.recv(int(incomingSize))
    # print(msg)
