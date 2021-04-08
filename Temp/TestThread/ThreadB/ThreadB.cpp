// ThreadB.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

#define NAME_LINE   40

//�����̺߳�����������Ľṹ��
typedef struct __THREAD_DATA
{
    int nMaxNum;
    char strThreadName[NAME_LINE];

    __THREAD_DATA() : nMaxNum(0)
    {
        memset(strThreadName, 0, NAME_LINE * sizeof(char));
    }
}THREAD_DATA;

HANDLE g_hMutex = NULL;     //������

//�̺߳���
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    THREAD_DATA* pThreadData = (THREAD_DATA*)lpParameter;

    for (int i = 0; i < pThreadData->nMaxNum; ++i)
    {
        //������һ����������
        WaitForSingleObject(g_hMutex, INFINITE);
        cout << pThreadData->strThreadName << " --- " << i << endl;
        //Sleep(100);
        //�ͷŻ�������
        ReleaseMutex(g_hMutex);
    }
    return 0L;
}

int main()
{
    //����һ��������
    g_hMutex = CreateMutex(NULL, FALSE, NULL);

    //��ʼ���߳�����
    THREAD_DATA threadData1, threadData2;
    threadData1.nMaxNum = 5;
    strcpy(threadData1.strThreadName, "�߳�1");
    threadData2.nMaxNum = 10;
    strcpy(threadData2.strThreadName, "�߳�2");

    //������һ�����߳�
    HANDLE hThread1 = CreateThread(NULL, 0, ThreadProc, &threadData1, 0, NULL);
    //�����ڶ������߳�
    HANDLE hThread2 = CreateThread(NULL, 0, ThreadProc, &threadData2, 0, NULL);
    //�ر��߳�
    CloseHandle(hThread1);
    CloseHandle(hThread2);

    //���̵߳�ִ��·��
    for (int i = 0; i < 8; ++i)
    {
        //������һ����������
        WaitForSingleObject(g_hMutex, INFINITE);
        cout << "���߳� === " << i << endl;
        //Sleep(100);
        //�ͷŻ�������
        ReleaseMutex(g_hMutex);
    }

    Sleep(3000);
    system("pause");
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
