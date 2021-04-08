#include "typedefine.pb.h"
#include "upmsgstruct.pb.h"
#include "bcscan.h"
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
#include "socketmonitor.h"
#include "vos_log.h"
extern int smpBarcode;
CBcScan::CBcScan(vos_u32 ulPid) : CCallBack(ulPid)
{
	m_testState = STATE_BCSCAN_IDLE;
	//创建执行动作超时定时器
	m_reagTimer = createTimer(SPid_BcScan, "REAGDSK_BCSCAN",BCSCAN_TIME_LEN);
	m_smpTimer = createTimer(SPid_BcScan, "SMPDSK_BC",BCSCAN_TIME_LEN);
}

vos_u32 CBcScan::_procBcScanReq(vos_msg_header_stru * pReqMsg)
{
    if (!g_pMonitor->CanBCScan())
    {
        WRITE_ERR_LOG("BcScan failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }
	m_SrcPid = pReqMsg->ulsrcpid;
	
	CMsg stMsg;
    stMsg.ParseMsg(pReqMsg);
	STReqBarcodeScan*  pReq = dynamic_cast<STReqBarcodeScan*>(stMsg.pPara);

	for(int i=0;i<pReq->streagcup_size();++i)
	{
		WRITE_INFO_LOG("m_reagCupInfo.push! \n");
		m_reagCupInfo.push_back(pReq->streagcup(i));
	}

	for(int i=0;i<pReq->stsmpcup_size();++i)
	{
        smpBarcode = 0;
		WRITE_INFO_LOG("m_smpCupInfo.push! \n");
		m_smpCupInfo.push_back(pReq->stsmpcup(i));
	}

	m_runState = getCurRunState();
	app_reportmidmachstatus(vos_midmachine_state_bcscan);

	m_testState = STATE_BCSCAN_RESET_NDL;


	bool  hasR1 = false;
	bool  hasR2 = false;
	for (vector<STCupPos> ::iterator iter = m_reagCupInfo.begin(); iter != m_reagCupInfo.end(); iter++)
	{
		if (R1Ndl == iter->uidskid())
		{
			hasR1 = true;
		}
		else if (R2Ndl == iter->uidskid())
		{
			hasR2 = true;
		}
	}
	m_resetNum =0;
	if (!m_reagCupInfo.empty())
	{
		if (hasR1)
		{
			m_resetNum += 1;
			app_reqReset(SPid_BcScan, SPid_R1, APP_YES);
		}
	    if (hasR2)
		{
			m_resetNum += 1;
			app_reqReset(SPid_BcScan, SPid_R2, APP_YES);
		}
	}

	if (!m_smpCupInfo.empty())
	{
		m_resetNum += 1;
		app_reqReset(SPid_BcScan, SPid_SmpNdl, APP_YES);
	}
	return VOS_OK;
}

vos_u32 CBcScan::_reagDiskRtt(vos_u32 uiDskId)
{
	vos_u32 dstPid;
	vos_u32 uiCupId;
	if(uiDskId == app_xc8002_reagent_inner_disk_id)//扫描内盘时，外盘需要转到扫描位
	{
		dstPid = SPid_R1Dsk;
		uiCupId = OUTERDSK_BCSCAN_POS;
	}
	if(uiDskId == app_xc8002_reagent_outer_disk_id)//扫描外盘时，内盘需要转到清洗液位
	{
		dstPid = SPid_R2Dsk;
		uiCupId = INNERDSK_BCSCAN_POS;
	}
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    act_stReagSysDskRtt *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(act_stReagSysDskRtt);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, dstPid, SPid_BcScan, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = Act_Move;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
    pstdwnmachreqmsg->ulDataLen = sizeof(act_stReagSysDskRtt) + sizeof(msg_stActHead) ;

	pdstrst = (act_stReagSysDskRtt*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->usCupId = uiCupId;
    
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);
}

vos_u32 CBcScan::_exeSmpBcScan(vos_u32 uiDskId,vos_u32 uiCupId)
{
	WRITE_INFO_LOG("Enter _exeSmpBcScan! \n");
	
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    msg_stSmpBcScanPara *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(msg_stSmpBcScanPara);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, SPid_SmpDsk, SPid_BcScan, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = Act_BcScan;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
    pstdwnmachreqmsg->ulDataLen = sizeof(msg_stSmpBcScanPara) + sizeof(msg_stActHead);

	pdstrst = (msg_stSmpBcScanPara*)(&(pstdwnmachreqmsg->astActInfo[0]));
    DiskPosToCup(uiCupId, pdstrst->uccircleid, pdstrst->uccupid);  
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);;
}

vos_u32 CBcScan::_exeReagBcScan(vos_u32 uiDskId,vos_u32 uiCupId)
{
	vos_u32 dstPid;
	if(uiDskId == app_xc8002_reagent_inner_disk_id)//扫描内盘时，外盘需要转到扫描位
	{
		dstPid = SPid_R2Dsk;
	}
	if(uiDskId == app_xc8002_reagent_outer_disk_id)//扫描外盘时，内盘需要转到清洗液位
	{
		dstPid = SPid_R1Dsk;
	}
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    msg_stReagBcScanPara *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(msg_stReagBcScanPara);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, dstPid, SPid_BcScan, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = Act_BcScan;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
    pstdwnmachreqmsg->ulDataLen = sizeof(msg_stReagBcScanPara) + sizeof(msg_stActHead) ;

	pdstrst = (msg_stReagBcScanPara*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->usCupId = uiCupId;
    
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);
}

vos_u32 CBcScan::_procReagBcScanRes(app_dwn_barcode_tlv_stru* pBcInfo)
{
	//缓存结果
	STBarcodeScan * pResult = m_stScanResult.add_streaginfo();
    STCupPos * pCupPos = pResult->mutable_stcuppos();
	STCupPos ReagPos;

	if(!m_reagCupInfo.empty())
	{
		vector<STCupPos> ::iterator iter;
		iter = m_reagCupInfo.begin();
		ReagPos = *iter;

		m_reagCupInfo.erase(iter);
	}
	
    pCupPos->set_uidskid(ReagPos.uidskid());
    pCupPos->set_uicupid(ReagPos.uicupid());
    if (pBcInfo->len > 0)
    {
        pResult->set_strbarcode(pBcInfo->aucbarcode, pBcInfo->len);
    }

	app_print(ps_bcscan,"\n");
    app_print(ps_bcscan," ReagDisk: \r\n");
	app_print(ps_bcscan," DiskId  :  %d\r\n",ReagPos.uidskid());
	app_print(ps_bcscan," CupId   :  %d\r\n",ReagPos.uicupid());
	app_print(ps_bcscan," BarCode :  %s\r\n",pBcInfo->aucbarcode);
	
	//发送下一杯位扫描  
	if(!m_reagCupInfo.empty())
	{
		STCupPos NextReagPos = m_reagCupInfo.front();

		vos_threadsleep(BCSCAN_INTERVAL_TIME_LEN);
		vos_starttimer(m_reagTimer);
		return _exeReagBcScan(NextReagPos.uidskid(), NextReagPos.uicupid());

		//if(NextReagPos.uidskid() != ReagPos.uidskid())
		//{
		//	vos_starttimer(m_reagTimer);
		//	return _reagDiskRtt(NextReagPos.uidskid());//这个地方去除
		//}
		//else
		//{
		//	vos_threadsleep(BCSCAN_INTERVAL_TIME_LEN);
		//	vos_starttimer(m_reagTimer);
		//	return _exeReagBcScan(NextReagPos.uidskid(),NextReagPos.uicupid());
		//}
	}
	//上报结果
	else if(m_reagCupInfo.empty() && m_smpCupInfo.empty())
	{
        sendUpMsg(m_SrcPid, SPid_BcScan, MSG_ResBarcodeScan, &m_stScanResult);
		m_stScanResult.Clear();
		app_reportmidmachstatus(m_runState);
	}
	return VOS_OK;
}

vos_u32 CBcScan::_procSmpBcScanRes(app_dwn_barcode_tlv_stru* pBcInfo)
{
	//缓存结果
	STBarcodeScan * pResult = m_stScanResult.add_stsmpinfo();
    STCupPos * pCupPos = pResult->mutable_stcuppos();
	STCupPos SmpPos;
	
	if(!m_smpCupInfo.empty())
	{
		vector<STCupPos> ::iterator iter;
		iter = m_smpCupInfo.begin();
		SmpPos = *iter;
		m_smpCupInfo.erase(iter);
	}
	
    pCupPos->set_uidskid(SmpPos.uidskid());
    pCupPos->set_uicupid(SmpPos.uicupid());
    if (pBcInfo->len > 0)
    {
        pResult->set_strbarcode(pBcInfo->aucbarcode, pBcInfo->len);
    }

    app_print(ps_bcscan,"\n");
    app_print(ps_bcscan," ReagDisk: \r\n");
	app_print(ps_bcscan," DiskId  :  %d\r\n",SmpPos.uidskid());
	app_print(ps_bcscan," CupId   :  %d\r\n",SmpPos.uicupid());
	app_print(ps_bcscan," BarCode :  %s\r\n",pBcInfo->aucbarcode);
	
	
	//发送下一杯位扫描
	if(!m_smpCupInfo.empty())
	{
        sendUpMsg(m_SrcPid, SPid_BcScan, MSG_ResBarcodeScan, &m_stScanResult);
        m_stScanResult.Clear();

        STCupPos NextSmpPos = m_smpCupInfo.front();
		vos_threadsleep(BCSCAN_INTERVAL_TIME_LEN);
        vos_starttimer(m_smpTimer);
		return _exeSmpBcScan(NextSmpPos.uidskid(),NextSmpPos.uicupid());
	}
	//上报结果
	else if(m_reagCupInfo.empty() && m_smpCupInfo.empty())
	{
		WRITE_INFO_LOG("sendUpMsg! \n");
        sendUpMsg(m_SrcPid, SPid_BcScan, MSG_ResBarcodeScan, &m_stScanResult);
		m_stScanResult.Clear();
		app_reportmidmachstatus(m_runState);
	}
	return VOS_OK;
}

vos_u32 CBcScan::_procDwnRes(vos_msg_header_stru* pResMsg)
{
	if(m_testState == STATE_BCSCAN_TEST_OVER)
	{
		return VOS_OK;
	}
	
	app_com_dev_act_result_stru* pDwnActRes = (app_com_dev_act_result_stru*)&(pResMsg->aucmsgload[0]);
	app_dwn_barcode_stru* pLoad = (app_dwn_barcode_stru*)&(pDwnActRes->aucrsltinfo[0]);
	app_dwn_barcode_tlv_stru* pBcInfo = (app_dwn_barcode_tlv_stru*)&(pLoad->auctlvinfo[0]);
	
    if ((pResMsg->ulsrcpid == SPid_R2Dsk || pResMsg->ulsrcpid == SPid_R1Dsk) && (pDwnActRes->enacttype == Act_Move))
	{
		STCupPos ReagPos;
		vos_stoptimer(m_reagTimer);
		
		if(!m_reagCupInfo.empty())
		{
			ReagPos = m_reagCupInfo.front();
		}
		
		vos_starttimer(m_reagTimer);
		return _exeReagBcScan(ReagPos.uidskid(),ReagPos.uicupid());
	}
    else if ((pResMsg->ulsrcpid == SPid_R2Dsk || pResMsg->ulsrcpid == SPid_R1Dsk) && (pDwnActRes->enacttype == Act_BcScan))
	{
		vos_stoptimer(m_reagTimer);
		return _procReagBcScanRes(pBcInfo);
	}
	else if(pResMsg->ulsrcpid == SPid_SmpDsk)
	{
		vos_stoptimer(m_smpTimer);
		return _procSmpBcScanRes(pBcInfo);
	}
	return VOS_OK;
}

vos_u32 CBcScan::_procResetNdlRes()
{
	WRITE_INFO_LOG("Enter _procResetNdlRes! \n");
	m_resetNum--;
	if(m_resetNum == 0)
	{
		m_testState = STATE_BCSCAN_RESET_DSK;

		bool  hasR1 = false;
		bool  hasR2 = false;
		for (vector<STCupPos> ::iterator iter = m_reagCupInfo.begin(); iter != m_reagCupInfo.end(); iter++)
		{
			if (R1Ndl == iter->uidskid())
			{
				hasR1 = true;
			}
			else if (R2Ndl == iter->uidskid())
			{
				hasR2 = true;
			}
		}

		if (!m_reagCupInfo.empty())
		{
			if (hasR1)
			{
				m_resetNum += 1;
				app_reqReset(SPid_BcScan, SPid_R1Dsk, APP_YES);
			}
			if (hasR2)
			{
				m_resetNum += 1;
				app_reqReset(SPid_BcScan, SPid_R2Dsk, APP_YES);
			}
		}

		if (!m_smpCupInfo.empty())
		{
			m_resetNum += 1;
			app_reqReset(SPid_BcScan, SPid_SmpDsk, APP_YES);	
		}
	}
	return VOS_OK;
}

vos_u32 CBcScan::_procResetDskRes()
{	
	WRITE_INFO_LOG("Enter _procResetDskRes! \n");
	m_resetNum--;
	if(m_resetNum == 0)
	{
		if(!m_reagCupInfo.empty())
		{
			STCupPos ReagPos = m_reagCupInfo.front();
			vos_starttimer(m_reagTimer);
			_exeReagBcScan(ReagPos.uidskid(), ReagPos.uicupid());
		}

		if(!m_smpCupInfo.empty())
		{
			STCupPos SmpPos = m_smpCupInfo.front();
			vos_starttimer(m_smpTimer);
			WRITE_INFO_LOG("First _exeSmpBcScan! \n");
			_exeSmpBcScan(SmpPos.uidskid(),SmpPos.uicupid());
		}	
	}
	return VOS_OK;
}

vos_u32 CBcScan::_procResetRes(vos_msg_header_stru* pMsg)
{
	WRITE_INFO_LOG("Enter _procResetRes! \n");

	if(m_testState == STATE_BCSCAN_TEST_OVER)
	{
		return VOS_OK;
	}

	app_com_dev_act_result_stru* pActInfo = (app_com_dev_act_result_stru*)&(pMsg->aucmsgload[0]);
	if(pActInfo->enactrslt != xc8002_exe_cmd_sucess)
    {
        _procAbnormal();
		WRITE_ERR_LOG("Reset Failed.\n");
		app_reportmidmachstatus(vos_midmachine_state_reseted_err);
		return VOS_OK;
    }

	switch(m_testState)
	{
		case STATE_BCSCAN_RESET_NDL:
			_procResetNdlRes();
			break;
			
		case STATE_BCSCAN_RESET_DSK:
			_procResetDskRes();
			break;

		default:
			break;
	}
	return VOS_OK;
}

vos_u32 CBcScan::_procAbnormal()
{
	if(!m_reagCupInfo.empty() || !m_smpCupInfo.empty())
	{
		m_reagCupInfo.clear();
		m_smpCupInfo.clear();
		sendUpMsg(m_SrcPid, SPid_BcScan, MSG_ResBarcodeScan, &m_stScanResult);
		m_stScanResult.Clear();
	}

	return VOS_OK;
}

vos_u32 CBcScan::_procTimeOut(vos_msg_header_stru* pResMsg)
{
	vos_stoptimer(m_reagTimer);
	vos_stoptimer(m_smpTimer);

	if(m_testState == STATE_BCSCAN_TEST_OVER)
	{
		return VOS_OK;
	}

	_procAbnormal();
	
	WRITE_ERR_LOG("BcScan time out.\n");
	
    app_reportmidmachstatus(vos_midmachine_state_bcscan_err);

	return VOS_OK;
}

vos_u32 CBcScan::_procStopReq()
{
	m_testState = STATE_BCSCAN_TEST_OVER;

	_procAbnormal();
	app_reportmidmachstatus(vos_midmachine_state_standby);

	return VOS_OK;
}

vos_u32 CBcScan::CallBack(vos_msg_header_stru* pMsg)
{
	if (VOS_NULL == pMsg)
    {
        WRITE_ERR_LOG("Input parameter is null \n");
        return app_para_null_err;
    }

    vos_u32 ulResult = VOS_OK;
    WRITE_INFO_LOG("curpid=%u Recevied msg(0x%x) from pid(%u)\n",SPid_BcScan,pMsg->usmsgtype, pMsg->ulsrcpid);
    switch (pMsg->usmsgtype)
    {
        case MSG_ReqBarcodeScan:
            ulResult = _procBcScanReq(pMsg);
            break;

		case MSG_ResReset:
			ulResult = _procResetRes(pMsg);
			break;
			
		case app_com_device_res_exe_act_msg_type:
            ulResult = _procDwnRes(pMsg);
            break;
			
		case app_req_stop_midmach_msg_type:
			_procStopReq();
			break;
			
		case vos_pidmsgtype_timer_timeout:
            ulResult = _procTimeOut(pMsg);
            break;

        default:
            WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
            break;
    }
    return ulResult;
}

