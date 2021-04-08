//modifyed
#ifndef __CMD_AUTOWASHNEEDLE_H__
#define __CMD_AUTOWASHNEEDLE_H__



//自动清洗针单元命令枚举定义
typedef enum{
    cmd_auto_wash_ndl_min           = 0x0c00, //自动清洗针单元最小命令
    cmd_auto_wash_ndl_reset         = 0x0c08, //自动清洗针单元复位命令
    cmd_auto_wash_ndl_vrise         = 0x0c09, //自动清洗针单元垂直向上运动命令
    cmd_auto_wash_ndl_vdecline      = 0x0c0a, //自动清洗针单元垂直向下运动命令
    cmd_auto_wash_ndl_exe_washing   = 0x0c15, //自动清洗针单元执行清洗命令
    cmd_auto_wash_ndl_slowdown      = 0x0c17, //自动清洗针单元指定步数、速度下降
    cmd_auto_wash_pvctl_p03         = 0x0c11,//自动清洗开关去离子水泵
    cmd_auto_wash_pvctl_p04         = 0x0c12,//自动清洗开关齿轮增压泵
    cmd_auto_wash_pvctl_v05         = 0x0c13,//自动清洗开关回流阀
    cmd_auto_wash_pvctl_v14         = 0x0c14,//自动清洗开关1阶
    cmd_auto_wash_pvctl_v15         = 0x0c18,//自动清洗开关2阶
    cmd_auto_wash_pvctl_v16         = 0x0c19,//自动清洗开关3阶
    cmd_auto_wash_pvctl_v17         = 0x0c1a,//自动清洗开关4阶
    cmd_auto_wash_pvctl_v18         = 0x0c1b,//自动清洗开关5阶
    cmd_auto_wash_pvctl_v19         = 0x0c1c,//自动清洗开关6阶
    cmd_auto_wash_pvctl_p06         = 0x0c1d,//自动清洗开关稀释清洗剂泵
    cmd_auto_wash_pvctl_p08         = 0x0c1f,//自动清洗开关主真空泵
    cmd_auto_wash_pvctl_v23         = 0x0c20,//自动清洗开关二级真空吸液阀
    cmd_auto_wash_pvctl_v25         = 0x0c21,//自动清洗开关二级真空放气阀
    cmd_auto_wash_pvctl_v27         = 0x0c22,//自动清洗开关主真空排液阀   
    cmd_auto_wash_cup_flood         = 0x0c23,//自动清洗杯注液
    cmd_auto_wash_cup_absorb        = 0x0c24,//自动清洗杯吸液
    cmd_auto_wash_openP04           = 0x0c25,   //齿轮增压泵回流阀控制
    cmd_auto_wash_pnctl             = 0x0c30,   //单个泵阀开关控制
    
    cmd_auto_wash_test_1            = 0x0c31,   //清洗剂清洗调试命令
    cmd_auto_wash_test_5            = 0x0c36,   //清洗水清洗调试命令
    cmd_auto_wash_test_8            = 0x0c3a,   //开主真空排液调试命令
    cmd_auto_wash_test_9            = 0x0c3c,   //关主真空排液调试命令
    
    cmd_auto_wash_motor_hold_ctl    = 0x0c50, //自动清洗电机保持力矩
    cmd_auto_wash_step_move         = 0x0c51, //自动清洗电机微步调整
	cmd_auto_wash_chip_reset        = 0x0c70, //芯片复位
	cmd_auto_wash_update            = 0x0c80, //清洗机构升级
    cmd_auto_wash_ndl_compensation  = 0x0cc0,//自动清洗针补偿命令
    cmd_auto_wash_control  			= 0x0cc1,//压力检测、浮子液位检测开关
	cmd_auto_wash_setFLstae         = 0x0cc2,   //设置浮子状态
	cmd_sleepCtl					= 0x0cc3, //液路休眠控制
    cmd_auto_wash_ndl_max           = 0x0cff, //自动清洗针单元最大命令
    cmd_atuo_wash_state_switch      = 0x0080, //自动清洗状态切换
}cmd_auto_wash_ndl_type_enum;

//---------------------查询-------------------------
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //上报数据类型
    vos_u32 dwnmachname;  //下位机名称
    char fl_status[8];  //标记浮子开关状态
    app_u16 pres_ad[4];   //压力ad值
}cmd_autowash_status_stru;

#endif

