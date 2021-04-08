#include "reset.h"
#include "vos_timer.h"
#include "typedefine.pb.h"
#include "app_msgtypedefine.h"
#include "app_comstructdefine.h"
#include "app_maintain.h"
#include "upmsg.pb.h"
#include "socketmonitor.h"
#include "apputils.h"
#include "vos_log.h"
#include "app_msg.h"
//王晖俊增加整机复位中常规抓手定位动作，延长定位器时间
#define RESET_TIME_LEN 12000//300

CReset::CReset(vos_u32 ulPid) :CCallBack(ulPid), m_bBusy(false), m_bUpReset(false), CSubActMng()
{
    m_ulTimer = createTimer(SPid_Reset, "RESET");

    m_mSubAct[SPid_SmpDsk] = new CSubAct(SPid_SmpDsk);
    m_mSubAct[SPid_SmpNdl] = new CSubAct(SPid_SmpNdl);
    m_mSubAct[SPid_R2Dsk] = new CSubAct(SPid_R2Dsk);
    m_mSubAct[SPid_R1] = new CSubAct(SPid_R1);
    m_mSubAct[SPid_R1Dsk] = new CSubAct(SPid_R1Dsk);
    m_mSubAct[SPid_R2] = new CSubAct(SPid_R2);
    m_mSubAct[SPid_SMix] = new CSubAct(SPid_SMix);
    m_mSubAct[SPid_AutoWash] = new CSubAct(SPid_AutoWash);
    m_mSubAct[SPid_ReactDsk] = new CSubAct(SPid_ReactDsk);
    m_mSubAct[SPid_R2Mix] = new CSubAct(SPid_R2Mix);
    m_mSubAct[SPid_R1b] = new CSubAct(SPid_R1b);
    m_mSubAct[SPid_R2b] = new CSubAct(SPid_R2b);
    m_mSubAct[SPid_SmpNdlb] = new CSubAct(SPid_SmpNdlb);
    m_mSubAct[SPid_SmpEmer] = new CSubAct(SPid_SmpEmer);


    m_mActFunc[SPid_SMix][Act_NdlVrt] = (ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_R2Mix][Act_NdlVrt] = (ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_SmpNdl][Act_NdlVrt] = (ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_R1][Act_NdlVrt]=(ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_R2][Act_NdlVrt] = (ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_SmpNdlb][Act_NdlVrt]=(ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_R1b][Act_NdlVrt]=(ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_R2b][Act_NdlVrt]=(ActFun)&CReset::_reqMixUp;
    m_mActFunc[SPid_SMix][Act_Move] = (ActFun)&CReset::_reqMixMove;
    m_mActFunc[SPid_R2Mix][Act_Move] = (ActFun)&CReset::_reqMixMove;

    m_mActFunc[SPid_SmpDsk][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_SmpDsk][Act_MoveInner] = (ActFun)&CReset::_reqSampleMove;
    m_mActFunc[SPid_SmpNdl][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_R2Dsk][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_R1][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_R1Dsk][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_R2][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_SMix][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_AutoWash][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_ReactDsk][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_R2Mix][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_R1b][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_R2b][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_SmpNdlb][Act_Reset] = (ActFun)&CReset::_reqReset;
    m_mActFunc[SPid_SmpEmer][Act_Reset] = (ActFun)&CReset::_reqReset;
}

CReset::~CReset()
{

}

vos_u32 CReset::CallBack(vos_msg_header_stru* pMsg)
{
    if (MSG_ReqReset == pMsg->usmsgtype)
    {
        return _procResetReq(pMsg);
    }
    if (MSG_ResReset == pMsg->usmsgtype || app_com_device_res_exe_act_msg_type == pMsg->usmsgtype)
    {
        return _procResetRes(pMsg);
    }
    if (vos_pidmsgtype_timer_timeout == pMsg->usmsgtype)
    {
        WRITE_ERR_LOG("Reset timeout\n");
        return _reportRes(EN_AF_Failed);
    }
    if (app_req_stop_midmach_msg_type == pMsg->usmsgtype)
    {
        ResetAct();
        m_bBusy = false;
        return vos_stoptimer(m_ulTimer);
    }
    return VOS_OK;
}
vos_u32 CReset::_procResetReq(vos_msg_header_stru* pMsg)
{
    if (m_bBusy)
    {
        WRITE_ERR_LOG("Reset is busy");
        return VOS_OK;
    }

    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }
    STReqReset * pPara = dynamic_cast<STReqReset*>(stMsg.pPara);
    m_ulSrcPid = pMsg->ulsrcpid;
    m_bUpReset = pPara->bupper();
    //如果是上位机主动复位需要报告状态
    if (m_bUpReset)
    {
        if (!g_pMonitor->CanReset())
        {
            WRITE_ERR_LOG("Reset failed, status=%lu \n", g_pMonitor->m_ulState);
            return VOS_OK;
        }
        app_reportmidmachstatus(vos_midmachine_state_resetting);
    }

    //复位过程中关闭AD采样
    app_adfpgapowerswitch(app_ad_fpga_power_mode_off);

    m_bBusy = true;
    _startTimer(RESET_TIME_LEN);
    if (VOS_OK != _insertAct())
    {
        return _reportRes(EN_AF_Failed);
    }
    return VOS_OK;
}

vos_u32 CReset::_startTimer(vos_u32 ulTimeLen)
{
    vos_u32 ulResult = vos_settimertimelen(m_ulTimer, ulTimeLen);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("vos_settimertimelen failed(%lx)\n", ulResult);
        return ulResult;
    }
    ulResult = vos_starttimer(m_ulTimer);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("vos_starttimer failed(%lx)\n", ulResult);
        return ulResult;
    }

    return VOS_OK;
}

app_u32 CReset::_insertAct()
{
    m_ulActIndex++;

    InsertAct(SPid_SMix, Act_NdlVrt);
    InsertAct(SPid_R2Mix, Act_NdlVrt);

    InsertAct(SPid_SmpNdl, Act_NdlVrt);
    InsertAct(SPid_R1, Act_NdlVrt);
    InsertAct(SPid_R2, Act_NdlVrt);
    InsertAct(SPid_SmpNdlb, Act_NdlVrt);
    InsertAct(SPid_R1b, Act_NdlVrt);
    InsertAct(SPid_R2b, Act_NdlVrt);
    InsertAct(SPid_SmpNdl, Act_Reset);
    InsertAct(SPid_R1, Act_Reset);
    InsertAct(SPid_R2, Act_Reset);
    InsertAct(SPid_SmpNdlb, Act_Reset);
    InsertAct(SPid_R1b, Act_Reset);
    InsertAct(SPid_R2b, Act_Reset);
    InsertAct(SPid_AutoWash, Act_Reset);

    STSubActInfo stActInfo1;
    stActInfo1.set_bdone(false);
    stActInfo1.set_uiacttype(Act_Reset);

    STRAct* pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_SMix);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct->set_uisubsys(SPid_R2Mix);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_SmpNdl);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R1);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R2);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_SmpNdlb);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R1b);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R2b);
    pRelyAct->set_uiacttype(Act_NdlVrt);
    pRelyAct->set_uiactindex(m_ulActIndex);
    pRelyAct = stActInfo1.add_strelyact();
    pRelyAct->set_uisubsys(SPid_AutoWash);
    pRelyAct->set_uiacttype(Act_Reset);
    pRelyAct->set_uiactindex(m_ulActIndex);
    InsertActST(SPid_ReactDsk, stActInfo1);

    InsertAct(SPid_SmpDsk, Act_Reset, SPid_SmpNdl, Act_NdlVrt, m_ulActIndex, SPid_SmpNdlb, Act_NdlVrt, m_ulActIndex);
    InsertAct(SPid_SmpEmer, Act_Reset, SPid_SmpNdl, Act_NdlVrt, m_ulActIndex, SPid_SmpNdlb, Act_NdlVrt, m_ulActIndex);
    InsertAct(SPid_R1Dsk, Act_Reset, SPid_R1, Act_NdlVrt, m_ulActIndex, SPid_R1b, Act_NdlVrt, m_ulActIndex);
    InsertAct(SPid_R2Dsk, Act_Reset, SPid_R2, Act_NdlVrt, m_ulActIndex, SPid_R2b, Act_NdlVrt, m_ulActIndex);
    InsertAct(SPid_SMix, Act_Reset, SPid_ReactDsk, Act_Reset, m_ulActIndex);
    InsertAct(SPid_R2Mix, Act_Reset, SPid_ReactDsk, Act_Reset, m_ulActIndex);
    InsertAct(SPid_SMix, Act_Move);
    InsertAct(SPid_R2Mix, Act_Move);
    //InsertAct(SPid_SmpDsk, Act_MoveInner);
    return SendNextAct();
}

vos_u32 CReset::_reportRes(vos_u32 ulResult)
{
    m_bBusy = false;
    ResetAct();
    if (!m_bUpReset)
    {
        STResResult pPara;
        pPara.set_uiresult(ulResult);
        sendInnerMsg(m_ulSrcPid, SPid_Reset, MSG_ResReset, &pPara);
    }
    else if (EN_AF_Success == ulResult)
    {
        app_reportmidmachstatus(vos_midmachine_state_standby);
    }
    else
    {
        app_reportmidmachstatus(vos_midmachine_state_reseted_err);
    }
    return vos_stoptimer(m_ulTimer);
}

vos_u32 CReset::_procResetRes(vos_msg_header_stru* pMsg)
{
    if (!m_bBusy)
    {
        WRITE_ERR_LOG("Not in reset,srcpid=%lu,msgtype=%lu\n", pMsg->ulsrcpid, pMsg->usmsgtype);
        return VOS_OK;
    }
    app_com_dev_act_result_stru * pResult = (app_com_dev_act_result_stru *)pMsg->aucmsgload;
    if (NULL == pResult || EN_AF_Success != pResult->enactrslt)
    {
        return _reportRes(EN_AF_Failed);
    }

    ProcResAct(pMsg);
    if (IsFinish())
    {
        return _reportRes(EN_AF_Success);
    }
    return VOS_OK;
}

app_u32 CReset::_reqReset(app_u32 subSys, app_u16 actType)
{
    vos_u32 ulResult = app_reqDwmCmd(SPid_Reset, subSys, Act_Reset, 0, NULL, APP_YES);
/*    vos_u32 ulResult = app_reqReset(SPid_Reset, subSys, APP_YES);*/
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("send reset to [%s] failed(%x)\n", g_mSubSys[subSys].c_str(), ulResult);
        _reportRes(EN_AF_Failed);
    }
    return ulResult;
}
app_u32 CReset::_reqSampleMove(app_u32 subSys, app_u16 actType)
{
    msg_paraSampleDskRtt samLoc;
    samLoc.usSmpCupId = 1;
    app_reqDwmCmd(SPid_Reset, SPid_SmpDsk, Act_MoveInner, sizeof(msg_paraSampleDskRtt), (char*)&samLoc, APP_YES);
}
app_u32 CReset::_reqMixUp(app_u32 subSys, app_u16 actType)
{
    msg_stVrt stPara;
    stPara.dir = cmd_motor_dir_up;
    vos_u32 ulResult = app_reqDwmCmd(SPid_Reset, subSys, Act_NdlVrt, sizeof(stPara), (char*)&stPara, APP_YES);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("send reset to SPid_SmpNdl failed(%x)\n", ulResult);
        _reportRes(EN_AF_Failed);
    }
    return ulResult;
}

app_u32 CReset::_reqMixMove(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt stPara = { 0 };
    return app_reqDwmCmd(SPid_Reset, subSys, actType, sizeof(stPara), (char*)&stPara, APP_YES);
}
