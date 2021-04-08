#ifndef _task_h_
#define _task_h_

#include "upmsg.pb.h"
#include "drv_ebi.h"
#include "app_innermsg.h"
#include "app_xc8002shareinfo.h"

#define APP_RTT_RCTCUP_PER_PERIOD     (42)
#define  ONE_REAG_INFO 1  //���Լ�

#define  ONE_REAG_INFO 1  //���Լ�

#define  Pollution_task  7 //������Ⱦ����
#define  DilStartPeriod  73  //ϡ������ʼ������

#define WHOLE_BLOOD (8) //ȫѪ��������

typedef enum
{

    PERIOD_WashEnd_33 = 46,         //33���ڣ���ϴ����
    PERIOD_R1_36 = 47,              //��R1
    PERIOD_Smp_47 = 54,             //����
    PERIOD_SmpMix_50 = 59,          //��������
    PERIOD_DilutInner = 64,        //��Ȧϡ��
    PERIOD_DilutOuter = 94,        //��Ȧϡ��
    PERIOD_DilutEnd_95 = 95,        //ϡ�ͽ���
    PERIOD_R2_97 = 116,             //��R2
    PERIOD_R2Mix_99 = 121,          //R2����
    PERIOD_R3_201 = 256,            //��R3
    PERIOD_R3Mix_215 = 268,         //R3����
    PERIOD_R4_262 = 325,            //��R4
    PERIOD_R4Mix_264 = 330,         //R4����

    PERIOD_R1Dsk_35 = 34,                  //��R1ת��
    PERIOD_SmpDsk_46 = 37,                 //ת��������
    PERIOD_SmpMixWash_49 = 39,             //����������ϴ
    PERIOD_R2Dsk_96 = 87,                  //��R2ת��
    PERIOD_R2MixWash_98 = 89,              //R2����
    PERIOD_END_210 = 210,                  //�������������ʱ���
    PERIOD_R3Dsk_200 = 199,                //��R3ת��
    PERIOD_R3MixWash_214 = 204,            //R3������ϴ
    PERIOD_R4Dsk_361 = 252,                //��R4ת��
    PERIOD_R4MixWash_263 = 254,            //R4������ϴ
    PERIOD_END_331 = 331,                  //˫�����������ʱ���
	PERIOD_SmpDsk_110 = 110,               //ת��������
	PERIOD_Smp_111 = 111,                  //����
}EN_PERIOD;

typedef enum
{
    EN_AC_Dilut = 17,               //ϡ������ķ�Ӧλ

    EN_AC_AddTask = 159,              //�����������  117+42��λ
    EN_AC_Wash1 =1,
    EN_AC_Wash2 = 209,              //2����ϴλ
    EN_AC_Wash3 = 208,              //3����ϴλ
    EN_AC_Wash4 = 207,              //4����ϴλ
    EN_AC_Wash5 = 206,              //5����ϴλ
    EN_AC_Wash6 = 205,              //6����ϴλ
    EN_AC_Wash7 = 204,              //7����ϴλ
    EN_AC_Wash8 = 203,              //8����ϴλ
    EN_AC_Wash9 = 202,              //9����ϴλ
    EN_AC_Wash10 = 201,             //10����ϴλ

    EN_AC_SimpleNdlAbsorb_DILU = 72,    //������Ⱦ���������Ŷ�������30+42
    EN_AC_SimpleNdlAbsorb_DILU_Outer = 66,    //������Ⱦ���������Ŷ�������24+42

    EN_AC_SimpleNdlAbsorb = 74,         //S���Ŷ�������32+42
    EN_AC_R1AbsorbAndDischarge = 159,   //R1���Ŷ�������117+42
    EN_AC_R2AbsorbAndDischarge = 187,   //R2���Ŷ�������145+42
    EN_AC_SimpleMix = 73,               //�������豭��73
    EN_AC_ReagMix = 186,                //�Լ����豭��186

    EN_AC_SimpleNdlToAbsorbPos = 74,    //������ת������λ����
    EN_AC_CrossPollution = 117,      //������Ⱦ��Ɑ��
    EN_AC_MoveR1Dsk = 73,           //ת���Լ����̱���
    EN_AC_R1ToReagPos = 114,            //R1ת�������Լ�λ����
    EN_AC_MoveSimpleDsk = 31,           //ת�������̱���
    EN_AC_FlickerSimpleDsk = 31,        //����ָ�����˸
    EN_AC_SampleMixNdlWash = 113,       //������������ϴ����
    EN_AC_SimpleMixNdlToMixPos = 154,   //����������ת������λ����
    EN_AC_MoveR2Dsk = 101,              //ת���Լ�����λ����
    EN_AC_R2ToReagPos = 142,            //R2ת�������Լ�λ����
    EN_AC_ReagMixNdlWash = 18,          //�Լ���������ϴλ
    EN_AC_ReagMixNdlToMixPos = 59,      //�Լ�������ת������
    EN_AC_SimpleNdlToAbsorbPos_DILU = 95,   //������ת������λ����
    EN_AC_MoveSimpleDsk_DILU = 54,          //ת�������̱���
    EN_AC_Skip = 164,                       //����
}EN_ActCup;


//Ԥ��������Ӧ�ı���
#define  PREACT_CUP_NUM  6
static const EN_ActCup g_preActCup[PREACT_CUP_NUM] =
{
	EN_AC_R1ToReagPos, EN_AC_SimpleNdlToAbsorbPos, EN_AC_SimpleMixNdlToMixPos, EN_AC_R2ToReagPos, EN_AC_ReagMixNdlToMixPos, EN_AC_SimpleNdlToAbsorbPos_DILU
};

//ִ�ж�����Ӧ�ı���,ǰ11�������Ƿ�Ӧϵͳ�쳣
#define  EXEACT_CUP_NUM  15
static const EN_ActCup g_exeActCup[EXEACT_CUP_NUM] =
{
    EN_AC_Wash1, EN_AC_Wash2, EN_AC_Wash3, EN_AC_Wash4, EN_AC_Wash5, EN_AC_Wash6, EN_AC_Wash7, EN_AC_Wash8, EN_AC_AddTask,
	EN_AC_SimpleMix, EN_AC_ReagMix, EN_AC_R1AbsorbAndDischarge, EN_AC_SimpleNdlAbsorb, EN_AC_R2AbsorbAndDischarge, EN_AC_SimpleNdlAbsorb_DILU
};

class CTask
{
public:
	CTask(vos_u32 ulSrcPid, STReqAddTask* pReqInfo);
    ~CTask();

    void PrintTaskInfo();
    void SetAnalyCircle(ReactCircle circleType);
public:
    vos_u32 SendTaskRes(vos_u32 ulResult, int iLgcId);
    CupState GetActStatus(int ulPeriod);
    vos_u32 SetAdValue(vos_u32 adval[WAVE_NUM], app_u16 usPeriod);

	bool GetDilFirstTask(STReqAddTask &stReqInfo);

    bool GetWBFirstTask(STReqAddTask &stReqInfo);
    void SkipDirtyCup();

    void CheckSmpTask(STNotifyFailedSmpTask& stPara);

    void CheckResultCalc(int iLgcId);

    bool IsDoublePeriod();

	bool NeedAddSmp();

	bool NeedAddSmpDilu();

	bool _checkReagent(vos_u32 ulActType, vos_u32 ulReagIndex, vos_u32 ulActSeq, vos_u32& usCupId);

	bool _checkSmpInfo(vos_u32 actSeq);

	bool CheckAllLiquid(vos_u32 actSeq, vos_u32& usR1CupId, vos_u32& usR2CupId, TaskResult& TR_type);
	
	STLiquidInfo* _getReagByIndex(int reagIndex);

    bool NeedAddReag(int reagent);

    const STLiquidInfo& GetReagInfo(int iPeriod);

    void GetCupStatus(STRctCupStatus * pStatus);

    vos_u32 GetSmpAddVol();

    void CalcDiskCup(app_u16 & disk, app_u16 & cup);
    void CalcDiskCup(app_u16 & disk);

    vos_u32 GetTaskId(){ return m_stReqInfo.uitaskid(); }
    vos_u32 GetMaintanType(){ return m_stReqInfo.uimaintantype(); }
    vos_u32 GetSmpType(){ return m_stReqInfo.uismptype(); }
    bool CheckLongWash(){ return (m_stReqInfo.uismptype() == WHOLE_BLOOD && m_ulDilutType == EN_DilutType); }
    const std::string & GetProjectName(){ return m_stReqInfo.strprojectname(); }

    void SetSimulateId(vos_u32 ulSimulateId){ m_ulSimulateId = ulSimulateId; }

	STLiquidInfo * GetSmpInfo(){ return m_stReqInfo.mutable_stsmpinfo(); }

	bool    HasDilutVol();//ϡ�����>0,����ȫѪ����
    bool    CheckWblood(){ return m_stReqInfo.uismptype() == WHOLE_BLOOD; }
	void    SetDilutType(vos_u32 dilutType){ m_ulDilutType = dilutType; }
	vos_u32 GetDilutType(){ return m_ulDilutType; };//��ȡϡ������
	
	vos_u32 GetSrcId(){ return m_ulSrcPid; }
    vos_u32 GetSmpId(){ return m_stReqInfo.uismpid(); }
    vos_u32 GetSmpShelf(){ return m_stReqInfo.stsmpinfo().stcuppos().uidskid(); }
    vos_u32 GetReqSize(){ return m_stReqInfo.streagents_size(); }
    void GetSmpNdlPos(app_u16& usDstPos);
    ReactCircle GetCircle(){ return m_stReqInfo.encircle(); }
private:
    vos_u32 _waveMapToId(vos_u32 enwaveval);

public:
	int m_iDilutPeriod;   //ϡ�ͱ����ڣ�������ͣ����˳��

private:
    vos_u32 m_ulSrcPid;
    STReqAddTask m_stReqInfo;
    STResAddTask m_stResInfo;
    vos_u32 m_ulSimulateId;
    bool m_bSendResult;
	vos_u32 m_ulDilutType;//ϡ����������  0 ��  1 ϡ������ 2 ϡ�Ͷ�Ӧ�ķ�Ӧ����
	vos_u32 m_cupvol;//ϡ�ͱ������
public:

};

#endif
