#include "reagent.h"
#include "vos_log.h"
#include "app_errorcode.h"
#include "app_xc8002shareinfo.h"
#include "socketmonitor.h"
#include "alarm.h"
#include "xc8002_middwnmachshare.h"
#include "autoload.h"

#define DETECTION_FALSE (0xFFFF) //误检
#define DETECTION_MISS  (0xFFFE) //漏检
#define Reagent_Wash_Threshold 20                //试剂清洗门限值

CReagent * g_pReagent = NULL;
CReagent::CReagent()
{
    m_mutex = vos_mallocmutex();
    if (NULL == m_mutex)
    {
        WRITE_ERR_LOG("vos_mallocmutex failed! \n")
    }
    ClearUnSmp();
}

CReagent::~CReagent()
{

}

vos_u32 CReagent::InitReagent(CMsg& stMsg)
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
    }
	printf("CReagent::InitReagent \n");
    return VOS_OK;
}

vos_u32 CReagent::ProcReagentMargin(vos_u32 dskId, vos_u32 cupId, vos_u32 &marginDepth)
{
    if (DETECTION_FALSE == marginDepth || 0 == marginDepth)
    {
        //处理误检
        WRITE_ERR_LOG("Reaginfo error dskid=%lu, cupid=%lu,marginDepth=%lu.\n", dskId, cupId, marginDepth);
        return app_invalid_extdev_name_err;
    }
	marginDepth = (marginDepth == DETECTION_MISS) ? 1 : marginDepth;
    //??? ??????,????????????
    g_pAutoLoad->_updateRReagent(dskId, cupId, marginDepth);
    //end
    return _updateReagent(dskId, cupId, marginDepth);
}

vos_u32 CReagent::_updateReagent(vos_u32 dskId, vos_u32 cupId, vos_u32 marginDepth)
{
    vos_u32 index = dskId * 1000 + cupId;
    map<vos_u32, STReagInfo>::iterator iter = m_stReagents.find(index);
    if (iter == m_stReagents.end())
    {
        WRITE_WARN_LOG("Not find reaginfo dskid=%lu, cupid=%lu.\n", dskId, cupId);
        return app_input_para_invalid_err;
    }
    iter->second.set_uimargindepth(marginDepth);
    return VOS_OK;
}

bool CReagent::CheckReagInfo(vos_u32 dskId, vos_u32& cupId, vos_u32 ReType, const string& reagName)
{
    vos_u32 index = dskId * 1000 + cupId;
    map<vos_u32, STReagInfo>::iterator iter = m_stReagents.find(index);
    if (iter != m_stReagents.end())
    {
        if ((iter->second.uiretype() == ReType)
            && _isMarginEnough(iter->second.enbtype(), iter->second.uimargindepth())
            && reagName == iter->second.strreagname())
        {
            return true;
        }
    }
	
    return _findSameReagent(dskId, cupId, reagName, ReType);
}

void CReagent::AddSmpUnEngh(vos_u32 cupId)
{
	vos_obtainmutex(m_mutex);
    m_stSmpLose.insert(cupId);
	vos_releasemutex(m_mutex);
}

bool CReagent::CheckSmp(vos_u32 uimaintantype,vos_u32 cupId)
{
    vos_u32 flag  = g_pMonitor->GetCollideNdlConfig();

    if (AutoTeskTask == uimaintantype || flag == collide_null)
    {
        return true;
    }
	vos_obtainmutex(m_mutex);
	bool res = !m_stSmpLose.count(cupId);
	vos_releasemutex(m_mutex);
	return res;
}

void CReagent::ClearUnSmp()
{
	vos_obtainmutex(m_mutex);
    if(!m_stSmpLose.empty())
    {
        m_stSmpLose.clear();
    }
	vos_releasemutex(m_mutex);
}

void CReagent::DeleteUnSmp(vos_u32 sampid)
{
	vos_obtainmutex(m_mutex);
	set<vos_u32>::iterator iter = m_stSmpLose.begin();
	for (; iter != m_stSmpLose.end(); iter++)
	{
		if (*iter == sampid)
		{
			m_stSmpLose.erase(iter);
			break;
		}
	}
	vos_releasemutex(m_mutex);
}

bool CReagent::GetReagCup(vos_u32 dskId, vos_u32& cupId, const string& strReagName, vos_u32 ReType)
{
    return _findSameReagent(dskId, cupId, strReagName, ReType);
}

bool CReagent::_findSameReagent(vos_u32 dskId, vos_u32& cupId, const string& strReagName, vos_u32 ReType)
{
    vos_u32 tempCup = 0xff;
    map<vos_u32, STReagInfo>::iterator iter = m_stReagents.begin();
    for (; iter != m_stReagents.end(); ++iter)
    {
        STReagInfo & info = iter->second;
		if (strReagName != info.strreagname())
		{
			continue;
		}
        if (dskId != info.stcuppos().uidskid())
        {
            continue;
        }
		if (ReType != info.uiretype())
		{
			continue;
		}
        if (!_isMarginEnough(info.enbtype(), info.uimargindepth()))
        {
            continue;
        }
        //使用试剂杯位号较小的试剂
        tempCup = (info.stcuppos().uicupid() < tempCup) ? info.stcuppos().uicupid() : tempCup;    
    }
    if (tempCup != 0xff)
    {
        cupId = tempCup;
        return true;     
    }

    WRITE_ERR_LOG("Not find Reagent dskid=%lu,cupid=%lu\n", dskId, cupId);
    return false;
}

bool CReagent::_isMarginEnough(BottleType btype, vos_u32 marginDepth)
{
    map<BottleType, vos_u32>::iterator iter = m_stThresholds.find(btype);
    if (iter == m_stThresholds.end())
    {
        WRITE_ERR_LOG("Not find reaginfo threshold,enbtype=%d.\n", btype);
        return APP_NO;
    }
    return marginDepth >= iter->second;
}

