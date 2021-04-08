#ifndef _ReactCup_h_
#define _ReactCup_h_

#include "task.h"
#include "app_msg.h"
#include <map>

using namespace std;
#define MAX_CUP_NUM 209
#define MAX_WASH_NUM 10

//业务故障
typedef enum
{
    app_alarm_fault_addreagentstop_r1 = 1,//加R1停止
    app_alarm_fault_addsamplestop = 2,//加样停止
    app_alarm_fault_addreagentstop_r2 = 3,//加R2停止
    app_alarm_fault_thetimingstop = 4,//整机时序停止    
}app_alarm_fault_enum;

class CSimuCup;
class CReactCupMng;
class CReactCup
{
public:
	CReactCup(CReactCupMng* pCupMng, int iLgcId);
    ~CReactCup();

public:
    //初始化反应杯
    void Initialize(int iWashCount, bool bFinish = false);
    void InitializeWithId(int iPhyId);

    //分配任务
	void AssignNewTask(CTask * pTask);
    bool CheckSkip();

    bool AssignSkip();
    void AssignNullTaske();
    void AssignWashTask(int washType = 0);
    void AssignWaitWashTask(int washType);
    bool AssignDilOneTask(CTask* pTask);
    bool AssignWBTask(CTask* pTask);
    void AssignDilTwoTask(CTask* pTask);

	bool StartDilTwoTask();

    bool CheckLongWash();

    bool CanAssignTwoTask();
    //获取反应杯状态
    CupState GetActStatus();

    //更新反应杯状态
    int Update();

    //挂起任务
    void SuspendTask(map<app_u16, CTask*>& mSuspendTasks);

	//跳过脏杯
	void SkipDirtyCup();

    //根据反应杯跳过稀释杯
    void SkipDilDirtyCup();

    //根据稀释杯跳过反应杯
    void SkipReactDirtyCup();

    //设置反应杯执行结果
    bool SetActResult(int iResult);

    void GetStatus(STRctCupStatus * pStatus);

    void CheckResultCalc();

    //检查是否清洗
    bool CheckWash();

    //是否需要执行吸排动作
    bool NeedSmpNdlExe(){ return true; }

    //是否需要执行吸排动作
    bool NeedSmpNdlExeDilu(){ return true; }
	  
    //发送样本针吸排动作
    vos_u32 SendSmpNdlExe();

    //发送样本针吸排动作
	vos_u32 SendSmpNdlExeDilu();
	
    bool NeedMoveSmpdsk();
	
	bool NeedMoveSmpdskDilu();

    //发送样本针旋转
	vos_u32 SendSmpNdlRtt(int actType = Act_Move);

	//样本针旋转到反应盘99#稀释位
	vos_u32 SendSmpNdlRttDilu();

    //是否需要R1针执行
    bool NeedR1NdlExe(){ return true; }

    
    //是否需要R2针执行
    bool NeedR2NdlExe(){ return true; }

    //发送试剂针执行动作
    vos_u32 SendReagNdlExeAct(int iDstPId);

    //是否需要转动外盘
    bool NeedMoveOuterDsk();

    //是否需要转动内盘
    bool NeedMoveInnerDsk();

    bool NeedSmpMix();

    bool NeedSmpMixWash();

    bool NeedReagMix();

    bool NeedReagMixWash();

    //发送试剂内盘转动动作
    vos_u32 SendInnerDskRtt();

    //发送试剂内盘转动动作
    vos_u32 SendOuterDskRtt();

    //发送样本盘转动动作
	vos_u32 SendSmpDskRtt(int actType = Act_Move);

    //处理暂停时间
    void ProcSuspendTime(int& iMaxTime);

    //检查针执行是否超时
    void CheckNdlTimeOut(vos_u32 ulFaultId, vos_u32 ulComp);

    //判断杯子是否为空闲
	bool IsNeedOver(){ return (NULL == m_pTask &&NULL == m_pDilTwoTask && 0 == m_iWashCount); }

    //是否处于未执行态
    bool IsDoing(){ return 0 != m_iActCount; }

    //取消任务
    void CancelTask(int iAlarmLevel, STNotifyFailedSmpTask&  stPara);

	void CancelTask(int iLgcId, vos_u32 ulResult);

    void CancelDilTwoTask(int iLgcId, vos_u32 ulResult);
    void CancelDilTask(int iLgcId, vos_u32 ulResult);
    bool CanWash();

    void SetActCount(int iActCount){ m_iActCount = iActCount; }

    bool IsToDoublePeriod();
	bool IsUnstartDilTask();

    bool IsDoublePeriod();

    int InitSimuCup(CSimuCup * pSimuCup);

	void StopPeriod(){ m_iPeriod = 0; m_iWashCount = 0; };
	int  GetPerioid(){return m_iPeriod; };
	void SetTask(CTask* pTask);
	CTask* GetTask(){ return m_pTask;};
	
	void ProcLiquid(TaskResult Trslt, vos_u16 dwnMachine);

	int GetPhyId(){ return m_iPhyId; }

    void CheckSmp();

    vos_u32 GetR1Pos(){ return (m_r1DiskCup <= 50) ? app_reagent_ndl_stop_pos_absrbOut : app_reagent_ndl_stop_pos_absrbIN; }
    vos_u32 GetR2Pos(){ return (m_r2DiskCup <= 50) ? app_reagent_ndl_stop_pos_absrbOut : app_reagent_ndl_stop_pos_absrbIN; }
    string GetPrePrjName(){ return m_prePrjName; }

    bool NeedSmpNdlExe(msg_paraSampleDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stSampleSysAbsorb& stNdllAbsb, msg_stSampleSysDischarge& stNdllDisg);
    bool NeedSmpNdlExeDilu(msg_paraSampleDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stSampleSysAbsorb& stNdllAbsb, msg_stSampleSysDischarge& stNdllDisg, ReactCircle circle);
    bool NeedR1NdlExe(act_stReagSysDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg);
    bool NeedR2NdlExe(act_stReagSysDskRtt& stRDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg);
    void ProcAutoWash(int iResult);
    //处理完成任务
    void ProcFinishTask();
    //处理AD值准备好
    void ProcAdReady(vos_u32 auladval[WAVE_NUM], ReactCircle circle);
    bool CheckClean(){ return (m_iWashCount == 0); }
    int GetWashCount(){ return m_iWashCount; }
private:
	//初始化清洗任务数据
	bool _initWashTask(STReqAddTask &rReq, int washType);
    //发送样本针吸样动作
	vos_u32 _sendSmpNdlAbsorb(int  actType = Act_Absorb);
	
    //发送样本针排样动作
    vos_u32 _sendSmpNdlDischarge();
	
	    //发送样本针排样动作
	vos_u32 _sendSmpNdlDischargeDilu();

    //发送试剂针吸样动作
    vos_u32 _sendReagNdlAbsorb(int iDstPid);

    //发送试剂针吸样动作
    vos_u32 SendReagNdlDischarge(int iDstPid);

	bool _checkLiquid(vos_u32& usR1CupId, vos_u32& usR2CupId, EN_ActFlag& dskFlag);
    //取消任务
    void _cancelTask(STNotifyFailedSmpTask& stPara);

private:
	CReactCupMng* m_pCupMng;       //反应杯管理对象
    int m_iLgcId;                 //逻辑ID  固定不变 第一个为40
    int m_iPhyId;                 //反应杯物理id
    int m_iPeriod;                 //该反应杯对应的测试当前周期数
    int m_iWashCount;                //需要清洗的次数
    int m_iTotalVol;                      //杯总体积
    int  m_iActCount;               //反应杯执行结果
	bool m_bDirtyCup;               //脏杯
    bool m_bSkipCup;                //是否跳过
    string m_prePrjName;            //前一个反应任务
    CTask* m_pTask;                 //加载的任务
	CTask* m_pDilTwoTask;           //稀释第二个任务

    vos_u32 m_r1DiskCup;
    vos_u32 m_r2DiskCup;

    vos_u32 m_cupBlank[WAVE_NUM];
};

class CSimuCup
{
public:
    CSimuCup();
    ~CSimuCup();

public:
    void init(int iLgcId, int iPhyId, int iPeriod, int iWashCount, bool bNeedStart);

    int Update();

    void StartDilTask();

    void ProcFinish(bool bDoublePeriod);

	bool ProcWash();

	bool CanWash(bool bDoublePeriod);

    int GetLgcId(){ return m_iLgcId; }

    bool IsOver();

private:
    int m_iLgcId;
    int m_iPhyId;                 //反应杯物理id
    int m_iPeriod;                 //该反应杯对应的测试当前周期数
    int m_iWashCount;              //需要清洗的次数
    bool m_bNeedStart;            //是否需要启动
};

#endif
