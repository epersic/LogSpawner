#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include <winsock2.h>

class tcpClient {
public:
    tcpClient(std::string serverIpAddr, USHORT serverPort);
    bool transmitData(std::string data);
    bool recvData(char *strBuffer);
    bool closeConnection();

private:
    std::string serverIpAddr;
    USHORT serverPort;
    SOCKET clientSoc;

    bool initConnection();
    bool connectToServer();
};

#endif // TCPCLIENT_H
