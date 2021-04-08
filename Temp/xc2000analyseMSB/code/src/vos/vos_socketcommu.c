/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_socketcommu.c
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��11��20��
  ����޸�   :
  ��������   : vos��Ϣsocketͨ��ʵ���ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include <string.h>
#include "app_msg.h"
#include "vos_configure.h"
#include "vos_basictypedefine.h"
#include "vos_errorcode.h"
#include "vos_selfmessagecenter.h"
#include "vos_selfsocketcommu.h"
#include "vos_adaptsys.h"
#include "vos_pid.h"
#include "vos_init.h"
#include "vos_log.h"
#include "vos_mnglist.h"
#include "vos_mngmemory.h"
#include "typedefine.pb.h"
#include "vos_messagecenter.h"
#include "socketmonitor.h"
#include "vos_socketcommu.h"
#include "apputils.h"
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>  
#include <fstream>  
#include <streambuf>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h> 


#ifdef __cplusplus
extern "C" {
#endif

STATIC vos_socket g_ulclientconnetsock      = VOS_NULL_SOCKET;
STATIC void*  g_pmutexforclienconntsock = VOS_NULL;
vos_socket g_ulserverconnetsock = VOS_NULL_SOCKET;
string g_szUpMachIp;
/*****************************************************************************
 �� �� ��  : vos_disconnectsocket
 ��������  : �Ͽ�socket����
 �������  : vos_u32 ulsocketipaddr
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��2��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_disconnectsocket()
{
    vos_obtainmutex(g_pmutexforclienconntsock);
    vos_closesocket(g_ulclientconnetsock);
    g_ulclientconnetsock = VOS_NULL_SOCKET;
    vos_releasemutex(g_pmutexforclienconntsock);

    if (vos_midmachine_state_resetting == g_pMonitor->m_ulState || vos_midmachine_state_rewarming == g_pMonitor->m_ulState)
    {
        return VOS_OK;
    }

    if (g_pMonitor->m_ulState >= vos_midmachine_state_testing &&g_pMonitor->m_ulState <= vos_midmachine_state_pause)
    {
        return VOS_OK;
    }

	printf("  dis connect \n");
    return sendInnerMsg(SPid_Oammng, SPid_SocketMonitor, MSG_ReqStopMach);
}

/*****************************************************************************
 �� �� ��  : vos_msgsocketsend
 ��������  : vos��Ϣsocket���ͽӿ�
 �������  : vos_msg_header_stru* pstvosmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��25��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_msgsocketsend(vos_msg_header_stru* pstvosmsg)
{
    if (VOS_NULL == pstvosmsg)
    {
        RETURNERRNO(VOS_COM_PID, vos_para_null_err);
    }

    vos_u32 ulpkglen = pstvosmsg->usmsgloadlen + sizeof(vos_msg_header_stru); 

    vos_obtainmutex(g_pmutexforclienconntsock);
    vos_u32 ulrunrslt = vos_socksend(g_ulclientconnetsock, (char*)pstvosmsg, ulpkglen);
    vos_releasemutex(g_pmutexforclienconntsock);

    if (VOS_OK != ulrunrslt)
    {
        g_pMonitor->Disconnect();
        WRITE_ERR_LOG("vos_socksend error=%lx\n", ulrunrslt);
    }
    return ulrunrslt;
}

/*****************************************************************************
 �� �� ��  : vos_builtvosmsgpackforsocketrcv
 ��������  : Ϊsocket������װvos��Ϣ��
 �������  : vos_msg_header_stru* pstmsgbuf
             vos_u32 ulbuflen
 �������  : ��
 �� �� ֵ  : vos_u32(��ǰ��װ��VOS��Ϣ����)
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��29��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_builtvosmsgpackforsocketrcv(vos_msg_header_stru* pstmsgbuf, vos_u32 ulbuflen)
{
    vos_u32 ulcurbuildmsglen = 0;
    vos_u32 ulrunrslt = VOS_OK;

    vos_msg_header_stru* pstnewmsgbuf = VOS_NULL;

    ulcurbuildmsglen = sizeof(vos_msg_header_stru) + pstmsgbuf->usmsgloadlen;
    if (ulcurbuildmsglen > ulbuflen)
    {
        WRITE_ERR_LOG("Received data error(msglen= %d, ulbuflen= %d)\r\n", ulcurbuildmsglen, ulbuflen);
        return 0;
    }

    ulrunrslt = vos_mallocmsgbuffer(ulcurbuildmsglen, (void**)(&pstnewmsgbuf));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer failed(0x%x)\r\n", ulrunrslt);
        return 0;
    }

    (void)memcpy((char*)pstnewmsgbuf, (char*)pstmsgbuf, ulcurbuildmsglen);

    /*����Ϣ�ڴ�ҽӵ�Ŀ��PID����FID�Ľ�����Ϣ������*/
    ulrunrslt = vos_mountmsgtorcvmailbox(pstnewmsgbuf);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("vos_mountmsgtorcvmailbox failed(0x%x),pid=%lu,msg=%#x\n", ulrunrslt, pstmsgbuf->uldstpid, pstmsgbuf->usmsgtype);
        vos_freemsgbuffer(pstnewmsgbuf);
    }

    return ulcurbuildmsglen;
}

//��ȡ����Ϣ����
vos_u32 vos_getMsgLen(vos_u32 ultotalrcvlen, vos_u32 ulMsgLen, char* pucrcvdata)
{
    vos_msg_header_stru* pMsg = NULL;
    while (ultotalrcvlen > ulMsgLen)
    {
        pMsg = (vos_msg_header_stru*)(pucrcvdata + ulMsgLen);
        ulMsgLen += (sizeof(vos_msg_header_stru) + pMsg->usmsgloadlen);
    }
    return ulMsgLen;
}

//�����յ�����Ϣ
void vos_procRecvMsg(char* pucrcvdata, vos_u32 ultotalrcvlen)
{
    vos_u32    ulsendedlen = 0;
    vos_u32    ulsendedsum = 0;
    char* pucsendpos = VOS_NULL;

    while (1)
    {
        pucsendpos = pucrcvdata + ulsendedsum;
        ulsendedlen = vos_builtvosmsgpackforsocketrcv((vos_msg_header_stru*)pucsendpos, (ultotalrcvlen - ulsendedsum));

        ulsendedsum += ulsendedlen;
        if ((0 == ulsendedlen) || (ulsendedsum >= ultotalrcvlen))
        {
            break;
        }
    }
}

//����һ����Ϣ
vos_u32 vos_recvMsg(vos_socket ulclientsock, char* pucrcvdata, vos_u32& ulTotalRcvLen, vos_u32& ulMsgLen)
{
    vos_u32 ulrunrslt = VOS_OK;
    char  aucrcvbuf[VOS_MAX_SOCK_RCV_LEN] = { '0' };
    vos_u32 ulrcvlen = 0;
    ulrunrslt = vos_sockreceive(ulclientsock, (char*)aucrcvbuf, VOS_MAX_SOCK_RCV_LEN, &ulrcvlen);
    if (VOS_OK != ulrunrslt)
    {
        //ʧ�ܱ�ʾ��ǰ�ͻ���sock�Ѿ������쳣��Ͽ�,�߳���Ҫ�˳�
        vos_closesocket(ulclientsock);
        vos_freememory((void*)pucrcvdata);

        WRITE_WARN_LOG("call vos_sockreceive failed(0x%x),exist cur thread\r\n", ulrunrslt);
        return VOS_EXIST_THREAD_CODE;
    }
    (void)memcpy((pucrcvdata + ulTotalRcvLen), (char*)aucrcvbuf, ulrcvlen);
    ulTotalRcvLen += ulrcvlen;
    ulMsgLen = vos_getMsgLen(ulTotalRcvLen, ulMsgLen, pucrcvdata);

    return VOS_OK;
}
//-----����accept�����߳�

vos_u32 vos_acceptthread(void* ppara)
{
	//���տͻ�������
	vos_u32 ulrunrslt = VOS_OK;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);

	while (true)
	{
		int clnt_sock = accept(g_ulserverconnetsock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if (0 > clnt_sock)
		{
			continue;
		}
        g_szUpMachIp = inet_ntoa(clnt_addr.sin_addr);
		vos_obtainmutex(g_pmutexforclienconntsock);	
		vos_closesocket(g_ulclientconnetsock);
		g_ulclientconnetsock = clnt_sock;
		vos_releasemutex(g_pmutexforclienconntsock);

		vos_multi_thread_para_stru stthdinfo;
		stthdinfo.arglen       = sizeof(g_ulclientconnetsock);
		stthdinfo.eninitstatus = vos_thread_init_status_running;
		stthdinfo.pthrdfunc    = vos_receiveclientsockthdenter;
		stthdinfo.pthrdinpara  = (void*)(&g_ulclientconnetsock);
		stthdinfo.ulstacksize  = VOS_FID_DEFAULT_STACK_SIZE;
		stthdinfo.ulthrdid     = 0;
		stthdinfo.ulthreadpri  = vos_thread_pri_70;

		 ulrunrslt = vos_createthread(&stthdinfo);
	

		if (VOS_OK != ulrunrslt)
		{
			continue;
		}		
        WRITE_INFO_LOG("client %s connect to server success.\n", g_szUpMachIp.c_str());
		printf("client %s connect to server success.\n", g_szUpMachIp.c_str());
		msg_stHead stHead;
		app_constructMsgHead(&stHead, SPid_SocketMonitor, APP_COM_PID, MSG_NotifyConnect);
		app_sendMsg(&stHead, NULL, 0);	
	}

	return true;
}

//���տͷ���sock���ݰ��߳����
vos_u32 vos_receiveclientsockthdenter(void* ppara)
{
    vos_socket ulclientsock = *((vos_socket*)ppara);
    vos_u32    ulrunrslt = VOS_OK;
    vos_u32    ultotalrcvlen = 0;
    vos_u32    ulrcvcounter = 1;
    char* pucrcvdata = VOS_NULL;
    char* puctmpbkup = VOS_NULL;

    vos_u32 ulMsgLen = 0;
    ulrunrslt = vos_mallocmemory(VOS_MAX_SOCK_RCV_LEN * ulrcvcounter, vos_mem_type_static, (void**)(&pucrcvdata));
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    ulrunrslt = vos_recvMsg(ulclientsock, pucrcvdata, ultotalrcvlen, ulMsgLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_WARN_LOG("vos_recvMsg failed=%lu\n", ulrunrslt);
        *((vos_socket*)ppara) = VOS_NULL_SOCKET;
        return ulrunrslt;
    }

    while (ultotalrcvlen != ulMsgLen)
    {
        ulrcvcounter += 1;
        puctmpbkup = pucrcvdata;
        pucrcvdata = VOS_NULL;
        ulrunrslt = vos_mallocmemory(VOS_MAX_SOCK_RCV_LEN * ulrcvcounter, vos_mem_type_static, (void**)(&pucrcvdata));
        if (VOS_OK != ulrunrslt)
        {
            vos_freememory(puctmpbkup);
            return ulrunrslt;
        }
        (void)memcpy(pucrcvdata, puctmpbkup, ultotalrcvlen);

        vos_freememory(puctmpbkup);
        puctmpbkup = VOS_NULL;
        ulrunrslt = vos_recvMsg(ulclientsock, pucrcvdata, ultotalrcvlen, ulMsgLen);
        if (VOS_OK != ulrunrslt)
        {
            WRITE_ERR_LOG("vos_recvMsg failed=%lu\n", ulrunrslt);
            *((vos_socket*)ppara) = VOS_NULL_SOCKET;
            return ulrunrslt;
        }
    }

    vos_procRecvMsg(pucrcvdata, ultotalrcvlen);
    vos_freememory((void*)pucrcvdata);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_clientsockinit
 ��������  : �ͻ���socket��ʼ���ӿ�
 �������  : vos_u32 ulservip
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��25��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_clientsockinit(void)
{
    g_pmutexforclienconntsock = vos_mallocmutex();
    (void)vos_createmutex(g_pmutexforclienconntsock);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_setupsignal
 ��������  : ����send�źŷ��͵�����socket�ϣ�����SIGPIPE�ź�
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��6��9��
    ��    ��   : �ƺ�
    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_setupsignal() 
{
    struct sigaction sa;

    //��linux��дsocket�ĳ����ʱ���������send��һ��disconnected socket�ϣ��ͻ��õײ��׳�һ��SIGPIPE�źš�
    //����źŵ�ȱʡ���������˳�����
    //��������źŵĴ�����,������յ�һ��SIGPIPE�źţ����Ը��ź�
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    
    //sigemptyset()����������set�źż���ʼ�������
    if ((sigemptyset(&sa.sa_mask) == -1) ||
        (sigaction(SIGPIPE, &sa, 0) == -1))
    {
        return;
    }
}

vos_u32 GetServerPort()
{
    vos_u32 uiport = 20000;

#ifdef COMPILER_IS_LINUX_GCC
    CServerPort rPort;
    TFName szBackCfg;
    sprintf(szBackCfg, "%s/serverport.cfg", GetExePath());
    int infile = open(szBackCfg, O_RDONLY);

    if (infile >= 0)
    {
        google::protobuf::io::FileInputStream fileInput(infile);
        if (google::protobuf::TextFormat::Parse(&fileInput, &rPort))
        {
            uiport = rPort.uiport() > 0 ? rPort.uiport() : uiport;
        }
        close(infile);
    }
#endif
    printf("socket bind port = %lu\n", uiport);
    return uiport;
}

/*****************************************************************************
 �� �� ��  : vos_initsockcommumd
 ��������  : socketͨ��ģ���ʼ���ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_initsockcommumd()
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_setupsignal();
    ulrunrslt = vos_clientsockinit();	
	
	vos_obtainmutex(g_pmutexforclienconntsock);
	if (VOS_NULL_SOCKET != g_ulclientconnetsock)
	{
		vos_releasemutex(g_pmutexforclienconntsock);
		return VOS_OK;
	}

    g_ulserverconnetsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int opt = 1;
    setsockopt(g_ulserverconnetsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	//�����׽���
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //ÿ���ֽڶ���0���
	serv_addr.sin_family = AF_INET;  //ʹ��IPv4��ַ
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    serv_addr.sin_port = htons(GetServerPort());  //�˿�

	ulrunrslt = ::bind(g_ulserverconnetsock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ulrunrslt != 0)
	{
		printf("socket bind is error %lu\n", ulrunrslt);
		vos_releasemutex(g_pmutexforclienconntsock);
		return ulrunrslt;
	}

	ulrunrslt = listen(g_ulserverconnetsock, 2);
	if (0 != ulrunrslt)
	{
        printf("socket bind is error %lu\n", ulrunrslt);
		vos_releasemutex(g_pmutexforclienconntsock);
		return ulrunrslt;
	}

	//---����accept�߳�
	vos_multi_thread_para_stru stthdinfoaccept;
	stthdinfoaccept.arglen = sizeof(g_ulserverconnetsock);
	stthdinfoaccept.eninitstatus = vos_thread_init_status_running;
	stthdinfoaccept.pthrdfunc = vos_acceptthread;
	stthdinfoaccept.pthrdinpara = (void*)(&g_ulserverconnetsock);
	stthdinfoaccept.ulstacksize = VOS_FID_DEFAULT_STACK_SIZE;
	stthdinfoaccept.ulthrdid = 0;
	stthdinfoaccept.ulthreadpri = vos_thread_pri_70;

	ulrunrslt = vos_createthread(&stthdinfoaccept);
	if (VOS_OK != ulrunrslt)
	{
		vos_releasemutex(g_pmutexforclienconntsock);
		return ulrunrslt;
	}
	
	vos_releasemutex(g_pmutexforclienconntsock);
    return ulrunrslt;
	
}

#ifdef __cplusplus
}
#endif

