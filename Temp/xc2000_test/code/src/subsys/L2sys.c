#include "L2sys.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"


CL2Sys::CL2Sys(vos_u32 ulPid, vos_u32 ulDstSubsys) : CSubSys(ulPid, ulDstSubsys)
{
    _initParseFuc();
}

CL2Sys::~CL2Sys()
{
}

//初始化解析函数
void CL2Sys::_initParseFuc()
{
    m_parseFucs[Act_Reset] = (ParseAct)&CL2Sys::_parseReset;
    m_parseFucs[Act_Move] = (ParseAct)&CL2Sys::_parseMove;
}
vos_u32 CL2Sys::_parseReset(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdSize = sizeof(CMD_Single_TYPE);

    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdSize);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    CMD_Single_TYPE* pCmp = (CMD_Single_TYPE*)pActInfo->stDwnCmd;
    pCmp->cmd = CMD_DSK_RESET;
    return VOS_OK;
}
//can通信协议转动有三种状态需要处理
vos_u32 CL2Sys::_parseMove(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_DSK_TURN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    act_autoload_move * pDskRtt = (act_autoload_move *)pComAct->aucPara;
    CMD_DSK_TURN_TYPE * pRttCmd = (CMD_DSK_TURN_TYPE *)&(pActInfo->stDwnCmd);
    pRttCmd->cmd = CMD_DSK_TURN;
    pRttCmd->type = 0;
    pRttCmd->cup_number = pDskRtt->place;
    return VOS_OK;
}


