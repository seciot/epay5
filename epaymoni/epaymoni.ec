/*****************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台系统状态监控模块
** 创 建 人：冯炜
** 创建日期：2012/10/30
**
** $Revision: 1.8 $
** $Log: epaymoni.ec,v $
** Revision 1.8  2013/06/08 05:30:57  fengw
**
** 1、修改数据库连接机制，每次查询系统状态前，检测数据库连接，如已断开，自动重连。
**
** Revision 1.7  2013/06/05 02:16:29  fengw
**
** 1、增加系统状态监控详细信息文件删除。
**
** Revision 1.6  2012/12/21 02:08:15  fengw
**
** 1、增加Revision、Log。
**
*******************************************************************/

#include "epaymoni.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    OCIClobLocator* clobLoc;                    /* 系统状态详细信息文件 CLOB */
    int     iOffset;                            /* 文件偏移位置 */
    int     iFileLen;                           /* 文件长度 */
    char    szFileBuf[1024];                    /* 文件缓存BUF */
    char    szMoniTime[14+1];                   /* 监控日期时间 */
    int     iHostNo;                            /* 服务器编号 */
    char    szHostName[32+1];                   /* 服务器名称 */
    char    szProcStatus[1024+1];               /* 进程状态信息 */
    char    szMsgStatus[1024+1];                /* 消息队列状态信息 */
    char    szCommStatus[1024+1];               /* 通讯端口状态信息 */
EXEC SQL END DECLARE SECTION;

void _proc_exit(int iSigNo)
{
    CloseDB();

    exit(0);
}

int main(int argc, char* argv[])
{
    int     i;
    int     iFlag;                              /* 成功标志 */
    int     iIntervalTime;                      /* 监控间隔时间 */
    char    szTmpBuf[64+1];                     /* 临时变量 */
    char    szFileName[128+1];                  /* 文件名 */

    /* 读取环境变量，获取机器名 */
    memset(szHostName, 0, sizeof(szHostName));
    if(getenv("HOSTNAME") != NULL)
    {
        strcpy(szHostName, getenv("HOSTNAME"));
    }

    /* 读取参数 */
    /* 读取当期服务器编号 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));

    if(ReadConfig(CONFIG_FILENAME, SECTION_PUBLIC, "HOST_NO", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_PUBLIC, "HOST_NO");

        return FAIL;
    }
    iHostNo = atoi(szTmpBuf);

    /* 检查服务器编号 */
    if(iHostNo <= 0)
    {
        WriteLog(ERROR, "获取主机[%s]服务器编号失败!", szHostName);

        return FAIL;
    }

    /* 读取监控时间间隔 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, "TIME_INTERVAL", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_EPAYMONI, "TIME_INTERVAL");

        return FAIL;
    }

    iIntervalTime = atoi(szTmpBuf);

    if(iIntervalTime < MIN_MONI_INTERVAL)
    {
        WriteLog(ERROR, "TIME_INTERVAL参数值[%d]小于最低值[%d]，默认取最低值!",
            iIntervalTime, MIN_MONI_INTERVAL);

        iIntervalTime = MIN_MONI_INTERVAL;
    }

    /* 生成精灵进程, 使进程与终端脱节 */
    /* 与终端脱节后,scanf等函数将无法使用 */
    switch(fork())
    {
        case 0 :
            break;
        case -1 :
            exit(-1);
        default :
            exit(0);
    }

    for(i=0;i<32;i++)
    {
        if(i == SIGALRM || i == SIGKILL ||
           i == SIGUSR1 || i == SIGUSR2)
        {
            continue;
        }

        signal(i, SIG_IGN);
    }

    signal(SIGUSR1, _proc_exit);

    /* Make a back process */
    setpgrp();

    while(1)
    {
        if(ChkDBLink() != SUCC && OpenDB() != SUCC)
        {
            WriteLog(ERROR, "打开数据库失败!");

            sleep(10);

            continue;
        }

        /* 获取当期日期时间 */
        memset(szMoniTime, 0, sizeof(szMoniTime));

        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        GetSysDate(szTmpBuf);
        memcpy(szMoniTime, szTmpBuf, 8);

        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        GetSysTime(szTmpBuf);
        memcpy(szMoniTime+8, szTmpBuf, 6);

        /* 生成保存详细信息文件名 */
        memset(szFileName, 0, sizeof(szFileName));

        sprintf(szFileName, "/tmp/%s_%s.txt", "EPAYStatus", szMoniTime);

        if(GetEpayStatus(szFileName, szProcStatus, szMsgStatus, szCommStatus) == SUCC)
        {
            BeginTran();

            /* 默认状态为成功 */
            iFlag = SUCC;

            /* 获取数据库系统日期 */
            memset(szMoniTime, 0, sizeof(szMoniTime));
            EXEC SQL SELECT TO_CHAR(SYSDATE,'YYYYMMDDHH24MISS') INTO :szMoniTime FROM dual;
            if(SQLCODE)
            {
                WriteLog(ERROR, "获取数据库系统日期时间失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

                iFlag = FAIL;
            }
            else
            {
                /* 插入系统状态记录 */
                EXEC SQL
                    INSERT INTO epay_moni (moni_time, host_no, host_name, proc_status,
                                           msg_status, comm_status, sys_info)
                    VALUES (:szMoniTime, :iHostNo, :szHostName, :szProcStatus,
                            :szMsgStatus, :szCommStatus, empty_clob());
                if(SQLCODE)
                {
                    WriteLog(ERROR, "插入系统状态监控表失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

                    iFlag = FAIL;
                }
                else
                {
                    /* 更新CLOB对象 */
                    if(UpdateClob(szFileName) != SUCC)
                    {
                        iFlag = FAIL;
                    }
                }
            }

            if(iFlag == SUCC)
            {
                CommitTran();
            }
            else
            {
                RollbackTran();
            }
        }
        else
        {
            /* 记录错误日志 */
            WriteLog(ERROR, "生成系统状态监控数据失败!");
        }

        /* 删除文件 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        sprintf(szTmpBuf, "rm -f %s", szFileName);

        system(szTmpBuf);

        /* 等待下次监控 */
        sleep(iIntervalTime);
    }
}

/****************************************************************
** 功    能：更新CLOB大对象
** 输入参数：
**        szFileName        详细信息文件名
** 输出参数：
**        无
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
int UpdateClob(char *szFileName)
{
    int     iRet;                   /* 函数执行结果 */

    /* 更新CLOB字段 */
    /* 为CLOB定位符分配内存 */
    EXEC SQL ALLOCATE :clobLoc;
    if(SQLCODE)
    {
        WriteLog(ERROR, "为CLOB定位符分配内存失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    /* 获取CLOB定位符 */
    EXEC SQL
        SELECT sys_info INTO :clobLoc FROM epay_moni
        WHERE moni_time = :szMoniTime AND host_no = :iHostNo FOR UPDATE;
    if(SQLCODE)
    {
        WriteLog(ERROR, "获取CLOB定位符失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        /* 释放clob定位符占用的内存 */
        EXEC SQL FREE :clobLoc;

        return FAIL;
    }

    /* 获得CLOB列数据长度 */
    EXEC SQL LOB DESCRIBE :clobLoc GET LENGTH INTO :iOffset;
    if(SQLCODE)
    {
        WriteLog(ERROR, "获取CLOB列数据长度失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        /* 释放clob定位符占用的内存 */
        EXEC SQL FREE :clobLoc;

        return FAIL;
    }

    /* 位移加一 */
    iOffset = iOffset + 1;

    /* 打开文件 */
    fpStatusFile = fopen(szFileName, "r");
    if(fpStatusFile == NULL)
    {
        WriteLog(ERROR, "打开文件[%s]失败!", szFileName);

        /* 释放clob定位符占用的内存 */
        EXEC SQL FREE :clobLoc;

        return FAIL;
    }

    while(1)
    {
        memset(szFileBuf, 0, sizeof(szFileBuf));

        if(fgets(szFileBuf, 1024, fpStatusFile) == NULL)
        {
            break;
        }

        iFileLen = strlen(szFileBuf);

        /* 写入CLOB */
        EXEC SQL LOB WRITE :iFileLen FROM :szFileBuf INTO :clobLoc AT :iOffset;
        if(SQLCODE)
        {
            WriteLog(ERROR, "写入CLOB数据失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            /* 释放clob定位符占用的内存 */
            EXEC SQL FREE :clobLoc;

            fclose(fpStatusFile);

            return FAIL;
        }

        iOffset += iFileLen;
    }

    /* 释放clob定位符占用的内存 */
    EXEC SQL FREE :clobLoc;

    fclose(fpStatusFile);

    return SUCC;
}
