/******************************************************************
 ** Copyright(C)2009－2012 福建联迪商用设备有限公司
 ** 主要内容：日志文件的打开，关闭，重定向。
 ** 创 建 人：zhangwm
 ** 创建日期：2012/12/03
 **
 ** ---------------------------------------------------------------
 **   $Revision: 1.12 $
 **   $Log: LogPrint.c,v $
 **   Revision 1.12  2012/12/11 02:14:48  fengw
 **
 **   1、增加对读取LOG_SWITCH环境变量判断，如果未定义LOG_SWITCH，则默认打印所有日志。
 **
 **   Revision 1.11  2012/11/29 07:02:31  zhangwm
 **
 **   增加是否打印日志控制
 **
 **   Revision 1.10  2012/11/29 01:56:41  zhangwm
 **
 **   增加设置交易信息进环境变量函数，供错误日志打印使用
 **
 **   Revision 1.9  2012/11/29 01:11:27  zhangwm
 **
 **   修改WriteETLog为WriteLog
 **
 **   Revision 1.8  2012/11/28 08:25:17  linqil
 **   *** empty log message ***
 **
 **   Revision 1.7  2012/11/28 08:24:53  linqil
 **   取掉了冗余头文件user.h
 **
 **   Revision 1.6  2012/11/27 09:26:21  zhangwm
 **
 **   修改条件操作系统条件判断Linux为LINUX
 **
 **   Revision 1.5  2012/11/27 08:39:40  zhangwm
 **
 **   用新的时间处理函数替换原来的
 **
 **   Revision 1.4  2012/11/27 06:13:41  zhangwm
 **
 **   将写APP日志功能移除
 **
 **   Revision 1.3  2012/11/27 03:36:31  zhangwm
 **
 **   修改头文件为内部使用
 **
 **   Revision 1.2  2012/11/26 06:45:35  zhangwm
 **
 **  将错误日志打印移植到公共库中
 **
 **   Revision 1.1  2012/11/20 03:27:37  chenjr
 **   init
 **
 ** ---------------------------------------------------------------
 **
 *******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef LINUX
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include "user.h"
#include "LogPrint.h"

/*****************************************************************
 ** 功    能：打开日志文件
 ** 输入参数：
 **        pszLogFile 日志文件名称
 **        iType 日志文件类型：    
 **            1、错误日志 2、跟踪日志
 **            3、16进制日志
 **            4、监控日志
 ** 输出参数：
 **        无
 ** 返 回 值：
 **          成功返回 0 
 **          失败返回 -1 
 ** 作    者：zhangwm
 ** 日    期：2012/12/03
 ** 修改日志：
 **          1、2012/12/03 初始创建 
 ****************************************************************/
int OpenLogFile( char* pszLogFile, int iType)
{
    FILE         *fpLogFile;
    FILE         *logfp;

    if ((fpLogFile=fopen(pszLogFile, "a+")) == NULL)
    {
        fprintf(stdout, "Open Log file[%s] Error\n", pszLogFile);
        return -1;
    }

    switch(iType)
    {
        case T_TYPE:
            if (fpTLog != NULL)
            {
                fclose(fpTLog);
            }
            fpTLog = fpLogFile;

            return 0; 
        case H_TYPE:
            if (fpHLog != NULL)
            {
                fclose(fpHLog);
            }

            fpHLog = fpLogFile;
            return 0;
        case M_TYPE:
            if (fpMLog != NULL)
            {
                fclose(fpMLog);
            }

            fpMLog = fpLogFile;
            return 0;
        default:
            break;
    }


#ifdef AIX
    if (dup2(fpLogFile->_file,2)<0)
    {
        fprintf( stdout, "\ndup2 log Error[%d-%s]\n", errno, strerror(errno));
        fclose( fpLogFile );
        return -1;
    }
#endif

#ifdef SCO_SV
    if ( dup2(fpLogFile->__file,2)<0 )
    {
        fprintf( stdout, "\ndup2 log Error[%d-%s]\n", errno, strerror(errno));
        fclose( fpLogFile );
        return -1;
    }
#endif

#ifdef LINUX
    if ( dup2(fileno(fpLogFile),2)<0 )
    {
        fprintf( stdout, "\ndup2 log Error[%d-%s]\n", errno, strerror(errno));
        fclose( fpLogFile );
        return -1;
    }
#endif

    return 0;
}

/*****************************************************************
 ** 功    能：写日志文件
 ** 输入参数：
 **        pszData 数据内容 
 **        iType 日志文件类型：    
 **            1、错误日志 2、跟踪日志
 **            3、16进制日志
 **            4、监控日志
 ** 输出参数：
 **        无
 ** 返 回 值：
 **          成功返回 0 
 **          失败返回 -1 
 ** 作    者：zhangwm
 ** 日    期：2012/12/03
 ** 修改日志：
 **          1、2012/12/03 初始创建 
 ****************************************************************/
int PrintLog(char* pszData, int iType)
{
    FILE* fpFile;
    int iLen;
    char szEnvKey[20], szDate[9];
    char szPath[PATH_LEN], szFileName[PATH_LEN], szOldFileName[PATH_LEN];

    switch(iType)
    {
        case E_TYPE:
            sprintf(szEnvKey, "__LOG_FILE_%s_", "E");
            sprintf(szFileName, "%s%s", getenv("WORKDIR"), E_LOG);
            break;
        case T_TYPE:
            sprintf(szEnvKey, "__LOG_FILE_%s_", "T");
            sprintf(szFileName, "%s%s", getenv("WORKDIR"), T_LOG);
            break;
        case H_TYPE:
            sprintf(szEnvKey, "__LOG_FILE_%s_", "H");
            sprintf(szFileName, "%s%s", getenv("WORKDIR"), H_LOG);
            break;
        case M_TYPE:
            sprintf(szEnvKey, "__LOG_FILE_%s_", "M");
            sprintf(szFileName, "%s%s", getenv("WORKDIR"), M_LOG);
            break;
        default:
            break;
    }
    GetSysDate(szDate);
    strcat(szFileName, szDate);

    /* 进程初次打印该日志，则打开日志文件 */
    if (getenv(szEnvKey) == NULL)
    {
        OpenLogFile(szFileName, iType);
        setenv(szEnvKey, szFileName, 1);
    }

    /* 当前日志文件名称于环境变量中的日志文件名称
       不一致，则说明过了一天，打开新文件名称 */
    strcpy(szOldFileName, getenv(szEnvKey));
    if (strcmp(szOldFileName, szFileName) != 0)
    {
        OpenLogFile(szFileName, iType);
        setenv(szEnvKey, szFileName, 1);
    }                

    iLen = strlen(pszData);

    switch(iType)
    {
        case T_TYPE:
            fpFile = fpTLog;
            break;
        case H_TYPE:
            fpFile = fpHLog;
            break;
        case M_TYPE:
            fpFile = fpMLog;
            break;
        default:
            break;
    }

    if (iType == E_TYPE)
    {
        fwrite(pszData, 1, iLen, stderr);
    }
    else
    {
        fwrite(pszData, 1, iLen, fpFile);
        fflush(fpFile);
    }

    return 0;
}

/*****************************************************************
 ** 功    能：打印错误日志和跟踪日志
 ** 输入参数：
 **        fpFile 文件指针
 **        pszData 数据内容 
 ** 输出参数：
 **        无
 ** 返 回 值：
 **        无 
 ** 作    者：zhangwm
 ** 日    期：2012/12/03
 ** 修改日志：
 **          1、2012/12/03 初始创建 
 ****************************************************************/
#ifdef LINUX   
void WriteLog( char* szFileInfo, int iLine, int iType, char* szFmt, ...)
#else                   
WriteLog ( szFileInfo, iLine, iType, szFmt, va_alist )
    char* szFileInfo; 
    int   iLine, iType;
    char* szFmt;
    va_dcl
#endif
{
    va_list args;
    int     iLen;
    FILE*    fpFile;
    char    szMDate[11], szMTime[13], szDate[11], szPsamNo[17];
    char    szLogData[DATA_LEN];

    if ((iType == T_TYPE) && ((IsPrint(DEBUG_TLOG))== NO))
    {
        return;
    }

    memset(szLogData, 0, sizeof(szLogData));

    if (getenv("__EPAY_TRANS_ID_") == NULL)
    {
        strcpy(szPsamNo, "FFFFFFFFFFFFFFFF");    
    }
    else
    {
        strcpy(szPsamNo, getenv("__EPAY_TRANS_ID_"));
    }

    GetSysDate(szDate);

#ifdef LINUX
    va_start(args, szFmt);
#else
    va_start(args);
#endif

    /* 该函数后期使用按C语言规范命名后的函数 */
    GetSysDTFmt("%F", szMDate);
    GetSysDTFmt("%T", szMTime);

    sprintf(szLogData, "%s %s %s(%d) %s: ", szMDate, szMTime, szFileInfo, iLine, szPsamNo);
    iLen = strlen(szLogData);

    vsprintf(szLogData + iLen, szFmt, args);
    strcat(szLogData, "\n");

    if (iType == E_ERROR)
    {
        PrintLog(szLogData, E_TYPE);
    }
    else
    {
        PrintLog(szLogData, T_TYPE);
    }

    return;
}

/*****************************************************************
 ** 功    能：设置日志中体现的交易信息
 ** 输入参数：
 **        pszTransId 需要设置进环境变量的字符串 
 ** 输出参数：
 **        无
 ** 返 回 值：
 **        无 
 ** 作    者：zhangwm
 ** 日    期：2012/12/03
 ** 修改日志：
 **          1、2012/12/03 初始创建 
 ****************************************************************/
int SetEnvTransId(char* pszTransId)
{
    if (pszTransId == NULL)
    {
        return FAIL;
    }

    setenv("__EPAY_TRANS_ID_", pszTransId, 1);
    
    return SUCC;
}

/*****************************************************************
 ** 功    能：判断日志类型是否需要打印
 ** 输入参数：
 **        iType 日志类型
 ** 输出参数：
 **        无
 ** 返 回 值：
 **        无 
 ** 作    者：zhangwm
 ** 日    期：2012/12/03
 ** 修改日志：
 **          1、2012/12/03 初始创建 
 ****************************************************************/
int IsPrint(int iType)
{
    char szDebug[4];
    int iDebug;

    if(getenv("LOG_SWITCH") == NULL)
    {
        iDebug = 15;    
    }
    else
    {
        strcpy(szDebug, getenv("LOG_SWITCH"));
        iDebug = atoi(szDebug);
    }
    if((iDebug & iType) == NO)
    {
        return NO;
    }

    return YES;
}
