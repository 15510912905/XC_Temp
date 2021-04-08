#include "verticalgripper.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include "app_downmsg.h"
#include "app_innermsg.h"
#include "subsys.h"
#include "apputils.h"
CVerticalGripper::CVerticalGripper(vos_u32 ulPid, vos_u32 ulDstSubsys) :CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}
CVerticalGripper::~CVerticalGripper()
{

}
void CVerticalGripper::_initParseFuc()
{
    m_mActCMD[Act_Reset] = CMD_AUTO_LOAD_VG_RE;
    m_parseFucs[Act_Reset] = (ParseAct)&CVerticalGripper::ParseSingleCMD;
    m_parseFucs[Act_Move] = (ParseAct)&CVerticalGripper::_parseLoction;
    m_parseFucs[Act_VGMoveF] = (ParseAct)&CVerticalGripper::_parseLoction;
    m_parseFucs[Act_VGMoveL] = (ParseAct)&CVerticalGripper::_parseLoction;
    m_parseFucs[Act_VGMoveR] = (ParseAct)&CVerticalGripper::_parseLoction;
}
//解析定位动作
vos_u32 CVerticalGripper::_parseLoction(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_AUTOLOAD_MOTZMOVEPOSITION_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    act_autoload_move * pPara = (act_autoload_move *)pComAct->aucPara;
    CMD_AUTOLOAD_MOTZMOVEPOSITION_TYPE * pDwnCmd = (CMD_AUTOLOAD_MOTZMOVEPOSITION_TYPE *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = CMD_AUTO_LOAD_VG_MOVE;
    pDwnCmd->type = pPara->type;
    pDwnCmd->place = pPara->place;
    app_print(ps_autoload, "AutoLoad VG %d %d\n", pPara->type,pDwnCmd->place);
    return VOS_OK;
}
vos_u32 CVerticalGripper::ParseSingleCMD(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdSize = sizeof(CMD_Single_TYPE);

    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdSize);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    CMD_Single_TYPE* pCmp = (CMD_Single_TYPE*)pActInfo->stDwnCmd;
    pCmp->cmd = CMD_AUTO_LOAD_VG_RE;
    return VOS_OK;
}