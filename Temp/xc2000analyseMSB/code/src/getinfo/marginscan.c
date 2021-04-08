#include "typedefine.pb.h"
#include "upmsgstruct.pb.h"
#include "marginscan.h"
#include "msg.h"
#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include "app_xc8002shareinfo.h"
#include "socketmonitor.h"
#include "app_msgtypedefine.h"
#include "app_msg.h"
#include <stdio.h>
#include "app_errorcode.h"
#include "vos_timer.h"
#include "apputils.h"
#include "vos_log.h"

CMarginScan::CMarginScan(vos_u32 ulPid) : CCallBack(ulPid)
{	
	m_testState = STATE_MARGINSCAN_IDLE;
	//创建执行动作超时定时器
	m_InulTimer = createTimer(SPid_MarginScan, "INDSK_MARGINSCAN",MarginScan_TIME_LEN);
	m_OutulTimer = createTimer(SPid_MarginScan, "OUTDSK_MARGINSCAN",MarginScan_TIME_LEN);
}

vos_u32 CMarginScan::_procTimeOut(vos_msg_header_stru* pResMsg)
{
	vos_stoptimer(m_InulTimer);
	vos_stoptimer(m_OutulTimer);

	if(m_testState == STATE_MARGINSCAN_TEST_OVER)
	{
		return VOS_OK;
	}

	_procAbnormal();
	
    WRITE_ERR_LOG("MarginScan time out.\n");
    app_reportmidmachstatus(vos_midmachine_state_margin_scan_err);

	return VOS_OK;
}

vos_u32 CMarginScan::_procAbnormal()
{
	if(!m_stInnerCups.empty() || !m_stOuterCups.empty())
	{	
		while(!m_stInnerCups.empty())
		{
			m_stInnerCups.pop();
		}
		while(!m_stOuterCups.empty())
		{
			m_stOuterCups.pop();
		}
		sendUpMsg(m_uiSrcPid, SPid_MarginScan, MSG_ResMarginScan, &m_stScanResult);
	}
	return VOS_OK;
}

vos_u32 CMarginScan::_procStopReq()
{
	m_testState = STATE_MARGINSCAN_TEST_OVER;
	_procAbnormal();
	app_reportmidmachstatus(vos_midmachine_state_standby);

	return VOS_OK;
}

vos_u32 CMarginScan::CallBack(vos_msg_header_stru* pMsg)
{
	if (VOS_NULL == pMsg)
	{
		WRITE_ERR_LOG("Input parameter is null \n");
		return app_para_null_err;
	}

	vos_u32 ulResult = VOS_OK;
	switch (pMsg->usmsgtype)
	{
		case MSG_ReqMarginScan:
			ulResult = _procMarginScanReq(pMsg);
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

vos_u32 CMarginScan::_procMarginScanReq(vos_msg_header_stru* pReqMsg)
{
	WRITE_INFO_LOG("Enter _procMarginScanReq\n");
	_putCupposIntoBuffer(pReqMsg);
	_ndlReset();

	return VOS_OK;
}

vos_u32 CMarginScan::_putCupposIntoBuffer(vos_msg_header_stru* pReqMsg)
{
	WRITE_INFO_LOG("Enter _putCupposIntoBuffer\n");

	m_runState = getCurRunState();
	
    if (!g_pMonitor->CanBCScan())
    {
        WRITE_ERR_LOG("MarginScan failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }
	m_uiSrcPid = pReqMsg->ulsrcpid;
    m_stScanResult.Clear();
    m_stScanResult.set_uimarginscan(VOS_YES);

	CMsg stMsg;
    stMsg.ParseMsg(pReqMsg);
	STReqMarginScan*  pReq = dynamic_cast<STReqMarginScan*>(stMsg.pPara);
	
	for(int i=0;i<pReq->stscancup_size();++i)
	{
		const STCupPos& rPos = pReq->stscancup(i);
		if(app_xc8002_reagent_outer_disk_id == rPos.uidskid())
		{
			m_stOuterCups.push(rPos.uicupid());

			WRITE_INFO_LOG("outerdskid : %d,outercupid : %d\r\n", rPos.uidskid(),rPos.uicupid());
		}
		else if(app_xc8002_reagent_inner_disk_id == rPos.uidskid())
		{
			m_stInnerCups.push(rPos.uicupid());

			WRITE_INFO_LOG("innerdskid : %d,innercupid : %d\r\n", rPos.uidskid(),rPos.uicupid());
		}
	}

	app_reportmidmachstatus(vos_midmachine_state_margin_scan);

    return VOS_OK;
}

vos_u32 CMarginScan::_ndlReset()
{
	WRITE_INFO_LOG("Enter _ndlReset\n");
	m_testState = STATE_MARGINSCAN_RESET_NDL;
	if(!m_stOuterCups.empty() && !m_stInnerCups.empty())
	{
		m_resetNum = 2;
		app_reqReset(SPid_MarginScan, SPid_R1, APP_YES); 
		app_reqReset(SPid_MarginScan, SPid_R2, APP_YES); 
	}
	else if(!m_stOuterCups.empty())
	{
		m_resetNum = 1;
		app_reqReset(SPid_MarginScan, SPid_R1, APP_YES); 
	}
	else
	{
		m_resetNum = 1;
		app_reqReset(SPid_MarginScan, SPid_R2, APP_YES);	
	}

    return VOS_OK;
}

vos_u32 CMarginScan::_procResetRes(vos_msg_header_stru* pMsg)
{
	WRITE_INFO_LOG("Enter _procResetRes\n");
	if(m_testState == STATE_MARGINSCAN_TEST_OVER)
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
	
	switch (m_testState)
    {
        case STATE_MARGINSCAN_RESET_NDL:
			_procNdlReset();
            break;
			
        case STATE_MARGINSCAN_RESET_DSK:
            _procDskReset(pMsg);
            break;
			
        default:
            WRITE_ERR_LOG("Unknown m_testState = %d\n", m_testState);
            break;
    }
	
    return VOS_OK;
}

vos_u32 CMarginScan::_procDwnRes(vos_msg_header_stru* pResMsg)
{
	WRITE_INFO_LOG("Enter _procDwnRes\n");
	if(m_testState == STATE_MARGINSCAN_TEST_OVER)
	{
		return VOS_OK;
	}
	
	app_com_dev_act_result_stru* pActInfo = (app_com_dev_act_result_stru*)&(pResMsg->aucmsgload[0]);
	if(pActInfo->enactrslt != xc8002_exe_cmd_sucess)
	{
		_procAbnormal();
		WRITE_ERR_LOG("margin scan downmach act failed\n");
		app_reportmidmachstatus(vos_midmachine_state_margin_scan_err);
		return VOS_OK;
	}

	switch (pActInfo->enacttype)
    {
        case Act_Move:
        {
			if(pResMsg->ulsrcpid == SPid_R1Dsk)
			{
                _singleNdlMarginScan(SPid_R1);
			}
			else if(pResMsg->ulsrcpid == SPid_R2Dsk)
			{
                _singleNdlMarginScan(SPid_R2);
			}
            break;
        }
		case Act_MarginScan:
        {
            _putResultIntoBuffer(pResMsg);
            break;
        }
        case Act_NdlVrt:
        {
            _procOneCupOverRes(pResMsg);
            break;
        }
        default:
        {
            WRITE_ERR_LOG("saveActHead failed usActType=0x%lx\n", pActInfo->enacttype);
            break;
        }
    }
	
    return VOS_OK;
}

vos_u32 CMarginScan::_singleDiskRtt(vos_u32 dstPid,app_u16 uiCupId)
{
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

	app_constructMsgHead(&rHead, dstPid, SPid_MarginScan, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = Act_Move;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
    pstdwnmachreqmsg->ulDataLen = sizeof(act_stReagSysDskRtt) + sizeof(msg_stActHead) ;

	pdstrst = (act_stReagSysDskRtt*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->usCupId = uiCupId;
    
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);;
}

vos_u32 CMarginScan::_outDiskRtt()
{
	WRITE_INFO_LOG("Enter _outDiskRtt\n");
	
	if(!m_stOuterCups.empty())
	{
        app_u16 uiCupId = (app_u16)m_stOuterCups.front();
        _singleDiskRtt(SPid_R1Dsk, uiCupId);
        vos_starttimer(m_OutulTimer);
	}
    return VOS_OK;
}

vos_u32 CMarginScan::_inDiskRtt()
{
	WRITE_INFO_LOG("Enter _inDiskRtt\n");

	if(!m_stInnerCups.empty())
	{
        app_u16 uiCupId = (app_u16)m_stInnerCups.front();
        _singleDiskRtt(SPid_R2Dsk, uiCupId);
        vos_starttimer(m_InulTimer);
	}
    return VOS_OK;
}

vos_u32 CMarginScan::_procNdlReset()
{
	WRITE_INFO_LOG("Enter _procNdlReset\n");
	m_resetNum--;
	if(m_resetNum == 0)
	{
		m_testState = STATE_MARGINSCAN_RESET_DSK;
		if(!m_stOuterCups.empty() && !m_stInnerCups.empty())
		{
			m_resetNum = 2;
			app_reqReset(SPid_MarginScan, SPid_R1Dsk, APP_YES); 
			app_reqReset(SPid_MarginScan, SPid_R2Dsk, APP_YES); 
		}
		else if(!m_stOuterCups.empty())
		{
			m_resetNum = 1;
			app_reqReset(SPid_MarginScan, SPid_R1Dsk, APP_YES); 
		}
		else
		{
			m_resetNum = 1;
			app_reqReset(SPid_MarginScan, SPid_R2Dsk, APP_YES);	
		}
	}

	return VOS_OK;
}

vos_u32 CMarginScan::_procDskReset(vos_msg_header_stru* pMsg)
{	
	WRITE_INFO_LOG("Enter _procDskReset\n");
	if(!m_stOuterCups.empty() && pMsg->ulsrcpid == SPid_R1Dsk)
	{
		_outDiskRtt();
	}
	if(!m_stInnerCups.empty() && pMsg->ulsrcpid == SPid_R2Dsk)
	{
		_inDiskRtt();
	}

	return VOS_OK;
}

vos_u32 CMarginScan::_singleNdlMarginScan(vos_u32 dstPid)
{
	WRITE_INFO_LOG("Enter _singleNdlMarginScan\n");
	vos_u32 ulResult;
	
    msg_stComDevActReq* pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) * 4 + sizeof(msg_stNdlRtt) + sizeof(msg_stVrt);
    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x) \n", ulResult);
        return ulResult;
    }
    pPara->usActNum = 4;

    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo*)(&(pPara->astActInfo[0]));
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stNdlRtt);
    pActInfo->stActHead.usActType = Act_Move;
    pActInfo->stActHead.usSingleRes = APP_NO;
	msg_stNdlRtt * pActPara0 = (msg_stNdlRtt *)(pActInfo->aucPara);
    if (dstPid == SPid_R1)
    {
        pActPara0->usDstPos = (m_stOuterCups.front()>50) ? app_reagent_ndl_stop_pos_absrbIN : app_reagent_ndl_stop_pos_absrbOut;
    }
    else
    {
        pActPara0->usDstPos = (m_stInnerCups.front()>50) ? app_reagent_ndl_stop_pos_absrbIN : app_reagent_ndl_stop_pos_absrbOut;
    }

    pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(pActInfo->ulDataLen) + pActInfo->ulDataLen);
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    pActInfo->stActHead.usActType = Act_MarginScan;
    pActInfo->stActHead.usSingleRes = APP_YES;

	pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(pActInfo->ulDataLen) + pActInfo->ulDataLen);
	pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stVrt);
    pActInfo->stActHead.usActType = Act_NdlVrt;
    pActInfo->stActHead.usSingleRes = APP_YES;
	msg_stVrt * pActPara1 = (msg_stVrt *)(pActInfo->aucPara);
    pActPara1->dir = cmd_motor_dir_up;

	pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(pActInfo->ulDataLen) + pActInfo->ulDataLen);
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    pActInfo->stActHead.usActType = Act_Wash;
    pActInfo->stActHead.usSingleRes = APP_NO;

    msg_stHead stHead;
    app_constructMsgHead(&stHead, dstPid, SPid_MarginScan, app_com_device_req_exe_act_msg_type);
    return app_sendMsg(&stHead, pPara, ulParaLen);
}

vos_u32 CMarginScan::_procResult(vos_u32 ulDskPid,vos_u32 ulCupId,vos_u32 ulDepth)
{
    STMarginScan * pResult = m_stScanResult.add_stmargininfo();
    STCupPos * pCupPos = pResult->mutable_stcuppos();
    
    pCupPos->set_uidskid(ulDskPid);
    pCupPos->set_uicupid(ulCupId);
    pResult->set_uimargindepth(ulDepth);
	if (ulDepth == 0xFFFE)
	{
		ulDepth = 1;
	}

    app_print(ps_reportData,"ulDskPid : %d,ulCupId : %d,ulDepth : %d \r\n", ulDskPid, ulCupId,ulDepth); 

	return VOS_OK;
}

vos_u32 CMarginScan::_putResultIntoBuffer(vos_msg_header_stru* pResMsg)
{
	WRITE_INFO_LOG("Enter _putResultIntoBuffer\n");
	app_com_dev_act_result_stru* pDwnActRes = (app_com_dev_act_result_stru*)&(pResMsg->aucmsgload[0]);
	msg_stScanResLoad* pLoad = (msg_stScanResLoad*)&(pDwnActRes->aucrsltinfo[0]);

	if(pResMsg->ulsrcpid == SPid_R1)
	{
		if(!m_stOuterCups.empty())
		{
			m_uiOutCupId = m_stOuterCups.front();
		}
		

		pLoad->usemptydepth = pLoad->usemptydepth == 0xFFFE ? 1 : pLoad->usemptydepth;
		printf("\r\n\n");
		printf("\r OuterDiskId  :  \r\n");
		printf("\r CupId   :  %lu\r\n",m_uiOutCupId);
		printf("\r Depth   :  %d\r\n",pLoad->usemptydepth);
		printf("\r\n\n");

        _procResult(app_xc8002_reagent_outer_disk_id,m_uiOutCupId,pLoad->usemptydepth);

	}
	else if(pResMsg->ulsrcpid == SPid_R2)
	{
		if(!m_stInnerCups.empty())
		{
			m_uiInCupId = m_stInnerCups.front();
		}
		
		printf("\r\n\n");
		printf("\r InnerDiskId  :  \r\n");
		printf("\r CupId   :  %lu\r\n",m_uiInCupId);
		printf("\r Depth   :  %d\r\n",pLoad->usemptydepth);
		printf("\r\n\n");

        _procResult(app_xc8002_reagent_inner_disk_id,m_uiInCupId,pLoad->usemptydepth);

	}

	return VOS_OK;
}

vos_u32 CMarginScan::_procOneCupOverRes(vos_msg_header_stru* pResMsg)
{
	WRITE_INFO_LOG("Enter _procOneCupOverRes\n");

	if(pResMsg->ulsrcpid == SPid_R1)
	{
		vos_stoptimer(m_OutulTimer);

		if(!m_stOuterCups.empty())
		{
			m_stOuterCups.pop();
		}
		
		if(!m_stOuterCups.empty())
		{
        	_outDiskRtt();
			return VOS_OK;
		}
	}
	else if(pResMsg->ulsrcpid == SPid_R2)
	{
		vos_stoptimer(m_InulTimer);

		if(!m_stInnerCups.empty())
		{
			m_stInnerCups.pop();
		}
		
		if(!m_stInnerCups.empty())
		{
			_inDiskRtt();
			return VOS_OK;
		}
	}
	if(m_stInnerCups.empty() && m_stOuterCups.empty())
	{
		sendUpMsg(m_uiSrcPid, SPid_MarginScan, MSG_ResMarginScan, &m_stScanResult);
		app_reportmidmachstatus(m_runState);
	}
	return VOS_OK;
}

