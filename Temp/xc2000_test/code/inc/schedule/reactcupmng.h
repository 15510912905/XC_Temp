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
    //�������з�Ӧ��
    void ResetAllCup();

    bool CheckLongWash(int iPhyId);


	//���ý�����Ⱦ����
	void SetRegPollution(STReqRegPollution* pCross);

	//����������Ⱦ
	void SetSmpPollution(STReqSmpPollution* pSmpPollution);
	
    void SetRctPollution(STRCtRegPollution* pReqCross);

	//�����౭
	void SkipDirtyCup(int iCupId);

    //���ݷ�Ӧ������ϡ�ͱ�
    void SkipDilDirtyCup(int iCupId);

    //����ϡ�ͱ�������Ӧ��
    void SkipReactDirtyCup(int iCupId);

    //�����������
    void CleanSkip();

	//����������
    void AssignNewTask();

    //�ж��Ƿ����
    bool IsNeedOver();

    //������ͣʱ��
    void ProcSuspendTime();

    //��ʼ��Ӧ����ϴ
    void StartWashCup(int iMsgId);

    vos_u32 ProcPreAct();

    //��������ϵͳִ�ж���
    vos_u32 ProcSmpExeAct();
	
	//��������ϵͳϡ��λִ�ж���
	vos_u32 ProcSmpExeActDilu();

    //����R1ϵͳִ�ж���
    vos_u32 ProcR1SysExeAct();

    //����R1ϵͳִ�ж���
    vos_u32 ProcR2SysExeAct();

    //������������ִ�ж���
    vos_u32 ProcSmpMixExeAct();

    //������������ִ�ж���
    vos_u32 ProcReagMixExeAct();

    vos_u32 CheckSmpDsk();
	
	vos_u32 CheckSmpDskDilu();

    vos_u32 CheckOuterDsk();

    vos_u32 CheckInnerDsk();

    //��������ִ�н��
    bool SetActResult(int iPhyId,int iResult);

    //���ͷ�Ӧ��״̬
    void SendReactCupStatus();

    //ȡ������
    void CancelTask(bool bAll= true, int iAlarmLevel = 0, bool bSend = true);

    //�����Ƿ�ʱ
    bool IsActTimeOut(int iPhyId);

    //����Զ���ϴ
    bool CheckAutoWash(app_u16& usWashBit);

    bool CanWash();
    //����������Ԥ������
    vos_u32 SendSmpNdlPreAct();

    //��鶯���Ƿ�ʱ
    void CheckActTimeOut(int iPhyId, vos_u32 ulFaultId, vos_u32 ulComp, vos_u32 ulAct);

    //������Ƿ�ʱ
    void CheckNdlTimeOut();

    //�Ƿ��г�ʱδ��ɵ�����
    bool HasActCupTimeOut(const EN_ActCup *pCups, int ulSize);

    //����ܼ���������Ŀ
    void CheckResultCalc();

	void ProcLiquid(EN_ActCup cup, TaskResult Trslt, vos_u16 dwnMachine);

	//ȡ��ϡ������
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
    //������ϴ���
    void ProcAutoWash();
    //�����������
    void ProcFinishTask();
    //����ADֵ׼����
    vos_u32 ProcAdReady(msg_ADValue* pAdValue);
    //���·�Ӧ��
    void UpdateCups(bool bSuspend);
   //
    CTask* GetTask(app_u16 cup);
    ReactCircle GetCircle(){ return m_enCircle; }
private:
	//�ж��Ƿ���Ҫ������ϴ
	bool _needBatchWash();

	//��������Ⱦ
	bool _checkPollution(vos_u32 dstSmpType, const string& dstPrj, bool bDilut);

	void _procPollution(int nWashCount, int nWashType);

    //�ϱ�ʣ��ʱ��
    void _procLeftTime();

    //����δ���������
    void _procUnHandleTask(STNotifyFailedSmpTask& stPara, bool bAll);

    vos_u32 _sendNdlRttPreAct(app_u16 usDstPos, vos_u32 ulDstPid);

    void _calcLeftTime();
	void _procFinish(map<int, CSimuCup*>& mSimuCups);
	bool _canWash(map<int, CSimuCup*>& mSimuCups);
	bool _procWash(map<int, CSimuCup*>& mSimuCups);

	bool _isNeedOver(map<int, CSimuCup*>& mSimuCups);
    bool _lightIsOut(vos_u32 count, vos_u32(*adValue)[14]);

    //���·�Ӧ��״̬
    void _updateCups(map<app_u16, CTask*>& mSuspendTasks, bool bSuspend);
private:
    vector<CReactCup*> m_vReactCups; //�����߼�ID������
    
    map<int, CReactCup*> m_mReactCups; //��������ID������

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
