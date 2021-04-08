#include "socketmonitor.h"
#include "apputils.h"
#include "vos_log.h"
#include "app_msg.h"
#include "vos_socketcommu.h"
#include "vos_errorcode.h"
#include <fcntl.h>
#include<google/protobuf/text_format.h>
#include<google/protobuf/io/zero_copy_stream_impl.h> 
#include "drv_ebi.h"
#include "alarm.h"

#define  HelloTimeLen 150
#define  MsgTimeLen  50
#define  LIGHT_TIME_LEN 18000

extern bool sleepFlag;
CSocketMonitor * g_pMonitor = NULL;

vos_u32 CSocketMonitor::m_ulState = vos_midmachine_state_standby;

CSocketMonitor::CSocketMonitor(vos_u32 ulPid) : CCallBack(ulPid), m_ulPort(VOS_SERVER_SOCKET_PORT_ID),m_bConnect(false),m_pMutexConn(NULL)
{
    m_pMutexConn = vos_mallocmutex();
    (void)vos_createmutex(m_pMutexConn);
    m_ulHelloTimer = createTimer(SPid_SocketMonitor, "HelloTimer");
	m_ulLightTimer = createTimer(SPid_SocketMonitor, "LightTimer",LIGHT_TIME_LEN);
    m_ulLiquidTimer = createTimer(SPid_SocketMonitor, "LiquidTimer", 3600);
	m_ulTempTimer = createTimer(SPid_SocketMonitor, "TempTimer", 3000);
    m_ulCollideNdl = collide_cancel;
	m_lightState = STATE_OFF;
	m_ulLiquidState = STATE_ON;
	m_TempState = STATE_ON;
}

CSocketMonitor::~CSocketMonitor()
{
	
}

vos_u32 CSocketMonitor::CallBack(vos_msg_header_stru* pMsg)
{
    if (MSG_NotifyConnect == pMsg->usmsgtype)
    {
        vos_obtainmutex(m_pMutexConn);
        m_bConnect = true;
        vos_releasemutex(m_pMutexConn);
        _reportState(m_ulState);
        sendInnerMsg(SPid_TaskResult, SPid_SocketMonitor, MSG_NotifyReportResult);
        vos_stoptimer(m_ulHelloTimer);
	
        return _startTimer(m_ulHelloTimer, HelloTimeLen);
    }
    if (MSG_ReqSetTime == pMsg->usmsgtype)
    {
        return _procSetTime(pMsg);
    }
    if (MSG_ReqHello == pMsg->usmsgtype)
    {
        _startTimer(m_ulHelloTimer, HelloTimeLen);
		return _sayHello();
    }
	if (MSG_NotifyMachineStatus == pMsg->usmsgtype)
	{
		return _procNotifyState(pMsg);
	}
    if (vos_pidmsgtype_timer_timeout == pMsg->usmsgtype)
    {
        return _procTimeOut((vos_timeroutmsg_stru*)pMsg);
    }
    if (MSG_CollideNdlConfig == pMsg->usmsgtype)
    {
        return _procCollideNdlConfig(pMsg);
    }
    if (MSG_SetSleep == pMsg->usmsgtype)
    {
        CMsg stMsg;
        stMsg.ParseMsg(pMsg);
        STSwitch*  pReq = dynamic_cast<STSwitch*>(stMsg.pPara);
        sleepFlag = (pReq->uiswitch()) ? true : false;
    }
    return VOS_OK;
}

vos_u32 CSocketMonitor::_startTimer(vos_u32 ulTimer, vos_u32 ulTimeLen)
{
    vos_u32 ulResult = vos_settimertimelen(ulTimer, ulTimeLen);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("vos_settimertimelen failed(%lx)\n", ulResult);
        return ulResult;
    }
    return vos_starttimer(ulTimer);
}

vos_u32 CSocketMonitor::_sayHello()
{
    STConsum msg;
    msg.set_type(Csum_Ligth);
    sendUpMsg(CPid_Consum, SPid_SocketMonitor, MSG_Consume, &msg);

    return sendUpMsg(CPid_SocketMonitor, SPid_SocketMonitor, MSG_ResHello);
}

vos_u32 CSocketMonitor::_procTimeOut(vos_timeroutmsg_stru* pMsg)
{
    if (m_ulHelloTimer == pMsg->ultimerid)
    {
		printf("  time out disconnect\n");
		return   Disconnect(ConnTimeLen, m_ulPort);
    }
	if (m_ulLightTimer == pMsg->ultimerid)
    {
        return _restLight();
    }
    if (m_ulLiquidTimer == pMsg->ultimerid)
	{
        vos_stoptimer(m_ulLiquidTimer);
		_sleepPump();
	}
	if (m_ulTempTimer == pMsg->ultimerid)
	{
		vos_stoptimer(m_ulTempTimer);
		_sleepTemp();
	}
    return VOS_OK;
}
vos_u32 CSocketMonitor::Disconnect(vos_u32 connTimeLen, vos_u32 ulPort)
{
    vos_obtainmutex(m_pMutexConn);
    if (m_bConnect)
    {
        m_bConnect = false;
        vos_releasemutex(m_pMutexConn);
        m_ulPort = ulPort;
        vos_disconnectsocket();
        return true;
    }
    vos_releasemutex(m_pMutexConn);
    return VOS_OK;
}

vos_u32 CSocketMonitor::_procSetTime(vos_msg_header_stru* pstvosmsg)
{
    CMsg stMsg;
    if (stMsg.ParseMsg(pstvosmsg))
    {
        STReqSetTime*  pSetTime = dynamic_cast<STReqSetTime*>(stMsg.pPara);
        char ustimestring[128] = { 0 };

        sprintf((char*)ustimestring, "date -s '%s'", pSetTime->strtime().c_str());
        printf("\r Upmachine time is %s!\r\n", ustimestring);

        if (VOS_OK != system((char *)ustimestring))
        {
            printf("Error system call: %s\r\n", (char *)ustimestring);
        }
#ifdef COMPILER_IS_ARM_LINUX_GCC
        if (VOS_OK != system("hwclock -w"))   //使硬件时间读取系统时间，实现同步
        {
            printf("Error system call hwclock -w\r\n");
        }
#endif
        _backLog();
    }
    g_pAlarm->SynAlarm();
    return VOS_OK;
}

vos_u32 CSocketMonitor::_controlLight(vos_u32 intensity)
{
    return 0;
    msg_stLightControl stPara;
    stPara.lightIntensity = intensity;
    return app_reqDwmCmd(SPid_SocketMonitor, SPid_ReactDsk, Act_Light, sizeof(stPara), (char*)&stPara, APP_NO);
// 	vos_u32 ulrunrslt = VOS_OK;
// 	vos_u32 ulmsglength = 0;
//     msg_stHead rHead;
// 
// 	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
//     msg_stLightControl *pdstrst = VOS_NULL;
// 
// 	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(msg_stLightControl);   
// 
// 	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
// 	if (VOS_OK != ulrunrslt)
//     {
//         WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
//         return ulrunrslt;
//     }
// 
// 	app_constructMsgHead(&rHead, SPid_ReactDsk, SPid_SocketMonitor, app_com_device_req_exe_act_msg_type);
// 	//填数据内容
//     pstdwnmachreqmsg->usActNum = 1;
//     pstdwnmachreqmsg->stActHead.usActType = Act_Light;
// 	pstdwnmachreqmsg->stActHead.usSingleRes = APP_NO;
//     pstdwnmachreqmsg->ulDataLen = sizeof(msg_stLightControl) + sizeof(msg_stActHead) ;
// 
// 	pdstrst = (msg_stLightControl*)(&(pstdwnmachreqmsg->astActInfo[0]));
// 	pdstrst->lightIntensity = intensity;
//     
//     return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);
}

vos_u32 CSocketMonitor::_openLight()
{
	if(m_lightState == STATE_OFF)
	{
		m_lightState = STATE_ON;
		_controlLight(LIGHTINTENSITY_ON);
	}

	return VOS_OK;
}

vos_u32 CSocketMonitor::_restLight()
{
	vos_stoptimer(m_ulLightTimer);
	m_lightState = STATE_OFF;
	_controlLight(LIGHTINTENSITY_REST);

	return VOS_OK;
}

void CSocketMonitor::_sleepPump()
{
    if (m_ulLiquidState == STATE_ON && sleepFlag)
    {
		WRITE_INFO_LOG("sleep Pump!\n");
		printf("sleep Pump!\n");
        m_ulLiquidState = STATE_OFF;
        msg_switch msgSwitch;
        msgSwitch.operation = 0;
        //app_reqDwmCmd(SPid_SocketMonitor, SPid_AutoWash, Act_sleepCtl, sizeof(msgSwitch), (char*)&msgSwitch, APP_NO);
    }
}

void CSocketMonitor::_sleepTemp()
{
	if (m_TempState == STATE_ON && sleepFlag)
	{
		WRITE_INFO_LOG("sleep Temp!\n");
		printf("sleep Temp!\n");
		m_TempState = STATE_OFF;
		tempSleepStausStr tempSwitch;
		tempSwitch.flag = 0;
		//app_reqDwmCmd(SPid_SocketMonitor, SPid_TmpCtrl, Act_sleepCtl, sizeof(tempSwitch), (char*)&tempSwitch, APP_NO);
	}
}

void CSocketMonitor::_recoverPumpTemp()
{
    if (m_ulLiquidState == STATE_OFF && sleepFlag)
	{
		WRITE_INFO_LOG("recover Pump!\n");
		printf("recover Pump!\n");
        m_ulLiquidState = STATE_ON;
        msg_switch msgSwitch;
        msgSwitch.operation = 1;
        //app_reqDwmCmd(SPid_SocketMonitor, SPid_AutoWash, Act_sleepCtl, sizeof(msgSwitch), (char*)&msgSwitch, APP_NO);
	}

	if (m_TempState == STATE_OFF && sleepFlag)
	{
		WRITE_INFO_LOG("recover Temp!\n");
		printf("recover Temp!\n");
		m_TempState = STATE_ON;
		tempSleepStausStr tempSwitch;
		tempSwitch.flag = 1;
		//app_reqDwmCmd(SPid_SocketMonitor, SPid_TmpCtrl, Act_sleepCtl, sizeof(tempSwitch), (char*)&tempSwitch, APP_NO);
	}
}

vos_u32 CSocketMonitor::_ctlByState(vos_u32 m_ulState)
{	
	if(m_ulState == vos_midmachine_state_standby || m_ulState >= vos_midmachine_state_reseted_err)
	{
		vos_starttimer(m_ulLightTimer);
        vos_starttimer(m_ulLiquidTimer);
		vos_starttimer(m_ulTempTimer);
	}
	else if(m_ulState == vos_midmachine_state_darkcurrent)
	{
		vos_stoptimer(m_ulLightTimer);
	}
	else
	{
		vos_stoptimer(m_ulLightTimer);
        vos_stoptimer(m_ulLiquidTimer);
		vos_stoptimer(m_ulTempTimer);
		_openLight();
        _recoverPumpTemp();
	}

	return VOS_OK;
}
vos_u32 CSocketMonitor::_procNotifyState(vos_msg_header_stru* pstvosmsg)
{
    CMsg stMsg;
    if (stMsg.ParseMsg(pstvosmsg))
    {
        STNotifyMachineStatus *pPara = (STNotifyMachineStatus*)(stMsg.pPara);
        m_ulState = pPara->uimachstatus();
		_ctlByState(m_ulState);
        return _reportState(m_ulState, pPara->uilefttime());
    }
    return vos_para_invalid_err;
}

vos_u32 CSocketMonitor::_procCollideNdlConfig(vos_msg_header_stru* pstvosmsg)
{
    CMsg stMsg;
    if (stMsg.ParseMsg(pstvosmsg))
    {
        STCollideNdlConfig *pPara = (STCollideNdlConfig*)(stMsg.pPara);
        m_ulCollideNdl = pPara->ulconfig();
        return VOS_OK;
    }
    return vos_para_invalid_err;
}

vos_u32 CSocketMonitor::GetCollideNdlConfig()
{
    return m_ulCollideNdl;
}

vos_u32 getCurRunState()
{
	return CSocketMonitor::m_ulState;
}

vos_u32 CSocketMonitor::_reportState(vos_u32 ulState, vos_u32 ulLeftTime)
{
    if (IsConnect())
    {
        STNotifyMachineStatus stStatus;
        stStatus.set_uimachstatus(ulState);
        stStatus.set_uilefttime(ulLeftTime);

        return sendUpMsg(CPid_SocketMonitor, SPid_SocketMonitor, MSG_NotifyMachineStatus, &stStatus);
    }
    return VOS_OK;
}

bool CSocketMonitor::CanMoveReagDsk()
{
    return vos_midmachine_state_standby == m_ulState || vos_midmachine_state_pause == m_ulState;
}

bool CSocketMonitor::CanHandleTask()
{
    if (vos_midmachine_state_testing == m_ulState || vos_midmachine_state_pause == m_ulState || vos_midmachine_state_wait_pause == m_ulState)
    {
        return true;
    }
    return _isFree();
}

bool CSocketMonitor::_isFree()
{
    return  vos_midmachine_state_standby == m_ulState;
}

bool CSocketMonitor::CanReset()
{
    return _isFree() || m_ulState >= vos_midmachine_state_reseted_err;
}
bool CSocketMonitor::CanHandleCmd()
{
    return _isFree();
}

bool CSocketMonitor::CanBCScan()
{
    if (vos_midmachine_state_pause == m_ulState)
    {
        return true;
    }
    return _isFree();
}

vos_u32 CSocketMonitor::_getDiffDay(vos_u32  nTime1, vos_u32  nTime2)
{
    static int nDaySec = 3600 * 24;

    vos_u32 ulDiff = nTime2 - nTime1;
    if (nTime1 > nTime2)
    {
        ulDiff = nTime1 - nTime2;
    }
    
    return ulDiff / nDaySec;
}

void CSocketMonitor::_backLog(char* szTraceDir, CBackLog& rBackLog)
{
    //备份
    TFName szBack;
    sprintf(szBack, "tar zcvf %s/log/backlog.tar.gz %s/log", szTraceDir, szTraceDir);
    system(szBack);

    //删除center
    TFName szCenter;
    sprintf(szCenter, "rm -rf %s/log/center/*", szTraceDir);
    system(szCenter);

    //删除rcd
    TFName szRcd;
    sprintf(szRcd, "rm -rf %s/log/rcd/*", szTraceDir);
    system(szRcd);

    //记录备份时间
    FILE* fp = VOS_NULL;
    TFName szbackCfg;
    sprintf(szbackCfg, "%s/log/backlog.CFG", szTraceDir);
    fp = fopen(szbackCfg, "w+");

    if (fp == NULL)
    {
        createPath(szTraceDir);
        fp = fopen(szbackCfg, "w+");
    }
    if (NULL != fp)
    {
        fprintf(fp, "%s", rBackLog.DebugString().c_str());
        fclose(fp);
    }
}

void CSocketMonitor::_backLog()
{
    CBackLog rBackLog;
    time_t uCurTime = time(NULL);
    char* szTraceDir = GetExePath();

    TFName szBackCfg;
    sprintf(szBackCfg, "%s/log/backlog.CFG", szTraceDir);
    int infile = open(szBackCfg, O_RDONLY);

    if (infile < 0)
    {
        rBackLog.set_uilogbacktime((vos_u32)uCurTime);
        _backLog(szTraceDir, rBackLog);
    }
    else
    {
        google::protobuf::io::FileInputStream fileInput(infile);
        if (google::protobuf::TextFormat::Parse(&fileInput, &rBackLog))
        {
            vos_u32 uiBackTime = rBackLog.uilogbacktime();
            if (_getDiffDay(uiBackTime, uCurTime) > 2)
            {
                //删除原有备份
                TFName szTraceFile;
                sprintf(szTraceFile, "rm %s/log/backlog.tar.gz", szTraceDir);
                system(szTraceFile);
                rBackLog.set_uilogbacktime((vos_u32)uCurTime);
                _backLog(szTraceDir, rBackLog);
            }
        }
        close(infile);
    }
}

bool CSocketMonitor::IsConnect()
{
    bool bConn = false;
    vos_obtainmutex(m_pMutexConn);
    bConn = m_bConnect;
    vos_releasemutex(m_pMutexConn);
    return bConn;
}
