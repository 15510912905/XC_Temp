#include "coolreag.h"
#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"

CCoolReag::CCoolReag(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}

CCoolReag::~CCoolReag()
{
}
//初始化解析函数
void CCoolReag::_initParseFuc()
{
    m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
    m_parseFucs[Act_TempQuery] = (ParseAct)&CCoolReag::_parseTempQuery;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
	m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_CoolingCtrl] = (ParseAct)&CCoolReag::_parseCoolingCtrl;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
}

vos_u32 CCoolReag::_parseTempQuery(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stOneCmd);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stOneCmd * pCmd = (msg_stOneCmd *)&(pActInfo->stDwnCmd);
    pCmd->cmd = XC8002_COM_QUERY_CMD;

    return VOS_OK;
}

vos_u32 CCoolReag::_parseCoolingCtrl(msg_stComDevActInfo* pComAct)
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
    pCmd->cmd = cmd_cooling_control;

    return VOS_OK;
}


