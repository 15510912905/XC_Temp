/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_socketcommu.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年11月20日
  最近修改   :
  功能描述   : vos消息socket通信实现文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng
    修改内容   : 创建文件

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
 函 数 名  : vos_disconnectsocket
 功能描述  : 断开socket连接
 输入参数  : vos_u32 ulsocketipaddr
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年2月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
 函 数 名  : vos_msgsocketsend
 功能描述  : vos消息socket发送接口
 输入参数  : vos_msg_header_stru* pstvosmsg
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月25日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
 函 数 名  : vos_builtvosmsgpackforsocketrcv
 功能描述  : 为socket接收组装vos消息包
 输入参数  : vos_msg_header_stru* pstmsgbuf
             vos_u32 ulbuflen
 输出参数  : 无
 返 回 值  : vos_u32(当前组装的VOS消息长度)
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月29日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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

    /*将消息内存挂接到目的PID所属FID的接收消息邮箱中*/
    ulrunrslt = vos_mountmsgtorcvmailbox(pstnewmsgbuf);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("vos_mountmsgtorcvmailbox failed(0x%x),pid=%lu,msg=%#x\n", ulrunrslt, pstmsgbuf->uldstpid, pstmsgbuf->usmsgtype);
        vos_freemsgbuffer(pstnewmsgbuf);
    }

    return ulcurbuildmsglen;
}

//获取总消息长度
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

//处理收到的消息
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

//接收一次消息
vos_u32 vos_recvMsg(vos_socket ulclientsock, char* pucrcvdata, vos_u32& ulTotalRcvLen, vos_u32& ulMsgLen)
{
    vos_u32 ulrunrslt = VOS_OK;
    char  aucrcvbuf[VOS_MAX_SOCK_RCV_LEN] = { '0' };
    vos_u32 ulrcvlen = 0;
    ulrunrslt = vos_sockreceive(ulclientsock, (char*)aucrcvbuf, VOS_MAX_SOCK_RCV_LEN, &ulrcvlen);
    if (VOS_OK != ulrunrslt)
    {
        //失败表示当前客户端sock已经出现异常或断开,线程需要退出
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
//-----创建accept连接线程

vos_u32 vos_acceptthread(void* ppara)
{
	//接收客户端请求
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

//接收客服端sock数据包线程入口
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
 函 数 名  : vos_clientsockinit
 功能描述  : 客户端socket初始化接口
 输入参数  : vos_u32 ulservip
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月25日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_clientsockinit(void)
{
    g_pmutexforclienconntsock = vos_mallocmutex();
    (void)vos_createmutex(g_pmutexforclienconntsock);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_setupsignal
 功能描述  : 对于send信号发送到断连socket上，忽略SIGPIPE信号
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月9日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
void vos_setupsignal() 
{
    struct sigaction sa;

    //在linux下写socket的程序的时候，如果尝试send到一个disconnected socket上，就会让底层抛出一个SIGPIPE信号。
    //这个信号的缺省处理方法是退出进程
    //重载这个信号的处理方法,如果接收到一个SIGPIPE信号，忽略该信号
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    
    //sigemptyset()用来将参数set信号集初始化并清空
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
 函 数 名  : vos_initsockcommumd
 功能描述  : socket通信模块初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
	//创建套接字
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
	serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    serv_addr.sin_port = htons(GetServerPort());  //端口

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

	//---创建accept线程
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

