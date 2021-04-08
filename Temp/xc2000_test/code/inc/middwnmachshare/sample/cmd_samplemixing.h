//modifyed
#ifndef _CMD_SAMPLEMIXING_H_
#define _CMD_SAMPLEMIXING_H_

#include"vos_basictypedefine.h"

//�������赥Ԫ����ö�ٶ���
typedef enum
{
    cmd_sample_mix_ndl_min             = 0x2000, //�������赥Ԫ��С����
    cmd_sample_mix_ndl_reset           = 0x2008, //�������赥Ԫ��λ����
    cmd_sample_mix_ndl_vrise           = 0x2009, //���������봹ֱ�����˶�����
    cmd_sample_mix_ndl_vdecline        = 0x200a, //���������봹ֱ�����˶�����
    cmd_sample_mix_ndl_hrotate_washpos = 0x200d, //����������ˮƽת������ϴλ����
    cmd_sample_mix_ndl_hrotate_mixpos  = 0x200e, //����������ˮƽת��������λ����
    cmd_sample_mix_ndl_exe_washing     = 0x2015, //����������ִ����ϴ����
    cmd_sample_mix_ndl_exe_mixxing     = 0x2016, //����������ִ�н�������
    cmd_sample_mix_ndl_v10_ctrl        = 0x2017, //�������������ϴ��ŷ�V10����
    cmd_sample_mix_ndl_slowdown        = 0x2018, //���������ָ���������ٶ��½�
    cmd_sample_pvctl_v10               = 0x2019, //��������˱÷���������
    
    cmd_sample_mix_motor_hold_ctl      = 0x2050, //�����������������ؿ���  
    cmd_sample_mix_step_move           = 0x2051, //��������΢���ƶ�����

	cmd_sample_mix_chip_reset          = 0x2070, //оƬ��λ
	cmd_sample_mix_update              = 0x2080, //������������
    
    cmd_sample_mix_ndl_compensation    = 0x20c0, //��������˲�������
    cmd_sample_mix_ndl_max             = 0x20ff, //�������赥Ԫ�������
}cmd_sample_mix_ndl_type_enum;

#endif
