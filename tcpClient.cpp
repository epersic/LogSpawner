//#include "tcpClient.h"
#include<iostream>
#include<winsock2.h>
#include<string>

class tcpClient{
public:
    tcpClient(std::string serverIpAddr,USHORT serverPort){
        this->serverIpAddr = serverIpAddr;
        this->serverPort = serverPort;
        this->initConnection();
        this->connectToServer();
    }

    bool transmitData(std::string data){
        
        std::string strBuf = std::to_string(data.size());
        std::cout<<"Data size: "<<strBuf.size()<<std::endl;
        if(strBuf.size() < 1024){
            strBuf.insert(0,1024-strBuf.size(),'0');
        }
        std::cout<<"Data size: "<<strBuf.size()<<std::endl;
        int bytesSent=send(this->clientSoc,strBuf.c_str(),strBuf.size(),0);
        std::cout<<bytesSent<<std::endl;

        if(bytesSent == SOCKET_ERROR){

            std::cout << WSAGetLastError() << std::endl;

            return false;
        }else{
            std::cout<<bytesSent<<std::endl;
        }

        return true;
    }


    bool recvData(char *strBuffer){
        /*char buff[1024];
        int bytesRcvd = recv(this->clientSoc,buff,1024,0);

        if(bytesRcvd < 0){
            std::cout<<WSAGetLastError()<< std::endl;
            return false;
        }

        *strBuffer = *buff;*/
        return true;
    }

    bool closeConnection(){
        closesocket(this->clientSoc);
        WSACleanup();
        return true;
    }

private:
    std::string serverIpAddr;
    USHORT serverPort;
    SOCKET clientSoc;

    bool initConnection(){
        WSADATA wsaData;
        int wserr = WSAStartup(MAKEWORD(2,2),&wsaData);     // didn't know what this was for at first, turns out you need to initialize Winsock maunally, it doesn't init it automatically like linux
        if (wserr != 0){
            std::cout<<"The winsock dll not found"<< std::endl;   ///obviously remove this in next stage, this should be for debugging only <- Remove after testing
            return 0;
        }

        clientSoc = INVALID_SOCKET;
        clientSoc = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

        if (clientSoc == INVALID_SOCKET){
            std::cout<<WSAGetLastError()<< std::endl;     ///remove after debug
            WSACleanup();
            return 0;
        }

        return 1;
    }

    bool connectToServer(){

        sockaddr_in clientService;

        clientService.sin_family = AF_INET;
        clientService.sin_addr.s_addr = inet_addr(this->serverIpAddr.c_str());
        clientService.sin_port = htons(this->serverPort);

        int connRes = connect(clientSoc,(SOCKADDR *) & clientService, sizeof(clientService));

        if(connRes == SOCKET_ERROR){
            WSACleanup();
            return false;
        }
        return true;
    }

};
