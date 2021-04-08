#ifndef _ActResHandler_h_
#define _ActResHandler_h_

#include <map>
#include "vos_messagecenter.h"
#include "app_comstructdefine.h"
#include "app_msg.h"

using namespace std;

class CResHandler
{
    typedef void (CResHandler::*ProcActResult)(int iResult);
public:
    CResHandler();
    ~CResHandler();
public:
    vos_u32 ProcActRes(vos_msg_header_stru* pstvosmsg);

    void SetAutoWash(){ m_bAutoWash = true; }

private:
    void _initProcFuc();

    void _procReactDskMove(int iResult);

    void _procSmpMixMove(int iResult);

    void _procSmpMixMix(int iResult);

    void _procReagMixMove(int iResult);

    void _procReagMixMix(int iResult);

    void _procR1Move(int iResult);

    void _procR1Absorb(int iResult);

    void _procR1Discharge(int iResult);

    void _procR2Move(int iResult);

    void _procR2Absorb(int iResult);

    void _procR2Discharge(int iResult);

    void _procSmpNdlMove(int iResult);
	
	void _procSmpNdlMoveDilu(int iResult);

    void _procSmpNdlAbsorb(int iResult);
	
	void _procSmpNdlAbsorbDilu(int iResult);

    void _procSmpNdlDischarge(int iResult);
	
	void _procSmpNdlDischargeDilu(int iResult);

    void _procAutoWash(int iResult);

    void _procSmpDskMove(int iResult);

    void _procOuterDskMove(int iResult);

    void _procInnerDskMove(int iResult);

	void _procSampMixWashCtrl(int iResult);

	void _procReagMixWashCtrl(int iResult);
    //设置盘转动结果
    void _setDskMoveResult(EN_ActFlag& actDskFlag, int iResult);

protected:
    map<app_u16, map<app_u16,ProcActResult> >  m_procFucs;     //结果处理函数信息

    bool m_bAutoWash;            //自动清洗完成标识
    bool m_bSmpNdlWash;        //样本针清洗标识
    bool m_bR1NdlWash;               //R1清洗标识
    bool m_bR2NdlWash;               //R2清洗标识
};

extern CResHandler* g_pResHandler;

#endif
