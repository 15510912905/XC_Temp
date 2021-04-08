#include "autoload.h"
#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include <fstream>
#include "vos_init.h"
#include <string>
#include <iomanip>
CAutoLoad* g_pAutoLoad = NULL;
CAutoLoad::CAutoLoad ():LoadSign(false), m_State(STATE_CAutoLoad_IDLE)
{
	m_LReagents.clear();
    queue<STReagInfo> q;
    swap(m_LReplaceReagents, q);
	swap(m_RReagents,q);
    m_stReagents.clear();
    m_load_instrance = 0;
    wheelset = false;
    _readfile_volumeStandard();
    _initActFuc();
}
CAutoLoad::~CAutoLoad()
{
	
}

void CAutoLoad::_initActFuc()
{
    m_mSubAct[SPid_Transport] = new CSubAct(SPid_Transport);
    m_mSubAct[SPid_VerticalGripper] = new CSubAct(SPid_VerticalGripper);
    m_mSubAct[SPid_L1] = new CSubAct(SPid_L1);
    m_mSubAct[SPid_L2] = new CSubAct(SPid_L2);
    m_mSubAct[SPid_R1] = new CSubAct(SPid_R1);
    m_mSubAct[SPid_R2] = new CSubAct(SPid_R2);

    m_mActFunc[SPid_Transport][Act_Reset] = (ActFun)&CAutoLoad::_singleAct;
    m_mActFunc[SPid_Transport][Act_Move] = (ActFun)&CAutoLoad::_transportMove;
    m_mActFunc[SPid_Transport][Act_TPMoveF] = (ActFun)&CAutoLoad::_transportMove;
    m_mActFunc[SPid_Transport][Act_TPMoveL] = (ActFun)&CAutoLoad::_transportMove;
    m_mActFunc[SPid_Transport][Act_TPMoveR] = (ActFun)&CAutoLoad::_transportMove;

    m_mActFunc[SPid_VerticalGripper][Act_Reset] = (ActFun)&CAutoLoad::_singleAct;
    m_mActFunc[SPid_VerticalGripper][Act_Move] = (ActFun)&CAutoLoad::_vgMove;
    m_mActFunc[SPid_VerticalGripper][Act_VGMoveF] = (ActFun)&CAutoLoad::_vgMove;
    m_mActFunc[SPid_VerticalGripper][Act_VGMoveL] = (ActFun)&CAutoLoad::_vgMove;
    m_mActFunc[SPid_VerticalGripper][Act_VGMoveR] = (ActFun)&CAutoLoad::_vgMove;

    m_mActFunc[SPid_L1][Act_Reset] = (ActFun)&CAutoLoad::_singleAct;
    m_mActFunc[SPid_L1][Act_Move] = (ActFun)&CAutoLoad::_L1Move;
    m_mActFunc[SPid_L1][Act_BcScan] = (ActFun)&CAutoLoad::_L1Scan;

    m_mActFunc[SPid_L2][Act_Reset] = (ActFun)&CAutoLoad::_singleAct;
    m_mActFunc[SPid_L2][Act_Move] = (ActFun)&CAutoLoad::_L2Move;
    m_mActFunc[SPid_L2][Act_BcScan] = (ActFun)&CAutoLoad::_L2Scan;

    m_mActFunc[SPid_R1][Act_Reset] = (ActFun)&CAutoLoad::_singleAct;
    m_mActFunc[SPid_R1][Act_Move] = (ActFun)&CAutoLoad::_R1Move;

    m_mActFunc[SPid_R2][Act_Reset] = (ActFun)&CAutoLoad::_singleAct;
    m_mActFunc[SPid_R2][Act_Move] = (ActFun)&CAutoLoad::_R2Move;
    g_pTaskMng = new CTaskMng();
}

vos_u32 CAutoLoad::CallBack(vos_msg_header_stru* pMsg)
{
	if(NULL==pMsg)
	{
		WRITE_ERR_LOG("Input parameter is null \n");
        return app_para_null_err;
	}
	vos_u32 ulResult = VOS_OK;
    WRITE_INFO_LOG("curpid=%u Recevied msg(0x%x) from pid(%u)\n", SPid_AutoLoad, pMsg->usmsgtype, pMsg->ulsrcpid);
    switch (pMsg->usmsgtype)
    {
		
	case app_com_device_res_exe_act_msg_type://公用仪器执行动作响应消息
        ulResult = _proResponse(pMsg);
        break;
    case app_req_stop_midmach_msg_type://中位机停止请求消息
        _procStopReq();
        break;
    case vos_pidmsgtype_timer_timeout://定时超时
        _procTimeOut(pMsg);
        break;
    case MSG_ReqDeleteAlarm://删除报警
        m_State = STATE_CAutoLoad_IDLE;
        break;
    case MSG_ReqReset:
        _onResetRes();
        break;
        /*case MSG_ResAddTask:
            _onTaskInfo(pMsg);
            break;*/
	case MSG_NotifyReagInfo:
		_updateReagent(pMsg);
		break;
	case MSG_ReqBarcodeScan:
		_procBcScanReq(pMsg);
		break;
    default:
        WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
        break;
    }
    return ulResult;
}
//整机复位
vos_u32 CAutoLoad::_onResetRes()
{
	m_State = STATE_CAutoLoad_LOAD;
    m_ulActIndex++;
    InsertAct(SPid_Transport, Act_Reset);
    InsertAct(SPid_VerticalGripper, Act_Reset);
    InsertAct(SPid_L1, Act_Reset);
    InsertAct(SPid_L2, Act_Reset);
    SendNextAct();
    return VOS_OK;
}

vos_u32 CAutoLoad::_procStopReq()
{
    return VOS_OK;
}

vos_u32 CAutoLoad::_procTimeOut(vos_msg_header_stru* pResMsg)
{
    m_State = STATE_CAutoLoad_ERROR;
    ResetAct();
    return VOS_OK;
}

vos_u32 CAutoLoad::_proResponse(vos_msg_header_stru* pResMsg)
{
    ProcResAct(pResMsg);
    //app_com_dev_act_result_stru* pMsg = (app_com_dev_act_result_stru*)pResMsg->aucmsgload;
    //if (pMsg->enactrslt == EN_AF_Failed)//失败处理 上报状态
    //{
    //    ResetAct();
    //    //m_State = STATE_CAutoLoad_ERROR;
    //    queue<STReagInfo> q;
    //    swap(m_LReplaceReagents, q);
    //    swap(m_RReagents, q);
    //    _reprotState(m_State);
    //    return EN_AF_Failed;
    //}
    if (IsFinish())
    {
        m_LReplaceReagents.pop();
        m_RReagents.pop();
        m_LReagents.erase(m_LReagents.find(nowLReagents.stcuppos().uidskid()*1000+nowLReagents.stcuppos().uicupid()%10));
        //R试剂盘的试剂信息不应删除，应更新后续更改
        //m_stReagents.erase(m_stReagents.find(nowRReagents.stcuppos().uidskid()*1000+nowRReagents.stcuppos().uicupid()));
        //m_stReagents.erase(m_stReagents.begin(), m_stReagents.end());
        //m_LReagents.erase(m_LReagents.begin(), m_LReagents.end());
        STReagChangeInfoList* pPara;
        STReagChangeInfo* changInfo = pPara->add_listinfo();
        changInfo->set_badd(true);
        changInfo->set_reagdisktype(ReagDisk_Default);
        changInfo->set_allocated_streaginfo(&nowRReagents);
        changInfo = pPara->add_listinfo();
        changInfo->set_badd(false);
        changInfo->set_reagdisktype(ReagDisk_AutoLoad);
        changInfo->set_allocated_streaginfo(&nowLReagents);
        sendUpMsg(0, SPid_Task, MSG_NotifyReagChange, pPara);
        wheelset=false;
        app_print(ps_autoload, "autoload end!\n");
        if (m_LReplaceReagents.size()!=0&&m_RReagents.size()!=0)
        { 
            startAutoLoad();
        }
        else{
            m_State = STATE_CAutoLoad_IDLE;
        }
    }
    return VOS_OK;
}

void CAutoLoad::_reprotState(app_u32 state)
{
    STNotifyMachineStatus stStatus;
    stStatus.set_uimachstatus(state);
    app_print(ps_taskmng, "AutoLoadStatus:%d\n", state);
    if (VOS_OK != sendUpMsg(CPid_Maintain, SPid_Task, MSG_AutoLoadStatus, &stStatus))
    {
        WRITE_ERR_LOG("call MSG_AutoLoadStatus failed\n");
    }
}

//复位
vos_u32 CAutoLoad::_singleAct(app_u32 subSys, app_u16 actType)
{
    return app_reqDwmCmd(SPid_Task, subSys, actType, 0, NULL, APP_YES);
}
 //定位
vos_u32 CAutoLoad::_vgMove(app_u32 subSys, app_u16 actType)
{ 
    act_autoload_move stpara = { 0 };
    if (m_load_instrance == INSTRANCE_R)
    {
        stpara.type = Action_Grap;
        stpara.place = Position_R;
    }
    else if (m_load_instrance == INSTRANCE_Free)
    {
        stpara.type = Action_Free;
        stpara.place = Position_F;
    }
    else if (m_load_instrance == INSTRANCE_L)
    {
        stpara.type = Action_Grap;
        stpara.place = Position_L;
    }
    else if (m_load_instrance == INSTRANCE_R1)
    {
        stpara.type = Action_Free;
        stpara.place = Position_R;
        m_load_instrance = 0;
    }
    return app_reqDwmCmd(SPid_Task, SPid_VerticalGripper, actType, sizeof(stpara), (char*)&stpara, APP_YES);
}
vos_u32 CAutoLoad::_L1Move(app_u32 subSys, app_u16 actType)
{
    act_autoload_move stpara = { 0 };
    stpara.place = nowLReagents.stcuppos().uicupid()%10;
    app_print(ps_autoload,"L1 : %d\n", stpara.place);
    return app_reqDwmCmd(SPid_Task, SPid_L1, actType, sizeof(stpara), (char*)&stpara, APP_YES);
}
vos_u32 CAutoLoad::_transportMove(app_u32 subSys, app_u16 actType)
{
    act_autoload_move stpara = { 0 };
    if (m_load_instrance == INSTRANCE_RESET || m_load_instrance == INSTRANCE_L)
    {
        stpara.place = (nowRReagents.stcuppos().uidskid() == 1) ? STATE_TranSport_R1 : STATE_TranSport_R2;
    }
    else if (m_load_instrance == INSTRANCE_R)
    {
        stpara.place = STATE_TranSport_UNLOAD;
    }
    else if (m_load_instrance == INSTRANCE_Free)
    {
        stpara.place = (nowLReagents.stcuppos().uidskid() == 1) ? STATE_TranSport_L1 : STATE_TranSport_L2;
    }
    m_load_instrance++;
    return app_reqDwmCmd(SPid_Task, SPid_Transport, actType, sizeof(stpara), (char*)&stpara, APP_YES);
}
vos_u32 CAutoLoad::_L2Move(app_u32 subSys, app_u16 actType)
{
    act_autoload_move stpara = { 0 };
    stpara.place = nowLReagents.stcuppos().uicupid();
    return app_reqDwmCmd(SPid_Task, SPid_L2, actType, sizeof(stpara), (char*)&stpara, APP_YES);
}
vos_u32 CAutoLoad::_R1Move(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt stpara = { 0 };
    stpara.usDstPos = nowRReagents.stcuppos().uicupid();
    return app_reqDwmCmd(SPid_Task, SPid_R1, actType, sizeof(stpara), (char*)&stpara, APP_YES);
}
vos_u32 CAutoLoad::_R2Move(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt stpara = { 0 };
    stpara.usDstPos = nowRReagents.stcuppos().uicupid();
    return app_reqDwmCmd(SPid_Task, SPid_R2, actType, sizeof(stpara), (char*)&stpara, APP_YES);
}
void CAutoLoad::startAutoLoad()
{
    if (m_LReplaceReagents.size()==0||m_RReagents.size()==0)
    {
        return;
    }
    app_print(ps_autoload, "CAutoLoad::start insertAct \n");
    m_State = STATE_CAutoLoad_LOAD;
    nowRReagents = m_RReagents.front();
    nowLReagents = m_LReplaceReagents.front();
    ////临时处理试剂信息
    //m_LReplaceReagents.pop();
    //m_RReagents.pop();
    //m_LReagents.erase(m_LReagents.find(nowLReagents.stcuppos().uidskid() * 1000 + nowLReagents.stcuppos().uicupid()%10));
    ////R试剂盘的试剂信息不应删除，应更新后续更改
    //m_stReagents.erase(m_stReagents.find(nowRReagents.stcuppos().uidskid() * 1000 + nowRReagents.stcuppos().uicupid()));

    m_ulActIndex++;
    vos_u32 pid = (nowLReagents.stcuppos().uidskid() == 1) ? SPid_L1 : SPid_L2;
    InsertAct(pid, Act_Move);
    InsertAct(SPid_Transport, Act_Move,pid,Act_Move,m_ulActIndex);
    //vos_u32 rpid = (nowRReagents.stcuppos().uidskid() == 1) ? SPid_R1 : SPid_R2;
    //InsertAct(rpid, Act_Move);
    InsertAct(SPid_VerticalGripper, Act_Move, SPid_Transport, Act_Move,m_ulActIndex);
    InsertAct(SPid_Transport, Act_TPMoveF, SPid_VerticalGripper, Act_Move, m_ulActIndex);
    InsertAct(SPid_VerticalGripper, Act_VGMoveF, SPid_Transport, Act_TPMoveF, m_ulActIndex);
    InsertAct(SPid_Transport, Act_TPMoveL, SPid_VerticalGripper, Act_VGMoveF, m_ulActIndex);
    InsertAct(SPid_VerticalGripper, Act_VGMoveL, SPid_Transport, Act_TPMoveL, m_ulActIndex);
    InsertAct(SPid_Transport, Act_TPMoveR, SPid_VerticalGripper, Act_VGMoveL, m_ulActIndex);
    InsertAct(SPid_VerticalGripper, Act_VGMoveR, SPid_Transport, Act_TPMoveR, m_ulActIndex);
    SendNextAct();
}

vos_u32 CAutoLoad::_procBcScanReq(vos_msg_header_stru * pReqMsg)
{
    return VOS_OK;
}

vos_u32 CAutoLoad::AddTaskInfo(CTask * pTask)
{  
    m_qTasks.push(pTask);
    //_compareReagent();
    return VOS_OK;
}

vos_u32 CAutoLoad::_updateReagent(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }
    STNotifyReagInfo*  pPara = dynamic_cast<STNotifyReagInfo*>(stMsg.pPara);
    assert(pPara);
    for (int i = 0; i < pPara->streaginfo_size(); ++i)
    {
        const STReagInfo & reagInfo = pPara->streaginfo(i);
        const STCupPos & cupPos = reagInfo.stcuppos();
        vos_u32 index = cupPos.uidskid() * 1000 + cupPos.uicupid();
        m_LReagents[index] = reagInfo;
    }
    return VOS_OK;
}
vos_u32 CAutoLoad::_L1Scan(msg_stComDevActInfo* pComAct)
{
    STActInfo* pActInfo=NULL;
    msg_stReagBcScanPara * pBcScan = (msg_stReagBcScanPara *)(pComAct->aucPara);
    msg_stReagdskRttToBcscan * pDwnCmd = (msg_stReagdskRttToBcscan *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = cmd_inner_reagentdisk_rtt_to_bcdscan;
    pDwnCmd->uccupnum = pBcScan->usCupId;
    return VOS_OK;
}
vos_u32 CAutoLoad::_L2Scan(msg_stComDevActInfo* pComAct)
{
    STActInfo* pActInfo=NULL;
    msg_stReagBcScanPara * pBcScan = (msg_stReagBcScanPara *)(pComAct->aucPara);
    msg_stReagdskRttToBcscan * pDwnCmd = (msg_stReagdskRttToBcscan *)&(pActInfo->stDwnCmd);
    pDwnCmd->cmd = cmd_inner_reagentdisk_rtt_to_bcdscan;
    pDwnCmd->uccupnum = pBcScan->usCupId;
    return VOS_OK;
}
vos_u32 CAutoLoad::InitReagent(CMsg& stMsg)
{
    STNotifyReagInfo*  pPara = dynamic_cast<STNotifyReagInfo*>(stMsg.pPara);
    assert(pPara);
    m_stThresholds.clear();
    m_stReagents.clear();
    for (int i = 0; i < pPara->stthreshold_size(); ++i)
    {
        const STThreshold& threshold = pPara->stthreshold(i);
        m_stThresholds[threshold.enbtype()] = threshold.uithreshold();
    }
    for (int i = 0; i < pPara->streaginfo_size(); ++i)
    {
        const STReagInfo & reagInfo = pPara->streaginfo(i);
        const STCupPos & cupPos = reagInfo.stcuppos();
        vos_u32 index = cupPos.uidskid() * 1000 + cupPos.uicupid();
        m_stReagents[index] = reagInfo;
        //王晖俊 临时增加的L盘试剂信息
        //m_LReagents[cupPos.uidskid() * 1000 + cupPos.uicupid()%10] = reagInfo;
        //app_print(ps_autoload, "L ReagentInfo dskid:%d cupid: %d depth:%d \n",cupPos.uidskid(),cupPos.uicupid(),reagInfo.uimargindepth());
    }
    for (int i = 0; i < pPara->stautoloadreaginfo_size();++i)
    {
        const STReagInfo & reagInfo = pPara->stautoloadreaginfo(i);
        const STCupPos & cupPos = reagInfo.stcuppos();
        vos_u32 index = cupPos.uidskid() * 1000 + cupPos.uicupid();
        m_LReagents[index] = reagInfo;
        //王晖俊 临时增加的L盘试剂信息
        app_print(ps_autoload, "L ReagentInfo dskid:%d cupid: %d depth:%d \n", cupPos.uidskid(), cupPos.uicupid(), reagInfo.uimargindepth());
    }
    app_print(ps_autoload,"CAutoLoad::InitReagent \n");
    //_compareReagent();
    return VOS_OK;
}

void CAutoLoad::_compareReagent()
{
    if (m_qTasks.size()==0||m_stReagents.size()==0)
    {
        return;
    }
    printf("CAutoLoad::compareReagent Task start\n");
    map<vos_u32, STReagInfo> comInfo;
    comInfo = m_stReagents;
    while (!m_qTasks.empty())
    {
        CTask* task = m_qTasks.front();
        m_qTasks.pop();
        if (task==NULL)
        {
            continue;
        }
        STLiquidInfo* taskReagent;
        int index = 0;
        do 
        {
            taskReagent = task->_getReagByIndex(index++);
            BottleType type = comInfo[taskReagent->stcuppos().uidskid() * 1000 + taskReagent->stcuppos().uicupid()].enbtype();
            map<vos_u32, float>* mi_volume = \
                (type == BT_standard) ? &standtrd_volume\
                : (type == BT_middle) ? &middle_volume\
                : (type == BT_micro) ? &micro_volume : 0;
            float subdepth = 0;
            float mmdepth = 0;
            for (map<vos_u32, float>::iterator viter = mi_volume->begin(); viter != mi_volume->end(); viter++)
            {
                if (viter->second < (taskReagent->uiaddvol() / 100000))
                {
                    subdepth = viter->second;
                    mmdepth = viter->first;
                    continue;
                }
                if (viter->second == (taskReagent->uiaddvol() / 100000))
                {
                    subdepth = viter->first;
                    break;
                }
                if (viter->second > (taskReagent->uiaddvol() / 100000))
                {
                    float sec = viter->second - taskReagent->uiaddvol() / 100000 > taskReagent->uiaddvol() / 100000 - \
                        subdepth ? mmdepth : viter->first;
                    subdepth = sec;
                    break;
                }
            }
            vos_u32 nowhigh=comInfo[taskReagent->stcuppos().uidskid() * 1000 + taskReagent->stcuppos().uicupid()].uimargindepth();
            comInfo[taskReagent->stcuppos().uidskid() * 1000 + taskReagent->stcuppos().uicupid()].set_uimargindepth(nowhigh - subdepth);
        } while (taskReagent==NULL);       
        for (map<vos_u32, STReagInfo>::iterator para = comInfo.begin(); para != comInfo.end(); para++)
        {
            if (para->second.uimargindepth()<=m_stThresholds[para->second.enbtype()])
            {
                for (map<vos_u32, STReagInfo>::iterator lpara = m_LReagents.begin(); lpara != m_LReagents.end(); lpara++)
                {
                    if (string(lpara->second.strreagname()) == string(para->second.strreagname()))
                    {
                        printf("CAutoLoad::m_LReagents  dsk %d\n",(int)lpara->first);
                        //m_RReagents.push(para->second);
                        //m_LReplaceReagents.push(lpara->second);
                        break;
                    }
                }
            }
        }
    }
    if (m_State == STATE_CAutoLoad_IDLE&&(!m_RReagents.empty())&&(!m_LReplaceReagents.empty()))
    {
        printf("CAutoLoad::compareReagent Task end!\n");
        startAutoLoad();
    }
}

void CAutoLoad::GetDebugLreagent(STNotifyReagInfo info)
{
    STNotifyReagInfo  pPara = info;
    for (int i = 0; i < pPara.streaginfo_size(); ++i)
    {
        const STReagInfo & reagInfo = pPara.streaginfo(i);
        const STCupPos & cupPos = reagInfo.stcuppos();
        vos_u32 index = cupPos.uidskid() * 1000 + cupPos.uicupid();
        m_LReagents[index] = reagInfo;
    }
    printf("CAutoLoad::L InitReagent \n");
    return ;
}

void CAutoLoad::GetDebugRreagent(STNotifyReagInfo info)
{
    STNotifyReagInfo  pPara = info;
    m_stThresholds.clear();
    m_stReagents.clear();
    for (int i = 0; i < pPara.stthreshold_size(); ++i)
    {
        const STThreshold& threshold = pPara.stthreshold(i);
        m_stThresholds[threshold.enbtype()] = threshold.uithreshold();
    }
    for (int i = 0; i < pPara.streaginfo_size(); ++i)
    {
        const STReagInfo & reagInfo = pPara.streaginfo(i);
        const STCupPos & cupPos = reagInfo.stcuppos();
        vos_u32 index = cupPos.uidskid() * 1000 + cupPos.uicupid();
        m_stReagents[index] = reagInfo;
    }
    app_print(ps_autoload,"CAutoLoad::R InitReagent \n");
    _compareReagent();
    return ;
}

bool CAutoLoad::NeedWheelset(CTask* ntask,CTask*& wTask)
{
    int porid[4] = { PERIOD_R1_36, PERIOD_R2_97, PERIOD_R3_201, PERIOD_R4_262 };
    int size_number = ntask->GetReqSize();
    for (int i = 0; i < size_number; i++)
    {
        const STLiquidInfo& reagent = ntask->GetReagInfo(porid[i]);
        if (reagent.uiaddvol() == 0)
        {
            break;
        }
        int place_regent = reagent.stcuppos().uidskid() * 1000 + reagent.stcuppos().uicupid();
        map<vos_u32, float>* mi_volume = \
            (m_stReagents[place_regent].enbtype() == BT_standard) ? &standtrd_volume\
            : (m_stReagents[place_regent].enbtype() == BT_middle) ? &middle_volume\
            : (m_stReagents[place_regent].enbtype() == BT_micro) ? &micro_volume : 0;
        int subdepth = 0;
        float mmdepth = 0;
        for (map<vos_u32, float>::iterator viter = mi_volume->begin(); viter != mi_volume->end(); viter++)
        {
            if (viter->second<(reagent.uiaddvol()/100))
            {
                subdepth = viter->first;
                mmdepth = viter->second;  
                continue;
            }
            if (viter->second == (reagent.uiaddvol()/100))
            {
                subdepth = viter->first;
                break;
            }
            if (viter->second>(reagent.uiaddvol()/100))
            {
                int sec = viter->second - reagent.uiaddvol() / 100 > reagent.uiaddvol() / 100 \
                    - mmdepth?subdepth:viter->first;
                subdepth = sec;
                break;
            }
        }
        app_print(ps_autoload, "reagent addvol:%d subdepth:%d \n", reagent.uiaddvol(),subdepth);
        int subOver = (m_stReagents[place_regent].uimargindepth() - subdepth);
        subOver = subOver > 0 ? subOver : 1;
        m_stReagents[place_regent].set_uimargindepth(subOver);
        app_print(ps_autoload, "R reagent dskid:%d cupid:%d subdepth:%d uimagrgindepth:%d\n",\
            reagent.stcuppos().uidskid(), reagent.stcuppos().uicupid(), subdepth, m_stReagents[place_regent].uimargindepth());
        if ((m_stReagents[place_regent].uimargindepth() < m_stThresholds[m_stReagents[place_regent].enbtype()]) || (wheelset == true))
        {
            for (map<vos_u32, STReagInfo>::iterator lpara = m_LReagents.begin(); lpara != m_LReagents.end(); lpara++)
            {
                if (lpara->second.strreagname() == m_stReagents[place_regent].strreagname()&&!wheelset)
                {
                    m_RReagents.push(m_stReagents[place_regent]);
                    m_LReplaceReagents.push(lpara->second);
                    wheelset = true;
                    break;
                }
            }
            if (wheelset)
            {
                STReqAddTask  reqAddTask;
                reqAddTask.set_uimaintantype(wheelset_task);
                STLiquidInfo* rq=reqAddTask.add_streagents();
                STCupPos* cp = rq->mutable_stcuppos();
                cp->set_uicupid(m_stReagents[place_regent].stcuppos().uicupid());
                cp->set_uidskid(m_stReagents[place_regent].stcuppos().uidskid());
                rq->set_uiaddvol(reagent.uiaddvol());
                wTask = new CTask(SPid_Task, &reqAddTask);
                app_print(ps_autoload, "create wheel task!\n");
                return true;
            }
        }
    }
    return false;
}

bool CAutoLoad::_readfile_volumeStandard()
{
    char  aucwkpath[128] = { 0 };
    char  aucabspath[128] = { 0 };
    vos_getworkpath((char*)aucwkpath);
    sprintf((char*)aucabspath, "/opt/project.ini", (char*)aucwkpath);
    ifstream ifs;
    ifs.open(aucabspath, ios::in);
    if (!ifs.is_open())
    {
        printf(" open project.ini failed path:%s \r\n",aucabspath);
        return APP_COM_ERR;
    }
    string buf;
    map<vos_u32, float>* nowmap=&standtrd_volume;
   do 
   {
       getline(ifs,buf);
       if (buf.find("standard") != buf.npos)
       {
           nowmap = &standtrd_volume;
           continue;
       }
       else if (buf.find("micro") != buf.npos)
       {
           nowmap = &micro_volume;
           continue;
       }
       else if (buf.find("middle") != buf.npos)
       {
           nowmap = &middle_volume;
           continue;
       }
       if (buf.find('=') != buf.npos)
       {
           vos_u32 fonint = atoi((const char*)buf.substr(0, buf.find('=') - 1).c_str());
           float sedint = atof((const char*)buf.substr(buf.find('=') + 1, buf.size() - 1).c_str());
           nowmap->insert(pair<vos_u32, float>(fonint,sedint));
       }
   } while (!buf.empty());
   return VOS_OK;
}
vos_u32 CAutoLoad::_updateRReagent(vos_u32 dskId, vos_u32 cupId, vos_u32 marginDepth)
{
    vos_u32 index = dskId * 1000 + cupId;
    map<vos_u32, STReagInfo>::iterator iter = m_stReagents.find(index);
    if (iter == m_stReagents.end())
    {
        WRITE_WARN_LOG("Not find reaginfo dskid=%lu, cupid=%lu.\n", dskId, cupId);
        return app_input_para_invalid_err;
    }
    iter->second.set_uimargindepth(marginDepth);
    app_print(ps_autoload, "app_simuRemainDepth_regent %d %d %d \n",dskId, cupId, marginDepth);
    return VOS_OK;
}