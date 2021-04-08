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
    //初始化试剂信息
    vos_u32 InitReagent(CMsg& stMsg);
    //处理试剂信息
    vos_u32 ProcReagentMargin(vos_u32 dskId, vos_u32 cupId,vos_u32 &marginDepth);   

    //检查试剂是否有效, 如果当前杯子无效，可检查同盘中的其他试剂
    bool CheckReagInfo(vos_u32 dskId, vos_u32& cupId, vos_u32 ReType, const string& reagName);

    //增加不足样本
    void AddSmpUnEngh(vos_u32 cupId);

    //检测不足样本
    bool CheckSmp(vos_u32 uimaintantype, vos_u32 cupId);

    //清除不足样本
    void ClearUnSmp();

	void DeleteUnSmp(vos_u32 sampid);

    bool GetReagCup(vos_u32 dskId, vos_u32& cupId, const string& strReagName, vos_u32 ReType);
private:
    //更新试剂信息
    vos_u32 _updateReagent(vos_u32 dskId, vos_u32 cupId,vos_u32 marginDepth);
    
    //检查余量是否足够
    bool _isMarginEnough(BottleType btype, vos_u32 marginDepth);

    //查找相同的试剂
	bool _findSameReagent(vos_u32 dskId, vos_u32& cupId, const string& strReagName, vos_u32 ReType);

private:
    map<BottleType, vos_u32> m_stThresholds;     //门限信息
    map<vos_u32, STReagInfo> m_stReagents;   //试剂信息
	void* m_mutex;
	set<vos_u32> m_stSmpLose;
};

extern CReagent * g_pReagent;

#endif
