//modifyed
#ifndef _CMD_R2REAGENTNEEDLE_H_
#define _CMD_R2REAGENTNEEDLE_H_

#include"vos_basictypedefine.h"

//R2�Լ��뵥Ԫ����ö�ٶ���
typedef enum{
    cmd_r2_reagent_ndl_min               = 0x3000, //R2�Լ��뵥Ԫ��С����
    cmd_r2_reagent_ndl_reset             = 0x3008, //R2�Լ��븴λ����
    cmd_r2_reagent_ndl_vrise             = 0x3009, //R2�Լ��봹ֱ����
    cmd_r2_reagent_ndl_vdecline          = 0x300a, //R2�Լ��봹ֱ�½�
    cmd_r2_reagent_ndl_hrotate_washpos   = 0x300d, //R2�Լ���ˮƽת����ϴλ
    cmd_r2_reagent_ndl_hrotate_absbpos   = 0x300e, //R2�Լ���ˮƽת�����Լ�λ
    cmd_r2_reagent_ndl_hrotate_dischpos  = 0x300f, //R2�Լ���ˮƽת�����Լ�λ
    cmd_r2_reagent_ndl_absorb_reagent    = 0x3010, //R2�Լ������Լ�
    cmd_r2_reagent_ndl_discharge_reagent = 0x3011, //R2�Լ������Լ�
    cmd_r2_reagent_ndl_exe_washaction    = 0x3018, //R2�Լ���ִ����ϴ
    cmd_r2_reagent_ndl_absorb_water      = 0x3019, //R2�Լ���������ˮ
    cmd_r2_reagent_ndl_marginscan        = 0x301a, //R2�Լ�������ɨ������
    cmd_r2_reagent_ndl_compensation      = 0x30c0, //R2�Լ��벹������
    cmd_r2_reagent_ndl_comb_cmd          = 0x30cc, //R2�Լ����������
    cmd_r2_reagent_ndl_slowdown          = 0x30cd, //R2ָ���������ٶ��½�
    cmd_r2_reagent_pvctl_v09             = 0x3023, //R2�÷�����
    cmd_r2_reagent_pvctl_v04             = 0x3024, //R2�÷�����

    cmd_r2_ndl_vrise_setspeed            = 0x3039, //R2�Լ��봹ֱ����
    cmd_r2_ndl_vdecline_setspeed         = 0x303a, //R2�Լ��봹ֱ�½�
    cmd_r2_ndl_hrotate_setspeed          = 0x303b, //R2�Լ���ˮƽת
    cmd_r2_ndl_absorb_reagent_setvol     = 0x303d, //R2�Լ������Լ�
    
    cmd_r2_ndl_motor_hold_ctl            = 0x3050, //R2�����������ؿ���  
    cmd_r2_ndl_step_move                 = 0x3051, //R2��΢���ƶ�����

	cmd_r2_ndl_chip_reset         		 = 0x3070, //оƬ��λ
	cmd_r2_ndl_intensify_wash         	 = 0x3071, //ǿ����ϴ
	cmd_r2_ndl_air_out         	  		 = 0x3073, //ע�������
	cmd_r2_ndl_update            		 = 0x3080, //R2 ����
    
    cmd_r2_reagent_ndl_max               = 0x30ff, //R2�Լ��뵥Ԫ�������
}cmd_r2_reagentndl_type_enum;

//R2�Լ���ȡ�Լ�λ����
typedef enum{
    cmd_r2_reagentndl_inner_absrb_pos = 0, //R2�Լ�����Ȧȡ�Լ�λ
    cmd_r2_reagentndl_outer_absrb_pos = 1, //R2�Լ�����Ȧȡ�Լ�λ

    cmd_r2_reagentndl_absorb_position_butt
}cmd_r2_reagentndl_absorb_position_enum;
typedef char cmd_r2_reagentndl_absorb_position_enum_u8;

/*R2�Լ����Լ�����������������ݽ��*/
typedef struct{
    app_u16 uscmdtype;
    char  enexerslt;      //����ִ�н��
    char  aucrsv[1];
    app_u16 usdeclinehigh;  //���½��߶�
    app_u16 usrsv;
}cmd_r2_reagent_marginscan_res_stru;

#endif
