/******************************************************************************

                  ��Ȩ���� (C), 2001-2100, �Ĵ�ʡ�½����ɹɷ����޹�˾

 ******************************************************************************
  �� �� ��   : app_statusrepdispose.c
  �� �� ��   : ����
  ��    ��   : zxc
  ��������   : 2014��8��27��
  ����޸�   :
  ��������   : ��λ�������ϱ�״̬�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��4��8��
    ��    ��   : zxc
    �޸�����   : �����ļ�

******************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "xc8002_middwnmachshare.h"
#include "app_xc8002shareinfo.h"
#include "app_errorcode.h"
#include "app_msgtypedefine.h"
#include "app_debugtty.h"
#include "app_statusrepdispose.h"
#include "app_msg.h"
#include "vos_mngmemory.h"
#include "vos_timer.h"
#include "vos_pid.h"
#include "reagent.h"
#include "upmsg.pb.h"
#include "apputils.h"
#include "socketmonitor.h"
#include "alarm.h"
#include "vos_log.h"
#include "showave.h"
#include "cmd_tempcontrol.h"
#include "reactcupmng.h"
#include "comstruct.pb.h"
#include "acthandler.h"
#ifdef __cplusplus
    extern "C" {
#endif
#define DETECTION_COLLIDE  (0xFFFD) //ײ����
#ifdef COMPILER_IS_LINUX_GCC
STATIC app_f64 g_slreactdsktemprature   = 37.0;
STATIC vos_u32 g_ulrctdsktempchgdir     = 0; //0:�¶��ϱ�,1:�¶��±�
STATIC vos_u32 g_ulreagdsktempchgdir    = 0; //0:�¶��ϱ�,1:�¶��±�
STATIC app_f64 g_slreagentdsktemprature = 3.0;
STATIC vos_u32 g_ultimeridfortemprature = APP_INVALID_U32;
#endif

STResShowSmooth stPara;
/*****************************************************************************
 �� �� ��  : app_remaindispose
 ��������  : �Լ������ϱ�������
 �������  : 
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��27��
    ��    ��   : zxc

    �޸�����   : �����ɺ���

*****************************************************************************/

vos_u32 app_remaindispose(xc8002_report_remaindepth_stru *remaininfo)
{
    //��װ���ݷ�����λ��APP  
    app_print(ps_reportData, "Report:dwnmach = %d \n", remaininfo->usmechdevid);
    app_print(ps_reportData, "Report:remain = %d \n", remaininfo->usremaindepth);
    
    if (xc8002_dwnmach_name_r1Ndl == remaininfo->usmechdevid || xc8002_dwnmach_name_r2Ndl == remaininfo->usmechdevid || xc8002_dwnmach_name_r1bNdl==remaininfo->usmechdevid)
    {
		vos_u32 depth = remaininfo->usremaindepth;
        //��λ����������
		if (VOS_OK == g_pReagent->ProcReagentMargin(remaininfo->cupinfo.diskid, remaininfo->cupinfo.cupid, depth))
        {
            app_print(ps_reportData, "SCHEDHLEP: update reagent disk(%d),cup(%d) remain =%d\n", remaininfo->cupinfo.diskid, remaininfo->cupinfo.cupid, remaininfo->usremaindepth);

            STResMarginScan stPara;
            STMarginScan * pInfo = stPara.add_stmargininfo();
            STCupPos * pPos = pInfo->mutable_stcuppos();
            pPos->set_uidskid(remaininfo->cupinfo.diskid);
            pPos->set_uicupid(remaininfo->cupinfo.cupid);
			pInfo->set_uimargindepth(depth);

            return sendUpMsg(CPid_Reagent, Pid_ReportStatus, MSG_ResMarginScan, &stPara);
        }
    }
    if (xc8002_dwnmach_name_smpndl == remaininfo->usmechdevid)
    {
        app_print(ps_reportData, "SCHEDHLEP: update sample disk(%d),cup(%d) remain =%d\n", remaininfo->cupinfo.diskid, remaininfo->cupinfo.cupid, remaininfo->usremaindepth);
        if (DETECTION_COLLIDE == remaininfo->usremaindepth)
        {
            app_print(ps_reportData, "UnEngh sample disk(%d),cup(%d)!\n", remaininfo->cupinfo.diskid, remaininfo->cupinfo.cupid);
            //��ϴλ��������
            vos_u32 ulCupPos;
            CupToDiskPos(remaininfo->cupinfo.diskid, remaininfo->cupinfo.cupid, ulCupPos);
            g_pReagent->AddSmpUnEngh(ulCupPos);

            if (cmd_sample_dsk_circle_4_id == remaininfo->cupinfo.diskid
                && (SMP_W_POS == remaininfo->cupinfo.cupid || SMP_D_POS == remaininfo->cupinfo.cupid))
            {
                g_pAlarm->GenerateAlarm(ALarm_Wash_Margin, xc8002_dwnmach_name_smpldisk, ulCupPos);
            }
            else if (0 != remaininfo->cupinfo.cupid)
            {         
                g_pAlarm->GenerateAlarm(ALarm_Smp_Margin, xc8002_dwnmach_name_smpndl, ulCupPos);
            }
        }
    }
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_rptspssystempratuetoupmach
 ��������  : �ϱ�ָ��ϵͳ�¶ȵ���λ��
 �������  : vos_u32 encommandid
             app_i32 slcurtemp
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��12��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_rptspssystempratuetoupmach(vos_u32 ulSubsys, app_i32 iTemp)
{
	STNotifyTemp stPara;
    stPara.set_ulsubsysid(ulSubsys);
    stPara.set_itemp(iTemp);

    return sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_NotifyTemp, &stPara);
}

/*****************************************************************************
 �� �� ��  : app_reportdwnfaulttofaultmngsys
 ��������  : ����λ���Ĺ����ϱ�����λ���Ĺ��Ϲ�������
 �������  : xc8002_fault_info_stru* pfault_info
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : wushuangxi

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_reportdwnfaulttofaultmngsys(xc8002_fault_info_stru *pfault_info)
{
    if (NULL == pfault_info)
    {
        WRITE_ERR_LOG("pfault_info is null.\n");
        return VOS_OK;
    }
	if (xc8002_fault_status_generate == pfault_info->chFaultStatus)
    {   
        if (g_pMonitor->m_ulState == vos_midmachine_state_rewarming && pfault_info->usFaultId == 11000)//��Ӧ���¶��쳣
        {
            return VOS_OK;
        }
		g_pAlarm->GenerateAlarm(pfault_info->usFaultId, pfault_info->chCpuId, 0);
    }
    else
    {
		g_pAlarm->RecoverAlarm(pfault_info->usFaultId, pfault_info->chCpuId, 0);
    }
    return VOS_OK;
}

#ifdef COMPILER_IS_LINUX_GCC
/*****************************************************************************
 �� �� ��  : app_settempraturereportbydisp
 ��������  : �ϱ�����ģ�����¶��ϱ�
 �������  : vos_u32 ulpara
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��12��17��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_settempraturereportbydisp(vos_u32 ulpara)
{
    vos_u32 ulrunrslt = VOS_OK;

    if (1 == ulpara)
    {
        ulrunrslt = vos_starttimer(g_ultimeridfortemprature);
        if (VOS_OK != ulrunrslt)
        {
            WRITE_INFO_LOG(" Oammng: call vos_starttimer() failed(0x%x) \n",
                ulrunrslt);
        }
    }
    else
    {
        ulrunrslt = vos_stoptimer(g_ultimeridfortemprature);
        if (VOS_OK != ulrunrslt)
        {
            WRITE_INFO_LOG(" Oammng: call vos_stoptimer() failed(0x%x) \n",
                ulrunrslt);
        }
    }
}

/*****************************************************************************
 �� �� ��  : app_simulatereporttempraturebyoam
 ��������  : OAMģ���¶��ϱ�
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��12��10��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_simulatereporttempraturebyoam(void)
{
    vos_u32 ulrunrslt = VOS_OK;
    app_i32 sltmpvar  = 0.0;

    if (0 == g_ulrctdsktempchgdir)
    {
        g_slreactdsktemprature += 0.1;
    }else
    {
        g_slreactdsktemprature -= 0.1;
    }

    if (0 == g_ulreagdsktempchgdir)
    {
        g_slreagentdsktemprature += 0.1;
    }else
    {
        g_slreagentdsktemprature -= 0.1;
    }

    if (37.1 <= g_slreactdsktemprature)
    {
        g_ulrctdsktempchgdir = 1;
    }else if (36.9 >= g_slreactdsktemprature)
    {
        g_ulrctdsktempchgdir = 0;
    }

    if (4.5 <= g_slreagentdsktemprature)
    {
        g_ulreagdsktempchgdir = 1;
    }else if (2.5 >= g_slreagentdsktemprature)
    {
        g_ulreagdsktempchgdir = 0;
    }

    sltmpvar  = (app_i32)(g_slreactdsktemprature * 10);
    ulrunrslt = app_rptspssystempratuetoupmach(SPid_ReactDsk, sltmpvar);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_INFO_LOG(" Rptdispose: call app_rptspssystempratuetoupmach() failed(0x%x) for react disk. \n",
                  ulrunrslt);

        return;
    }

    WRITE_INFO_LOG(" Rptdispose: reactdisk current temprature is %f \n",
              (app_f64)(((app_f64)sltmpvar) / 10));
    
    sltmpvar  = (app_i32)(g_slreagentdsktemprature * 10);
    ulrunrslt = app_rptspssystempratuetoupmach(SPid_R1Dsk, sltmpvar);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_INFO_LOG(" Rptdispose: call app_rptspssystempratuetoupmach() failed(0x%x) for reagent disk. \n",
                  ulrunrslt);

        return;
    }

    WRITE_INFO_LOG(" Rptdispose: reagentdisk current temprature is %f \n",
              (app_f64)(((app_f64)sltmpvar) / 10));
    
    return;
}
#endif

//����������ת�����ϱ�
vos_u32 app_procreagkeydowninnerdisk()
{
    return sendInnerMsg(SPid_Oammng, Pid_ReportStatus, app_indisc_rotate_req);
}

//����������ת�����ϱ�
vos_u32 app_procreagkeydownouterdisk()
{
    return sendInnerMsg(SPid_Oammng, Pid_ReportStatus, app_outdisc_rotate_req);
}

//��������ʾ������λ�������ϱ�
vos_u32 procShowaveReport(xc8002_com_report_cmd_stru * pWave)
{
	WRITE_INFO_LOG("Enter procShowaveReport.\r\n");
	WRITE_INFO_LOG("usstatustype = %d.\r\n",pWave->usstatustype);

	vos_u32 ulrunrslt = VOS_OK;
	xc8002_wave_smp_report_stru * pPara = VOS_NULL;
	

	ulrunrslt = app_mallocBuffer(sizeof(xc8002_wave_smp_report_stru),(void * *)&pPara);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	if (VOS_NULL != pWave)
	{
    	memcpy(pPara, pWave, sizeof(xc8002_wave_smp_report_stru));
	}
	
	msg_stHead rHead;
    app_constructMsgHead(&rHead, SPid_Showave, Pid_ReportStatus, app_report_msg_type);
    return app_sendMsg(&rHead, pPara, sizeof(xc8002_wave_smp_report_stru));
}

/*****************************************************************************
 �� �� ��  : app_procdwnversions
 ��������  : ������λ���汾��Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��14��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procdwnversions(cmd_fpga_dwnmach_version_stru* pstrepcmd)
{ 
    printf("dwnmachname     = %lu\n", pstrepcmd->dwnmachname);
    printf("date_time       = %s\n", pstrepcmd->date_time);
    printf("dwnmachin_ver   = %s\n", pstrepcmd->dwnmachin_ver);
    printf("fpga_ver        = 0x%x\n\n", (unsigned int)pstrepcmd->fpga_ver);

    STSubVers stSubVer;

    stSubVer.set_ulsubsysid(pstrepcmd->dwnmachname+900);
    stSubVer.set_strtime((const char*)(pstrepcmd->date_time));
    stSubVer.set_strversions((const char*)(pstrepcmd->dwnmachin_ver));
    stSubVer.set_fpgaver(pstrepcmd->fpga_ver);

    return sendInnerMsg(SPid_DwnCmd, Pid_ReportStatus, MSG_ResVres, &stSubVer);;
}

/*****************************************************************************
 �� �� ��  : app_procLightBulbAD
 ��������  : �����Դ�Ƶ�ѹADֵ
 �������  : ��
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��14��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procLightBulbAD(cmd_react_disk_lightgetval_stru *pstrepcmd)
{
    printf("AD_EEPROM = %d\n",pstrepcmd->get_eeprom_lightintensity);
    printf("AD_lamp   = %d\n",pstrepcmd->get_lamp_lightintensity);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_proccompensation(pstrepcmd)
 ��������  : �����������ϱ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��24��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_proccompensation(xc8002_com_report_cmd_stru *pstrepcmd)
{
    Report_PAGE_st *compensation = (Report_PAGE_st *)pstrepcmd;
    app_u32* para = compensation->datas;

    STNotifyOffset stPara;
    vos_u16 subsysId = 900 + compensation->dwnmachinetype;
    stPara.set_uisubsysid(subsysId);
    stPara.set_type(compensation->number);
    app_print(ps_reportData, "subsysId��%d\n", subsysId);
    app_print(ps_reportData, "number��%d \n", compensation->number);
    for(app_u16 loopi = 0; loopi < compensation->count; loopi++)
    {
        app_print(ps_reportData, "No.%d --%d\n", loopi + 1, *para);
        stPara.add_para((int)(*para));
        para++;
    } 
    return sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_NotifyOffset, &stPara);
}

vos_u32 app_procMotorSpeed(xc8002_com_report_cmd_stru *pstrepcmd)
{
    xc8002_report_dwnmachine_motorspeed *pMotorSpeed = (xc8002_report_dwnmachine_motorspeed *)pstrepcmd;
    xc8002_report_motor_para* para = pMotorSpeed->ppara;

    STSubSysMotor stPara;
    vos_u16 subsysId = 900 + pMotorSpeed->dwnmachinetype;
    stPara.set_uisysid(subsysId);

    for (app_u16 loopi = 0; loopi < pMotorSpeed->paranum; loopi++)
    {
        //�ϴ���ֵ
        STMotorPara * pPara = stPara.add_stpara();
        pPara->set_uidrvid(para->drvId);
        pPara->set_uilineid(para->lineId);
        pPara->set_uistartfre(para->startFre);
        pPara->set_uiendfre(para->endFre);
        pPara->set_uiechstep(para->echLaderStep);
        pPara->set_uimaxladder(para->upMaxLader);
        pPara->set_uispar(para->sPar);
        para++;
        app_print(ps_reportData, "%s\n", pPara->DebugString().c_str());
    }


    return sendInnerMsg(SPid_Oammng, Pid_ReportStatus, MSG_NotifyMotor, &stPara);
}

/*****************************************************************************
 �� �� ��  : app_procfanstatus
 ��������  : �������״̬�ϱ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��22��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procfanstatus(cmd_cooling_fan_status_stru* pstrepcmd)
{
//     vos_u32 ulrunrslt = VOS_OK;
//     vos_u32 infosize =0;
//     app_xc8002_entiredebug_fanstatus_res_stru *fanstatus = VOS_NULL;
     
    printf("cmd = %#x,dwnmachname = %lu, enstatustype = %d\n",pstrepcmd->cmd,pstrepcmd->dwnmachname,pstrepcmd->enstatustype);
    for(vos_u32 loopi = 0; loopi < 8; loopi++)
    {
        printf("fan_status[%lu]=%d\n",loopi,pstrepcmd->status[loopi]);   
    }

//     infosize = sizeof(app_xc8002_entiredebug_fanstatus_res_stru) + 8*sizeof(app_xc8002_entiredebug_fanstatus_stru);
//     ulrunrslt = vos_mallocmemory(infosize, vos_mem_type_static,(void**)(&fanstatus));
//     if(VOS_OK != ulrunrslt)
//     {
//         WRITE_ERR_LOG("Call vos_mallocmemory() failed(0x%x) \n", ulrunrslt);
//         return ulrunrslt;
//     }
//     
//     fanstatus->ufancount = 8;
//     for(loopi = 0; loopi < 8; loopi++)
//     {
//         fanstatus->stfanarray[loopi].ufanno = loopi;
//         fanstatus->stfanarray[loopi].ufanstatus= pstrepcmd->status[loopi];
//     }
// 
// //     app_sendreportresult2upmach(app_entiredebug_thermotechnical_fanstatusquery_answer, 
// //                                 infosize,fanstatus);
//     DELETE_MEMEORY(fanstatus);
    return 0;
}

/*****************************************************************************
 �� �� ��  : app_proccoolelectricity
 ��������  : �������䵥Ԫ������ѹֵ
 �������  : ��
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��22��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_proccoolelectricity(cmd_cooling_vol_stru* pstrepcmd)
{
	printf("cmd = %#x,dwnmachname = %lu, enstatustype = %d\n", pstrepcmd->cmd, pstrepcmd->dwnmachname, pstrepcmd->enstatustype);
	CTemperatureQuery CTemperature;
	STKeyValue* tempValue;

	for (vos_u32 loopi = 0; loopi < 16; loopi++)
	{
		tempValue = CTemperature.add_temperature();
		tempValue->set_uikey(10 + loopi);
		tempValue->set_uival(pstrepcmd->vol[loopi]);
		printf("cooling_vol[%lu]=%lu\n", loopi, pstrepcmd->vol[loopi]);
	}
	return sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_TempeQuery, &CTemperature);
}


/*****************************************************************************
 �� �� ��  : app_procautowashstatus
 ��������  : �����Զ���ϴ�ϱ��ĸ��Ӽ�ѹ��ֵ
 �������  : cmd_autowash_status_stru* strepcmd
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��22��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procautowashstatus( cmd_autowash_status_stru* pstrepcmd )
{
    printf("cmd = %#x,dwnmachname = %lu, enstatustype = %d\n",pstrepcmd->cmd,pstrepcmd->dwnmachname,pstrepcmd->enstatustype);
    printf("shui xiang         = %d(1 kong; 0 man; 2 zheng chang;)\n",pstrepcmd->fl_status[0]);
    printf("nong suo qing xi ji= %d(1 kong; 0 zheng chang;)\n",pstrepcmd->fl_status[1]);
    printf("xi shi qing xi ji  = %d(1 kong; 0 man; 2 zheng chang;)\n",pstrepcmd->fl_status[2]);
    printf("wai zhi fei ye     = %d(1 zheng chang; 0 man;)\n",pstrepcmd->fl_status[3]);
    printf("gao nong du fei ye = %d(1 zheng chang; 0 man;)\n",pstrepcmd->fl_status[4]);
    printf("di  nong du fei ye = %d(1 zheng chang; 0 man;)\n",pstrepcmd->fl_status[5]);
    printf("zheng kong rong qi = %d(1 zheng chang; 0 man;)\n",pstrepcmd->fl_status[6]);

    printf("tuo qi = %d\n",pstrepcmd->pres_ad[0]);
    printf("ISE    = %d\n",pstrepcmd->pres_ad[1]);
    printf("zheng kong rong qi = %d\n",pstrepcmd->pres_ad[2]);
    
    STFLxState flxState;
    flxState.add_uival(pstrepcmd->fl_status[0]);
    flxState.add_uival(pstrepcmd->fl_status[1]);
    flxState.add_uival(pstrepcmd->fl_status[2]);
    flxState.add_uival(pstrepcmd->fl_status[3]);
    flxState.add_uival(pstrepcmd->fl_status[4]);
    flxState.add_uival(pstrepcmd->fl_status[5]);
    flxState.add_uival(pstrepcmd->fl_status[6]);

    flxState.add_uival(pstrepcmd->pres_ad[0]);
    flxState.add_uival(pstrepcmd->pres_ad[1]);
    flxState.add_uival(pstrepcmd->pres_ad[2]);
	return sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_FLXQuery, &flxState);
}

/*****************************************************************************
 �� �� ��  : app_proc
 ��������  : ������λ������ģ������¶��ϱ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��22��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procreagdsktemp(cmd_cooling_temp_stru* pstrepcmd)
{
    printf("cmd = %#x,dwnmachname = %lu, enstatustype = %d\n",
        pstrepcmd->cmd,pstrepcmd->dwnmachname,pstrepcmd->enstatustype);

    printf("temp_board = %f,temp_liqiud = %f\n",
        pstrepcmd->temp_board/100.0, pstrepcmd->temp_liqiud/100.0);

    CTemperatureQuery CTemperature;
    STKeyValue* tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(4);
    tempValue->set_uival(pstrepcmd->temp_board);
    tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(5);
    tempValue->set_uival(pstrepcmd->temp_liqiud);

	return sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_TempeQuery, &CTemperature);
}

/*****************************************************************************
 �� �� ��  : app_procthermaltemp
 ��������  : �����¿�ģ���ϱ����¶ȼ�ռ�ձ�
 �������  : app_procthermaltemp((cmd_temp_control_report_stru*)pstrepcmd)
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��22��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procthermaltemp(cmd_temp_control_report_stru* pstrepcmd)
{
    printf("cmd = %#x,dwnmachname = %lu, enstatustype = %d\n",pstrepcmd->cmd,pstrepcmd->dwnmachname,pstrepcmd->enstatustype);
    printf("temp_reac1 = %ld \n",pstrepcmd->temp_reac1);                
    printf("temp_reac2 = %ld \n",pstrepcmd->temp_reac2);
    printf("temp_reac3 = %ld \n",pstrepcmd->temp_reac3);
    printf("temp_env   = %ld \n",pstrepcmd->temp_env);
    printf("temp_washmach = %ld \n",pstrepcmd->temp_washmach);
    printf("temp_washcup  = %ld \n",pstrepcmd->temp_washcup);
    printf("temp_detergentcup = %ld \n",pstrepcmd->temp_detergentcup);

    CTemperatureQuery CTemperature;
    STKeyValue* tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(1);
    tempValue->set_uival(pstrepcmd->temp_reac1);
    tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(2);
    tempValue->set_uival(pstrepcmd->temp_reac2);
    tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(3);
    tempValue->set_uival(pstrepcmd->temp_reac3);
    tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(6);
    tempValue->set_uival(pstrepcmd->temp_washmach);
    tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(7);
    tempValue->set_uival(pstrepcmd->temp_washcup);
    tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(8);
    tempValue->set_uival(pstrepcmd->temp_detergentcup);
    tempValue = CTemperature.add_temperature();
    tempValue->set_uikey(9);
    tempValue->set_uival(pstrepcmd->temp_env);

	return sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_TempeQuery, &CTemperature);
}



/*****************************************************************************
�� �� ��  : app_procthermaltemp
��������  : �����¿�ģ���ϱ����¶ȼ�ռ�ձ�
�������  : app_procthermaltemp((cmd_temp_control_report_stru*)pstrepcmd)
�������  : ��
�� �� ֵ  : vos_u32
���ú���  :
��������  :

�޸���ʷ      :
1.��    ��   : 2015��7��22��
��    ��   : zxc
�޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_proctempad(cmd_temp_control_report_stru* pstrepcmd)
{
	printf("cmd = %#x,dwnmachname = %lu, enstatustype = %d\n", pstrepcmd->cmd, pstrepcmd->dwnmachname, pstrepcmd->enstatustype);
	printf("adtemp_reac1 = %ld \n", pstrepcmd->temp_reac1);
	printf("adtemp_reac2 = %ld \n", pstrepcmd->temp_reac2);
	printf("adtemp_reac3 = %ld \n", pstrepcmd->temp_reac3);
	printf("adtemp_env   = %ld \n", pstrepcmd->temp_env);
	printf("adtemp_washmach = %ld \n", pstrepcmd->temp_washmach);
	printf("adtemp_washcup  = %ld \n", pstrepcmd->temp_washcup);
	printf("adtemp_detergentcup = %ld \n", pstrepcmd->temp_detergentcup);

	CTemperatureQuery CTemperature;
	STKeyValue* tempValue = CTemperature.add_temperature();
	tempValue->set_uikey(1);
	tempValue->set_uival(pstrepcmd->temp_reac1);
	tempValue = CTemperature.add_temperature();
	tempValue->set_uikey(2);
	tempValue->set_uival(pstrepcmd->temp_reac2);
	tempValue = CTemperature.add_temperature();
	tempValue->set_uikey(3);
	tempValue->set_uival(pstrepcmd->temp_reac3);
	tempValue = CTemperature.add_temperature();
	tempValue->set_uikey(6);
	tempValue->set_uival(pstrepcmd->temp_washmach);
	tempValue = CTemperature.add_temperature();
	tempValue->set_uikey(7);
	tempValue->set_uival(pstrepcmd->temp_washcup);
	tempValue = CTemperature.add_temperature();
	tempValue->set_uikey(8);
	tempValue->set_uival(pstrepcmd->temp_detergentcup);
	tempValue = CTemperature.add_temperature();
	tempValue->set_uikey(9);

	tempValue->set_uival(pstrepcmd->temp_env);


	return sendUpMsg(CPid_OffSet, Pid_ReportStatus, MSG_ResShowAD, &CTemperature);
}
vos_u32 app_procclotairdata(ClotAirDataReport_st* pstrepcmd)
{
	app_print(ps_reportData, "ClotValue = %d\n", pstrepcmd->ClotValue);
	app_print(ps_reportData, "AirMax    = %d\n", pstrepcmd->AirMax);
	app_print(ps_reportData, "AirSum    = %d\n", pstrepcmd->AirSum);
	app_print(ps_reportData, "WashMax   = %d\n", pstrepcmd->WashMax);
	return VOS_OK;
}
/*****************************************************************************
 �� �� ��  : app_proceeprominfo(pstrepcmd)
 ��������  : ��λ��EEPROMֵ����
 �������  : ��
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��7��24��
    ��    ��   : zxc
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_proceeprominfo(xc8002_com_report_cmd_stru *pstrepcmd)
{
    app_u16 dwnmachinetype = (app_u16)pstrepcmd->aucloadinfo[0];

    printf("app_proceeprominfo dwnmachinetype = %d \n", dwnmachinetype);
    switch (dwnmachinetype)
    {
    case xc8002_dwnmach_name_cooling_reag:
    {
        cmd_temp_cooling_paraset_stru *eeprominfo = (cmd_temp_cooling_paraset_stru *)pstrepcmd;
        printf("close_temp          = %d \n", eeprominfo->close_temp);
        printf("open_temp           = %d \n", eeprominfo->open_temp);
        printf("LoopPumpOpen_temp   = %d \n", eeprominfo->LoopPumpOpen_temp);

        break;
    }
    case xc8002_dwnmach_name_auto_Rtmpctrl:
    {
        cmd_temp_control_eeprom_info_stru *eeprominfo = (cmd_temp_control_eeprom_info_stru *)pstrepcmd;
        printf("tempreact = %d \n", eeprominfo->tempreact);
        printf("delta = %d \n", eeprominfo->delta);
        printf("tempwash1 = %d \n", eeprominfo->tempwash1);
        printf("tempwash2 = %d \n", eeprominfo->tempwash2);
        printf("tempwash3 = %d \n", eeprominfo->tempwash3);
        STTempPara stPata;
        stPata.set_tempreact(eeprominfo->tempreact);
        stPata.set_delta(eeprominfo->delta);
        stPata.set_tempwash1(eeprominfo->tempwash1);
        stPata.set_tempwash2(eeprominfo->tempwash2);
        stPata.set_tempwash3(eeprominfo->tempwash3);
        sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_TempPara, &stPata);
    }
        break;
    default:
        break;
    }
    return 0;
}

/*****************************************************************************
 �� �� ��  : app_procbbloginfo
 ��������  : ����EEPROM��־
 �������  : xc8002_com_report_cmd_stru *pstrepcmd  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��8��20��
    ��    ��   : �ƺ�
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procbbloginfo(xc8002_com_report_cmd_stru *pstrepcmd)
{
    app_u16 i = 0;
    xc8002_report_dwnmachine_bblog *bblog = (xc8002_report_dwnmachine_bblog *)pstrepcmd;
    
    printf("recordnum = %d \n", bblog->recordnum);
    printf("currpage = %d \n", bblog->currpage);
    for (i = 0; i < bblog->recordnum; i++)
    {
        //��ʾ������Ҫд����־ҳ����*ǰһҳ��ʾ���һ����־
        if (bblog->currpage == i+33)
        {
            printf("*");
        }
        printf("[%d] %s \n", i+33, bblog->records[i].record);
    }
    
    return 0;
}

vos_u32 app_procPidStade(cmd_temp_pid_stru* pstrepcmd)
{
	printf("PID:state(%d),p(%.2f),i(%.2f),d(%0.2f) tmp(%.2f)!\n", pstrepcmd->state, pstrepcmd->kp, pstrepcmd->ki, pstrepcmd->kd, pstrepcmd->dsttmp);
	//WRITE_INFO_LOG("PID:state(%d),p(%.2f),i(%.2f),d(%0.2f) tmp(%.2f)!\n", pstrepcmd->state, pstrepcmd->kp, pstrepcmd->ki, pstrepcmd->kd, pstrepcmd->dsttmp);
	
	return VOS_OK;
}

void _procShowSmooth(xc8002_com_report_cmd_stru * pstrepcmd)
{
    Report_ShowOP_st*pPara = (Report_ShowOP_st*)pstrepcmd;
    AD_OP_TYPE *pValue = pPara->value;
    stPara.Clear();
    stPara.set_cirle((pPara->dwnmachinetype == xc8002_dwnmach_name_ADOuter) ? Circle_Outer : Circle_Inner);
    app_print(ps_reportData, "\n");
    for (int i = 0; i < 2000; i++)
    {
        adOP *pAD = stPara.add_value();
        pAD->set_ad(pValue->ad);
        pAD->set_op(pValue->op*25000);
        app_print(ps_reportData, "down[%d] op = %d,ad = %d\n", pPara->dwnmachinetype, pValue->op, pValue->ad);
        pValue++;
    }
    sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_ResShowSmooth, &stPara);
    pValue = NULL;
}

/*****************************************************************************
 �� �� ��  : app_stautsrepdispose
 ��������  : ʱ��״̬�ϱ�������
 �������  : 
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��27��
    ��    ��   : zxc

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_stautsrepdispose(xc8002_com_report_cmd_stru *pstrepcmd)
{
	WRITE_INFO_LOG("Enter app_stautsrepdispose.\r\n");
	WRITE_INFO_LOG("usstatustype = %d.\r\n",pstrepcmd->usstatustype);
    vos_u32 ulrunrslt = VOS_OK;
    
    xc8002_report_temprature_status_stru* pstrpttemp = VOS_NULL;
    xc8002_fault_report_stru* pstrfault = VOS_NULL;
	STNotifyTemp stPara;
    switch(pstrepcmd->usstatustype)
    {
        case xc8002_dwnmach_report_remain_type:
            ulrunrslt = app_remaindispose((xc8002_report_remaindepth_stru*)pstrepcmd);
            break;
            
        case xc8002_dwnmach_report_rctdsk_temp_type:
            pstrpttemp = (xc8002_report_temprature_status_stru*)pstrepcmd;
			
			stPara.set_ulsubsysid(SPid_ReactDsk);
			stPara.set_itemp(pstrpttemp->slcurtemp);

			sendInnerMsg(SPid_Oammng, SPid_ReactDsk, MSG_NotifyTemp, &stPara);
			ulrunrslt = sendUpMsg(CPid_Maintain, Pid_ReportStatus, MSG_NotifyTemp, &stPara);
            break;

        case xc8002_dwnmach_report_reagdsk_temp_type:
            pstrpttemp = (xc8002_report_temprature_status_stru*)pstrepcmd;
            
            ulrunrslt = app_rptspssystempratuetoupmach(SPid_R1Dsk, pstrpttemp->slcurtemp);
            break;

        case xc8002_dwnmach_report_fault_type:
        	pstrfault = (xc8002_fault_report_stru*)pstrepcmd;
            
            ulrunrslt = app_reportdwnfaulttofaultmngsys(&(pstrfault->fault_info));
            break; 
 
        case xc8002_dwnmach_report_close_reagcabin_type:
            break;
        case xc8002_dwnmach_report_open_reagcabin_type:
            break;
		case xc8002_dwnmach_report_keydown_innerdisk_type:
            ulrunrslt = app_procreagkeydowninnerdisk();
            break;
		case xc8002_dwnmach_report_keydown_outerdisk_type:
            ulrunrslt = app_procreagkeydownouterdisk();
            break;
		case xc8002_dwnmach_report_innerdisk_wave0_type:
		case xc8002_dwnmach_report_innerdisk_wave1_type:
		case xc8002_dwnmach_report_outerdisk_wave0_type:
		case xc8002_dwnmach_report_smpdisk_wave0_type:
            ulrunrslt = procShowaveReport(pstrepcmd);
            break;
        //---------------�豸���Բ�ѯ-----------------
        case xc8002_dwnmach_report_downmachin_versions:
            ulrunrslt = app_procdwnversions((cmd_fpga_dwnmach_version_stru*)pstrepcmd);
            break;
        case xc8002_dwnmach_report_LightBulbAD_type:
            ulrunrslt = app_procLightBulbAD((cmd_react_disk_lightgetval_stru*)pstrepcmd);
            break;
            
        case xc8002_dwnmach_report_compensation_type:
            ulrunrslt = app_proccompensation(pstrepcmd);
            break;
        case xc8002_dwnmach_report_motorline_type:
            ulrunrslt = app_procMotorSpeed(pstrepcmd);
            break;
        case xc8002_dwnmach_report_fanstatus_type:  
            ulrunrslt = app_procfanstatus((cmd_cooling_fan_status_stru*)pstrepcmd);
            break;
            
        case xc8002_dwnmach_report_electricity_type: 
            ulrunrslt = app_proccoolelectricity((cmd_cooling_vol_stru*)pstrepcmd);
            break;
            
        case xc8002_dwnmach_report_fl_and_pre_type:  
            ulrunrslt = app_procautowashstatus((cmd_autowash_status_stru*)pstrepcmd);
            break;
            
        case xc8002_dwnmach_report_reagdsk_temp_query_type:
            ulrunrslt = app_procreagdsktemp((cmd_cooling_temp_stru*)pstrepcmd);
            break;

        case xc8002_dwnmach_report_thermal_control_temp_type:
			ulrunrslt = app_procthermaltemp((cmd_temp_control_report_stru*)pstrepcmd);
			break;	
		case xc8002_dwnmach_report_ad_temp_type:
			ulrunrslt = app_proctempad((cmd_temp_control_report_stru*)pstrepcmd);
            break;
        case xc8002_dwnmach_report_eeprom_info_type:
            ulrunrslt = app_proceeprominfo(pstrepcmd);
            break;
        case xc8002_dwnmach_report_bblog_info_type:
            ulrunrslt = app_procbbloginfo(pstrepcmd);
            break;
		case xc8002_dwnmach_report_clot_air_type:
			ulrunrslt = app_procclotairdata((ClotAirDataReport_st*)pstrepcmd);
			break;
		case xc8002_dwnmach_report_pidstade:
			ulrunrslt = app_procPidStade((cmd_temp_pid_stru*)pstrepcmd);
			break;
        case  xc8001_dwnmach_report_ID:
        {
            cmd_CPUID *pMsg = (cmd_CPUID*)pstrepcmd;
            app_print(ps_reportData, "ID[%d] = %x %x %x %x\n", pMsg->dwnmachname, pMsg->id[0], pMsg->id[1], pMsg->id[2], pMsg->id[3]);
            STCupID para;
            para.add_id(pMsg->dwnmachname);
            para.add_id(pMsg->id[0]);
            para.add_id(pMsg->id[1]);
            para.add_id(pMsg->id[2]);
            para.add_id(pMsg->id[3]);
            sendUpMsg(CPid_Maintain, SPid_DwnCmd, MSG_CPUID, &para);
            break;
        }
        case xc8002_dwnmach_report_GwayState:
        {
            GwayState_st *pMsg = (GwayState_st*)pstrepcmd;
            STNotifState stPara;
            stPara.set_enunit(Unit_A1);
            stPara.set_ulloca(AL_NormC);
            stPara.set_ulstate(GState_Idle);
            stPara.set_ulshelfnum(0);
            app_print(ps_reportData, "AL_NormC is Idle\n");
#ifndef Version_A			
            sendUpMsg(CPid_Relation, Pid_ReportStatus, MSG_NotifyState, &stPara);
#endif
            break;
        }
        case dwnmach_show_wave:
        {
            _procShowSmooth(pstrepcmd);

            break;
        }
        default:               
            WRITE_ERR_LOG("Unknown report status(%u) \n", pstrepcmd->usstatustype);
            ulrunrslt = GENERRNO(app_report_dispose_errorcode_base,app_unknown_rpt_status_type_err);
            break;
    }

    return ulrunrslt;
}

/*****************************************************************************
 �� �� ��  : app_proctimeroutmsgbydispose
 ��������  : ��λ���ϱ���Ϣ����ģ�鴦��ʱ����ʱ��Ϣ
 �������  : vos_timeroutmsg_stru* pstmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��12��10��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_proctimeroutmsgbydispose(vos_timeroutmsg_stru* pstmsg)
{
    #ifdef COMPILER_IS_LINUX_GCC
    if (g_ultimeridfortemprature == pstmsg->ultimerid)
    {
        app_simulatereporttempraturebyoam();
    }
    xc8002_report_remaindepth_stru remaininfo = {
            0,0,xc8002_dwnmach_name_r1Ndl,59,
            {1,10,20}};
    app_remaindispose(&remaininfo);
    #endif
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_stautsrepdisposecallback
 ��������  : ״̬�ϱ�����ģ��ص�
 �������  : vos_msg_header_stru *pstmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��27��
    ��    ��   : zxc

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_stautsrepdisposecallback(vos_msg_header_stru *pstmsg)
{
    vos_u32 ulrunrslt = VOS_OK;
    if (VOS_NULL == pstmsg)
    {
       WRITE_ERR_LOG("Input parameter is null \n");

       RETURNERRNO(app_maintain_errorcode_base,app_para_null_err);
    }
    
    switch (pstmsg->usmsgtype)
    {
        //��λ��״̬�ϱ���Ϣ
        case app_dwnmachin_stautsrep_msg_type:
        
            ulrunrslt = app_stautsrepdispose((xc8002_com_report_cmd_stru*)&(pstmsg->aucmsgload));
            break;

        case vos_pidmsgtype_timer_timeout:
            ulrunrslt = app_proctimeroutmsgbydispose((vos_timeroutmsg_stru*)pstmsg);
            break;
        
        default:
            WRITE_ERR_LOG("unknown msg type(0x%x)(pid:%d->%d\n", pstmsg->usmsgtype, pstmsg->ulsrcpid, pstmsg->uldstpid);                       
            ulrunrslt = GENERRNO(app_resource_schedule_errorcode_base,app_unknown_msg_type_err);
            break;
    }

    return ulrunrslt;
}

/*****************************************************************************
 �� �� ��  : app_initstautsrepdispose
 ��������  : ��ʼ��״̬�ϱ�����ģ��
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��18��
    ��    ��   : zxc

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_initstautsrepdispose(void)
{
    vos_u32 ulrunrslt = VOS_OK;
    ulrunrslt = vos_registerpid(app_self_oam_fid, Pid_ReportStatus, app_stautsrepdisposecallback);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("register Pid_OamMaintain management pid failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }


#ifdef COMPILER_IS_LINUX_GCC
    //����������ʱ��ʱ��
    g_ultimeridfortemprature = createTimer(Pid_ReportStatus,"tempraturedemotimer",20,vos_timer_type_loop);
#endif

    return VOS_OK;
}

#ifdef __cplusplus
    }
#endif

