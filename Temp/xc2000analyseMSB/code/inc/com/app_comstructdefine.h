//modifyed
#ifndef _app_comstructdefine_h_
#define _app_comstructdefine_h_


#include "cmd_downmachine.h"
#include "xc8002_middwnmachshare.h"
#include "app_debugcmd.h"
#include "vos_messagecenter.h"

#define TESTMODE_LIGHTCHECK  (0)     //AD�ɼ����ڹ������
#define TESTMODE_NORMAL      (1)     //AD�ɼ���������ģʽ,�������Բ�AD
#define TESTMODE_DEBUG       (2)     //AD�ɼ���������debug
#define TESTMODE_SMOOTH 	 (3)     //AD�ɼ�����ƽ̹������
#define TESTMODE_DARKCURRENT (4)     //AD�ɼ����ڰ���������

typedef enum
{
    app_action_res_success = 0, //����ִ�гɹ�
    app_action_res_failed  = 1, //����ִ��ʧ��
    app_action_res_unknown = 2, //������֧��

    app_action_res_butt
}app_action_res_enum;

//��������ṹ����
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

//AD �߼���ͣ����
typedef enum
{
    app_ad_fpga_power_mode_off = 0, //ֹͣADת��
    app_ad_fpga_power_mode_on  = 1, //����ADת��

    app_ad_fpga_power_mode_butt
}app_ad_fpga_power_mode_enum;

//��ʼ��λ�����ڻ��ز���������Ϣ���ݽṹ
typedef struct
{
    vos_msg_header;
    app_u16 endwnmachname;
    app_u16 enloopendlayer;
    app_u16 ustimeoutlen;
    app_u16 uslpbcknum;
}app_start_lpbck_tst_req_msg_stru;

//AD �߼�����ģʽ
typedef enum
{
    app_ad_fpga_work_mode_nml 	 = 0,  //����ģʽ
    app_ad_fpga_work_mode_tst 	 = 1,  //����ģʽ
    app_ad_fpga_work_mode_smooth = 2,  //ƽ̹������ģʽ

    app_ad_fpga_work_mode_butt
}app_ad_fpga_work_mode_enum;

//��λ������vos��Ϣ�ṹ����
typedef struct
{
    vos_msg_header;
    app_u16 endwnmachsubsys;  //��λ��ϵͳ��(��Ӧ�̡��Լ��̡������̡��Լ�����ϵͳ)
    app_u16 usdevtype;
    app_u16 encmdtype;         //��������
    char  aucrsv[2];         //�����������
}msg_stDwnCmdReq;

//������λ����ʼ��
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
	
//������λ����ʼ�����
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

//������λ�������ϱ�����
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

//��λ������ִ�н��vos��Ϣ�ṹ����
typedef struct
{
    vos_msg_header;
    app_u16 endwnmachsubsys;  //��λ��ϵͳ��(��Ӧ�̡��Լ��̡������̡��Լ�����ϵͳ)
    app_u16 usdevtype;
    app_u16 encmdtype;         //��������
    app_u16 encmdexerslt;      //����ִ�н��
    vos_u32 ulpayloadlen;      //���س���
    char aucpayload[0];      //������Ϣ
}app_dwnmach_action_res_msg_stru;

//�����ⲿ�豸��Ӧ��Ϣ
typedef struct
{
    vos_msg_header;
    app_u16 enextdevname; //��������������
    app_u16 enopmode;     //�������ģʽ
    app_u16 enexerslt;    //��������������
    char  aucrsv[2];
}app_op_extdev_res_msg_stru;

//��λ���߼��豸����ģʽ������Ӧ��Ϣ
typedef struct
{
    vos_msg_header;
    app_u16 enextdevname;    //��������������
    app_u16 enopmode;        //�������ģʽ
    app_u16 enexerslt;       //��������������
    app_u16 enuartname;      //��������
}app_midmach_op_lpbck_res_msg_stru;

//��λ�����趨��
typedef enum
{
    app_midmach_extdev_name_fpga   = 0,  //��λ���߼�
    app_midmach_extdev_name_drvebi = 1,  //��λ��ebi����

    app_midmach_extdev_name_butt
}app_midmach_extdev_name_enum;

//��λ����������������Ͷ���
typedef enum
{
    app_op_extdev_action_reset           = 0,  //���踴λ
    app_op_extdev_action_set_lpbck_md    = 1,  //���軷��ģʽ����
    app_op_extdev_action_clr_lpbck_md    = 2,  //���軷��ģʽ���
    app_op_extdev_action_query_wk_status = 3,  //���蹤��״̬��ѯ
    app_op_extdev_action_switch_on       = 4,  //���迪�ش�
    app_op_extdev_action_switch_off      = 5,  //���迪�عر�
    
    app_operate_extdev_action_butt
}app_operate_extdev_action_enum;

//�ⲿ�豸״̬����
typedef enum
{
    app_midmach_extdev_status_normal   = 0,
    app_midmach_extdev_status_abnormal = 1,

    app_midmach_extdev_status_butt
}app_midmach_extdev_status_enum;

//��λ������vos��Ϣ���ؽṹ����
typedef struct
{
    app_u16 endwnmachsubsys;  //��λ��ϵͳ��(��Ӧ�̡��Լ��̡������̡��Լ�����ϵͳ)
    app_u16 usdevtype;
    app_u16 encmdtype;         //��������
    char  aucrsv[2];         //�����������
}app_dwnmach_action_req_msgload_stru;

#endif