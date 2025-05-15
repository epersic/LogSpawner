#include "tcpClient.h"
#include <iostream>

tcpClient::tcpClient(std::string serverIpAddr, USHORT serverPort) {
    this->serverIpAddr = serverIpAddr;
    this->serverPort = serverPort;
    
}

bool tcpClient::transmitData(std::string data) {

    int bytesSent = send(this->clientSoc, data.c_str(), data.size(), 0);

    if (bytesSent == SOCKET_ERROR) {
        std::cout << WSAGetLastError() << std::endl;
        return false;
    } else {
        std::cout << bytesSent << std::endl;
    }

    return true;
}

int tcpClient::recvData(char *strBuffer) {
    int bytesReceived = recv(this->clientSoc, strBuffer, 1024, 0);
    
    return bytesReceived;
}

bool tcpClient::closeConnection() {
    this->transmitData("[TCP SHUTDOWN]");
    closesocket(this->clientSoc);
    WSACleanup();
    return true;
}

bool tcpClient::initConnection() {
    WSADATA wsaData;
    int wserr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wserr != 0) {
        std::cout << "The winsock dll not found" << std::endl;
        return false;
    }

    clientSoc = INVALID_SOCKET;
    clientSoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSoc == INVALID_SOCKET) {
        std::cout << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    return true;
}

bool tcpClient::connectToServer() {
    this->initConnection();
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(this->serverIpAddr.c_str());
    clientService.sin_port = htons(this->serverPort);

    int connRes = connect(clientSoc, (SOCKADDR *)&clientService, sizeof(clientService));

    if (connRes == SOCKET_ERROR) {
        WSACleanup();
        return false;
    }
    return true;
}
