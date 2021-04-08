#include "typedefine.pb.h"
#include "upmsgstruct.pb.h"
#include "darkcurrent.h"
#include "msg.h"
#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include "app_xc8002shareinfo.h"
#include "app_msgtypedefine.h"
#include "app_msg.h"
#include <stdio.h>
#include "app_errorcode.h"
#include "vos_timer.h"
#include "apputils.h"
#include "drv_ebi.h"
#include "inituart.h"
#include "vos_adaptsys.h"
#include "socketmonitor.h"
#include <sys/ioctl.h>
#include "xc8002_middwnmachshare.h"
#include "vos_log.h"
#include "inituart.h"

extern vos_u32 g_ultestmode;
extern map<app_u16, app_u16> g_uartHdl;
extern map<app_u16, app_u16> m_appWaveMap;
extern map<app_u16, app_u16> m_dpWaveMap;

CDarkCurrent::CDarkCurrent(vos_u32 ulPid) : CCallBack(ulPid)
{
	m_testState = STATE_TEST_IDLE;
	//创建执行动作超时定时器
	m_timerId = createTimer(SPid_DarkCurrent, "DarkCurrent",DARKCURRENT_TIME_LEN);
}

vos_u32 CDarkCurrent::_controlLight(vos_u32 intensity)
{
    return 0;
    msg_stLightControl stPara;
    stPara.lightIntensity = intensity;
    return app_reqDwmCmd(SPid_DarkCurrent, SPid_ReactDsk, Act_Light, sizeof(stPara), (char*)&stPara, APP_YES);
// 	WRITE_INFO_LOG("Enter _controlLight.\r\n");
// 	vos_u32 ulrunrslt = VOS_OK;
// 	vos_u32 ulmsglength = 0;
//     msg_stHead rHead;
// 
// 	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
//     msg_stLightControl *pdstrst = VOS_NULL;
// 
// 	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(msg_stLightControl);   
// 
// 	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
// 	if (VOS_OK != ulrunrslt)
//     {
//         WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
//         return ulrunrslt;
//     }
// 
// 	app_constructMsgHead(&rHead, SPid_ReactDsk, SPid_DarkCurrent, app_com_device_req_exe_act_msg_type);
// 	//填数据内容
//     pstdwnmachreqmsg->usActNum = 1;
//     pstdwnmachreqmsg->stActHead.usActType = Act_Light;
// 	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
//     pstdwnmachreqmsg->ulDataLen = sizeof(msg_stLightControl) + sizeof(msg_stActHead) ;
// 
// 	pdstrst = (msg_stLightControl*)(&(pstdwnmachreqmsg->astActInfo[0]));
// 	pdstrst->lightIntensity = intensity;
//     
//     return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);
}

vos_u32 CDarkCurrent::_procDarkCurrentReq(vos_msg_header_stru * pReqMsg)
{
    if (!g_pMonitor->CanHandleCmd())
    {
        WRITE_ERR_LOG("DarkCurrent failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }
	WRITE_INFO_LOG("Enter _procDarkCurrentReq.\r\n");
	m_srcPid = pReqMsg->ulsrcpid;
	
	CMsg stMsg;
    stMsg.ParseMsg(pReqMsg);
	STReqDarkCurrentTest*  pReq = dynamic_cast<STReqDarkCurrentTest*>(stMsg.pPara);
    m_stTestResResult.set_testtype(pReq->testtype());
	
	m_testCount = pReq->testcount();
	WRITE_INFO_LOG("Enter _procDarkCurrentReq,m_testCount = %d.\r\n",m_testCount);

	if(pReq->dpvalueinfo_size() != WAVE_NUM)
	{
		WRITE_ERR_LOG("Dpvalue count err,expect 14,but the real count  = %d.\r\n",pReq->dpvalueinfo_size());
	}

	vos_u32 dpValue[WAVE_NUM] = {0};
	for(int i=0 ; i < WAVE_NUM ; i++)
	{
		STKeyValue * pVal = pReq->mutable_dpvalueinfo(i);
		dpValue[i] = (pVal->uival());
	}
// 	setDpValue(dpValue);
// 	writeDpValueToFile(dpValue);

	app_reportmidmachstatus(vos_midmachine_state_darkcurrent);
	m_testState = STATE_CLOSE_LIGHT;
	_controlLight(LIGHTINTENSITY_OFF);//关光源灯

#ifdef COMPILER_IS_LINUX_GCC
	m_stTestResResult.set_testtype(1);

	int cupNum = 1;
	for (int i = 0; i < cupNum; i++)
	{
		STDarkCurrentAD  *pstDark = m_stTestResResult.add_darkcurrentadinfo();
		for (int j = 0; j < 14; j++)
		{
			STKeyValue  *pstValue = pstDark->add_stadinfo();
			pstValue->set_uikey(j);
			pstValue->set_uival(125 + j);
		}
	}
	_sendToUpmach();
	app_reportmidmachstatus(vos_midmachine_state_standby);
#endif

	return VOS_OK;
}

vos_u32 CDarkCurrent::_enDarkCurrentTest()
{
	WRITE_INFO_LOG("Enter _enDarkCurrentTest.\r\n");

	g_ultestmode = TESTMODE_DARKCURRENT;
    app_reqDwmCmd(SPid_DarkCurrent, SPid_ADInner, Act_OneAD, 0, NULL, APP_NO);
    app_reqDwmCmd(SPid_DarkCurrent, SPid_ADOuter, Act_OneAD, 0, NULL, APP_NO);
	return VOS_OK;
}

vos_u32 CDarkCurrent::_procCloseLightRes()
{
	startTimer(m_timerId,WAIT_LIGHT_CLOSE_TIME_LEN);
	
	return VOS_OK;
}

vos_u32 CDarkCurrent::_procOpenLightRes()
{
	WRITE_INFO_LOG("Test finished.\r\n");
	app_reportmidmachstatus(vos_midmachine_state_standby);
	return VOS_OK;
}

vos_u32 CDarkCurrent::_procDwnActRes(vos_msg_header_stru * pResMsg)
{
	if(STATE_DARKCURRENT_TEST_OVER == m_testState)
	{
		return VOS_OK;
	}
	
	switch(m_testState)
	{
		case STATE_CLOSE_LIGHT:
			_procCloseLightRes();
			break;

		case STATE_OPEN_LIGHT:
			_procOpenLightRes();
			break;

		default:
			WRITE_ERR_LOG("Unknown state(%d) \n", m_testState);
			break;
	}
	return VOS_OK;
}

vos_u32 CDarkCurrent::_sendToUpmach()
{
    if (m_stTestResResult.darkcurrentadinfo_size() > 0)
    {
        sendUpMsg(m_srcPid, SPid_DarkCurrent, MSG_ResDarkCurrent, &m_stTestResResult);
        m_stTestResResult.clear_darkcurrentadinfo();
    }
	return VOS_OK;
}

vos_u32 CDarkCurrent::_timeDelay(vos_u32 timeLen)
{
	vos_threadsleep(timeLen);
	return VOS_OK;
}

vos_u32 CDarkCurrent::_procAdRes(vos_msg_header_stru * pResMsg)
{
	WRITE_INFO_LOG("Enter _procAdRes.\r\n");

	if(STATE_DARKCURRENT_TEST_OVER == m_testState)
	{
		return VOS_OK;
	}
	
	//从共享内存中取数据
    vos_u32 collectCupNumOnce = 0;
    vos_u32 collectAdValueOnce[WAVE_NUM] = {0};
	vos_u32 collectAdKey[WAVE_NUM] = {app_rscsch_wave_340nm_id,app_rscsch_wave_405nm_id,app_rscsch_wave_450nm_id,app_rscsch_wave_478nm_id,
		app_rscsch_wave_505nm_id,app_rscsch_wave_542nm_id,app_rscsch_wave_570nm_id,app_rscsch_wave_605nm_id,app_rscsch_wave_630nm_id,
		app_rscsch_wave_660nm_id,app_rscsch_wave_700nm_id,app_rscsch_wave_750nm_id,app_rscsch_wave_805nm_id,app_rscsch_wave_850nm_id};

	GetAbsValueFromShareMem(&collectCupNumOnce, collectAdValueOnce, sizeof(collectAdValueOnce));
	
    m_stTestResResult.Clear();
	STDarkCurrentAD * pCupInfo = m_stTestResResult.add_darkcurrentadinfo();
	STKeyValue * pAdInfo = NULL;	
    msg_ADValue* pAdValue = (msg_ADValue*)pResMsg->aucmsgload;
    app_u32* pValue = pAdValue->value;

    m_stTestResResult.set_testtype(pAdValue->dwnName);

    for (int i = 0; i < pAdValue->AdCount; i++)
	{
		pAdInfo = pCupInfo->add_stadinfo();
		pAdInfo->set_uikey(collectAdKey[i]);
        pAdInfo->set_uival(*pValue);
        pValue++;
	}
	
	m_testCount--;
	if(m_testCount <= 0)
    {
    	_sendToUpmach();
		m_testState = STATE_OPEN_LIGHT;
		_controlLight(LIGHTINTENSITY_ON);//开光源灯
		return VOS_OK;
    }

	_timeDelay(DELAY_TIME_LEN_100MS);
	_enDarkCurrentTest();
	
	return VOS_OK;
}

vos_u32 CDarkCurrent::_procStopReq()
{
	_sendToUpmach();
	m_testState = STATE_DARKCURRENT_TEST_OVER;
	_controlLight(LIGHTINTENSITY_ON);//开光源灯
	return VOS_OK;
}

vos_u32 CDarkCurrent::_procTimeOut(vos_msg_header_stru* pResMsg)
{
	vos_stoptimer(m_timerId);

	_enDarkCurrentTest();

	return VOS_OK;
}

vos_u32 CDarkCurrent::CallBack(vos_msg_header_stru* pMsg)
{
	if (VOS_NULL == pMsg)
    {
        WRITE_ERR_LOG("Input parameter is null \n");
        return app_para_null_err;
    }

    vos_u32 ulResult = VOS_OK;
    WRITE_INFO_LOG("curpid=%u Recevied msg(0x%x) from pid(%u)\n",SPid_DarkCurrent,pMsg->usmsgtype, pMsg->ulsrcpid);
    switch (pMsg->usmsgtype)
    {
        case MSG_ReqDarkCurrent:
            ulResult = _procDarkCurrentReq(pMsg);
            break;
			
		case app_com_device_res_exe_act_msg_type:
            ulResult = _procDwnActRes(pMsg);
            break;
			
		case app_com_ad_data_ready_notice_msg_type:
            ulResult = _procAdRes(pMsg);
            break;

		case app_req_stop_midmach_msg_type:
			_procStopReq();
			break;

		case vos_pidmsgtype_timer_timeout:
            ulResult = _procTimeOut(pMsg);
            break;

		case MSG_ReqReadDarkCurrent:
			ulResult = _prodpValue(pMsg);
			break;


        default:
            WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
            break;
    }
    return ulResult;
}

vos_u32 CDarkCurrent::_prodpValue(vos_msg_header_stru * pResMsg)
{
	m_srcPid = pResMsg->ulsrcpid;
	vos_u32 dpValue[WAVE_NUM] = { 0 };
	readDpValueFromFile(dpValue);
	STDarkCurrentAD   stResult;
	for (int i = 0; i < WAVE_NUM; i++)
	{
		STKeyValue  *pValue = stResult.add_stadinfo();
		pValue->set_uikey(i);
		pValue->set_uival(dpValue[i]);
	}
	sendUpMsg(m_srcPid, SPid_DarkCurrent, MSG_ResReadDarkCurrent, &stResult);
	return VOS_OK;
}
