#ifndef _app_inner_msg_h_
#define _app_inner_msg_h_

#include "vos_basictypedefine.h"
#include <stdio.h>
#include <drv_ebi.h>
#include "cmd_downmachine.h"
#include "typedefine.pb.h"


typedef struct
{
    app_u16 dwnName;
    app_u16 AdCount;
    app_u32 value[0];
}msg_ADValue;

//===========动作头结构==========
typedef struct
{
	app_u16 usActType;
	app_u16 usRctCupId;
	app_u16 usDevType;
	app_u16 usSingleRes;
    app_u32 ulActIndex;
}msg_stActHead;

//===========光源灯控制数据结构==========
typedef struct {
    app_u16 lightIntensity;  //光强度 0~255亮度调整
}msg_stLightControl;

//===========温控开关数据结构==========
typedef struct {
    app_u16 switchState;  //开关控制 0-关闭 1-打开
}msg_stTempSwitch;

//三路清洗加热器目标温度
typedef struct {
    vos_u32 temp1;     //加热器1目标温度
    app_u16 temp2;     //加热器2目标温度
    app_u16 temp3;     //加热器3目标温度
}msg_stWTemp;

//反应盘目标温度
typedef struct {
    vos_u32 delta;
    vos_u32 tempreact;  //反应盘目标温度
}msg_stRTemp;

//===========配置AD控制数据结构==========
typedef struct {
    vos_u32 dpValue[WAVE_NUM];  //每个通道对应的AD增益系数
}msg_stAdConfig;

//===========故障恢复命令结构==========
typedef struct {
    vos_u32 faultId;
    app_u16 boardId;
    app_u16 deviceId;
}msg_stRsmFault;

//===========中位机状态下发命令结构==========
typedef struct {
    app_u16 midState;
}msg_stMidState;

//===========定义虚拟示波器开关数据结构==========
typedef struct {
    char  channel;
	char  switch_state;
}msg_stShowave;

//===========试剂条码扫描数据结构==========
typedef struct
{
    app_u16 usCupId;    //试剂杯号
    char  aucrsv[2];
}msg_stReagBcScanPara;

//===========样本条码扫描数据结构==========
typedef struct
{
    app_u16  uccircleid;  //样本盘圈号 (0xFF: 3圈同时扫描)
    app_u16  uccupid;     //样本盘杯号:(0:整圈扫描完再返回数据，1-最大杯号:返回指定杯号数据)
}msg_stSmpBcScanPara;

#ifdef Version_A
typedef struct
{
    app_u16 usCircleId;  //样本盘编号
    app_u16 usSmpCupId; //样本杯编号
}msg_paraSampleDskRtt;
#else
//===========样本盘水平旋转动作数据结构==========
typedef struct
{
    app_u16 usCircleId;  //样本盘编号
    app_u16 usSmpCupId; //样本杯编号
    uint32_t channel;	//通道号 ,0 急诊，1 常规
}msg_paraSampleDskRtt;
#endif

//===========注射泵排空气请求消息==========
typedef struct
{
    app_u16 usAirOutTimes;
    char  aucrsv[2];
}msg_stAirout;

//===========样本针、试剂针转动动作数据结构==========
typedef struct
{
    app_u16 usDstPos;   //目标位置
    app_u16  aucRsv;
}msg_stNdlRtt;

//===========吸样本动作数据结构==========
typedef struct
{
    char  enabsrbtype;    //吸取类型，8：全血
    char  reportmarginflag; //上报余量标志，1:上报，0:不上报
	app_u16 usTotalVol;
    vos_u32 ulsamplevolume; //样本体积(由于样本稀释过程与反应过程一样,这里不带
    app_u16 diskid;   //盘号，用于余量上报
    app_u16 cupid;    //杯位号，用于余量上报
    vos_u32 reactcup;   //任务号，用于误检、漏检处理
}msg_paraSampleAbsrb;

//===========排样本动作数据结构==========
typedef struct
{
    app_u16 usdischpos;     //排样本的位置(吸完样本后要转到排样本位上方)
    app_u16 ustotalvol;     //杯总体积
    vos_u32 ulsamplevolume; //样本体积
    vos_u32 uldlntvolume;   //稀释液体积
}msg_paraSampleDischarge;

//===========齿轮增压泵控制命令数据结构==========
typedef struct
{
    char operateType;
	char rsv;
}msg_stCtrlP04;

//===========排样本动作数据结构==========
typedef struct
{
	app_u16 usDischPos;     //排样本的位置(吸完样本后要转到排样本位上方)
	app_u16 usTotalVol;     //杯总体积
	vos_u32 ulSampleVolume; //样本体积
	vos_u32 ulDlntVolume;   //稀释液体积
}msg_stSampleSysDischarge;

//===========吸样本动作数据结构==========
typedef struct
{
	char  enAbsrbType;    //吸取类型，8：全血
	char  reportMarginFlag; //上报余量标志，1:上报，0:不上报
	app_u16 usTotalVol;  //杯总体积
	vos_u32 ulSampleVolume; //样本体积(由于样本稀释过程与反应过程一样,这里不带 	//稀释液的体积)
	app_u16 diskId;   //盘号，用于余量上报
	app_u16 cupId;    //杯位号，用于余量上报
	vos_u32 reactCup;   //任务号，用于误检、漏检处理
}msg_stSampleSysAbsorb;

//===========排试剂动作数据结构==========
typedef struct
{
	app_u16 usDischPos;  //吸取完后试剂针需要停止的目标位
	app_u16 usTotalVol;  //杯总体积
	vos_u32 ulReagVolume; //排试剂体积
	vos_u32 ulDlntVolume; //排稀释液体积
}msg_stReagSysDischarge;

//===========吸试剂动作数据结构==========
typedef struct
{
	app_u16 usAbsrbPos;   //吸试剂位置(内圈、外圈)
	char  enAbsrbType;  //吸取类型(蒸馏水or试剂)
	char  reportMarginFlag; //上报余量标志，1:上报，0:不上报
	vos_u32 ulVolume;     //吸取体积
	app_u16 diskId;   //盘号，用于余量上报
	app_u16 cupId;    //杯位号，用于余量上报
	vos_u32 reactCup;   //任务号，用于误检、漏检处理
}msg_stReagSysAbsorb;

//===========动作数据信息==========
typedef struct
{
	vos_u32 ulDataLen;  //整个动作数据长度
	msg_stActHead stActHead;
	char  aucPara[0]; //参数多于2字节时保存的参数
}msg_stComDevActInfo;

//===========动作请求消息  app_com_device_req_exe_act_msg_type==========
typedef struct
{
	app_u16 usActNum;
	app_u16 usReserve;
	char astActInfo[0];      //动作信息
}msg_stComDevActReq;

//===========单一动作==========
typedef struct
{
	app_u16 usActNum;
	app_u16 usReserve;
	vos_u32 ulDataLen;  //整个动作数据长度
	msg_stActHead stActHead;  //动作头
	char astActInfo[0];
}msg_stSingleActUni;

//===========电机微步调整动作命令数据结构==========
typedef struct
{
    app_u16 motorType;  //电机类型
    app_u16 dir;        //方向
    vos_u32 step;       //微步数
}msg_stStepMove;

//===========下位机补偿命令数据结构==========
//补偿参数
typedef struct {
    app_u16 location;   //补偿位置
    app_u16 dir;        //补偿方向 1: 顺时针 2:逆时针
    vos_u32 step;       //补偿步数 
}msg_stCmpPara;

//补偿命令
typedef struct {
    vos_u16 type;     //补偿类型  1: 复位补偿  2: 旋转补偿 3: 主备区切换 4: 清除 5:垂直补偿
    vos_u16 paranum;  //补偿参数个数
    msg_stCmpPara pPara[0];  //补偿参数 变长
}msg_stCompensation;

//补偿命令
typedef struct {
    vos_u16 type;     //补偿类型  页码
    vos_u16 paranum;  //补偿参数个数
    vos_u32 pPara[0]; //补偿参数 变长
}msg_stSaveOffset;

//补偿参数
typedef struct {
    unsigned char  drvId;
    unsigned char  lineId;
    app_u16 startFre;			//启动频率
    app_u16 endFre;			//最高频率
    app_u16  echLaderStep;		//每台阶微步
    app_u16  upMaxLader;		//最大加速台阶
    app_u16  sPar;				//S型曲线的形状参数}motor_para;
}msg_stMotorPara;

//补偿命令
typedef struct {
    vos_u32 paranum;  //补偿参数个数
    msg_stMotorPara pPara[0];  //补偿参数 变长
}msg_stMotor;


//===========下位机查询命令数据结构==========
typedef struct
{
    vos_u32 queryType;     //查询的状态类型
}msg_stDwnQuery;
//===========下位机查询命令数据结构==========
typedef struct
{
    char num;
    char queryType;     //查询的状态类型
}msg_st2000DwnQuery;


//===========盘正、逆时针转动数据结构==========
typedef struct
{
    char dskId;     //样本盘圈号,(试剂和反应盘不填)
    char dir;       //方向
    vos_u16 num;    //转动杯位数     
}msg_stDskHrotate;

//===========针垂直运动命令数据结构==========
typedef struct{
    char dir;       //方向,(向上 时下面的数据不用写)
    char marginFlag;//上报余量标志，1:上报，0:不上报
    app_u16 totalVol; //杯总体积
    app_u16 diskId;     //盘号，用于余量上报
    app_u16 cupId;      //杯位号，用于余量上报
    vos_u32 reactCupId;   //任务号，用于误检、漏检处理
}msg_stVrt;

//===========调试-正常态切换==========
typedef struct
{
    vos_u32 operation;    //仪器操作态-0:调试态，1:正常态
}msg_stMachineDebug;

/*typedef enum{
    Press_our = 2,  //外圈增压
    Press_in = 3,   //内圈增压
    p07 = 1,    //废液泵
    p08 = 2,	//主真空泵
    p09 = 3,    //脱气泵
    v06 = 8,    //稀释阀
    v07 = 9,    //S外壁清洗，调试使用
    v08 = 10,   //R1外壁清洗，调试使用
    v09 = 11,   //R2外壁清洗，调试使用
    v10 = 13,   //样本搅拌外壁清洗阀，调试使用
    v11 = 12,   //试剂搅拌外壁清洗阀，调试使用
    v01 = 15,	//进水阀
    v02 = 18,   //S内壁清洗，调试使用
    v03 = 16,   //R1内壁清洗，调试使用
    v04 = 17,   //R2内壁清洗，调试使用
    v05 = 19,   //回流阀
    v14 = 20,   //一阶注液
    v15 = 21,   //二阶注液
    v16 = 22,   //三阶注液
    v17 = 23,   //四阶注液
    v18 = 24,   //五阶注液
    v19 = 25,   //六阶注液
    v21 = 27,	//高浓度排液阀
    v22 = 28,	//低浓度排液阀
    v23 = 29,	//二级真空吸液阀
    v25 = 31,   //二级真空排气阀
    v27 = 33,   //主真空排液阀
    v28 = 34,   //脱气阀
    p04 = 2,    //齿轮增压泵 2000
    p011 = 56,  //直流进水泵
    p06 = 57,   //稀释清洗剂泵
    p05 = 58,   //浓缩清洗剂泵
    p03 = 59,	//去离子水泵
}app_autowash_pvnum_enum;*/
typedef enum{
    Press_our = 2,  //外圈增压
    Press_in = 3,   //内圈增压
    p07 = 34,   //废液泵
    p08 = 32,	//主真空泵
    p09 = 29,   //脱气泵

    v07 = 1,   //S1外壁清洗，调试使用
    v32 = 1,   //S2外壁清洗，
    v08 = 1,   //R11外壁清洗，调试使用
    v34 = 1,   //R12外壁清洗，调试使用
    v09 = 1,   //R21外壁清洗，调试使用
    v38 = 1,   //R22外壁清洗，调试使用
    v10 = 1,   //样本搅拌外壁清洗阀，调试使用
    v11 = 1,   //试剂搅拌外壁清洗阀，调试使用
    v01 = 12,	//进水阀
    v02 = 2,   //S1内壁清洗，调试使用
    v31 = 2,   //S2内壁清洗，
    v03 = 2,   //R11内壁清洗，调试使用
    v35 = 2,   //R12内壁清洗，调试使用
    v04 = 2,   //R21内壁清洗，调试使用
    v37 = 2,   //R22内壁清洗，调试使用
    v05 = 19,   //回流阀
    v06 = 8,    //稀释阀

    v14 = 13,   //内一阶注液
    v15 = 14,   //内二阶注液
    v16 = 15,   //内三阶注液
    v17 = 16,   //内四阶注液
    v18 = 17,   //内五阶注液
    v19 = 18,   //内六阶注液
    v40 = 19,   //外一阶注液
    v41 = 20,   //外二阶注液
    v42 = 21,   //外三阶注液
    v43 = 22,   //外四阶注液
    v44 = 23,   //外五阶注液
    v45 = 24,   //外六阶注液
    v21 = 9,	//高浓度排液阀
    v22 = 10,	//低浓度排液阀
    v23 = 26,	//二级真空吸液阀
    v25 = 31,   //二级真空排气阀
    v27 = 25,   //主真空排液阀
    v46 = 2,   //ISE样本内壁阀
    v47 = 11,   //低浓度排液阀2
    v48 = 27,   //二级真空吸液阀2
    v49 = 28,   //二级真空吸液阀3
    v28 = 34,   //脱气阀

    p04 = 2,   //齿轮增压泵
    p12 = 3,   //齿轮增压泵2
    p13 = 35,   //外置真空泵1
    p14 = 35,   //外置真空泵2
    p15 = 83,   //浓缩清洗剂运转泵
    p16 = 33,   //主真空泵2
    p17 = 85,   //ISE内壁增压泵
    p18 = 86,   //ISE外壁清洗泵
    p19 = 8,   //稀释泵
    p011 = 6,  //外置直流进水泵
    p06 = 5,   //稀释清洗剂泵
    p05 = 4,   //浓缩清洗剂泵
    p03 = 30,	//去离子水泵
}app_autowash_pvnum_enum;

//===========泵\阀控制命令数据结构==========
typedef struct
{
    vos_u16 operateType;
	vos_u16 PVNum;
}msg_PVCtrl;

//===========注射泵控制命令数据结构=========
typedef struct
{
    vos_u16 dir;
    vos_u16 volume;
}msg_PumpCtrl;

//===========开关=========
typedef struct
{
    vos_u32 operation; //1:开、0:关
}msg_switch;

//样本指示灯控制
typedef struct
{
    char    ctl;
    char    rsv[3];
}msg_sample_lightCtl;

//下位机升级数据传输
typedef struct
{
    vos_u16 exeCmd;//执行动作
    vos_u16 rsv;
    vos_u32 dataPage;//页数
    char data[DWNPAGE_SIZE];//数据
}msg_updateDwn;

//升级FPGA
typedef struct
{
    vos_u16 flag;  //是否为最后一帧数据标志(下发给下位机)
    vos_u16 dataPage;//本帧数据执行状态(从下位机反馈)
    vos_u16 length;//data的长度(取值只能为0----256)
    vos_u16 rsv;
    char  data[DWNPAGE_SIZE]; //数据内容
}msg_updateFpga;
//液路调试
typedef struct
{
    LiquidDBG_Type debugType;
}msg_liquidDBG;
typedef struct 
{
    vos_u16 state;
}msg_stateSwitch;


#define ACT

//===========反应盘水平旋转动作数据结构==========
typedef struct
{
    app_u16 usRttedCupNum; //转过杯位数
    char dir;
    char aucRsv;
}act_stReactsysDskRtt;

//===========反应杯自动清洗动作数据结构==========
typedef struct
{
    app_u16 usWashOutBitMap; //需要清洗路位图(bit0:1号位,bit9:10号位),相应bit为1；代表需要清洗,为0代表不需要清洗
    app_u16 usWashInBitMap2;
}act_stReactSysAutoWash;

//===========试剂盘水平旋转动作数据结构==========
typedef struct
{
    app_u16 usCupId;    //试剂杯号
    app_u16 usDstPos;   //目标位置(吸试剂位/换试剂位)
}act_stReagSysDskRtt;


typedef struct
{
    app_u32 channel;
}act_stReadOneAD;

typedef struct
{
    app_u32 AdCount;
}act_stReadAD;

typedef struct
{
    uint16_t channel;			/*通道号*/
    uint16_t gain;				/*电阻值*/
}act_stGain;

typedef struct
{
    uint16_t channel;			/*通道号*//*通道，0 急诊，1 常规*/
    uint16_t place;             /*获取位置*/
}act_stGetin;
/*自动装载定位*/
typedef struct
{
    uint32_t  type;  /*取/放标志*/
    uint32_t  place; /*目标位置*/
}act_autoload_move;
typedef struct  
{
    vos_u16 cmd;
    uint8_t state;/*控制状态*/
}act_state_switch;
#endif
