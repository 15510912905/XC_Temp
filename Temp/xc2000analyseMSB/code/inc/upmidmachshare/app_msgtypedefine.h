#ifndef _app_msgtypedefine_h_
#define _app_msgtypedefine_h_

typedef enum
{
    app_com_device_req_exe_act_msg_type = 0x03eb, //��������ִ�ж���������Ϣ
    app_com_device_res_exe_act_msg_type = 0x03ec, //��������ִ�ж�����Ӧ��Ϣ

    app_com_ad_data_ready_notice_msg_type = 0x03ef, //AD����׼����֪ͨ��Ϣ

    app_req_dwnmach_act_cmd_msg_type = 0x03f5, //������λ����������
    app_res_dwnmach_act_cmd_msg_type = 0x03f6, //��λ������������Ӧ


    app_req_spsuart_lpbck_tst_msg_type = 0x041b, //����ָ�����ڻ��ز�����Ϣ

    app_operate_extdev_res_msg_type = 0x041d, //�����ⲿ�豸��Ӧ��Ϣ

    app_dwnmachin_stautsrep_msg_type = 0x0421, //��λ��״̬�ϱ���Ϣ
    app_req_stop_midmach_msg_type = 0x042b, //��λ��ֹͣ������Ϣ

    app_maintain_msg_type = 0x044a,	 //ά��ģ����Ϣ

    app_outdisc_rotate_req = 0x464,	//(0x464) ���̰�ť���   
    app_indisc_rotate_req = 0x465,	//(0x465) ���̰�ť���

    MSG_STAddLiquid = 0x0467, //΢������ƽ̨-��������
    app_report_msg_type = 0x468,//app_statusrepdisposeģ�鵽��λ������ģ��ͨ��msg_type
    vos_appmsgtype_max,
}app_msgtype_enum;

#endif