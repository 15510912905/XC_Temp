#ifndef _Reagent_h_
#define _Reagent_h_

#include "upmsg.pb.h"
#include "msg.h"
#include <map>

using namespace std;
enum
{
    ReType_R1 = 1,
    ReType_R2 = 2,
    ReType_R3 = 3,
    ReType_R4 = 4,
};
enum
{
    ReagDisk_R1 = 1,
    ReagDisk_R2 = 2,
};

class CReagent
{
public:
    CReagent();
    ~CReagent();

public:
    //��ʼ���Լ���Ϣ
    vos_u32 InitReagent(CMsg& stMsg);
    //�����Լ���Ϣ
    vos_u32 ProcReagentMargin(vos_u32 dskId, vos_u32 cupId,vos_u32 &marginDepth);   

    //����Լ��Ƿ���Ч, �����ǰ������Ч���ɼ��ͬ���е������Լ�
    bool CheckReagInfo(vos_u32 dskId, vos_u32& cupId, vos_u32 ReType, const string& reagName);

    //���Ӳ�������
    void AddSmpUnEngh(vos_u32 cupId);

    //��ⲻ������
    bool CheckSmp(vos_u32 uimaintantype, vos_u32 cupId);

    //�����������
    void ClearUnSmp();

	void DeleteUnSmp(vos_u32 sampid);

    bool GetReagCup(vos_u32 dskId, vos_u32& cupId, const string& strReagName, vos_u32 ReType);
private:
    //�����Լ���Ϣ
    vos_u32 _updateReagent(vos_u32 dskId, vos_u32 cupId,vos_u32 marginDepth);
    
    //��������Ƿ��㹻
    bool _isMarginEnough(BottleType btype, vos_u32 marginDepth);

    //������ͬ���Լ�
	bool _findSameReagent(vos_u32 dskId, vos_u32& cupId, const string& strReagName, vos_u32 ReType);

private:
    map<BottleType, vos_u32> m_stThresholds;     //������Ϣ
    map<vos_u32, STReagInfo> m_stReagents;   //�Լ���Ϣ
	void* m_mutex;
	set<vos_u32> m_stSmpLose;
};

extern CReagent * g_pReagent;

#endif
