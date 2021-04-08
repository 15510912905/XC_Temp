//modifyed
#ifndef _CMD_R1REAGENTNEEDLE_H_
#define _CMD_R1REAGENTNEEDLE_H_

#include"vos_basictypedefine.h"

//R1�Լ��뵥Ԫ����ö�ٶ���
typedef enum{
    cmd_r1_reagent_ndl_min               = 0x2c00, //R1�Լ��뵥Ԫ��С����
    cmd_r1_reagent_ndl_reset             = 0x2c08, //R1�Լ��븴λ����
    cmd_r1_reagent_ndl_vrise             = 0x2c09, //R1�Լ��봹ֱ����
    cmd_r1_reagent_ndl_vdecline          = 0x2c0a, //R1�Լ��봹ֱ�½�
    cmd_r1_reagent_ndl_hrotate_washpos   = 0x2c0d, //R1�Լ���ˮƽת����ϴλ
    cmd_r1_reagent_ndl_hrotate_absbpos   = 0x2c0e, //R1�Լ���ˮƽת�����Լ�λ
    cmd_r1_reagent_ndl_hrotate_dischpos  = 0x2c0f, //R1�Լ���ˮƽת�����Լ�λ
    cmd_r1_reagent_ndl_absorb_reagent    = 0x2c10, //R1�Լ������Լ�
    cmd_r1_reagent_ndl_discharge_reagent = 0x2c11, //R1�Լ������Լ�
    cmd_r1_reagent_ndl_exe_washaction    = 0x2c18, //R1�Լ���ִ����ϴ
    cmd_r1_reagent_ndl_absorb_water      = 0x2c19, //R1�Լ���������ˮ
    cmd_r1_reagent_ndl_marginscan        = 0x2c1a, //R1�Լ�������ɨ������
    cmd_r1_reagent_ndl_compensation      = 0x2cc0, //R1�Լ��벹������
    cmd_r1_reagent_ndl_comb_cmd          = 0x2ccc, //R1�Լ����������
    cmd_r1_reagent_ndl_slowdown          = 0x2ccd, //R1ָ���������ٶ��½�
    cmd_r1_reagent_pvctl_v08             = 0x2c23, //R1v08��������-��
    cmd_r1_reagent_pvctl_v03             = 0x2c24, //R1v03��������-��
    
    cmd_r1_ndl_vrise_setspeed            = 0x2c39, //R1�Լ��봹ֱ����
    cmd_r1_ndl_vdecline_setspeed         = 0x2c3a, //R1�Լ��봹ֱ�½�
    cmd_r1_ndl_hrotate_setspeed          = 0x2c3b, //R1�Լ���ˮƽת
    cmd_r1_ndl_absorb_reagent_setvol     = 0x2c3d, //R1�Լ������Լ�
    
    cmd_r1_ndl_motor_hold_ctl            = 0x2c50, //R1�����������ؿ���  
    cmd_r1_ndl_step_move                 = 0x2c51, //R1��΢���ƶ�����

	cmd_r1_ndl_chip_reset         		 = 0x2c70, //оƬ��λ
	cmd_r1_ndl_intensify_wash         	 = 0x2c71, //ǿ����ϴ
	cmd_r1_ndl_air_out         	  		 = 0x2c73, //ע�������
	cmd_r1_ndl_chip_update         		 = 0x2c80, //R1����

    cmd_r1_ndl_update                    = 0x2cf0, //R1����������
    cmd_r1_reagent_ndl_max               = 0x2cff, //R1�Լ��뵥Ԫ�������
}cmd_r1_reagentndl_type_enum;

//R1�Լ���ȡ�Լ�λ����
typedef enum{
    cmd_r1_reagentndl_inner_absrb_pos = 0, //R1�Լ�����Ȧȡ�Լ�λ
    cmd_r1_reagentndl_outer_absrb_pos = 1, //R1�Լ�����Ȧȡ�Լ�λ

    cmd_r1_reagentndl_absorb_position_butt
}cmd_r1_reagentndl_absorb_position_enum;
typedef char cmd_r1_reagentndl_absorb_position_enum_u8;

/*R1�Լ��봹ֱ��ת����ṹ*/
typedef struct{
    app_u16 cmd;
    char  enposition; //�˶�����Ŀ��λ��
    char  reportmarginflag;//�ϱ�������־��1:�ϱ���0:���ϱ�
    app_u16 ustotalvol; //�������
    char  rsv[2];
    app_u16 diskid;   //�̺ţ����������ϱ�
    app_u16 cupid;    //��λ�ţ����������ϱ�
    vos_u32 reactcup;   //����ţ�������졢©�촦��
}cmd_r1_reagent_ndl_vrotate_stru;

/*R1�Լ������Լ�������ˮ����ṹ*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //ȡ�Լ���
}cmd_r1_reagent_ndl_absorb_stru;

/*R1�Լ������Լ�����ṹ*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //���Լ���
    char  diluteflag;
    char  rsv[3];
}cmd_r1_reagent_ndl_discharge_stru;

#endif
