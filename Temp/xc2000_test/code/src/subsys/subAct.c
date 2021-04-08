#include "subAct.h"
#include "vos_log.h"
#include "apputils.h"
using namespace std;

CSubAct::CSubAct(app_u32 subSys) :m_uSubSys(subSys), m_bFault(false), m_uPreAct(INVALID_ACT), m_uActIndex(0)
{

}

CSubAct::~CSubAct() 
{

}

void CSubAct::Initialize()
{
    m_mActs.clear();
    m_qActInfo.clear();
}

void CSubAct::InsertAct(app_u32 actIndex, const STSubActInfo& stActInfo)
{
    m_uPreAct = INVALID_ACT;
    m_uActIndex = actIndex;
    m_mActs[stActInfo.uiacttype()] = false;
    m_qActInfo.push_back(stActInfo);
}

void CSubAct::SetDone(app_u32 actType, bool bDone, app_u32 actIndex /*= 0*/)
{
    if (actIndex != 0 && actIndex != m_uActIndex)
    {
        WRITE_ERR_LOG("sys[%d]act[%d] actIndex[%d]!= current[%d] is invalid\n", m_uSubSys, actType, actIndex, m_uActIndex);
        return;
    }

    if (bDone == false)
    {
        m_bFault = true;
        m_qActInfo.clear();
    }
    map<app_u32, bool>::iterator iter = m_mActs.find(actType);
    if (iter != m_mActs.end())
    {
        iter->second = bDone;
    }
}

bool CSubAct::IsFinish()
{
    if (m_bFault)
    {
        WRITE_ERR_LOG("---Sub(%d) Is bFault---\n", m_uSubSys);
        return false;
    }
    map<app_u32, bool>::iterator iter;
    for (iter = m_mActs.begin(); iter != m_mActs.end();++iter)
    {
        if (iter->second == false)
        {
            WRITE_ERR_LOG("---Sub(%d)Act(%d) Is Not Finish---\n", m_uSubSys, iter->first);
            return false;
        }
    }
    return true;
}

bool CSubAct::GetNextAct(STSubActInfo& stActInfo)
{
    if (!m_qActInfo.empty())
    { 
        stActInfo = m_qActInfo.front();
        return  CheckDone(m_uPreAct, m_uActIndex);
    }
    return false;
}

void CSubAct::PopNextAct()
{
    if (!m_qActInfo.empty())
    {
        STSubActInfo stActInfo = m_qActInfo.front();
        m_uPreAct = stActInfo.uiacttype();
        m_qActInfo.pop_front();
    }
}

bool CSubAct::CheckDone(app_u32 actType, app_u32 index)
{
    map<app_u32, bool>::iterator iter = m_mActs.find(actType);
    if (iter != m_mActs.end())
    {
        return ((index >= m_uActIndex) ? iter->second : true);
    }
    return true;
}

void CSubAct::ResetAct()
{
    m_uPreAct = INVALID_ACT;
    m_bFault = false;
    m_qActInfo.clear();
    m_mActs.clear();
}

void CSubAct::ClearAct()
{
    m_uPreAct = INVALID_ACT;
    m_qActInfo.clear();
    m_mActs.clear();
}


vos_u32 CSubAct::ReqDwmCmd(vos_u32 srcPid, vos_u32 dstPid, app_u16 actType, vos_u32 cmdLen, char* cmdPara, app_u16 singRes)
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq * pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) + cmdLen;

    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return ulResult;
    }

    pPara->usActNum = 1;
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead) + cmdLen;
    app_setActHead(&pActInfo->stActHead, actType, 0, 0, singRes);
    pActInfo->stActHead.ulActIndex = m_uActIndex;
    if (cmdLen != 0 && cmdPara != VOS_NULL)
    {
        memcpy(pActInfo->aucPara, cmdPara, cmdLen);
    }

    msg_stHead stHead;
    app_constructMsgHead(&stHead, dstPid, srcPid, app_com_device_req_exe_act_msg_type);

    WRITE_INFO_LOG("ActReq: acttype=%lu,dstpid=%lu,res=%u\n", actType, dstPid, singRes);
    return app_sendMsg(&stHead, pPara, ulParaLen);
}