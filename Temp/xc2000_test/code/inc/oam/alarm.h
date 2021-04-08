#ifndef __ALARM_H__
#define __ALARM_H__

#include "vos_pid.h"
#include <vector>
#include <map>
#include "compensation.pb.h"
#include "upmsg.pb.h"
#include <stdio.h>
using namespace std;

typedef enum
{
	FaDownSmpDisk = 0,		//样本盘单元
	FaDownSmpNdl = 1,		//样本针单元
	FaDownReagDiskIn = 2,   //内圈试剂盘单元
	FaDownR1Ndl = 3,		//R1试剂针单元
	FaDownReagDiskOut = 4,  //外圈试剂盘单元
	FaDownR2Ndl = 5,		//R2试剂针单元
	FaDownReactDisk = 6,	//反应盘单元
	FaDownAutoWash = 7,		//自动清洗单元
	FaDownSmpMix = 8,		//样本搅拌杆单元
	FaDownReagMix = 9,		//试剂搅拌杆单元
	FaDownTempCtrl = 10,	//自动温控单元
	FaDownReagCool = 11,	//试剂制冷单元

	FaDownButt
}FaDownId;

//中位机报警ID
typedef enum
{
	ALarm_AbsorbAir = 10110,	//空吸
	ALarm_AbsorbAirPos = 34000,	//	空吸 + 杯位号
    ALarm_R1_System = 31001,    //R1系统异常
	ALarm_R2_System = 31002,    //R2系统异常
	ALarm_Smp_System = 31003,   //样本系统异常
	ALarm_React_System = 31004,   //反应系统异常

	ALarm_Smp_Margin = 31007,   //样本余量不足
	ALarm_Wash_Margin = 31008,  //清洗液余量不足
    ALarm_Light = 31009,  //光源灯异常
	ALarm_Dwn_Act_Failed = 32000,   //下位机动作执行失败 + 下位机号
	ALarm_Dwn_Timeout = 33000,   //下位机动作超时	+ 下位机号

}EN_Alarm_ID;

extern void a5_ioCtrl(int value, string gpio);
class CBeep
{
public:
	void SetBeepGpio(vos_u32 i);

	bool IsNeedPlayBeep()
	{
		return GetBeepCount();
	}
	vos_u32 GetBeepCount() const
	{
		return m_BeepCount;
	}
	void UpdateBeepCount()
	{
		m_BeepCount = m_BeepCount > 0 ? --m_BeepCount : m_BeepCount;
	}
	void ConvertBeep()
	{
		SetBeepGpio(!m_BeepFlag);
	}
	vos_u32 ParseBeepAlarm(vos_msg_header_stru* pMsg);
	void CheckBeepAlarm(vos_u32 level);
	void PlayBeepCount(vos_u32 count);
private:
	void _setBeepCount(const vos_u32 count);
	vos_u32 m_BeepCount;
	vos_u32 m_BeepFlag;
	vos_u32 m_EnbaleFlag;
	map<vos_u32, vos_u32> m_mBeepAlarmCfg;
};

class CAlarm : public CCallBack
{
    typedef vector<STAlarmInfo>::iterator CAIter;
public:
    CAlarm(vos_u32 ulPid);
    ~CAlarm();

public:

    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

    //告警模块初始化
    vos_u32 Initialize();

    //产生报警
    void GenerateAlarm(vos_u32 ulAlarmId, vos_u32 ulPara1, vos_u32 ulPara2 = 0);

    //恢复报警
    void RecoverAlarm(vos_u32 ulAlarmId, vos_u32 ulPara1, vos_u32 ulPara2 = 0);

    //同步报警
    void SynAlarm();

    //检查报警
    vos_u32 FindMaxLevelAlarm();

    //设置温度报警整起复位过程中的过滤标识
    void SetFilterInReset(bool isFilter){ m_isFilterInReset = isFilter; }

	bool HasAlarm(vos_u32 alarmType);

private:
    void _generateAlarm(const STAlarmInfo& alarm);

    //检查两个报警是否相同
    bool _isEqualAlarm(const STAlarmInfo& alarm1, const STAlarmInfo& alarm2);

    //通知上位机告警信息
    void _notifyAlarm(const STAlarmInfo& alarm, vos_u32 ulActType);

    //删除报警
    vos_u32 _deleteAlarm(vos_msg_header_stru* pMsg);
    void _deleteAlarm(const STAlarmInfo& alarm);
    
    //是否需要过滤
    bool _isNeedFilter(vos_u32 ulAlarmId);
private:
    void* m_mutex;
    vector<STAlarmInfo>    m_activeAlarms;   //活动报警
    map<vos_u32, CAlarmInfo> m_allAlarm;   //全部报警

    bool m_isFilterInReset;
	CBeep m_beep;
	vos_u32 m_ulTimerId;  
	set<vos_u32> m_alarm;
};

extern CAlarm* g_pAlarm;
#endif
