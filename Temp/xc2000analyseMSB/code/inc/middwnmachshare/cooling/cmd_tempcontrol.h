//modifyed
#ifndef __CMD_TEMPCONTROL_H__
#define __CMD_TEMPCONTROL_H__



//温度控制单元命令枚举定义
typedef enum
{
    cmd_temp_control_min                    = 0x3800,   //温度控制单元最小命令
    cmd_temp_control_switch                 = 0x3801,   //温度控制开关 0-关闭 1-打开  
    cmd_temp_control_set_temp           	= 0x3804,   //设置目标温度
    cmd_temp_control_set_reacttemp          = 0x3805,   //设置反应盘目标温度    
    cmd_temp_three_routes_ctl               = 0x3806,   //三路温控开关控制
	cmd_temp_sleep							= 0x3807,	//三路温控休眠

	cmd_temp_control_chip_reset         	= 0x3870, //芯片复位
	cmd_temp_control_update             	= 0x3880, //温控升级
	
    cmd_temp_control_max                    = 0x38ff,   //温度控制单元最大命令
}cmd_tempcontrol_type_enum;

//温度控制数据结构
typedef struct {
    app_u16 cmd;
    app_u16 switchctrl;  //开关控制 0-关闭 1-打开
}cmd_temp_ctrl_stru;

//加热器控制
typedef struct {
    app_u16 cmd;
    char heaternum;     //加热器编号(1-3三路，4反应盘，5全部)
    char switchctrl;    //开关控制 0-关闭 1-打开
}cmd_heater_ctrl_stru;

//反应盘目标温度
typedef struct {
    app_u16 cmd;
    app_u16 delta;        
    vos_u32 tempreact;  //反应盘目标温度
}cmd_reactdisk_tempset_stru;

//三路清洗加热器目标温度
typedef struct {
    app_u16 cmd;
    app_u16 temp1;     //加热器1目标温度
    app_u16 temp2;     //加热器2目标温度
    app_u16 temp3;     //加热器3目标温度
}cmd_heater_targettemp_stru;

//-------------------调试上报------------------------//

//温控上报信息结构体（实际温度为小数，需要扩大100倍后取整）
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    app_i32 temp_reac1; //1-3分别对应反应盘温度1、反应盘温度2、反应盘温度3
    app_i32 temp_reac2;
    app_i32 temp_reac3;
    app_i32 temp_env;          //环境温度
    app_i32 temp_washmach;       //整机清洗水温度
    app_i32 temp_washcup;        //反应杯清洗水温度
    app_i32 temp_detergentcup;   //反应杯清洗剂
    app_u16 duty_reaction;
    app_u16 duty_washmach;
    app_u16 duty_washcup;
    app_u16 duty_detergentcup;
}cmd_temp_control_report_stru;

typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    app_u16 tempreact;
    app_u16 tempwash1;
    app_u16 tempwash2;
    app_u16 tempwash3;
    app_u16 delta;
    app_u16 rsv;
}cmd_temp_control_eeprom_info_stru;

typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    app_u16 close_temp;
    app_u16 open_temp;
    app_u16 LoopPumpOpen_temp;
}cmd_temp_cooling_paraset_stru;
typedef struct {
	app_u16 cmd;
	app_u16 enstatustype; //上报数据类型
	app_u16 dwnmachname;  //下位机名称
	app_u16 state;
	float kp;
	float ki;
	float kd;
	float dsttmp;
}cmd_temp_pid_stru;

#endif
