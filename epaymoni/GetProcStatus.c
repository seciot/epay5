/*****************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台系统状态监控模块 获取进程信息
** 创 建 人：冯炜
** 创建日期：2012/10/30
**
** $Revision: 1.5 $
** $Log: GetProcStatus.c,v $
** Revision 1.5  2012/12/21 02:08:15  fengw
**
** 1、增加Revision、Log。
**
*******************************************************************/

#define _EXTERN_

#include "epaymoni.h"

/****************************************************************
** 功    能：检查进程状态
** 输入参数：
**        无
** 输出参数：
**        szChkStatus       状态检查结果
** 返 回 值：
**        SUCC              检查成功
**        FAIL              检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        状态详细信息直接写入文件
** 修改日志：
****************************************************************/
int GetProcStatus(char *szChkStatus)
{
    int     i;
    int     iProcCount;                 /* 待监控进程个数 */
    char    szTmpBuf[64+1];             /* 临时变量 */
    char    szProcName[32+1];           /* 进程名 */
    char    szCommnts[64+1];            /* 进程中文名 */
    char    szStatus[64+1];             /* 进程状态 */
    char    szInfo[4096+1];             /* 进程详细信息 */
    int     iIndex;                     /* 字符串索引 */

    /* 读取参数 */
    /* 获取待监控进程个数 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, "PROC_MON_COUNT", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_EPAYMONI, "PROC_MON_COUNT");

        return FAIL;
    }
    iProcCount = atoi(szTmpBuf);

    /* 判断待监控进程个数，小于等于0，返回错误 */
    if(iProcCount <=0)
    {
        WriteLog(ERROR, "PROC_MON_COUNT参数值[%d]不正确!", iProcCount);

        return FAIL;
    }

    /* 写详细信息文件，数据库sysinfo字段 */
    fprintf(fpStatusFile, "平台进程当前状态\x0D\x0A");
    fprintf(fpStatusFile, "********************************************************************************\x0D\x0A");
    fprintf(fpStatusFile, "USER    PID    PPID    PGID    TIME        PCPU    PMEM    STATUS    START        COMMAND\x0D\x0A");

    /* 写状态信息，数据库proc_status字段 */
    iIndex = 0;

    sprintf(szChkStatus, "%d|", iProcCount);
    iIndex += strlen(szChkStatus);

    /* 循环获取进程信息 */
    for(i=1;i<=iProcCount;i++)
    {
        /* 获取进程名 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szProcName, 0, sizeof(szProcName));
        sprintf(szTmpBuf, "PROC_NAME_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szProcName) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }

        /* 获取进程中文名 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szCommnts, 0, sizeof(szCommnts));
        sprintf(szTmpBuf, "PROC_COMMENTS_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szCommnts) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }

        memset(szStatus, 0, sizeof(szStatus));
        memset(szInfo, 0, sizeof(szInfo));
        if(ChkProcStatus(szProcName, szCommnts, szStatus, szInfo) != SUCC)
        {
            return FAIL;
        }

        /* 状态信息 2.进程状态 */
        memcpy(szChkStatus+iIndex, szStatus, strlen(szStatus));
        iIndex += strlen(szStatus);

        /* 详细信息文件 */
        fprintf(fpStatusFile, "%s", szInfo);
    }

    /* 状态信息 结束标志 */
    szChkStatus[iIndex] = '|';
    iIndex += 1;

    /* 详细信息文件  结束标志 */
    fprintf(fpStatusFile,  "********************************************************************************\x0D\x0A");

    return SUCC;
}

/****************************************************************
** 功    能：检查单个进程状态
** 输入参数：
**        szProcName        进程名
**        szComments        进程中文名
** 输出参数：
**        szStatus          状态信息
** 返 回 值：
**        SUCC              检查成功
**        FAIL              检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**
** 修改日志：
****************************************************************/
int ChkProcStatus(char *szProcName, char *szComments, char *szStatus, char *szInfo)
{
    int     i;
    FILE    *fp;                            /* FILE指针 */
    char    szCmd[256+1];                   /* 查询进程状态命令 */
    char    szField1[32+1];                 /* 结果第一域 进程用户 */
    char    szField2[32+1];                 /* 结果第二域 进程号 */
    char    szField3[32+1];                 /* 结果第三域 父进程号 */
    char    szField4[32+1];                 /* 结果第四域 进程组号 */
    char    szField5[32+1];                 /* 结果第五域 CPU占用时间 */
    char    szField6[32+1];                 /* 结果第六域 CPU占用百分比 */
    char    szField7[32+1];                 /* 结果第七域 内存占用百分比 */
    char    szField8[32+1];                 /* 结果第八域 进程状态 */
    char    szField9[1024+1];               /* 结果第九域 进程参数 */
    char    szField10[32+1];                /* 结果第十域 进程启动时间 */
    char    szTmpBuf[512+1];                /* 临时变量 */
    int     iProcCount;                     /* 进程个数 */
    int     iIndex;                         /* 字符串索引 */
    int     iTmp;                           /* 临时变量 */
    char    cStatus;                        /* 进程模块状态 */

    memset(szCmd, 0, sizeof(szCmd));

    sprintf(szCmd,    
            "ps -u `whoami` -o user,pid,ppid,pgid,time,pcpu,pmem,%s -o \"%%a|\" -o start | tr -s ' ' | awk '{if($9==\"%s\")print $0}'",
            cnPSStat, szProcName);

    fp = popen(szCmd, "r");
    if(fp == NULL)
    {
        WriteLog(ERROR, "执行popen失败!CMD:[%s]", szCmd);

        return FAIL;
    }

    /* 默认状态正常 */
    cStatus = STATUS_YES;
    iProcCount = 0;
    iIndex = 0;

    while(1)
    {
        memset(szField1, 0, sizeof(szField1));
        memset(szField2, 0, sizeof(szField2));
        memset(szField3, 0, sizeof(szField3));
        memset(szField4, 0, sizeof(szField4));
        memset(szField5, 0, sizeof(szField5));
        memset(szField6, 0, sizeof(szField6));
        memset(szField7, 0, sizeof(szField7));
        memset(szField8, 0, sizeof(szField8));
        memset(szField9, 0, sizeof(szField9));
        memset(szField10, 0, sizeof(szField10));

        if(fscanf(fp, "%s %s %s %s %s %s %s %s %[^|] |%[^\n]",
                  szField1, szField2, szField3, szField4, szField5, szField6,
                  szField7, szField8, szField9, szField10) != EOF)
        {
            memset(szTmpBuf, 0, sizeof(szTmpBuf));
            /* szField用于存放args字段内容，由于tomcat进程参数过长，所以使用1024长度保存 */
            /* 明细中最大只保存256长度内容，剩余内容自动截断 */
            if(strlen(szField9) > 256)
            {
                szField9[256] = 0x00;
            }

            sprintf(szTmpBuf, "%s    %s    %s    %s    %s    %s    %s    %s    %s    %s\x0D\x0A",
                    szField1, szField2, szField3, szField4, szField5, szField6,
                    szField7, szField8, szField10, szField9);

            memcpy(szInfo+iIndex, szTmpBuf, strlen(szTmpBuf));
            iIndex += strlen(szTmpBuf);

            /* 判断进程状态 */
            if(szField8[0] != cnProcStatus)
            {
                cStatus = STATUS_NO;
            }

            /* 判断进程占用CPU时间 */
            if(strcmp(szField5, "00:00:00") != 0)
            {
                cStatus = STATUS_NO;
            }

            /* 判断进程占用CPU百分比 */
            if(atof(szField6)- 5.0 > 0.001)
            {
                cStatus = STATUS_NO;
            }

            /* 判断进程占用内存百分比 */
            if(atof(szField7)- 5.0 > 0.001)
            {
                cStatus = STATUS_NO;
            }

            iProcCount++;
        }
        else
        {
            break;
        }
    }

    if(iProcCount == 0)
    {
        cStatus = STATUS_NO;
    }

    /* 进程状态信息 */
    sprintf(szStatus, "%c,%s,%s,%d#", cStatus, szProcName, szComments, iProcCount);

    pclose(fp);

    return SUCC;
}
