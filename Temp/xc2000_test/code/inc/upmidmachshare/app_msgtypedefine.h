#ifndef _app_msgtypedefine_h_
#define _app_msgtypedefine_h_

typedef enum
{
    app_com_device_req_exe_act_msg_type = 0x03eb, //公用仪器执行动作请求消息
    app_com_device_res_exe_act_msg_type = 0x03ec, //公用仪器执行动作响应消息

    app_com_ad_data_ready_notice_msg_type = 0x03ef, //AD数据准备好通知消息

    app_req_dwnmach_act_cmd_msg_type = 0x03f5, //请求下位机动作命令
    app_res_dwnmach_act_cmd_msg_type = 0x03f6, //下位机动作命令响应


    app_req_spsuart_lpbck_tst_msg_type = 0x041b, //请求指定串口环回测试消息

    app_operate_extdev_res_msg_type = 0x041d, //操作外部设备响应消息

    app_dwnmachin_stautsrep_msg_type = 0x0421, //下位机状态上报信息
    app_req_stop_midmach_msg_type = 0x042b, //中位机停止请求消息

    app_maintain_msg_type = 0x044a,	 //维护模块消息

    app_outdisc_rotate_req = 0x464,	//(0x464) 外盘按钮点击   
    app_indisc_rotate_req = 0x465,	//(0x465) 内盘按钮点击

    MSG_STAddLiquid = 0x0467, //微量加样平台-调试命令
    app_report_msg_type = 0x468,//app_statusrepdispose模块到中位机其他模块通用msg_type
    vos_appmsgtype_max,
}app_msgtype_enum;

#endif