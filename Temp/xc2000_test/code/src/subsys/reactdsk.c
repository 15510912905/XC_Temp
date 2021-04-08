#include "reactdsk.h"

#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"

CReactDsk::CReactDsk(vos_u32 ulPid) : CSubSys(ulPid, xc8002_dwnmach_name_reactdisk)
{
    _initParseFuc();
}

CReactDsk::~CReactDsk()
{
}
//初始化解析函数
void CReactDsk::_initParseFuc()
{
    m_parseFucs[Act_Reset] = (ParseAct)&CReactDsk::_parseReset;
    m_parseFucs[Act_Move] = (ParseAct)&CReactDsk::_parseMove;
	m_parseFucs[Act_Light] = (ParseAct)&CReactDsk::_parseLight;
	m_parseFucs[Act_Ad] = (ParseAct)&CReactDsk::_parseAd;//暂时未用
	m_parseFucs[Act_TrimOffset] = (ParseAct)&CReactDsk::_parseTrimOffset;
    m_parseFucs[Act_DwnQuery] = &CSubSys::ParseDwnQuery;
	m_parseFucs[Act_Fault] = &CSubSys::ParseDwnFault;
    m_parseFucs[Act_DskHrotate] = (ParseAct)&CReactDsk::_parseDskHrotate;
	m_parseFucs[Act_MidState] = &CSubSys::ParseMidState;
    m_parseFucs[Act_BBClear] = &CSubSys::ParseBBCLEAR;
}

vos_u32 CReactDsk::_parseReset(msg_stComDevActInfo* pComAct)
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

vos_u32 CReactDsk::_parseMove(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(CMD_DSK_TURN_TYPE);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    act_stReactsysDskRtt * pDskRtt = (act_stReactsysDskRtt *)pComAct->aucPara;
    CMD_DSK_TURN_TYPE * pRttCmd = (CMD_DSK_TURN_TYPE *)&(pActInfo->stDwnCmd);
    pRttCmd->cmd = CMD_DSK_TURN;
    pRttCmd->type = (cmd_motor_dir_ANTICLOCKWISE == pDskRtt->dir) ? 1 : 2;
    pRttCmd->cup_number = pDskRtt->usRttedCupNum;

    return VOS_OK;
}

vos_u32 CReactDsk::_parseLight(msg_stComDevActInfo* pComAct)
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

vos_u32 CReactDsk::_parseAd(msg_stComDevActInfo* pComAct)////暂时未用
{
    vos_u32 cmdLen = sizeof(cmd_stAdConfig);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stAdConfig * pPara = (msg_stAdConfig *)pComAct->aucPara;
    cmd_stAdConfig * pCmd = (cmd_stAdConfig *)&(pActInfo->stDwnCmd);
    pCmd->cmd = cmd_react_disk_get_adval;
	pCmd->srv = 0;

	memcpy((char*)&(pCmd->dpValue[0]),(char*)&(pPara->dpValue[0]),sizeof(vos_u32)*WAVE_NUM);

    return VOS_OK;
}

vos_u32 CReactDsk::_parseTrimOffset(msg_stComDevActInfo* pComAct)
{
    return ParseTrimOffset(pComAct,cmd_react_disk_step_move); 
}


vos_u32 CReactDsk::_parseDskHrotate(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stRDskHrotate);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_stDskHrotate * pPara = (msg_stDskHrotate *)pComAct->aucPara;
    cmd_stReactDskHrotate* pDwnCmd = (cmd_stReactDskHrotate *)&(pActInfo->stDwnCmd);  
    pDwnCmd->cupNum= pPara->num;
    if (cmd_motor_dir_ANTICLOCKWISE == pPara->dir)
    {
        pDwnCmd->cmd = cmd_react_disk_anticlockwise_rtt; 
    }
    else if (cmd_motor_dir_CLOCKWISE == pPara->dir)
    {
        pDwnCmd->cmd = cmd_react_disk_clockwise_rtt; 
    }
    
    return VOS_OK;
}

