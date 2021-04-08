//modifyed
#ifndef _app_comstructdefine_h_
#define _app_comstructdefine_h_


#include "cmd_downmachine.h"
#include "xc8002_middwnmachshare.h"
#include "app_debugcmd.h"
#include "vos_messagecenter.h"

#define TESTMODE_LIGHTCHECK  (0)     //AD采集用于光量检测
#define TESTMODE_NORMAL      (1)     //AD采集用于正常模式,整机测试采AD
#define TESTMODE_DEBUG       (2)     //AD采集用于用于debug
#define TESTMODE_SMOOTH 	 (3)     //AD采集用于平坦区测试
#define TESTMODE_DARKCURRENT (4)     //AD采集用于暗电流测试

typedef enum
{
    app_action_res_success = 0, //动作执行成功
    app_action_res_failed  = 1, //动作执行失败
    app_action_res_unknown = 2, //动作不支持

    app_action_res_butt
}app_action_res_enum;

//动作结果结构定义
typedef struct
{
    app_u16 enacttype;
    app_u16 enactrslt;
    app_u16 usreactcupid;
    app_u16 usdevtype;
    app_u32 ulActIndex;
    char  aucrsv[2];
    app_u16 usrsltinfolen;
    char  aucrsltinfo[0];
}app_com_dev_act_result_stru;

//AD 逻辑启停定义
typedef enum
{
    app_ad_fpga_power_mode_off = 0, //停止AD转换
    app_ad_fpga_power_mode_on  = 1, //启动AD转换

    app_ad_fpga_power_mode_butt
}app_ad_fpga_power_mode_enum;

//开始下位机串口环回测试请求消息数据结构
typedef struct
{
    vos_msg_header;
    app_u16 endwnmachname;
    app_u16 enloopendlayer;
    app_u16 ustimeoutlen;
    app_u16 uslpbcknum;
}app_start_lpbck_tst_req_msg_stru;

//AD 逻辑工作模式
typedef enum
{
    app_ad_fpga_work_mode_nml 	 = 0,  //正常模式
    app_ad_fpga_work_mode_tst 	 = 1,  //测试模式
    app_ad_fpga_work_mode_smooth = 2,  //平坦区测试模式

    app_ad_fpga_work_mode_butt
}app_ad_fpga_work_mode_enum;

//下位机动作vos消息结构定义
typedef struct
{
    vos_msg_header;
    app_u16 endwnmachsubsys;  //下位机系统名(反应盘、试剂盘、样本盘、试剂制冷系统)
    app_u16 usdevtype;
    app_u16 encmdtype;         //命令类型
    char  aucrsv[2];         //命令参数或保留
}msg_stDwnCmdReq;

//发送中位机初始化
#define SENDREBOOT2DOWNM() \
    vos_u32 endwnmachname = 0; \
    cmd_simplecmd_stru cmdinfo = {0}; \
    cmdinfo.cmd = XC8002_COM_MIDRUNSTATE_CMD; \
    cmdinfo.para= cmd_bootinfo_reboot; \
    printf("->Oammng: send reboot to downmachine.\r\n"); \
    while (xc8002_dwnmach_name_fpga_ad > endwnmachname) \
    {\
        app_sendMidRunStateToDwnmach(endwnmachname,&cmdinfo); \
        endwnmachname++;\
    }
	
//发送中位机初始化完成
#define SENDBOOTFINISH2DOWNM() \
    vos_u32 endwnmachname = 0; \
    cmd_simplecmd_stru cmdinfo = {0}; \
    cmdinfo.cmd = XC8002_COM_MIDRUNSTATE_CMD; \
    cmdinfo.para= cmd_bootinfo_bootfinish; \
    printf("->Oammng: send boot finish to downmachine.\r\n"); \
    while (xc8002_dwnmach_name_fpga_ad > endwnmachname) \
    {\
        app_sendMidRunStateToDwnmach(endwnmachname,&cmdinfo); \
        endwnmachname++;\
    }

//发送下位机故障上报控制
#define SENDFAULTCTL(ctl) \
    vos_u32 endwnmachname = 0; \
    cmd_simplecmd_stru cmdinfo = {0}; \
    cmdinfo.cmd = XC8002_COM_MIDRUNSTATE_CMD; \
    cmdinfo.para= (ctl); \
    while(xc8002_dwnmach_name_fpga_ad > endwnmachname) \
    {\
        app_sendMidRunStateToDwnmach(endwnmachname,&cmdinfo); \
        endwnmachname++;\
    }

//下位机动作执行结果vos消息结构定义
typedef struct
{
    vos_msg_header;
    app_u16 endwnmachsubsys;  //下位机系统名(反应盘、试剂盘、样本盘、试剂制冷系统)
    app_u16 usdevtype;
    app_u16 encmdtype;         //命令类型
    app_u16 encmdexerslt;      //命令执行结果
    vos_u32 ulpayloadlen;      //负载长度
    char aucpayload[0];      //负载信息
}app_dwnmach_action_res_msg_stru;

//操作外部设备响应消息
typedef struct
{
    vos_msg_header;
    app_u16 enextdevname; //操作的外设名称
    app_u16 enopmode;     //外设操作模式
    app_u16 enexerslt;    //操作的外设名称
    char  aucrsv[2];
}app_op_extdev_res_msg_stru;

//中位机逻辑设备环回模式操作响应消息
typedef struct
{
    vos_msg_header;
    app_u16 enextdevname;    //操作的外设名称
    app_u16 enopmode;        //外设操作模式
    app_u16 enexerslt;       //操作的外设名称
    app_u16 enuartname;      //串口名称
}app_midmach_op_lpbck_res_msg_stru;

//中位机外设定义
typedef enum
{
    app_midmach_extdev_name_fpga   = 0,  //中位机逻辑
    app_midmach_extdev_name_drvebi = 1,  //中位机ebi驱动

    app_midmach_extdev_name_butt
}app_midmach_extdev_name_enum;

//中位机外设操作动作类型定义
typedef enum
{
    app_op_extdev_action_reset           = 0,  //外设复位
    app_op_extdev_action_set_lpbck_md    = 1,  //外设环回模式设置
    app_op_extdev_action_clr_lpbck_md    = 2,  //外设环回模式清除
    app_op_extdev_action_query_wk_status = 3,  //外设工作状态查询
    app_op_extdev_action_switch_on       = 4,  //外设开关打开
    app_op_extdev_action_switch_off      = 5,  //外设开关关闭
    
    app_operate_extdev_action_butt
}app_operate_extdev_action_enum;

//外部设备状态定义
typedef enum
{
    app_midmach_extdev_status_normal   = 0,
    app_midmach_extdev_status_abnormal = 1,

    app_midmach_extdev_status_butt
}app_midmach_extdev_status_enum;

//下位机动作vos消息负载结构定义
typedef struct
{
    app_u16 endwnmachsubsys;  //下位机系统名(反应盘、试剂盘、样本盘、试剂制冷系统)
    app_u16 usdevtype;
    app_u16 encmdtype;         //命令类型
    char  aucrsv[2];         //命令参数或保留
}app_dwnmach_action_req_msgload_stru;

#endif