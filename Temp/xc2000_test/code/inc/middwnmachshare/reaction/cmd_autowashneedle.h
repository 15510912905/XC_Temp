//modifyed
#ifndef __CMD_AUTOWASHNEEDLE_H__
#define __CMD_AUTOWASHNEEDLE_H__



//�Զ���ϴ�뵥Ԫ����ö�ٶ���
typedef enum{
    cmd_auto_wash_ndl_min           = 0x0c00, //�Զ���ϴ�뵥Ԫ��С����
    cmd_auto_wash_ndl_reset         = 0x0c08, //�Զ���ϴ�뵥Ԫ��λ����
    cmd_auto_wash_ndl_vrise         = 0x0c09, //�Զ���ϴ�뵥Ԫ��ֱ�����˶�����
    cmd_auto_wash_ndl_vdecline      = 0x0c0a, //�Զ���ϴ�뵥Ԫ��ֱ�����˶�����
    cmd_auto_wash_ndl_exe_washing   = 0x0c15, //�Զ���ϴ�뵥Ԫִ����ϴ����
    cmd_auto_wash_ndl_slowdown      = 0x0c17, //�Զ���ϴ�뵥Ԫָ���������ٶ��½�
    cmd_auto_wash_pvctl_p03         = 0x0c11,//�Զ���ϴ����ȥ����ˮ��
    cmd_auto_wash_pvctl_p04         = 0x0c12,//�Զ���ϴ���س�����ѹ��
    cmd_auto_wash_pvctl_v05         = 0x0c13,//�Զ���ϴ���ػ�����
    cmd_auto_wash_pvctl_v14         = 0x0c14,//�Զ���ϴ����1��
    cmd_auto_wash_pvctl_v15         = 0x0c18,//�Զ���ϴ����2��
    cmd_auto_wash_pvctl_v16         = 0x0c19,//�Զ���ϴ����3��
    cmd_auto_wash_pvctl_v17         = 0x0c1a,//�Զ���ϴ����4��
    cmd_auto_wash_pvctl_v18         = 0x0c1b,//�Զ���ϴ����5��
    cmd_auto_wash_pvctl_v19         = 0x0c1c,//�Զ���ϴ����6��
    cmd_auto_wash_pvctl_p06         = 0x0c1d,//�Զ���ϴ����ϡ����ϴ����
    cmd_auto_wash_pvctl_p08         = 0x0c1f,//�Զ���ϴ��������ձ�
    cmd_auto_wash_pvctl_v23         = 0x0c20,//�Զ���ϴ���ض��������Һ��
    cmd_auto_wash_pvctl_v25         = 0x0c21,//�Զ���ϴ���ض�����շ�����
    cmd_auto_wash_pvctl_v27         = 0x0c22,//�Զ���ϴ�����������Һ��   
    cmd_auto_wash_cup_flood         = 0x0c23,//�Զ���ϴ��עҺ
    cmd_auto_wash_cup_absorb        = 0x0c24,//�Զ���ϴ����Һ
    cmd_auto_wash_openP04           = 0x0c25,   //������ѹ�û���������
    cmd_auto_wash_pnctl             = 0x0c30,   //�����÷����ؿ���
    
    cmd_auto_wash_test_1            = 0x0c31,   //��ϴ����ϴ��������
    cmd_auto_wash_test_5            = 0x0c36,   //��ϴˮ��ϴ��������
    cmd_auto_wash_test_8            = 0x0c3a,   //���������Һ��������
    cmd_auto_wash_test_9            = 0x0c3c,   //���������Һ��������
    
    cmd_auto_wash_motor_hold_ctl    = 0x0c50, //�Զ���ϴ�����������
    cmd_auto_wash_step_move         = 0x0c51, //�Զ���ϴ���΢������
	cmd_auto_wash_chip_reset        = 0x0c70, //оƬ��λ
	cmd_auto_wash_update            = 0x0c80, //��ϴ��������
    cmd_auto_wash_ndl_compensation  = 0x0cc0,//�Զ���ϴ�벹������
    cmd_auto_wash_control  			= 0x0cc1,//ѹ����⡢����Һλ��⿪��
	cmd_auto_wash_setFLstae         = 0x0cc2,   //���ø���״̬
	cmd_sleepCtl					= 0x0cc3, //Һ·���߿���
    cmd_auto_wash_ndl_max           = 0x0cff, //�Զ���ϴ�뵥Ԫ�������
    cmd_atuo_wash_state_switch      = 0x0080, //�Զ���ϴ״̬�л�
}cmd_auto_wash_ndl_type_enum;

//---------------------��ѯ-------------------------
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    char fl_status[8];  //��Ǹ��ӿ���״̬
    app_u16 pres_ad[4];   //ѹ��adֵ
}cmd_autowash_status_stru;

#endif

