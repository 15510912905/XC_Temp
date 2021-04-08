// ThreadA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdlib.h>
using namespace std;

DWORD WINAPI MyThreadProc1(LPVOID lpParameter);
DWORD WINAPI MyThreadProc2(LPVOID lpParameter);
DWORD WINAPI MyThreadProc3(LPVOID lpParameter);

HANDLE hMutex;
int index = 100;
int i = 0, y = 0;

int main( int argc,char *argv[] )
{
    std::cout << "ThreadA!\n";

    hMutex = CreateMutex(NULL, FALSE, NULL);

    HANDLE handle1, handle2, handle3;
    handle1 = CreateThread(NULL, 0, MyThreadProc1, NULL, 0, NULL);
    handle2 = CreateThread(NULL, 0, MyThreadProc2, NULL, 0, NULL);
    handle3 = CreateThread(NULL, 0, MyThreadProc3, NULL, 0, NULL);

    if (NULL == handle1)
    {
        cout << "Create Thread1 failed !\n" << endl;
        return -1;
    }
    if (NULL == handle2)
    {
        cout << "Create Thread2 failed !\n" << endl;
        return -1;
    }
    if (NULL == handle3)
    {
        cout << "Create Thread3 failed !\n" << endl;
        return -1;
    }

    CloseHandle(handle1);
    CloseHandle(handle2);
    CloseHandle(handle3);

    //WaitForSingleObject( hMutex, INFINITE );
    cout << "The Main Thread is Running !\n"<< index-- << "\n" << endl;
    //ReleaseMutex( hMutex );

    Sleep(2000);
    system("PAUSE");

    return 0;
}


DWORD WINAPI MyThreadProc1(LPVOID lpParameter)
{
    DWORD dw;

    cout << "The MyThreadProc1 is Running !\n" << endl;

    while (true)
    {
        dw = WaitForSingleObject(hMutex, INFINITE);
        switch (dw)
        {
            case WAIT_OBJECT_0:
                // hProcess所代表的进程在5秒内结束
                break;

            case WAIT_TIMEOUT:
                // 等待时间超过5秒
                break;

            case WAIT_FAILED:
                // 函数调用失败，比如传递了一个无效的句柄
                break;
        }

        if (index > 0)
        {
            Sleep(1);
            cout << "The Index1 Number is : " << index-- << endl;
        }
        else {
            break;
        }    

        ReleaseMutex(hMutex);
    }

    return 0;
}

DWORD WINAPI MyThreadProc2(LPVOID lpParameter)
{
    DWORD dw;

    cout << "The MyThreadProc2 is Running !\n" << endl;

    while (true)
    {
        dw = WaitForSingleObject(hMutex, INFINITE);
        switch (dw)
        {
            case WAIT_OBJECT_0:
                // hProcess所代表的进程在5秒内结束
                break;

            case WAIT_TIMEOUT:
                // 等待时间超过5秒
                break;

            case WAIT_FAILED:
                // 函数调用失败，比如传递了一个无效的句柄
                break;
        }

        if (index > 0)
        {
            Sleep(1);
            cout << "The Index2 Number is : " << index-- << endl;
        }
        else {
            break;
        }

        ReleaseMutex(hMutex);
    }

    return 0;
}

DWORD WINAPI MyThreadProc3(LPVOID lpParameter)
{
    DWORD dw;

    cout << "The MyThreadProc3 is Running !\n" << endl;

    while (true)
    {
        dw = WaitForSingleObject(hMutex, INFINITE);
        switch (dw)
        {
            case WAIT_OBJECT_0:
                // hProcess所代表的进程在5秒内结束
                break;

            case WAIT_TIMEOUT:
                // 等待时间超过5秒
                break;

            case WAIT_FAILED:
                // 函数调用失败，比如传递了一个无效的句柄
                break;
        }

        if (index > 0)
        {
            Sleep(1);
            cout << "The Index3 Number is : " << index-- << endl;
        }
        else {
            break;
        }

        ReleaseMutex(hMutex);
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
