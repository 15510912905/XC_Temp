#ifndef _ReactCupMng_h_
#define _ReactCupMng_h_

#include "reactcup.h"
#include <vector>

using namespace std;

class CReactCupMng
{
public:
    CReactCupMng(ReactCircle circle);
    ~CReactCupMng();


public:
    //重置所有反应杯
    void ResetAllCup();

    bool CheckLongWash(int iPhyId);


	//设置交叉污染数据
	void SetRegPollution(STReqRegPollution* pCross);

	//设置样本污染
	void SetSmpPollution(STReqSmpPollution* pSmpPollution);
	
    void SetRctPollution(STRCtRegPollution* pReqCross);

	//跳过脏杯
	void SkipDirtyCup(int iCupId);

    //根据反应杯跳过稀释杯
    void SkipDilDirtyCup(int iCupId);

    //根据稀释杯跳过反应杯
    void SkipReactDirtyCup(int iCupId);

    //清理跳过标记
    void CleanSkip();

	//分配新任务
    void AssignNewTask();

    //判断是否结束
    bool IsNeedOver();

    //处理暂停时间
    void ProcSuspendTime();

    //开始反应杯清洗
    void StartWashCup(int iMsgId);

    vos_u32 ProcPreAct();

    //处理样本系统执行动作
    vos_u32 ProcSmpExeAct();
	
	//处理样本系统稀释位执行动作
	vos_u32 ProcSmpExeActDilu();

    //处理R1系统执行动作
    vos_u32 ProcR1SysExeAct();

    //处理R1系统执行动作
    vos_u32 ProcR2SysExeAct();

    //处理样本搅拌执行动作
    vos_u32 ProcSmpMixExeAct();

    //处理样本搅拌执行动作
    vos_u32 ProcReagMixExeAct();

    vos_u32 CheckSmpDsk();
	
	vos_u32 CheckSmpDskDilu();

    vos_u32 CheckOuterDsk();

    vos_u32 CheckInnerDsk();

    //设置任务执行结果
    bool SetActResult(int iPhyId,int iResult);

    //发送反应杯状态
    void SendReactCupStatus();

    //取消任务
    void CancelTask(bool bAll= true, int iAlarmLevel = 0, bool bSend = true);

    //动作是否超时
    bool IsActTimeOut(int iPhyId);

    //检查自动清洗
    bool CheckAutoWash(app_u16& usWashBit);

    bool CanWash();
    //发送样本针预备动作
    vos_u32 SendSmpNdlPreAct();

    //检查动作是否超时
    void CheckActTimeOut(int iPhyId, vos_u32 ulFaultId, vos_u32 ulComp, vos_u32 ulAct);

    //检查针是否超时
    void CheckNdlTimeOut();

    //是否有超时未完成的任务
    bool HasActCupTimeOut(const EN_ActCup *pCups, int ulSize);

    //检查能计算结果的项目
    void CheckResultCalc();

	void ProcLiquid(EN_ActCup cup, TaskResult Trslt, vos_u16 dwnMachine);

	//取消稀释任务
	void CancelDilTask(int iLgcId, int nResult);

    void CancelReactTask(int iLgcId, int nResult);
    void SetBloodAbsorbMode(bool mode);

	bool GetBloodAbsorbMode();
    void CheckSmp();
    bool NeedSmpNdlAct(msg_paraSampleDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stSampleSysAbsorb& stNdllAbsb, msg_stSampleSysDischarge& stNdllDisg);
    bool NeedR1NdlAct(act_stReagSysDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg);
    bool NeedR2NdlAct(act_stReagSysDskRtt& stRDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg);
    bool NeedSMixAct();
    bool NeedRMixAct();
    bool NeedR1AutoLoad(act_stReagSysDskRtt& stRtt);
    bool NeedR2AutoLoad(act_stReagSysDskRtt& stNdlRtt);
    //设置清洗结果
    void ProcAutoWash();
    //处理完成任务
    void ProcFinishTask();
    //处理AD值准备好
    vos_u32 ProcAdReady(msg_ADValue* pAdValue);
    //更新反应杯
    void UpdateCups(bool bSuspend);
   //
    CTask* GetTask(app_u16 cup);
    ReactCircle GetCircle(){ return m_enCircle; }
private:
	//判断是否需要批次清洗
	bool _needBatchWash();

	//处理交叉污染
	bool _checkPollution(vos_u32 dstSmpType, const string& dstPrj, bool bDilut);

	void _procPollution(int nWashCount, int nWashType);

    //上报剩余时间
    void _procLeftTime();

    //处理未分配的任务
    void _procUnHandleTask(STNotifyFailedSmpTask& stPara, bool bAll);

    vos_u32 _sendNdlRttPreAct(app_u16 usDstPos, vos_u32 ulDstPid);

    void _calcLeftTime();
	void _procFinish(map<int, CSimuCup*>& mSimuCups);
	bool _canWash(map<int, CSimuCup*>& mSimuCups);
	bool _procWash(map<int, CSimuCup*>& mSimuCups);

	bool _isNeedOver(map<int, CSimuCup*>& mSimuCups);
    bool _lightIsOut(vos_u32 count, vos_u32(*adValue)[14]);

    //更新反应杯状态
    void _updateCups(map<app_u16, CTask*>& mSuspendTasks, bool bSuspend);
private:
    vector<CReactCup*> m_vReactCups; //根据逻辑ID的排序
    
    map<int, CReactCup*> m_mReactCups; //根据物理ID的排序

    int m_iLeftTime;
	int m_iBatachWashCount;
	STReqRegPollution m_stReqRegPollution;
	STReqSmpPollution m_stReqSmpPollution;
    STRCtRegPollution m_stRctRegPollution;

	bool m_bloodAbsrobMode;

    ReactCircle m_enCircle;
public:
	bool m_bUpdateCup;
    bool m_bGetAd;
};

extern CReactCupMng * g_pOuterCupMng;
extern CReactCupMng * g_pInnerCupMng;
#endif
