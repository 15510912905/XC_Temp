/******************************************************************************

                   版权所有 (C), 2001-2100, 四川省新健康成股份有限公司

 ******************************************************************************
  文 件 名   : maintain.c
  版 本 号   : 初稿
  作    者   : zxc
  生成日期   : 2015年5月19日
  最近修改   :
  功能描述   : 维护功能
  函数列表   :
  修改历史   :
  1.日    期   : 2015年5月19日
    作    者   : zxc
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app_errorcode.h"
#include "app_msgtypedefine.h"
#include "app_debugtty.h"
#include "app_xc8002shareinfo.h"
#include "app_comstructdefine.h"
#include "app_maintain.h"
#include "app_downmsg.h"
#include "typedefine.pb.h"
#include "upmsg.pb.h"
#include "vos_mngmemory.h"
#include "vos_timer.h"
#include "vos_pid.h"
#include "msg.h"
#include "apputils.h"
#include "app_msg.h"
#include "vos_log.h"
#include "vos_adaptsys.h"
#include "vos_init.h"
CMaintain::CMaintain(vos_u32 ulPid) : CCallBack(ulPid)
{
    _initMtReq();
    m_msgFucs[app_com_device_res_exe_act_msg_type] = &CMaintain::_procDwnRes;
    m_msgFucs[MSG_STAddLiquid] = &CMaintain::_procAddLiquid;
    m_msgFucs[app_maintain_msg_type] = &CMaintain::_procMtReq;
    m_msgFucs[vos_pidmsgtype_timer_timeout] = &CMaintain::_procTimeOut;
    m_msgFucs[app_req_stop_midmach_msg_type] = &CMaintain::_procStopMtCMD;

    m_stateMng.actTimer = createTimer(Pid_Maintain, "Maintain_act",50);//动作超时 5s
    m_stateMng.nextActTimer = createTimer(Pid_Maintain, "Maintain_nextAct");
    m_exeFunc = NULL;
}

void CMaintain::_initMtReq()
{
    //请求与执行映射
    m_procMtReq[app_deion_pipe_flood_req] = &CMaintain::_exeReq_1;
    m_procMtReq[app_smp_pipe_flood_req] = &CMaintain::_exeReq_2;
    m_procMtReq[app_r1_pipe_flood_req] = &CMaintain::_exeReq_3;
    m_procMtReq[app_r2_pipe_flood_req] = &CMaintain::_exeReq_4;
    m_procMtReq[app_complete_drain_req] = &CMaintain::_exeReq_5;
    m_procMtReq[app_add_smponly_req] = &CMaintain::_exeReq_6;
    m_procMtReq[app_add_r1only_req] = &CMaintain::_exeReq_7;
    m_procMtReq[app_add_r2only_req] = &CMaintain::_exeReq_8;
    m_procMtReq[app_drain_smponly_req] = &CMaintain::_exeReq_9;
    m_procMtReq[app_drain_r1only_req] = &CMaintain::_exeReq_10;
    m_procMtReq[app_drain_r2only_req] = &CMaintain::_exeReq_11;
    m_procMtReq[app_drain_add_smp_req] = &CMaintain::_exeReq_12;
    m_procMtReq[app_drain_add_r1_req] = &CMaintain::_exeReq_13;
    m_procMtReq[app_drain_add_r2_req] = &CMaintain::_exeReq_14;
    m_procMtReq[app_auto_wash_test_2_req] = &CMaintain::_exeReq_15;
    m_procMtReq[app_auto_wash_test_3_req] = &CMaintain::_exeReq_16;
    m_procMtReq[app_auto_wash_test_4_req] = &CMaintain::_exeReq_17;
    m_procMtReq[app_auto_wash_test_6_req] = &CMaintain::_exeReq_18;
    m_procMtReq[app_auto_wash_test_71_req] = &CMaintain::_exeReq_19;
    m_procMtReq[app_auto_wash_test_72_req] = &CMaintain::_exeReq_20;
    m_procMtReq[app_auto_wash_close_allPV_req] = &CMaintain::_exeReq_21;
    m_procMtReq[app_accuracyAddS] = &CMaintain::_exeReq_22; 
    m_procMtReq[app_accuracyAddR1] = &CMaintain::_exeReq_23;
    m_procMtReq[app_accuracyAddR2] = &CMaintain::_exeReq_24;
    m_procMtReq[app_accuracyRMix] = &CMaintain::_exeReq_25;
    m_procMtReq[app_accuracySMix] = &CMaintain::_exeReq_26;

    m_procMtReq[app_smpb_pipe_flood_req] = &CMaintain::_exeReq_27;
    m_procMtReq[app_r1b_pipe_flood_req] = &CMaintain::_exeReq_28;
    m_procMtReq[app_r2b_pipe_flood_req] = &CMaintain::_exeReq_29;
    m_procMtReq[app_drain_add_req] = &CMaintain::_exeReq_30;
    m_procMtReq[app_add_req] = &CMaintain::_exeReq_31;
    m_procMtReq[app_drain_req] = &CMaintain::_exeReq_32;
    m_procMtReq[app_accuracyAdd] = &CMaintain::_exeReq_33;
}

vos_u32 CMaintain::CallBack(vos_msg_header_stru* pMsg)
{
    map<vos_u32, msgFucs>::iterator iter = m_msgFucs.find(pMsg->usmsgtype);
    if (iter != m_msgFucs.end())
    {
        (this->*(iter->second))(pMsg);
    }
    return VOS_OK;
}

vos_u32 CMaintain::_procMtReq(vos_msg_header_stru* pMsg)
{   
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STReqActCmd*  pReq = dynamic_cast<STReqActCmd*>(stMsg.pPara);

    m_exeFunc = NULL;
    map<vos_u32, exeReq>::iterator iter = m_procMtReq.find(pReq->uiparams(0));
    if (iter != m_procMtReq.end())
    {
        m_exeFunc = iter->second;
    }
    if (m_stateMng.curtstate == app_state_idll&&m_exeFunc != NULL)
    {
        (this->*m_exeFunc)(app_state_idll, (void *)(pReq));
        return VOS_OK;
    }
    WRITE_ERR_LOG("ERROR! Maintain State(%d),commandid(%d)!\n", m_stateMng.curtstate, pReq->uiparams(0));
    return VOS_OK;
}

vos_u32 CMaintain::_procDwnRes(vos_msg_header_stru* pMsg)
{
    app_com_dev_act_result_stru * pDwnRes = (app_com_dev_act_result_stru *)pMsg->aucmsgload;

    if (pDwnRes->enactrslt != app_action_res_success)
    {
        WRITE_ERR_LOG("Act(%#x) failed, state(%d)!", pDwnRes->enacttype,m_stateMng.curtstate)
        m_stateMng.curtstate = app_state_idll;
        app_reportmidmachstatus(vos_midmachine_state_midmach_err, SPid_Maintain);
        return VOS_OK;
    }

    if (++m_stateMng.ucovercmd == m_stateMng.ucsumcmdnum)
    {
        vos_stoptimer(m_stateMng.actTimer);         //发送的命令都返回了，停止命令执行定时器
        if (m_stateMng.curtstate != app_state_idll)//不是idll态，开启进入下一个状态的定时器
        {
            return startTimer(m_stateMng.nextActTimer, m_stateMng.timeLen);
        } 
    }
    return VOS_OK;
}

vos_u32 CMaintain::_procTimeOut(vos_msg_header_stru* pMsg)
{
    vos_timeroutmsg_stru* pTimeMsg = (vos_timeroutmsg_stru*)pMsg;

    if (pTimeMsg->ultimerid == m_stateMng.nextActTimer 
        && m_stateMng.ucovercmd == m_stateMng.ucsumcmdnum)
    {
        if (m_stateMng.curtstate == app_state_idll)
        {
            app_reportmidmachstatus(vos_midmachine_state_standby, SPid_Maintain);
        }
        (this->*m_exeFunc)(++m_stateMng.curtstate, NULL);//执行下一个动作
    }
    else if (pTimeMsg->ultimerid == m_stateMng.actTimer)
    {
        app_reportmidmachstatus(vos_midmachine_state_midmach_err, SPid_Maintain);
        WRITE_ERR_LOG("Act Time out(%d)!\n", m_stateMng.curtstate);
        m_stateMng.curtstate = app_state_idll;//停止动作
    }
    else
    {
        WRITE_ERR_LOG("unknown timerid \n");
    }
    return VOS_OK;
}
vos_u32 CMaintain::_procStopMtCMD(vos_msg_header_stru* pMsg)
{
    m_stateMng.curtstate = app_state_idll;
    vos_stoptimer(m_stateMng.actTimer);
    vos_stoptimer(m_stateMng.nextActTimer);
    app_reportmidmachstatus(vos_midmachine_state_standby, Pid_Maintain);
    return VOS_OK;
}

vos_u32 CMaintain::_procAddLiquid(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STReqActCmd*  stLiquid = dynamic_cast<STReqActCmd*>(stMsg.pPara);

    m_AddInfo.addvolume = stLiquid->uiparams(1);
    m_AddInfo.addcount = (vos_i32)stLiquid->uiparams(2);

    m_exeFunc = NULL;
    if (stLiquid->uisubsysid() == SPid_SmpNdl)
    {
        m_exeFunc = m_procMtReq[app_accuracyAddS];
    }
    else if (stLiquid->uisubsysid() == SPid_R1)
    {
        m_exeFunc = m_procMtReq[app_accuracyAddR1];
    }
    else if (stLiquid->uisubsysid() == SPid_R2)
    {
        m_exeFunc = m_procMtReq[app_accuracyAddR2];
    }

    if (m_stateMng.curtstate == app_state_idll&&m_exeFunc != NULL)
    {
        INITP04(SPid_Maintain);
        OPEN_P04(SPid_Maintain);
        (this->*m_exeFunc)(app_state_idll, (void *)&m_AddInfo);
    }
    return VOS_OK;
}

void CMaintain::_exeReq_1(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_deion_pipeflood_open_p03:
    case app_state_idll:
        _configMtState(app_deion_pipeflood_open_p03, 1, 0, 600);
        _sendPvCtl(xc8002_dwnmach_name_autowash, p03, APP_OPEN);
        break;

    case app_deion_pipeflood_open_v10v11:
        _configMtState(app_deion_pipeflood_open_v10v11, 2, 0, 200);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_OPEN);
        break;

    case app_deion_pipeflood_close_v10v11:
        _configMtState(app_deion_pipeflood_close_v10v11, 3, 0, 20);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_autowash, v05, APP_OPEN);
        break;

    case app_deion_pipeflood_open_p04:
        _configMtState(app_deion_pipeflood_open_p04, 1, 0, 10);
        _sendPvCtl(xc8002_dwnmach_name_autowash, p04, APP_OPEN);
        break;

    case app_deion_pipeflood_close_v05:
        _configMtState(app_deion_pipeflood_close_v05, 1, 0, 0);
        _sendPvCtl(xc8002_dwnmach_name_autowash, v05, APP_CLOSE);
        break;

    case app_deion_pipeflood_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_2(vos_u32 state,void* info)
{
    switch (state)
    {
        //样本针管路灌注
    case app_smp_pipeflood_reset:
    case app_state_idll:
        _configMtState(app_smp_pipeflood_reset, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_SmpNdl, Act_Reset, 0, VOS_NULL, APP_YES);
        break;

    case app_smp_pipeflood_open_v02:
        _configMtState(app_smp_pipeflood_open_v02, 1, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_OPEN);
        break;

    case app_smp_pipeflood_open_v07:
        _configMtState(app_smp_pipeflood_open_v07, 2, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_OPEN);
        //吸满
        _sendPumpAct(cmd_motor_dir_IN, 80, SPid_SmpNdl);
        break;

    case app_smp_pipeflood_close_v02v07:
        _configMtState(app_smp_pipeflood_close_v02v07, 3, 0, 0);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_CLOSE);
        //排空
        _sendPumpAct(cmd_motor_dir_OUT, 100, SPid_SmpNdl);
        break;

    case app_smp_pipeflood_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_3(vos_u32 state,void* info)
{
    switch (state)
    {
        //R1单元管路灌注
    case app_r1_pipeflood_reset:
    case app_state_idll:
        _configMtState(app_r1_pipeflood_reset, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_R1, Act_Reset, 0, VOS_NULL, APP_YES);
        break;

    case app_r1_pipeflood_open_v03:
        _configMtState(app_r1_pipeflood_open_v03, 1, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_OPEN);
        break;

    case app_r1_pipeflood_open_v08:
        _configMtState(app_r1_pipeflood_open_v08, 2, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_OPEN);
        //吸满
        _sendPumpAct(cmd_motor_dir_IN, 900, SPid_R1);
        break;

    case app_r1_pipeflood_close_v03v08:
        _configMtState(app_r1_pipeflood_close_v03v08, 3, 0, 0);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_CLOSE);
        //排空
        _sendPumpAct(cmd_motor_dir_OUT, 1000, SPid_R1);
        break;

    case app_r1_pipeflood_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_4(vos_u32 state, void* info)
{
    switch (state)
    {
        //R2单元管路灌注
    case app_r2_pipeflood_reset:
    case app_state_idll:
        _configMtState(app_r2_pipeflood_reset, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_R2, Act_Reset, 0, VOS_NULL, APP_YES);
        break;

    case app_r2_pipeflood_open_v04:
        _configMtState(app_r2_pipeflood_open_v04, 1, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_OPEN);
        break;

    case app_r2_pipeflood_open_v09:
        _configMtState(app_r2_pipeflood_open_v09, 2, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_OPEN);
        //吸满
        _sendPumpAct(cmd_motor_dir_IN, 900, SPid_R2);
        break;

    case app_r2_pipeflood_close_v04v09:
        _configMtState(app_r2_pipeflood_close_v04v09, 3, 0, 0);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_CLOSE);
        //排空
        _sendPumpAct(cmd_motor_dir_OUT, 1000, SPid_R2);
        break;

    case app_r2_pipeflood_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_5(vos_u32 state, void* info)
{
    switch (state)
    {
        //整机排空
    case app_drain_closetempctl:
    case app_state_idll:
        _configMtState(app_drain_closetempctl, 2, 0, 0);
        _sendTempCtl(APP_CLOSE);
        _sendPrssCtl(APP_CLOSE);
        break;

    case app_drain_releaseprss:
        _configMtState(app_drain_releaseprss, 3, 0, 100);
        _sendPvCtl(p08, APP_CLOSE);
        _sendPvCtl(v23, APP_OPEN);
        _sendPvCtl(v25, APP_OPEN);
        break;

    case app_drain_emptydrain:
        _configMtState(app_drain_emptydrain, 2, 0, 100);
        _sendPvCtl(p07, APP_OPEN);
        _sendPvCtl(v27, APP_OPEN);
        //app_reqemptydrain();
        break;

    case app_drain_emptydrainover:
        _configMtState(app_drain_emptydrainover, 3, 0, 0);
        _sendPvCtl(p07, APP_CLOSE);
        _sendPvCtl(v25, APP_CLOSE);
        _sendPvCtl(v27, APP_CLOSE);
        break;

    case app_drain_closepx:
        _configMtState(app_drain_closepx, 4, 0, 0);
        _sendPvCtl(p03, APP_CLOSE);
        _sendPvCtl(p05, APP_CLOSE);
        _sendPvCtl(p06, APP_CLOSE);
        _sendPvCtl(p07, APP_CLOSE);
        break;
    case app_drain_openvx:
        _configMtState(app_drain_openvx, 17, 0, 0);
        _sendPvCtl(v01, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_OPEN);
        _sendPvCtl(v05, APP_OPEN);
        _sendPvCtl(v06, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_OPEN);
        _sendPvCtl(v14, APP_OPEN);
        _sendPvCtl(v15, APP_OPEN);
        _sendPvCtl(v16, APP_OPEN);
        _sendPvCtl(v17, APP_OPEN);
        _sendPvCtl(v18, APP_OPEN);
        _sendPvCtl(v19, APP_OPEN);
        break;

    case app_drain_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_6(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_addsmp_smpndl_abair:        //只吸调试样本   
        _configMtState(app_addsmp_smpndl_abair, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumeair, SPid_SmpNdl);
        break;

    case app_addsmp_smpndl_down:
        _configMtState(app_addsmp_smpndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, xc8002_dwnmach_name_smpndl);
        break;

    case app_addsmp_smpndl_absmp:
        _configMtState(app_addsmp_smpndl_absmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumesmp, SPid_SmpNdl);
        break;
    case app_addsmp_smpndl_up:
        _configMtState(app_addsmp_smpndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, xc8002_dwnmach_name_smpndl);
        break;
    case app_addsmp_smpndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_6(app_addsmp_smpndl_abair, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_7(vos_u32 state, void* info)
{
    switch (state)
    {
        //只吸调试R1
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_addsmp_r1ndl_abair: //R1针吸空气
        _configMtState(app_addsmp_r1ndl_abair, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumeair, SPid_R1);
        break;
    case app_addsmp_r1ndl_down:  //R1针下降
        _configMtState(app_addsmp_r1ndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, xc8002_dwnmach_name_r1Ndl);
        break;
    case app_addsmp_r1ndl_absmp: //R1针吸样
        _configMtState(app_addsmp_r1ndl_absmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumesmp, SPid_R1);
        break;
    case app_addsmp_r1ndl_up:    //R1针上升
        _configMtState(app_addsmp_r1ndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, xc8002_dwnmach_name_r1Ndl);
        break;
    case app_addsmp_r1ndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_7(app_addsmp_r1ndl_abair, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_8(vos_u32 state, void* info)
{
    switch (state)
    {
        //只吸调试R2
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_addsmp_r2ndl_abair: //R2针吸空气
        _configMtState(app_addsmp_r2ndl_abair, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumeair, SPid_R2);
        break;
    case app_addsmp_r2ndl_down:  //R2针下降
        _configMtState(app_addsmp_r2ndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, xc8002_dwnmach_name_r2Ndl);
        break;
    case app_addsmp_r2ndl_absmp: //R2针吸样
        _configMtState(app_addsmp_r2ndl_absmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumesmp, SPid_R2);
        break;
    case app_addsmp_r2ndl_up:    //R2针上升
        _configMtState(app_addsmp_r2ndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, xc8002_dwnmach_name_r2Ndl);
        break;
    case app_addsmp_r2ndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_8(app_addsmp_r2ndl_abair, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_9(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_drainsmp_smpndl_down: //样本针下降
        _configMtState(app_drainsmp_smpndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, xc8002_dwnmach_name_smpndl);
        break;
    case app_drainsmp_smpndl_psmp:  //样本针排样
        _configMtState(app_drainsmp_smpndl_psmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT, m_AddOnlyInfo.volumesmp, SPid_SmpNdl);
        break;
    case app_drainsmp_smpndl_up:    //样本针上升
        _configMtState(app_addsmp_smpndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, xc8002_dwnmach_name_smpndl);
    case app_drainsmp_smpndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_9(app_drainsmp_smpndl_down, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_10(vos_u32 state, void* info)
{
    switch (state)
    {
        //只排调试R1
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_drainsmp_r1ndl_down:  //R1针下降
        _configMtState(app_drainsmp_r1ndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, xc8002_dwnmach_name_r1Ndl);
        break;
    case app_drainsmp_r1ndl_psmp: //R1针排样
        _configMtState(app_drainsmp_r1ndl_psmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT, m_AddOnlyInfo.volumesmp, SPid_R1);
        break;
    case app_drainsmp_r1ndl_up:    //R1针上升
        _configMtState(app_drainsmp_r1ndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, xc8002_dwnmach_name_r1Ndl);
        break;
    case app_drainsmp_r1ndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_10(app_drainsmp_r1ndl_down, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_11(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_drainsmp_r2ndl_down:  //R1针下降
        _configMtState(app_drainsmp_r2ndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, xc8002_dwnmach_name_r2Ndl);
        break;
    case app_drainsmp_r2ndl_psmp: //R1针排样
        _configMtState(app_drainsmp_r2ndl_psmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT, m_AddOnlyInfo.volumesmp, SPid_R2);
        break;
    case app_drainsmp_r2ndl_up:    //R1针上升
        _configMtState(app_drainsmp_r2ndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, xc8002_dwnmach_name_r2Ndl);
        break;
    case app_drainsmp_r2ndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_11(app_drainsmp_r2ndl_down, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_12(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOrDrainInfo((STReqActCmd *)info))
        {
            return;
        }
    case app_addordrain_smp_add:
        _configMtState(app_addordrain_smp_add, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumemore, SPid_SmpNdl);
        break;
    case app_addordrain_smp_drain:
        _configMtState(app_addordrain_smp_drain, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT,m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumeless, SPid_SmpNdl);
        break;
    case app_addordrain_smp_success:
        if (m_AddOrDrainInfo.addcount > 0)
        {
            if (0 < --m_AddOrDrainInfo.addcount)
            {
                _exeReq_12(app_addordrain_smp_add, NULL);
                break;
            }  
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_13(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOrDrainInfo((STReqActCmd *)info))
        {
            return;
        }
    case app_addordrain_r1_add:
        _configMtState(app_addordrain_r1_add, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumemore, SPid_R1);
        break;
    case app_addordrain_r1_drain:
        _configMtState(app_addordrain_r1_drain, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT,m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumeless, SPid_R1);
        break;
    case app_addordrain_r1_success:
        if (m_AddOrDrainInfo.addcount > 0)
        {
            if (0 < --m_AddOrDrainInfo.addcount)
            {
                _exeReq_13(app_addordrain_r1_add, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_14(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOrDrainInfo((STReqActCmd *)info))
        {
            return;
        }
    case app_addordrain_r2_add:
        _configMtState(app_addordrain_r2_add, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumemore, SPid_R2);
        break;
    case app_addordrain_r2_drain:
        _configMtState(app_addordrain_r2_drain, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT,m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumeless, SPid_R2);
        break;
    case app_addordrain_r2_success:
        if (m_AddOrDrainInfo.addcount > 0)
        {
            if (0 < --m_AddOrDrainInfo.addcount)
            {
                _exeReq_14(app_addordrain_r2_add, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_15(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_outer_wash_open_p03_p07_v07_v08_v09:
    case app_state_idll:
        _configMtState(app_outer_wash_open_p03_p07_v07_v08_v09, 5, 0, 0);
        _sendPvCtl(p03, APP_OPEN);
        _sendPvCtl(p07, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_OPEN);
        break;

    case app_outer_wash_open_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_16(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_stir_open_p03_p07_v10_v11:
    case app_state_idll:
        _configMtState(app_stir_open_p03_p07_v10_v11, 4, 0, 0);
        _sendPvCtl(p03, APP_OPEN);
        _sendPvCtl(p07, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_OPEN);
        break;

    case app_stir_open_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_17(vos_u32 state, void* info)
{
    switch (state)
    {
        // 4
    case app_inner_wash_open_p03_p07_v05:
    case app_state_idll:
        _configMtState(app_inner_wash_open_p03_p07_v05, 3, 0, 2);
        _sendPvCtl(p03, APP_OPEN);
        _sendPvCtl(p07, APP_OPEN);
        _sendPvCtl(v05, APP_OPEN);
        break;

    case app_inner_wash_open_p04:
        _configMtState(app_inner_wash_open_p04, 1, 0, 2);
        _sendPvCtl(p04, APP_OPEN);
        break;

    case app_inner_wash_close_v05_open_v02_v03_v04:
        _configMtState(app_inner_wash_close_v05_open_v02_v03_v04, 4, 0, 0);
        _sendPvCtl(v05, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_OPEN);
        break;

    case app_inner_wash_success:
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_18(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_autowash_flood_open_p03_p06_p07_p08_v05:
    case app_state_idll:
        _configMtState(app_autowash_flood_open_p03_p06_p07_p08_v05, 5, 0, 20);
        _sendPvCtl(p03, APP_OPEN);
        _sendPvCtl(p06, APP_OPEN);
        _sendPvCtl(p07, APP_OPEN);
        _sendPvCtl(p08, APP_OPEN);
        _sendPvCtl(v05, APP_OPEN);
        break;

    case app_autowash_flood_open_p04:
        _configMtState(app_autowash_flood_open_p04, 1, 0, 2);
        _sendPvCtl(p04, APP_OPEN);
        break;

    case app_autowash_flood_close_v05_open_v02_to_v19:
        _configMtState(app_autowash_flood_close_v05_open_v02_to_v19, 14, 0, 0);
        _sendPvCtl(v05, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_OPEN);

        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_OPEN);
        _sendPvCtl(v14, APP_OPEN);
        _sendPvCtl(v15, APP_OPEN);
        _sendPvCtl(v16, APP_OPEN);
        _sendPvCtl(v17, APP_OPEN);
        _sendPvCtl(v18, APP_OPEN);
        _sendPvCtl(v19, APP_OPEN);
        break;

    case app_autowash_flood_success:
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_19(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_autowash_timing1_open_p03_p06_p07_p08:
    case app_state_idll:
        _configMtState(app_autowash_timing1_open_p03_p06_p07_p08, 4, 0, 0);
        _sendPvCtl(p03, APP_OPEN);
        _sendPvCtl(p06, APP_OPEN);
        _sendPvCtl(p07, APP_OPEN);
        _sendPvCtl(p08, APP_OPEN);
        break;

    case app_autowash_timing1_success:
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_20(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_autowash_timing_open_v23:
    case app_state_idll:
        _configMtState(app_autowash_timing_open_v23, 1, 0, 10);
        _sendPvCtl(v23, APP_OPEN);
        break;

    case app_autowash_timing_open_v14_v15_v16_v17_v18_v19:
        _configMtState(app_autowash_timing_open_v14_v15_v16_v17_v18_v19, 6, 0, 9);
        _sendPvCtl(v14, APP_OPEN);
        _sendPvCtl(v15, APP_OPEN);
        _sendPvCtl(v16, APP_OPEN);
        _sendPvCtl(v17, APP_OPEN);
        _sendPvCtl(v18, APP_OPEN);
        _sendPvCtl(v19, APP_OPEN);
        break;

    case app_autowash_timing_open_v10_v11:
        _configMtState(app_autowash_timing_open_v10_v11, 2, 0, 1);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_OPEN);
        break;

    case app_autowash_timing_close_v14_v15_v16_v17_v18_v19:
        _configMtState(app_autowash_timing_close_v14_v15_v16_v17_v18_v19, 6, 0, 1);
        _sendPvCtl(v14, APP_CLOSE);
        _sendPvCtl(v15, APP_CLOSE);
        _sendPvCtl(v16, APP_CLOSE);
        _sendPvCtl(v17, APP_CLOSE);
        _sendPvCtl(v18, APP_CLOSE);
        _sendPvCtl(v19, APP_CLOSE);
        break;

    case app_autowash_timing_close_v23_open_v05:
        _configMtState(app_autowash_timing_close_v23_open_v05, 2, 0, 2);
        _sendPvCtl(v23, APP_CLOSE);
        _sendPvCtl(v05, APP_OPEN);
        break;

    case app_autowash_timing_open_p04:
        _configMtState(app_autowash_timing_open_p04, 1, 0, 2);
        _sendPvCtl(p04, APP_OPEN);
        break;

    case app_autowash_timing_close_v05_open_v21_v22_v25:
        _configMtState(app_autowash_timing_close_v05_open_v21_v22_v25, 4, 0, 1);
        _sendPvCtl(v05, APP_CLOSE);

#ifdef Edition_A
        _sendPvCtl(v21, APP_OPEN);
        _sendPvCtl(v22, APP_OPEN);
#else
        _sendPvCtl(v21, APP_CLOSE);
        _sendPvCtl(v22, APP_CLOSE);
#endif
        _sendPvCtl(v25, APP_OPEN);
        break;

    case app_autowash_timing_open_v04_v09:
        _configMtState(app_autowash_timing_open_v04_v09, 2, 0, 1);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_OPEN);
        break;

    case app_autowash_timing_open_v03_v08:
        _configMtState(app_autowash_timing_open_v03_v08, 2, 0, 1);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_OPEN);
        break;

    case app_autowash_timing_open_v02_v07:
        _configMtState(app_autowash_timing_open_v02_v07, 2, 0, 4);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_OPEN);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_OPEN);
        break;

    case app_autowash_timing_close_v10_v11:
        _configMtState(app_autowash_timing_close_v10_v11, 2, 0, 1);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_CLOSE);
        break;

    case app_autowash_timing_close_v04:
        _configMtState(app_autowash_timing_close_v04, 1, 0, 1);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_CLOSE);
        break;

    case app_autowash_timing_close_v03:
        _configMtState(app_autowash_timing_close_v03, 1, 0, 1);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_CLOSE);
        break;

    case app_autowash_timing_close_v02_v09:
        _configMtState(app_autowash_timing_close_v02_v09, 2, 0, 1);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_CLOSE);
        break;

    case app_autowash_timing_close_v07_v08:
        _configMtState(app_autowash_timing_close_v07_v08, 2, 0, 4);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_CLOSE);
        break;

    case app_autowash_timing_close_p04_v21_v22_v25:
        _configMtState(app_autowash_timing_close_p04_v21_v22_v25, 4, 0, 0);
        _sendPvCtl(p04, APP_CLOSE);
        _sendPvCtl(v25, APP_CLOSE);
#ifdef Edition_A
        _sendPvCtl(v21, APP_CLOSE);
        _sendPvCtl(v22, APP_CLOSE);
#else
        _sendPvCtl(v21, APP_OPEN);
        _sendPvCtl(v22, APP_OPEN);
#endif
        break;

    case app_autowash_timing_success:
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_21(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_autowash_close_allPV:
    case app_state_idll:
        _configMtState(app_autowash_close_allPV, 31, 0, 0);
        _sendPvCtl(p07, APP_CLOSE);
        _sendPvCtl(p08, APP_CLOSE);
        _sendPvCtl(p09, APP_CLOSE);
        _sendPvCtl(p19, APP_CLOSE);
        _sendPvCtl(v01, APP_CLOSE);
        //_sendPvCtl(v05, APP_CLOSE);
        _sendPvCtl(v14, APP_CLOSE);
        _sendPvCtl(v15, APP_CLOSE);
        _sendPvCtl(v16, APP_CLOSE);
        _sendPvCtl(v17, APP_CLOSE);
        _sendPvCtl(v18, APP_CLOSE);
        _sendPvCtl(v19, APP_CLOSE);
        _sendPvCtl(v21, APP_CLOSE);
        _sendPvCtl(v22, APP_CLOSE);
        _sendPvCtl(v23, APP_CLOSE);
        //_sendPvCtl(v25, APP_CLOSE);
        _sendPvCtl(v27, APP_CLOSE);
        _sendPvCtl(v28, APP_CLOSE);
        _sendPvCtl(p04, APP_CLOSE);
        _sendPvCtl(p011, APP_CLOSE);   // p11
        _sendPvCtl(p06, APP_CLOSE);
        _sendPvCtl(p05, APP_CLOSE);
        _sendPvCtl(p03, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_smpndl, v02, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v03, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v04, APP_CLOSE);

        _sendPvCtl(xc8002_dwnmach_name_smpndl, v07, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r1Ndl, v08, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r2Ndl, v09, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_smpl_mixrod, v10, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_reag_mixrod, v11, APP_CLOSE);
        break;

    case app_autowash_close_allPV_success:
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_22(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        //微量加样S
    case app_accuracyAddS_rtt:
        _configMtState(app_accuracyAddS_rtt, 1, 0, 0);
        _sendNdlPreact(SPid_SmpNdl, Act_Move);
        break;
    case app_accuracyAddS_add:
        _configMtState(app_accuracyAddS_add, 1, 0, 0);
         _sendNdlExeact(SPid_SmpNdl);
        break;
    case app_accuracyAddS_wash:
        _configMtState(app_accuracyAddS_wash, 1, 0, 0);
        _sendNdlPreact(SPid_SmpNdl, Act_Wash);
        break;
    case app_accuracyAddS_success:
        if (--m_AddInfo.addcount > 0)
        {
            return _exeReq_22(app_accuracyAddS_rtt,NULL);
        }
        CLOSE_P04(SPid_Maintain);
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_23(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
        //微量加样R1
    case app_accuracyAddR1_rtt:
        _configMtState(app_accuracyAddR1_rtt, 1, 0, 0);
        _sendNdlPreact(SPid_R1, Act_Move);
        break;
    case app_accuracyAddR1_add:
        _configMtState(app_accuracyAddR1_add, 1, 0, 0);
        _sendNdlExeact(SPid_R1);
        break;
    case app_accuracyAddR1_wash:
        _configMtState(app_accuracyAddR1_wash, 1, 0, 0);
        _sendNdlPreact(SPid_R1, Act_Wash);
        break;
    case app_accuracyAddR1_success:
        if (--m_AddInfo.addcount > 0)
        {
            return _exeReq_23(app_accuracyAddR1_rtt, NULL);      
        }
        CLOSE_P04(SPid_Maintain);
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_24(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:

        //微量加样R2
    case app_accuracyAddR2_rtt:
        _configMtState(app_accuracyAddR2_rtt, 1, 0, 0);
        _sendNdlPreact(SPid_R2, Act_Move);
        break;
    case app_accuracyAddR2_add:
        _configMtState(app_accuracyAddR2_add, 1, 0, 0);
        _sendNdlExeact(SPid_R2);
        break;
    case app_accuracyAddR2_wash:
        _configMtState(app_accuracyAddR2_wash, 1, 0, 0);
        _sendNdlPreact(SPid_R2, Act_Wash);
        break;
    case app_accuracyAddR2_success:
        if (--m_AddInfo.addcount > 0)
        {
            return _exeReq_24(app_accuracyAddR2_rtt, NULL);
        }
        CLOSE_P04(SPid_Maintain);
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_25(vos_u32 state, void* info)
{
    switch (state)
    {
    case app_state_idll:
    {
        STReqActCmd* reqInfo = (STReqActCmd*)info;
        m_MixInfo.mixCount = reqInfo->uiparams(2);
    }
    case app_accuracySMix_rtt:
        _configMtState(app_accuracySMix_rtt, 1, 0, 0);
        msg_stNdlRtt stPara;
        stPara.usDstPos = app_reactsys_mix_ndl_mixing_38pos;
        app_reqDwmCmd(SPid_Maintain, SPid_SMix, Act_Move, sizeof(msg_stNdlRtt), (char*)&stPara, APP_YES);
        break;
    case app_accuracySMix_mix:
        _configMtState(app_accuracySMix_mix, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_SMix, Act_Mix, 0, NULL, APP_YES);
        break;
    case app_accuracySMix_wash:
        _configMtState(app_accuracySMix_wash, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_SMix, Act_Wash, 0, NULL, APP_YES);
        break;
    case app_accuracySMix_success:
        if (--m_MixInfo.mixCount > 0)
        {
            return _exeReq_25(app_accuracySMix_rtt, NULL);
        }
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}


void CMaintain::_exeReq_26(vos_u32 state, void*info)
{
    switch (state)
    {
    case app_state_idll:
    {
        STReqActCmd* reqInfo = (STReqActCmd*)info;
        m_MixInfo.mixCount = reqInfo->uiparams(2);
    }
    case app_accuracyRMix_rtt:
        _configMtState(app_accuracyRMix_rtt, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_R2Mix, Act_Move, 0, NULL, APP_YES);
        break;
    case app_accuracyRMix_mix:
        _configMtState(app_accuracyRMix_mix, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_R2Mix, Act_Mix, 0, NULL, APP_YES);
        break;
    case app_accuracyRMix_wash:
        _configMtState(app_accuracyRMix_wash, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_R2Mix, Act_Wash, 0, NULL, APP_YES);
        break;
    case app_accuracyRMix_success:
        if (--m_MixInfo.mixCount > 0)
        {
            return _exeReq_25(app_accuracyRMix_rtt, NULL);
        }
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_27(vos_u32 state, void* info)
{
    switch (state)
    {
        //样本针s2管路灌注
    case app_smpb_pipeflood_reset:
    case app_state_idll:
        _configMtState(app_smpb_pipeflood_reset, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_SmpNdlb, Act_Reset, 0, VOS_NULL, APP_YES);
        break;

    case app_smpb_pipeflood_open_v02:
        _configMtState(app_smpb_pipeflood_open_v02, 1, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_smpndlb, v31, APP_OPEN);
        break;

    case app_smpb_pipeflood_open_v07:
        _configMtState(app_smpb_pipeflood_open_v07, 2, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_smpndlb, v32, APP_OPEN);
        //吸满
        _sendPumpAct(cmd_motor_dir_IN, 80, SPid_SmpNdl);
        break;

    case app_smpb_pipeflood_close_v02v07:
        _configMtState(app_smpb_pipeflood_close_v02v07, 3, 0, 0);
        _sendPvCtl(xc8002_dwnmach_name_smpndlb, v31, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_smpndlb, v32, APP_CLOSE);
        //排空
        _sendPumpAct(cmd_motor_dir_OUT, 100, SPid_SmpNdlb);
        break;

    case app_smp_pipeflood_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_28(vos_u32 state, void* info)
{
    switch (state)
    {
        //R12单元管路灌注
    case app_r12_pipeflood_reset:
    case app_state_idll:
        _configMtState(app_r12_pipeflood_reset, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_R1b, Act_Reset, 0, VOS_NULL, APP_YES);
        break;

    case app_r12_pipeflood_open_v03:
        _configMtState(app_r12_pipeflood_open_v03, 1, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r1bNdl, v35, APP_OPEN);
        break;

    case app_r12_pipeflood_open_v08:
        _configMtState(app_r12_pipeflood_open_v08, 2, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r1bNdl, v34, APP_OPEN);
        //吸满
        _sendPumpAct(cmd_motor_dir_IN, 900, SPid_R1b);
        break;

    case app_r12_pipeflood_close_v03v08:
        _configMtState(app_r12_pipeflood_close_v03v08, 3, 0, 0);
        _sendPvCtl(xc8002_dwnmach_name_r1bNdl, v34, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r1bNdl, v35, APP_CLOSE);
        //排空
        _sendPumpAct(cmd_motor_dir_OUT, 1000, SPid_R1b);
        break;

    case app_r12_pipeflood_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_29(vos_u32 state, void* info)
{
    switch (state)
    {
        //R22单元管路灌注
    case app_r22_pipeflood_reset:
    case app_state_idll:
        _configMtState(app_r22_pipeflood_reset, 1, 0, 0);
        app_reqDwmCmd(SPid_Maintain, SPid_R2b, Act_Reset, 0, VOS_NULL, APP_YES);
        break;

    case app_r22_pipeflood_open_v04:
        _configMtState(app_r22_pipeflood_open_v04, 1, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r2bNdl, v37, APP_OPEN);
        break;

    case app_r22_pipeflood_open_v09:
        _configMtState(app_r22_pipeflood_open_v09, 2, 0, 50);
        _sendPvCtl(xc8002_dwnmach_name_r2bNdl, v38, APP_OPEN);
        //吸满
        _sendPumpAct(cmd_motor_dir_IN, 900, SPid_R2b);
        break;

    case app_r22_pipeflood_close_v04v09:
        _configMtState(app_r22_pipeflood_close_v04v09, 3, 0, 0);
        _sendPvCtl(xc8002_dwnmach_name_r2bNdl, v37, APP_CLOSE);
        _sendPvCtl(xc8002_dwnmach_name_r2bNdl, v38, APP_CLOSE);
        //排空
        _sendPumpAct(cmd_motor_dir_OUT, 1000, SPid_R2b);
        break;

    case app_r22_pipeflood_success:
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_30(vos_u32 state, void* info)
{
    if (info != NULL)
    {
        addsys = ((STReqActCmd*)info)->uiparams(5);
    }
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOrDrainInfo((STReqActCmd *)info))
        {
            return;
        }
    case app_addordrain_smp_add:
        _configMtState(app_addordrain_smp_add, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumemore, addsys);
        break;
    case app_addordrain_smp_drain:
        _configMtState(app_addordrain_smp_drain, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT, m_AddOrDrainInfo.volumesmp + m_AddOrDrainInfo.volumeless, addsys);
        break;
    case app_addordrain_smp_success:
        if (m_AddOrDrainInfo.addcount > 0)
        {
            if (0 < --m_AddOrDrainInfo.addcount)
            {
                _exeReq_30(app_addordrain_smp_add, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_31(vos_u32 state, void* info)
{
    if (info!=NULL)
    {
        addsys = ((STReqActCmd*)info)->uiparams(4);
    }
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_addsmp_smpndl_abair:        //只吸调试样本   
        _configMtState(app_addsmp_smpndl_abair, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumeair, addsys);
        break;

    case app_addsmp_smpndl_down:
        _configMtState(app_addsmp_smpndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, addsys-900);
        break;

    case app_addsmp_smpndl_absmp:
        _configMtState(app_addsmp_smpndl_absmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_IN, m_AddOnlyInfo.volumesmp, addsys);
        break;
    case app_addsmp_smpndl_up:
        _configMtState(app_addsmp_smpndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, addsys - 900);
        break;
    case app_addsmp_smpndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_31(app_addsmp_smpndl_abair, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_32(vos_u32 state, void* info)
{
    if (info != NULL)
    {
        addsys = ((STReqActCmd*)info)->uiparams(4);
    }
    switch (state)
    {
    case app_state_idll:
        if (VOS_OK != _configAddOnlyInfo((STReqActCmd*)info))
        {
            return;
        }
    case app_drainsmp_smpndl_down: //样本针下降
        _configMtState(app_drainsmp_smpndl_down, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_down, addsys-900);
        break;
    case app_drainsmp_smpndl_psmp:  //样本针排样
        _configMtState(app_drainsmp_smpndl_psmp, 1, 0, 1);
        _sendPumpAct(cmd_motor_dir_OUT, m_AddOnlyInfo.volumesmp, addsys);
        break;
    case app_drainsmp_smpndl_up:    //样本针上升
        _configMtState(app_addsmp_smpndl_up, 1, 0, 1);
        _sendNdlUpDown(cmd_motor_dir_up, addsys-900);
    case app_drainsmp_smpndl_success:
        if (m_AddOnlyInfo.addcount > 0)
        {
            if (0 < --m_AddOnlyInfo.addcount)
            {
                _exeReq_32(app_drainsmp_smpndl_down, NULL);
                break;
            }
        }
        _configMtState(app_state_idll, 0, 0, 0);
        return;
    }
    vos_starttimer(m_stateMng.actTimer);
}

void CMaintain::_exeReq_33(vos_u32 state, void* info)
{
    if (info!=NULL)
    {
        addsys = ((STReqActCmd*)info)->uiparams(1);
    }
    switch (state)
    {
    case app_state_idll:
        //微量加样S
    case app_accuracyAddS_rtt:
        _configMtState(app_accuracyAddS_rtt, 1, 0, 0);
        _sendNdlPreact(addsys, Act_Move);
        break;
    case app_accuracyAddS_add:
        _configMtState(app_accuracyAddS_add, 1, 0, 0);
        _sendNdlExeact(addsys);
        break;
    case app_accuracyAddS_wash:
        _configMtState(app_accuracyAddS_wash, 1, 0, 0);
        _sendNdlPreact(addsys, Act_Wash);
        break;
    case app_accuracyAddS_success:
        if (--m_AddInfo.addcount > 0)
        {
            return _exeReq_33(app_accuracyAddS_rtt, NULL);
        }
        CLOSE_P04(SPid_Maintain);
        return _configMtState(app_state_idll, 0, 0, 0);
    }
    vos_starttimer(m_stateMng.actTimer);
}

vos_u32 CMaintain::_configAddOnlyInfo(STReqActCmd *AddOnlyInfo)
{
    if (AddOnlyInfo != NULL)
    {
        m_AddOnlyInfo.volumesmp = AddOnlyInfo->uiparams(1);
        m_AddOnlyInfo.addcount = AddOnlyInfo->uiparams(2);
        m_AddOnlyInfo.volumeair = AddOnlyInfo->uiparams(3);
        return VOS_OK;
    }
    return VOS_INVALID_U32;
}

vos_u32 CMaintain::_configAddOrDrainInfo(STReqActCmd *AddOrDrainInfo)
{
    if (AddOrDrainInfo !=NULL)
    {
        m_AddOrDrainInfo.addcount = AddOrDrainInfo->uiparams(4);
        m_AddOrDrainInfo.volumeless = AddOrDrainInfo->uiparams(3);
        m_AddOrDrainInfo.volumemore = AddOrDrainInfo->uiparams(2);
        m_AddOrDrainInfo.volumesmp = AddOrDrainInfo->uiparams(1);
        return VOS_OK;
    }
    return VOS_INVALID_U32;
}
vos_u32 CMaintain::_sendPvCtl(vos_u32 ensubsys, vos_u32 pvNum, vos_u32 operate)
{
    msg_PVCtrl pvCtrl;
    pvCtrl.operateType = operate;
    pvCtrl.PVNum = pvNum;
    return app_reqDwmCmd(SPid_Maintain, ensubsys + 900, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
}

vos_u32 CMaintain::_sendPvCtl(vos_u32 pvNum, vos_u32 operate)
{
    msg_PVCtrl pvCtrl;
    pvCtrl.operateType = operate;
    pvCtrl.PVNum = pvNum;
    return app_reqDwmCmd(SPid_Maintain, SPid_AutoWash, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
}

vos_u32 CMaintain::_sendNdlUpDown(vos_u32 dir, vos_u32 subsys)
{
    msg_stVrt ndlVrt = { 0 };
    ndlVrt.dir = dir;
    ndlVrt.marginFlag = 0;
    return app_reqDwmCmd(SPid_Maintain, subsys + 900, Act_NdlVrt, sizeof(ndlVrt), (char *)&ndlVrt, VOS_YES);
}

vos_u32 CMaintain::_sendPumpAct(vos_u32 dir, vos_u32 volume, vos_u32 ulDstPid)
{
    msg_PumpCtrl pumpAct;
    pumpAct.dir = dir;
    pumpAct.volume = volume;
    return app_reqDwmCmd(SPid_Maintain, ulDstPid, Act_PumpCtrl, sizeof(msg_PumpCtrl), (char *)&pumpAct, VOS_YES);
}

vos_u32 CMaintain::_sendPrssCtl(vos_u32 ctrl)
{
    msg_switch stSwitch;
    stSwitch.operation = ctrl;
    return app_reqDwmCmd(SPid_Maintain, SPid_AutoWash, Act_LiquidsCheck, sizeof(msg_switch), (char *)&stSwitch, VOS_YES);
}

vos_u32 CMaintain::_sendTempCtl(vos_u32 tempctl)
{
    msg_switch stSwitch;
    stSwitch.operation = tempctl;
    return app_reqDwmCmd(SPid_Maintain, SPid_CoolReag, Act_CoolingCtrl, sizeof(msg_switch), (char *)&stSwitch, VOS_YES);
}

vos_u32 CMaintain::_sendNdlPreact(vos_u32 subsys, vos_u32 acttype)
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq * pPara = VOS_NULL;
    msg_stComDevActInfo * pActInfo = VOS_NULL;

    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo);
    if (acttype == Act_Move)
    {
        ulParaLen += sizeof(msg_stNdlRtt);
    }

    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x)\n", ulResult);
        return ulResult;
    }
    pPara->usActNum = 1;
    pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    if (acttype == Act_Move)
    {
        pActInfo->ulDataLen += sizeof(msg_stNdlRtt);
        msg_stNdlRtt * pNdlRtt = (msg_stNdlRtt *)pActInfo->aucPara;
        if (SPid_SmpNdl ==subsys)
        {
            pNdlRtt->usDstPos = SmpPosAbsrb1;
        }
        else
        {
            pNdlRtt->usDstPos = app_reagent_ndl_stop_pos_absrbOut;
        }
    }
    app_setActHead(&pActInfo->stActHead, acttype, 0, 0, APP_YES);

    msg_stHead stHead;
    app_constructMsgHead(&stHead, subsys, SPid_Maintain, app_com_device_req_exe_act_msg_type);
    return app_sendMsg(&stHead, pPara, ulParaLen);

}

vos_u32 CMaintain::_sendNdlExeact(vos_u32 subsys)
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq * pPara = VOS_NULL;

    vos_u32 ulParaLen = 0;
    if (subsys == SPid_R1 || subsys == SPid_R2)
    {
        ulParaLen = sizeof(msg_stComDevActReq) + (2 * sizeof(msg_stComDevActInfo) + sizeof(msg_stReagSysAbsorb) + sizeof(msg_stReagSysDischarge));
    }
    else
    {
        ulParaLen = sizeof(msg_stComDevActReq) + 2 * sizeof(msg_stComDevActInfo) + sizeof(msg_stSampleSysAbsorb) + sizeof(msg_stSampleSysDischarge);
    }

    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x)", ulResult);
        return ulResult;
    }
    pPara->usActNum = 2;

    if (subsys == SPid_R1 || subsys == SPid_R2)
    {
        configReagInfo(pPara);
    }
    else
    {
        configSmpInfo(pPara);
    }

    msg_stHead stHead;
    app_constructMsgHead(&stHead, subsys, SPid_Maintain, app_com_device_req_exe_act_msg_type);
    return app_sendMsg(&stHead, pPara, ulParaLen);
}

void CMaintain::configSmpInfo(msg_stComDevActReq * pPara)
{
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stSampleSysAbsorb);
    app_setActHead(&pActInfo->stActHead, Act_Absorb, 0, 0, APP_NO);
    msg_stSampleSysAbsorb * pAbsorb = (msg_stSampleSysAbsorb *)pActInfo->aucPara;
    pAbsorb->enAbsrbType = 0;
    pAbsorb->reportMarginFlag = 0;
    pAbsorb->ulSampleVolume = m_AddInfo.addvolume;
    pAbsorb->diskId = 0;
    pAbsorb->cupId = 0;
    pAbsorb->reactCup = 0;

    pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(msg_stComDevActInfo) + sizeof(msg_stSampleSysAbsorb));
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stSampleSysDischarge);
    app_setActHead(&pActInfo->stActHead, Act_Discharge, 0, 0, APP_YES);
    msg_stSampleSysDischarge * pDischarge = (msg_stSampleSysDischarge *)pActInfo->aucPara;
    pDischarge->usDischPos = SmpPosDischNml;
    pDischarge->ulSampleVolume = m_AddInfo.addvolume;
    pDischarge->usTotalVol = 0; //杯总体积
    pDischarge->ulDlntVolume = 0;
}

void CMaintain::configReagInfo(msg_stComDevActReq * pPara)
{
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stReagSysAbsorb);
    app_setActHead(&pActInfo->stActHead, Act_Absorb, 0, 0, APP_NO);
    msg_stReagSysAbsorb * pAbsorb = (msg_stReagSysAbsorb *)pActInfo->aucPara;
    pAbsorb->usAbsrbPos = 0;
    pAbsorb->enAbsrbType = 0;
    pAbsorb->reportMarginFlag = 0;
    pAbsorb->ulVolume = m_AddInfo.addvolume;
    pAbsorb->diskId = 0;
    pAbsorb->cupId = 0;
    pAbsorb->reactCup = 0;

    pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(msg_stComDevActInfo) + sizeof(msg_stReagSysAbsorb));
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stReagSysDischarge);
    app_setActHead(&pActInfo->stActHead, Act_Discharge, 0, 0, APP_YES);
    msg_stReagSysDischarge * pDischarge = (msg_stReagSysDischarge *)pActInfo->aucPara;
    pDischarge->usDischPos = app_reagent_ndl_stop_pos_discharg;
    pDischarge->ulReagVolume = m_AddInfo.addvolume;
    pDischarge->ulDlntVolume = 0;
    pDischarge->usTotalVol = 0; //杯总体积
}

void CMaintain::_configMtState(app_pipe_flood_state_enum state, char sumcmdnum, char overcmd, vos_u32 timelen)
{
    m_stateMng.curtstate = state;
    m_stateMng.ucovercmd = overcmd;
    m_stateMng.ucsumcmdnum = sumcmdnum;
    m_stateMng.timeLen = timelen;
    if (state == app_state_idll)
    {
        app_reportmidmachstatus(vos_midmachine_state_standby, SPid_Maintain);
    }
}

