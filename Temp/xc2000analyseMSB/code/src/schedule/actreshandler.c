#include "actreshandler.h"
#include "typedefine.pb.h"
#include "apputils.h"
#include "reactcupmng.h"
#include "vos_log.h"
#include "taskmng.h"

CResHandler* g_pResHandler = NULL;
CResHandler::CResHandler() : m_bAutoWash(false), m_bSmpNdlWash(false), m_bR1NdlWash(false), m_bR2NdlWash(false)
{
    _initProcFuc();
}

CResHandler::~CResHandler()
{

}
void CResHandler::_initProcFuc()
{
    m_procFucs[SPid_SmpDsk][Act_Move] = &CResHandler::_procSmpDskMove;

    m_procFucs[SPid_SmpNdl][Act_Move] = &CResHandler::_procSmpNdlMove;
	m_procFucs[SPid_SmpNdl][Act_MoveDilu] = &CResHandler::_procSmpNdlMoveDilu;//稀释位转动
	
    m_procFucs[SPid_SmpNdl][Act_Absorb] = &CResHandler::_procSmpNdlAbsorb;
	m_procFucs[SPid_SmpNdl][Act_AbsorbDilu] = &CResHandler::_procSmpNdlAbsorbDilu;
	
    m_procFucs[SPid_SmpNdl][Act_Discharge] = &CResHandler::_procSmpNdlDischarge;
	m_procFucs[SPid_SmpNdl][Act_DischargeDilu] = &CResHandler::_procSmpNdlDischargeDilu;//稀释位转动
	
    m_procFucs[SPid_R2Dsk][Act_Move] = &CResHandler::_procInnerDskMove;

    m_procFucs[SPid_R1][Act_Move] = &CResHandler::_procR1Move;
    m_procFucs[SPid_R1][Act_Absorb] = &CResHandler::_procR1Absorb;
    m_procFucs[SPid_R1][Act_Discharge] = &CResHandler::_procR1Discharge;

    m_procFucs[SPid_R1Dsk][Act_Move] = &CResHandler::_procOuterDskMove;

    m_procFucs[SPid_R2][Act_Move] = &CResHandler::_procR2Move;
    m_procFucs[SPid_R2][Act_Absorb] = &CResHandler::_procR2Absorb;
    m_procFucs[SPid_R2][Act_Discharge] = &CResHandler::_procR2Discharge;

    m_procFucs[SPid_ReactDsk][Act_Move] = &CResHandler::_procReactDskMove;

    m_procFucs[SPid_AutoWash][Act_Wash] = &CResHandler::_procAutoWash;

    m_procFucs[SPid_SMix][Act_Move] = &CResHandler::_procSmpMixMove;
    m_procFucs[SPid_SMix][Act_Mix] = &CResHandler::_procSmpMixMix;
	

    m_procFucs[SPid_R2Mix][Act_Move] = &CResHandler::_procReagMixMove;
    m_procFucs[SPid_R2Mix][Act_Mix] = &CResHandler::_procReagMixMix;

#ifndef Edition_A
    m_procFucs[SPid_SMix][Act_Wash] = &CResHandler::_procSampMixWashCtrl;
	m_procFucs[SPid_R2Mix][Act_Wash] = &CResHandler::_procReagMixWashCtrl;
#endif
}
vos_u32 CResHandler::ProcActRes(vos_msg_header_stru* pstvosmsg)
{
    app_com_dev_act_result_stru * pstmvmsg = (app_com_dev_act_result_stru *)pstvosmsg->aucmsgload;
    WRITE_INFO_LOG("ActRes: srcpid=%lu, acttype=%lu, result=%u\n", pstvosmsg->ulsrcpid, pstmvmsg->enacttype, pstmvmsg->enactrslt);

    map<app_u16, map<app_u16,ProcActResult> >::iterator iter = m_procFucs.find(pstvosmsg->ulsrcpid);
    if (iter != m_procFucs.end())
    {
        map<app_u16, ProcActResult>::iterator item = iter->second.find(pstmvmsg->enacttype);
        if (item != iter->second.end())
        {
            (this->*(item->second))(pstmvmsg->enactrslt);
            return VOS_OK;
        }
    }
    WRITE_ERR_LOG("ProcActRes failed srcpid=%lu,acttype=%lu\n", pstvosmsg->ulsrcpid, pstmvmsg->enacttype);
    return VOS_OK;
}

void CResHandler::_procReactDskMove(int iResult)
{
    _setDskMoveResult(g_pRscMng->enReactDskFlag, iResult);
}

void CResHandler::_procSmpMixMove(int iResult)
{
    g_pOuterCupMng->SetActResult(EN_AC_SimpleMixNdlToMixPos, iResult);
}

void CResHandler::_procSmpMixMix(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_SimpleMix, iResult))
    {
#ifdef Edition_A
        app_reqDwmCmd(SPid_Task, SPid_SMix, Act_Wash, 0, NULL, APP_NO);
#else
        MixWashCtrl(SPid_Task, SPid_SMix, APP_OPEN);
        app_reqDwmCmd(SPid_Task, SPid_SMix, Act_Wash, 0, NULL, APP_YES);
#endif		
    }
}

#ifndef Edition_A
void CResHandler::_procSampMixWashCtrl(int iResult)
{
	MixWashCtrl(SPid_Task, SPid_SMix, APP_CLOSE);
}
void CResHandler::_procReagMixWashCtrl(int iResult)
{
	MixWashCtrl(SPid_Task, SPid_R2Mix, APP_CLOSE);
}
#endif

void CResHandler::_procReagMixMove(int iResult)
{
    g_pOuterCupMng->SetActResult(EN_AC_ReagMixNdlToMixPos, iResult);
}

void CResHandler::_procReagMixMix(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_ReagMix, iResult))
    {
#ifdef Edition_A
        app_reqDwmCmd(SPid_Task, SPid_R2Mix, Act_Wash, 0, NULL, APP_NO);
#else
        MixWashCtrl(SPid_Task, SPid_R2Mix, APP_OPEN);
        app_reqDwmCmd(SPid_Task, SPid_R2Mix, Act_Wash, 0, NULL, APP_YES);
#endif
    }
}

void CResHandler::_procR1Move(int iResult)
{
    g_pOuterCupMng->SetActResult(EN_AC_R1ToReagPos, iResult);
}

void CResHandler::_procR1Absorb(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_R1AbsorbAndDischarge, iResult))
    {
        g_pOuterCupMng->CheckOuterDsk();
    }
}

void CResHandler::_procR1Discharge(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_R1AbsorbAndDischarge, iResult))
    {
        if (!m_bAutoWash)
        {
			OPEN_P04(SPid_Task); //p04控制
            app_reqDwmCmd(SPid_Task, SPid_R1, Act_Wash, 0, NULL, APP_NO);
        }
        else
        {
            m_bR1NdlWash = true;
        }
    }
	g_pOuterCupMng->ProcLiquid(EN_AC_R1ToReagPos, TR_AddR1Err, xc8002_dwnmach_name_r1Ndl);
}

void CResHandler::_procR2Move(int iResult)
{
    g_pOuterCupMng->SetActResult(EN_AC_R2ToReagPos, iResult);
}

void CResHandler::_procR2Absorb(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_R2AbsorbAndDischarge, iResult))
    {
        g_pOuterCupMng->CheckInnerDsk();
    }
}

void CResHandler::_procR2Discharge(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_R2AbsorbAndDischarge, iResult))
    {
        if (!m_bAutoWash)
        {
			OPEN_P04(SPid_Task); //p04控制
            app_reqDwmCmd(SPid_Task, SPid_R2, Act_Wash, 0, NULL, APP_NO);
        }
        else
        {
            m_bR2NdlWash = true;
        }
    }
	g_pOuterCupMng->ProcLiquid(EN_AC_R2ToReagPos, TR_AddR2Err, xc8002_dwnmach_name_r2Ndl);
}

void CResHandler::_procSmpNdlMove(int iResult)
{
    g_pOuterCupMng->SetActResult(EN_AC_SimpleNdlToAbsorbPos, iResult);
}

void CResHandler::_procSmpNdlMoveDilu(int iResult)
{
	g_pOuterCupMng->SetActResult(EN_AC_SimpleNdlToAbsorbPos_DILU, iResult);
}



void CResHandler::_procSmpNdlAbsorb(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_SimpleNdlAbsorb, iResult))
    {
        g_pOuterCupMng->CheckSmpDsk();
    }
}

void CResHandler::_procSmpNdlAbsorbDilu(int iResult)
{
	if (g_pOuterCupMng->SetActResult(EN_AC_SimpleNdlAbsorb_DILU, iResult))
	{
		g_pOuterCupMng->CheckSmpDskDilu();
	}
}

void CResHandler::_procSmpNdlDischargeDilu(int iResult)
{
	if (g_pOuterCupMng->SetActResult(EN_AC_SimpleNdlAbsorb_DILU, iResult))
	{
        if (!m_bAutoWash)
        {
            OPEN_P04(SPid_Task); //p04控制
            app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_Wash, 0, NULL, APP_NO);
        }
		else
		{
			m_bSmpNdlWash = true;
		}
	}
}



void CResHandler::_procSmpNdlDischarge(int iResult)
{
    if (g_pOuterCupMng->SetActResult(EN_AC_SimpleNdlAbsorb, iResult))
    {
        if (!m_bAutoWash)
        {
            OPEN_P04(SPid_Task); //p04控制
            if (g_pOuterCupMng->CheckLongWash(EN_AC_SimpleNdlAbsorb))
            {
                app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_washLong, 0, NULL, APP_NO);
            }
            else
            {
            	app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_Wash, 0, NULL, APP_NO);
            }
        }
        else
        {
            m_bSmpNdlWash = true;
        }
    }
	g_pOuterCupMng->ProcLiquid(EN_AC_SimpleNdlAbsorb, TR_AddSmpErr, xc8002_dwnmach_name_smpndl);
}

void CResHandler::_procAutoWash(int iResult)
{
    if (EN_AF_Success == iResult)
    {
        g_pOuterCupMng->ProcAutoWash();
        m_bAutoWash = false;

        if (m_bSmpNdlWash)
        {
            m_bSmpNdlWash = false;
            if (g_pOuterCupMng->CheckLongWash(EN_AC_SimpleNdlAbsorb))
            {
                app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_washLong, 0, NULL, APP_NO);
            }
            else
            {
                app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_Wash, 0, NULL, APP_NO);
            }  
        }
        if (m_bR1NdlWash)
        {
            m_bR1NdlWash = false;
            app_reqDwmCmd(SPid_Task, SPid_R1, Act_Wash, 0, NULL, APP_NO);
        }
        if (m_bR2NdlWash)
        {
            m_bR2NdlWash = false;
            app_reqDwmCmd(SPid_Task, SPid_R2, Act_Wash, 0, NULL, APP_NO);
        }
    }
}

void CResHandler::_procSmpDskMove(int iResult)
{
    _setDskMoveResult(g_pRscMng->enSimpleDskFlag, iResult);
}

void CResHandler::_procOuterDskMove(int iResult)
{
    _setDskMoveResult(g_pRscMng->enOuterDskFlag, iResult);
}

void CResHandler::_procInnerDskMove(int iResult)
{
    _setDskMoveResult(g_pRscMng->enInnerDskFlag, iResult);
}

void CResHandler::_setDskMoveResult(EN_ActFlag& actDskFlag, int iResult)
{
    if (EN_AF_Doing == actDskFlag&& EN_AF_Success == iResult)
    {
        actDskFlag = EN_AF_Unstart;
    }
}

