//modifyed
#ifndef _CMD_REACTIONDISK_H_
#define _CMD_REACTIONDISK_H_



//��Ӧ�̵�Ԫ����ö�ٶ���
typedef enum
{
    cmd_react_disk_min               = 0x0400,  //��Ӧ�̵�Ԫ��С����
    cmd_react_disk_reset             = 0x0408,  //��Ӧ�̸�λ����
    cmd_react_disk_anticlockwise_rtt = 0x040c,  //��Ӧ����ʱ����תX����λ
    cmd_react_disk_clockwise_rtt     = 0x040d,  //��Ӧ��˳ʱ����תX����λ
    cmd_react_disk_light_control     = 0x040f,  //��Ӧ�̹�Դ���� 0-�ر� 100-��
    cmd_react_disk_get_adval         = 0x0410,  //��ȡ��Ӧ��ADֵ
    cmd_react_disk_src_2_dst         = 0x0411,  //��Ӧ��ָ����λ��ת��Ŀ��λ��
    cmd_react_disk_rtt_onecycle      = 0x0412,  //��Ӧ����תһ��
    cmd_react_disk_lightsize_ctl     = 0x0413,  //�����ȵ���
    
    cmd_react_disk_motor_hold_ctl    = 0x0450, //��Ӧ�̵���������ؿ���  
    cmd_react_disk_step_move         = 0x0451, //��Ӧ��΢���ƶ�����
    cmd_react_disk_get_LightBulbAD   = 0x0452, //��ȡ��Դ��ADֵ 

	cmd_react_disk_chip_reset         = 0x0470, //оƬ��λ
	cmd_react_disk_update            = 0x0480, //��Ӧ������
    
    cmd_react_disk_compensation      = 0x04c0,  //��Ӧ�̲�������
    cmd_react_disk_max               = 0x04ff,  //��Ӧ�̵�Ԫ�������
}cmd_reactdisk_type_enum;

#endif
