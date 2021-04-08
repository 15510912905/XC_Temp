//modifyed
#ifndef _cmd_coolingreagent_h_
#define _cmd_coolingreagent_h_



//温度控制单元命令枚举定义
typedef enum
{
    cmd_cooling_min             = 0x4000,  //制冷单元最小命令
    cmd_cooling_debug           = 0x4001,  //制冷查询调试
    cmd_cooling_control		    = 0x4002,  //制冷开关控制,参数0开制冷，1关制冷
    cmd_cooling_settemp         = 0x4003,  //设置温度参数

	cmd_cooling_chip_reset      = 0x4070, //芯片复位
	cmd_cooling_update          = 0x4080, //芯片复位
	
    cmd_cooling_max             = 0x40ff,  //制冷单元最大命令
}cmd_cooling_type_enum;

//制冷控制数据结构
typedef struct{
    app_u16 cmd;
    app_u16 switchctrl;  //开关控制 0-关闭 1-打开
}cmd_cooling_debug_stru;

//制冷参数设置结构体
typedef struct{
    app_u16 cmd;
    app_u16 close_temp;       //关闭制冷门限温度
    app_u16 open_temp;        //开启制冷门限温度
    app_u16 LoopPumpOpen_temp;//循环泵开启门限温度
}cmd_cooling_paraset_stru;

//--------------查询------------------
//上报温度显示结构体（实际温度为小数，需要扩大100倍后取整）
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    app_i32 temp_liqiud;
    app_i32 temp_board;
}cmd_cooling_temp_stru;

//上报风扇状态显示结构
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    char status[8];   //标记风扇状态，正常or异常
}cmd_cooling_fan_status_stru;

//上报电压显示结构体（实际电压为小数，需要扩大100倍后取整）
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    vos_u32 vol[16];
}cmd_cooling_vol_stru;

#endif
