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
	FaDownSmpDisk = 0,		//�����̵�Ԫ
	FaDownSmpNdl = 1,		//�����뵥Ԫ
	FaDownReagDiskIn = 2,   //��Ȧ�Լ��̵�Ԫ
	FaDownR1Ndl = 3,		//R1�Լ��뵥Ԫ
	FaDownReagDiskOut = 4,  //��Ȧ�Լ��̵�Ԫ
	FaDownR2Ndl = 5,		//R2�Լ��뵥Ԫ
	FaDownReactDisk = 6,	//��Ӧ�̵�Ԫ
	FaDownAutoWash = 7,		//�Զ���ϴ��Ԫ
	FaDownSmpMix = 8,		//��������˵�Ԫ
	FaDownReagMix = 9,		//�Լ�����˵�Ԫ
	FaDownTempCtrl = 10,	//�Զ��¿ص�Ԫ
	FaDownReagCool = 11,	//�Լ����䵥Ԫ

	FaDownButt
}FaDownId;

//��λ������ID
typedef enum
{
	ALarm_AbsorbAir = 10110,	//����
	ALarm_AbsorbAirPos = 34000,	//	���� + ��λ��
    ALarm_R1_System = 31001,    //R1ϵͳ�쳣
	ALarm_R2_System = 31002,    //R2ϵͳ�쳣
	ALarm_Smp_System = 31003,   //����ϵͳ�쳣
	ALarm_React_System = 31004,   //��Ӧϵͳ�쳣

	ALarm_Smp_Margin = 31007,   //������������
	ALarm_Wash_Margin = 31008,  //��ϴҺ��������
    ALarm_Light = 31009,  //��Դ���쳣
	ALarm_Dwn_Act_Failed = 32000,   //��λ������ִ��ʧ�� + ��λ����
	ALarm_Dwn_Timeout = 33000,   //��λ��������ʱ	+ ��λ����

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

    //�澯ģ���ʼ��
    vos_u32 Initialize();

    //��������
    void GenerateAlarm(vos_u32 ulAlarmId, vos_u32 ulPara1, vos_u32 ulPara2 = 0);

    //�ָ�����
    void RecoverAlarm(vos_u32 ulAlarmId, vos_u32 ulPara1, vos_u32 ulPara2 = 0);

    //ͬ������
    void SynAlarm();

    //��鱨��
    vos_u32 FindMaxLevelAlarm();

    //�����¶ȱ�������λ�����еĹ��˱�ʶ
    void SetFilterInReset(bool isFilter){ m_isFilterInReset = isFilter; }

	bool HasAlarm(vos_u32 alarmType);

private:
    void _generateAlarm(const STAlarmInfo& alarm);

    //������������Ƿ���ͬ
    bool _isEqualAlarm(const STAlarmInfo& alarm1, const STAlarmInfo& alarm2);

    //֪ͨ��λ���澯��Ϣ
    void _notifyAlarm(const STAlarmInfo& alarm, vos_u32 ulActType);

    //ɾ������
    vos_u32 _deleteAlarm(vos_msg_header_stru* pMsg);
    void _deleteAlarm(const STAlarmInfo& alarm);
    
    //�Ƿ���Ҫ����
    bool _isNeedFilter(vos_u32 ulAlarmId);
private:
    void* m_mutex;
    vector<STAlarmInfo>    m_activeAlarms;   //�����
    map<vos_u32, CAlarmInfo> m_allAlarm;   //ȫ������

    bool m_isFilterInReset;
	CBeep m_beep;
	vos_u32 m_ulTimerId;  
	set<vos_u32> m_alarm;
};

extern CAlarm* g_pAlarm;
#endif
