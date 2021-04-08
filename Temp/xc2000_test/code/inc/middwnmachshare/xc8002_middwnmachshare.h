//modified
#ifndef _xc8002_middwnmachshare_h_
#define _xc8002_middwnmachshare_h_

#include "cmd_downmachine.h"

#define xc8002_cmdkey_mapto_cpuid(cmd) ((app_u16)(((cmd)&0xff00)>>10))
#define LiquidErr  0xf
//����ִ�н������
typedef enum
{
    xc8002_exe_cmd_sucess  = 0,
    xc8002_exe_cmd_failed  = 1,
    xc8002_exe_cmd_timeout = 2,
}xc8002_exe_cmd_result_enum;

//Ӳ����Ԫ���Ͷ���
typedef enum
{
    xc8002_dwnmach_name_smpndl          = 1,   //�����뵥Ԫ-��
    xc8002_dwnmach_name_smpndlb         = 2,   //�����뵥Ԫ-��
    xc8002_dwnmach_name_reactdisk       = 3,   //��Ӧ�̵�Ԫ
    xc8002_dwnmach_name_autowash        = 4,   //�Զ���ϴ��Ԫ
    xc8002_dwnmach_name_smpl_mixrod     = 5,   //��������˵�Ԫ
    xc8002_dwnmach_name_reag_mixrod     = 6,   //�Լ�����˵�Ԫ
    xc8002_dwnmach_name_r1Ndl           = 7,   //R1�Լ��뵥Ԫ-��
    xc8002_dwnmach_name_R1disk          = 8,   //R1�Լ��̵�Ԫ
    xc8002_dwnmach_name_r1bNdl          = 9,   //R1�Լ��뵥Ԫ-��
    xc8002_dwnmach_name_r2Ndl           = 10,  //R2�Լ��뵥Ԫ-��
    xc8002_dwnmach_name_R2disk          = 11,  //R2�Լ��̵�Ԫ
    xc8002_dwnmach_name_r2bNdl          = 12,  //R2�Լ��뵥Ԫ-��   
    xc8002_dwnmach_name_auto_Rtmpctrl   = 13,  //��Ӧ���¿ؿ���
    xc8002_dwnmach_name_auto_wtmpctrl   = 14,  //��ϴˮ�¶ȿ���
    xc8002_dwnmach_name_cooling_reag    = 15,  //�Լ����䵥Ԫ
    xc8002_dwnmach_name_ADOuter         = 16,  // AD��Ԫ��Ȧ
    xc8002_dwnmach_name_ADInner         = 17,  // AD��Ԫ��Ȧ
    xc8002_dwnmach_name_smpldisk        = 19,  //���涨λ
    xc8002_dwnmach_name_smplEmer        = 20,  //���ﶨλ
    xc8002_dwnmach_name_press           = 21,  //��ѹ��
    xc2000_dwnmach_name_Light           =22,   //��Դ��
    xc8002_dwnach_name_ALTspt           =30,  //�Զ�װ��ת��
    xc8002_dwnach_name_ALVG             =31,  //�Զ�װ�ش�ֱץ��
    xc8002_dwnach_name_ALL1             =32,  //�Զ�װ��L1��
    xc8002_dwnach_name_ALL2             =33,  //�Զ�װ��L2��
    xc8002_dwnmach_name_fpga_ad		  = 41,  //FPGA AD�ɼ���Ԫ
    xc8002_link_debug                 = 42,  //��·����
    
    xc8002_dwnmach_name_butt
}xc8002_dwnmach_name_enum;

//��Ϣ�������Ͷ���
typedef enum
{
    xc8002_req_msgtran_type = 0,  //������Ϣ����
    xc8002_ack_msgtran_type = 1,  //ȷ����Ϣ����
    xc8002_nak_msgtran_type = 2,  //����Ϣ����
    xc8002_res_msgtran_type = 3,  //��Ӧ��Ϣ����

    xc8002_msg_tran_type_butt
}xc8002_msg_tran_type_enum;
typedef char xc8002_msg_tran_type_enum_u8;

//����λ����Ϣ����ͷ
#define xc8002_middwnmach_msg_header \
    char  endwnmachname;   \
    char  enmsgtrantype;   \
    app_u16 uscmdinfolen;    \
    vos_u32 ulcurcmdframeid
    

#pragma warning(disable:4200)
//��λ����λ��ͨ�����ݴ���ṹ
typedef struct
{
    xc8002_middwnmach_msg_header;
    app_u16 uscmdtype;
    char  aucrsv[2];
    char aucdata[0];
}xc8002_middwnmach_com_msgtran_stru;

//��λ����λ����Ϣ�����Ӧ�������ݽṹ
typedef struct
{
    xc8002_middwnmach_msg_header;
    app_u16 uscmdtype;
    app_u16  enexerslt;      //����ִ�н��
    app_u32  rsv;
    char  aucinfo[0];
}xc8002_middwnmach_res_msgtran_stru;

//===================��Ϣ���ظ��ؽṹ=================
//����TLV�ṹ��
typedef struct
{
    char trayno;      //�̱��
    char cupno;       //�����
    app_u16 len;        //����
    char aucbarcode[0];   //������ !!!��λ����ر�֤4�ֽڶ���,��ͬʱ����lenֵ!!!
}app_dwn_barcode_tlv_stru;

//��λ������ṹ��
typedef struct
{
    app_u16 traytype;   //������
    app_u16 tlvnum;     //tlv����    
    app_dwn_barcode_tlv_stru auctlvinfo[0];  //tlv��Ϣ
}app_dwn_barcode_stru;
/**********end of ����ɨ�����ݽṹ��**********/

//��Ӧ���¶���Ϣ���أ�
typedef struct
{
    app_i32 slavertemp;    //ƽ���¶�ֵ   
    app_i32 slcurtemp1;    //ͨ��1�¶�ֵ 
    app_i32 slcurtemp2;    //ͨ��2�¶�ֵ 
    app_i32 slcurtemp3;    //ͨ��3�¶�ֵ 
    app_i32 slcurtemp4;    //ͨ��4�¶�ֵ(=ʵ���¶�*10��,��ֹ��ϵͳ��С������) 
}ReactTemp_st;

//��ϴ�¶���Ϣ���أ�
typedef struct
{
    app_i32 slmachine2;    //��������2�¶�ֵ 
    app_i32 slmachine1;    //��������1�¶�ֵ 
    app_i32 slcleaner;     //��ϴ���¶�ֵ 
    app_i32 slwater;      //��ϴˮ�¶�ֵ(=ʵ���¶�*10��,��ֹ��ϵͳ��С������) 
}WarterTemp_st;
#pragma warning(default:)

#endif
