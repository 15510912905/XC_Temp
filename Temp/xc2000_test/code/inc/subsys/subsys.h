#ifndef __SUBSYS_H__
#define __SUBSYS_H__

#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include <queue>
#include "vos_pid.h"
#include "typedefine.pb.h"

using namespace std;

//创建执行动作状态机数据结构
typedef struct
{
    vos_u32 ulSrcIp;            //命令源IP
    vos_u32 ulSrcPid;          //命令源PID
    app_u16 usResMsgType;       //响应消息类型
}StMsgHead;

//注册子动作数据结构
typedef struct
{
    msg_stActHead stActHead;
    app_u16 usRsltInfoLen;
    char  aucActRsltInfo[APP_MAX_CMD_RSLT_INFO_LEN];
    vos_u32 ulCmdInfoLen;    //命令信息stcmdinfo的长度
    char stDwnCmd[0];
}STActInfo;

class CSubSys : public CCallBack
{
protected:
    typedef vos_u32(CSubSys::*ParseAct)(msg_stComDevActInfo* pComAct);
public:
    CSubSys(vos_u32 ulPid, vos_u32 ulDstSubsys);
    virtual ~CSubSys();

public:   
    //回调函数
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

    //处理请求
    vos_u32 ProcReqMsg(vos_msg_header_stru* pMsg);

	//下位机查询
    vos_u32 ParseDwnQuery(msg_stComDevActInfo* pComAct);
    
    //仪器切换调试状态
    vos_u32 ParseMachineDBG(msg_stComDevActInfo* pComAct);
	
	//下位机故障恢复
	vos_u32 ParseDwnFault(msg_stComDevActInfo* pComAct);

	//中位机状态下发给下位机
	vos_u32 ParseMidState(msg_stComDevActInfo* pComAct);
	
    //清除下位机日志
    vos_u32 ParseBBCLEAR(msg_stComDevActInfo* pComAct);

    vos_u32 ParseSingleCMD(msg_stComDevActInfo* pComAct);

    vos_u32 ParseNdlReset(msg_stComDevActInfo* pComAct);
protected:
    //初始化执行动作
    STActInfo * InitActInfo(msg_stActHead& stActHead, vos_u32 cmdLen);

    //解析微步调整
    vos_u32 ParseTrimOffset(msg_stComDevActInfo* pComAct, vos_u32 cmd);

    //设置补偿参数
    vos_u32 ParseSaveOffset(msg_stComDevActInfo* pComAct);

    //升级下位机
    vos_u32 ParseUpdateDwn(msg_stComDevActInfo* pComAct);

    //下位机电机
    vos_u32 ParseWriteMotor(msg_stComDevActInfo* pComAct);
	
	//升级FPGA
    vos_u32 ParseUpFpga(msg_stComDevActInfo* pComAct, vos_u32 cmd);

private:
    //解析执行动作
    vos_u32 ParseActInfo(vos_msg_header_stru* pMsg);

    //缓存消息
    vos_u32 _pushMsgToBuf(vos_msg_header_stru* pMsg);

    //取出缓存消息
    char* _popMsgFromBuf();

    //执行请求消息
    vos_u32 _procReqMsg();

    //发送命令到命令中心
    vos_u32 _sendCmdToDwnmach();

    //发送命令动作
    vos_u32 _sendCmdAct(STActInfo * pActInfo);

    //上报响应信息
    vos_u32 _reportRes(STActInfo* pActInfo, vos_u32 ulResult);

    //初始化数据
    void _initialize();

    //处理命令中心响应
    vos_u32 _procResMsg(vos_msg_header_stru* pMsg);

    //清除当前消息
    void _clearCurAct();

    //解析消息头
    void _parseMsgHead(vos_msg_header_stru* pMsg);

    //清除消息缓存
    void _clearMsgBuf();

    bool _checkAct(app_u32 subSys, app_u32 actType);
    void _setAct(app_u32 subSys, app_u32 actType, bool bAct);
protected:
    map<app_u16, ParseAct>  m_parseFucs;     //解析动作函数集合
    map<app_u16, app_u16> m_mActCMD;

private:
    vos_u32 m_ulDstSubsys;  //目的子系统
    vos_u32 m_ulBusy; //是否正在处理消息
    vos_u32 m_ulTimer;     //定时器

    StMsgHead m_stHead; //正在处理的消息头
    queue<char *> m_stCurAct; //正在处理的消息队列
    queue<char *> m_stMsgBuf;  //消息缓存

    static bool m_bReactDisk;
    static bool m_bAutoWash;
    static bool m_bRmix;
    static bool m_bSMix;
};

#endif
