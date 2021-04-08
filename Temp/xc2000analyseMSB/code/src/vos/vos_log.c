#include <stdlib.h>
#include <stdio.h>
#include "vos_log.h"
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include "vos_fid.h"
#include "upmsg.pb.h"
#include "msg.h"
#include <sstream>
#include "app_msg.h"
#include "apputils.h"

using namespace std;

#define MAX_LOG_LEN 4096
#define INFO_LEVEL 3
static vos_u32 g_uiLogLevel = 0;
extern vos_u32 g_ulmdprintswitch;

const char * GetCurDateStr()
{
    static char cDate[50] = { 0 };

    time_t  iCurTime;
    iCurTime = time(NULL);
    struct tm curr;

    curr = *localtime(&iCurTime);
    snprintf(cDate, 50, "%04d-%02d-%02d", curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday);
    return cDate;
}

CLog::CLog() :CCallBack(SPid_Log)
{
    _openFile();
    m_fp << GetCurTimeStr() << " 程序启动......" << endl;
}

CLog::~CLog()
{
    m_fp << GetCurTimeStr() << " 程序退出......" << endl;
    m_fp.close();
}

vos_u32 CLog::CallBack(vos_msg_header_stru* pMsg)
{
    if (MSG_WriteLog == pMsg->usmsgtype)
    {
        CMsg stMsg;
        if (!stMsg.ParseMsg(pMsg))
        {
            m_fp << GetCurTimeStr() << " 消息解析错误......" << endl;
            return VOS_OK;
        }
        STLog * pLog = dynamic_cast<STLog*>(stMsg.pPara);
        m_fp << pLog->szloginfo() << flush;
    }
    return VOS_OK;
}

void CLog::_openFile()
{
    ostringstream ss;
    ss << "/log/rcd/" << GetCurDateStr() << ".log";

    const char *pTraceDir = GetExePath();
    char chFile[MAX_PATH] = { 0 };
    sprintf(chFile, "%s%s", pTraceDir, ss.str().c_str());
    m_fp.open(chFile, ios::app);
    if (!m_fp.is_open())
    {
        createPath(chFile);
        m_fp.open(chFile, ios::app);
    }
}

char * GetExePath()
{
    vos_u32 ulZoneflag = APP_NO;
    vos_u32 ulLoop = 0;
    int count = MAX_PATH;
    static TFName buf = "";
    if (0 != strcmp(buf, ""))
    {
        return buf;
    }
    int iResult = readlink("/proc/self/exe", buf, count - 1);
    if (iResult < 0 || (iResult >= count - 1))
    {
        return buf;
    }
    buf[iResult] = '\0';
    for (ulLoop = iResult; ulLoop >= 0; ulLoop--)
    {
        if (buf[ulLoop] == '/')
        {
            if (APP_YES == ulZoneflag)
            {
                buf[ulLoop] = '\0';
                break;
            }
            ulZoneflag = APP_YES;
        }
    }
    return buf;
}

void createPath(char szLogPath[MAX_PATH])
{
    if (0 == szLogPath[0])
    {
        printf("CreatePath Error!\n");
        return;
    }
    char szTempLogPath[MAX_PATH];
    memset(szTempLogPath, 0, sizeof(szTempLogPath));
    strncpy(szTempLogPath, szLogPath, sizeof(szTempLogPath) - 1);
    int iTempLen = strlen(szTempLogPath);

    if ('/' == szTempLogPath[iTempLen - 1])
    {
        szTempLogPath[iTempLen - 1] = 0;
    }
    char *pTemp = szTempLogPath;
    char *pTemp1;
    char chTemp;
    while (1)
    {
        pTemp1 = strchr(pTemp, '/');
        if (0 == pTemp1)
        {
            mkdir(szTempLogPath, 0777);
            break;
        }
        if (0 == *(pTemp1 + 1))
        {
            break;
        }

        chTemp = *(pTemp1 + 1);
        *(pTemp1 + 1) = 0;
        if (0 != strcmp("../", szTempLogPath) && 0 != strcmp("./", szTempLogPath))
        {
            mkdir(szTempLogPath, 0777);
        }
        *(pTemp1 + 1) = chTemp;
        pTemp = pTemp1 + 1;
    }
}
void printLog(const char* msg, ...)
{
    if (0 == ((g_ulmdprintswitch >> ps_ADval) & 0x00000001))
    {
        char  aucdatabuf[MAX_LOG_LEN] = { '\0' };
        printf("%s ", GetCurTimeStr());
        va_list ag;
        va_start(ag, msg);
        vsnprintf(aucdatabuf, MAX_LOG_LEN, msg, ag);
        printf("%s", (char *)aucdatabuf);
        fflush(stdout);
        va_end(ag);
    }
}

void writeLog(vos_u32 uiLevel, const char* msg, ...)
{
    if (uiLevel <= g_uiLogLevel)
    {
        char szInfo[MAX_LOG_LEN] = { 0 };
        va_list ag;
        va_start(ag, msg);
        vsnprintf(szInfo, MAX_LOG_LEN, msg, ag);
        va_end(ag);
        ostringstream ss;
        ss << GetCurTimeStr() << szInfo;
        STLog stPara;
        stPara.set_szloginfo(ss.str());
        sendInnerMsg(SPid_Log, 0, MSG_WriteLog, &stPara);
    }
}

char * GetCurTimeStr()
{
    time_t  iCurTime;
    time(&iCurTime);

    static char s[50];
    struct tm curr;

    curr = *localtime(&iCurTime);

    sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
        curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday,
        curr.tm_hour, curr.tm_min, curr.tm_sec);
    return s;
}

void SetLogLevel(vos_u32 uiLogLevel)
{
    if (uiLogLevel > INFO_LEVEL)
    {
        printf("Input loglevel error.\n");
    }
    else
    {
        g_uiLogLevel = uiLogLevel;
    }
}

