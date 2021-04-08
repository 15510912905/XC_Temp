#include "preciseadd.h"
#include "vos_log.h"
#include "apputils.h"
#include "vos_errorcode.h"
#include "app_msg.h"
#include "app_comstructdefine.h"
#include "socketmonitor.h"

#define TimeLen 500  //一次加样5s

CPreciseAdd::CPreciseAdd(vos_u32 ulPid) : CCallBack(ulPid), CSubActMng()
{
    m_ulTimer = createTimer(SPid_PreciseAdd, "PreciseAdd");

    m_mSubAct[SPid_R1Dsk] = new CSubAct(SPid_R1Dsk);
    m_mSubAct[SPid_R2Dsk] = new CSubAct(SPid_R2Dsk);
    m_mSubAct[SPid_SmpDsk] = new CSubAct(SPid_SmpDsk);
    m_mSubAct[SPid_R1] = new CSubAct(SPid_R1);
    m_mSubAct[SPid_R2] = new CSubAct(SPid_R2);
    m_mSubAct[SPid_SmpNdl] = new CSubAct(SPid_SmpNdl);
    m_mSubAct[SPid_R1b] = new CSubAct(SPid_R1b);
    m_mSubAct[SPid_R2b] = new CSubAct(SPid_R2b);
    m_mSubAct[SPid_SmpNdlb] = new CSubAct(SPid_SmpNdlb);


    m_mActFunc[SPid_SmpNdl][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R1][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R1Dsk][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_SmpNdlb][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R1b][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R2][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R2b][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R2Dsk][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_SmpDsk][Act_Reset] = (ActFun)&CPreciseAdd::_singleAct;

    m_mActFunc[SPid_R1Dsk][Act_Move] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_R2Dsk][Act_Move] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_SmpDsk][Act_Move] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_R1Dsk][Act_MoveOuter] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_R2Dsk][Act_MoveOuter] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_SmpDsk][Act_MoveOuter] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_R1Dsk][Act_MoveInner] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_R2Dsk][Act_MoveInner] = (ActFun)&CPreciseAdd::_diskMove;
    m_mActFunc[SPid_SmpDsk][Act_MoveInner] = (ActFun)&CPreciseAdd::_diskMove;

    m_mActFunc[SPid_SmpNdl][Act_MoveAbs] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_SmpNdl][Act_Absorb] = (ActFun)&CPreciseAdd::_ndlAbsorb;
    m_mActFunc[SPid_SmpNdl][Act_Discharge] = (ActFun)&CPreciseAdd::_ndlDischarge;
    m_mActFunc[SPid_SmpNdl][Act_MoveW] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_SmpNdl][Act_Wash] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R1][Act_MoveAbs] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R1][Act_Absorb] = (ActFun)&CPreciseAdd::_ndlAbsorb;
    m_mActFunc[SPid_R1][Act_Discharge] = (ActFun)&CPreciseAdd::_ndlDischarge;
    m_mActFunc[SPid_R1][Act_MoveW] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R1][Act_Wash] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R2][Act_MoveAbs] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R2][Act_Absorb] = (ActFun)&CPreciseAdd::_ndlAbsorb;
    m_mActFunc[SPid_R2][Act_Discharge] = (ActFun)&CPreciseAdd::_ndlDischarge;
    m_mActFunc[SPid_R2][Act_MoveW] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R2][Act_Wash] = (ActFun)&CPreciseAdd::_singleAct;

    m_mActFunc[SPid_SmpNdlb][Act_MoveAbs] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_SmpNdlb][Act_Absorb] = (ActFun)&CPreciseAdd::_ndlAbsorb;
    m_mActFunc[SPid_SmpNdlb][Act_Discharge] = (ActFun)&CPreciseAdd::_ndlDischarge;
    m_mActFunc[SPid_SmpNdlb][Act_MoveW] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_SmpNdlb][Act_Wash] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R1b][Act_MoveAbs] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R1b][Act_Absorb] = (ActFun)&CPreciseAdd::_ndlAbsorb;
    m_mActFunc[SPid_R1b][Act_Discharge] = (ActFun)&CPreciseAdd::_ndlDischarge;
    m_mActFunc[SPid_R1b][Act_MoveW] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R1b][Act_Wash] = (ActFun)&CPreciseAdd::_singleAct;
    m_mActFunc[SPid_R2b][Act_MoveAbs] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R2b][Act_Absorb] = (ActFun)&CPreciseAdd::_ndlAbsorb;
    m_mActFunc[SPid_R2b][Act_Discharge] = (ActFun)&CPreciseAdd::_ndlDischarge;
    m_mActFunc[SPid_R2b][Act_MoveW] = (ActFun)&CPreciseAdd::_ndlMove;
    m_mActFunc[SPid_R2b][Act_Wash] = (ActFun)&CPreciseAdd::_singleAct;


    m_mResFuncs[SPid_SmpNdl][Act_Wash] = (ProcResFuc)&CPreciseAdd::_procNdlWash;
    m_mResFuncs[SPid_R1][Act_Wash] = (ProcResFuc)&CPreciseAdd::_procNdlWash;
}

CPreciseAdd::~CPreciseAdd()
{

}

void CPreciseAdd::_reset(app_u32 subSys)
{
    m_ulActIndex++;
    if (subSys == SPid_SmpNdl || subSys == SPid_SmpNdlb)
    {
        InsertAct(SPid_SmpNdl, Act_Reset);
        InsertAct(SPid_SmpNdlb, Act_Reset);
        InsertAct(SPid_SmpDsk, Act_Reset, SPid_SmpNdl, Act_Reset, m_ulActIndex, SPid_SmpNdlb, Act_Reset, m_ulActIndex);
    }
    if (subSys == SPid_R1 || subSys == SPid_R1b)
    {
        InsertAct(SPid_R1, Act_Reset);
        InsertAct(SPid_R1b, Act_Reset);
        InsertAct(SPid_R1Dsk, Act_Reset, SPid_R1, Act_Reset, m_ulActIndex, SPid_R1b, Act_Reset, m_ulActIndex);
    }
    if (subSys == SPid_R2 || subSys == SPid_R2b)
    {
        InsertAct(SPid_R2, Act_Reset);
        InsertAct(SPid_R2b, Act_Reset);
        InsertAct(SPid_R2Dsk, Act_Reset, SPid_R2, Act_Reset, m_ulActIndex, SPid_R2b, Act_Reset, m_ulActIndex);
    }
    startTimer(m_ulTimer, TimeLen);
    SendNextAct();
}

void CPreciseAdd::_addPrecisea(app_u32 ndlSys)
{
    m_ulLeftTimes--;
    m_ulActIndex++;
    app_u32 diskSys = (ndlSys == SPid_R1 || ndlSys == SPid_R1b) ? SPid_R1Dsk
        : (ndlSys == SPid_R2 || ndlSys == SPid_R2b) ? SPid_R2Dsk
        : (ndlSys == SPid_SmpNdl || ndlSys == SPid_SmpNdlb) ? SPid_SmpDsk : 0;
    app_u32 diskAct = (ndlSys == SPid_R1 || ndlSys == SPid_R2 || ndlSys == SPid_SmpNdl) ? Act_MoveOuter : Act_MoveInner;

    InsertAct(diskSys, diskAct);
    InsertAct(ndlSys, Act_MoveAbs);
    InsertAct(ndlSys, Act_Absorb, diskSys, diskAct, m_ulActIndex);
    InsertAct(diskSys, Act_Move, ndlSys, Act_Absorb, m_ulActIndex);
    InsertAct(ndlSys, Act_Discharge, diskSys, Act_Move, m_ulActIndex);
    InsertAct(ndlSys, Act_MoveW);
    InsertAct(ndlSys, Act_Wash);
    startTimer(m_ulTimer, TimeLen);
    SendNextAct();
}

app_u32 CPreciseAdd::_stop()
{
    ResetAct();
    INITP04(m_ulPid,Press_our);
    INITP04(m_ulPid, Press_in);
    m_ulLeftTimes = 0;
    return 0;
}

app_u32 CPreciseAdd::_nextAdd()
{
    if (IsFinish())
    {
        vos_stoptimer(m_ulTimer);
        if (m_ulLeftTimes == 0)
        {
            INITP04(m_ulPid, Press_our);
            INITP04(m_ulPid, Press_in);
            return _procActResult(vos_midmachine_state_standby);
        }
        _addPrecisea(m_stReqPreciseAdd.uisubsysid());
    }
    return VOS_OK;
}

app_u32 CPreciseAdd::_diskMove(app_u32 subSys, app_u16 actType)
{
    msg_paraSampleDskRtt stSmpPara;
    act_stReagSysDskRtt stPara;
    stPara.usCupId = (actType == Act_Move) ? m_stReqPreciseAdd.uidischargepos()
        : m_stReqPreciseAdd.uiabsorbpos();
    stPara.usDstPos = (m_stReqPreciseAdd.uisubsysid() == SPid_SmpNdl || m_stReqPreciseAdd.uisubsysid() == SPid_SmpNdlb) ? 1
        : (m_stReqPreciseAdd.uisubsysid() == SPid_R1) ? R1DskPosouter1
        : (m_stReqPreciseAdd.uisubsysid() == SPid_R1b) ? R1DskPosInner1
        : (m_stReqPreciseAdd.uisubsysid() == SPid_R2) ? R2DskPosouter1
        : (m_stReqPreciseAdd.uisubsysid() == SPid_R2b) ? R2DskPosInner1 : 0;
    printf("Disk(%s),%d Move to %d\n", g_mSubSys[subSys].c_str(), stPara.usCupId, stPara.usDstPos);
    if (subSys == SPid_SmpDsk)
    {
        stSmpPara.usSmpCupId = stPara.usCupId;
        stSmpPara.usCircleId = stPara.usDstPos;
        return app_reqDwmCmd(m_ulPid, subSys, actType, sizeof(msg_paraSampleDskRtt), (char*)&stSmpPara, APP_YES);
    }
    return app_reqDwmCmd(m_ulPid, subSys, actType, sizeof(act_stReagSysDskRtt), (char*)&stPara, APP_YES);
}

app_u32 CPreciseAdd::_ndlMove(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt stPara;
    if (actType == Act_MoveW)
    {
        stPara.usDstPos = (subSys == SPid_SmpNdl || subSys == SPid_SmpNdlb) ? SNdlPosReset : RNdlPosReset;
    }
    else if (subSys == SPid_R1 || subSys == SPid_R1b || subSys == SPid_R2 || subSys == SPid_R2b)
    {
        stPara.usDstPos = RNdlPosAbsorb1;
    }
    else
    {
        stPara.usDstPos = SNdlPosNorm;
    }
    return app_reqDwmCmd(m_ulPid, subSys, actType, sizeof(msg_stNdlRtt), (char*)&stPara, APP_YES);
}

app_u32 CPreciseAdd::_ndlAbsorb(app_u32 subSys, app_u16 actType)
{
    if (subSys == SPid_SmpNdl || subSys == SPid_SmpNdlb)
    {
        msg_stSampleSysAbsorb stPara;
        stPara.ulSampleVolume = m_stReqPreciseAdd.uiabsorbamount();
        stPara.cupId = m_stReqPreciseAdd.uiabsorbpos();
        return app_reqDwmCmd(m_ulPid, subSys, actType, sizeof(msg_stSampleSysAbsorb), (char*)&stPara, APP_YES);
    }

    msg_stReagSysAbsorb stPara;
    stPara.ulVolume = m_stReqPreciseAdd.uiabsorbamount();
    stPara.cupId = m_stReqPreciseAdd.uiabsorbpos();
    return app_reqDwmCmd(m_ulPid, subSys, actType, sizeof(msg_stReagSysAbsorb), (char*)&stPara, APP_YES);
}

app_u32 CPreciseAdd::_ndlDischarge(app_u32 subSys, app_u16 actType)
{
    if (subSys == SPid_SmpNdl || subSys == SPid_SmpNdlb)
    {
        msg_stSampleSysDischarge stPara;
        stPara.usDischPos = SNdlPosNorm;
        stPara.usTotalVol = m_stReqPreciseAdd.uidischargeamount();
        stPara.ulSampleVolume = m_stReqPreciseAdd.uidischargeamount();
        return app_reqDwmCmd(m_ulPid, subSys, actType, sizeof(msg_stSampleSysDischarge), (char*)&stPara, APP_YES);
    }

    msg_stReagSysDischarge stPara;
    stPara.usDischPos = RNdlPosAbsorb1;
    stPara.usTotalVol = m_stReqPreciseAdd.uidischargeamount();
    stPara.ulReagVolume = m_stReqPreciseAdd.uidischargeamount();
    return app_reqDwmCmd(m_ulPid, subSys, actType, sizeof(msg_stReagSysDischarge), (char*)&stPara, APP_YES);
}

app_u32 CPreciseAdd::_singleAct(app_u32 subSys, app_u16 actType)
{
    app_u32 ulresult = app_reqDwmCmd(m_ulPid, subSys, actType, 0, NULL, APP_YES);
    if ((subSys == SPid_SmpNdl || subSys == SPid_R1) && (actType == Act_Wash))
    {
        OPEN_P04(m_ulPid);
    }
    if (actType == Act_MoveInner || actType == Act_MoveOuter)
    {
        CLOSE_P04(m_ulPid);
    }
    return ulresult;
}

vos_u32 CPreciseAdd::CallBack(vos_msg_header_stru* pMsg)
{
    if (MSG_ReqPreciseAdd == pMsg->usmsgtype)
    {
        return _procRecieveReq(pMsg);
    }
    if (app_com_device_res_exe_act_msg_type == pMsg->usmsgtype)
    {
        ProcResAct(pMsg);
        _nextAdd();
        return VOS_OK;
    }
    if (vos_pidmsgtype_timer_timeout == pMsg->usmsgtype)
    {
        WRITE_ERR_LOG("CPreciseAdd timeout\n");
        _stop();
        return _procActResult(vos_midmachine_state_precise_add_err);
    }
    if (app_req_stop_midmach_msg_type == pMsg->usmsgtype)
    {
        _stop();
        return vos_stoptimer(m_ulTimer);
    }
    WRITE_ERR_LOG("Unkown msgtype=%lu\n", pMsg->usmsgtype);
    return VOS_OK;
}

vos_u32  CPreciseAdd::_procRecieveReq(vos_msg_header_stru* pMsg)
{
    if (!g_pMonitor->CanHandleCmd())
    {
        WRITE_ERR_LOG("PreciseAdd failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }
    CMsg stMsg;
    if (stMsg.ParseMsg(pMsg))
    {
        app_reportmidmachstatus(vos_midmachine_state_precise_add, m_ulPid);
        m_stReqPreciseAdd.CopyFrom(*stMsg.pPara);
        m_ulLeftTimes = m_stReqPreciseAdd.uilooptimes();
        _nextAdd();
        //_reset(m_stReqPreciseAdd.uisubsysid());
        return VOS_OK;
    }
    return vos_para_invalid_err;
}

vos_u32 CPreciseAdd::_procActResult(vos_u32 ulResult)
{
    app_reportmidmachstatus(ulResult, m_ulPid);
    return VOS_OK;
}

void CPreciseAdd::_procNdlWash(app_com_dev_act_result_stru* pRes)
{
    CLOSE_P04(SPid_PreciseAdd);
}