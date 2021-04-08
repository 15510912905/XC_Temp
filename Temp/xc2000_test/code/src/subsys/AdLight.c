#include "AdLight.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include "app_downmsg.h"
#include "app_innermsg.h"

CAdLight::CAdLight(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}


CAdLight::~CAdLight()
{
}

void CAdLight::_initParseFuc()
{
    m_mActCMD[Act_Showave] = CMD_READ_ALL_AD;
    m_parseFucs[Act_Showave] = (ParseAct)&CSubSys::ParseSingleCMD;

    m_parseFucs[Act_Ad] = (ParseAct)&CAdLight::_parseReadAD;
    m_parseFucs[Act_OneAD] = (ParseAct)&CAdLight::_parseReadOneAD;
    m_parseFucs[Act_SetGain] = (ParseAct)&CAdLight::_parseSetGain;   
}

vos_u32 CAdLight::_parseReadAD(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_READ_AD_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stReadAD * pPara = (act_stReadAD *)pComAct->aucPara;
    CMD_READ_AD_TYPE * pDwnCmd = (CMD_READ_AD_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_READ_AD;
    pDwnCmd->AdCount = pPara->AdCount;
    return VOS_OK;
}

vos_u32 CAdLight::_parseReadOneAD(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_READ_ONE_AD_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stReadOneAD * pPara = (act_stReadOneAD *)pComAct->aucPara;
    CMD_READ_ONE_AD_TYPE * pDwnCmd = (CMD_READ_ONE_AD_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_READ_ONE_AD;
    pDwnCmd->channel = pPara->channel;
    return VOS_OK;
}

vos_u32 CAdLight::_parseSetGain(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_NDL_TURN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stGain * pPara = (act_stGain *)pComAct->aucPara;
    CMD_SET_GAIN_TYPE * pDwnCmd = (CMD_SET_GAIN_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_SET_GAIN;
    pDwnCmd->channel = pPara->channel;
    pDwnCmd->gain = pPara->gain;
    return VOS_OK;
}