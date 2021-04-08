#ifndef __UART_H__
#define __UART_H__

#include <map>

#include"xc8002_middwnmachshare.h"
#include "vos_pid.h"
#include "app_comstructdefine.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
//#include <mutex>

using namespace std;

#define  UART_RES_TIME_LEN 320
#define  UART_RESEND_TIMES 3 
#define  UART_CMDINFOLEN_OF_NOPAYLOADMSG 4 

#define XC8002_UART_FMT_HEAD (0x550f)

//串口通信协议帧格式定义
typedef struct
{
    app_u16 ushead;      //帧头固定为0x550f
    app_u16 usdatalen;   //后面数据长度
    app_u16 uscpuid;     //下位机CPU号
    app_u16 uschksum;    //整个数据帧校验和
    char  aucdata[0];
}xc8002_uart_prot_data_format_stru;

typedef struct
{
    app_u16 cmdType;
    char  rsv[2];
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;//uscmdtype（包含）以后的长度
    vos_u32 ulcurcmdframeid;
    app_u16 uscmdtype;
    char  aucrsv[2];
    char aucdata[0];
}st_CanFormat;

#define xc8002_calcprotldlenbydtlen(datalen)  ((vos_u32)((datalen) + sizeof(xc8002_uart_prot_data_format_stru) - 4))
#define xc8002_calcprotpcklenbyldlen(loadlen) ((vos_u32)((loadlen) + 4))
#define xc8002_calcprotdtlenbyldlen(loadlen)  ((vos_u32)((loadlen) + 4 - sizeof(xc8002_uart_prot_data_format_stru)))
#define xc8002_cmdkey_mapto_cpuid(cmd) ((app_u16)(((cmd)&0xff00)>>10))

//定义测试状态
typedef enum{
	UART_IDLE        =0,   //空闲
    UART_IS_BUSY     =1,   //正在处理命令

}uartStateEunm;

//定义环回测试消息负载结构
typedef struct
{
    xc8002_middwnmach_msg_header;
    xc8002_loopback_dwnmach_cmd_stru stloopbackinfo;
}app_loopback_test_vos_msgload_stru;

//定义串口管理模块与中位机下位机命令交互中心的VOS消息结构
typedef struct
{
    vos_msg_header;
    xc8002_middwnmach_msg_header;
    app_u16 uscmdtype;
    char  aucrsv[2];
    char aucinfo[0];
}app_mnguart_middwnmach_cmd_msg_stru;

//下位机状态上报vos消息结构
typedef struct
{
    vos_msg_header;
    xc8002_com_report_cmd_stru strepinfo;
}app_dwnmach_rep_msg_stru;


typedef struct
{
    xc8002_middwnmach_msg_header;
    vos_u32  auccmdbuf[0];
}app_msg_payload_stru;

//单个下位机命令缓存
typedef struct
{
    vos_msg_header;
	vos_u32 usCmdNum;
	vos_u32 usTimerId;
	vos_u32 runState;
    xc8002_middwnmach_msg_header;
    app_u16 uscmdtype;
    char  auccmdbuf[APP_DWNMACH_CMDBUF_TOTAL_LEN];
}app_single_dwnmach_cmdbuf_stru;

class CUart : public CCallBack
{
	typedef vos_u32(CUart::*MsgFuc)(vos_msg_header_stru* pMsg);

public:
    CUart(vos_u32 ulPid);
    ~CUart();

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
	//初始化
	void _initMap();

	//处理下位机动作请求
    vos_u32 _procDwnActReq(vos_msg_header_stru* pReqMsg);
	vos_u32 _sendToDwn(xc8002_middwnmach_com_msgtran_stru * pMsg);

	//处理下位机动作响应
    vos_u32 _procDwnActRes(vos_msg_header_stru* pResMsg);
    vos_u32 _procFpgaAdRes(ADValue_st* pstrepcmd, char dwnName);
    void _printAD(ADValue_st* pstrepcmd, char dwnName);
    
    vos_u32 _procDwnmachCmdReport(xc8002_middwnmach_res_msgtran_stru* pstcmdres);
	vos_u32 _procDwnmachCmdResponse(xc8002_middwnmach_res_msgtran_stru* pstcmdres);
	vos_u32 _reportExecmdResult(vos_u32 CmdExeRslt,vos_u32 CmdInfoLen,vos_u32 DwnMachName,char* pPayLoad);
	vos_u32 _reportLoopTestResult(xc8002_middwnmach_com_msgtran_stru* pstresinfo);

	//处理超时
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);
	vos_u32 _getFrameId(vos_u32 endwnmachname);
	vos_u32 _calcChecksumForPacket(xc8002_uart_prot_data_format_stru* pstpack);
    vos_u32 _noticePidReadAd(vos_u32 dstpid, ADValue_st* pstrepcmd, char dwnName);
	void _sendRepmsgToStatusrep(xc8002_com_report_cmd_stru* pstrepcmd);
    vos_u32 _sendTestADVal(vos_u32 adNum, vos_u32*adVal, vos_u32 dwnName);
	void _procLiquidFlag(xc8002_middwnmach_res_msgtran_stru *pstcmdres);
private:
	map<app_u16, char*> m_dwnMsgBuf;
	map<app_u16, MsgFuc> m_msgFucs;
	map<vos_u32, char> m_timerDwnNameMap;


    int m_canSocket;
    struct sockaddr_can m_stSockAddr;
    void CanWrite(char* pData, app_u32 len, app_u32 id);
    void _initCan();
    char _getSerial(app_u32 len);
};
vos_u32 app_setdwnmachmask(vos_u32 dwnMachName, vos_u32 maskState);

void * _canRecvThreadEnter(void *para);

#define FORMAT_LEN (3072)
class subCan
{
public:
    subCan();
    subCan(app_u32 len);
    ~subCan();

public:
    void AnalysisData(can_frame *pFrame);

private:
    void _initialize(app_u32 len);
    void _saveResInfo(can_frame *pFrame);
    app_u32 _upToCan();


private:
    char *m_rcvBuffer;
    app_u32 m_bufferIndex;
    //std::mutex CANMutex;
};
#endif
