//modify
#ifndef _app_xc8002shareinfo_h_
#define _app_xc8002shareinfo_h_

#include "vos_basictypedefine.h"
 
#define AutoTeskTask 33
#define NdlWashTask 46

#define Smp_Alarm_Wash_Pos 161  //样本清洗位统一编号位置
#define SMP_W_POS   26 //样本强化清洗杯位
#define SMP_D_POS   27 //样本强化清洗杯位
#define R1_WASH_CUP      50  //R1强化清洗杯位
#define R2_WASH_CUP      50  //R2强化清洗杯位

typedef enum{
	emergence_task = 1,   //急诊样本测试
	carlibrate_task = 2,   //校准测试
	qc_task = 3,   //质控测试
	review_task = 4,   //复查样本测试
	common_task = 5,   //普通样本测试
	oam_task = 6,	//维护测试
	pollution_task = 7,//交叉污染
	colormetry_task = 8,//比色法测试
    wash_task = 9,//清洗任务
    wheelset_task = 10, //轮空任务
}tasktype;

typedef enum
{
    app_xc8002_tray_type_sample = 1,    //样本盘
    app_xc8002_tray_type_reagent = 2,   //试剂盘

    app_xc8002_tray_type_butt
}app_xc8002_tray_type_enum;

//试剂盘编号定义
typedef enum
{
    app_xc8002_reagent_outer_disk_id = 1,  //外圈试剂盘
    app_xc8002_reagent_inner_disk_id = 2,  //内圈试剂盘
}app_xc8002_reagent_disk_id_enum;

//14路通道的号定义
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
    app_xc8002_wave_id_main = 1,  //主波长
    app_xc8002_wave_id_sub = 2,   //副波长
}app_xc8002_wave_id_enum;

#define MAX_BARCODE_LEN 24

typedef struct _app_maintain_console_req_stru
{
	char aucCommand[128];
}app_maintain_console_req_stru;

#endif
