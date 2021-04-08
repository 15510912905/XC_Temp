//modify
#ifndef _app_xc8002shareinfo_h_
#define _app_xc8002shareinfo_h_

#include "vos_basictypedefine.h"
 
#define AutoTeskTask 33
#define NdlWashTask 46

#define Smp_Alarm_Wash_Pos 161  //������ϴλͳһ���λ��
#define SMP_W_POS   26 //����ǿ����ϴ��λ
#define SMP_D_POS   27 //����ǿ����ϴ��λ
#define R1_WASH_CUP      50  //R1ǿ����ϴ��λ
#define R2_WASH_CUP      50  //R2ǿ����ϴ��λ

typedef enum{
	emergence_task = 1,   //������������
	carlibrate_task = 2,   //У׼����
	qc_task = 3,   //�ʿز���
	review_task = 4,   //������������
	common_task = 5,   //��ͨ��������
	oam_task = 6,	//ά������
	pollution_task = 7,//������Ⱦ
	colormetry_task = 8,//��ɫ������
    wash_task = 9,//��ϴ����
    wheelset_task = 10, //�ֿ�����
}tasktype;

typedef enum
{
    app_xc8002_tray_type_sample = 1,    //������
    app_xc8002_tray_type_reagent = 2,   //�Լ���

    app_xc8002_tray_type_butt
}app_xc8002_tray_type_enum;

//�Լ��̱�Ŷ���
typedef enum
{
    app_xc8002_reagent_outer_disk_id = 1,  //��Ȧ�Լ���
    app_xc8002_reagent_inner_disk_id = 2,  //��Ȧ�Լ���
}app_xc8002_reagent_disk_id_enum;

//14·ͨ���ĺŶ���
typedef enum
{	
    app_rscsch_wave_340nm_id = 340,
    app_rscsch_wave_405nm_id = 405,
    app_rscsch_wave_450nm_id = 450,
    app_rscsch_wave_478nm_id = 478,
    app_rscsch_wave_505nm_id = 505,
    app_rscsch_wave_542nm_id = 542,
    app_rscsch_wave_570nm_id = 570,
    app_rscsch_wave_605nm_id = 605,
    app_rscsch_wave_630nm_id = 630,
    app_rscsch_wave_660nm_id = 660,
    app_rscsch_wave_700nm_id = 700,
    app_rscsch_wave_750nm_id = 750,
    app_rscsch_wave_805nm_id = 805,
    app_rscsch_wave_850nm_id = 850,
    
    app_rscsch_wave_id_butt
}app_rscsch_wave_id_enum;

typedef enum
{
    app_xc8002_wave_id_main = 1,  //������
    app_xc8002_wave_id_sub = 2,   //������
}app_xc8002_wave_id_enum;

#define MAX_BARCODE_LEN 24

typedef struct _app_maintain_console_req_stru
{
	char aucCommand[128];
}app_maintain_console_req_stru;

#endif
