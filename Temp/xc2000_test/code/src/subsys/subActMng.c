#include "subActMng.h"
#include "apputils.h"
#include "vos_log.h"

map<app_u32, string> g_mSubSys;
map <app_u32, string> g_mAct;
map<app_u32, long> g_useTime;
using namespace std;
CSubActMng::CSubActMng() :m_ulActIndex(0)
{
    g_mSubSys[SPid_SmpDsk]   = "SmpDsk  ";
    g_mSubSys[SPid_SmpNdl]   = "SmpNdl  ";
    g_mSubSys[SPid_R2Dsk]    = "R2Dsk	 ";
    g_mSubSys[SPid_R1]       = "R1      ";
    g_mSubSys[SPid_R1Dsk]    = "R1Dsk   ";
    g_mSubSys[SPid_R2]       = "R2      ";
    g_mSubSys[SPid_ReactDsk] = "ReactDsk";
    g_mSubSys[SPid_AutoWash] = "AutoWash";
    g_mSubSys[SPid_SMix]     = "SMix    ";
    g_mSubSys[SPid_R2Mix]    = "R2Mix   ";
    g_mSubSys[SPid_RTmpCtrl] = "TmpCtrl ";
    g_mSubSys[SPid_CoolReag] = "CoolReag";
    g_mSubSys[SPid_R1b]      = "R1b     ";
    g_mSubSys[SPid_R2b]      = "R2b     ";
    g_mSubSys[SPid_SmpNdlb]  = "smpNdlb ";
    g_mSubSys[SPid_SmpEmer]  = "smpNdlb ";
    g_mSubSys[SPid_L1] = "L1";
    g_mSubSys[SPid_L2] = "L2";
    g_mSubSys[SPid_Transport] = "AutoLoadTransport";
    g_mSubSys[SPid_VerticalGripper] = "AutoLoadVerticalGripper";
    g_mSubSys[SPid_Light] = "LightControl";

    g_mAct[Act_Reset          ] = "Reset ";
    g_mAct[Act_Move           ] = "Move  ";
    g_mAct[Act_Absorb         ] = "Absorb";
    g_mAct[Act_Discharge      ] = "Discharge ";
    g_mAct[Act_Wash           ] = "Wash  ";
    g_mAct[Act_AirOut         ] = "AirOut";
    g_mAct[Act_Mix            ] = "Mix   ";
    g_mAct[Act_IntensifyWash  ] = "IntensifyWash";
    g_mAct[Act_BcScan         ] = "BcScan";
    g_mAct[Act_DwnQuery       ] = "DwnQuery";
    g_mAct[Act_TrimOffset     ] = "TrimOffset";
    g_mAct[Act_SaveOffset     ] = "SaveOffset";
    g_mAct[Act_NdlVrt         ] = "NdlVrt";
    g_mAct[Act_Light          ] = "Light";
    g_mAct[Act_Ad             ] = "Ad";
    g_mAct[Act_DskHrotate     ] = "DskHrotate";
    g_mAct[Act_MarginScan     ] = "MarginScan";
    g_mAct[Act_TempSwitch     ] = "TempSwitch";
    g_mAct[Act_Fault          ] = "Fault     ";
    g_mAct[Act_Debug          ] = "Debug     ";
    g_mAct[Act_TempQuery      ] = "TempQuery ";
    g_mAct[Act_CtrlP04        ] = "CtrlP04   ";
    g_mAct[Act_MidState       ] = "MidState  ";
    g_mAct[Act_PVCtrl         ] = "PVCtrl    ";
    g_mAct[Act_PumpCtrl       ] = "PumpCtrl  ";
    g_mAct[Act_LiquidsCheck   ] = "LiquidsCheck";
    g_mAct[Act_CoolingCtrl    ] = "CoolingCtrl ";
    g_mAct[Act_BBClear        ] = "BBClear";
    g_mAct[Act_SmpLightCtrl   ] = "SmpLightCtrl";
    g_mAct[Act_UpdateDwn      ] = "UpdateDwn ";
    g_mAct[Act_OlnyMix        ] = "OlnyMix   ";
    g_mAct[Act_Showave        ] = "Showave   ";
    g_mAct[Act_LiquidDBG      ] = "LiquidDBG ";
    g_mAct[Act_SetFlState     ] = "SetFlState";
    g_mAct[Act_WriteMotor     ] = "WriteMotor";
    g_mAct[Act_MoveDilu       ] = "MoveDilu  ";
    g_mAct[Act_AbsorbDilu     ] = "AbsorbDilu";
    g_mAct[Act_DischargeDilu  ] = "DischargeDilu";
    g_mAct[Act_UpdateFpga     ] = "UpdateFpga";
    g_mAct[Act_sleepCtl       ] = "sleepCtl  ";
    g_mAct[Act_washLong       ] = "washLong  ";
    g_mAct[Act_washShort      ] = "washShort ";
    g_mAct[Act_SetWTemp		  ] = "SetWTemp";
    g_mAct[Act_SetRTemp	      ] = "SetRTemp";
    g_mAct[Act_Timer	      ] = "Timer";
    g_mAct[Act_MoveAbs	      ] = "MoveAbs ";
    g_mAct[Act_MoveW	      ] = "MoveW ";
    g_mAct[Act_MoveInner      ] = "MoveInner ";
    g_mAct[Act_MoveOuter      ] = "MoveOuter ";
    g_mAct[Act_MoveDisg       ] = "MoveDisg  ";
    g_mAct[Act_TPMoveF] = "TPMoveF";
    g_mAct[Act_TPMoveL] = "TPMoveL";
    g_mAct[Act_TPMoveR] = "TPMoveR";
    g_mAct[Act_VGMoveL] = "VGMoveL";
    g_mAct[Act_VGMoveF] = "VGMoveF";
    g_mAct[Act_VGMoveR] = "VGMoveR";
}

CSubActMng::~CSubActMng()
{

}

void CSubActMng::InsertActST(app_u32 subSys, const STSubActInfo& stActInfo)
{
    m_mSubAct[subSys]->InsertAct(m_ulActIndex, stActInfo);
}

void CSubActMng::InsertAct(app_u32 subSys, app_u32 actType, 
    app_u32 rSubSys /*= 0*/, app_u32 rActTyp /*= 0*/, app_u32 rIndex /*= 0*/,
    app_u32 rSubSys2 /*= 0*/, app_u32 rActTyp2 /*= 0*/, app_u32 rIndex2 /*= 0*/)
{
    STSubActInfo stActInfo;
    stActInfo.set_bdone(false);
    stActInfo.set_uiacttype(actType);

    if (rSubSys != 0)
    {
        STRAct* pRelyAct = stActInfo.add_strelyact();
        pRelyAct->set_uisubsys(rSubSys);
        pRelyAct->set_uiacttype(rActTyp);
        pRelyAct->set_uiactindex(rIndex);
    }
    if (rSubSys2 != 0)
    {
        STRAct* pRelyAct = stActInfo.add_strelyact();
        pRelyAct->set_uisubsys(rSubSys2);
        pRelyAct->set_uiacttype(rActTyp2);
        pRelyAct->set_uiactindex(rIndex2);
    }
    m_mSubAct[subSys]->InsertAct(m_ulActIndex, stActInfo);

//     va_list arg_ptr;
//     app_u32 index = 0;
//     va_start(arg_ptr, actType);
//     app_u32 tempValue = va_arg(arg_ptr, app_u32);
//     while (tempValue != 0)
//     {
//         if (index%3==0)
//         {
//             STRAct* pRelyAct = stActInfo.add_strelyact();
//             pRelyAct->set_uisubsys(tempValue);
//         }
//         index++;
//         tempValue = va_arg(arg_ptr, app_u32);
//     }
// 
//     va_end(arg_ptr);
//    return sum;
}

app_u32 CSubActMng::ProcResAct(vos_msg_header_stru* pstvosmsg)
{
    app_com_dev_act_result_stru* pMsg = (app_com_dev_act_result_stru*)pstvosmsg->aucmsgload;

    app_u32 subSys = pstvosmsg->ulsrcpid;
    app_u32 actType = pMsg->enacttype;

    //处理动作返回
    app_print(ps_rscsch, "        (%04d) TIME %ld  Res  [%s][%s] USETIME %ld\n", pMsg->ulActIndex, getTime_ms(), 
        g_mSubSys[subSys].c_str(), g_mAct[actType].c_str(),getTime_ms()-g_useTime[subSys*1000+actType]);
    g_useTime.erase(subSys * 1000 + actType);
    map<app_u32, map<app_u32, ProcResFuc> >::iterator iter = m_mResFuncs.find(subSys);
    if (iter != m_mResFuncs.end())
    {
        map<app_u32, ProcResFuc>::iterator item = iter->second.find(actType);
        if (item != iter->second.end())
        {
            (this->*(item->second))(subSys, pMsg);
        }
    }

    //处理失败动作
    if (pMsg->enactrslt != EN_AF_Success)
    {
        WRITE_ERR_LOG("SubAct failed --- [%s], [%s]", g_mSubSys[subSys].c_str(), g_mAct[actType].c_str());
        (this->*m_mAlarmFunc)(subSys, pMsg);
    }

    //触发下一个动作
    bool bDone = (pMsg->enactrslt == EN_AF_Success) ? true : false;
    m_mSubAct[subSys]->SetDone(actType, bDone, pMsg->ulActIndex);// pMsg->ulActIndex);
    return SendNextAct();
}

app_u32 CSubActMng::SendNextAct()
{
    map<app_u32, CSubAct *> ::iterator itAct;
    for (itAct = m_mSubAct.begin(); itAct != m_mSubAct.end(); itAct++)
    {
        CSubAct * pSubAct = itAct->second;
        STSubActInfo stActInfo;
        while (pSubAct->GetNextAct(stActInfo))
        {
            if (!_checkAct(stActInfo))
            {
                break;
            }
            pSubAct->PopNextAct();
            g_useTime[itAct->first * 1000 + stActInfo.uiacttype()] = getTime_ms();
            app_print(ps_rscsch, "ActIndex(%04d) TIME %ld  Send [%s][%s] \n", m_ulActIndex, getTime_ms(), 
                g_mSubSys[itAct->first].c_str(), g_mAct[stActInfo.uiacttype()].c_str());
            app_u32 ulrunrslt = (this->*(m_mActFunc[itAct->first][stActInfo.uiacttype()]))(itAct->first, stActInfo.uiacttype());
            if (ulrunrslt != VOS_OK)
            {
                return ulrunrslt;
            }
        }
    }
    return VOS_OK;
}

void CSubActMng::ResetAct(app_u32 ulSubsys /*= 0*/)
{
    map<app_u32, CSubAct *> ::iterator itAct;
    if (ulSubsys != 0)
    {
        itAct = m_mSubAct.find(ulSubsys);
        if (itAct != m_mSubAct.end())
        {
            itAct->second->ClearAct();
        }
    }
    else
    {
        for (itAct = m_mSubAct.begin(); itAct != m_mSubAct.end(); itAct++)
        {
            itAct->second->ResetAct();
        }
    }
}

bool CSubActMng::IsFinish()
{
    map<app_u32, CSubAct *> ::iterator itAct;
    for (itAct = m_mSubAct.begin(); itAct != m_mSubAct.end(); itAct++)
    {
        if (itAct->second->IsFinish() == false)
        {
            return false;
        }
    }
    return true;
}

vos_u32 CSubActMng::app_reqDwmCmd(vos_u32 srcPid, vos_u32 dstPid, app_u16 actType, vos_u32 cmdLen, char* cmdPara, app_u16 singRes)
{
    map<app_u32, CSubAct *> ::iterator itAct = m_mSubAct.find(dstPid);
    if (itAct != m_mSubAct.end())
    {
       return itAct->second->ReqDwmCmd(srcPid, dstPid, actType, cmdLen, cmdPara, singRes);
    }
    return VOS_INVALID_U32;
}

bool CSubActMng::_checkAct(STSubActInfo& stActInfo)
{
    for (int i = 0; i < stActInfo.strelyact_size(); i++)
    {
        STRAct * pRelyAct = stActInfo.mutable_strelyact(i);
        map<app_u32, CSubAct *> ::iterator itSubAct = m_mSubAct.find(pRelyAct->uisubsys());
        if (itSubAct->second->CheckDone(pRelyAct->uiacttype(), pRelyAct->uiactindex()) == false)
        {
            return false;
        }
    }
    return true;
}

