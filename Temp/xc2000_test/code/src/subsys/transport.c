#include "transport.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include "app_downmsg.h"
#include "app_innermsg.h"
#include "subsys.h"
#include "apputils.h"
CTransport::CTransport(vos_u32 ulPid, vos_u32 ulDstSubsys) :CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}
CTransport::~CTransport()
{

}
void CTransport::_initParseFuc()
{
    m_mActCMD[Act_Reset] = CMD_AUTO_LOAD_TSPT_RE;
    //m_mActCMD[Act_Move] = CMD_Loction_L0c;
    m_parseFucs[Act_Reset] = (ParseAct)&CTransport::ParseSingleCMD;
    m_parseFucs[Act_Move] = (ParseAct)&CTransport::_parseLoction;
    m_parseFucs[Act_TPMoveF] = (ParseAct)&CTransport::_parseLoction;
    m_parseFucs[Act_TPMoveL] = (ParseAct)&CTransport::_parseLoction;
    m_parseFucs[Act_TPMoveR] = (ParseAct)&CTransport::_parseLoction;
}
//解析定位动作
vos_u32 CTransport::_parseLoction(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(act_autoload_move);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_autoload_move * pPara = (act_autoload_move *)pComAct->aucPara;
    CMD_MODULE_MOVE_POSITION_TYPE * pDwnCmd = (CMD_MODULE_MOVE_POSITION_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_AUTO_LOAD_TSPT_MOVE;
    pDwnCmd->number = (pPara->place);
    app_print(ps_autoload, "AutoLoad  TP Loction %d\n", pDwnCmd->number);
    return VOS_OK;
}
vos_u32 CTransport::ParseSingleCMD(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdSize = sizeof(CMD_Single_TYPE);

    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdSize);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    CMD_Single_TYPE* pCmp = (CMD_Single_TYPE*)pActInfo->stDwnCmd;
    pCmp->cmd = CMD_AUTO_LOAD_TSPT_RE;
    return VOS_OK;
}