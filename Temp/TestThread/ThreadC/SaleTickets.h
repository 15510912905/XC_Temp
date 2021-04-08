#pragma once

#include <windows.h>
#include <iostream>
#include <strstream> 
#include <string>

using namespace std;

#define NAME_LINE   40

extern HANDLE g_hMutex;

//�����̺߳�����������Ľṹ��
typedef struct __TICKET
{
    int nCount;
    char strTicketName[NAME_LINE];

    __TICKET() : nCount(0)
    {
        memset(strTicketName, 0, NAME_LINE * sizeof(char));
    }
}TICKET;

typedef struct __THD_DATA
{
    TICKET* pTicket;
    char strThreadName[NAME_LINE];

    __THD_DATA() : pTicket(NULL)
    {
        memset(strThreadName, 0, NAME_LINE * sizeof(char));
    }
}THD_DATA;


//������������ת�����ַ���
template<typename  T>
string convertToString(const T val)
{
    string s;
    std::strstream ss;
    ss << val;
    ss >> s;
    return s;
}

//��Ʊ����
DWORD WINAPI SaleTicket(LPVOID lpParameter);
void TestSys();