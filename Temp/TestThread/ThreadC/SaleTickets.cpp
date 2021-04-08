#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iostream>
#include "SaleTickets.h"

using namespace std;

HANDLE g_hMutex;

//��Ʊ����
DWORD WINAPI SaleTicket(LPVOID lpParameter)
{

    THD_DATA* pThreadData = (THD_DATA*)lpParameter;
    TICKET* pSaleData = pThreadData->pTicket;

    while (pSaleData->nCount > 0)
    {
        //������һ����������
        WaitForSingleObject(g_hMutex, INFINITE);
        if (pSaleData->nCount > 0)
        {
            cout << pThreadData->strThreadName << "���۵�" << pSaleData->nCount-- << "��Ʊ,";
            if (pSaleData->nCount >= 0) {
                cout << "��Ʊ�ɹ�!ʣ��" << pSaleData->nCount << "��Ʊ." << endl;
            }
            else {
                cout << "��Ʊʧ�ܣ���Ʊ�����ꡣ" << endl;
            }
        }
        Sleep(10);
        //�ͷŻ�������
        ReleaseMutex(g_hMutex);
    }

    return 0L;
}

//��Ʊϵͳ
void TestSys()
{
    //����һ��������
    g_hMutex = CreateMutex(NULL, FALSE, NULL);

    //��ʼ����Ʊ
    TICKET ticket;
    ticket.nCount = 100;
    strcpy(ticket.strTicketName, "����-->����");

    const int THREAD_NUMM = 8;
    THD_DATA threadSale[THREAD_NUMM];
    HANDLE hThread[THREAD_NUMM];
    for (int i = 0; i < THREAD_NUMM; ++i)
    {
        threadSale[i].pTicket = &ticket;
        //string strThreadName = convertToString(i);
        string strThreadName = to_string(i);

        strThreadName = "����" + strThreadName;

        strcpy(threadSale[i].strThreadName, strThreadName.c_str());

        //�����߳�
        hThread[i] = CreateThread(NULL, NULL, SaleTicket, &threadSale[i], 0, NULL);

        //������һ����������
        WaitForSingleObject(g_hMutex, INFINITE);
        cout << threadSale[i].strThreadName << "��ʼ���� " << threadSale[i].pTicket->strTicketName << " ��Ʊ..." << endl;
        //�ͷŻ�������
        ReleaseMutex(g_hMutex);

        //�ر��߳�
        CloseHandle(hThread[i]);
    }

    Sleep( 5000 );
    system("pause");
}