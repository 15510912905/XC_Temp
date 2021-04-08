#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "vos_pid.h"
#include "app_msgtypedefine.h"
#include "app_errorcode.h"

#include "app_debugtty.h"
#include "xc8002_middwnmachshare.h"
#include "app_testcase.h"
#include "app_msg.h"
#include "vos_log.h"
#include "app_simulation.h"
#include "cmd_downmachine.h"
#include "vos_log.h"
#include "app_xc8002shareinfo.h"
#include "typedefine.pb.h"
#include "app_downmsg.h"
#include "apputils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


//注册命令类型与响应函数
app_simuResFc_mng_stru g_simuResFc[] =
{
    {cmd_r1_reagent_ndl_marginscan,         app_simuReagMargScan},
    {cmd_r2_reagent_ndl_marginscan,         app_simuReagMargScan},
    {cmd_outer_reagentdisk_rtt_to_bcdscan,  app_simuReagBcdScan},
    {cmd_inner_reagentdisk_rtt_to_bcdscan,  app_simuReagBcdScan},
    {cmd_sample_dsk_rtt_to_bcdscan,         app_simuSampBcdScan},
    {XC8002_COM_DWNMACH_COMB_CMD,           app_simuDwnmachComCmdRes},
    {cmd_r1_reagent_ndl_compensation,       app_simuSetComp},
    {cmd_r2_reagent_ndl_compensation,       app_simuSetComp},
    {cmd_sample_dsk_compensation,           app_simuSetComp},
    {cmd_sample_mix_ndl_compensation,       app_simuSetComp},
    {cmd_sample_ndl_compensation,           app_simuSetComp},
    {cmd_inner_reagentdisk_compensation,    app_simuSetComp},
    {cmd_outer_reagentdisk_compensation,    app_simuSetComp},
    {cmd_react_disk_compensation,           app_simuSetComp},
    {cmd_reagent_mix_compensation,          app_simuSetComp},
    {cmd_r1_reagent_ndl_vdecline,           app_simuNdlDown},
    {cmd_r2_reagent_ndl_vdecline,           app_simuNdlDown},
    {cmd_sample_ndl_vdecline,               app_simuNdlDown},
    {CMD_READ_AD ,                          app_simuReadAD},
    { CMD_READ_ONE_AD,                      app_simuReadAD },
    { CMD_NDL_UP,                           app_simuNdlDown },
    { XC8002_COM_UPDATEDWN_CMD,             app_simuDwnUpdate},
    {cmd_outer_reagentdisk_fpga_update,     app_simuFpgaUpdate},
    {cmd_sample_dsk_fpga_update,            app_simuFpgaUpdate},
    {cmd_reagent_mix_fpga_update,           app_simuFpgaUpdate},
    { CMD_READ_TEMPERATURE,                 app_simuTempQuery},
    {CMD_NDL_DOWN,                          app_simuRNdlDown}
};

//命令类型与异常构造函数
app_faultFc_mng_stru g_faultFc[] = 
{
    {INVALID_CMDTYPE, 0, INVALID_FAULTID, VOS_NULL,VOS_NULL},
    {INVALID_CMDTYPE, 0, INVALID_FAULTID, VOS_NULL,VOS_NULL},
    {INVALID_CMDTYPE, 0, INVALID_FAULTID, VOS_NULL,VOS_NULL},
    {INVALID_CMDTYPE, 0, INVALID_FAULTID, VOS_NULL,VOS_NULL},
    {INVALID_CMDTYPE, 0, INVALID_FAULTID, VOS_NULL,VOS_NULL},
};

//可注册的
typedef enum
{
    app_faultFc_1   = 0,
    app_faultFc_2   = 1,
    app_faultFc_3   = 2,
    app_faultFc_4   = 3,
    app_faultFc_5   = 4,
    
    app_faultFc_butt
}app_variable_faultFc_enum;

app_auto_compensation_req_stru *g_pcpsautoreq = VOS_NULL;   //补偿文件分配内存指针

sim_stCondMutex stCondMutex = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER };

vos_u32 app_simuReport(msg_stMidDwnCom* pstLoad, app_u32 ulmemsize, char* pReport)
{
    vos_u32 ulrunrslt = VOS_OK;
    app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = (app_mnguart_middwnmach_cmd_res_msg_stru*)pReport;
    reportmsg->uldstpid = Pid_DwnMachUart;
    reportmsg->ulsrcpid = Pid_DwnMachUart;
    reportmsg->usmsgtype = app_res_dwnmach_act_cmd_msg_type;
    reportmsg->usmsgloadlen = ulmemsize - sizeof(vos_msg_header_stru);
    reportmsg->stresinfo.enmsgtrantype = xc8002_req_msgtran_type;
    reportmsg->stresinfo.ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    reportmsg->stresinfo.endwnmachname = pstLoad->endwnmachname;
    reportmsg->stresinfo.uscmdinfolen = ulmemsize - (sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) - 4);
    return vos_sendmsg((vos_msg_header_stru*)reportmsg);
}

void constructResPara(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd, app_u32 ulParaLen, char *pPara)
{
    vos_u32 ulrunrslt = VOS_OK;

    msg_stMidRes* pRes = VOS_NULL;
    app_u32 msgLen = ulParaLen + sizeof(msg_stMidRes);
    ulrunrslt = app_mallocBuffer(msgLen, (void**)(&pRes));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)", ulrunrslt);
        return;
    }
    pRes->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pRes->endwnmachname = pstLoad->endwnmachname;
    pRes->uscmdtype = cmdType;
    pRes->enmsgtrantype = xc8002_res_msgtran_type;
    pRes->enexerslt = xc8002_exe_cmd_sucess;
    pRes->uscmdinfolen = 4 + ulParaLen;

    if (ulParaLen > 0 && (pPara != 0))
    {
        memcpy(pRes->aucinfo, pPara, ulParaLen);
    }
    //执行异常处理
    app_exeFaultFc(pRes, cmdType, pstLoad, pstCmd);

    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pRes, msgLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)", ulrunrslt)
    }

    return;
}
/*****************************************************************************
 函 数 名  : app_constructSingleCmdFault
 功能描述  : 构造单一命令的异常
 输入参数  : msg_stMidRes* pPara  
             vos_u32 taskCount    
             vos_u32 faultId      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月22日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_constructSingleCmdFault(msg_stMidRes* pPara, vos_u32 taskCount,vos_u32 faultId)
{
    //异常构造
    pPara->enexerslt = xc8002_exe_cmd_failed;
    
    if (INVALID_FAULTID != faultId)
    {
        //上报故障
        app_simuReportFault(faultId);   
    }
    return;
}

vos_u32 app_constructRemainFault(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmemsize = 0;

	cmd_r1_reagent_ndl_vrotate_stru *pstLoadcmdinfo = (cmd_r1_reagent_ndl_vrotate_stru *)pstCmd;
    if(0 ==  pstLoadcmdinfo->reportmarginflag)
    {
        return VOS_OK;
    }
	app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = VOS_NULL;
	xc8002_report_remaindepth_stru* reportdepthmsg = VOS_NULL;

	ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) + sizeof(xc8002_report_remaindepth_stru) - 4;
    ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&reportmsg));
	
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

	reportmsg->uldstpid     = Pid_DwnMachUart;
    reportmsg->ulsrcpid     = Pid_DwnMachUart;
    reportmsg->usmsgtype    = app_res_dwnmach_act_cmd_msg_type;
    reportmsg->usmsgloadlen = ulmemsize - sizeof(vos_msg_header_stru);
	
	reportmsg->stresinfo.enmsgtrantype   = xc8002_req_msgtran_type;
    reportmsg->stresinfo.ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    reportmsg->stresinfo.endwnmachname   = pstLoad->endwnmachname;
	reportmsg->stresinfo.uscmdinfolen = sizeof(xc8002_report_remaindepth_stru);

	reportdepthmsg = (xc8002_report_remaindepth_stru*)&(reportmsg->stresinfo.uscmdtype);
	
	reportdepthmsg->cmd = XC8002_COM_REPORT_CMD;
    reportdepthmsg->usstatustype  = xc8002_dwnmach_report_remain_type;
	reportdepthmsg->usmechdevid = pstLoad->endwnmachname;    
    reportdepthmsg->usremaindepth = 0xFFFD;//根据需要设定
    reportdepthmsg->cupinfo.cupid    = pstLoadcmdinfo->cupid;
	reportdepthmsg->cupinfo.diskid   = pstLoadcmdinfo->diskid;
	reportdepthmsg->cupinfo.reactcup = pstLoadcmdinfo->reactcup;
    printf("cup(%d)  disk(%d) %lu===\n",reportdepthmsg->cupinfo.cupid ,  
              reportdepthmsg->cupinfo.diskid  ,
              reportdepthmsg->cupinfo.reactcup);
	ulrunrslt = vos_sendmsg((vos_msg_header_stru*)reportmsg);
        
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_sendmsg() failed(0x%x)\r\n", ulrunrslt);
    }

	return ulrunrslt;
}

/*****************************************************************************
 函 数 名  : app_cleanFaultFc
 功能描述  : 清理异常注册函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月22日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_cleanFaultFc()
{
    vos_u32 ulIndex = 0;
    for (ulIndex = 0; ulIndex < COUNTOF(g_faultFc); ulIndex++)
    {
        g_faultFc[ulIndex].cmdType = INVALID_CMDTYPE;
        g_faultFc[ulIndex].taskCount= 0;
        g_faultFc[ulIndex].faultId= 0;
        g_faultFc[ulIndex].faultFc= NULL;
        g_faultFc[ulIndex].reporFc= NULL;
        g_faultFc[ulIndex].cmdCount= 0;
    }

    return;
}

/*****************************************************************************
 函 数 名  : app_registerFaultFc
 功能描述  : 注册异常构造函数
 输入参数  : app_faultFc_mng_stru stFaultFc  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月22日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_registerFaultFc(app_faultFc_mng_stru stFaultFc)
{
    vos_u32 ulIndex = 0;
    for (ulIndex = 0; ulIndex < COUNTOF(g_faultFc); ulIndex++)
    {
        if (g_faultFc[ulIndex].cmdType== INVALID_CMDTYPE)
        {
            memcpy((void*)&g_faultFc[ulIndex], (void*)&stFaultFc, sizeof(stFaultFc));
            g_faultFc[ulIndex].cmdCount = 0;
            return VOS_OK;
        }
    }
    return APP_INVALID_U32;
}    

/*****************************************************************************
 函 数 名  : app_exeFaultFc
 功能描述  : 执行异常错误
 输入参数  : vos_u32 cmdType  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月22日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
bool app_exeFaultFc(msg_stMidRes* pPara, vos_u32 cmdType, msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulIndex = 0;
    for (ulIndex = 0; ulIndex < COUNTOF(g_faultFc); ulIndex++)
    {
        if (g_faultFc[ulIndex].cmdType== cmdType)
        {
            g_faultFc[ulIndex].cmdCount++;
            if ((g_faultFc[ulIndex].cmdCount == g_faultFc[ulIndex].taskCount) && (VOS_NULL != *g_faultFc[ulIndex].faultFc))
            {
                (*g_faultFc[ulIndex].faultFc)(pPara, g_faultFc[ulIndex].taskCount,g_faultFc[ulIndex].faultId);
            }
            if ((g_faultFc[ulIndex].cmdCount == g_faultFc[ulIndex].taskCount) && VOS_NULL != *g_faultFc[ulIndex].reporFc)
            {
                ((*g_faultFc[ulIndex].reporFc)(pstLoad, pstCmd));
                return true;
            }
            break;
        }
    }
    return false;
}


/*****************************************************************************
 函 数 名  : app_exeResFc
 功能描述  : 执行模拟函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月21日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_exeResFc(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulIndex = 0;
    for (ulIndex = 0; ulIndex < COUNTOF(g_simuResFc); ulIndex++)
    {
        if (g_simuResFc[ulIndex].cmdType== cmdType)
        {
            if (VOS_NULL != *g_simuResFc[ulIndex].resFc)
            {
                (*g_simuResFc[ulIndex].resFc)(pstLoad, cmdType, pstCmd);
                return VOS_OK;
            }
        }
    }

    return APP_INVALID_U32;
}


vos_u32 app_prostorecompensation(vos_u32 cmd, vos_u32 compType, cmd_compensation_para *compPara)
{
    vos_u32 ulrunrslt = VOS_OK;

    switch(cmd)
    {
        case cmd_sample_dsk_compensation:
            g_pcpsautoreq->g_cpucps0[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps0[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps0[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps0[compPara->location].step = compPara->step;
            break;
        case cmd_sample_ndl_compensation:
            g_pcpsautoreq->g_cpucps1[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps1[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps1[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps1[compPara->location].step = compPara->step;
            break;
        case cmd_inner_reagentdisk_compensation:
            g_pcpsautoreq->g_cpucps2[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps2[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps2[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps2[compPara->location].step = compPara->step;
            break;
        case cmd_r1_reagent_ndl_compensation:
            g_pcpsautoreq->g_cpucps3[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps3[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps3[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps3[compPara->location].step = compPara->step;
            break;
        case cmd_outer_reagentdisk_compensation:
            g_pcpsautoreq->g_cpucps4[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps4[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps4[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps4[compPara->location].step = compPara->step;
            break;
        case cmd_r2_reagent_ndl_compensation:
            g_pcpsautoreq->g_cpucps5[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps5[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps5[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps5[compPara->location].step = compPara->step;
            break;
        case cmd_react_disk_compensation:
            g_pcpsautoreq->g_cpucps6[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps6[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps6[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps6[compPara->location].step = compPara->step;
            break;
        case cmd_auto_wash_ndl_compensation:
            break;
        case cmd_sample_mix_ndl_compensation:
            g_pcpsautoreq->g_cpucps8[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps8[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps8[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps8[compPara->location].step = compPara->step;
            break;
        case cmd_reagent_mix_compensation:
            g_pcpsautoreq->g_cpucps9[compPara->location].comptype = compType;
            g_pcpsautoreq->g_cpucps9[compPara->location].location = compPara->location;
            g_pcpsautoreq->g_cpucps9[compPara->location].dir = compPara->dir;
            g_pcpsautoreq->g_cpucps9[compPara->location].step = compPara->step;
            break;
        default:
            ulrunrslt = APP_NO;
            break;
    }

    return ulrunrslt;
}


vos_u32 app_simuCompReport(msg_stMidDwnCom* pstload)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmemsize = 0;

    msg_stDwnmachCompensationReprot* pstresmsg = VOS_NULL;
    xc8002_com_report_cmd_stru* pstreport = VOS_NULL;

    msg_stHead rHead;

    ulmemsize = sizeof(msg_stDwnmachCompensationReprot) + 30*sizeof(xc8002_report_compensation_para);
    ulrunrslt = app_mallocBuffer(ulmemsize, (void**)(&pstresmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }
    
    app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);

    pstresmsg->ulcurcmdframeid = pstload->ulcurcmdframeid;
    pstresmsg->ulDwnmachName   = pstload->endwnmachname;
    
    pstreport = (xc8002_com_report_cmd_stru*)&(pstresmsg->usCmdType);
    pstreport->usstatustype = xc8002_dwnmach_report_compensation_type;

    if(xc8002_dwnmach_name_smpldisk == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_smpldisk;
        pstresmsg->usParaNum = 10 + 1;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);

        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps0,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_smpndl == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_smpndl;
        pstresmsg->usParaNum = 25 + 1;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);
                    
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps1,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_R2disk == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_R2disk;
        pstresmsg->usParaNum = 4 + 2;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);
                    
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps2,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_r1Ndl == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_r1Ndl;
        pstresmsg->usParaNum = 16 + 1;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);
                    
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps3,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_R1disk == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_R1disk;
        pstresmsg->usParaNum = 4 + 1;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);
                    
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps4,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_r2Ndl == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_r2Ndl;
        pstresmsg->usParaNum = 16 + 1;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);
                    
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps5,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_reactdisk == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_reactdisk;
        pstresmsg->usParaNum = 2 + 2;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);
                    
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps6,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_smpl_mixrod == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_smpl_mixrod;
        pstresmsg->usParaNum = 1 + 1;
                
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);   
                    
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps8,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
    else if(xc8002_dwnmach_name_reag_mixrod == pstload->endwnmachname)
    {
        pstresmsg->ulDwnmachName = xc8002_dwnmach_name_reag_mixrod;
        pstresmsg->usParaNum = 6 + 1;
                    
        pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
                                          + pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);   
            
        memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps9,
            pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
    }
	else if (xc8002_dwnmach_name_autowash == pstload->endwnmachname)
	{
		pstresmsg->ulDwnmachName = xc8002_dwnmach_name_autowash;
		pstresmsg->usParaNum = 1 + 1;

		pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachCompensationReprot)
			+ pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para);

		memcpy((char*)pstresmsg->ppara, (char*)g_pcpsautoreq->g_cpucps7,
			pstresmsg->usParaNum * sizeof(xc8002_report_compensation_para));
	}
    pstresmsg->usCmdType       = XC8002_COM_REPORT_CMD;
    pstresmsg->enmsgtrantype   = xc8002_req_msgtran_type;
    
    ulrunrslt = app_sendMsg(&rHead, pstresmsg, ulmemsize);
	
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_sendMsg() failed(%d) \n", ulrunrslt);
    }
    
    return ulrunrslt;
}

vos_u32 app_simuMotorReport(msg_stMidDwnCom* pstload)
{
    msg_stDwnmachMotorReprot* pstresmsg = VOS_NULL;
    vos_u32 ulmemsize = sizeof(msg_stDwnmachMotorReprot) + sizeof(xc8002_report_motor_para);
    vos_u32 ulrunrslt = app_mallocBuffer(ulmemsize, (void**)(&pstresmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

    pstresmsg->endwnmachname = pstload->endwnmachname;
    pstresmsg->enmsgtrantype = xc8002_req_msgtran_type;
    pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachMotorReprot) + pstresmsg->usParaNum * sizeof(xc8002_report_motor_para);
    pstresmsg->ulcurcmdframeid = pstload->ulcurcmdframeid;

    pstresmsg->usCmdType = XC8002_COM_REPORT_CMD;
    pstresmsg->enStatusType = xc8002_dwnmach_report_motorline_type;

    pstresmsg->ulDwnmachName = pstload->endwnmachname;
    pstresmsg->usParaNum = 1;

    xc8002_report_motor_para * pPara = (xc8002_report_motor_para *)pstresmsg->ppara;
    pPara->drvId = 1;
    pPara->lineId = 1;
    pPara->startFre = 10;
    pPara->endFre = 200;
    pPara->echLaderStep = 10;
    pPara->upMaxLader = 10;
    pPara->sPar = 600;
    msg_stHead rHead;
    app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    return app_sendMsg(&rHead, pstresmsg, ulmemsize);
}

vos_u32 app_simuShelfReport(msg_stMidDwnCom* pstload)
{
    msg_stDwnmachShelfReprot* pstresmsg = VOS_NULL;
    vos_u32 ulmemsize = sizeof(msg_stDwnmachShelfReprot);
    vos_u32 ulrunrslt = app_mallocBuffer(ulmemsize, (void**)(&pstresmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

    pstresmsg->endwnmachname = pstload->endwnmachname;
    pstresmsg->enmsgtrantype = xc8002_req_msgtran_type;
    pstresmsg->uscmdinfolen = sizeof(msg_stDwnmachShelfReprot);
    pstresmsg->ulcurcmdframeid = pstload->ulcurcmdframeid;

    pstresmsg->usCmdType = XC8002_COM_REPORT_CMD;
    pstresmsg->enStatusType = dwnmach_report_shelf;

    pstresmsg->shelfNum = 3;
    msg_stHead rHead;
    app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    return app_sendMsg(&rHead, pstresmsg, ulmemsize);
}

void app_simuSetComp(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes); //根据具体长度来算的
                
    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)",ulrunrslt);
        return;
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname   = pstLoad->endwnmachname;
    pPara->uscmdtype       = cmdType;
    pPara->enmsgtrantype   = xc8002_res_msgtran_type;
    pPara->enexerslt       = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen    = 4;
    
    cmd_stCompensation * pComp = (cmd_stCompensation *)(&(pstLoad->uscmdtype));
    cmd_compensation_para * compPara = (cmd_compensation_para *)&(pComp->ppara);
    
    for (vos_u32 i = 0; i < pComp->paranum; i++)
    {
        app_prostorecompensation(cmdType, pComp->type,compPara);
        compPara++;
    }
    //执行异常处理
    app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd);
    
    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)",ulrunrslt)
    }

    return;
}


/*****************************************************************************
 函 数 名  : app_simuReagMargScan
 功能描述  : 模拟试剂余量扫描0x2c1a和0x301a
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月20日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_simuReagMargScan(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes)+4; //根据具体长度来算的
                
    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)",ulrunrslt);
        return;
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname   = pstLoad->endwnmachname;
    pPara->uscmdtype       = cmdType;
    pPara->enmsgtrantype   = xc8002_res_msgtran_type;
	pPara->enexerslt       = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen    = sizeof(cmd_r2_reagent_marginscan_res_stru);
    
    cmd_r2_reagent_marginscan_res_stru *pstmargscnrslt = (cmd_r2_reagent_marginscan_res_stru*)(&(pPara->uscmdtype));
    pstmargscnrslt->usdeclinehigh = 76;

    //执行异常处理
    app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd);
    
    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)",ulrunrslt)
    }
    return;
}

/*****************************************************************************
 函 数 名  : app_simuRemainDepth
 功能描述  : 模拟试剂余量上报
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月26日
    作    者   : lixin
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_simuRemainDepth(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmemsize = 0;

	cmd_r1_reagent_ndl_vrotate_stru *pstLoadcmdinfo = (cmd_r1_reagent_ndl_vrotate_stru *)pstCmd;
    if(0 ==  pstLoadcmdinfo->reportmarginflag)
    {
        return VOS_OK;
    }
    
	app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = VOS_NULL;
	xc8002_report_remaindepth_stru* reportdepthmsg = VOS_NULL;

	ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) + sizeof(xc8002_report_remaindepth_stru) - 4;
    ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&reportmsg));
	
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

	reportmsg->uldstpid     = Pid_DwnMachUart;
    reportmsg->ulsrcpid     = Pid_DwnMachUart;
    reportmsg->usmsgtype    = app_res_dwnmach_act_cmd_msg_type;
    reportmsg->usmsgloadlen = ulmemsize - sizeof(vos_msg_header_stru);
	
	reportmsg->stresinfo.enmsgtrantype   = xc8002_req_msgtran_type;
    reportmsg->stresinfo.ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    reportmsg->stresinfo.endwnmachname   = pstLoad->endwnmachname;
	reportmsg->stresinfo.uscmdinfolen = sizeof(xc8002_report_remaindepth_stru);

	reportdepthmsg = (xc8002_report_remaindepth_stru*)&(reportmsg->stresinfo.uscmdtype);
	
	reportdepthmsg->cmd = XC8002_COM_REPORT_CMD;
    reportdepthmsg->usstatustype  = xc8002_dwnmach_report_remain_type;
    reportdepthmsg->usmechdevid = pstLoad->endwnmachname;
    reportdepthmsg->usremaindepth = 66;//根据需要设定
    reportdepthmsg->cupinfo.cupid    = pstLoadcmdinfo->cupid;
	reportdepthmsg->cupinfo.diskid   = pstLoadcmdinfo->diskid;
	reportdepthmsg->cupinfo.reactcup = pstLoadcmdinfo->reactcup;
    //printf("===cup(%d)  disk(%d) %lu\n",reportdepthmsg->cupinfo.cupid ,  
    //          reportdepthmsg->cupinfo.diskid  ,
    //          reportdepthmsg->cupinfo.reactcup);
	ulrunrslt = vos_sendmsg((vos_msg_header_stru*)reportmsg);
        
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_sendmsg() failed(0x%x)\r\n", ulrunrslt);
    }

	return ulrunrslt;
}
vos_u32 app_simuRemainDepth_regent(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmemsize = 0;
    static int reDeth = 18;
    CMD_NDL_DOWN_TYPE *pstLoadcmdinfo = (CMD_NDL_DOWN_TYPE *)pstCmd;

    app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = VOS_NULL;
    xc8002_report_remaindepth_stru* reportdepthmsg = VOS_NULL;

    ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) + sizeof(xc8002_report_remaindepth_stru) - 4;
    ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&reportmsg));

    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

    reportmsg->uldstpid = Pid_DwnMachUart;
    reportmsg->ulsrcpid = Pid_DwnMachUart;
    reportmsg->usmsgtype = app_res_dwnmach_act_cmd_msg_type;
    reportmsg->usmsgloadlen = ulmemsize - sizeof(vos_msg_header_stru);

    reportmsg->stresinfo.enmsgtrantype = xc8002_req_msgtran_type;
    reportmsg->stresinfo.ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    reportmsg->stresinfo.endwnmachname = pstLoad->endwnmachname;
    reportmsg->stresinfo.uscmdinfolen = sizeof(xc8002_report_remaindepth_stru);

    reportdepthmsg = (xc8002_report_remaindepth_stru*)&(reportmsg->stresinfo.uscmdtype);

    reportdepthmsg->cmd = XC8002_COM_REPORT_CMD;
    reportdepthmsg->usstatustype = xc8002_dwnmach_report_remain_type;
    reportdepthmsg->usmechdevid = pstLoad->endwnmachname;
    reportdepthmsg->usremaindepth = reDeth++;//根据需要设定
    reportdepthmsg->cupinfo.cupid = pstLoadcmdinfo->place_number;
    reportdepthmsg->cupinfo.diskid = pstLoadcmdinfo->ring_number;
    reportdepthmsg->cupinfo.reactcup = pstLoadcmdinfo->cup_number;
    //printf("===cup(%d)  disk(%d) %lu\n",reportdepthmsg->cupinfo.cupid ,  
    //          reportdepthmsg->cupinfo.diskid  ,
    //          reportdepthmsg->cupinfo.reactcup);
    ulrunrslt = vos_sendmsg((vos_msg_header_stru*)reportmsg);

    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_sendmsg() failed(0x%x)\r\n", ulrunrslt);
    }

    return ulrunrslt;
}
vos_u32 app_simuADValue(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    CMD_READ_AD_TYPE *pstLoadcmdinfo = (CMD_READ_AD_TYPE *)pstCmd;
    if (0 == pstLoadcmdinfo->AdCount)
    {
        return VOS_OK;
    }

    app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = VOS_NULL;
    app_u32 ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) + sizeof(ADValue_st) - 4
        + pstLoadcmdinfo->AdCount*sizeof(uint32_t) * WAVE_NUM;

    vos_u32 ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&reportmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

    ADValue_st* reportdepthmsg = (ADValue_st*)&(reportmsg->stresinfo.uscmdtype);

    reportdepthmsg->cmd = XC8002_COM_REPORT_CMD;
    reportdepthmsg->usstatustype = dwnmach_report_AD;
    reportdepthmsg->AdCount = pstLoadcmdinfo->AdCount * WAVE_NUM;
    app_u32* pValue = reportdepthmsg->value;
    for (app_u32 iCount = 0; iCount < reportdepthmsg->AdCount; iCount++)
    {
        *pValue = iCount * 5;
        pValue++;
    }
    return app_simuReport(pstLoad, ulmemsize, (char*)reportmsg);
}

vos_u32 app_simuOneADValue(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru * pstCmd)
{
    app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = VOS_NULL;
    app_u32 ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) + sizeof(ADValue_st) - 4
        + 1*sizeof(uint32_t) * WAVE_NUM;

    vos_u32 ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&reportmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

    ADValue_st* reportdepthmsg = (ADValue_st*)&(reportmsg->stresinfo.uscmdtype);

    reportdepthmsg->cmd = XC8002_COM_REPORT_CMD;
    reportdepthmsg->usstatustype = dwnmach_report_AD;
    reportdepthmsg->AdCount = 1 * WAVE_NUM;
    app_u32* pValue = reportdepthmsg->value;
    for (app_u32 iCount = 0; iCount < reportdepthmsg->AdCount; iCount++)
    {
        *pValue = iCount * 5;
        pValue++;
    }
    return app_simuReport(pstLoad, ulmemsize, (char*)reportmsg);
}

void app_simuReadAD(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes); //根据具体长度来算的

    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)", ulrunrslt);
        return;
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname = pstLoad->endwnmachname;
    pPara->uscmdtype = cmdType;
    pPara->enmsgtrantype = xc8002_res_msgtran_type;
    pPara->enexerslt = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen = 4;
    //pPara->rsv[1] = 123u;
    //执行异常处理
    if (!app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd))
    {
        //AD上报
        if (cmdType == CMD_READ_AD)
        {
            app_simuADValue(pstLoad, pstCmd);
        }
        else
        {
            app_simuOneADValue(pstLoad, pstCmd);
        }
    }

    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)", ulrunrslt)
    }
}

/*****************************************************************************
 函 数 名  : app_simuNdlDown
 功能描述  : 模拟试剂针下降0x2c0a和0x300a
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月26日
    作    者   : lixin
    修改内容   : 新生成函数

*****************************************************************************/
void app_simuNdlDown(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes); //根据具体长度来算的
                
    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)",ulrunrslt);
        return;
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname   = pstLoad->endwnmachname;
    pPara->uscmdtype       = cmdType;
    pPara->enmsgtrantype   = xc8002_res_msgtran_type;
    pPara->enexerslt       = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen    = 4;
    //pPara->rsv[1] = 123u;
    //执行异常处理
    if (!app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd))
    {
        //试剂余量上报
        app_simuRemainDepth(pstLoad, pstCmd);
    }
    
//     if (cmdType == CMD_NDL_UP)
//     {
//         pPara->enexerslt = xc8002_exe_cmd_failed;
//     }
    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)",ulrunrslt)
    }
}

vos_u32 app_simuWaveReport(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = VOS_NULL;
    app_u32 ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) + sizeof(Report_ShowOP_st) - 4;
    vos_u32 ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&reportmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }
    Report_ShowOP_st* reportdepthmsg = (Report_ShowOP_st*)&(reportmsg->stresinfo.uscmdtype);

    reportdepthmsg->cmd = XC8002_COM_REPORT_CMD;
    reportdepthmsg->usstatustype = dwnmach_show_wave;
    reportdepthmsg->dwnmachinetype = pstLoad->endwnmachname;

    AD_OP_TYPE*pPara = (AD_OP_TYPE*)reportdepthmsg->value;
    for (app_u32 iCount = 0; iCount < 2000; iCount++)
    {
        pPara->ad = iCount*10;
        pPara->op = iCount;
        pPara++;
    }
    return app_simuReport(pstLoad, ulmemsize, (char*)reportmsg);
}

typedef struct
{
    vos_u16 execmd;
    vos_u16 rsv1;
    vos_u32 dataPage;
}updateRes;

void app_simuDwnUpdate(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes) + sizeof(updateRes); //根据具体长度来算的

    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)", ulrunrslt);
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname = pstLoad->endwnmachname;
    pPara->uscmdtype = cmdType;
    pPara->enmsgtrantype = xc8002_res_msgtran_type;
    pPara->enexerslt = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen = 4+8;

    cmd_updateDwn* pUpdate = (cmd_updateDwn*)pstCmd;
    updateRes *pUpdateRes = (updateRes*)pPara->aucinfo;
    pUpdateRes->dataPage = pUpdate->dataPage;
    pUpdateRes->execmd = pUpdate->exeCmd;

    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)", ulrunrslt)
    }
}

void app_simuFpgaUpdate(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes) + sizeof(updateRes); //根据具体长度来算的

    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)", ulrunrslt);
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname = pstLoad->endwnmachname;
    pPara->uscmdtype = cmdType;
    pPara->enmsgtrantype = xc8002_res_msgtran_type;
    pPara->enexerslt = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen = 4 + 8;

    cmd_updateFpga* pUpdate = (cmd_updateFpga*)pstCmd;
    updateRes *pUpdateRes = (updateRes*)pPara->aucinfo;
    pUpdateRes->dataPage = pUpdate->dataPage;
    pUpdateRes->execmd = pUpdate->flag;

    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)", ulrunrslt)
    }
}

void app_simuRNdlDown(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;
    static vos_u32 ndlDownNumber = 0;
    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes); //根据具体长度来算的

    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)", ulrunrslt);
        return;
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname = pstLoad->endwnmachname;
    pPara->uscmdtype = cmdType;
    pPara->enmsgtrantype = xc8002_res_msgtran_type;
    pPara->enexerslt = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen = 4;
    //pPara->rsv[1] = 123u;
    //执行异常处理
    if (pstLoad->endwnmachname == xc8002_dwnmach_name_r1Ndl || pstLoad->endwnmachname == xc8002_dwnmach_name_r1bNdl)//!app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd))
    {
        //试剂余量上报
        if (ndlDownNumber++%2==0)
        {
            app_simuRemainDepth_regent(pstLoad, pstCmd);
        }
        
    }
    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)", ulrunrslt)
    }
}

/*****************************************************************************
 函 数 名  : app_simuReagBcdScan
 功能描述  : 试剂盘条码扫描
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月20日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_simuReagBcdScan(msg_stMidDwnCom* pstLoad, vos_u32 cmdType,xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;
   
    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes)+ sizeof(app_dwn_barcode_stru) 
                + sizeof(app_dwn_barcode_tlv_stru) + MAX_BARCODE_LEN;
    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)",ulrunrslt);
        return;
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname   = pstLoad->endwnmachname;
    pPara->uscmdtype       = cmdType;
    pPara->enmsgtrantype   = xc8002_res_msgtran_type;
    pPara->enexerslt       = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen    = 4 + sizeof(app_dwn_barcode_stru) + sizeof(app_dwn_barcode_tlv_stru)+ 20;
    
    cmd_outer_reagdsk_rtt_to_bcdscan_stru *pbcscanreq  = VOS_NULL;
    app_dwn_barcode_stru *pdwnbarcode = VOS_NULL;
    app_dwn_barcode_tlv_stru *pdwnbarcodetlv = VOS_NULL;

    pbcscanreq = (cmd_outer_reagdsk_rtt_to_bcdscan_stru *)(&pstLoad->uscmdtype);
        
    pdwnbarcode = (app_dwn_barcode_stru *)(pPara->aucinfo);    
    pdwnbarcode->traytype = app_xc8002_tray_type_reagent;
    pdwnbarcode->tlvnum = 1;
    pdwnbarcodetlv = (app_dwn_barcode_tlv_stru *)(pdwnbarcode->auctlvinfo);

    pdwnbarcodetlv->trayno = (pstLoad->uscmdtype == cmd_outer_reagentdisk_rtt_to_bcdscan)
                        ? app_xc8002_reagent_outer_disk_id : app_xc8002_reagent_inner_disk_id;
    pdwnbarcodetlv->cupno = (char)pbcscanreq->uccupnum;
    pdwnbarcodetlv->len = 17;            
	memcpy(pdwnbarcodetlv->aucbarcode, "19217030350082I", 17); 
    
    //执行异常处理
    app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd);
    
    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)",ulrunrslt)
    }

    return;
}

vos_u32 _reportParameter(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmemsize = 0;
    
    CMD_COMMON_DATA_RW_PAGE_DATA_TYPE *pstLoadcmdinfo = (CMD_COMMON_DATA_RW_PAGE_DATA_TYPE *)pstCmd;
    if (1 == pstLoadcmdinfo->rw)
    {
        return VOS_OK;
    }

    app_mnguart_middwnmach_cmd_res_msg_stru* reportmsg = VOS_NULL;
    Report_PAGE_st *reportdepthmsg = VOS_NULL;
    ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru) + sizeof(Report_PAGE_st) - sizeof(xc8002_com_report_cmd_stru)
        + 5 * sizeof(app_u32);
    ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&reportmsg));

    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

    reportmsg->uldstpid = Pid_DwnMachUart;
    reportmsg->ulsrcpid = Pid_DwnMachUart;
    reportmsg->usmsgtype = app_res_dwnmach_act_cmd_msg_type;
    reportmsg->usmsgloadlen = ulmemsize - sizeof(vos_msg_header_stru);

    reportmsg->stresinfo.enmsgtrantype = xc8002_req_msgtran_type;
    reportmsg->stresinfo.ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    reportmsg->stresinfo.endwnmachname = pstLoad->endwnmachname;
    reportmsg->stresinfo.uscmdinfolen = sizeof(Report_PAGE_st) + 5 * sizeof(app_u32);

    reportdepthmsg = (Report_PAGE_st*)&(reportmsg->stresinfo.uscmdtype);

    reportdepthmsg->cmd = XC8002_COM_REPORT_CMD;
    reportdepthmsg->usstatustype = xc8002_dwnmach_report_compensation_type;
    reportdepthmsg->dwnmachinetype = pstLoad->endwnmachname;
    reportdepthmsg->number = pstLoadcmdinfo->number;

    reportdepthmsg->count = 5;
    app_u32* pValue = reportdepthmsg->datas;
    for (app_u32 iCount = 0; iCount < reportdepthmsg->count; iCount++)
    {
        *pValue = iCount * 5;
        pValue++;
    }
    return vos_sendmsg((vos_msg_header_stru*)reportmsg);
}

void app_simuTempQuery(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    if (pstLoad->endwnmachname == xc8002_dwnmach_name_auto_Rtmpctrl)
    {
        ReactTemp_st stPara;
        stPara.slavertemp = 373;
        stPara.slcurtemp1 = 371;
        stPara.slcurtemp2 = 372;
        stPara.slcurtemp3 = 373;
        stPara.slcurtemp4 = 376;
        constructResPara(pstLoad, cmdType, pstCmd, sizeof(ReactTemp_st), (char*)&stPara);
    }
    if (pstLoad->endwnmachname == xc8002_dwnmach_name_auto_wtmpctrl)
    {
        WarterTemp_st stPara;
        stPara.slmachine2 = 323;
        stPara.slmachine1 = 331;
        stPara.slcleaner = 342;
        stPara.slwater = 353;
        constructResPara(pstLoad, cmdType, pstCmd, sizeof(WarterTemp_st), (char*)&stPara);
    }
}

int smpBarcode = 0;
/*****************************************************************************
 函 数 名  : app_constructSampBcd
 功能描述  : 构造样本条码
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月20日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_constructSampBcd(app_dwn_barcode_stru *pdwnbarcode, vos_u32 count,
                                       vos_u32 trayno, vos_u32 cupid)
{
    app_dwn_barcode_tlv_stru *pdwnbarcodetlv = VOS_NULL;
    
    if (count == SAMPLE_DISK_3_CIRCLE_TOGETHER_SCAN)
    { 
        pdwnbarcode->traytype = app_xc8002_tray_type_sample;
        pdwnbarcode->tlvnum = 3;
        pdwnbarcodetlv = (app_dwn_barcode_tlv_stru *)(pdwnbarcode->auctlvinfo);
        
        pdwnbarcodetlv->trayno = 0;
        pdwnbarcodetlv->cupno = (char)cupid;
        pdwnbarcodetlv->len = 17;
        memcpy(pdwnbarcodetlv->aucbarcode, "008141020500097a4", 17);

        pdwnbarcodetlv = (app_dwn_barcode_tlv_stru *)((char *)pdwnbarcodetlv + sizeof(app_dwn_barcode_tlv_stru) + pdwnbarcodetlv->len);

        pdwnbarcodetlv->trayno = 1;
        pdwnbarcodetlv->cupno = (char)cupid;
        pdwnbarcodetlv->len = 17;
        memcpy(pdwnbarcodetlv->aucbarcode, "012141020500097a4", 17);

        pdwnbarcodetlv = (app_dwn_barcode_tlv_stru *)((char *)pdwnbarcodetlv + sizeof(app_dwn_barcode_tlv_stru) + pdwnbarcodetlv->len);

        pdwnbarcodetlv->trayno = 2;
        pdwnbarcodetlv->cupno = (char)cupid;
        pdwnbarcodetlv->len = 17;
        memcpy(pdwnbarcodetlv->aucbarcode, "072141020500097a4", 17);
    }
    else
    {         
        pdwnbarcode->traytype = app_xc8002_tray_type_sample;
        pdwnbarcode->tlvnum = 1;
        pdwnbarcodetlv = (app_dwn_barcode_tlv_stru *)(pdwnbarcode->auctlvinfo);
        
        pdwnbarcodetlv->trayno = trayno;
        pdwnbarcodetlv->cupno = cupid;
        pdwnbarcodetlv->len = 10;

        snprintf(pdwnbarcodetlv->aucbarcode, 11, "%d", smpBarcode++);

        WRITE_INFO_LOG(" ### trayno = %d, cupno = %d\n", (int)pdwnbarcodetlv->trayno, (int)pdwnbarcodetlv->cupno);
    } 
    return;
}

/*****************************************************************************
 函 数 名  : app_simuSampBcdScan
 功能描述  : 样本条码扫描
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月20日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_simuSampBcdScan(msg_stMidDwnCom* pstLoad, vos_u32 cmdType,xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;

    cmd_sample_dsk_bcdscan_stru *psamplebcscan = (cmd_sample_dsk_bcdscan_stru *)(&pstLoad->uscmdtype);

    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes)+ sizeof(app_dwn_barcode_stru) 
                + sizeof(app_dwn_barcode_tlv_stru) + MAX_BARCODE_LEN;
    if (psamplebcscan->uccircleid == SAMPLE_DISK_3_CIRCLE_TOGETHER_SCAN)
    {
        ulParaLen += 2*(sizeof(app_dwn_barcode_tlv_stru) + MAX_BARCODE_LEN);
    }
    
    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmsgbuffer() failed(0x%x)",ulrunrslt);
        return;
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname   = pstLoad->endwnmachname;
    pPara->uscmdtype       = cmdType;
    pPara->enmsgtrantype   = xc8002_res_msgtran_type;
    pPara->enexerslt       = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen    = 4 + sizeof(app_dwn_barcode_stru) + sizeof(app_dwn_barcode_tlv_stru)+ 20;
    if (psamplebcscan->uccircleid == SAMPLE_DISK_3_CIRCLE_TOGETHER_SCAN)
    {
        pPara->uscmdinfolen += 2*(sizeof(app_dwn_barcode_tlv_stru)+ 20);
    }
   
    app_dwn_barcode_stru *pdwnbarcode = VOS_NULL;

                
    pdwnbarcode = (app_dwn_barcode_stru *)(pPara->aucinfo);  

    app_constructSampBcd(pdwnbarcode,psamplebcscan->uccircleid
                           ,psamplebcscan->uccircleid, psamplebcscan->uccupid);
    //执行异常处理
    app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd);
    
    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)",ulrunrslt)
    }

    return;
}



/*****************************************************************************
 函 数 名  : app_simuDwnmachSingleCmdRes
 功能描述  : 模拟下位机单一动作返回
 输入参数  :   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月20日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_simuDwnmachSingleCmdRes(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    vos_u32 ulParaLen = 0;
    ulParaLen = sizeof(msg_stMidRes); //根据具体长度来算的
                
    msg_stMidRes* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer failed(0x%x)",ulrunrslt);
    }
    pPara->ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pPara->endwnmachname   = pstLoad->endwnmachname;
    pPara->uscmdtype       = cmdType;
    pPara->enmsgtrantype   = xc8002_res_msgtran_type;
    pPara->enexerslt       = xc8002_exe_cmd_sucess;
    pPara->uscmdinfolen    = 4;

    //执行异常处理
    app_exeFaultFc(pPara, cmdType, pstLoad, pstCmd);
    
    msg_stHead pHead;
    app_constructMsgHead(&pHead, Pid_DwnMachUart, Pid_SimulatorTest, app_res_dwnmach_act_cmd_msg_type);
    ulrunrslt = app_sendMsg(&pHead, pPara, ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("send msg failed(0x%x)",ulrunrslt)
    }

    return;
}

/*****************************************************************************
 函 数 名  : app_simuDwnmachComCmdRes
 功能描述  : 模拟下位机组合动作返回
 输入参数  :   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月20日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_simuDwnmachComCmdRes(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd)
{
    vos_u32 ulloopvar = 0;

    xc8002_com_dwnmach_comb_cmd_stru*       pstcombcmd = VOS_NULL;
    xc8002_com_comb_single_cmd_stru*      pstsinglecmd = VOS_NULL;
    pstcombcmd   = (xc8002_com_dwnmach_comb_cmd_stru*)(&(pstLoad->uscmdtype));
    pstsinglecmd = (xc8002_com_comb_single_cmd_stru*)(&(pstcombcmd->acmdinfo[0]));
    
    for (ulloopvar = 0; pstcombcmd->cmdnum > ulloopvar; ulloopvar++)
    {   
        xc8002_com_dwnmach_cmd_stru *pstCmd = (xc8002_com_dwnmach_cmd_stru *)&(pstsinglecmd->stcmdinfo);
        if (APP_INVALID_U32 == app_exeResFc(pstLoad, pstsinglecmd->stcmdinfo.cmd, pstCmd))  
        {
            app_simuDwnmachSingleCmdRes(pstLoad, pstsinglecmd->stcmdinfo.cmd, pstCmd);
        }
        
        pstsinglecmd = (xc8002_com_comb_single_cmd_stru*)(((char*)(pstsinglecmd))
                     + pstsinglecmd->cmdsize + sizeof(pstsinglecmd->cmdsize));
    }

    return ;
}


/*****************************************************************************
 函 数 名  : app_dbguartsendackmsg
 功能描述  : 串口调试模块发送确认消息
 输入参数  : xc8002_uart_prot_data_format_stru* paurtpack
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月14日
    作    者   : zxc
    修改内容   : 新生成函数


*****************************************************************************/
extern void app_simuUartAckMsg(msg_stMidDwnCom* paurtpack)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmemsize = 0;
    
    msg_stMidDwnCom*      pstLoad = paurtpack;
    app_mnguart_middwnmach_cmd_res_msg_stru* pstresmsg = VOS_NULL;

    ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru);
    
    ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&pstresmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                   "Call vos_mallocmsgbuffer() failed(0x%x)\r\n",
                   ulrunrslt);
        return;
    }

    pstresmsg->uldstpid     = Pid_DwnMachUart;
    pstresmsg->ulsrcpid     = Pid_SimulatorTest;
    pstresmsg->usmsgtype    = app_res_dwnmach_act_cmd_msg_type;
    pstresmsg->usmsgloadlen = vos_calcvosmsgldlen(app_mnguart_middwnmach_cmd_res_msg_stru);

    pstresmsg->stresinfo.ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pstresmsg->stresinfo.endwnmachname   = pstLoad->endwnmachname;
    pstresmsg->stresinfo.uscmdinfolen    = 4;
    pstresmsg->stresinfo.uscmdtype       = pstLoad->uscmdtype;

    pstresmsg->stresinfo.enmsgtrantype = xc8002_ack_msgtran_type;

    ulrunrslt = vos_sendmsg((vos_msg_header_stru*)pstresmsg);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                   "Call vos_sendmsg() failed(0x%x)\r\n",
                   ulrunrslt);
    }
    
    return;
}


/*****************************************************************************
 函 数 名  : app_dbguartsenddwnmachresmsg
 功能描述  : 调试用串口发送下位机响应消息
 输入参数  : xc8002_uart_prot_data_format_stru* paurtpack
             vos_u32 ulrestype(0:命令确认响应, 1:命令执行结果响应)
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月14日
    作    者   : zxc
    修改内容   : 新生成函数


*****************************************************************************/
void app_simuUartResMsg(msg_stMidDwnCom* paurtpack)
{
    msg_stMidDwnCom* pstLoad = paurtpack;
    xc8002_com_dwnmach_cmd_stru *pstCmd = (xc8002_com_dwnmach_cmd_stru *)&(pstLoad->uscmdtype);
    
    if (APP_INVALID_U32 == app_exeResFc(pstLoad, pstLoad->uscmdtype, pstCmd))
    {
        
        app_simuDwnmachSingleCmdRes(pstLoad,pstLoad->uscmdtype,pstCmd); 
    }
    
    return;
}


/*****************************************************************************
 函 数 名  : app_simuDwnmachQueryData
 功能描述  : 模拟下位机查询数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月14日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_simuDwnmachQueryData(msg_stMidDwnCom* paurtpack)
{
    vos_u32 ulrunrslt = VOS_OK;
    msg_stMidDwnCom* pstLoad = paurtpack;
    
    if (CMD_COM_RW_PARAMETER_PAGE == pstLoad->uscmdtype)
    {
        xc8002_com_dwnmach_cmd_stru* pstCmd = (xc8002_com_dwnmach_cmd_stru*)&(pstLoad->uscmdtype);
        return _reportParameter(pstLoad, pstCmd);
    }
    if (CMD_READ_ALL_AD == pstLoad->uscmdtype)
    {
        xc8002_com_dwnmach_cmd_stru* pstCmd = (xc8002_com_dwnmach_cmd_stru*)&(pstLoad->uscmdtype);
        return app_simuWaveReport(pstLoad, pstCmd);
    }
    if(XC8002_COM_COMQUERY_CMD != pstLoad->uscmdtype)
    {
        return ulrunrslt;
    }
   
    vos_u32 ulmemsize = 0;    
    app_mnguart_middwnmach_cmd_res_msg_stru* pstresmsg = VOS_NULL; 
    cmd_stDwnQuery* pstquery = VOS_NULL;
    xc8002_com_report_cmd_stru* pstreport = VOS_NULL;

    cmd_fpga_dwnmach_version_stru* pstversion = VOS_NULL;
    cmd_react_disk_lightgetval_stru* lightgetval = VOS_NULL;
    //xc8002_report_dwnmachine_compensation* compensation = VOS_NULL;
    cmd_cooling_fan_status_stru* fan_status     = VOS_NULL;
    cmd_cooling_vol_stru* cooling_vol           = VOS_NULL;
    cmd_autowash_status_stru* autowash_status   = VOS_NULL;
    cmd_cooling_temp_stru* cooling_temp         = VOS_NULL;
    cmd_temp_control_report_stru* tempreport    = VOS_NULL;
    cmd_temp_control_eeprom_info_stru* tempeeprom = VOS_NULL;
    xc8002_report_dwnmachine_bblog *bblog = VOS_NULL;
    
    pstquery = (cmd_stDwnQuery*)&(pstLoad->uscmdtype);
    printf("Simulation Receive cmd = 0x%x, queryType = %d\n",(int)pstquery->cmd, (int)pstquery->queryType);

    ulmemsize = sizeof(app_mnguart_middwnmach_cmd_res_msg_stru);
    
    if (xc8002_dwnmach_report_compensation_type == pstquery->queryType)
    {
        app_simuCompReport(pstLoad);
        return 0;
    }
    
    if (xc8002_dwnmach_report_motorline_type == pstquery->queryType)
    {
        app_simuMotorReport(pstLoad);
        return 0;
    }

    switch (pstquery->queryType)
    {
        case xc8002_dwnmach_report_downmachin_versions://下位机版本上报
            ulmemsize += sizeof(cmd_fpga_dwnmach_version_stru);
            break;
        case xc8002_dwnmach_report_LightBulbAD_type ://光源灯电压AD值上报
            ulmemsize += sizeof(cmd_react_disk_lightgetval_stru);
            break;
        case xc8002_dwnmach_report_compensation_type ://下位机机械补偿类型
            ulmemsize += sizeof(xc8002_report_dwnmachine_compensation)
                       + 30*sizeof(xc8002_report_compensation_para);
            break;
        case xc8002_dwnmach_report_fanstatus_type ://风扇运行状态
            ulmemsize += sizeof(cmd_cooling_fan_status_stru);  
            break;
        case xc8002_dwnmach_report_electricity_type ://制冷片电流
            ulmemsize += sizeof(cmd_cooling_vol_stru);
            break;
        case xc8002_dwnmach_report_fl_and_pre_type://浮子、压力查询
        ulmemsize += sizeof(cmd_autowash_status_stru);
            break;
        case xc8002_dwnmach_report_reagdsk_temp_query_type://制冷温度查询上报，非定时上报
        ulmemsize += sizeof(cmd_cooling_temp_stru);
            break;
        case xc8002_dwnmach_report_thermal_control_temp_type://温控温度查询上报，非定时上报
		case xc8002_dwnmach_report_ad_temp_type://温控温度AD查询上报，非定时上报
        ulmemsize += sizeof(cmd_temp_control_report_stru);
            break;
        case xc8002_dwnmach_report_eeprom_info_type://eeprom存储信息上报
            if(xc8002_dwnmach_name_cooling_reag == pstLoad->endwnmachname)
            {
                ulmemsize += sizeof(cmd_temp_control_eeprom_info_stru);
            }
            else//TODO
            {
                ulmemsize += sizeof(cmd_temp_control_eeprom_info_stru);
            }
            break;
        case xc8002_dwnmach_report_bblog_info_type:
        {
            ulmemsize += sizeof(xc8002_report_dwnmachine_bblog)
                        + 16*sizeof(xc8002_bblog_record);
            break;
        }
        case xc8001_dwnmach_report_ID:
        {
            ulmemsize += sizeof(cmd_CPUID);
            break;
        }
        default:
            break;
    }
    ulmemsize -= 2;
    ulrunrslt = vos_mallocmsgbuffer(ulmemsize, (void**)(&pstresmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                   "Call vos_mallocmsgbuffer() failed(0x%x)\r\n",
                   ulrunrslt);
        return ulrunrslt;
    }

    pstresmsg->uldstpid     = Pid_DwnMachUart;
    pstresmsg->ulsrcpid     = Pid_SimulatorTest;
    pstresmsg->usmsgtype    = app_res_dwnmach_act_cmd_msg_type;
    pstresmsg->usmsgloadlen = vos_calcvosmsgldlen(app_mnguart_middwnmach_cmd_res_msg_stru);

    pstresmsg->stresinfo.ulcurcmdframeid = pstLoad->ulcurcmdframeid;
    pstresmsg->stresinfo.endwnmachname   = pstLoad->endwnmachname;

    vos_u32 loopi = 0;
	static  vos_u32  countad = 0;
	countad += 200;
	if (countad > 10000)
	{
		countad = 0;
	}


    pstreport = (xc8002_com_report_cmd_stru*)&(pstresmsg->stresinfo.uscmdtype);
    pstreport->usstatustype = pstquery->queryType;
    switch (pstquery->queryType)
        {
            case xc8002_dwnmach_report_downmachin_versions://下位机版本上报
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_fpga_dwnmach_version_stru); 
                pstversion = (cmd_fpga_dwnmach_version_stru *)(pstreport);
                sprintf((char *)(pstversion->date_time), "%s", "2020-01-01 00:00:00");
                sprintf((char *)(pstversion->dwnmachin_ver), "%s", "B001");
                pstversion->fpga_ver = 123456;
                pstversion->dwnmachname = pstLoad->endwnmachname;
                break;
            case xc8002_dwnmach_report_LightBulbAD_type ://光源灯电压AD值上报
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_react_disk_lightgetval_stru);
                lightgetval = (cmd_react_disk_lightgetval_stru *)(pstreport);
                lightgetval->get_eeprom_lightintensity = 185;
                lightgetval->get_lamp_lightintensity = 180;
                break;
            case xc8002_dwnmach_report_fanstatus_type ://风扇运行状态
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_cooling_fan_status_stru);
                fan_status = (cmd_cooling_fan_status_stru *)(pstreport);
                for(loopi = 0; loopi < 8; loopi++)
                {
                    fan_status->status[loopi] = 1;
                }
                break;
            case xc8002_dwnmach_report_electricity_type ://制冷片电流
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_cooling_vol_stru);
                cooling_vol = (cmd_cooling_vol_stru *)(pstreport);
                for(loopi = 0; loopi < 16; loopi++)
                {
                    cooling_vol->vol[loopi] = 300;
                }
                break;
            case xc8002_dwnmach_report_fl_and_pre_type://浮子、压力查询
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_autowash_status_stru);
                autowash_status = (cmd_autowash_status_stru *)(pstreport); 
                for(loopi = 0; loopi < 7; loopi++)
                {
                    autowash_status->fl_status[loopi] = 1;  
                }
                for(loopi = 0; loopi < 3; loopi++)
                {
                    autowash_status->pres_ad[loopi] = 8500;
                }
                break;
            case xc8002_dwnmach_report_reagdsk_temp_query_type://制冷温度查询上报，非定时上报
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_cooling_temp_stru);
                cooling_temp = (cmd_cooling_temp_stru *)(pstreport);
                cooling_temp->temp_board = 3400;
                cooling_temp->temp_liqiud = 3300;
                break;
            case xc8002_dwnmach_report_thermal_control_temp_type://温控温度查询上报，非定时上报
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_temp_control_report_stru);
                tempreport = (cmd_temp_control_report_stru *)(pstreport);
                tempreport->temp_reac1 = 3501;               
                tempreport->temp_reac2 = 3502;
                tempreport->temp_reac3 = 3503;
                tempreport->temp_env = 3504;
                tempreport->temp_washmach = 3505;
                tempreport->temp_washcup = 3506;
                tempreport->temp_detergentcup = 3507;
                tempreport->duty_reaction = 3508;
                tempreport->duty_washmach = 3509;
                tempreport->duty_washcup = 3510;
                tempreport->duty_detergentcup = 3511;
                break;
			case	xc8002_dwnmach_report_ad_temp_type://温度AD值查询上报，非定时上报
				pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_temp_control_report_stru);
				tempreport = (cmd_temp_control_report_stru *)(pstreport);
				tempreport->temp_reac1 = 50000 + countad;
				tempreport->temp_reac2 = 0 + countad;
				tempreport->temp_reac3 = 43503 + countad;
				tempreport->temp_env = 33504+countad;
				tempreport->temp_washmach = 23505 + countad;
				tempreport->temp_washcup = 40000 + countad;
				tempreport->temp_detergentcup = 36000 + countad;
				tempreport->duty_reaction = 30000 + countad;
				tempreport->duty_washmach = 25000 + countad;
				tempreport->duty_washcup = 20000 + countad;
				tempreport->duty_detergentcup = 10000+countad;
				break;
            case xc8002_dwnmach_report_eeprom_info_type://eeprom存储信息上报
                if(xc8002_dwnmach_name_cooling_reag == pstLoad->endwnmachname)
                {
                    pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_temp_control_eeprom_info_stru);
                    tempeeprom = (cmd_temp_control_eeprom_info_stru *)(pstreport);
                    tempeeprom->tempreact = 3600;
                    tempeeprom->tempwash1 = 3500;
                    tempeeprom->tempwash2 = 3400;
                    tempeeprom->tempwash3 = 3300;
                    tempeeprom->dwnmachname = pstLoad->endwnmachname;
                }    
                else//TODO
                {
                    pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_temp_control_eeprom_info_stru);
                    tempeeprom = (cmd_temp_control_eeprom_info_stru *)(pstreport);
                    tempeeprom->dwnmachname = pstLoad->endwnmachname;
                }
                break;
            case xc8002_dwnmach_report_bblog_info_type:
            {
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(xc8002_report_dwnmachine_bblog)
                                                      + sizeof(xc8002_bblog_record);
                bblog = (xc8002_report_dwnmachine_bblog *)(pstreport);
                bblog->currpage = 35;
                bblog->recordnum = 15;

                for (loopi = 0; loopi < 15; loopi++)
                {
                    memcpy(bblog->records[loopi].record, "Hello world!", strlen("Hello world!"));
                }
            }
            case xc8001_dwnmach_report_ID:
            {
                pstresmsg->stresinfo.uscmdinfolen = 4 + sizeof(cmd_CPUID);
                cmd_CPUID*pPara = (cmd_CPUID*)pstreport;
                pPara->dwnmachname = pstLoad->endwnmachname;
                pPara->id[0] = 1;
                pPara->id[1] = 2;
                pPara->id[2] = 3;
                pPara->id[3] = 4;
                break;
            }
            default:
                break;
        }

        pstresmsg->stresinfo.uscmdtype       = XC8002_COM_REPORT_CMD;
        pstresmsg->stresinfo.enmsgtrantype   = xc8002_req_msgtran_type;

        ulrunrslt = vos_sendmsg((vos_msg_header_stru*)pstresmsg);
        
        if (VOS_OK != ulrunrslt)
        {
            WRITE_ERR_LOG(
                       "Call vos_sendmsg() failed(0x%x)\r\n",
                       ulrunrslt);
        }
    return ulrunrslt;
}

/*****************************************************************************
 函 数 名  : app_simudwnmachresponse
 功能描述  :用于测试模拟下位机响应消息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月10日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_simuDwnmachResponse(msg_stMidDwnCom* paurtpack)
{
    //模拟返回确认 transtype  = 1
    app_simuUartAckMsg(paurtpack);
    
    //模拟返回响应 transtype  = 3
    app_simuUartResMsg(paurtpack);
    
    //模拟查询数据返回
    app_simuDwnmachQueryData(paurtpack);

    return;
}

vos_u32 SendMsgToAppUart(char* pmsgbuf, vos_u32 ulbuflen)
{
	vos_u32 ulvosmsglen = ulbuflen;
    vos_u32 ulrunrslt   = VOS_OK;
    
    msg_stHead rHead;
    
    msg_stMnguart2Downmach* pstmsgbuf = VOS_NULL;
    xc8002_middwnmach_res_msgtran_stru*  pstcomcmd = VOS_NULL;

    pstcomcmd = (xc8002_middwnmach_res_msgtran_stru*)pmsgbuf;

    WRITE_INFO_LOG("Mnguartcomm: dwnmach= %u, trantype= %u, len= %u, cmd= 0x%x, rslt= %d \n",
              pstcomcmd->endwnmachname, pstcomcmd->enmsgtrantype, 
              pstcomcmd->uscmdinfolen,  pstcomcmd->uscmdtype,
              pstcomcmd->enexerslt);

    if (0 != (ulvosmsglen % 4))//消息四字节对齐
    {
        WRITE_INFO_LOG("UART : NOT 4 byte !!\n");
        ulvosmsglen += (4 - (ulvosmsglen % 4));
    }
    
    ulrunrslt = app_mallocBuffer(ulvosmsglen, (void**)(&pstmsgbuf));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_SimulatorTest, 
        app_res_dwnmach_act_cmd_msg_type);
    memcpy((char*)(&(pstmsgbuf->endwnmachname)), pmsgbuf, ulbuflen);
    
    WRITE_INFO_LOG("UART:uart -> dwnmach center \n");
    
    return app_sendMsg(&rHead, pstmsgbuf, ulvosmsglen);
}

//模拟暗电流测试Fpga Ad响应
vos_u32 app_simuFpgaAdResponse(xc8002_middwnmach_com_msgtran_stru *pfpgapack)
{
	xc8002_middwnmach_res_msgtran_stru stmsg;

    stmsg.endwnmachname     = pfpgapack->endwnmachname;

    SendMsgToAppUart((char *)&stmsg, sizeof(xc8002_middwnmach_res_msgtran_stru));
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : app_simuTestMsgCallBack
 功能描述  : 模拟测试模块回调函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  :    vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月14日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_simuTestMsgCallBack(vos_msg_header_stru* pstvosmsg)
{
//     vos_command_msg_stru* commandmsg = (vos_command_msg_stru*)pstvosmsg;
//     if (commandmsg->pcmdinfo.ulcommandid == app_cmdtype_res_testtask)
//     {
//         msg_stTaskRes* pTaskRes = (msg_stTaskRes*)(commandmsg->pcmdinfo.auccmdload);
// 
//         printf(" ulTaskId = %lu,period = %lu,Result = %lu\n", pTaskRes->ulTaskId, pTaskRes->ulPeriodNum, pTaskRes->enTaskResult);
// 
//         msg_stTaskPeriodResInfo * pWaveInfo = (msg_stTaskPeriodResInfo *)pTaskRes->aucPeriodInfo;
// 
//         printf("ulWaveNum=%lu\n", pWaveInfo->ulWaveNum);
// 
//         msg_stTaskWaveResInfo * ptaskWave = (msg_stTaskWaveResInfo *)pWaveInfo->aucWaveInfo;
// 
//         printf("usWaveValue = %d,enWaveAttr = %d\n", ptaskWave->usWaveValue, ptaskWave->enWaveAttr);
// 
//         vos_u32 i = 0;
//         for (; i < 41; ++i)
//         {
//             printf("%d ", ptaskWave->astPointInfo[i].usAdValue);
//         }
//         printf("\n");
// 
//         ++g_taskNum;
//     }
//     if (commandmsg->pcmdinfo.ulcommandid == app_monitor_reactdisk_query_status_answer)
//     {
//         msg_stReactDiskQueryStatusRes* pRes = (msg_stReactDiskQueryStatusRes*)commandmsg->pcmdinfo.auccmdload;
//         msg_stReactCupStatusInfo * pStatus = &(pRes->stRctCupStatusInfo[0]);
//         vos_u32 ulLoop= 0;
//         for (; ulLoop < REACT_CUP_NUM; ++ulLoop)
//         {
//             if (APP_INVALID_U32 != pStatus->ulTaskId)
//             {
//                 WRITE_INFO_LOG("%lu ,%lu ,%lu\n", pStatus->ulTaskId, pStatus->enRctCupStatus, pStatus->usReactTime);
//             }
//             if (APP_YES == pStatus->enHasAdValFlag)
//             {
//                 pStatus = (msg_stReactCupStatusInfo*)(((char*)pStatus) + sizeof(msg_stReactCupStatusInfo) + sizeof(msg_stRptAdVal));
//             }
//             else
//             {
//                 pStatus = (msg_stReactCupStatusInfo*)(((char*)pStatus) + sizeof(msg_stReactCupStatusInfo));
//             }
//         }
//     }
    return VOS_OK;
}

vos_u32 app_initcompensationpara(app_auto_compensation_req_stru *data)
{
    data->g_cpucps0[0].comptype = compensation_type_rtt;
    data->g_cpucps0[0].location = 0;
    data->g_cpucps0[0].dir = compensation_dir_clockwise;
    data->g_cpucps0[0].step = 80;
    data->g_cpucps0[1].comptype = compensation_type_rtt;
    data->g_cpucps0[1].location = 1;
    data->g_cpucps0[1].dir = compensation_dir_anticlockwise;
    data->g_cpucps0[1].step = 80;
    data->g_cpucps0[2].comptype = compensation_type_rtt;
    data->g_cpucps0[2].location = 2;
    data->g_cpucps0[2].dir = compensation_dir_clockwise;
    data->g_cpucps0[2].step = 114;
    data->g_cpucps0[3].comptype = compensation_type_rtt;
    data->g_cpucps0[3].location = 3;
    data->g_cpucps0[3].dir = compensation_dir_anticlockwise;
    data->g_cpucps0[3].step = 30;
    data->g_cpucps0[4].comptype = compensation_type_rtt;
    data->g_cpucps0[4].location = 4;
    data->g_cpucps0[4].dir = compensation_dir_clockwise;
    data->g_cpucps0[4].step = 104;
    data->g_cpucps0[5].comptype = compensation_type_rtt;
    data->g_cpucps0[5].location = 5;
    data->g_cpucps0[5].dir = compensation_dir_anticlockwise;
    data->g_cpucps0[5].step = 50;
    data->g_cpucps0[6].comptype = compensation_type_rtt;
    data->g_cpucps0[6].location = 6;
    data->g_cpucps0[6].dir = compensation_dir_clockwise;
    data->g_cpucps0[6].step = 30;
    data->g_cpucps0[7].comptype = compensation_type_rtt;
    data->g_cpucps0[7].location = 7;
    data->g_cpucps0[7].dir = compensation_dir_anticlockwise;
    data->g_cpucps0[7].step = 114;
    data->g_cpucps0[8].comptype = compensation_type_rtt;
    data->g_cpucps0[8].location = 8;
    data->g_cpucps0[8].dir = compensation_dir_clockwise;
    data->g_cpucps0[8].step = 30;  
    data->g_cpucps0[9].comptype = compensation_type_rtt;
    data->g_cpucps0[9].location = 9;
    data->g_cpucps0[9].dir = compensation_dir_anticlockwise;
    data->g_cpucps0[9].step = 104;
    data->g_cpucps0[10].comptype = compensation_type_reset;
    data->g_cpucps0[10].location = 0;
    data->g_cpucps0[10].dir = compensation_dir_clockwise;
    data->g_cpucps0[10].step = 3920;

    data->g_cpucps1[0].comptype = compensation_type_rtt;
    data->g_cpucps1[0].location = 0;
    data->g_cpucps1[0].dir = compensation_dir_clockwise;
    data->g_cpucps1[0].step = 1;
    data->g_cpucps1[1].comptype = compensation_type_rtt;
    data->g_cpucps1[1].location = 1;
    data->g_cpucps1[1].dir = compensation_dir_clockwise;
    data->g_cpucps1[1].step = 5;
    data->g_cpucps1[2].comptype = compensation_type_rtt;
    data->g_cpucps1[2].location = 2;
    data->g_cpucps1[2].dir = compensation_dir_clockwise;
    data->g_cpucps1[2].step = 75;
    data->g_cpucps1[3].comptype = compensation_type_rtt;
    data->g_cpucps1[3].location = 3;
    data->g_cpucps1[3].dir = compensation_dir_clockwise;
    data->g_cpucps1[3].step = 5;
    data->g_cpucps1[4].comptype = compensation_type_rtt;
    data->g_cpucps1[4].location = 4;
    data->g_cpucps1[4].dir = compensation_dir_clockwise;
    data->g_cpucps1[4].step = 30;
    data->g_cpucps1[5].comptype = compensation_type_rtt;
    data->g_cpucps1[5].location = 5;
    data->g_cpucps1[5].dir = compensation_dir_clockwise;
    data->g_cpucps1[5].step = 60;
    data->g_cpucps1[6].comptype = compensation_type_rtt;
    data->g_cpucps1[6].location = 6;
    data->g_cpucps1[6].dir = compensation_dir_clockwise;
    data->g_cpucps1[6].step = 60;
    data->g_cpucps1[7].comptype = compensation_type_rtt;
    data->g_cpucps1[7].location = 7;
    data->g_cpucps1[7].dir = compensation_dir_clockwise;
    data->g_cpucps1[7].step = 60;
    data->g_cpucps1[8].comptype = compensation_type_rtt;
    data->g_cpucps1[8].location = 8;
    data->g_cpucps1[8].dir = compensation_dir_clockwise;
    data->g_cpucps1[8].step = 60;
    data->g_cpucps1[9].comptype = compensation_type_rtt;
    data->g_cpucps1[9].location = 9;
    data->g_cpucps1[9].dir = compensation_dir_clockwise;
    data->g_cpucps1[9].step = 30;
    data->g_cpucps1[10].comptype = compensation_type_rtt;
    data->g_cpucps1[10].location = 10;
    data->g_cpucps1[10].dir = compensation_dir_clockwise;
    data->g_cpucps1[10].step = 20;
    data->g_cpucps1[11].comptype = compensation_type_rtt;
    data->g_cpucps1[11].location = 11;
    data->g_cpucps1[11].dir = compensation_dir_clockwise;
    data->g_cpucps1[11].step = 8;
    data->g_cpucps1[12].comptype = compensation_type_rtt;
    data->g_cpucps1[12].location = 12;
    data->g_cpucps1[12].dir = compensation_dir_anticlockwise;
    data->g_cpucps1[12].step = 50;
    data->g_cpucps1[13].comptype = compensation_type_rtt;
    data->g_cpucps1[13].location = 13;
    data->g_cpucps1[13].dir = compensation_dir_anticlockwise;
    data->g_cpucps1[13].step = 50;
    data->g_cpucps1[14].comptype = compensation_type_rtt;
    data->g_cpucps1[14].location = 14;
    data->g_cpucps1[14].dir = compensation_dir_down;
    data->g_cpucps1[14].step = 0;
    data->g_cpucps1[15].comptype = compensation_type_rtt;
    data->g_cpucps1[15].location = 15;
    data->g_cpucps1[15].dir = compensation_dir_down;
    data->g_cpucps1[15].step = 80;
    data->g_cpucps1[16].comptype = compensation_type_rtt;
    data->g_cpucps1[16].location = 16;
    data->g_cpucps1[16].dir = compensation_dir_down;
    data->g_cpucps1[16].step = 128;
    data->g_cpucps1[17].comptype = compensation_type_reset;
    data->g_cpucps1[17].location = 0;
    data->g_cpucps1[17].dir = compensation_dir_clockwise;
    data->g_cpucps1[17].step = 0;

    data->g_cpucps2[0].comptype = compensation_type_rtt;
    data->g_cpucps2[0].location = 0;
    data->g_cpucps2[0].dir = compensation_dir_clockwise;
    data->g_cpucps2[0].step = 470;
    data->g_cpucps2[1].comptype = compensation_type_rtt;
    data->g_cpucps2[1].location = 1;
    data->g_cpucps2[1].dir = compensation_dir_anticlockwise;
    data->g_cpucps2[1].step = 290;
    data->g_cpucps2[2].comptype = compensation_type_rtt;
    data->g_cpucps2[2].location = 2;
    data->g_cpucps2[2].dir = compensation_dir_clockwise;
    data->g_cpucps2[2].step = 490;
    data->g_cpucps2[3].comptype = compensation_type_rtt;
    data->g_cpucps2[3].location = 3;
    data->g_cpucps2[3].dir = compensation_dir_anticlockwise;
    data->g_cpucps2[3].step = 200;
    data->g_cpucps2[4].comptype = compensation_type_reset;
    data->g_cpucps2[4].location = 0;
    data->g_cpucps2[4].dir = compensation_dir_anticlockwise;
    data->g_cpucps2[4].step = 80;
    data->g_cpucps2[5].comptype = compensation_type_reset;
    data->g_cpucps2[5].location = 1;
    data->g_cpucps2[5].dir = compensation_dir_clockwise;
    data->g_cpucps2[5].step = 7;

    data->g_cpucps3[0].comptype = compensation_type_rtt;
    data->g_cpucps3[0].location = 0;
    data->g_cpucps3[0].dir = compensation_dir_clockwise;
    data->g_cpucps3[0].step = 2;
    data->g_cpucps3[1].comptype = compensation_type_rtt;
    data->g_cpucps3[1].location = 1;
    data->g_cpucps3[1].dir = compensation_dir_clockwise;
    data->g_cpucps3[1].step = 10;
    data->g_cpucps3[2].comptype = compensation_type_rtt;
    data->g_cpucps3[2].location = 2;
    data->g_cpucps3[2].dir = compensation_dir_clockwise;
    data->g_cpucps3[2].step = 2;
    data->g_cpucps3[3].comptype = compensation_type_rtt;
    data->g_cpucps3[3].location = 3;
    data->g_cpucps3[3].dir = compensation_dir_clockwise;
    data->g_cpucps3[3].step = 47;
    data->g_cpucps3[4].comptype = compensation_type_rtt;
    data->g_cpucps3[4].location = 4;
    data->g_cpucps3[4].dir = compensation_dir_anticlockwise;
    data->g_cpucps3[4].step = 100;
    data->g_cpucps3[5].comptype = compensation_type_rtt;
    data->g_cpucps3[5].location = 5;
    data->g_cpucps3[5].dir = compensation_dir_anticlockwise;
    data->g_cpucps3[5].step = 15;
    data->g_cpucps3[6].comptype = compensation_type_rtt;
    data->g_cpucps3[6].location = 6;
    data->g_cpucps3[6].dir = compensation_dir_clockwise;
    data->g_cpucps3[6].step = 70;
    data->g_cpucps3[7].comptype = compensation_type_rtt;
    data->g_cpucps3[7].location = 7;
    data->g_cpucps3[7].dir = compensation_dir_anticlockwise;
    data->g_cpucps3[7].step = 150;
    data->g_cpucps3[8].comptype = compensation_type_rtt;
    data->g_cpucps3[8].location = 8;
    data->g_cpucps3[8].dir = compensation_dir_anticlockwise;
    data->g_cpucps3[8].step = 150;
    data->g_cpucps3[9].comptype = compensation_type_rtt;
    data->g_cpucps3[9].location = 9;
    data->g_cpucps3[9].dir = compensation_dir_down;
    data->g_cpucps3[9].step = 0;
    data->g_cpucps3[10].comptype = compensation_type_rtt;
    data->g_cpucps3[10].location = 10;
    data->g_cpucps3[10].dir = compensation_dir_down;
    data->g_cpucps3[10].step = 0;
    data->g_cpucps3[11].comptype = compensation_type_rtt;
    data->g_cpucps3[11].location = 11;
    data->g_cpucps3[11].dir = compensation_dir_down;
    data->g_cpucps3[11].step = 128;
    data->g_cpucps3[12].comptype = compensation_type_rtt;
    data->g_cpucps3[12].location = 12;
    data->g_cpucps3[12].dir = compensation_dir_down;
    data->g_cpucps3[12].step = 128;
    data->g_cpucps3[13].comptype = compensation_type_reset;
    data->g_cpucps3[13].location = 0;
    data->g_cpucps3[13].dir = compensation_dir_clockwise;
    data->g_cpucps3[13].step = 0;

    data->g_cpucps4[0].comptype = compensation_type_rtt;
    data->g_cpucps4[0].location = 0;
    data->g_cpucps4[0].dir = compensation_dir_clockwise;
    data->g_cpucps4[0].step = 80;
    data->g_cpucps4[1].comptype = compensation_type_rtt;
    data->g_cpucps4[1].location = 1;
    data->g_cpucps4[1].dir = compensation_dir_anticlockwise;
    data->g_cpucps4[1].step = 365;
    data->g_cpucps4[2].comptype = compensation_type_rtt;
    data->g_cpucps4[2].location = 2;
    data->g_cpucps4[2].dir = compensation_dir_clockwise;
    data->g_cpucps4[2].step = 290;
    data->g_cpucps4[3].comptype = compensation_type_rtt;
    data->g_cpucps4[3].location = 3;
    data->g_cpucps4[3].dir = compensation_dir_anticlockwise;
    data->g_cpucps4[3].step = 160;
    data->g_cpucps4[4].comptype = compensation_type_reset;
    data->g_cpucps4[4].location = 0;
    data->g_cpucps4[4].dir = compensation_dir_anticlockwise;
    data->g_cpucps4[4].step = 350;

    data->g_cpucps5[0].comptype = compensation_type_rtt;
    data->g_cpucps5[0].location = 0;
    data->g_cpucps5[0].dir = compensation_dir_clockwise;
    data->g_cpucps5[0].step = 10;
    data->g_cpucps5[1].comptype = compensation_type_rtt;
    data->g_cpucps5[1].location = 1;
    data->g_cpucps5[1].dir = compensation_dir_clockwise;
    data->g_cpucps5[1].step = 2;
    data->g_cpucps5[2].comptype = compensation_type_rtt;
    data->g_cpucps5[2].location = 2;
    data->g_cpucps5[2].dir = compensation_dir_clockwise;
    data->g_cpucps5[2].step = 2;
    data->g_cpucps5[3].comptype = compensation_type_rtt;
    data->g_cpucps5[3].location = 3;
    data->g_cpucps5[3].dir = compensation_dir_clockwise;
    data->g_cpucps5[3].step = 20;
    data->g_cpucps5[4].comptype = compensation_type_rtt;
    data->g_cpucps5[4].location = 4;
    data->g_cpucps5[4].dir = compensation_dir_anticlockwise;
    data->g_cpucps5[4].step = 67;
    data->g_cpucps5[5].comptype = compensation_type_rtt;
    data->g_cpucps5[5].location = 5;
    data->g_cpucps5[5].dir = compensation_dir_clockwise;
    data->g_cpucps5[5].step = 15;
    data->g_cpucps5[6].comptype = compensation_type_rtt;
    data->g_cpucps5[6].location = 6;
    data->g_cpucps5[6].dir = compensation_dir_clockwise;
    data->g_cpucps5[6].step = 50;
    data->g_cpucps5[7].comptype = compensation_type_rtt;
    data->g_cpucps5[7].location = 7;
    data->g_cpucps5[7].dir = compensation_dir_anticlockwise;
    data->g_cpucps5[7].step = 150;
    data->g_cpucps5[8].comptype = compensation_type_rtt;
    data->g_cpucps5[8].location = 8;
    data->g_cpucps5[8].dir = compensation_dir_anticlockwise;
    data->g_cpucps5[8].step = 150;
    data->g_cpucps5[9].comptype = compensation_type_rtt;
    data->g_cpucps5[9].location = 9;
    data->g_cpucps5[9].dir = compensation_dir_down;
    data->g_cpucps5[9].step = 0;
    data->g_cpucps5[10].comptype = compensation_type_rtt;
    data->g_cpucps5[10].location = 10;
    data->g_cpucps5[10].dir = compensation_dir_down;
    data->g_cpucps5[10].step = 0;
    data->g_cpucps5[11].comptype = compensation_type_rtt;
    data->g_cpucps5[11].location = 11;
    data->g_cpucps5[11].dir = compensation_dir_down;
    data->g_cpucps5[11].step = 128;
    data->g_cpucps5[12].comptype = compensation_type_rtt;
    data->g_cpucps5[12].location = 12;
    data->g_cpucps5[12].dir = compensation_dir_down;
    data->g_cpucps5[12].step = 128;
    data->g_cpucps5[13].comptype = compensation_type_reset;
    data->g_cpucps5[13].location = 0;
    data->g_cpucps5[13].dir = compensation_dir_anticlockwise;
    data->g_cpucps5[13].step = 1;

    data->g_cpucps6[0].comptype = compensation_type_rtt;
    data->g_cpucps6[0].location = 0;
    data->g_cpucps6[0].dir = compensation_dir_clockwise;
    data->g_cpucps6[0].step = 120;
    data->g_cpucps6[1].comptype = compensation_type_rtt;
    data->g_cpucps6[1].location = 1;
    data->g_cpucps6[1].dir = compensation_dir_anticlockwise;
    data->g_cpucps6[1].step = 105;
	data->g_cpucps6[2].comptype = compensation_type_rtt;
	data->g_cpucps6[2].location = 2;
	data->g_cpucps6[2].dir = compensation_dir_clockwise;
	data->g_cpucps6[2].step = 25;
	data->g_cpucps6[3].comptype = compensation_type_reset;
	data->g_cpucps6[3].location = 0;
	data->g_cpucps6[3].dir = compensation_dir_anticlockwise;
	data->g_cpucps6[3].step = 3;
	data->g_cpucps6[4].comptype = compensation_type_reset;
	data->g_cpucps6[4].location = 1;
	data->g_cpucps6[4].dir = compensation_dir_clockwise;
	data->g_cpucps6[4].step = 178;

    data->g_cpucps8[0].comptype = compensation_type_rtt;
    data->g_cpucps8[0].location = 0;
    data->g_cpucps8[0].dir = compensation_dir_clockwise;
    data->g_cpucps8[0].step = 20; 
    data->g_cpucps8[1].comptype = compensation_type_rtt;
    data->g_cpucps8[1].location = 1;
    data->g_cpucps8[1].dir = compensation_dir_clockwise;
    data->g_cpucps8[1].step = 53;
    data->g_cpucps8[2].comptype = compensation_type_rtt;
    data->g_cpucps8[2].location = 2;
    data->g_cpucps8[2].dir = compensation_dir_up;
    data->g_cpucps8[2].step = 50;
    data->g_cpucps8[3].comptype = compensation_type_rtt;
    data->g_cpucps8[3].location = 3;
    data->g_cpucps8[3].dir = compensation_dir_up;
    data->g_cpucps8[3].step = 0;
    data->g_cpucps8[4].comptype = compensation_type_rtt;
    data->g_cpucps8[4].location = 4;
    data->g_cpucps8[4].dir = compensation_dir_up;
    data->g_cpucps8[4].step = 0;
    data->g_cpucps8[5].comptype = compensation_type_reset;
    data->g_cpucps8[5].location = 0;
    data->g_cpucps8[5].dir = compensation_dir_clockwise;
    data->g_cpucps8[5].step = 40; 

    data->g_cpucps9[0].comptype = compensation_type_rtt;
    data->g_cpucps9[0].location = 0;
    data->g_cpucps9[0].dir = compensation_dir_clockwise;
    data->g_cpucps9[0].step = 40;
    data->g_cpucps9[1].comptype = compensation_type_rtt;
    data->g_cpucps9[1].location = 1;
    data->g_cpucps9[1].dir = compensation_dir_clockwise;
    data->g_cpucps9[1].step = 2;
    data->g_cpucps9[2].comptype = compensation_type_rtt;
    data->g_cpucps9[2].location = 2;
    data->g_cpucps9[2].dir = compensation_dir_up;
    data->g_cpucps9[2].step = 0;
    data->g_cpucps9[3].comptype = compensation_type_rtt;
    data->g_cpucps9[3].location = 3;
    data->g_cpucps9[3].dir = compensation_dir_up;
    data->g_cpucps9[3].step = 0;
    data->g_cpucps9[4].comptype = compensation_type_reset;
    data->g_cpucps9[4].location = 0;
    data->g_cpucps9[4].dir = compensation_dir_clockwise;
    data->g_cpucps9[4].step = 50;
	
	
	data->g_cpucps7[0].comptype = compensation_type_reset;
	data->g_cpucps7[0].location = 0;
	data->g_cpucps7[0].dir = compensation_dir_clockwise;
	data->g_cpucps7[0].step = 1;
	data->g_cpucps7[1].comptype = compensation_type_rtt;
	data->g_cpucps7[1].location = 0;
	data->g_cpucps7[1].dir = compensation_dir_clockwise;
	data->g_cpucps7[1].step = 20;
	return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_initSimuTest
 功能描述  : 初始化模拟测试模块
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月14日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_initSimuTest(void)
{

    vos_u32 ulrunrslt = VOS_OK;
    //注册PID
    ulrunrslt = vos_registerpid(app_auto_test_fid, Pid_SimulatorTest, app_simuTestMsgCallBack);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_registerpid failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

 
    //测试用例初始化
    app_initTestCase();

    #ifdef COMPILER_IS_LINUX_GCC
    vos_u32 ulmemsize = sizeof(app_auto_compensation_req_stru);
    if (VOS_OK != vos_mallocmemory(ulmemsize, vos_mem_type_static, (void **)(&g_pcpsautoreq)))
    {
        WRITE_ERR_LOG("Call vos_mallocmemory() failed(0x%x)\r\n", (int)ulrunrslt);
        return ulrunrslt;
    }
    app_initcompensationpara(g_pcpsautoreq);
    #endif
    
    return ulrunrslt;
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

