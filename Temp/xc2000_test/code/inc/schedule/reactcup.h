#ifndef _ReactCup_h_
#define _ReactCup_h_

#include "task.h"
#include "app_msg.h"
#include <map>

using namespace std;
#define MAX_CUP_NUM 209
#define MAX_WASH_NUM 10

//ҵ�����
typedef enum
{
    app_alarm_fault_addreagentstop_r1 = 1,//��R1ֹͣ
    app_alarm_fault_addsamplestop = 2,//����ֹͣ
    app_alarm_fault_addreagentstop_r2 = 3,//��R2ֹͣ
    app_alarm_fault_thetimingstop = 4,//����ʱ��ֹͣ    
}app_alarm_fault_enum;

class CSimuCup;
class CReactCupMng;
class CReactCup
{
public:
	CReactCup(CReactCupMng* pCupMng, int iLgcId);
    ~CReactCup();

public:
    //��ʼ����Ӧ��
    void Initialize(int iWashCount, bool bFinish = false);
    void InitializeWithId(int iPhyId);

    //��������
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
    //��ȡ��Ӧ��״̬
    CupState GetActStatus();

    //���·�Ӧ��״̬
    int Update();

    //��������
    void SuspendTask(map<app_u16, CTask*>& mSuspendTasks);

	//�����౭
	void SkipDirtyCup();

    //���ݷ�Ӧ������ϡ�ͱ�
    void SkipDilDirtyCup();

    //����ϡ�ͱ�������Ӧ��
    void SkipReactDirtyCup();

    //���÷�Ӧ��ִ�н��
    bool SetActResult(int iResult);

    void GetStatus(STRctCupStatus * pStatus);

    void CheckResultCalc();

    //����Ƿ���ϴ
    bool CheckWash();

    //�Ƿ���Ҫִ�����Ŷ���
    bool NeedSmpNdlExe(){ return true; }

    //�Ƿ���Ҫִ�����Ŷ���
    bool NeedSmpNdlExeDilu(){ return true; }
	  
    //�������������Ŷ���
    vos_u32 SendSmpNdlExe();

    //�������������Ŷ���
	vos_u32 SendSmpNdlExeDilu();
	
    bool NeedMoveSmpdsk();
	
	bool NeedMoveSmpdskDilu();

    //������������ת
	vos_u32 SendSmpNdlRtt(int actType = Act_Move);

	//��������ת����Ӧ��99#ϡ��λ
	vos_u32 SendSmpNdlRttDilu();

    //�Ƿ���ҪR1��ִ��
    bool NeedR1NdlExe(){ return true; }

    
    //�Ƿ���ҪR2��ִ��
    bool NeedR2NdlExe(){ return true; }

    //�����Լ���ִ�ж���
    vos_u32 SendReagNdlExeAct(int iDstPId);

    //�Ƿ���Ҫת������
    bool NeedMoveOuterDsk();

    //�Ƿ���Ҫת������
    bool NeedMoveInnerDsk();

    bool NeedSmpMix();

    bool NeedSmpMixWash();

    bool NeedReagMix();

    bool NeedReagMixWash();

    //�����Լ�����ת������
    vos_u32 SendInnerDskRtt();

    //�����Լ�����ת������
    vos_u32 SendOuterDskRtt();

    //����������ת������
	vos_u32 SendSmpDskRtt(int actType = Act_Move);

    //������ͣʱ��
    void ProcSuspendTime(int& iMaxTime);

    //�����ִ���Ƿ�ʱ
    void CheckNdlTimeOut(vos_u32 ulFaultId, vos_u32 ulComp);

    //�жϱ����Ƿ�Ϊ����
	bool IsNeedOver(){ return (NULL == m_pTask &&NULL == m_pDilTwoTask && 0 == m_iWashCount); }

    //�Ƿ���δִ��̬
    bool IsDoing(){ return 0 != m_iActCount; }

    //ȡ������
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
    //�����������
    void ProcFinishTask();
    //����ADֵ׼����
    void ProcAdReady(vos_u32 auladval[WAVE_NUM], ReactCircle circle);
    bool CheckClean(){ return (m_iWashCount == 0); }
    int GetWashCount(){ return m_iWashCount; }
private:
	//��ʼ����ϴ��������
	bool _initWashTask(STReqAddTask &rReq, int washType);
    //������������������
	vos_u32 _sendSmpNdlAbsorb(int  actType = Act_Absorb);
	
    //������������������
    vos_u32 _sendSmpNdlDischarge();
	
	    //������������������
	vos_u32 _sendSmpNdlDischargeDilu();

    //�����Լ�����������
    vos_u32 _sendReagNdlAbsorb(int iDstPid);

    //�����Լ�����������
    vos_u32 SendReagNdlDischarge(int iDstPid);

	bool _checkLiquid(vos_u32& usR1CupId, vos_u32& usR2CupId, EN_ActFlag& dskFlag);
    //ȡ������
    void _cancelTask(STNotifyFailedSmpTask& stPara);

private:
	CReactCupMng* m_pCupMng;       //��Ӧ���������
    int m_iLgcId;                 //�߼�ID  �̶����� ��һ��Ϊ40
    int m_iPhyId;                 //��Ӧ������id
    int m_iPeriod;                 //�÷�Ӧ����Ӧ�Ĳ��Ե�ǰ������
    int m_iWashCount;                //��Ҫ��ϴ�Ĵ���
    int m_iTotalVol;                      //�������
    int  m_iActCount;               //��Ӧ��ִ�н��
	bool m_bDirtyCup;               //�౭
    bool m_bSkipCup;                //�Ƿ�����
    string m_prePrjName;            //ǰһ����Ӧ����
    CTask* m_pTask;                 //���ص�����
	CTask* m_pDilTwoTask;           //ϡ�͵ڶ�������

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
    int m_iPhyId;                 //��Ӧ������id
    int m_iPeriod;                 //�÷�Ӧ����Ӧ�Ĳ��Ե�ǰ������
    int m_iWashCount;              //��Ҫ��ϴ�Ĵ���
    bool m_bNeedStart;            //�Ƿ���Ҫ����
};

#endif
