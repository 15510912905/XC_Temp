//modifyed
#ifndef _CMD_SAMPLENEEDLE_H_
#define _CMD_SAMPLENEEDLE_H_

#include"vos_basictypedefine.h"

//�����뵥Ԫ����ö�ٶ���
typedef enum
{
    cmd_sample_ndl_min                = 0x2400, //�����뵥Ԫ��С����
    cmd_sample_ndl_reset              = 0x2408, //�����븴λ����
    cmd_sample_ndl_vrise              = 0x2409, //�����봹ֱ�����˶�
    cmd_sample_ndl_vdecline           = 0x240a, //�����봹ֱ�����˶�
    cmd_sample_ndl_hrtt_nml_dischpos  = 0x240d, //������ˮƽ��ת������������λ
    cmd_sample_ndl_hrtt_ise_dischpos  = 0x241a, //������ˮƽ��ת��ISE������λ
    cmd_sample_ndl_hrtt_dsk_absrb1pos = 0x240e, //������ˮƽ��ת��������������λ1(����Ȧ)
    cmd_sample_ndl_hrtt_dsk_absrb2pos = 0x240f, //������ˮƽ��ת��������������λ2
    cmd_sample_ndl_hrtt_dsk_absrb3pos = 0x2410, //������ˮƽ��ת��������������λ3
    cmd_sample_ndl_hrtt_dsk_absrb4pos = 0x2411, //������ˮƽ��ת��������������λ4
    cmd_sample_ndl_hrtt_dsk_absrb5pos = 0x2412, //������ˮƽ��ת��������������λ5
    cmd_sample_ndl_hrtt_dilute_pos    = 0x241c, //������ˮƽ��ת��ϡ��λ
    cmd_sample_ndl_hrtt_rail_absrbpos = 0x241b, //������ˮƽ��ת�����������λ
    cmd_sample_ndl_hrtt_wash_pos      = 0x2413, //������ˮƽ��ת����ϴλ
    cmd_sample_ndl_absorb_sample      = 0x241d, //������������
    cmd_sample_ndl_discharge          = 0x241e, //������������
    cmd_sample_ndl_exe_wash           = 0x241f, //��������ϴ��������
    cmd_sample_ndl_absorb_water       = 0x2420, //������������ˮ
    cmd_sample_ndl_slowdown           = 0x2421, //������ָ���������ٶ��½�
    cmd_sample_pvctl_v07              = 0x2423, //������÷�����-��
    cmd_sample_pvctl_v02              = 0x2424, //������÷�����-��
    
    cmd_sample_ndl_vrise_setspeed     = 0x2439, //ָ���ٶ������봹ֱ�����˶�
    cmd_sample_ndl_vdecline_setspeed  = 0x243a, //ָ���ٶ������봹ֱ�����˶�
    cmd_sample_ndl_hrtt_setspeed      = 0x243b, //ָ���ٶ�������ˮƽ��ת��ָ��λ��
    cmd_sample_ndl_absorb_sample_setvol=0x243d, //ָ�������������

    cmd_sample_ndl_set_blood_clot_value=0x2440, //����Ѫ����������ֵ
    
    cmd_sample_ndl_motor_hold_ctl     = 0x2450, //���������������ؿ���  
    cmd_sample_ndl_step_move          = 0x2451, //������΢���ƶ�����

	cmd_sample_ndl_chip_reset         = 0x2470, //оƬ��λ
	cmd_sample_ndl_intensify_wash     = 0x2471, //ǿ����ϴ
	cmd_sample_ndl_air_out         	  = 0x2473, //ע�������
	cmd_sample_ndl_update             = 0x2480, //����������
    
    cmd_sample_ndl_compensation       = 0x24c0, //�����벹������
    cmd_sample_ndl_pressurecheck      = 0x24c1, //������ѹ�����
    cmd_sample_ndl_long_wash          = 0x24c2, //��������ϴ
    cmd_sample_ndl_wash_rtt           = 0x24c3, //����ϴ��ˮƽ�˶�
    cmd_sample_ndl_max                = 0x24ff, //�����뵥Ԫ�������
}cmd_sampleneddle_type_enum;

//������ˮƽλ�ö���
typedef enum
{
    cmd_sample_ndl_1circle_absorb_spot  = 0,  //����Ȧȡ����λ
    cmd_sample_ndl_2circle_absorb_spot  = 1,
    cmd_sample_ndl_3circle_absorb_spot  = 2,
    cmd_sample_ndl_4circle_absorb_spot  = 3,
    cmd_sample_ndl_5circle_absorb_spot  = 4,  //����Ȧȡ����λ
	cmd_sample_ndl_wash_spot            = 5,  //��ϴλ
    cmd_sample_ndl_rail_absorb_spot     = 6,  //�����ȡ����λ
	cmd_sample_ndl_dilute_spot          = 7,  //ϡ��λ
    cmd_sample_ndl_nml_discharge_spot   = 8,  //��Ӧ��������λ
    cmd_sample_ndl_ise_discharge_spot   = 9,  //ISE������λ
    
    
    cmd_sample_ndl_absorb_spot_butt
}cmd_sampleneddle_absorb_spot_enum;

/*������������or����ˮ�����������ݽṹ*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;//��������
}cmd_sample_ndl_absrb_stru;

/*�����������������������ݽṹ*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //���Լ���
    char  diluteflag;
    char  rsv[3];
}cmd_sample_ndl_discharge_stru;

#endif
