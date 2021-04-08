#ifndef __SINGLE_DWNCMD_H__
#define __SINGLE_DWNCMD_H__

#include "vos_pid.h"
#include "xc8002_middwnmachshare.h"

//�ض������봹ֱ�½�����
struct paraMode0
{
	char para0;//����
	char rsv0[1];
	app_u16 para1; //��Ӧ����Һ���������������������
	char rsv1[8];
};

//ͨ�����������һ��������app_u16���Ͳ����Ľṹ
struct paraMode1
{
	app_u16 para0;
	app_u16 para1;
};

class CChooseModeStrategy
{
public:
	//ѡ����ģʽ
	vos_u32 _chooseModeByActtype(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1);
	
};

//������������ṹ�����������Ϊ����ģʽ
class CConstructCmdMode0
{
public:
	//�����������
	vos_u32 _constructCmdPara(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1);
};

class CConstructCmdMode1
{
public:
	//�����������
	vos_u32 _constructCmdPara(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1);
};

#endif
