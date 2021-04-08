#ifndef __APPUTILS_H__
#define __APPUTILS_H__


#include "app_innermsg.h"
#include "vos_timer.h"

//��λ��״̬����
//!!!ע��: ���ﶼ��ƽ̨�������λ��״̬��Ӧ�ò��ڶ����Լ���״̬ʱ����������ظ�!!!
//!!!�����������ζ��弰���ã�����ѯ�ƺ�!!!
typedef enum
{
    //���ӺͿ���̬
    vos_midmachine_state_standby = 1,  //����״̬

    //��������̬
    vos_midmachine_state_testing = 11,  //���ڲ���״̬
    vos_midmachine_state_wait_pause = 12,  //�ȴ���ͣ
    vos_midmachine_state_pause = 13, //��λ��������ͣ״̬

    //��������̬
    vos_midmachine_state_resetting = 21, //��λ����λ��
    vos_midmachine_state_rewarming = 22, //��λ��������
    vos_midmachine_state_intensify_wash = 23, //ǿ����ϴ��
    vos_midmachine_state_air_out = 24, //�ſ�����
    vos_midmachine_state_wash_rctcup = 25, //��Ӧ����ϴ��
    vos_midmachine_state_margin_scan = 26, //����ɨ�������
    vos_midmachine_state_bcscan = 27, //����ɨ�������
    vos_midmachine_state_darkcurrent = 28, //������������
    vos_midmachine_state_precise_add = 29, //��׼��Һ������
    vos_midmachine_state_dwncmd = 30,      //����ִ����
    vos_midmachine_state_light_check = 31, //��Ӧ����ϴ��
    vos_midmachine_state_cup_blank = 32, //��Ӧ����ϴ��

    
    //�쳣̬
    vos_midmachine_state_reseted_err = 51, //��λ����λʧ��
    vos_midmachine_state_rewarming_err = 52, //��λ������ʧ��
    vos_midmachine_state_intensify_wash_err = 53, //ǿ����ϴʧ��
    vos_midmachine_state_air_out_err = 54, //�ſ���ʧ��
    vos_midmachine_state_midmach_err = 55,  //��λ���쳣
    vos_midmachine_state_margin_scan_err = 56, //����ɨ�����ʧ��
    vos_midmachine_state_bcscan_err = 57, //����ɨ�����ʧ��
    vos_midmachine_state_darkcurrent_err = 58, //����������ʧ��
    vos_midmachine_state_precise_add_err = 59, //��׼��Һ����ʧ��
}vos_midmachine_state_enum;

//����ģ�鼶���ڴ�ӡ����λͼ
typedef enum
{
    ps_dwnCmd       = 0,
    ps_commonSys    = 1,
    ps_cmdCenter    = 2,
    ps_stateMach    = 3,
    ps_mnguartcomm  = 4,
    ps_oam          = 5,
    ps_reactsys     = 6,
    ps_reagentsys   = 7,
    ps_samplesys    = 8,
    ps_rscsch       = 9,
    ps_taskmng      = 10,
    ps_darkCurrent  = 11,
    ps_oammng       = 12,
    ps_addReag      = 13,
    ps_addSample    = 14,
	ps_uartLink		= 15,
	ps_appDrv		= 16,
    ps_schCup       = 17,
    ps_bcscan       = 18,
	ps_ADval        = 19,
	ps_singleCmd    = 20,
    ps_lightCheck   = 21,
    ps_reportData   = 22,
	ps_washCup      = 23,
	ps_compensation = 24,
    ps_dwnFault     = 25,
    ps_midFault     = 26,
	ps_crossWash    = 27,
    ps_maitain      = 28,
    ps_equipment    = 29,
    ps_cmdSch       = 30,
    ps_can          = 31,
    ps_autoload = 32,
    //��Ӧ�ó���ģ���ӡ����
    ps_ebiDrv = 33,
    ps_all = 34
}app_md_print_switch_enum;

const vos_u32 g_ResetTimeLen = 500;

//����p04
void OPEN_P04(vos_u32 srcpid);

//�ر�p04
void CLOSE_P04(vos_u32 srcpid);

//��ʼ��p04,��ֹg_P04Flag�����쳣����p04ʧ��
void INITP04(vos_u32 srcpid, vos_u16 pNum = Press_our);

//������λ
vos_u32 app_reqMachReset(vos_u32 ulSrcPid);

//��λ
vos_u32 app_reqReset(vos_u32 ulSrcPid, vos_u32 ulDstPid, vos_u32 ulRes);

//���ö���ͷ��Ϣ
void app_setActHead(msg_stActHead * pActHead, app_u16 actType, app_u16 cupId, app_u16 devType, app_u16 singRes);

//�ϱ���λ��״̬
void app_reportmidmachstatus(vos_u32 ulStatus, vos_u32 ulPid = -1, vos_u32 ulLeftTime = 0);

//������λ����һ����
vos_u32 app_reqDwmCmd(vos_u32 srcPid, vos_u32 dstPid, app_u16 actType, vos_u32 cmdLen, char* cmdPara, app_u16 singRes);

void app_setioctlvalue(vos_u32 ctrlkey, vos_u32 ctrlval);

void app_print(vos_u32 printswich, const char* msg, ...);

long getTime_ms();

void app_smpLightCtl(vos_u32 srcpid, char ctrl);

void app_adfpgapowerswitch(vos_u32 ulpowermode); //�򿪲�������

vos_u32 staticTestAD();//��̬���

vos_u32 dynamicTsetAD();//��̬���

vos_u32 SampMixWashCtrl(vos_u32 srcPid, vos_u32 on_off);

vos_u32 ReagMixWashCtrl(vos_u32 srcPid, vos_u32 on_off);

#ifndef Edition_A
vos_u32 MixWashCtrl(vos_u32 srcPid, vos_u32 dstPid, vos_u32 on_off);
#endif

void DiskPosToCup(vos_u32 dikPos, app_u16 & disk, app_u16 & cup);

void CupToDiskPos(app_u16 disk, app_u16 cup, vos_u32 &diskPos);
#endif
