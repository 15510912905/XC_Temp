#ifndef __OAMMNG_H__
#define __OAMMNG_H__

#include "vos_pid.h"
#include "msg.h"
#include "compensation.pb.h"

#define  TEMPQUERY_TIME_LEN 20

class COamMng : public CCallBack
{
    typedef vos_u32(COamMng::*ProcMsg)(vos_msg_header_stru* pMsg);
    
public:
    COamMng(vos_u32 ulPid);
    ~COamMng();

public:
    //消息入口
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

    //初始化设备
    void InitMachine();

private:
    //初始化消息处理函数
    void _initProcFuc();

    //初始化需要发送停止命令的模块
    void _initStopPid();

    //停止机器运行
    vos_u32 _procStopMach(vos_msg_header_stru* pMsg);

    //按键旋转试剂外盘
    vos_u32 _procKeyMoveOuterDsk(vos_msg_header_stru* pMsg);

    // 按键旋转试剂内盘
    vos_u32 _procKeyMoveInnerDsk(vos_msg_header_stru* pMsg);

    //转到试剂盘
    vos_u32 _moveReagDsk(vos_u32 dstPid, char ucCupNum);

    //处理超时
    vos_u32 _procTimeOut(vos_msg_header_stru* pMsg);

    //处理温度查询
    vos_u32 _procQueryTemp(vos_msg_header_stru* pMsg);

    //获取中位机日志
    vos_u32 _procGetMidLog(vos_msg_header_stru* pMsg);

    //处理整机复位响应
    vos_u32 _procResetRes(vos_msg_header_stru* pMsg);

    //打开光源灯
    vos_u32 _setLampBrightness();
    //打开温控 切换正常态
    vos_u32 _openControl();

    //补偿下发
    void _setOffset(CMcu *pCMcu);
    //处理补偿恢复请求
    vos_u32 _procCompRecover(vos_msg_header_stru* pMsg);
    vos_u32 _procMotorRecover(vos_msg_header_stru* pMsg);

    void _saveMotor(const STSubSysMotor& pReqMotor);

    //处理补偿备份请求
    vos_u32 _procReqCompBack(vos_msg_header_stru* pMsg);
    vos_u32 _procReqMotorBack(vos_msg_header_stru* pMsg);

    //处理补偿返回
    vos_u32 _procResCompBack(vos_msg_header_stru* pMsg);
    vos_u32 _procResMotorBack(vos_msg_header_stru* pMsg);

    
    //查找补偿
    CMcu* _findCompData(vos_u32 comptype, vos_u32 subsysid);
    //处理补偿参数
    void _saveOffsetPara(const STOffsetPara & pPara, vos_u32 subsysid);

    //处理下位机命令响应
    vos_u32 _procDwnRes(vos_msg_header_stru* pMsg);

	vos_u32 _procReactDiskTemp(vos_msg_header_stru* pMsg);

private:
    map<app_u16, ProcMsg>  m_procFucs;     //消息处理函数集合
    vector<vos_u32> m_vStopPids;    //需要停止的模块的pid

    vos_u32 m_ulSrcPid;
    vos_u32 m_ulSrcIp;
    vos_u32 m_ulInitTimer;
    CMcuList m_compBack;
    CCompSetRes m_compSetRes;
    STMotorList m_motorList;
    int m_waitCount;
public:    
    static vos_u32 m_ulTimer;     //定时器
    static vos_u32 CompBackFlg;
};

#endif