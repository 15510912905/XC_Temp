#include "smpndl.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include "app_downmsg.h"
#include "app_innermsg.h"

#define  INTENSIFY_WASH_QUANTITY  350

CSmpNdl::CSmpNdl(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}

CSmpNdl::~CSmpNdl()
{
}
//初始化解析函数
void CSmpNdl::_initParseFuc()
{
    m_parseFucs[Act_Reset] = &CSubSys::ParseNdlReset;
    m_parseFucs[Act_Move] = (ParseAct)&CSmpNdl::_parseNdlRtt;
    m_parseFucs[Act_MoveAbs] = (ParseAct)&CSmpNdl::_parseNdlRtt;
	m_parseFucs[Act_MoveDilu] = (ParseAct)&CSmpNdl::_parseNdlRtt;
    m_parseFucs[Act_MoveDisg] = (ParseAct)&CSmpNdl::_parseNdlRtt;
    m_parseFucs[Act_MoveW] = (ParseAct)&CSmpNdl::_parseNdlRtt;
    m_parseFucs[Act_Absorb] = (ParseAct)&CSmpNdl::_parseAbsrb;
	m_parseFucs[Act_AbsorbDilu] = (ParseAct)&CSmpNdl::_parseAbsrb;
    m_parseFucs[Act_Discharge] = (ParseAct)&CSmpNdl::_parseDischarge;
	m_parseFucs[Act_DischargeDilu] = (ParseAct)&CSmpNdl::_parseDischarge;
	m_parseFucs[Act_Wash] = (ParseAct)&CSmpNdl::_parseWashing;
    m_parseFucs[Act_washLong] = (ParseAct)&CSmpNdl::_parseWashLong;
    m_parseFucs[Act_PVCtrl] = (ParseAct)&CSmpNdl::_parsePVCtrl;
    m_parseFucs[Act_PumpCtrl] = (ParseAct)&CSmpNdl::_parsePumpCtrl;
    m_parseFucs[Act_AirOut] = (ParseAct)&CSmpNdl::_parseAirOut;
    m_parseFucs[Act_NdlVrt] = (ParseAct)&CSmpNdl::_parseNdlVrt;

    m_parseFucs[Act_IntensifyWash] = (ParseAct)&CSmpNdl::_parseIntensifyWash;
	m_parseFucs[Act_TrimOffset] = (ParseAct)&CSmpNdl::_parseTrimOffset;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
	m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
	m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
}

vos_u32 CSmpNdl::_parseNdlRtt(msg_stComDevActInfo* pComAct)
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

vos_u32 CSmpNdl::_parseAbsrb(msg_stComDevActInfo* pComAct)
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

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)&(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = cmdNum;
    msg_stSampleSysAbsorb * pPara = (msg_stSampleSysAbsorb *)pComAct->aucPara;

    //垂直下降
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_DOWN_TYPE);
    CMD_NDL_DOWN_TYPE* pDwnCmd0 = (CMD_NDL_DOWN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd0->total_volume = pPara->ulSampleVolume;
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
    pDwnCmd1->size = pPara->ulSampleVolume;

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_UP_TYPE);
    CMD_NDL_UP_TYPE* pDwnCmd2 = (CMD_NDL_UP_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd2->cmd = CMD_NDL_UP;
    // 
    //     //旋转
    //     pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    //     pSingleCmd->ulCmdSize = sizeof(CMD_NDL_TURN_TYPE);
    //     CMD_NDL_TURN_TYPE* pDwnCmd3 = (CMD_NDL_TURN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    //     pDwnCmd3->cmd = CMD_NDL_TURN;
    //     pDwnCmd3->place = pPara->usDischPos;
    return VOS_OK;
}

vos_u32 CSmpNdl::_parseDischarge(msg_stComDevActInfo* pComAct)
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

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)&(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = cmdNum;
    msg_stSampleSysDischarge * pPara = (msg_stSampleSysDischarge *)pComAct->aucPara;

    //运动到排样位
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_TURN_TYPE);
    CMD_NDL_TURN_TYPE* pDwnCmd0 = (CMD_NDL_TURN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd0->cmd = CMD_NDL_TURN;
    pDwnCmd0->place = _setPos(pPara->usDischPos);
    //_setSampleNdlRttPos((msg_stNdlHrotate*) pDwnCmd0, pPara->usdischpos);

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
    pDwnCmd2->size = (app_u16)(pPara->ulSampleVolume);

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_UP_TYPE);
    CMD_NDL_UP_TYPE* pDwnCmd3 = (CMD_NDL_UP_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd3->cmd = CMD_NDL_UP;
    // 
    //     //回到清洗位
    //     pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    //     pSingleCmd->ulCmdSize = sizeof(msg_stNdlHrotate);
    //     msg_stNdlHrotate* pDwnCmd4 = (msg_stNdlHrotate*)(&(pSingleCmd->aucCmdLoad));
    //     _setSampleNdlRttPos((msg_stNdlHrotate*)pDwnCmd4, SmpPosWash);

    return VOS_OK;
}



vos_u32 CSmpNdl::_parseWashing(msg_stComDevActInfo* pComAct)
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
    pDwnCmd0->place = _setPos(SNdlPosReset);

    //清洗
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_WASH_TYPE);
    CMD_NDL_WASH_TYPE* pDwnCmd1 = (CMD_NDL_WASH_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd1->cmd = CMD_NDL_WASH;
    pDwnCmd1->type = 0;
    pDwnCmd1->time = 0;

    return VOS_OK;
}

vos_u32 CSmpNdl::_parseWashLong(msg_stComDevActInfo* pComAct)
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
    pDwnCmd0->place = _setPos(SNdlPosReset);

    //清洗
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_NDL_WASH_TYPE);
    CMD_NDL_WASH_TYPE* pDwnCmd1 = (CMD_NDL_WASH_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pDwnCmd1->cmd = CMD_NDL_WASH;
    pDwnCmd1->type = 1;
    pDwnCmd1->time = 0;

    return VOS_OK;
}

void CSmpNdl::_setSampleNdlRttPos(msg_stNdlHrotate* pHrotate, app_u16 dstPos)
{
	switch (dstPos)
    {
        case SmpPosWash:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_wash_pos;
            pHrotate->enPosition = cmd_sample_ndl_wash_spot;
            break;
            
        case SmpPosDilute:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_dilute_pos;
            pHrotate->enPosition = cmd_sample_ndl_dilute_spot;
            break;
            
        case SmpPosDischNml:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_nml_dischpos;
            pHrotate->enPosition = cmd_sample_ndl_nml_discharge_spot;
            break;
            
        case SmpPosDischISE:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_ise_dischpos;
            pHrotate->enPosition = cmd_sample_ndl_ise_discharge_spot;
            break;
            
        case SmpPosAbsrb1:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_dsk_absrb1pos;
            pHrotate->enPosition = cmd_sample_ndl_1circle_absorb_spot;
            break;
            
        case SmpPosAbsrb2:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_dsk_absrb2pos;
            pHrotate->enPosition = cmd_sample_ndl_2circle_absorb_spot;
            break;
            
        case SmpPosAbsrb3:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_dsk_absrb3pos;
            pHrotate->enPosition = cmd_sample_ndl_3circle_absorb_spot;
            break;
            
        case SmpPosAbsrb4:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_dsk_absrb4pos;
            pHrotate->enPosition = cmd_sample_ndl_4circle_absorb_spot;
            break;
            
        case SmpPosAbsrb5:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_dsk_absrb5pos;
            pHrotate->enPosition = cmd_sample_ndl_5circle_absorb_spot;
            break;

        case SmpPosRailAbsrb:
            pHrotate->cmd        = cmd_sample_ndl_hrtt_rail_absrbpos;
            pHrotate->enPosition = cmd_sample_ndl_rail_absorb_spot;
            break;
            
        default:
            WRITE_ERR_LOG("Invalid sample needle stop position(%d) \n", dstPos);
            break;
    }
}

vos_u32 CSmpNdl::_parseIntensifyWash(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stComDwnmachCombCmd) + sizeof(msg_stComCombSingleCmd) * 6
        + sizeof(msg_stNdlHrotate) * 2 + sizeof(cmd_stNdlVrt) * 2 + sizeof(msg_stNdlAbsrb) + sizeof(msg_stOneCmd);
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
    pRtt->cmd = cmd_sample_ndl_hrtt_dsk_absrb5pos;
    pRtt->enPosition = cmd_sample_ndl_5circle_absorb_spot;

    //垂直下降
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(cmd_stNdlVrt);
    cmd_stNdlVrt* pDown = (cmd_stNdlVrt*)(&(pSingleCmd->aucCmdLoad));
    pDown->cmd = cmd_sample_ndl_vdecline;

    //吸清洗剂
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stNdlAbsrb);
    msg_stNdlAbsrb* pAbsorb = (msg_stNdlAbsrb*)(&(pSingleCmd->aucCmdLoad));
    pAbsorb->cmd = cmd_sample_ndl_absorb_sample;
    pAbsorb->quantity = INTENSIFY_WASH_QUANTITY;

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(cmd_stNdlVrt);
    cmd_stNdlVrt* pUp = (cmd_stNdlVrt*)(&(pSingleCmd->aucCmdLoad));
    pUp->cmd = cmd_sample_ndl_vrise;

    //回清洗位
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stNdlHrotate);
    msg_stNdlHrotate* pToWashPos = (msg_stNdlHrotate*)(&(pSingleCmd->aucCmdLoad));
    pToWashPos->cmd = cmd_sample_ndl_hrtt_wash_pos;
    pToWashPos->enPosition = cmd_sample_ndl_wash_spot;

    //强化清洗
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(msg_stOneCmd);
    msg_stOneCmd* pIntensifyWash = (msg_stOneCmd*)(&(pSingleCmd->aucCmdLoad));
    pIntensifyWash->cmd = cmd_sample_ndl_intensify_wash;

    return VOS_OK;
}

vos_u32 CSmpNdl::_parseAirOut(msg_stComDevActInfo* pComAct)
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

vos_u32 CSmpNdl::_parseTrimOffset(msg_stComDevActInfo* pComAct)
{
    return ParseTrimOffset(pComAct,cmd_sample_ndl_step_move); 
}

vos_u32 CSmpNdl::_parseNdlVrt(msg_stComDevActInfo* pComAct)
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


vos_u32 CSmpNdl::_parsePVCtrl(msg_stComDevActInfo* pComAct)
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

vos_u32 CSmpNdl::_parsePumpCtrl(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_NDL_SAMPLE_TYPE);
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

uint8_t CSmpNdl::_setPos(app_u16 usDstPos)
{
    if (m_ulPid == SPid_SmpNdl)
    {
        return (usDstPos == SNdlPosDilute) ? PLACE_S1RTT_THINNING
            : (usDstPos == SNdlPosDischarg) ? PLACE_S1RTT_DISCHARGE
            : (usDstPos == SNdlPosReset) ? PLACE_S1RTT_RESET
            : (usDstPos == SNdlPosW1) ? PLACE_S1RTT_WASH1
            : (usDstPos == SNdlPosW2) ? PLACE_S1RTT_WASH2
            : (usDstPos == SNdlPosW3) ? PLACE_S1RTT_WASH3
            : (usDstPos == SNdlPosEmer) ? PLACE_S1RTT_INHALATION1
            : (usDstPos == SNdlPosNorm) ? PLACE_S1RTT_INHALATION2 : 0;
    }
    return (usDstPos == SNdlPosDilute) ? PLACE_S2RTT_THINNING
        : (usDstPos == SNdlPosDischarg) ? PLACE_S2RTT_DISCHARGE
        : (usDstPos == SNdlPosReset) ? PLACE_S2RTT_RESET
        : (usDstPos == SNdlPosW1) ? PLACE_S2RTT_WASH1
        : (usDstPos == SNdlPosW2) ? PLACE_S2RTT_WASH2
        : (usDstPos == SNdlPosW3) ? PLACE_S2RTT_WASH3
        : (usDstPos == SNdlPosEmer) ? PLACE_S2RTT_INHALATION2
        : (usDstPos == SNdlPosNorm) ? PLACE_S2RTT_INHALATION1 : 0;
}