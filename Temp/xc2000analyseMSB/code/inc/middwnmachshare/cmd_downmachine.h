//modifyed
#ifndef _CMD_DOWNMACHINE_H_
#define _CMD_DOWNMACHINE_H_

/*样本子系统*/
#include"cmd_sampledisk.h"
#include"cmd_samplemixing.h"
#include"cmd_sampleneedle.h"

/*反应子系统*/
#include"cmd_reactiondisk.h"
#include"cmd_reagentmixing.h"
#include"cmd_autowashneedle.h"

/*试剂子系统*/
#include"cmd_innerreagentdisk.h"
#include"cmd_outerreagentdisk.h"
#include"cmd_r1reagentneedle.h"
#include"cmd_r2reagentneedle.h"

/*温控子系统*/
#include "cmd_tempcontrol.h"

/*试剂制冷子系统*/
#include "cmd_coolingreagent.h"
#include "stdint.h"
#define DWNPAGE_SIZE  (256)   //下位机升级页大小


#define XC8002_COM_DWNMACH_COMB_CMD (0x0c)


#define XC8002_COM_DWNMACH_LOOP_CMD (0xff03)
#define XC8002_COM_REPORT_CMD       (0xff04)//上报命令关键字
#define XC8002_COM_QUERY_CMD        (0xff06)//查询温度命令关键字
#define XC8002_COM_FAULT_RESTORE_CMD (0xff07)//故障恢复命名关键字
#define XC8002_COM_MIDRUNSTATE_CMD  (0xff08)//中位机通知下位机中位机运行状态命令
#define XC8002_COM_COMQUERY_CMD     (0x0009)//下位机通用查询命令
#define XC8002_COM_OPERATION_CMD    (0xff0a)//下位机调试-正常态切换
#define XC8002_COM_BBCLEAR_CMD      (0xff0b)//清除下位机日志
#define XC8002_COM_UPDATEDWN_CMD    (0xff0c)//下位机升级
#define XC8002_COM_SET_MOTOR_CMD    (0xff0d)


/*串口环回测试层定义*/
typedef enum
{
    xc8002_com_loopback_layer_linuxapp = 0, //中位机应用层
    xc8002_com_loopback_layer_linuxdrv = 1, //中位机驱动层
    xc8002_com_loopback_layer_linuxlgc = 2, //中位机逻辑层
    xc8002_com_loopback_layer_ucosdrv  = 3, //下位机驱动层
    xc8002_com_loopback_layer_ucosapp  = 4, //下位机应用层

    xc8002_com_loopback_layer_butt
}xc8002_com_loopback_layer_enum;
typedef char xc8002_com_loopback_layer_enum_u8;

/*定义下位机通用命令数据结构*/
typedef struct
{
    app_u16 cmd;
    char  cmdrsv[2];  //没有命令参数作为四字节对齐保留位,否则保存参数
}xc8002_com_dwnmach_cmd_stru;

/*定义下位机通用单条组合命令数据结构*/
typedef struct
{
    vos_u32 cmdsize;  //命令内存大小
    xc8002_com_dwnmach_cmd_stru stcmdinfo;
}xc8002_com_comb_single_cmd_stru;

#pragma warning(disable:4200)
/*定义下位机通用组合命令数据结构*/
typedef struct
{
    app_u16 cmd;
    app_u16 cmdnum;  //命令条数
    xc8002_com_comb_single_cmd_stru acmdinfo[0];
}xc8002_com_dwnmach_comb_cmd_stru;

/*定义环回测试命令数据结构*/
typedef struct
{
    app_u16 cmd;
    app_u16 usframeid;
    app_u16 usttl;
    app_u16 enloopbacklayer;  //环回层
}xc8002_loopback_dwnmach_cmd_stru;

//反应盘光源获取AD参数数据结构
typedef struct {
    app_u16 cmd;
    app_u16 reprot_cmd_type;
    vos_u32 dwnmachname; 
    app_u16 get_eeprom_lightintensity;  //获取EEPROM存储值
    app_u16 get_lamp_lightintensity;  //获取灯的设定值   
}cmd_react_disk_lightgetval_stru;

/*定义下位机上报的状态类型*/
typedef enum
{
    xc8002_dwnmach_report_remain_type       = 1, //余量上报
    xc8002_dwnmach_report_rctdsk_temp_type  = 2, //反应盘温度上报类型
    xc8002_dwnmach_report_reagdsk_temp_type = 3, //试剂盘温度上报类型
    xc8002_dwnmach_report_fault_type        = 4, //下位机故障上报类型
    xc8002_dwnmach_report_close_reagcabin_type = 5, //关闭试剂舱类型
    xc8002_dwnmach_report_open_reagcabin_type  = 6, //打开试剂舱类型
    xc8002_dwnmach_report_downmachin_versions  = 7, //下位机版本上报
    xc8002_dwnmach_report_LightBulbAD_type     = 8, //光源灯电压AD值上报
    xc8002_dwnmach_report_compensation_type    = 9, //下位机机械补偿类型
    xc8002_dwnmach_report_fanstatus_type       = 10,//风扇运行状态
    xc8002_dwnmach_report_electricity_type     = 11,//制冷片电流
    xc8002_dwnmach_report_fl_and_pre_type      = 12,//浮子、压力查询
    xc8002_dwnmach_report_reagdsk_temp_query_type   =13,//制冷温度查询上报，非定时上报    
    xc8002_dwnmach_report_thermal_control_temp_type = 14,//温控温度查询上报，非定时上报
    xc8002_dwnmach_report_eeprom_info_type = 15,//eeprom存储信息上报
    xc8002_dwnmach_report_bblog_info_type  = 16,     //日志信息上报 
    xc8002_dwnmach_report_keydown_innerdisk_type = 17,//内盘按下旋转按钮
    xc8002_dwnmach_report_keydown_outerdisk_type = 18,//外盘按下旋转按钮
    xc8002_dwnmach_report_bootloader_versions = 19,//bootloader版本号

	xc8002_dwnmach_report_innerdisk_wave0_type = 20,//虚拟示波器内盘0通道
	xc8002_dwnmach_report_innerdisk_wave1_type = 21,//虚拟示波器内盘1通道
	xc8002_dwnmach_report_outerdisk_wave0_type = 22,//虚拟示波器外盘0通道
	xc8002_dwnmach_report_smpdisk_wave0_type   = 24,//虚拟示波器样本盘0通道
	xc8002_dwnmach_report_pidstade			 = 28,        //pid状态记录
	xc8002_dwnmach_report_ad_temp_type =30,
    xc8002_dwnmach_report_GwayState = 31,           //轨道状态上报
    xc8002_dwnmach_report_motorline_type = 40,   //电机曲线上报
	xc8002_dwnmach_report_clot_air_type = 41,	//堵针空吸数据上报
    xc8001_dwnmach_report_ID = 42,
    dwnmach_report_shelf = 43,//样本架上报
    dwnmach_report_AD = 44,//光源灯AD值上报
    dwnmach_show_wave = 45,
    xc8002_dwnmach_report_status_type_butt
}xc8002_dwnmach_report_status_type_enum;

/*定义下位机状态上报消息结构*/
typedef struct
{
	app_u16 cmd;              //上报命令关键字:0xff04
	app_u16 usstatustype; //状态类型    
	char  aucloadinfo[0];   //消息负载
}xc8002_com_report_cmd_stru;

//-----------------余量、误检、漏检信息结构体-------------
typedef struct{
    app_u16 diskid;   //盘号，用于余量上报
    app_u16 cupid;    //杯位号，用于余量上报
    vos_u32 reactcup;   //任务号，用于误检、漏检处理
}xc8002_cup_info_stru;

/*定义试剂余量上报信息结构体*/
typedef struct
{
    app_u16 cmd;              //命令类型
	app_u16 usstatustype;     //状态类型
	app_u16 usmechdevid;      //机械器件id
    app_u16 usremaindepth;    //试剂余量深度
    xc8002_cup_info_stru cupinfo; //杯信息
}xc8002_report_remaindepth_stru;
//--------------------------------------------------------

//下位机温度上报数据结构
typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    app_i32 slcurtemp;    //当前温度值(=实际温度*10倍,防止异系统间小数传输)
}xc8002_report_temprature_status_stru;

//定义下位机版本上报结构体(带fpga版本号)
//只有主CPU会带fpga的版本号
typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    char  date_time[32];
    char  dwnmachin_ver[24];
    vos_u32 fpga_ver;
}cmd_fpga_dwnmach_version_stru;

//CPU ID号
typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    vos_u32 id[4];
}cmd_CPUID;

typedef enum
{
    xc8002_fault_status_restore  = 0,//故障恢复
    xc8002_fault_status_generate = 1,//故障产生
    xc8002_fault_status_butt
} xc8002_fault_status_enum;//故障状态

//下位机故障信息数据结构体
typedef struct {
	unsigned char chCpuId;          //单片机ID
	unsigned char chFaultStatus;      //故障状态:产生、恢复
	app_u16 usFaultId;          //故障ID
} xc8002_fault_info_stru;

//下位机故障上报数据结构
typedef struct
{
    app_u16 cmd;              //上报命令关键字:0xff04
    app_u16 enstatustype;     //上报数据类型
    xc8002_fault_info_stru fault_info;
} xc8002_fault_report_stru;

//------------------------End: 吴双喜3.23添加--------------------------------------
/*****end of fault check module*****/

//------------------------Begin: 补偿命令--------------------------------------
//旋转补偿方向枚举值
typedef enum
{
	compensation_dir_clockwise      = 1,    //顺时针
    compensation_dir_anticlockwise  = 2,    //逆时针
	
	compensation_dir_butt
}compensation_dir_enum;
typedef app_u16 compensation_dir_enum_u16;

//垂直补偿方向枚举值
typedef enum
{
	compensation_dir_up      = 1,    //向上
    compensation_dir_down  = 2,    //向下
	
	compensation_dir_vrt_butt
}compensation_dir_vrt_enum;

//补偿类型枚举值
typedef enum
{
	compensation_type_reset             = 1,    //复位补偿
    compensation_type_rtt               = 2,    //旋转补偿
    compensation_type_mainslave_switch  = 3,    //主备区切换	
    compensation_type_clear             = 4,    //清除补偿值
    compensation_type_vrt               = 5,    //垂直补偿
    
	compensation_type_butt
}compensation_type_enum;
typedef app_u16 compensation_type_enum_u16;

//补偿参数
typedef struct {
    app_u16 location;   //补偿位置
    compensation_dir_enum_u16 dir;        //补偿方向 1: 顺时针 2:逆时针
    vos_u32 step;       //补偿步数 
} cmd_compensation_para;


//------------------------Begin: 简单命令-------------------------------------
typedef struct{
    app_u16 cmd;      //命令字
    app_u16 para;     //参数
}cmd_simplecmd_stru;

/*******************简单命令参数para说明*******************/
//中下位机启动交互
typedef enum{
    cmd_bootinfo_reboot     = 0, //中位机重启
    cmd_bootinfo_bootfinish = 1, //中位机启动初始化完成

    cmd_bootinfo_butt = 2,
}cmd_bootinfo_enum;
//------------------------end:    简单命令-------------------------------------

//定义方向(与下位机一致)
#define cmd_motor_dir_up                (0)
#define cmd_motor_dir_down              (1)
#define cmd_motor_dir_ANTICLOCKWISE     (1)
#define cmd_motor_dir_CLOCKWISE         (0)
#define cmd_motor_dir_IN				(1)
#define cmd_motor_dir_OUT				(0)

//--------------下位机EEPROM值上报------------------------
//补偿参数
typedef struct
{
    compensation_type_enum_u16 comptype;//类型
    app_u16 location;        //位置
    compensation_dir_enum_u16 dir;     //补偿方向 1: 顺时针 2:逆时针
    app_u16 step;           //补偿步数 
}xc8002_report_compensation_para;
/*电机加减速配置结构体*/
typedef struct
{
    unsigned char  drvId;
    unsigned char  lineId;
    app_u16 startFre;			//启动频率
    app_u16 endFre;			//最高频率
    app_u16  echLaderStep;		//每台阶微步
    app_u16  upMaxLader;		//最大加速台阶
    app_u16  sPar;				//S型曲线的形状参数}motor_para;
}xc8002_report_motor_para;

//单片机对应补偿
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    app_u16 dwnmachinetype; //下位机号
    app_u16 paranum;  //参数个数
    xc8002_report_compensation_para ppara[0];  //补偿参数 变长
}xc8002_report_dwnmachine_compensation;


//===========下位机补偿上报结构体==========
typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;

    app_u16 usCmdType;
    app_u16 enStatusType; //上报数据类型
    app_u16 ulDwnmachName;  //下位机名称
    app_u16 usParaNum;  //参数个数
    xc8002_report_compensation_para ppara[0];  //补偿参数 变长
}msg_stDwnmachCompensationReprot;

//===========下位机电机上报结构体==========
typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;

    app_u16 usCmdType;
    app_u16 enStatusType; //上报数据类型
    app_u16 ulDwnmachName;  //下位机名称
    app_u16 usParaNum;  //参数个数
    xc8002_report_motor_para ppara[0];  //补偿参数 变长
}msg_stDwnmachMotorReprot;

typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    app_u16 dwnmachinetype; //下位机号
    app_u16 paranum;  	//参数个数
    xc8002_report_motor_para ppara[0];
}xc8002_report_dwnmachine_motorspeed;

typedef struct
{
	app_u16 cmd1;
	app_u16 enstatustype; //上报数据类型
	app_u16	ClotValue;
	app_u16	AirMax;
	app_u16	AirSum;
	app_u16	WashMax;

}ClotAirDataReport_st;
//bblog单记录结构体
typedef struct
{
    char record[64];    //一条记录中保存的数据
}xc8002_bblog_record;

//单片机bblog
typedef struct {
    app_u16 cmd;    
    app_u16 enstatustype; //上报数据类型    
    app_u16 currpage;     //当前页
    app_u16 recordnum;  //日志个数
    xc8002_bblog_record records[];  //日志条数
}xc8002_report_dwnmachine_bblog;


typedef struct
{
    app_u16 cmd;              //命令类型
    app_u16 usstatustype;     //状态类型
    app_u32 shelfNum;
}Report_shelf_st;

typedef struct
{
    app_u16 cmd;
    app_u16 usstatustype;     //状态类型    dwnmach_report_AD=44,//光源灯AD值上报
    app_u32 AdCount;
    app_u32 value[0];
}ADValue_st;

typedef struct
{
    app_u16 cmd;
    app_u16 usstatustype;    //状态类型     31//轨道状态上报
    app_u16 usLoca;          //位置  
    app_u16 usState;         //状态
}GwayState_st;


typedef struct {
    app_u16 cmd;
    app_u16 usstatustype;    //状态类型     9//参数上报
    app_u16 dwnmachinetype;  //下位机号
    char number;             //页码
    char count;              //个数
    app_u32 datas[0];        /*参数值*/
}Report_PAGE_st;

typedef struct{
    app_u16 op;
    app_u16 ad;
} AD_OP_TYPE;

typedef struct {
    app_u16 cmd;
    app_u16 usstatustype;    //状态类型     45//参数上报
    app_u32 dwnmachinetype;  //下位机号
    AD_OP_TYPE value[2000];
}Report_ShowOP_st;

#pragma warning(default:)
#endif
