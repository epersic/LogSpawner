#include "networkControl.h"
#include "tcpClient.h"
#include<thread>
#include<chrono>
#include<atomic>
#include<string>
#include<iostream>
namespace networkControl{
    void sendNetworkThread( tcpClient *newClient ,std::mutex* m, std::condition_variable* cv,bool* ready, std::wstring* buffer, std::atomic<bool>* quitSig){
        
        while(true){

            
            std::unique_lock<std::mutex> lock(*m);
            cv->wait(lock,[ready]{return *ready;});
  
            *ready = false;
            
            if(*quitSig == true){                                   //chech if quit signal is set and exit if true
  
                newClient->transmitData("[CLOSING CONNECTION]");
                newClient->closeConnection();
                return;

            }

            
            
            std::string strBuff( (*buffer).begin(),(*buffer).end());    //convert wstring to string for transmission
            if(newClient->transmitData(strBuff) == false){              //if error occurs during transmission, close connection and exit
                return;
            }
            buffer->clear();                                          //clear the buffer after sending data
        }
    }
    bool networkControlThread(std::mutex* m, std::condition_variable* cv,bool* ready, std::wstring* buffer,std::atomic<bool>* quitSig){
        
        tcpClient newClient("127.0.0.1",(USHORT)1234);      //Socket object and assign it to a private class variable

        while (true)
        {                  //initialize connection
            if(newClient.connectToServer() == false){
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                continue;
            }else{
                
                std::thread sendThread(
                    networkControl::sendNetworkThread,&newClient, m, cv, ready, buffer, quitSig
                );

                sendThread.join();
            }
                
            /*while(*quitSig == false){
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
            
            {
                std::lock_guard<std::mutex> lock(*m);
                *ready = true;
            }
            cv->notify_one();*/
        }
            
    }



}