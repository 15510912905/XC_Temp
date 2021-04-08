#include "smpmix.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"

CSmpMix::CSmpMix(vos_u32 ulPid) : CSubSys(ulPid, xc8002_dwnmach_name_smpl_mixrod)
{
    _initParseFuc();
}

CSmpMix::~CSmpMix()
{
}
//初始化解析函数
void CSmpMix::_initParseFuc()
{
    m_parseFucs[Act_Reset] = (ParseAct)&CSmpMix::_parseReset;
    m_parseFucs[Act_Move] = (ParseAct)&CSmpMix::_parseMove;
    m_parseFucs[Act_Mix] = (ParseAct)&CSmpMix::_parseMix;
    m_parseFucs[Act_Wash] = (ParseAct)&CSmpMix::_parseWash;
    m_parseFucs[Act_NdlVrt] = (ParseAct)&CSmpMix::_parseNdlVrt;
    m_parseFucs[Act_PVCtrl] = (ParseAct)&CSmpMix::_parsePVCtrl;
    m_parseFucs[Act_OlnyMix] = (ParseAct)& CSmpMix::_parseOnlyMix;

	m_parseFucs[Act_TrimOffset] = (ParseAct)&CSmpMix::_parseTrimOffset;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
	m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
	m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
}

vos_u32 CSmpMix::_parseReset(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdSize = sizeof(CMD_STIR_RESET_TYPE);

    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdSize);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    //msg_stAirout *pAiroutInfo = (msg_stAirout *)(pComAct->aucPara);
    CMD_STIR_RESET_TYPE* pCmd = (CMD_STIR_RESET_TYPE*)&(pActInfo->stDwnCmd);
    pCmd->cmd = CMD_STIR_RESET;
    pCmd->dis_wash = 1;
    pCmd->time = 0;
    return VOS_OK;
}

vos_u32 CSmpMix::_parseMove(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_STIR_TURN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stNdlRtt * pNdlRtt = (msg_stNdlRtt *)pComAct->aucPara;
    CMD_STIR_TURN_TYPE * pHrotate = (CMD_STIR_TURN_TYPE *)&(pActInfo->stDwnCmd);
    pHrotate->cmd = CMD_STIR_TURN;
    pHrotate->place = pNdlRtt->usDstPos;
    return VOS_OK;
}

vos_u32 CSmpMix::_parseMix(msg_stComDevActInfo* pComAct)
{
    app_u16 cmdNum = 3;
    vos_u32 cmdLen = sizeof(msg_stComDwnmachCombCmd) + sizeof(msg_stComCombSingleCmd) * cmdNum
        + sizeof(CMD_STIR_UPDOWN_TYPE) + sizeof(CMD_STIR_TYPE) + sizeof(CMD_STIR_UPDOWN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)&(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = cmdNum;
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);

    //垂直下降
    pSingleCmd->ulCmdSize = sizeof(CMD_STIR_UPDOWN_TYPE);
    CMD_STIR_UPDOWN_TYPE* pVrotateDwn = (CMD_STIR_UPDOWN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pVrotateDwn->cmd = CMD_STIR_UPDOWN;
    pVrotateDwn->place = 1;

    //搅拌
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_STIR_TYPE);
    CMD_STIR_TYPE* pMixing = (CMD_STIR_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pMixing->cmd = CMD_STIR;
    pMixing->dis_wash = 0;
    pMixing->time = 0;

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_STIR_UPDOWN_TYPE);
    CMD_STIR_UPDOWN_TYPE* pVrotateUp = (CMD_STIR_UPDOWN_TYPE*)(&(pSingleCmd->aucCmdLoad));
    pVrotateUp->cmd = CMD_STIR_UPDOWN;
    pVrotateUp->place = 0;

    //     //水平转动
    //     pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    //     pSingleCmd->ulCmdSize = sizeof(msg_stNdlHrotate);
    //     msg_stNdlHrotate* pHrotate = (msg_stNdlHrotate*)(&(pSingleCmd->aucCmdLoad));
    /*    pHrotate->cmd = cmd_sample_mix_ndl_hrotate_washpos;*/
    return VOS_OK;
}
vos_u32 CSmpMix::_parseWash(msg_stComDevActInfo* pComAct)
{
    app_u16 cmdNum = 3;
    vos_u32 cmdLen = sizeof(msg_stComDwnmachCombCmd) + sizeof(msg_stComCombSingleCmd) * cmdNum
        + sizeof(CMD_STIR_UPDOWN_TYPE) + sizeof(CMD_STIR_WASH_TYPE) + sizeof(CMD_STIR_UPDOWN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stComDwnmachCombCmd * pCombCmd = (msg_stComDwnmachCombCmd *)&(pActInfo->stDwnCmd);
    pCombCmd->cmd = XC8002_COM_DWNMACH_COMB_CMD;
    pCombCmd->cmdNum = cmdNum;

    //垂直下降
    msg_stComCombSingleCmd * pSingleCmd = (msg_stComCombSingleCmd *)&(pCombCmd->aucCmdLoad);
    pSingleCmd->ulCmdSize = sizeof(CMD_STIR_UPDOWN_TYPE);
    CMD_STIR_UPDOWN_TYPE* pVrotateDwn = (CMD_STIR_UPDOWN_TYPE*)(&(pSingleCmd->aucCmdLoad));

    //清洗
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_STIR_WASH_TYPE);
    CMD_STIR_WASH_TYPE* pMixing = (CMD_STIR_WASH_TYPE*)(&(pSingleCmd->aucCmdLoad));

    //垂直上升
    pSingleCmd = (msg_stComCombSingleCmd*)(((char*)pSingleCmd) + pSingleCmd->ulCmdSize + sizeof(vos_u32));
    pSingleCmd->ulCmdSize = sizeof(CMD_STIR_UPDOWN_TYPE);
    CMD_STIR_UPDOWN_TYPE* pVrotateUp = (CMD_STIR_UPDOWN_TYPE*)(&(pSingleCmd->aucCmdLoad));

    pVrotateDwn->cmd = CMD_STIR_UPDOWN;
    pVrotateDwn->place = 1;
    pMixing->cmd = cmd_sample_mix_ndl_exe_washing;
    pMixing->dis_wash = 1;
    pMixing->time = 0;
    pVrotateUp->cmd = CMD_STIR_UPDOWN;
    pVrotateUp->place = 0;
    return VOS_OK;
}

vos_u32 CSmpMix::_parseTrimOffset(msg_stComDevActInfo* pComAct)
{
    return ParseTrimOffset(pComAct,cmd_sample_mix_step_move); 
}

vos_u32 CSmpMix::_parseNdlVrt(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_STIR_UPDOWN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stVrt *pMsgNdlVrt = (msg_stVrt *)(pComAct->aucPara);
    CMD_STIR_UPDOWN_TYPE* pCmdNdlVrtUp = (CMD_STIR_UPDOWN_TYPE*)&(pActInfo->stDwnCmd);
    pCmdNdlVrtUp->cmd = CMD_STIR_UPDOWN;
    if (cmd_motor_dir_up == pMsgNdlVrt->dir)
    {
        pCmdNdlVrtUp->place = 0;
    }
    else if (cmd_motor_dir_down == pMsgNdlVrt->dir)
    {
        pCmdNdlVrtUp->place = 1;
    }
    return VOS_OK;
}

vos_u32 CSmpMix::_parsePVCtrl(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_STIR_PVCONTROL_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_PVCtrl* pPara = (msg_PVCtrl *)pComAct->aucPara;
    CMD_STIR_PVCONTROL_TYPE * pDwnCmd = (CMD_STIR_PVCONTROL_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_STIR_PVCONTROL;
    pDwnCmd->number = pPara->PVNum;
    pDwnCmd->status = pPara->operateType;
    return VOS_OK;
}

vos_u32 CSmpMix::_parseOnlyMix(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_STIR_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    CMD_STIR_TYPE * pDwnCmd = (CMD_STIR_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_STIR;
    pDwnCmd->dis_wash = 1;
    pDwnCmd->time = 0;
    return VOS_OK;
}


