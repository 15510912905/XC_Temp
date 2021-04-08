#include "task.h"
#include "vos_log.h"
#include "vos_init.h"
#include "typedefine.pb.h"
#include "app_errorcode.h"
#include "adsimulate.h"
#include "reagent.h"
#include "app_msg.h"
#include "apputils.h"

#include<google/protobuf/io/zero_copy_stream_impl.h> 
#include<google/protobuf/text_format.h>
#include <fcntl.h>
#include <ustat.h>

CTask::CTask(vos_u32 ulSrcPid, STReqAddTask* pReqInfo) : m_iDilutPeriod(0), m_ulSrcPid(ulSrcPid), m_bSendResult(false), m_ulDilutType(EN_DilutNorm)
{
	m_stReqInfo.CopyFrom(*pReqInfo);
	while (m_stReqInfo.streagents_size() < 4)
	{
		STLiquidInfo* pInfo = m_stReqInfo.add_streagents();
		pInfo->set_uiaddvol(0);
	}
	m_stResInfo.Clear();
	m_stResInfo.set_uitaskid(pReqInfo->uitaskid());
	m_stResInfo.set_uimaintantype(pReqInfo->uimaintantype());
	m_stResInfo.set_szprjname(pReqInfo->strprojectname());

	for (int i = 0; i < WAVE_NUM; ++i)
	{
		m_stResInfo.add_stadpoints();
	}
	STCupPos* pSmpPos = m_stResInfo.mutable_stsmppos();
	pSmpPos->set_uidskid(pReqInfo->stsmpinfo().stcuppos().uidskid());
	pSmpPos->set_uicupid(pReqInfo->stsmpinfo().stcuppos().uicupid());
}

CTask::~CTask()
{

}

void CTask::PrintTaskInfo()
{
    app_print(ps_samplesys, "taskid=%d,SmpType=%d,MType=%d,Prj=%s\n", m_stReqInfo.uitaskid(), m_stReqInfo.uismptype()
        , m_stReqInfo.uimaintantype(), m_stReqInfo.strprojectname().c_str());
    for (int index = 0; index < m_stReqInfo.stwavechs_size(); index++)
    {
        STKeyValue* pWaveChs = m_stReqInfo.mutable_stwavechs(index);
        app_print(ps_samplesys, "WaveChs=%d,%d\n", pWaveChs->uikey(), pWaveChs->uival());
    }
    STLiquidInfo* pSmpInfo = m_stReqInfo.mutable_stsmpinfo();
    app_print(ps_samplesys, "Sample at (%d)(%d),volue=%d\n", pSmpInfo->stcuppos().uidskid(), pSmpInfo->stcuppos().uicupid()
        , pSmpInfo->uiaddvol());
    for (int index = 0; index < m_stReqInfo.streagents_size(); index++)
    {
        STLiquidInfo* pReagInfo = m_stReqInfo.mutable_streagents(index);
        app_print(ps_samplesys, "R%d at (%d)(%d),volue=%d\n", index + 1, pReagInfo->stcuppos().uidskid(), pReagInfo->stcuppos().uicupid()
            , pReagInfo->uiaddvol());
    }
    app_print(ps_samplesys, "uiTestPoint=%d\n", m_stReqInfo.uitestpoint());
    app_print(ps_samplesys, "uiSmpId=%d\n", m_stReqInfo.uismpid());
    app_print(ps_samplesys, "uiDiluentVol=%d\n", m_stReqInfo.uidiluentvol());
    app_print(ps_samplesys, "uiLiquidVol=%d\n", m_stReqInfo.uiliquidvol());
    app_print(ps_samplesys, "Circle=%d\n", m_stReqInfo.encircle());
}

void CTask::SetAnalyCircle(ReactCircle circleType)
{
    m_stResInfo.set_encircle(circleType);
}

CupState CTask::GetActStatus(int ulPeriod)
{
    if (EN_DilutType == m_ulDilutType && ulPeriod >= PERIOD_DilutEnd_95)
    {
        return CUP_STATE_DIRTY;
    }
    if (EN_ReactionType == m_ulDilutType && ulPeriod < 1)
    {
        return CUP_STATE_BUTT;
    }

    if (ulPeriod <= PERIOD_WashEnd_33)
    {
        return CUP_STATE_WASH;
    }

    if (ulPeriod <= PERIOD_R1_36)
    {
        const STLiquidInfo& liquid = m_stReqInfo.streagents(0);
        if (liquid.uiaddvol() > 0)
        {
            return CUP_STATE_R1;
        }
    }

    if (ulPeriod <= PERIOD_SmpMix_50)
    {
        const STLiquidInfo& liquid = m_stReqInfo.stsmpinfo();
        if (liquid.uiaddvol() > 0)
        {
            return CUP_STATE_SAMPLE;
        }
    }

    if (ulPeriod <= PERIOD_R2Mix_99)
    {
        const STLiquidInfo& liquid = m_stReqInfo.streagents(1);
        if (liquid.uiaddvol() > 0)
        {
            return CUP_STATE_R2;
        }
    }

    if (ulPeriod <= PERIOD_R3Mix_215&&m_stReqInfo.streagents_size() >= 3)
    {
        const STLiquidInfo& liquid = m_stReqInfo.streagents(2);
        if (liquid.uiaddvol() > 0)
        {
            return CUP_STATE_R3;
        }
    }

    if (ulPeriod <= PERIOD_R4Mix_264&&m_stReqInfo.streagents_size() == 4)
    {
        const STLiquidInfo& liquid = m_stReqInfo.streagents(3);
        if (liquid.uiaddvol() > 0)
        {
            return CUP_STATE_R4;
        }
    }

    return CUP_STATE_REACTION;
}

const STLiquidInfo& CTask::GetReagInfo(int iPeriod)
{
    if (PERIOD_R1_36 >= iPeriod)
    {
        return m_stReqInfo.streagents(0);
    }
    if (PERIOD_R2_97 >= iPeriod)
    {
        return m_stReqInfo.streagents(1);
    }
    if (PERIOD_R3_201 >= iPeriod)
    {
        return m_stReqInfo.streagents(2);
    }
    return m_stReqInfo.streagents(3);
}

vos_u32 CTask::SetAdValue(vos_u32 adval[WAVE_NUM], app_u16 usPeriod)
{
    app_print(ps_ADval, "P %d ", usPeriod);
    for (int i = 0; i < WAVE_NUM; ++i)
    {
        STAdPoints * pPoint = m_stResInfo.mutable_stadpoints(i);
#ifdef COMPILER_IS_LINUX_GCC
        app_print(ps_ADval, "ad[%d]=%d, ", i + 1, g_pADSimulate->GetAdValue(m_ulSimulateId, i, pPoint->uiadpoints_size()));
        pPoint->add_uiadpoints(g_pADSimulate->GetAdValue(m_ulSimulateId, i, pPoint->uiadpoints_size()));
#else
        app_print(ps_ADval, "ad[%d]=%d, ", i + 1, adval[i]);
        pPoint->add_uiadpoints(adval[i]);
#endif
    }
    app_print(ps_ADval, "\n");
    return VOS_OK;
}

bool CTask::GetDilFirstTask(STReqAddTask &stReqInfo)
{
	stReqInfo.set_uitaskid(m_stReqInfo.uitaskid());
	stReqInfo.set_uismpid(m_stReqInfo.uismpid());
	stReqInfo.set_uimaintantype(m_stReqInfo.uimaintantype());
    stReqInfo.set_uismptype(m_stReqInfo.uismptype());
    stReqInfo.set_strprojectname("W2");

	STLiquidInfo *pSmpInfo = stReqInfo.mutable_stsmpinfo();
	pSmpInfo->set_uiaddvol(m_stReqInfo.uiliquidvol());   //原液体积

	pSmpInfo->mutable_stcuppos()->CopyFrom(m_stReqInfo.stsmpinfo().stcuppos()); //样本位置

	STLiquidInfo *pR1Info = stReqInfo.add_streagents();
	pR1Info->set_uiaddvol(m_stReqInfo.uidiluentvol());//稀释液

	STCupPos *pstCupPos = pR1Info->mutable_stcuppos();
    
    vos_u32 nCup = 0;
    bool bGetCup = g_pReagent->GetReagCup(ReagDisk_R1, nCup, "W2", ReType_R1);
    pstCupPos->set_uicupid(nCup);
    pstCupPos->set_uidskid(ReagDisk_R1);
    return bGetCup;
}

bool CTask::GetWBFirstTask(STReqAddTask &stReqInfo)
{
    stReqInfo.set_uitaskid(m_stReqInfo.uitaskid());
    stReqInfo.set_uismpid(m_stReqInfo.uismpid());
    stReqInfo.set_uimaintantype(m_stReqInfo.uimaintantype());
    stReqInfo.set_uismptype(m_stReqInfo.uismptype());
    stReqInfo.set_strprojectname("_WB");

    STLiquidInfo *pSmpInfo = stReqInfo.mutable_stsmpinfo();
    pSmpInfo->set_uiaddvol(35);   //原液体积

    pSmpInfo->mutable_stcuppos()->CopyFrom(m_stReqInfo.stsmpinfo().stcuppos()); //样本位置

    STLiquidInfo *pR1Info = stReqInfo.add_streagents();
    pR1Info->set_uiaddvol(3500);//稀释液

    STCupPos *pstCupPos = pR1Info->mutable_stcuppos();
    
    vos_u32 nCup = 0;
    bool bGetCup = g_pReagent->GetReagCup(ReagDisk_R1, nCup, "_WB", ReType_R1);
    pstCupPos->set_uicupid(nCup);
    pstCupPos->set_uidskid(ReagDisk_R1);
    return bGetCup;
}

void CTask::SkipDirtyCup()
{
    m_stResInfo.clear_stadpoints();
    for (int i = 0; i < WAVE_NUM; ++i)
    {
        m_stResInfo.add_stadpoints();
    }
}

vos_u32 CTask::SendTaskRes(vos_u32 ulResult, int iLgcId)
{
    if (m_ulDilutType == EN_DilutType)
	{
        ulResult = TR_DilOver;
	}

    m_stResInfo.set_uiresult(ulResult);
    m_stResInfo.set_uircupno(iLgcId);
    if (m_ulSrcPid == SPid_Task)//中位机自己创建的任务：如清洗任务，不返回
    {
        return 0;
    }
    /*int infile = open("/opt/saveAD.txt", O_RDWR | O_APPEND | O_CREAT | O_NONBLOCK, 0777);
    if (infile >= 0)
    {
        google::protobuf::io::FileOutputStream fileOutput(infile);
        google::protobuf::TextFormat::Print(m_stResInfo, &fileOutput);
        fileOutput.Flush();
        fileOutput.Close();
        close(infile);
        infile = -1;
    }*/
    return sendInnerMsg(SPid_TaskResult, m_ulSrcPid, MSG_ResAddTask, &m_stResInfo);
}

void CTask::CheckSmpTask(STNotifyFailedSmpTask& stPara)
{
    if (common_task == m_stReqInfo.uimaintantype() || emergence_task == m_stReqInfo.uimaintantype())
    {
        stPara.add_ultaskid(m_stReqInfo.uitaskid());
    }
}

vos_u32 CTask::_waveMapToId(vos_u32 enwaveval)
{
    vos_u32 ulwaveid = 0;

    switch (enwaveval)
    {
        case app_rscsch_wave_340nm_id:
            ulwaveid = 0;
            break;
        case app_rscsch_wave_405nm_id:
            ulwaveid = 1;
            break;
        case app_rscsch_wave_450nm_id:
            ulwaveid = 2;
            break;
        case app_rscsch_wave_478nm_id:
            ulwaveid = 3;
            break;
        case app_rscsch_wave_505nm_id:
            ulwaveid = 4;
            break;
        case app_rscsch_wave_542nm_id:
            ulwaveid = 5;
            break;
        case app_rscsch_wave_570nm_id:
            ulwaveid = 6;
            break;
        case app_rscsch_wave_605nm_id:
            ulwaveid = 7;
            break;
        case app_rscsch_wave_630nm_id:
            ulwaveid = 8;
            break;
        case app_rscsch_wave_660nm_id:
            ulwaveid = 9;
            break;
        case app_rscsch_wave_700nm_id:
            ulwaveid = 10;
            break;
        case app_rscsch_wave_750nm_id:
            ulwaveid = 11;
            break;
        case app_rscsch_wave_805nm_id:
            ulwaveid = 12;
            break;
        case app_rscsch_wave_850nm_id:
            ulwaveid = 13;
            break;
        default:
            break;
    }

    return ulwaveid;
}

void CTask::GetSmpNdlPos(app_u16& usDstPos)
{
    if (m_ulDilutType == EN_ReactionType)
    {
        usDstPos = SNdlPosDilute;
    }
    else if (m_stReqInfo.uimaintantype() == emergence_task)
    {
        usDstPos = SNdlPosEmer;
    }
    else if (m_stReqInfo.uimaintantype() == wash_task)//清洗任务
    {
        usDstPos = SNdlPosW1;// m_stReqInfo.stsmpinfo().stcuppos().uicupid();
    }
    else
    {
        usDstPos = SNdlPosNorm;
    }
}

void CTask::CheckResultCalc(int iLgcId)
{
    if (!m_bSendResult)
    {
        const STAdPoints& rAdPoint = m_stResInfo.stadpoints(0);
        vos_u32 ulTestPoint = rAdPoint.uiadpoints_size();
        if (0 != ulTestPoint&& ulTestPoint == m_stReqInfo.uitestpoint())
        {
            SendTaskRes(TR_Success, iLgcId);
            m_bSendResult = true;
        }
    }
}

bool CTask::IsDoublePeriod()
{
    for (int i = 2; i < m_stReqInfo.streagents_size(); ++i)
    {
        const STLiquidInfo& liquid = m_stReqInfo.streagents(i);
        if (liquid.uiaddvol() > 0)
        {
            return true;
        }
    }
    return false;
}

bool CTask::NeedAddReag(int reagent)
{
    const STLiquidInfo& liquid = m_stReqInfo.streagents(reagent - 1);
    return liquid.uiaddvol() > 0;
}

void CTask::GetCupStatus(STRctCupStatus * pStatus)
{
    pStatus->set_uitaskid(m_stReqInfo.uitaskid());

    STCupPos* pSmpPos = pStatus->mutable_stsmppos();
    pSmpPos->set_uidskid(m_stReqInfo.stsmpinfo().stcuppos().uidskid());
    pSmpPos->set_uicupid(m_stReqInfo.stsmpinfo().stcuppos().uicupid());
}

vos_u32 CTask::GetSmpAddVol()
{
    return m_stReqInfo.stsmpinfo().uiaddvol();
}

void CTask::CalcDiskCup(app_u16 & disk, app_u16 & cup)
{
    disk = m_stReqInfo.stsmpinfo().stcuppos().uidskid();
    cup = m_stReqInfo.stsmpinfo().stcuppos().uicupid();
}

void CTask::CalcDiskCup(app_u16 & disk)
{
    app_u16 cupTemp;
    DiskPosToCup(m_stReqInfo.stsmpinfo().stcuppos().uicupid(), disk, cupTemp);
}

bool CTask::HasDilutVol()
{
    return m_stReqInfo.uidiluentvol() > 0 || m_stReqInfo.uismptype() == WHOLE_BLOOD;
}

bool CTask::NeedAddSmp()
{
	if (m_stReqInfo.uimaintantype() == Pollution_task)
	{
		return false;
	}
	return m_stReqInfo.stsmpinfo().uiaddvol() > 0;
}

bool CTask::NeedAddSmpDilu()
{
	if (m_stReqInfo.uimaintantype() != Pollution_task)
	{
		return false;
	}
	return m_stReqInfo.stsmpinfo().uiaddvol() > 0;
}
STLiquidInfo* CTask::_getReagByIndex(int reagIndex)
{
	if (m_stReqInfo.streagents_size() > reagIndex)
	{
		return m_stReqInfo.mutable_streagents(reagIndex);
	}
	return NULL;
}
bool CTask::_checkReagent(vos_u32 ulActType, vos_u32 ulReagIndex, vos_u32 period, vos_u32& usCupId)
{
	vos_u32 actType = GetActStatus(period);
	if (actType > ulActType)
	{
		return true;
	}

	STLiquidInfo * pReagInfo = _getReagByIndex(ulReagIndex - 1);
	if (NULL == pReagInfo || pReagInfo->uiaddvol() == 0)
	{
		return true;
	}

    vos_u32 usDskId = pReagInfo->stcuppos().uidskid();
    usCupId = pReagInfo->stcuppos().uicupid();

	//维护任务，自动测试任务，针清洗任务不需要检查试剂余量
    if (AutoTeskTask == GetMaintanType() || oam_task == GetMaintanType() || pollution_task == GetMaintanType() || colormetry_task == GetMaintanType())
	{
		return true;
	}
    string name = m_stReqInfo.strprojectname();
    name = (name == "D" && usDskId == ReagDisk_R1 && usCupId == 50) ? "D2"
        : (name == "D" && usDskId == ReagDisk_R2&& usCupId == 50) ? "D3"
        : name;
    if (!g_pReagent->CheckReagInfo(usDskId, usCupId, ulReagIndex, name))
    {
        WRITE_ERR_LOG("R%lu Disk:%lu; Cup:%lu;\n", ulReagIndex, usDskId, usCupId);
        return false;
    }
	pReagInfo->mutable_stcuppos()->set_uicupid(usCupId);

	return true;
}

bool CTask::_checkSmpInfo(vos_u32 period)
{
	//检查s
	vos_u32 actType = GetActStatus(period);
    if (actType > CUP_STATE_SAMPLE || m_stReqInfo.uimaintantype() == NdlWashTask || m_ulDilutType == EN_ReactionType || m_stReqInfo.uimaintantype() == AutoTeskTask)
	{
		return true;
	}

	vos_u32 usCupId = GetSmpInfo()->stcuppos().uicupid();
    m_stReqInfo.uismpid();
	if (!g_pReagent->CheckSmp(GetMaintanType(), usCupId))
	{
		WRITE_ERR_LOG("ERROR! Sample Cup:%lu;\n", usCupId);
		return false;
	}
	return true;
}

bool CTask::CheckAllLiquid(vos_u32 period, vos_u32& usR1CupId, vos_u32& usR2CupId, TaskResult& TR_type)
{
	vos_u32 r1cup = 0, r2cup = 0, r3cup = 0, r4cup = 0;
	//检查r1
	if (!_checkReagent(CUP_STATE_R1, 1, period, r1cup))
	{
        TR_type = (m_stReqInfo.strprojectname() == "_WB") ? TR_ReagErr : TR_AddR1Err;
		return false;
	}
	//检查s
	if (!_checkSmpInfo(period))
	{
		TR_type = TR_AddSmpErr;
		return false;
	}

	//检查r2
	if (!_checkReagent(CUP_STATE_R2, 2, period, r2cup))
	{
		TR_type = TR_AddR2Err;
		return false;
	}

	//检查r3
	if (!_checkReagent(CUP_STATE_R3, 3, period, r3cup))
	{
		TR_type = TR_AddR3Err;
		return false;
	}

	//检查r4
	if (!_checkReagent(CUP_STATE_R4, 4, period, r4cup))
	{
		TR_type = TR_AddR4Err;
		return false;
	}
	usR1CupId = (period <= PERIOD_R1_36) ? r1cup : r3cup;
	usR2CupId = (period <= PERIOD_R2_97) ? r2cup : r4cup;
	//printf("CheckAllLiquid period = %d ;usR1CupId = %d ; usR2CupId = %d\n", period,usR1CupId, usR2CupId);
	return true;
}
