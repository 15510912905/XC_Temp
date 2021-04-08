#ifndef _AD_SIMULATE_h_
#define _AD_SIMULATE_h_

#include "adsimulate.pb.h"
#include "vos_basictypedefine.h"

class CAdSimulate
{
public:
    CAdSimulate();
    ~CAdSimulate();

public:
    vos_u32 LoadCfg();
    vos_u32 GetAdValue(vos_u32 taskId, vos_u32 channelIdx, int index);

private:
    ADSimulate m_stAdSimulate;
};

#ifdef COMPILER_IS_LINUX_GCC
extern CAdSimulate * g_pADSimulate;
#endif

#endif
