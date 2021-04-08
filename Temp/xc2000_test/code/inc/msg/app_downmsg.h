#ifndef _app_down_msg_h_
#define _app_down_msg_h_


#include "cmd_downmachine.h"
#include <stdio.h>
#include <drv_ebi.h>
#include <stdint.h>
//===========下位机查询命令数据结构==========
typedef struct
{
    app_u16 cmd;           //查询命令关键字:0xff09
    app_u16 queryType;     //查询的状态类型
}cmd_stDwnQuery;
typedef struct
{
    app_u16 cmd;           //查询命令关键字:0x0009
    char  num;           //页号
    char queryType;     //读写标志
}cmd_st2000DwnQuery;
//===========故障恢复命令结构==========
typedef struct {
	app_u16 cmd;
    app_u16 rsv;  
    vos_u32 faultId;
    app_u16 boardId;
    app_u16 deviceId;
}cmd_stRsmFault;

//===========中位机状态下发命令结构==========
typedef struct {
	app_u16 cmd;
    app_u16 midState;
}cmd_stMidState;

//定义虚拟示波器开关cmd数据结构
typedef struct
{
    app_u16 cmd;
    char  channel;
	char  switch_state;
}cmd_stShowave;

//===========下位机补偿命令数据结构==========
//补偿参数
typedef struct {
    app_u16 location;   //补偿位置
    app_u16 dir;        //补偿方向 1: 顺时针 2:逆时针
    vos_u32 step;       //补偿步数 
}cmd_stCmpPara;

//补偿命令
typedef struct {
    app_u16 cmd;      //补偿命令字
    app_u16 type;  //补偿类型  1: 复位补偿  2: 旋转补偿 3: 主备区切换 4: 清除 5:垂直补偿
    vos_u32 paranum;  //补偿参数个数
    cmd_stCmpPara *ppara;  //补偿参数 变长
}cmd_stCompensation;

typedef struct
{
	app_u16 cmd;
	app_u16 flag;
}tempSleepStausStr;
//===========电机微步调整动作命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char    motorType;  //电机类型
    char    dir;        //方向
    vos_u32 step;       //微步数
}cmd_stStepMove;

//===========反应盘水平转动动作命令数据结构==========
typedef struct
{
    app_u16 cmd;
    app_u16 rttCupNum;      //转动的杯位数
}msg_stReactDskHrotate;

//===========反应盘光源灯控制命令数据结构==========
typedef struct {
    app_u16 cmd;
    app_u16 lightIntensity;  //光强度 0~255亮度调整
}cmd_stLightControl;

//===========温控开关命令数据结构==========
typedef struct {
    app_u16 cmd;
    app_u16 switchState;  //开关控制 0-关闭 1-打开
}cmd_stTempSwitch;

typedef struct {
    app_u16 cmd;
    app_u16 temp1;     //加热器1目标温度
    app_u16 temp2;     //加热器2目标温度
    app_u16 temp3;     //加热器3目标温度
}cmd_stWTemp;

//反应盘目标温度
typedef struct {
    app_u16 cmd;
    app_u16 delta;
    vos_u32 tempreact;  //反应盘目标温度
}cmd_stRTem;

//===========配置AD控制数据结构==========
typedef struct {
	app_u16 cmd;    //命令字
    app_u16 srv;    //保留字节
    vos_u32 dpValue[WAVE_NUM];  //每个通道对应的AD增益系数
}cmd_stAdConfig;

//===========样本盘水平转动动作命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char  uccircleid;  //样本盘圈号
    char  uccupid;     //指定样本杯号或者待转过的杯位数
}msg_cmdSampleDskRtt;

//===========注射泵排气命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char  num;//注射泵排气整个过程的次数
    char  rsv;//预留
}msg_stNdlAirout;

//===========试剂盘水平转动动作命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char  ucPos;  //转到的位置
    char  ucCupId;     //指定杯号或者待转过的杯位数
}cmd_stRDskMove;

//===========试剂盘正、逆时针转动动作命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char  cupNum;  //转过的杯位数
    char  rsv[1];  //指定杯号或者待转过的杯位数
}cmd_stRDskHrotate;

//===========试剂盘转动到换试剂位动作命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char  cupId;  //转到换试剂位的杯位号
    char  rsv[1];  
}cmd_stRDskSwappos;

typedef struct
{
    app_u16 cmd;
    app_u16 cupNum;  //转过的杯位数
}cmd_stReactDskHrotate;

//===========样本针/试剂针吸样本or蒸馏水动作命令数据结构==========
typedef struct{
    app_u16 cmd;
    app_u16 quantity;//吸样本量
}msg_stNdlAbsrb;

//===========针垂直转动命令数据结构==========
typedef struct{
    app_u16 cmd;
    char  enposition; //运动到的目标位置
    char  reportmarginflag;//上报余量标志，1:上报，0:不上报
    app_u16 ustotalvol; //杯总体积用于随量跟踪
    char unCheckLiquid;//屏蔽液面检测，1：屏蔽
    char  rsv;
    app_u16 diskid;   //盘号，用于余量上报
    app_u16 cupid;    //杯位号，用于余量上报
    vos_u32 reactcup;   //任务号，用于误检、漏检处理
}cmd_stNdlVrt;

//===========样本针排样本动作命令数据结构==========
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //排试剂量
    char  diluteflag;
    char  rsv[3];
}msg_cmdSampleNdlDischarge;

//===========自动清洗命令数据结构==========
typedef struct
{
    app_u16 cmd;
    app_u16 washBitMap;
}msg_stAutoWash;

//===========泵\阀控制命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char PVNum;
    char operateType;   //开关
}cmd_PVCtrl;

//===========泵\阀控制命令数据结构==========
typedef struct
{
    app_u16 cmd;
    char operateType;   //开关
    char rsv[1];
}cmd_stPVCtrl;

//===========单一命令数据结构==========
typedef struct{
    app_u16 cmd;
    char rsv[2];
}msg_stOneCmd;

//===========试剂盘X杯位旋转到条码扫描位==========
typedef struct
{
    app_u16 cmd;
    char  uccupnum; //某杯位转到换试剂位
    char  rsv[1];
}msg_stReagdskRttToBcscan;

//===========样本盘X杯位旋转到条码扫描位==========
typedef struct
{
    app_u16 cmd;
    char  uccircleid;  //样本盘圈号 (0xFF: 3圈同时扫描)
    char  uccupid;     //样本盘杯号:(0:整圈扫描完再返回数据，1-最大杯号:返回指定杯号数据)
}msg_stSmpdskRttToBcscan;

//===========针水平转动命令数据结构==========
typedef struct{
	app_u16 cmd;
	char  enPosition;  //运动到的目标位置
	char  rsv[1];
}msg_stNdlHrotate;

//===========定义下位机通用单条组合命令数据结构==========
typedef struct
{
	vos_u32 ulCmdSize;            //命令内存大小
	char  aucCmdLoad[0];     //命令负载 
}msg_stComCombSingleCmd;

//===========定义下位机通用组合命令数据结构==========
typedef struct
{
	app_u16 cmd;
	app_u16 cmdNum;               //命令条数
	char  aucCmdLoad[0];         //命令负载
}msg_stComDwnmachCombCmd;

//===========下位机动作vos消息结构定义==========
typedef struct
{
	app_u16 enDwnmachSubSys;  //下位机系统名(反应盘、试剂盘、样本盘、试剂制冷系统)
	app_u16 usDevType;
	char  aucCmdLoad[0];     //命令负载 
}msg_stDwnmachActionReq;

//===========定义串口管理模块与中位机下位机命令交互中心的VOS消息结构==========
typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;
    
    app_u16 usCmdType;
    char  enExeRslt;      //命令执行结果
    char  ucRsv[1];
    char aucinfo[0];
}msg_stMnguart2Downmach;

//===========下位机执行结果vos消息定义==========
typedef struct
{
    app_u16 enDwnmachSubSys;  //下位机系统名(反应盘、试剂盘、样本盘、试剂制冷系统)
    app_u16 usDevType;
    app_u16 enCmdType;         //命令类型
    app_u16 enCmdExeRslt;      //命令执行结果
    vos_u32 ulPayLoadLen;      //负载长度
    char aucPayLoad[0];      //负载信息
}msg_stDwnmachActionMsg;

typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;

    app_u16 usCmdType;
    app_u16 enStatusType; //上报数据类型
    app_u32 shelfNum;
}msg_stDwnmachShelfReprot;

/*R1试剂针排试剂命令结构*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //排试剂量
    char  diluteflag;
    char  rsv[3];
}msg_stR1Discharge;


//---------------下位机调试-正常态切换--------------------
typedef struct
{
    app_u16 cmd;        //命令关键字0xff0a
    app_u16 operation;  //切换0:调试态，1:正常态
}cmd_stMachineDebug;


//===========开关=========
typedef struct
{
    vos_u16 cmd;
    app_u16 operation; //1:开、0:关
}cmd_switch;


typedef enum
{
    samp_key_led_off = 0,
    samp_key_led_on = 1,
    samp_key_led_flicker = 2,
    samp_key_led_butt,
}samp_key_led_value;

//样本指示灯控制
typedef struct
{
    vos_u16 cmd;
    char    ctl;
    char    rsv;
}cmd_sample_lightCtl;

//下位机升级数据传输
typedef struct
{
    vos_u16 cmd;
    vos_u16 exeCmd;//执行动作
    vos_u32 dataPage;//页数
    char data[DWNPAGE_SIZE];//数据
}cmd_updateDwn;

//升级FPGA
typedef struct
{
    vos_u16 cmd;
    vos_u16 flag;  //是否为最后一帧数据标志(下发给下位机)
    vos_u16 dataPage;//本帧数据执行状态(从下位机反馈)
    vos_u16 length;//data的长度(取值只能为0----256)
    char  data[DWNPAGE_SIZE]; //数据内容
}cmd_updateFpga;


#define CMDTYPE
typedef enum {
    CMD_NDL_RESET = 0x0011,		        /*针复位*/
    CMD_NDL_UP = 0x0012,		        /*针上升*/
    CMD_NDL_DOWN = 0x0013,		        /*针下降*/
    CMD_NDL_TURN = 0x0014,		        /*针旋转*/
    CMD_NDL_SAMPLE = 0x0015,		    /*针采样*/
    CMD_NDL_BLOWOFF = 0x0016,		    /*针排样*/
    CMD_NDL_WASH = 0x0017,		        /*针清洗*/
    CMD_NDL_WATER_UPTAKE = 0x0018,	    /*针吸水*/
    CMD_NDL_WASH_VALVE_CONTROL = 0x0019,/*针清洗阀控制*/
    CMD_NDL_AIR_OUT = 0x001a,		    /*针排气*/
} CMD_NDL_ENUM;

typedef enum {
    CMD_DSK_RESET = 0x0053,		/*盘复位*/
    CMD_DSK_TURN = 0x0054,		/*盘旋转*/
} CMD_DSK_ENUM;

typedef enum {
    CMD_AUTOWASH_RESET = 0x0081,	 /*自动清洗复位*/
    CMD_AUTOWASH_MOVE = 0x0082,		/*自动清洗升降*/
    CMD_AUTOWASH = 0x0083,		    /*自动清洗*/
    CMD_AUTOWASH_PVCONTROL = 0x0084,/*单个泵阀控制*/
} CMD_AUTOWASH_ENUM;

typedef enum {
    CMD_STIR_RESET = 0x0021,		/*搅拌复位*/
    CMD_STIR_UPDOWN = 0x0022,		/*搅拌升降*/
    CMD_STIR_TURN = 0x0023,		    /*搅拌转动*/
    CMD_STIR_WASH = 0x0024,		    /*搅拌清洗*/
    CMD_STIR = 0x0025,		        /*搅拌*/
    CMD_STIR_PVCONTROL = 0x0026,    /*搅拌泵阀控制*/
} CMD_STIR_ENUM;

typedef enum {
    CMD_ORBITRESET = 0x0040,    /*模块复位*/
    CMD_PUSHIN = 0x0041,        /*进架*/
    CMD_TRANSFER = 0x0042,      /*进样*/
    CMD_PUSHOUT = 0x0043,       /*退架*/
    CMD_TRANSFERFREE = 0x0044,  /*进样释放*/
    CMD_TRANSFER_STEP = 0x0045, /*进样N位*/
} CMD_TRANSFER_ORBIT_ENUM;

typedef enum {
    CMD_READ_ONE_AD = 0x00B5,	/*读一个AD值*/
    CMD_READ_AD = 0x00B6,		/*检测流程*/
    CMD_SET_GAIN = 0x00B7,		/*设置增益*/
    CMD_READ_ALL_AD = 0x00B8,		/*读平坦区流程*/
} CMD_AD_ENUM;

#ifdef Version_A
typedef enum {
    CMD_ResetShelf = 0x0090,
    CMD_GetShelf = 0x0091,	    /*读一个AD值*/
    CMD_MoveShelf = 0x0092,		/*检测流程*/
    CMD_FreeShelf = 0x0093,		/*设置增益*/
} CMD_Gway_ENUM;
#else
typedef enum {
    CMD_ORBIT_RESET = 0x0090,		/*模块复位*/
    CMD_GET = 0x0091,		        /*获取*/
    CMD_LOCATING = 0x0092,		    /*定位*/
    CMD_FREE = 0x0093,		        /*释放出*/
    CMD_FREEIN = 0x0098,				/*释放入*/
    CMD_GET_IN = 0x0099,			/*传入*/
} CMD_ORBIT_ENUM;
#endif

typedef enum {
    CMD_READ_TEMPERATURE = 0x00BA,      /*读取温度值*/
    CMD_TEMPERATURE_SWITCH = 0x00BB,      /*温控开关，默认关闭*/
} CMD_REACTION_TEMPERATURE_CONTROL_ENUM;

typedef enum {
    CMD_COM_RW_PARAMETER_PAGE = 0x0009,//读写下位机补偿
}CMD_COM_RW_PARAMETER_PAGE_ENUM;

typedef enum {
    CMD_SET_LIGHT = 0x00BC,	/*设置光源灯参数*/
} CMD_LIGHT_SOURCE_ENUM;
/*自动装载下位机通信*/
typedef enum{
    CMD_AUTO_LOAD_VG_RE = 0x0040,
    CMD_AUTO_LOAD_TSPT_RE = 0x0041,
    CMD_AUTO_LOAD_TSPT_MOVE = 0x0042,
    CMD_AUTO_LOAD_VG_MOVE=0x0043,
}CMD_AUTO_LOAD_DOWN;

#define  CMDSTRUCT

/*简单命令*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t rsv[2];
} CMD_Single_TYPE;

/*针复位*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t dis_wash;			/*不清洗标志位*/
    uint32_t time;				/*清洗时间*/
} CMD_NDL_RESET_TYPE;

/*针上升*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t  rsv[2];
} CMD_NDL_UP_TYPE;

/*针下降*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t en_liquid_level_measuring;	/*使能液面检测标志位*/
    uint8_t en_reporting_margin;	/*使能余量上报标志位*/
    uint16_t total_volume;	/*总体积*/
    uint16_t ring_number;	/*圈号*/
    uint16_t place_number;	/*位号*/
    uint16_t cup_number;	/*反应杯号*/
} CMD_NDL_DOWN_TYPE;

/*针转动*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t  place;				/*目标位置*/
    uint8_t  rsv;
} CMD_NDL_TURN_TYPE;

/*针吸样*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t size;				/*吸样量*/
} CMD_NDL_SAMPLE_TYPE;

/*针排样*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t size;				/*排样量*/
} CMD_NDL_BLOWOFF_TYPE;

/*针清洗*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t type;				/*清洗方式*/
    uint32_t time;				/*清洗时间*/
} CMD_NDL_WASH_TYPE;

/*针吸水*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t size;				/*吸水量*/
} CMD_NDL_WATER_UPTAKE_TYPE;

/*针清洗阀控制*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t number;				/*阀编号*/
    uint8_t status;				/*状态*/
    uint32_t appedn[2];
} CMD_NDL_WASH_VALVE_CONTROL_TYPE;

/*针排气*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t number;			/*次数*/
} CMD_NDL_AIR_OUT_TYPE;


/*盘复位*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t rsv[2];
} CMD_DSK_RESET_TYPE;

/*盘转动*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t type;				/*运动类型*/
    uint16_t cup_number;		/*位号或间隔*/
    uint16_t place;				/*位置*/
} CMD_DSK_TURN_TYPE;

//架子定位
typedef struct {
    uint16_t cmd;			/*命令字*/
    uint8_t currloca;	    /*当前位置*/
    uint8_t destloca;	    /*目标位置*/
}CMD_SHELF_LOCA_TYPE;

/*搅拌复位*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t dis_wash;			/*不清洗标志位*/
    uint32_t time;				/*清洗时间*/
} CMD_STIR_RESET_TYPE;

/*搅拌升降*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t place;				/*位置编号*/
    uint8_t rsv;
} CMD_STIR_UPDOWN_TYPE;

/*搅拌转动*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t  place;				/*位置编号*/
    uint8_t  rsv;
} CMD_STIR_TURN_TYPE;

/*搅拌清洗*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t dis_wash;			/*禁止清洗标志*/
    uint32_t time;				/*搅拌/清洗时间*/
} CMD_STIR_WASH_TYPE;

/*搅拌*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t  dis_wash;			/*禁止清洗标志*/
    uint32_t  time;				/*搅拌/清洗时间*/
} CMD_STIR_TYPE;

/*搅拌泵阀控制*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t  number;			/*泵阀编号*/
    uint8_t  status;			/*开关状态*/
} CMD_STIR_PVCONTROL_TYPE;


/*自动清洗复位*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t rsv[2];
} CMD_AUTOWASH_RESET_TYPE;

/*自动清洗升降*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t place;				/*位置编号*/
} CMD_AUTOWASH_MOVE_TYPE;

/*自动清洗*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t listOut;			/*外圈位号表*/
    uint8_t listIn;				/*内圈位号表*/
} CMD_AUTOWASH_TYPE;

/*自动清洗单个泵阀控制*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t number;				/*泵阀编号*/
    uint8_t status;				/*控制状态*/
} CMD_AUTOWASH_PVCONTROL_TYPE;

/*读一个AD值*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t channel;			/*通道号*/
    uint8_t rsv;
} CMD_READ_ONE_AD_TYPE;

/*检测流程*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t AdCount;				/*检测个数*/
} CMD_READ_AD_TYPE;

/*设置增益*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t channel;			/*通道号*/
    uint32_t gain;				/*电阻值*/
} CMD_SET_GAIN_TYPE;

/*复位*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint8_t rsv[2];
} CMD_ORBITRESET_TYPE;

/*进架*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint8_t rsv[2];
} CMD_PUSHIN_TYPE;

/*进样N位*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint8_t number;            /*要进样的次数*/
    uint8_t rsv;
} CMD_TRANSFER_STEP_TYPE;

/*进样一步*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint8_t number;          /*进样个数*/
    uint8_t rsv;
} CMD_TRANSFER_TYPE;

/*退架*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint8_t rsv[2];
} CMD_PUSHOUT_TYPE;

/*释放*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint8_t rsv[2];
} CMD_TRANSFERFREE_TYPE;

/*读取清洗水温度值*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint16_t channel;         /*通道号，1 整机加热2温度，2 整机加热1温度，3 清洗剂加热温度， 4 清洗水加热温度*/
} CMD_READ_TEMP_TYPE;

/*清洗水温控开关*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint16_t mode;            /*0 关闭，1启动*/
} CMD_TEMP_SWITCH_TYPE;

/*读写参数页*/
typedef struct {
    uint16_t cmd;            /*命令字*/
    uint8_t  number;         /*页号*/
    uint8_t  rw;             /*读写选择*/
    uint8_t  datas[0];       /*参数值*/
}CMD_COMMON_DATA_RW_PAGE_DATA_TYPE;

/*设置光源灯参数*/
typedef struct {
    uint16_t cmd;	/*命令字*/
    uint16_t voltage;	/*电压值*/
} CMD_SET_LIGHT_TYPE;

/*模块复位*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t rsv[2];
} CMD_ORBIT_RESET_TYPE;

/*获取*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t channel;			/*通道，0 急诊，1 常规*/
    uint16_t  place;             /*获取位置*/
} CMD_GET_TYPE;

/*定位*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint8_t currloca;			/*杯号*/
    uint8_t destloca;			/*目标位置*/
    uint16_t channel;			/*通道，0 急诊，1 常规*/
}CMD_LOCATING_TYPE;

/*释放*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t channel;			/*通道，0 急诊，1 常规*/
}CMD_FREE_TYPE;

/*传入*/
typedef struct {
    uint16_t cmd;				/*命令字*/
    uint16_t channel;			/*通道，0 急诊，1 常规*/
}CMD_GET_IN_TYPE;
/*试剂盒自动装载转运机构移动位置*/
typedef struct
{
    uint16_t cmd;    /*命令字*/
    uint8_t  number; /*位置编号*/
}CMD_MODULE_MOVE_POSITION_TYPE;
/*自动装载垂直抓手模块运动*/
typedef struct
{
    uint16_t cmd;   /*命令字*/
    uint8_t  type;  /*取/放标志*/
    uint8_t  place; /*目标位置*/
}CMD_AUTOLOAD_MOTZMOVEPOSITION_TYPE;
#endif