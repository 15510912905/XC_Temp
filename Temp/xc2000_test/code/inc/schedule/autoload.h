#ifndef  _AUTOLOAD
#define  _AUTOLOAD
#include <map>
#include "taskmng.h"
#include "vos_timer.h"
#include "apputils.h"
#include "subActMng.h"
#include "vos_pid.h"
#include "subsys.h"
#include <queue>

//MSG_LoadNTermination 动作自动中断
typedef enum{
	STATE_CAutoLoad_IDLE  =0,   //待机
    STATE_CAutoLoad_LOAD  =1,   //装载
	STATE_CAutoLoad_ERROR =2,   //异常
}CAutoLoadStateEunm;
typedef enum 
{
    STATE_TranSport_L1 = 0,
    STATE_TranSport_R1 = 1,
    STATE_TranSport_UNLOAD=2,
    STATE_TranSport_R2=3,
    STATE_TranSport_L2=4,
}CTranSportState;
typedef enum 
{
    Action_Grap = 0,
    Action_Free = 1,
}CVGAction;
typedef enum 
{
    Position_L=0,
    Position_R=1,
    Position_F=2,
}CVGPosition;
typedef enum 
{
    INSTRANCE_RESET =0,
    INSTRANCE_R     =1,
    INSTRANCE_Free  =2,
    INSTRANCE_L     =3,
    INSTRANCE_R1    =4,
}CTrancePort;
class CAutoLoad:public CSubActMng
{
public:
	//实现CAutoLoad模块的初始化，设置CAutoLoad模块的PID，
	CAutoLoad();
    //析构CAutoLoad模块
    ~CAutoLoad();
	public:
    //动作初始化
    void _initActFuc();
	//消息接收接口 在此接口函数中处理来自上位机、中位机、下位机的消息类型。
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);
    //复位响应接口
    vos_u32 _onResetRes();
    //处理停止请求接口
    vos_u32 _procStopReq();
    //处理超时接口
    vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);
    //处理下位机响应接口
    vos_u32 _proResponse(vos_msg_header_stru* pResMsg);
    //上报CAutoLoad状态
    void _reprotState(app_u32 state);
    //开始装载接口
    void _startLoad();
    //复位
    vos_u32 _singleAct(app_u32 subSys, app_u16 actType);
    //定位
    vos_u32 _vgMove(app_u32 subSys, app_u16 actType);
    vos_u32 _L1Move(app_u32 subSys, app_u16 actType);
    vos_u32 _transportMove(app_u32 subSys, app_u16 actType);
    vos_u32 _L2Move(app_u32 subSys, app_u16 actType);
    vos_u32 _R1Move(app_u32 subSys, app_u16 actType);
    vos_u32 _R2Move(app_u32 subSys, app_u16 actType);
    void startAutoLoad();
    //处理条码扫描请求
    vos_u32 _procBcScanReq(vos_msg_header_stru * pReqMsg);
    //任务信息接收
    vos_u32 AddTaskInfo(CTask * pTask);
    //处理试剂信息
	vos_u32 _updateReagent(vos_msg_header_stru* pMsg);
    //更新试剂信息
    vos_u32 _updateRReagent(vos_u32 dskId, vos_u32 cupId, vos_u32 marginDepth);
    //条码扫描
    vos_u32 _L1Scan(msg_stComDevActInfo* pComAct);
    vos_u32 _L2Scan(msg_stComDevActInfo* pComAct);
    //初始化试剂信息
    vos_u32 InitReagent(CMsg& stMsg);
    //处理装载是否生效
    vos_u32 _setLoadSign(vos_msg_header_stru* pMsg);
	//下位机动作正常终止
 	vos_u32 _loadNormalTermination(vos_msg_header_stru* pMsg);
	//(注：正常终止后，更新条码，试剂信息。重新定位。继续装载。时间允许的情况下)
    //终止正在进行的任务
	void  _stopNowTask(); 
	//清空所有装卸任务
	void _clearAllTask();
    //对比试剂信息和任务信息
    void _compareReagent();
    //获取轮空标志
	bool GetWheelset(){return wheelset;}
    //获取调试信息-L盘试剂信息
    void GetDebugLreagent(STNotifyReagInfo info);
    //获取调试信息-R盘试剂信息
    void GetDebugRreagent(STNotifyReagInfo info);
    //是否需要轮空接口
    bool NeedWheelset(CTask* ntask, CTask*& wTask);
	private:
    bool _readfile_volumeStandard();              //读取配置文件瓶型-体积
	bool                     LoadSign; 			  //装载标志
	map<vos_u32, STReagInfo> m_LReagents;         //自动装载L盘上已有的试剂信息和对应位置
	queue <STReagInfo>       m_LReplaceReagents;  //自动装载L盘上需要更换的试剂信息
	queue <STReagInfo>       m_RReagents;         //R盘需要更换试剂信息
	vos_u32                  m_State;			  //状态
	STReagInfo               nowRReagents;        //现在正在替换的R试剂信息
	STReagInfo               nowLReagents;        //现在正在替换的L试剂信息
	bool                     wheelset;            //轮空标志
    vos_u32                  m_load_instrance;    //动作注册的顺序
    queue<CTask*>            m_qTasks;            //任务队列
    map<BottleType, vos_u32> m_stThresholds;      //门限信息
    map<vos_u32, STReagInfo> m_stReagents;        //试剂信息
    map<vos_u32, float>    standtrd_volume;     //液面高度(毫米)-吸液量
    map<vos_u32, float>    micro_volume;
    map<vos_u32, float>    middle_volume;
};
extern CAutoLoad* g_pAutoLoad;
#endif