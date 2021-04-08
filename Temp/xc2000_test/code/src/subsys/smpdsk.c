#include "smpdsk.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"

CSmpDsk::CSmpDsk(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}

CSmpDsk::~CSmpDsk()
{
}
//初始化解析函数
void CSmpDsk::_initParseFuc()
{
#ifdef Version_A
    m_mActCMD[Act_Reset] = CMD_ResetShelf;
    m_mActCMD[Act_GetShelf] = CMD_GetShelf;
    m_mActCMD[Act_FreeShelf] = CMD_FreeShelf;
	m_parseFucs[Act_GetShelf] = &CSubSys::ParseSingleCMD;
    m_parseFucs[Act_FreeShelf] = &CSubSys::ParseSingleCMD;
#else
    m_mActCMD[Act_Reset] = CMD_ORBIT_RESET;
    m_parseFucs[Act_GetIn] = (ParseAct)&CSmpDsk::_parseGetIn; 
    m_parseFucs[Act_FreeShelf] = (ParseAct)&CSmpDsk::_parseFreeShelf;
    m_parseFucs[Act_FreeShelfIn] = (ParseAct)&CSmpDsk::_parseFreeShelfIn;
    m_parseFucs[Act_GetShelf] = (ParseAct)&CSmpDsk::_parseGetShelf;
#endif
    m_parseFucs[Act_Reset] = &CSubSys::ParseSingleCMD;
    m_parseFucs[Act_Move] = (ParseAct)&CSmpDsk::_parseMove;
    m_parseFucs[Act_MoveInner] = (ParseAct)&CSmpDsk::_parseMove;
    m_parseFucs[Act_MoveOuter] = (ParseAct)&CSmpDsk::_parseMove;
	
    m_parseFucs[Act_BcScan] = (ParseAct)&CSmpDsk::_parseBcScan;
	m_parseFucs[Act_TrimOffset] = (ParseAct)&CSmpDsk::_parseTrimOffset;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
    m_parseFucs[Act_DskHrotate] = (ParseAct)&CSmpDsk::_parseDskHrotate;
	m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
	m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
    m_parseFucs[Act_SmpLightCtrl] = (ParseAct)&CSmpDsk::_parseLightCtrl;
	m_parseFucs[Act_Showave] = (ParseAct)&CSmpDsk::ParseShowave;
    m_parseFucs[Act_UpdateFpga] = (ParseAct)&CSmpDsk::_parseUpFpga;
}

vos_u32 CSmpDsk::_parseMove(msg_stComDevActInfo* pComAct)
{
#ifdef Version_A
    vos_u32 cmdLen = sizeof(CMD_SHELF_LOCA_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_paraSampleDskRtt * pPara = (msg_paraSampleDskRtt *)pComAct->aucPara;
    CMD_SHELF_LOCA_TYPE * pDwnCmd = (CMD_SHELF_LOCA_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_MoveShelf;
    pDwnCmd->currloca = pPara->usCircleId;
    pDwnCmd->destloca = 1;
    return VOS_OK;
#else
    vos_u32 cmdLen = sizeof(CMD_LOCATING_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_paraSampleDskRtt * pPara = (msg_paraSampleDskRtt *)pComAct->aucPara;
    CMD_LOCATING_TYPE * pDwnCmd = (CMD_LOCATING_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_LOCATING;
    pDwnCmd->currloca = pPara->usSmpCupId;
    pDwnCmd->destloca = 1;
    pDwnCmd->channel = 1;// pPara->channel;
    return VOS_OK;
#endif
}

vos_u32 CSmpDsk::_parseBcScan(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stSmpdskRttToBcscan);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

	msg_stSmpBcScanPara * pBcScan = (msg_stSmpBcScanPara *)(pComAct->aucPara);
    msg_stSmpdskRttToBcscan * pDwnCmd = (msg_stSmpdskRttToBcscan *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = cmd_sample_dsk_rtt_to_bcdscan;
	pDwnCmd->uccircleid = pBcScan->uccircleid;
	pDwnCmd->uccupid = pBcScan->uccupid;
    return VOS_OK;
}

vos_u32 CSmpDsk::_parseTrimOffset(msg_stComDevActInfo* pComAct)
{
    return ParseTrimOffset(pComAct,cmd_sample_dsk_step_move); 
}

vos_u32 CSmpDsk::_parseDskHrotate(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_cmdSampleDskRtt);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stDskHrotate * pPara = (msg_stDskHrotate *)pComAct->aucPara;
    msg_cmdSampleDskRtt * pDwnCmd = (msg_cmdSampleDskRtt *)&(pActInfo->stDwnCmd);
    pDwnCmd->uccircleid = pPara->dskId;
    pDwnCmd->uccupid = pPara->num;
    if (cmd_motor_dir_ANTICLOCKWISE == pPara->dir)
    {
        pDwnCmd->cmd = cmd_sample_dsk_anticlockwise_rtt; 
    }
    else if (cmd_motor_dir_CLOCKWISE == pPara->dir)
    {
        pDwnCmd->cmd = cmd_sample_dsk_clockwise_rtt; 
    }
    
    return VOS_OK;
}

vos_u32 CSmpDsk::_parseLightCtrl(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_sample_lightCtl);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    cmd_sample_lightCtl* pCmd = (cmd_sample_lightCtl*)pActInfo->stDwnCmd;
    msg_sample_lightCtl* pPara = (msg_sample_lightCtl*)pComAct->aucPara;
    pCmd->cmd = cmd_sample_dsk_lightCtl;
    pCmd->ctl = pPara->ctl;

    return VOS_OK;
}

vos_u32 CSmpDsk::ParseShowave(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stShowave);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    
    msg_stShowave* pPara = (msg_stShowave*)pComAct->aucPara;
    cmd_stShowave* pCmd = (cmd_stShowave*)pActInfo->stDwnCmd;
    pCmd->cmd = cmd_sample_dsk_show_wave_switch;
    pCmd->channel = pPara->channel;
	pCmd->switch_state = pPara->switch_state;

    return VOS_OK;
}

vos_u32 CSmpDsk::_parseUpFpga(msg_stComDevActInfo* pComAct)
{
    return ParseUpFpga(pComAct, cmd_sample_dsk_fpga_update);
}

#ifdef Version_A
vos_u32 CSmpDsk::_parseGetShelf(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stOneCmd);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stOneCmd * pDwnCmd = (msg_stOneCmd *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = cmd_sample_dsk_reset;
    return VOS_OK;
}

vos_u32 CSmpDsk::_parseFreeShelf(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stOneCmd);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stOneCmd * pDwnCmd = (msg_stOneCmd *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = cmd_sample_dsk_reset;
    return VOS_OK;
}
#else
vos_u32 CSmpDsk::_parseGetIn(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_GET_IN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stGetin* pPara = (act_stGetin*)pComAct->aucPara;
    CMD_GET_IN_TYPE * pDwnCmd = (CMD_GET_IN_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_GET_IN;
    pDwnCmd->channel = pPara->channel;
    return VOS_OK;
}

vos_u32 CSmpDsk::_parseFreeShelf(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_FREE_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stGetin* pPara = (act_stGetin*)pComAct->aucPara;
    CMD_FREE_TYPE * pDwnCmd = (CMD_FREE_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_FREE;
    pDwnCmd->channel = pPara->channel;
    return VOS_OK;
}

vos_u32 CSmpDsk::_parseFreeShelfIn(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_FREE_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stGetin* pPara = (act_stGetin*)pComAct->aucPara;
    CMD_FREE_TYPE * pDwnCmd = (CMD_FREE_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_FREEIN;
    pDwnCmd->channel = pPara->channel;
    return VOS_OK;
}

vos_u32 CSmpDsk::_parseGetShelf(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_GET_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stGetin* pPara = (act_stGetin*)pComAct->aucPara;
    CMD_GET_TYPE * pDwnCmd = (CMD_GET_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_GET;
    pDwnCmd->channel = pPara->channel;
    pDwnCmd->place = pPara->place;
    return VOS_OK;
}
#endif