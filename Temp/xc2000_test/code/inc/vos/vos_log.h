#ifndef _app_log_h_
#define _app_log_h_

#include "vos_messagecenter.h"
#include "app_innermsg.h"
#include "app_downmsg.h"
#include "unistd.h"
#include "app_msgtypedefine.h"
#include "vos_adaptsys.h"
#include <string.h>
#include <fstream>
#include "vos_pid.h"

#define  SPid_Log     601                        //日志模块Pid
#define MAX_PATH   1024                       /*文件的最大路径名*/
typedef char TFName[MAX_PATH];

class CLog : public CCallBack
{
public:
    CLog();
    ~CLog();

public:
    vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    void _openFile();

private:
    std::ofstream m_fp;
};

void SetLogLevel(vos_u32 uiLogLevel);

//获取工作路径
char * GetExePath();

//创建文件
void createPath(char szLogPath[MAX_PATH]);

//打印日志信息
void writeLog(vos_u32 uiLevel, const char* msg, ...);

//打印空日志
void printLog(const char* msg, ...);

//获取当前时间
char * GetCurTimeStr();

//打印错误日志
#define WRITE_ERR_LOG(fmt, args...) \
    printLog("[ERROR][%s(%d)] " fmt, vos_filterpathforfile((char*)__FILE__), __LINE__, ##args);\
    writeLog(0,"[ERROR][%s(%d)] " fmt, vos_filterpathforfile((char*)__FILE__), __LINE__, ##args);

//打印告警日志
#define WRITE_TRACE_LOG(fmt, args...) \
    writeLog(1,fmt, ##args);

//打印告警日志
#define WRITE_WARN_LOG(fmt, args...) \
    writeLog(2,"[WARN][%s(%d)] " fmt, vos_filterpathforfile((char*)__FILE__), __LINE__, ##args);

//打印正常日志
#define WRITE_INFO_LOG(fmt, args...) \
    writeLog(2,"[INFO][%s(%d)] " fmt, vos_filterpathforfile((char*)__FILE__), __LINE__, ##args);
#endif
