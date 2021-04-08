#include "autowash.h"
#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include "typedefine.pb.h"
#include "upmsg.pb.h"

CAutoWash::CAutoWash(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}

CAutoWash::~CAutoWash()
{
}
//初始化解析函数
void CAutoWash::_initParseFuc()
{
    m_mActCMD[Act_Reset] = CMD_AUTOWASH_RESET;
    m_parseFucs[Act_Reset] = &CSubSys::ParseSingleCMD;
    m_parseFucs[Act_Wash] = (ParseAct)&CAutoWash::_parseAutoWash;
    m_parseFucs[Act_NdlVrt] = (ParseAct)&CAutoWash::_parseNdlVrt;
    m_parseFucs[Act_CtrlP04] = (ParseAct)&CAutoWash::_ParseCtrlP04;
    m_parseFucs[Act_PVCtrl] = (ParseAct)&CAutoWash::_parsePVCtrl;

    m_parseFucs[Act_TrimOffset] = (ParseAct)&CAutoWash::_parseTrimOffset;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
   // m_parseFucs[Act_Debug] = &CSubSys::ParseMachineDBG;
    m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
    m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_LiquidsCheck] = (ParseAct)&CAutoWash::_parseLiquidsCheck;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
    m_parseFucs[Act_LiquidDBG] = (ParseAct)&CAutoWash::_parseLiquidDBG;
    m_parseFucs[Act_SetFlState] = (ParseAct)&CAutoWash::_parseSetFlState;
	m_parseFucs[Act_sleepCtl] = (ParseAct)&CAutoWash::_parseSleep;
    m_parseFucs[Act_StateSwitch] = (ParseAct)&CAutoWash::_parseSwitch;
}

vos_u32 CAutoWash::_parseAutoWash(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_AUTOWASH_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stReactSysAutoWash * pAutoWash = (act_stReactSysAutoWash *)pComAct->aucPara;
    CMD_AUTOWASH_TYPE * pDwnCmd = (CMD_AUTOWASH_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_AUTOWASH;
    pDwnCmd->listOut = pAutoWash->usWashOutBitMap;
    pDwnCmd->listIn = pAutoWash->usWashInBitMap2;
    //_sendConsum(pDwnCmd);
    return VOS_OK;
}

void CAutoWash::_sendConsum(msg_stAutoWash * pDwnCmd)
{
    STConsum msg;
    msg.set_type(Csum_Wash);
    vos_u32 para = ((pDwnCmd->washBitMap & 0x180) == 0x180) ? 2
        : ((pDwnCmd->washBitMap & 0x180) == 0x100) ? 1
        : ((pDwnCmd->washBitMap & 0x180) == 0x80) ? 1
        : 0;
    msg.set_para(para);
    sendUpMsg(CPid_Consum, SPid_AutoWash, MSG_Consume, &msg);
}

vos_u32 CAutoWash::_parseTrimOffset(msg_stComDevActInfo* pComAct)
{
    return ParseTrimOffset(pComAct,cmd_auto_wash_step_move); 
}

vos_u32 CAutoWash::_parseNdlVrt(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_AUTOWASH_MOVE_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stVrt *pMsgNdlVrt = (msg_stVrt *)(pComAct->aucPara);
    CMD_AUTOWASH_MOVE_TYPE* pCmdNdlVrtUp = (CMD_AUTOWASH_MOVE_TYPE*)&(pActInfo->stDwnCmd);

    pCmdNdlVrtUp->cmd = CMD_AUTOWASH_MOVE;
    pCmdNdlVrtUp->place = pMsgNdlVrt->dir;
    return VOS_OK;
}

vos_u32 CAutoWash::_ParseCtrlP04(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_AUTOWASH_PVCONTROL_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stCtrlP04 * pPara = (msg_stCtrlP04 *)pComAct->aucPara;
    CMD_AUTOWASH_PVCONTROL_TYPE * pDwnCmd = (CMD_AUTOWASH_PVCONTROL_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_AUTOWASH_PVCONTROL;
    pDwnCmd->number = p04;
    pDwnCmd->status = pPara->operateType;

    return VOS_OK;
}

vos_u32 CAutoWash::_parsePVCtrl(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_AUTOWASH_PVCONTROL_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_PVCtrl* pPara = (msg_PVCtrl *)pComAct->aucPara;
    CMD_AUTOWASH_PVCONTROL_TYPE * pDwnCmd = (CMD_AUTOWASH_PVCONTROL_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_AUTOWASH_PVCONTROL;
    pDwnCmd->number = pPara->PVNum;
    pDwnCmd->status = pPara->operateType;
    return VOS_OK;
}

vos_u32 CAutoWash::_parseLiquidsCheck(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_switch);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_switch * pMsg = (msg_switch *)pComAct->aucPara;
    cmd_switch * pCmd = (cmd_switch *)&(pActInfo->stDwnCmd);
    pCmd->operation = pMsg->operation;
    pCmd->cmd = cmd_auto_wash_control;

    return VOS_OK;
}

vos_u32 CAutoWash::_parseLiquidDBG(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stOneCmd);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    msg_liquidDBG *pMsg = (msg_liquidDBG *)pComAct->aucPara;
    msg_stOneCmd * pCmd = (msg_stOneCmd *)&(pActInfo->stDwnCmd);
    switch (pMsg->debugType)
    {
    case washDBG_WR:
        pCmd->cmd = cmd_auto_wash_test_1;
        break;
    case washDBG_WW:
        pCmd->cmd = cmd_auto_wash_test_5;
        break;
    case washDBG_OMVA:
        pCmd->cmd = cmd_auto_wash_test_8;
        break;
    case washDBG_CMVA:
        pCmd->cmd = cmd_auto_wash_test_9;
        break;
    default:
        return VOS_INVALID_U32;
    }
    return VOS_OK;
}

vos_u32 CAutoWash::_parseSetFlState(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_PVCtrl);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    msg_PVCtrl *pMsg = (msg_PVCtrl *)pComAct->aucPara;
    cmd_PVCtrl * pCmd = (cmd_PVCtrl *)&(pActInfo->stDwnCmd);
    pCmd->cmd = cmd_auto_wash_setFLstae;
    pCmd->operateType = pMsg->operateType;
    pCmd->PVNum = pMsg->PVNum;

	return VOS_OK;
}
vos_u32 CAutoWash::_parseSleep(msg_stComDevActInfo* pComAct)
{
	vos_u32 cmdLen = sizeof(msg_switch);
	STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
	if (VOS_NULL == pActInfo)
	{
		WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
		return vos_malloc_mem_err;
	}
	msg_switch *pMsg = (msg_switch *)pComAct->aucPara;
	cmd_switch * pCmd = (cmd_switch *)&(pActInfo->stDwnCmd);
	pCmd->cmd = cmd_sleepCtl;
	pCmd->operation = pMsg->operation;

	return VOS_OK;
}
vos_u32 CAutoWash::_parseSwitch(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(act_state_switch);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    msg_stateSwitch *pMsg = (msg_stateSwitch *)pComAct->aucPara;
    act_state_switch * pCmd = (act_state_switch *)&(pActInfo->stDwnCmd);
    pCmd->cmd = cmd_atuo_wash_state_switch;
    pCmd->state = pMsg->state;

    return VOS_OK;
}