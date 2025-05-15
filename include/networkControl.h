
#ifndef NETWORK_CONTROL_H
#define NETWORK_CONTROL_H

#include<string>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include "tcpClient.h"

namespace networkControl{

    bool networkControlThread(std::mutex* m, std::condition_variable* cv,bool* ready, std::wstring* buffer,std::atomic<bool>* quitSig); //main function for network control

    void sendNetworkThread(tcpClient *newClient,std::mutex* m, std::condition_variable* cv,bool* ready, std::wstring* buffer,std::atomic<bool>* quitSig); //function for sending data, never used explicitly by functions outside this namespace
    
};
#endif