//modifyed
#ifndef _CMD_DOWNMACHINE_H_
#define _CMD_DOWNMACHINE_H_

/*������ϵͳ*/
#include"cmd_sampledisk.h"
#include"cmd_samplemixing.h"
#include"cmd_sampleneedle.h"

/*��Ӧ��ϵͳ*/
#include"cmd_reactiondisk.h"
#include"cmd_reagentmixing.h"
#include"cmd_autowashneedle.h"

/*�Լ���ϵͳ*/
#include"cmd_innerreagentdisk.h"
#include"cmd_outerreagentdisk.h"
#include"cmd_r1reagentneedle.h"
#include"cmd_r2reagentneedle.h"

/*�¿���ϵͳ*/
#include "cmd_tempcontrol.h"

/*�Լ�������ϵͳ*/
#include "cmd_coolingreagent.h"
#include "stdint.h"
#define DWNPAGE_SIZE  (256)   //��λ������ҳ��С


#define XC8002_COM_DWNMACH_COMB_CMD (0x0c)


#define XC8002_COM_DWNMACH_LOOP_CMD (0xff03)
#define XC8002_COM_REPORT_CMD       (0xff04)//�ϱ�����ؼ���
#define XC8002_COM_QUERY_CMD        (0xff06)//��ѯ�¶�����ؼ���
#define XC8002_COM_FAULT_RESTORE_CMD (0xff07)//���ϻָ������ؼ���
#define XC8002_COM_MIDRUNSTATE_CMD  (0xff08)//��λ��֪ͨ��λ����λ������״̬����
#define XC8002_COM_COMQUERY_CMD     (0x0009)//��λ��ͨ�ò�ѯ����
#define XC8002_COM_OPERATION_CMD    (0xff0a)//��λ������-����̬�л�
#define XC8002_COM_BBCLEAR_CMD      (0xff0b)//�����λ����־
#define XC8002_COM_UPDATEDWN_CMD    (0xff0c)//��λ������
#define XC8002_COM_SET_MOTOR_CMD    (0xff0d)


/*���ڻ��ز��Բ㶨��*/
typedef enum
{
    xc8002_com_loopback_layer_linuxapp = 0, //��λ��Ӧ�ò�
    xc8002_com_loopback_layer_linuxdrv = 1, //��λ��������
    xc8002_com_loopback_layer_linuxlgc = 2, //��λ���߼���
    xc8002_com_loopback_layer_ucosdrv  = 3, //��λ��������
    xc8002_com_loopback_layer_ucosapp  = 4, //��λ��Ӧ�ò�

    xc8002_com_loopback_layer_butt
}xc8002_com_loopback_layer_enum;
typedef char xc8002_com_loopback_layer_enum_u8;

/*������λ��ͨ���������ݽṹ*/
typedef struct
{
    app_u16 cmd;
    char  cmdrsv[2];  //û�����������Ϊ���ֽڶ��뱣��λ,���򱣴����
}xc8002_com_dwnmach_cmd_stru;

/*������λ��ͨ�õ�������������ݽṹ*/
typedef struct
{
    vos_u32 cmdsize;  //�����ڴ��С
    xc8002_com_dwnmach_cmd_stru stcmdinfo;
}xc8002_com_comb_single_cmd_stru;

#pragma warning(disable:4200)
/*������λ��ͨ������������ݽṹ*/
typedef struct
{
    app_u16 cmd;
    app_u16 cmdnum;  //��������
    xc8002_com_comb_single_cmd_stru acmdinfo[0];
}xc8002_com_dwnmach_comb_cmd_stru;

/*���廷�ز����������ݽṹ*/
typedef struct
{
    app_u16 cmd;
    app_u16 usframeid;
    app_u16 usttl;
    app_u16 enloopbacklayer;  //���ز�
}xc8002_loopback_dwnmach_cmd_stru;

//��Ӧ�̹�Դ��ȡAD�������ݽṹ
typedef struct {
    app_u16 cmd;
    app_u16 reprot_cmd_type;
    vos_u32 dwnmachname; 
    app_u16 get_eeprom_lightintensity;  //��ȡEEPROM�洢ֵ
    app_u16 get_lamp_lightintensity;  //��ȡ�Ƶ��趨ֵ   
}cmd_react_disk_lightgetval_stru;

/*������λ���ϱ���״̬����*/
typedef enum
{
    xc8002_dwnmach_report_remain_type       = 1, //�����ϱ�
    xc8002_dwnmach_report_rctdsk_temp_type  = 2, //��Ӧ���¶��ϱ�����
    xc8002_dwnmach_report_reagdsk_temp_type = 3, //�Լ����¶��ϱ�����
    xc8002_dwnmach_report_fault_type        = 4, //��λ�������ϱ�����
    xc8002_dwnmach_report_close_reagcabin_type = 5, //�ر��Լ�������
    xc8002_dwnmach_report_open_reagcabin_type  = 6, //���Լ�������
    xc8002_dwnmach_report_downmachin_versions  = 7, //��λ���汾�ϱ�
    xc8002_dwnmach_report_LightBulbAD_type     = 8, //��Դ�Ƶ�ѹADֵ�ϱ�
    xc8002_dwnmach_report_compensation_type    = 9, //��λ����е��������
    xc8002_dwnmach_report_fanstatus_type       = 10,//��������״̬
    xc8002_dwnmach_report_electricity_type     = 11,//����Ƭ����
    xc8002_dwnmach_report_fl_and_pre_type      = 12,//���ӡ�ѹ����ѯ
    xc8002_dwnmach_report_reagdsk_temp_query_type   =13,//�����¶Ȳ�ѯ�ϱ����Ƕ�ʱ�ϱ�    
    xc8002_dwnmach_report_thermal_control_temp_type = 14,//�¿��¶Ȳ�ѯ�ϱ����Ƕ�ʱ�ϱ�
    xc8002_dwnmach_report_eeprom_info_type = 15,//eeprom�洢��Ϣ�ϱ�
    xc8002_dwnmach_report_bblog_info_type  = 16,     //��־��Ϣ�ϱ� 
    xc8002_dwnmach_report_keydown_innerdisk_type = 17,//���̰�����ת��ť
    xc8002_dwnmach_report_keydown_outerdisk_type = 18,//���̰�����ת��ť
    xc8002_dwnmach_report_bootloader_versions = 19,//bootloader�汾��

	xc8002_dwnmach_report_innerdisk_wave0_type = 20,//����ʾ��������0ͨ��
	xc8002_dwnmach_report_innerdisk_wave1_type = 21,//����ʾ��������1ͨ��
	xc8002_dwnmach_report_outerdisk_wave0_type = 22,//����ʾ��������0ͨ��
	xc8002_dwnmach_report_smpdisk_wave0_type   = 24,//����ʾ����������0ͨ��
	xc8002_dwnmach_report_pidstade			 = 28,        //pid״̬��¼
	xc8002_dwnmach_report_ad_temp_type =30,
    xc8002_dwnmach_report_GwayState = 31,           //���״̬�ϱ�
    xc8002_dwnmach_report_motorline_type = 40,   //��������ϱ�
	xc8002_dwnmach_report_clot_air_type = 41,	//������������ϱ�
    xc8001_dwnmach_report_ID = 42,
    dwnmach_report_shelf = 43,//�������ϱ�
    dwnmach_report_AD = 44,//��Դ��ADֵ�ϱ�
    dwnmach_show_wave = 45,
    xc8002_dwnmach_report_status_type_butt
}xc8002_dwnmach_report_status_type_enum;

/*������λ��״̬�ϱ���Ϣ�ṹ*/
typedef struct
{
	app_u16 cmd;              //�ϱ�����ؼ���:0xff04
	app_u16 usstatustype; //״̬����    
	char  aucloadinfo[0];   //��Ϣ����
}xc8002_com_report_cmd_stru;

//-----------------��������졢©����Ϣ�ṹ��-------------
typedef struct{
    app_u16 diskid;   //�̺ţ����������ϱ�
    app_u16 cupid;    //��λ�ţ����������ϱ�
    vos_u32 reactcup;   //����ţ�������졢©�촦��
}xc8002_cup_info_stru;

/*�����Լ������ϱ���Ϣ�ṹ��*/
typedef struct
{
    app_u16 cmd;              //��������
	app_u16 usstatustype;     //״̬����
	app_u16 usmechdevid;      //��е����id
    app_u16 usremaindepth;    //�Լ��������
    xc8002_cup_info_stru cupinfo; //����Ϣ
}xc8002_report_remaindepth_stru;
//--------------------------------------------------------

//��λ���¶��ϱ����ݽṹ
typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    app_i32 slcurtemp;    //��ǰ�¶�ֵ(=ʵ���¶�*10��,��ֹ��ϵͳ��С������)
}xc8002_report_temprature_status_stru;

//������λ���汾�ϱ��ṹ��(��fpga�汾��)
//ֻ����CPU���fpga�İ汾��
typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    char  date_time[32];
    char  dwnmachin_ver[24];
    vos_u32 fpga_ver;
}cmd_fpga_dwnmach_version_stru;

//CPU ID��
typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    vos_u32 id[4];
}cmd_CPUID;

typedef enum
{
    xc8002_fault_status_restore  = 0,//���ϻָ�
    xc8002_fault_status_generate = 1,//���ϲ���
    xc8002_fault_status_butt
} xc8002_fault_status_enum;//����״̬

//��λ��������Ϣ���ݽṹ��
typedef struct {
	unsigned char chCpuId;          //��Ƭ��ID
	unsigned char chFaultStatus;      //����״̬:�������ָ�
	app_u16 usFaultId;          //����ID
} xc8002_fault_info_stru;

//��λ�������ϱ����ݽṹ
typedef struct
{
    app_u16 cmd;              //�ϱ�����ؼ���:0xff04
    app_u16 enstatustype;     //�ϱ���������
    xc8002_fault_info_stru fault_info;
} xc8002_fault_report_stru;

//------------------------End: ��˫ϲ3.23���--------------------------------------
/*****end of fault check module*****/

//------------------------Begin: ��������--------------------------------------
//��ת��������ö��ֵ
typedef enum
{
	compensation_dir_clockwise      = 1,    //˳ʱ��
    compensation_dir_anticlockwise  = 2,    //��ʱ��
	
	compensation_dir_butt
}compensation_dir_enum;
typedef app_u16 compensation_dir_enum_u16;

//��ֱ��������ö��ֵ
typedef enum
{
	compensation_dir_up      = 1,    //����
    compensation_dir_down  = 2,    //����
	
	compensation_dir_vrt_butt
}compensation_dir_vrt_enum;

//��������ö��ֵ
typedef enum
{
	compensation_type_reset             = 1,    //��λ����
    compensation_type_rtt               = 2,    //��ת����
    compensation_type_mainslave_switch  = 3,    //�������л�	
    compensation_type_clear             = 4,    //�������ֵ
    compensation_type_vrt               = 5,    //��ֱ����
    
	compensation_type_butt
}compensation_type_enum;
typedef app_u16 compensation_type_enum_u16;

//��������
typedef struct {
    app_u16 location;   //����λ��
    compensation_dir_enum_u16 dir;        //�������� 1: ˳ʱ�� 2:��ʱ��
    vos_u32 step;       //�������� 
} cmd_compensation_para;


//------------------------Begin: ������-------------------------------------
typedef struct{
    app_u16 cmd;      //������
    app_u16 para;     //����
}cmd_simplecmd_stru;

/*******************���������para˵��*******************/
//����λ����������
typedef enum{
    cmd_bootinfo_reboot     = 0, //��λ������
    cmd_bootinfo_bootfinish = 1, //��λ��������ʼ�����

    cmd_bootinfo_butt = 2,
}cmd_bootinfo_enum;
//------------------------end:    ������-------------------------------------

//���巽��(����λ��һ��)
#define cmd_motor_dir_up                (0)
#define cmd_motor_dir_down              (1)
#define cmd_motor_dir_ANTICLOCKWISE     (1)
#define cmd_motor_dir_CLOCKWISE         (0)
#define cmd_motor_dir_IN				(1)
#define cmd_motor_dir_OUT				(0)

//--------------��λ��EEPROMֵ�ϱ�------------------------
//��������
typedef struct
{
    compensation_type_enum_u16 comptype;//����
    app_u16 location;        //λ��
    compensation_dir_enum_u16 dir;     //�������� 1: ˳ʱ�� 2:��ʱ��
    app_u16 step;           //�������� 
}xc8002_report_compensation_para;
/*����Ӽ������ýṹ��*/
typedef struct
{
    unsigned char  drvId;
    unsigned char  lineId;
    app_u16 startFre;			//����Ƶ��
    app_u16 endFre;			//���Ƶ��
    app_u16  echLaderStep;		//ÿ̨��΢��
    app_u16  upMaxLader;		//������̨��
    app_u16  sPar;				//S�����ߵ���״����}motor_para;
}xc8002_report_motor_para;

//��Ƭ����Ӧ����
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    app_u16 dwnmachinetype; //��λ����
    app_u16 paranum;  //��������
    xc8002_report_compensation_para ppara[0];  //�������� �䳤
}xc8002_report_dwnmachine_compensation;


//===========��λ�������ϱ��ṹ��==========
typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;

    app_u16 usCmdType;
    app_u16 enStatusType; //�ϱ���������
    app_u16 ulDwnmachName;  //��λ������
    app_u16 usParaNum;  //��������
    xc8002_report_compensation_para ppara[0];  //�������� �䳤
}msg_stDwnmachCompensationReprot;

//===========��λ������ϱ��ṹ��==========
typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;

    app_u16 usCmdType;
    app_u16 enStatusType; //�ϱ���������
    app_u16 ulDwnmachName;  //��λ������
    app_u16 usParaNum;  //��������
    xc8002_report_motor_para ppara[0];  //�������� �䳤
}msg_stDwnmachMotorReprot;

typedef struct
{
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    app_u16 dwnmachinetype; //��λ����
    app_u16 paranum;  	//��������
    xc8002_report_motor_para ppara[0];
}xc8002_report_dwnmachine_motorspeed;

typedef struct
{
	app_u16 cmd1;
	app_u16 enstatustype; //�ϱ���������
	app_u16	ClotValue;
	app_u16	AirMax;
	app_u16	AirSum;
	app_u16	WashMax;

}ClotAirDataReport_st;
//bblog����¼�ṹ��
typedef struct
{
    char record[64];    //һ����¼�б��������
}xc8002_bblog_record;

//��Ƭ��bblog
typedef struct {
    app_u16 cmd;    
    app_u16 enstatustype; //�ϱ���������    
    app_u16 currpage;     //��ǰҳ
    app_u16 recordnum;  //��־����
    xc8002_bblog_record records[];  //��־����
}xc8002_report_dwnmachine_bblog;


typedef struct
{
    app_u16 cmd;              //��������
    app_u16 usstatustype;     //״̬����
    app_u32 shelfNum;
}Report_shelf_st;

typedef struct
{
    app_u16 cmd;
    app_u16 usstatustype;     //״̬����    dwnmach_report_AD=44,//��Դ��ADֵ�ϱ�
    app_u32 AdCount;
    app_u32 value[0];
}ADValue_st;

typedef struct
{
    app_u16 cmd;
    app_u16 usstatustype;    //״̬����     31//���״̬�ϱ�
    app_u16 usLoca;          //λ��  
    app_u16 usState;         //״̬
}GwayState_st;


typedef struct {
    app_u16 cmd;
    app_u16 usstatustype;    //״̬����     9//�����ϱ�
    app_u16 dwnmachinetype;  //��λ����
    char number;             //ҳ��
    char count;              //����
    app_u32 datas[0];        /*����ֵ*/
}Report_PAGE_st;

typedef struct{
    app_u16 op;
    app_u16 ad;
} AD_OP_TYPE;

typedef struct {
    app_u16 cmd;
    app_u16 usstatustype;    //״̬����     45//�����ϱ�
    app_u32 dwnmachinetype;  //��λ����
    AD_OP_TYPE value[2000];
}Report_ShowOP_st;

#pragma warning(default:)
#endif
