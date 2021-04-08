#include "outerdsk.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"

COuterDsk::COuterDsk(vos_u32 ulPid) : CSubSys(ulPid, xc8002_dwnmach_name_R1disk)
{
    _initParseFuc();
}

COuterDsk::~COuterDsk()
{
}
//初始化解析函数
void COuterDsk::_initParseFuc()
{
    m_mActCMD[Act_Reset] = CMD_DSK_RESET;
    m_parseFucs[Act_Reset] = &CSubSys::ParseSingleCMD;
    m_parseFucs[Act_Move] = (ParseAct)&COuterDsk::_parseMove;
    m_parseFucs[Act_MoveOuter] = (ParseAct)&COuterDsk::_parseMove;
    m_parseFucs[Act_MoveInner] = (ParseAct)&COuterDsk::_parseMove;
    m_parseFucs[Act_DskHrotate] = (ParseAct)&COuterDsk::_parseDskHrotate;

    m_parseFucs[Act_BcScan] = (ParseAct)&COuterDsk::_parseBcScan;
    m_parseFucs[Act_TrimOffset] = (ParseAct)&COuterDsk::_parseTrimOffset;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
	m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
	m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
	m_parseFucs[Act_Showave] = (ParseAct)&COuterDsk::ParseShowave;
    m_parseFucs[Act_UpdateFpga] = (ParseAct)&COuterDsk::_parseUpFpga;
}

vos_u32 COuterDsk::_parseMove(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_DSK_TURN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stReagSysDskRtt * pDskRtt = (act_stReagSysDskRtt *)(pComAct->aucPara);

    CMD_DSK_TURN_TYPE* pCmd = (CMD_DSK_TURN_TYPE*)(pActInfo->stDwnCmd);
    pCmd->cmd = CMD_DSK_TURN;
    pCmd->type = 0;
    pCmd->cup_number = pDskRtt->usCupId;
    pCmd->place = pDskRtt->usDstPos;
    return VOS_OK;
}

vos_u32 COuterDsk::_parseBcScan(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stReagdskRttToBcscan);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

	msg_stReagBcScanPara * pBcScan = (msg_stReagBcScanPara *)(pComAct->aucPara);
    msg_stReagdskRttToBcscan * pDwnCmd = (msg_stReagdskRttToBcscan *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = cmd_outer_reagentdisk_rtt_to_bcdscan;
	pDwnCmd->uccupnum = pBcScan->usCupId;
    return VOS_OK;
}

vos_u32 COuterDsk::_parseTrimOffset(msg_stComDevActInfo* pComAct)
{
    return ParseTrimOffset(pComAct,cmd_outer_reagentdisk_step_move); 
}

vos_u32 COuterDsk::_parseDskHrotate(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_DSK_TURN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stDskHrotate * pPara = (msg_stDskHrotate *)pComAct->aucPara;
    CMD_DSK_TURN_TYPE* pCmd = (CMD_DSK_TURN_TYPE*)(pActInfo->stDwnCmd);
    pCmd->cmd = CMD_DSK_TURN;
    pCmd->type = (cmd_motor_dir_CLOCKWISE == pPara->dir) ? 1 : 2;
    pCmd->cup_number = pPara->num;
    return VOS_OK;
}

vos_u32 COuterDsk::ParseShowave(msg_stComDevActInfo* pComAct)
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
    pCmd->cmd = cmd_outer_reagentdisk_show_wave_switch;
    pCmd->channel = pPara->channel;
	pCmd->switch_state = pPara->switch_state;

    return VOS_OK;
}

vos_u32 COuterDsk::_parseUpFpga(msg_stComDevActInfo* pComAct)
{
    return ParseUpFpga(pComAct, cmd_outer_reagentdisk_fpga_update);
}

