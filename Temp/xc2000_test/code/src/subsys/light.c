#include "light.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include "app_downmsg.h"
#include "app_innermsg.h"

CLight::CLight(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}


CLight::~CLight()
{
}
void CLight::_initParseFuc()
{
    m_parseFucs[Act_Light] = (ParseAct)&CLight::_parseSetLight;
}
vos_u32 CLight::_parseSetLight(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_SET_LIGHT_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stLightControl * pPara = (msg_stLightControl *)pComAct->aucPara;
    CMD_SET_LIGHT_TYPE * pCmd = (CMD_SET_LIGHT_TYPE *)&(pActInfo->stDwnCmd);
    pCmd->cmd = CMD_SET_LIGHT;
    pCmd->voltage = pPara->lightIntensity;

    return VOS_OK;
}
