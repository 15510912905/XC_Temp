#include "tempctrl.h"
#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"

CTempCtrl::CTempCtrl(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}

CTempCtrl::~CTempCtrl()
{
}
//初始化解析函数
void CTempCtrl::_initParseFuc()
{
    m_parseFucs[Act_TempSwitch] = (ParseAct)&CTempCtrl::_parseTempSwitch;
    m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
    m_parseFucs[Act_TempQuery] = (ParseAct)&CTempCtrl::_parseTempQuery;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
    m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
    m_parseFucs[Act_sleepCtl] = (ParseAct)&CTempCtrl::_preseTempSleep;
    m_parseFucs[Act_SetWTemp] = (ParseAct)&CTempCtrl::_parseSetWTemp;
    m_parseFucs[Act_SetRTemp] = (ParseAct)&CTempCtrl::_parseSetRTemp;
    m_parseFucs[Act_TempControl] = (ParseAct)&CTempCtrl::_parseTempControl;
}

vos_u32 CTempCtrl::_parseTempSwitch(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stTempSwitch);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

	msg_stTempSwitch * pPara = (msg_stTempSwitch *)pComAct->aucPara;
    cmd_stTempSwitch * pCmd = (cmd_stTempSwitch *)&(pActInfo->stDwnCmd);
    pCmd->cmd = cmd_temp_control_switch;
    pCmd->switchState = pPara->switchState;

    return VOS_OK;
}


vos_u32 CTempCtrl::_parseTempQuery(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_READ_TEMP_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    
    CMD_READ_TEMP_TYPE * pCmd = (CMD_READ_TEMP_TYPE *)&(pActInfo->stDwnCmd);
    pCmd->cmd = CMD_READ_TEMPERATURE;
    return VOS_OK;
}

vos_u32 CTempCtrl::_preseTempSleep(msg_stComDevActInfo* pComAct)
{
	vos_u32 cmdLen = sizeof(tempSleepStausStr);
	STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
	if (VOS_NULL == pActInfo)
	{
		WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
		return vos_malloc_mem_err;
	}

	tempSleepStausStr * pPara = (tempSleepStausStr *)pComAct->aucPara;
	tempSleepStausStr * pCmd = (tempSleepStausStr *)&(pActInfo->stDwnCmd);
	pCmd->cmd = cmd_temp_sleep;
	pCmd->flag = pPara->flag;

	return VOS_OK;
}

vos_u32 CTempCtrl::_parseSetWTemp(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stWTemp);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stWTemp * pPara = (msg_stWTemp *)pComAct->aucPara;
    cmd_stWTemp * pCmd = (cmd_stWTemp *)&(pActInfo->stDwnCmd);
    pCmd->cmd = cmd_temp_control_set_temp;
    pCmd->temp1 = pPara->temp1;
    pCmd->temp2 = pPara->temp2;
    pCmd->temp3 = pPara->temp3;

    return VOS_OK;
}

vos_u32 CTempCtrl::_parseSetRTemp(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stRTem);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stRTemp * pPara = (msg_stRTemp *)pComAct->aucPara;
    cmd_stRTem * pCmd = (cmd_stRTem *)&(pActInfo->stDwnCmd);
    pCmd->cmd = cmd_temp_control_set_reacttemp;
    pCmd->delta = pPara->delta;
    pCmd->tempreact = pPara->tempreact;
    return VOS_OK;
}
vos_u32 CTempCtrl::_parseTempControl(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stTempSwitch);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stTempSwitch * pPara = (msg_stTempSwitch *)pComAct->aucPara;
    cmd_stTempSwitch * pCmd = (cmd_stTempSwitch *)&(pActInfo->stDwnCmd);
    pCmd->cmd = CMD_TEMPERATURE_SWITCH;
    pCmd->switchState = pPara->switchState;
    return VOS_OK;
}


