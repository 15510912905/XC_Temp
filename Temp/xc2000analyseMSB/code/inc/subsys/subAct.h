#pragma once

#include "privatestruct.pb.h"
#include "vos_basictypedefine.h"
#include <deque>
#include <map>
#include "msg.h"
#include "app_msg.h"

#define INVALID_ACT (0xffff)
class CSubAct
{
public:
    CSubAct(app_u32 subSys);
    ~CSubAct();

public:
    void Initialize();
    void InsertAct(app_u32 actIndex, const STSubActInfo& stActInfo);
    void SetDone(app_u32 actType, bool bDone, app_u32 actIndex = 0);
    bool IsFinish();
    bool GetNextAct(STSubActInfo& stAct);
    void PopNextAct();
    bool CheckDone(app_u32 actType,app_u32 index);
    void ResetAct();
    void ClearAct();
    vos_u32 ReqDwmCmd(vos_u32 srcPid, vos_u32 dstPid, app_u16 actType, vos_u32 cmdLen, char* cmdPara, app_u16 singRes);
    app_u32 GetActIndex(){ return m_uActIndex; }
private:
    std::deque<STSubActInfo> m_qActInfo;
    std::map<app_u32, bool>m_mActs;//actType,act
    app_u32 m_uSubSys;
    app_u32 m_uActIndex;
    app_u32 m_uPreAct;//上一个动作
    bool m_bFault;
};