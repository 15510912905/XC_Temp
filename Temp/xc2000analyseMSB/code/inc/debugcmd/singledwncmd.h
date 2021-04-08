#ifndef __SINGLE_DWNCMD_H__
#define __SINGLE_DWNCMD_H__

#include "vos_pid.h"
#include "xc8002_middwnmachshare.h"

//特定用于针垂直下降命令
struct paraMode0
{
	char para0;//方向
	char rsv0[1];
	app_u16 para1; //反应杯内液体总体积，用于随量跟踪
	char rsv1[8];
};

//通用命令，可用于一个或两个app_u16类型参数的结构
struct paraMode1
{
	app_u16 para0;
	app_u16 para1;
};

class CChooseModeStrategy
{
public:
	//选择处理模式
	vos_u32 _chooseModeByActtype(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1);
	
};

//根据命令参数结构，将命令归类为几种模式
class CConstructCmdMode0
{
public:
	//构造命令参数
	vos_u32 _constructCmdPara(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1);
};

class CConstructCmdMode1
{
public:
	//构造命令参数
	vos_u32 _constructCmdPara(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1);
};

#endif
