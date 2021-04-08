#include "alarm.h"
#include<google/protobuf/text_format.h>
#include<google/protobuf/io/zero_copy_stream_impl.h> 
#include <fcntl.h>
#include "vos_log.h"
#include "app_errorcode.h"
#include "app_msg.h"
#include "msg.h"
#include "apputils.h"
#include "socketmonitor.h"
#include "reagent.h"

#define  BEEP_TIME_LEN		(5)
#define  BEEP_EMPTY_LEN		(100)
CAlarm * g_pAlarm = NULL;

CAlarm::CAlarm(vos_u32 ulPid) :CCallBack(ulPid), m_mutex(NULL), m_isFilterInReset(false)
{
	m_ulTimerId = createTimer(SPid_Alarm, "beep", BEEP_TIME_LEN);
	m_beep.SetBeepGpio(0);
	vos_starttimer(m_ulTimerId);
}

CAlarm::~CAlarm()
{
    vos_deletemutex(m_mutex);
    vos_freemutex(m_mutex);
}

vos_u32 CAlarm::Initialize()
{
    int infile = open("/opt/alarm.cfg", O_RDONLY);

    if (infile < 0)
    {
        WRITE_ERR_LOG("open alarm.cfg failed.\n");
        return app_open_file_error;
    }
    CAlarmList stAlarmList;
    google::protobuf::io::FileInputStream fileInput(infile);
    if (!google::protobuf::TextFormat::Parse(&fileInput, &stAlarmList))
    {
        WRITE_ERR_LOG("Parse alarm.cfg failed.\n");
        close(infile);
        return app_open_file_error;
    }
    close(infile);
    for (int i = 0; i < stAlarmList.data_size(); ++i)
    {
        const CAlarmInfo & rInfo = stAlarmList.data(i);
        m_allAlarm[rInfo.ulalarmid()] = rInfo;
    }
    m_mutex = vos_mallocmutex();
    if (NULL != m_mutex)
    {
        return vos_createmutex(m_mutex);
    }
    return VOS_OK;
}

bool CAlarm::_isEqualAlarm(const STAlarmInfo& alarm1, const STAlarmInfo& alarm2)
{
    if (alarm1.ulalarmid() != alarm2.ulalarmid())
    {
        return false;
    }
    if (alarm1.ulparas_size() != alarm2.ulparas_size())
    {
        return false;
    }
    for (int i = 0; i < alarm1.ulparas_size(); ++i)
    {
        if (alarm1.ulparas(i) != alarm2.ulparas(i))
        {
            return false;
        }
    }
    return true;
}
void CAlarm::_notifyAlarm(const STAlarmInfo& alarm, vos_u32 ulActType)
{
    CNotifyAlarm stPara;
    stPara.set_ulacttype(ulActType);
    STAlarmInfo* pAlarm = stPara.mutable_stalarm();
    pAlarm->CopyFrom(alarm);
    if (VOS_OK != sendUpMsg(CPid_Alarm, m_ulPid, MSG_NotifyAlarm, &stPara))
    {
        WRITE_ERR_LOG("Notify alarm failed.\n");
    }
}

bool CAlarm::_isNeedFilter(vos_u32 ulAlarmId)
{
    if (m_isFilterInReset || vos_midmachine_state_rewarming == g_pMonitor->m_ulState)
    {
        map<vos_u32, CAlarmInfo>::iterator iter = m_allAlarm.find(ulAlarmId);
        if (iter != m_allAlarm.end())
        {
            return iter->second.bbootfilter();
        }
    }
    return false;
}

void CAlarm::GenerateAlarm(vos_u32 ulAlarmId, vos_u32 ulPara1, vos_u32 ulPara2)
{
	vos_u32 ulpara = ulPara2;
	vos_u32 ulalarmid = ulAlarmId;
	if (ALarm_AbsorbAirPos/1000 == ulAlarmId/1000)
	{
		ulpara = ulAlarmId % 1000;//杯位信息
		ulalarmid = ALarm_AbsorbAir;
		g_pReagent->AddSmpUnEngh(ulpara);
	}
	printf(" GenerateAlarm   %d  %d  %d\n ", (int)ulAlarmId, (int)ulPara1, (int)ulpara);
    if (ulAlarmId == 0)
    {
        return;
    }
	if (!_isNeedFilter(ulalarmid))
    {
        STAlarmInfo stAlarm;
		stAlarm.set_ulalarmid(ulalarmid);
        stAlarm.set_strtime(GetCurTimeStr());
		stAlarm.add_ulparas(ulPara1);
		stAlarm.add_ulparas(ulpara);
        _generateAlarm(stAlarm);
    }
}

void CAlarm::RecoverAlarm(vos_u32 ulAlarmId, vos_u32 ulPara1, vos_u32 ulPara2)
{
	uint32 uCombId = ulAlarmId;
	if (ulAlarmId < 100)
	{
		uCombId = 1 * 10000 + ulPara1 * 100 + ulAlarmId;
	}
	else if (ulAlarmId == ALarm_Dwn_Act_Failed || ulAlarmId == ALarm_Dwn_Timeout)
	{
		uCombId = ulAlarmId + ulPara1;
	}
	printf(" RecoverAlarm   %d  %d  %d\n ", (int)ulAlarmId, (int)ulPara1, (int)ulPara2);

    STAlarmInfo stAlarm;
	stAlarm.set_ulalarmid(uCombId);
    stAlarm.add_ulparas(ulPara1);
    stAlarm.add_ulparas(ulPara2);
    _deleteAlarm(stAlarm);

	//printf(" RecoverAlarm uCombId = %d ----\n ", stAlarm.ulalarmid());

    _notifyAlarm(stAlarm, xc8002_fault_status_restore);
}

void CAlarm::SynAlarm()
{
    CNotifyAlarmList stPara;
    vos_obtainmutex(m_mutex);
    for (CAIter iter = m_activeAlarms.begin(); iter != m_activeAlarms.end(); ++iter)
    {
        const string& strTime = iter->strtime();
        if (string::npos != strTime.find("2007-01-01"))
        {
            iter->set_strtime(GetCurTimeStr());
        }
        STAlarmInfo* pAlarm = stPara.add_stalarms();
        pAlarm->CopyFrom(*iter);
    }
    vos_releasemutex(m_mutex);
    sendUpMsg(CPid_Alarm, m_ulPid, MSG_NotifySynAlarm, &stPara);
}

vos_u32 CAlarm::FindMaxLevelAlarm()
{
    vos_u32 ulMaxAlarm = 0;
	m_alarm.clear();
    vos_obtainmutex(m_mutex);
    for (CAIter iter = m_activeAlarms.begin(); iter != m_activeAlarms.end(); ++iter)
    {
        map<vos_u32, CAlarmInfo>::iterator aIter = m_allAlarm.find(iter->ulalarmid());
        if (aIter != m_allAlarm.end())
        {
            if (aIter->second.ulalarmlevel() > ulMaxAlarm)
            {
                ulMaxAlarm = aIter->second.ulalarmlevel();
				m_alarm.insert(ulMaxAlarm);
				printf("max alarm   %d  %d\n", (int)(iter->ulalarmid()), (int)ulMaxAlarm);
            }
        }
    }
    vos_releasemutex(m_mutex);
    return ulMaxAlarm;
}

bool CAlarm::HasAlarm(vos_u32 alarmType)
{
	set<vos_u32>::iterator iter = m_alarm.find(alarmType);
	if (iter != m_alarm.end())
	{
		return true;
	}
	return false;
}

vos_u32 CAlarm::CallBack(vos_msg_header_stru* pMsg)
{
    if (MSG_ReqDeleteAlarm == pMsg->usmsgtype)
    {
        return _deleteAlarm(pMsg);
    }
	if (vos_pidmsgtype_timer_timeout == pMsg->usmsgtype)
	{
		vos_settimertimelen(m_ulTimerId, BEEP_TIME_LEN);
		if (m_beep.IsNeedPlayBeep())
		{
			m_beep.ConvertBeep();
			m_beep.UpdateBeepCount();
			if (!m_beep.IsNeedPlayBeep())
			{
				vos_settimertimelen(m_ulTimerId, BEEP_EMPTY_LEN);
			}
		}
		vos_starttimer(m_ulTimerId);
		return VOS_OK;
	}
	if (MSG_ReqBeepAlarm == pMsg->usmsgtype)
	{
		return m_beep.ParseBeepAlarm(pMsg);
	}
	if (MSG_ReqBeepPlay == pMsg->usmsgtype)
	{
		CMsg stMsg;
		if (stMsg.ParseMsg(pMsg))
		{
			BeepAlarm * pBeep = dynamic_cast<BeepAlarm*>(stMsg.pPara);
			m_beep.PlayBeepCount(pBeep->beepcount());
		}
		return VOS_OK;
	}

    WRITE_ERR_LOG("Unknow msgtype=%lu\n", pMsg->usmsgtype);
    return VOS_OK;
}

vos_u32 CAlarm::_deleteAlarm(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (stMsg.ParseMsg(pMsg))
    {
        CNotifyAlarmList * pAlarmList = dynamic_cast<CNotifyAlarmList*>(stMsg.pPara);
        if (pAlarmList->stalarms_size() > 1)
        {
            vos_obtainmutex(m_mutex);
            m_activeAlarms.clear();
			g_pReagent->ClearUnSmp();
            vos_releasemutex(m_mutex);
        }
        else if (1 == pAlarmList->stalarms_size())
        {
            const STAlarmInfo& rInfo = pAlarmList->stalarms(0);
            _deleteAlarm(rInfo);
        }
    }
    return VOS_OK;
}

void CAlarm::_deleteAlarm(const STAlarmInfo& alarm)
{
    vos_obtainmutex(m_mutex);
    for (CAIter iter = m_activeAlarms.begin(); iter != m_activeAlarms.end(); ++iter)
    {
        if (_isEqualAlarm(alarm, *iter))
        {
            m_activeAlarms.erase(iter);

			if (alarm.ulalarmid() == ALarm_Smp_Margin && alarm.ulparas_size() >= 2)
			{
				g_pReagent->DeleteUnSmp(alarm.ulparas(1));
			}
            if (alarm.ulalarmid() == ALarm_Wash_Margin)
            {
                g_pReagent->DeleteUnSmp(alarm.ulparas(1));
            }
            break;
        }
    }
    vos_releasemutex(m_mutex);
}

void CAlarm::_generateAlarm(const STAlarmInfo& alarm)
{
    vos_obtainmutex(m_mutex);
    for (CAIter iter = m_activeAlarms.begin(); iter != m_activeAlarms.end(); ++iter)
    {
        if (_isEqualAlarm(alarm,*iter))
        {
            WRITE_ERR_LOG("Alarm = %lu is exist.\n", alarm.ulalarmid());
            vos_releasemutex(m_mutex);
            return;
        }
    }

    m_activeAlarms.push_back(alarm);
    vos_releasemutex(m_mutex);

    _notifyAlarm(alarm, xc8002_fault_status_generate);

    //蜂鸣器声音
// 	map<vos_u32, CAlarmInfo>::iterator aIter = m_allAlarm.find(alarm.ulalarmid());
// 	if (aIter != m_allAlarm.end())
// 	{
// 		m_beep.CheckBeepAlarm(aIter->second.ulalarmlevel());
// 	}
    //TODO 后处理逻辑添加
}

vos_u32 CBeep::ParseBeepAlarm(vos_msg_header_stru* pMsg)
{
	CMsg stMsg;
	if (stMsg.ParseMsg(pMsg))
	{
		STReqBeepAlarm * pBeep = dynamic_cast<STReqBeepAlarm*>(stMsg.pPara);
		m_mBeepAlarmCfg.clear();
		m_EnbaleFlag = pBeep->beepenable();
		for (int i = 0; i < pBeep->beepcountlist_size(); i++)
		{
			const BeepAlarm& beep = pBeep->beepcountlist(i);
			m_mBeepAlarmCfg[beep.alarmlevel()] = beep.beepcount();
		}
		return true;
	}
	return false;
}

void CBeep::CheckBeepAlarm(vos_u32 level)
{
	if (m_EnbaleFlag && level)
	{
		map<vos_u32, vos_u32>::iterator iter = m_mBeepAlarmCfg.find(level);
		if (iter != m_mBeepAlarmCfg.end())
		{
			_setBeepCount(iter->second);
		}
	}
}

void CBeep::PlayBeepCount(vos_u32 count)
{
	_setBeepCount(count);
}

void CBeep::_setBeepCount(const vos_u32 count)
{
	if (!m_BeepCount && count != 0)
	{
		m_BeepCount = count * 2;
		SetBeepGpio(0);
	}
}

void CBeep::SetBeepGpio(vos_u32 i)
{
	string io = "96";
	m_BeepFlag = i;
#ifdef COMPILER_IS_ARM_LINUX_GCC
	a5_ioCtrl(i, io);
#endif
	printf("[BEEP] is %lu\n", i);
}