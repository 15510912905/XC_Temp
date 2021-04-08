#include "app_errorcode.h"
#include "schedule.h"
#include "vos_fid.h"
#include "app_maintain.h"
#include "app_simulation.h"
#include "app_debugtty.h"
#include "app_debugcmd.h"
#include "app_statusrepdispose.h"
#include "smpndl.h"
#include "smpdsk.h"
#include "r1sys.h"
#include "r2sys.h"
#include "autowash.h"
#include "airout.h"
#include "outerdsk.h"
#include "innerdsk.h"
#include "reactdsk.h"
#include "reagmix.h"
#include "smpmix.h"
#include "tempctrl.h"
#include "coolreag.h"
#include "marginscan.h"
#include "reset.h"
#include "inituart.h"
#include "uart.h"
#include "intensifywash.h"
#include "looptest.h"
#include "bcscan.h"
#include "dwmcmd.h"
#include "darkcurrent.h"
#include "preciseadd.h"
#include "alarm.h"
#include "vos_log.h"
#include "smooth.h"
#include "showave.h"
#include "updatedwn.h"
#include "taskresult.h"
#include "acquisAD.h"
#include "AdLight.h"
#include "light.h"

#include "transport.h"
#include "verticalgripper.h"
#include "L1sys.h"
#include "L2sys.h"
#include "autoload.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : app_registerprofidtovos
 ��������  : ��vosƽ̨ע���Ʒfid
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��4��10��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_registerprofidtovos(void)
{
    vos_u32 ulrunrslt = VOS_OK;

    WRITE_INFO_LOG("Register oam_fid \n");
    //ע���ƷOAMģ��FID
    ulrunrslt = vos_registerfid(vos_thread_pri_60, app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Register app_oam fid failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }

    WRITE_INFO_LOG("Register rscsch_fid \n");
    //ע���Ʒ��Դ����ģ��FID
    ulrunrslt = vos_registerfid(vos_thread_pri_80, app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Register resource schedule fid failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }
    
    //ע���Ʒ��Դ����ģ��FID
    ulrunrslt = vos_registerfid(vos_thread_pri_80, app_task_result_fid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Register resource schedule fid failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }

    WRITE_INFO_LOG("Register rctsys_fid \n");
    //ע���Ʒ��Ӧ�̹���ģ��FID
    ulrunrslt = vos_registerfid(vos_thread_pri_85, app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Register react_disk management fid failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }

    WRITE_INFO_LOG("Register dwnmachcmdcenter_fid \n");
    
    //ע���Ʒ��λ������λ�����������ģ��FID
    ulrunrslt = vos_registerfid(vos_thread_pri_90, app_dwnmach_cmd_center_fid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Register downmach cmd center management fid failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }
    
    ulrunrslt = vos_registerfid(vos_thread_pri_90, app_auto_test_fid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Register downmach cmd center management fid failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }
    
    return VOS_OK;
}

//��ʼ����ϵͳ
vos_u32 app_initSubsy()
{
    //�Զ���ϴģ��
    CAutoWash *pAutoWash = new CAutoWash(SPid_AutoWash, xc8002_dwnmach_name_autowash);
    vos_u32 ulrunrslt = pAutoWash->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //��ʼ����ϵͳģ��
    CReactDsk *pReactDsk = new CReactDsk(SPid_ReactDsk);
    ulrunrslt = pReactDsk->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //��ʼ����ϵͳģ��
    CReagMix *pReagMix = new CReagMix(SPid_R2Mix);
    ulrunrslt = pReagMix->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //��ʼ����ϵͳģ��
    CSmpMix *pSmpMix = new CSmpMix(SPid_SMix);
    ulrunrslt = pSmpMix->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //������ģ��
    CSmpNdl *pSmpNdl = new CSmpNdl(SPid_SmpNdl, xc8002_dwnmach_name_smpndl);
    ulrunrslt = pSmpNdl->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //����������λ��Ԫ
    CSmpDsk *pSmpDsk = new CSmpDsk(SPid_SmpDsk, xc8002_dwnmach_name_smpldisk);
    ulrunrslt = pSmpDsk->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //����������λ��Ԫ
    CSmpDsk *pSmpEmer = new CSmpDsk(SPid_SmpEmer, xc8002_dwnmach_name_smplEmer);
    ulrunrslt = pSmpEmer->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CR1Sys *pR1Sys = new CR1Sys(SPid_R1, xc8002_dwnmach_name_r1Ndl);
    ulrunrslt = pR1Sys->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    COuterDsk *pOuterDsk = new COuterDsk(SPid_R1Dsk);
    ulrunrslt = pOuterDsk->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CR2Sys *pR2Sys = new CR2Sys(SPid_R2, xc8002_dwnmach_name_r2Ndl);
    ulrunrslt = pR2Sys->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
	
    CInnerDsk *pInnerDsk = new CInnerDsk(SPid_R2Dsk);
    ulrunrslt = pInnerDsk->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CTempCtrl *pTempCtrl = new CTempCtrl(SPid_RTmpCtrl, xc8002_dwnmach_name_auto_Rtmpctrl);
    ulrunrslt = pTempCtrl->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CTempCtrl *pWTempCtrl = new CTempCtrl(SPid_WTmpCtrl, xc8002_dwnmach_name_auto_wtmpctrl);
    ulrunrslt = pWTempCtrl->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CCoolReag *pCoolReag = new CCoolReag(SPid_CoolReag, xc8002_dwnmach_name_cooling_reag);
    ulrunrslt = pCoolReag->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CR1Sys *pCoolReag12 = new CR1Sys(SPid_R1b, xc8002_dwnmach_name_r1bNdl);
    ulrunrslt = pCoolReag12->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    CR2Sys *pCoolReag13 = new CR2Sys(SPid_R2b, xc8002_dwnmach_name_r2bNdl);
    ulrunrslt = pCoolReag13->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    CSmpNdl *pCoolReag14 = new CSmpNdl(SPid_SmpNdlb, xc8002_dwnmach_name_smpndlb);
    ulrunrslt = pCoolReag14->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CAdLight *pAdLightOut = new CAdLight(SPid_ADOuter, xc8002_dwnmach_name_ADOuter);
    ulrunrslt = pAdLightOut->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CAdLight *pAdLightIn = new CAdLight(SPid_ADInner, xc8002_dwnmach_name_ADInner);
    ulrunrslt = pAdLightIn->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CAutoWash *pPress = new CAutoWash(SPid_Press, xc8002_dwnmach_name_press);
    ulrunrslt = pPress->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CTransport *pTransport = new CTransport(SPid_Transport, xc8002_dwnach_name_ALTspt);
    ulrunrslt = pTransport->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    CVerticalGripper *pVerGripper = new CVerticalGripper(SPid_VerticalGripper, xc8002_dwnach_name_ALVG);
    ulrunrslt = pVerGripper->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    CL1Sys *pL1sys = new CL1Sys(SPid_L1, xc8002_dwnach_name_ALL1);
    ulrunrslt = pL1sys->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    CL2Sys *pL2sys = new CL2Sys(SPid_L2, xc8002_dwnach_name_ALL2);
    ulrunrslt = pL2sys->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    CLight * pLight = new CLight(SPid_Light, xc2000_dwnmach_name_Light);
    ulrunrslt = pLight->RegCallBack(app_subsys_mng_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_productinit
 ��������  : ��Ʒ��λ��Ӧ�ó����ʼ���ܽӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��4��2��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_productinit(void)
{
    vos_u32 ulrunrslt = VOS_OK;

    WRITE_INFO_LOG("enter app_productinit \n");
    
    //ע���Ʒ����FID
    ulrunrslt = app_registerprofidtovos();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    WRITE_INFO_LOG("register fid ok \n");

    ulrunrslt = app_initDebugttymd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    WRITE_INFO_LOG("init debugtty ok \n");

    ulrunrslt = app_initdebugcmdmd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    WRITE_INFO_LOG("init debugcmd ok \n");
    
//     ulrunrslt = InitUart();
//     if (VOS_OK != ulrunrslt)
//     {
//         return ulrunrslt;
//     }
//     WRITE_INFO_LOG("init uart ok \n");

    //��ʼ����λ���ϱ�ģ��
	ulrunrslt = app_initstautsrepdispose();
	if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    WRITE_INFO_LOG("init statuesrpds mng ok \n");         
    CUart *pUart = new CUart(Pid_DwnMachUart);
    ulrunrslt = pUart->RegCallBack(app_dwnmach_cmd_center_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
	
    //��ʼ����ϵͳ
    ulrunrslt = app_initSubsy();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    WRITE_INFO_LOG("app_initSubsy ok \n");

    //��׼��Һģ��
    CPreciseAdd * pPreciseAdd = new CPreciseAdd(SPid_PreciseAdd);
    ulrunrslt = pPreciseAdd->RegCallBack(app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //��ʼ���ſ���ģ��
    CAirOut *pAirOut = new CAirOut(SPid_AirOut);
    ulrunrslt = pAirOut->RegCallBack(app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //������λģ��
    CReset *pReset = new CReset(SPid_Reset);
    ulrunrslt = pReset->RegCallBack(app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //ǿ����ϴģ��
    CIntensifyWash *pIntensifyWash = new CIntensifyWash(SPid_IntensifyWash);
    ulrunrslt = pIntensifyWash->RegCallBack(app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    //��ʼ����λ������ģ��
    CDwnCmd *pDwnCmd = new CDwnCmd(SPid_DwnCmd);
    ulrunrslt = pDwnCmd->RegCallBack(app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    //��ʼ����������ģ��
    ulrunrslt = app_initSimuTest(); 
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    WRITE_INFO_LOG("app_initSimuTest ok \n");

	//��ʼ���ػ�����ģ��
    CLoopTest *pLoopTest = new CLoopTest(Pid_LoopTest);
    ulrunrslt = pLoopTest->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CMarginScan *pMarginScan = new CMarginScan(SPid_MarginScan);
    ulrunrslt = pMarginScan->RegCallBack(app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CBcScan *pBcScan = new CBcScan(SPid_BcScan);
    ulrunrslt = pBcScan->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CDarkCurrent *pDarkCurrent = new CDarkCurrent(SPid_DarkCurrent);
    ulrunrslt = pDarkCurrent->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    g_pAlarm = new CAlarm(SPid_Alarm);
    ulrunrslt = g_pAlarm->RegCallBack(app_self_oam_fid);
    if (VOS_OK == ulrunrslt)
    {
        ulrunrslt = g_pAlarm->Initialize();
    }

	CSmooth *pSmooth = new CSmooth(SPid_Smooth);
    ulrunrslt = pSmooth->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
	
	CAcquisAD  *pAcquisAD = new CAcquisAD(SPid_ShowAD);
	ulrunrslt = pAcquisAD->RegCallBack(app_self_oam_fid);
	if (VOS_OK != ulrunrslt)
	{
		return ulrunrslt;
	}
	
    //��ʼ����Դ����ģ��
    CSchedule * pSchedule = new CSchedule();
    ulrunrslt = pSchedule->RegCallBack(app_resource_schedule_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CTaskResult * pTaskResult = new CTaskResult();
    ulrunrslt = pTaskResult->RegCallBack(app_task_result_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

	CShowave *pShowave = new CShowave(SPid_Showave);
    ulrunrslt = pShowave->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
	
	//��ʼ����λ������ģ��
    CUpdateMachine *pUpdateMachine = new CUpdateMachine(Spid_UpdateDwn);
    ulrunrslt = pUpdateMachine->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CMaintain *pMaintain = new CMaintain(SPid_Maintain);
    ulrunrslt = pMaintain->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    return ulrunrslt;
}

#ifdef __cplusplus
}
#endif


