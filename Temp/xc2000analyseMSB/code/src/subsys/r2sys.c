#include "r2sys.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"

#define  INTENSIFY_WASH_QUANTITY  4000

CR2Sys::CR2Sys(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}

CR2Sys::~CR2Sys()
{
}

//初始化解析函数
void CR2Sys::_initParseFuc()
{

    m_parseFucs[Act_Reset] = &CSubSys::ParseNdlReset;
    m_parseFucs[Act_Move] = (ParseAct)&CR2Sys::_parseR2Rtt;
    m_parseFucs[Act_MoveAbs] = (ParseAct)&CR2Sys::_parseR2Rtt;
    m_parseFucs[Act_MoveW] = (ParseAct)&CR2Sys::_parseR2Rtt;
    m_parseFucs[Act_MoveDisg] = (ParseAct)&CR2Sys::_parseR2Rtt;
    m_parseFucs[Act_Absorb] = (ParseAct)&CR2Sys::_parseR2Absrb;
    m_parseFucs[Act_Discharge] = (ParseAct)&CR2Sys::_parseR2Discharge;
    m_parseFucs[Act_Wash] = (ParseAct)&CR2Sys::_parseR2Wash;
    m_parseFucs[Act_AirOut] = (ParseAct)&CR2Sys::_parseAirOut;
    m_parseFucs[Act_NdlVrt] = (ParseAct)&CR2Sys::_parseNdlVrt;
    m_parseFucs[Act_PVCtrl] = (ParseAct)&CR2Sys::_parsePVCtrl;
    m_parseFucs[Act_PumpCtrl] = (ParseAct)&CR2Sys::_parsePumpCtrl;

    m_parseFucs[Act_MarginScan] = (ParseAct)&CR2Sys::_parseR2MarginScan;
    m_parseFucs[Act_IntensifyWash] = (ParseAct)&CR2Sys::_parseIntensifyWash;
    m_parseFucs[Act_TrimOffset] = (ParseAct)&CR2Sys::_parseTrimOffset;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
	m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
	m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
}

vos_u32 CR2Sys::_parseR2Rtt(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_NDL_TURN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stNdlRtt * pPara = (msg_stNdlRtt *)pComAct->aucPara;
    CMD_NDL_TURN_TYPE * pDwnCmd = (CMD_NDL_TURN_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_NDL_TURN;
    pDwnCmd->place = _setPos(pPara->usDstPos);
    //_setSampleNdlRttPos((msg_stNdlHrotate*)pDwnCmd, pPara->usDstPos);
    return VOS_OK;
}

vos_u32 CR2Sys::_parseR2Absrb(msg_stComDevActInfo* pComAct)
{
    app_u16 cmdNum = 3;
    vos_u32 cmdLen = sizeof(msg_stComDwnmachCombCmd) + sizeof(msg_stComCombSingleCmd) * cmdNum
        + sizeof(CMD_NDL_DOWN_TYPE) + sizeof(CMD_NDL_SAMPLE_TYPE) + sizeof(CMD_NDL_UP_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = cmdNum;
    msg_stReagSysAbsorb *pPara = (msg_stReagSysAbsorb *)(pComAct->aucPara);

    //垂直下降
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_DOWN_TYPE);
    CMD_NDL_DOWN_TYPE* pDwnCmd0 = (CMD_NDL_DOWN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd0->cmd = CMD_NDL_DOWN;
    pDwnCmd0->en_reporting_margin = pPara->reportMarginFlag;
    pDwnCmd0->place_number = pPara->cupId;
    pDwnCmd0->ring_number = pPara->diskId;
    pDwnCmd0->cup_number = pPara->reactCup;
    pDwnCmd0->en_liquid_level_measuring = 1;

    //吸样
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_SAMPLE_TYPE);
    CMD_NDL_SAMPLE_TYPE* pDwnCmd1 = (CMD_NDL_SAMPLE_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd1->cmd = CMD_NDL_SAMPLE;
    pDwnCmd1->size = pPara->ulVolume;

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_UP_TYPE);
    CMD_NDL_UP_TYPE* pDwnCmd2 = (CMD_NDL_UP_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd2->cmd = CMD_NDL_UP;

    //     pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    //     pSingleCmd->ulCmdSize = sizeof(CMD_NDL_TURN_TYPE);
    //     CMD_NDL_TURN_TYPE* pDwnCmd3 = (CMD_NDL_TURN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    //     pDwnCmd3->cmd = CMD_NDL_TURN;
    //     pDwnCmd3->place = pPara->usDischPos;
    return VOS_OK;
}

vos_u32 CR2Sys::_parseR2Discharge(msg_stComDevActInfo* pComAct)
{
    app_u16 cmdNum = 4;
    vos_u32 cmdLen = sizeof(msg_stComDwnmachCombCmd) + sizeof(msg_stComCombSingleCmd) * cmdNum
        + sizeof(CMD_NDL_TURN_TYPE) + sizeof(CMD_NDL_DOWN_TYPE) + sizeof(CMD_NDL_BLOWOFF_TYPE) + sizeof(CMD_NDL_UP_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = cmdNum;
    msg_stReagSysDischarge *pPara = (msg_stReagSysDischarge *)(pComAct->aucPara);

    //运动到排样位
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_TURN_TYPE);
    CMD_NDL_TURN_TYPE* pDwnCmd0 = (CMD_NDL_TURN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd0->cmd = CMD_NDL_TURN;
    pDwnCmd0->place = _setPos(pPara->usDischPos);

    //垂直下降
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_DOWN_TYPE);
    CMD_NDL_DOWN_TYPE* pDwnCmd1 = (CMD_NDL_DOWN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd1->cmd = CMD_NDL_DOWN;
    pDwnCmd1->total_volume = pPara->usTotalVol;
    pDwnCmd1->en_reporting_margin = 0;
    pDwnCmd1->en_liquid_level_measuring = 0;
    /*    pDwnCmd1->place_number = (app_u16)(pPara->uldlntvolume);//加样称重传杯位号*/

    //排样
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_BLOWOFF_TYPE);
    CMD_NDL_BLOWOFF_TYPE* pDwnCmd2 = (CMD_NDL_BLOWOFF_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd2->cmd = CMD_NDL_BLOWOFF;
    pDwnCmd2->size = (app_u16)(pPara->ulReagVolume);

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_UP_TYPE);
    CMD_NDL_UP_TYPE* pDwnCmd3 = (CMD_NDL_UP_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd3->cmd = CMD_NDL_UP;
    return VOS_OK;
}

vos_u32 CR2Sys::_parseR2Wash(msg_stComDevActInfo* pComAct)
{
    app_u16 cmdNum = 2;
    vos_u32 cmdLen = sizeof(msg_stComDwnmachCombCmd) + sizeof(msg_stComCombSingleCmd) * cmdNum
        + sizeof(CMD_NDL_TURN_TYPE) + sizeof(CMD_NDL_WASH_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)&(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = cmdNum;

    //回清洗位
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_TURN_TYPE);
    CMD_NDL_TURN_TYPE* pDwnCmd0 = (CMD_NDL_TURN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd0->cmd = CMD_NDL_TURN;
    pDwnCmd0->place = _setPos(RNdlPosReset);

    //清洗
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_WASH_TYPE);
    CMD_NDL_WASH_TYPE* pDwnCmd1 = (CMD_NDL_WASH_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd1->cmd = CMD_NDL_WASH;
    pDwnCmd1->type = 0;
    pDwnCmd1->time = 0;

    return VOS_OK;
}

vos_u32 CR2Sys::_parseR2MarginScan(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stOneCmd);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stOneCmd* pCmd = (msg_stOneCmd*)&(pActInfo->stDwnCmd);
    pCmd->cmd = cmd_r2_reagent_ndl_marginscan;

    return VOS_OK;
}

vos_u32 CR2Sys::_parseIntensifyWash(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stComDwnmachCombCmd) + sizeof(msg_stComCombSingleCmd) * 6
        + sizeof(msg_stNdlHrotate) * 2 + sizeof(msg_stNdlAbsrb) + sizeof(msg_stOneCmd) * 3;
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)&(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = 6;

    //转动到吸清洗液位
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);
    pSingleCmd->ulCmdSize = sizeof(msg_stNdlHrotate);
    msg_stNdlHrotate* pRtt = (msg_stNdlHrotate*)(&(pSingleCmd->aucCmdLoad));
    pRtt->cmd = cmd_r2_reagent_ndl_hrotate_absbpos;
    pRtt->enPosition = cmd_r2_reagentndl_inner_absrb_pos;

    //垂直下降
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stOneCmd);
    msg_stOneCmd* pDown = (msg_stOneCmd*)(&(pSingleCmd->aucCmdLoad));
    pDown->cmd = cmd_r2_reagent_ndl_vdecline;

    //吸清洗剂
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stNdlAbsrb);
    msg_stNdlAbsrb* pAbsorb = (msg_stNdlAbsrb*)(&(pSingleCmd->aucCmdLoad));
    pAbsorb->cmd = cmd_r2_reagent_ndl_absorb_reagent;
    pAbsorb->quantity = INTENSIFY_WASH_QUANTITY;

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stOneCmd);
    msg_stOneCmd* pUp = (msg_stOneCmd*)(&(pSingleCmd->aucCmdLoad));
    pUp->cmd = cmd_r2_reagent_ndl_vrise;

    //回清洗位
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stNdlHrotate);
    msg_stNdlHrotate* pToWashPos = (msg_stNdlHrotate*)(&(pSingleCmd->aucCmdLoad));
    pToWashPos->cmd = cmd_r2_reagent_ndl_hrotate_washpos;

    //强化清洗
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stOneCmd);
    msg_stOneCmd* pIntensifyWash = (msg_stOneCmd*)(&(pSingleCmd->aucCmdLoad));
    pIntensifyWash->cmd = cmd_r2_ndl_intensify_wash;

    return VOS_OK;
}
vos_u32 CR2Sys::_parseAirOut(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_NDL_AIR_OUT_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stAirout *pAiroutInfo = (msg_stAirout *)(pComAct->aucPara);
    CMD_NDL_AIR_OUT_TYPE* pAirout = (CMD_NDL_AIR_OUT_TYPE*)&(pActInfo->stDwnCmd);
    pAirout->cmd = CMD_NDL_AIR_OUT;
    pAirout->number = pAiroutInfo->usAirOutTimes;
    return VOS_OK;
}

vos_u32 CR2Sys::_parseTrimOffset(msg_stComDevActInfo* pComAct)
{
    return ParseTrimOffset(pComAct,cmd_r2_ndl_step_move); 
}

vos_u32 CR2Sys::_parseNdlVrt(msg_stComDevActInfo* pComAct)
{
    msg_stVrt *pMsgNdlVrt = (msg_stVrt *)(pComAct->aucPara);
    vos_u32 cmdLen = (cmd_motor_dir_up == pMsgNdlVrt->dir) ? sizeof(CMD_NDL_UP_TYPE) : sizeof(CMD_NDL_DOWN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    if (cmd_motor_dir_up == pMsgNdlVrt->dir)
    {
        CMD_NDL_UP_TYPE* pCmdNdlVrtUp = (CMD_NDL_UP_TYPE*)&(pActInfo->stDwnCmd);
        pCmdNdlVrtUp->cmd = CMD_NDL_UP;
    }
    else if (cmd_motor_dir_down == pMsgNdlVrt->dir)
    {
        CMD_NDL_DOWN_TYPE* pCmdNdlVrtDwn = (CMD_NDL_DOWN_TYPE*)&(pActInfo->stDwnCmd);
        pCmdNdlVrtDwn->cmd = CMD_NDL_DOWN;
        pCmdNdlVrtDwn->total_volume = pMsgNdlVrt->totalVol;
        pCmdNdlVrtDwn->en_reporting_margin = 1;
        pCmdNdlVrtDwn->place_number = pMsgNdlVrt->cupId;
        pCmdNdlVrtDwn->ring_number = pMsgNdlVrt->diskId;
        pCmdNdlVrtDwn->cup_number = pMsgNdlVrt->reactCupId;
    }
    return VOS_OK;
}

vos_u32 CR2Sys::_parsePVCtrl(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_NDL_WASH_VALVE_CONTROL_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_PVCtrl* pPara = (msg_PVCtrl *)pComAct->aucPara;
    CMD_NDL_WASH_VALVE_CONTROL_TYPE * pDwnCmd = (CMD_NDL_WASH_VALVE_CONTROL_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_NDL_WASH_VALVE_CONTROL;
    pDwnCmd->number = pPara->PVNum;
    pDwnCmd->status = pPara->operateType;

    return VOS_OK;
}

vos_u32 CR2Sys::_parsePumpCtrl(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_r1_reagent_ndl_discharge_stru);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_PumpCtrl* pPara = (msg_PumpCtrl *)pComAct->aucPara;
    CMD_NDL_BLOWOFF_TYPE *pPumpOUT = NULL;
    CMD_NDL_SAMPLE_TYPE* pPumpIN = NULL;

    if (cmd_motor_dir_IN == pPara->dir)
    {
        pPumpIN = (CMD_NDL_SAMPLE_TYPE *)&(pActInfo->stDwnCmd);
        pPumpIN->cmd = CMD_NDL_SAMPLE;
        pPumpIN->size = pPara->volume;
    }
    else
    {
        pPumpOUT = (CMD_NDL_BLOWOFF_TYPE *)&(pActInfo->stDwnCmd);
        pPumpOUT->cmd = CMD_NDL_BLOWOFF;
        pPumpOUT->size = pPara->volume;;
    }
    return VOS_OK;
}

uint8_t CR2Sys::_setPos(app_u16 usDstPos)
{
    if (m_ulPid == SPid_R2)
    {
        return (usDstPos == RNdlPosDischarg) ? PLACE_R21RTT_DISCHARGE
            : (usDstPos == RNdlPosReset) ? PLACE_R21RTT_RESET
            : (usDstPos == RNdlPosW1) ? PLACE_R21RTT_WASH1
            : (usDstPos == RNdlPosW2) ? PLACE_R21RTT_WASH2
            : (usDstPos == RNdlPosAbsorb1) ? PLACE_R21RTT_INHALATION1
            : (usDstPos == RNdlPosAbsorb2) ? PLACE_R21RTT_INHALATION2 : 0;
    }
    return (usDstPos == RNdlPosDischarg) ? PLACE_R22RTT_DISCHARGE
        : (usDstPos == RNdlPosReset) ? PLACE_R22RTT_RESET
        : (usDstPos == RNdlPosW1) ? PLACE_R22RTT_WASH1
        : (usDstPos == RNdlPosW2) ? PLACE_R22RTT_WASH2
        : (usDstPos == RNdlPosAbsorb1) ? PLACE_R22RTT_INHALATION1
        : (usDstPos == RNdlPosAbsorb2) ? PLACE_R22RTT_INHALATION2 : 0;
}