#ifndef _task_h_
#define _task_h_

#include "upmsg.pb.h"
#include "drv_ebi.h"
#include "app_innermsg.h"
#include "app_xc8002shareinfo.h"

#define APP_RTT_RCTCUP_PER_PERIOD     (42)
#define  ONE_REAG_INFO 1  //单试剂

#define  ONE_REAG_INFO 1  //单试剂

#define  Pollution_task  7 //交叉污染任务
#define  DilStartPeriod  73  //稀释任务开始周期数

#define WHOLE_BLOOD (8) //全血任务类型

typedef enum
{

    PERIOD_WashEnd_33 = 46,         //33周期，清洗结束
    PERIOD_R1_36 = 47,              //加R1
    PERIOD_Smp_47 = 54,             //加样
    PERIOD_SmpMix_50 = 59,          //样本搅拌
    PERIOD_DilutInner = 64,        //内圈稀释
    PERIOD_DilutOuter = 94,        //外圈稀释
    PERIOD_DilutEnd_95 = 95,        //稀释结束
    PERIOD_R2_97 = 116,             //加R2
    PERIOD_R2Mix_99 = 121,          //R2搅拌
    PERIOD_R3_201 = 256,            //加R3
    PERIOD_R3Mix_215 = 268,         //R3搅拌
    PERIOD_R4_262 = 325,            //加R4
    PERIOD_R4Mix_264 = 330,         //R4搅拌

    PERIOD_R1Dsk_35 = 34,                  //加R1转盘
    PERIOD_SmpDsk_46 = 37,                 //转动样本盘
    PERIOD_SmpMixWash_49 = 39,             //样本搅拌清洗
    PERIOD_R2Dsk_96 = 87,                  //加R2转盘
    PERIOD_R2MixWash_98 = 89,              //R2搅拌
    PERIOD_END_210 = 210,                  //单周期任务结束时间点
    PERIOD_R3Dsk_200 = 199,                //加R3转盘
    PERIOD_R3MixWash_214 = 204,            //R3搅拌清洗
    PERIOD_R4Dsk_361 = 252,                //加R4转盘
    PERIOD_R4MixWash_263 = 254,            //R4搅拌清洗
    PERIOD_END_331 = 331,                  //双周期任务结束时间点
	PERIOD_SmpDsk_110 = 110,               //转动样本盘
	PERIOD_Smp_111 = 111,                  //加样
}EN_PERIOD;

typedef enum
{
    EN_AC_Dilut = 17,               //稀释任务的反应位

    EN_AC_AddTask = 159,              //添加新任务杯子  117+42杯位
    EN_AC_Wash1 =1,
    EN_AC_Wash2 = 209,              //2号清洗位
    EN_AC_Wash3 = 208,              //3号清洗位
    EN_AC_Wash4 = 207,              //4号清洗位
    EN_AC_Wash5 = 206,              //5号清洗位
    EN_AC_Wash6 = 205,              //6号清洗位
    EN_AC_Wash7 = 204,              //7号清洗位
    EN_AC_Wash8 = 203,              //8号清洗位
    EN_AC_Wash9 = 202,              //9号清洗位
    EN_AC_Wash10 = 201,             //10号清洗位

    EN_AC_SimpleNdlAbsorb_DILU = 72,    //交叉污染样本针吸排动作杯子30+42
    EN_AC_SimpleNdlAbsorb_DILU_Outer = 66,    //交叉污染样本针吸排动作杯子24+42

    EN_AC_SimpleNdlAbsorb = 74,         //S吸排动作杯子32+42
    EN_AC_R1AbsorbAndDischarge = 159,   //R1吸排动作杯子117+42
    EN_AC_R2AbsorbAndDischarge = 187,   //R2吸排动作杯子145+42
    EN_AC_SimpleMix = 73,               //样本搅拌杯子73
    EN_AC_ReagMix = 186,                //试剂搅拌杯子186

    EN_AC_SimpleNdlToAbsorbPos = 74,    //样本针转到吸样位杯子
    EN_AC_CrossPollution = 117,      //交叉污染检测杯子
    EN_AC_MoveR1Dsk = 73,           //转动试剂外盘杯子
    EN_AC_R1ToReagPos = 114,            //R1转动到吸试剂位杯子
    EN_AC_MoveSimpleDsk = 31,           //转动样本盘杯子
    EN_AC_FlickerSimpleDsk = 31,        //样本指针灯闪烁
    EN_AC_SampleMixNdlWash = 113,       //样本搅拌针清洗杯子
    EN_AC_SimpleMixNdlToMixPos = 154,   //样本搅拌针转到搅拌位杯子
    EN_AC_MoveR2Dsk = 101,              //转动试剂内盘位杯子
    EN_AC_R2ToReagPos = 142,            //R2转动到吸试剂位杯子
    EN_AC_ReagMixNdlWash = 18,          //试剂搅拌针清洗位
    EN_AC_ReagMixNdlToMixPos = 59,      //试剂搅拌针转动杯子
    EN_AC_SimpleNdlToAbsorbPos_DILU = 95,   //样本针转到吸样位杯子
    EN_AC_MoveSimpleDsk_DILU = 54,          //转动样本盘杯子
    EN_AC_Skip = 164,                       //跳过
}EN_ActCup;


//预备动作对应的杯子
#define  PREACT_CUP_NUM  6
static const EN_ActCup g_preActCup[PREACT_CUP_NUM] =
{
	EN_AC_R1ToReagPos, EN_AC_SimpleNdlToAbsorbPos, EN_AC_SimpleMixNdlToMixPos, EN_AC_R2ToReagPos, EN_AC_ReagMixNdlToMixPos, EN_AC_SimpleNdlToAbsorbPos_DILU
};

//执行动作对应的杯子,前11个动作是反应系统异常
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

	bool    HasDilutVol();//稀释体积>0,或者全血任务
    bool    CheckWblood(){ return m_stReqInfo.uismptype() == WHOLE_BLOOD; }
	void    SetDilutType(vos_u32 dilutType){ m_ulDilutType = dilutType; }
	vos_u32 GetDilutType(){ return m_ulDilutType; };//获取稀释任务
	
	vos_u32 GetSrcId(){ return m_ulSrcPid; }
    vos_u32 GetSmpId(){ return m_stReqInfo.uismpid(); }
    vos_u32 GetSmpShelf(){ return m_stReqInfo.stsmpinfo().stcuppos().uidskid(); }
    vos_u32 GetReqSize(){ return m_stReqInfo.streagents_size(); }
    void GetSmpNdlPos(app_u16& usDstPos);
    ReactCircle GetCircle(){ return m_stReqInfo.encircle(); }
private:
    vos_u32 _waveMapToId(vos_u32 enwaveval);

public:
	int m_iDilutPeriod;   //稀释杯周期，用于暂停保持顺序

private:
    vos_u32 m_ulSrcPid;
    STReqAddTask m_stReqInfo;
    STResAddTask m_stResInfo;
    vos_u32 m_ulSimulateId;
    bool m_bSendResult;
	vos_u32 m_ulDilutType;//稀释任务类型  0 无  1 稀释任务 2 稀释对应的反应任务
	vos_u32 m_cupvol;//稀释杯子体积
public:

};

#endif
