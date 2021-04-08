#pragma once
#include "subAct.h"
#include "app_comstructdefine.h"
#include "vos_messagecenter.h"

class CSubActMng
{
protected:
    typedef vos_u32(CSubActMng::*ActFun)(app_u32 subSys, app_u16 actType);
    typedef void(CSubActMng::*ProcResFuc)(app_u32 subSys, app_com_dev_act_result_stru* pRes);

public:
    CSubActMng();
    ~CSubActMng();

public:
    void InsertActST(app_u32 subSys, const STSubActInfo& stActInfo);
    void InsertAct(app_u32 subSys, app_u32 actType, app_u32 rSubSys = 0, app_u32 rActTyp = 0, app_u32 rIndex = 0, app_u32 rSubSys2 = 0, app_u32 rActTyp2 = 0, app_u32 rIndex2 = 0);
    app_u32 ProcResAct(vos_msg_header_stru* pstvosmsg);
    app_u32 SendNextAct();
    void ResetAct(app_u32 ulSubSys = 0);
    bool IsFinish();
    vos_u32 app_reqDwmCmd(vos_u32 srcPid, vos_u32 dstPid, app_u16 actType, vos_u32 cmdLen, char* cmdPara, app_u16 singRes);
private:
    bool _checkAct(STSubActInfo& stActInfo);

protected:
    app_u32 m_ulActIndex;
    std::map<app_u32, CSubAct*> m_mSubAct;
    std::map<app_u32, std::map<app_u32, ActFun> > m_mActFunc;
    std::map<app_u32, std::map<app_u32, ProcResFuc> > m_mResFuncs;
    ProcResFuc m_mAlarmFunc;
};
extern map<app_u32, string> g_mSubSys;
extern map<app_u32, string> g_mAct;