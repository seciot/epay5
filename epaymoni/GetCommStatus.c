/*****************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台系统状态监控模块 获取消息队列信息
** 创 建 人：冯炜
** 创建日期：2012/10/30
**
** $Revision: 1.5 $
** $Log: GetCommStatus.c,v $
** Revision 1.5  2012/12/21 02:08:15  fengw
**
** 1、增加Revision、Log。
**
*******************************************************************/

#define _EXTERN_

#include "epaymoni.h"

/****************************************************************
** 功    能：检查通讯端口状态
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
int GetCommStatus(char *szChkStatus)
{
    int     i;
    int     iIndex;
    int     iCommCount;             /* 待监控通讯端口个数 */
    char    szTmpBuf[64+1];         /* 临时变量 */
    char    szValueBuf[64+1];       /* 保存参数配置临时变量 */
    char    szComments[32+1];       /* 通讯端口中文名称 */
    int     iType;                  /* 通讯类型 */
    char    szServIP[15+1];         /* 服务端IP地址 */
    int     iServPort;              /* 服务端监听端口 */
    int     iLocalPort;             /* 本地监听端口 */
    char    szStatus[64+1];         /* 通讯端口状态信息 */
    char    cCommStatus;            /* 通讯端口状态 */

    /* 获取待监听通讯端口个数 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, "COMM_MON_COUNT", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_EPAYMONI, "COMM_MON_COUNT");

        return FAIL;
    }
    iCommCount = atoi(szTmpBuf);
    
    /* 判断待监控通讯端口个数，小于等于0，返回错误 */
    if(iCommCount <=0)
    {
        WriteLog(ERROR, "COMM_MON_COUNT参数值[%d]不正确!", iCommCount);

        return FAIL;
    }

    /* 写详细信息文件，数据库sysinfo字段 */
    fprintf(fpStatusFile, "通讯端口状态\x0D\x0A");
    fprintf(fpStatusFile, "********************************************************************************\x0D\x0A");

    /* 写状态信息，数据库comm_status字段 */
    iIndex = 0;

    /* 状态信息 1.通讯端口个数*/
    sprintf(szChkStatus, "%d|", iCommCount);
    iIndex += strlen(szChkStatus);

    /* 循环获取通讯端口状态信息 */
    for(i=1;i<=iCommCount;i++)
    {    
        /* 获取通讯端口中文名 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szComments, 0, sizeof(szComments));
        sprintf(szTmpBuf, "COMM_COMMENTS_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szComments) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }

        /* 获取通讯端口类型 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szValueBuf, 0, sizeof(szValueBuf));
        sprintf(szTmpBuf, "COMM_TYPE_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szValueBuf) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }
        iType = atoi(szValueBuf);

        /* 获取服务器IP地址 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szServIP, 0, sizeof(szServIP));
        sprintf(szTmpBuf, "COMM_SERV_IP_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szServIP) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }

        /* 获取服务器监听端口 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szValueBuf, 0, sizeof(szValueBuf));
        sprintf(szTmpBuf, "COMM_SERV_PORT_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szValueBuf) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }
        iServPort = atoi(szValueBuf);

        /* 获取本地监听端口 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szValueBuf, 0, sizeof(szValueBuf));
        sprintf(szTmpBuf, "COMM_LOCAL_PORT_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szValueBuf) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }
        iLocalPort = atoi(szValueBuf);

        /* 通讯状态默认为Y */
        cCommStatus = STATUS_YES;

        switch(iType)
        {
            /* 双工长链服务端 */
            case DUPLEX_KEEPALIVE_SERV:
                if(ChkDKSStatus(szComments, iLocalPort, &cCommStatus) != SUCC)
                {
                    return FAIL;
                }
                break;
            /* 双工长链客户端 */
            case DUPLEX_KEEPALIVE_CLIT:
                if(ChkDKCStatus(szComments, szServIP, iServPort, &cCommStatus) != SUCC)
                {
                    return FAIL;
                }
                break;
            /* 单工长链 */
            case SIMPLEX_KEEPALIVE:
                if(ChkSKStatus(szComments, iLocalPort, szServIP, iServPort, &cCommStatus) != SUCC)
                {
                    return FAIL;
                }
                break;
            /* 双工短链服务端 */
            case DUPLEX_SERVER:
                if(ChkDSStatus(szComments, iLocalPort, &cCommStatus) != SUCC)
                {
                    return FAIL;
                }            
                break;
            default:
                return FAIL;
        }

        /* 状态信息 2.通讯端口状态 */
        memset(szStatus, 0, sizeof(szStatus));
        sprintf(szStatus, "%c,%s#", cCommStatus, szComments);

        memcpy(szChkStatus+iIndex, szStatus, strlen(szStatus));
        iIndex += strlen(szStatus);

        /* 详细信息文件 换行 */
        fprintf(fpStatusFile, "\x0D\x0A");
    }

    /* 状态信息 结束标志 */
    szChkStatus[iIndex] = '|';
    iIndex += 1;

    /* 详细信息文件 结束标志 */
    fprintf(fpStatusFile, "********************************************************************************\x0D\x0A");

    return SUCC;
}

/****************************************************************
** 功    能：检查双工长链服务端状态
** 输入参数：
**        szComments            通讯中文名称
**        iLocalPort            本地监听端口
** 输出参数：
**        pcCommStatus          链接状态 Y：正常 N：异常
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        1.检查服务端端口监听状态
**        2.检查客户端链接状态
** 修改日志：
****************************************************************/
int ChkDKSStatus(char *szComments, int iLocalPort, char *pcCommStatus)
{
    char szInfo[1024+1];
    char cStatus;

    /* 检查监听状态 */
    memset(szInfo, 0, sizeof(szInfo));
    if(ChkServListen(iLocalPort, szInfo, &cStatus) != SUCC)
    {
        return FAIL;
    }

    /* 判断状态 */
    if(cStatus == 'N')
    {
        *pcCommStatus = cStatus;
    }

    /* 将状态信息写入详细文件 */
    fprintf(fpStatusFile, "%s 监听端口:[%d]\x0D\x0A", szComments, iLocalPort);
    fprintf(fpStatusFile, "监听状态:\x0D\x0A");
    fprintf(fpStatusFile, "%s", szInfo);

    /* 检查客户端链接状态 */
    memset(szInfo, 0, sizeof(szInfo));
    if(ChkClitConnection(iLocalPort, szInfo, &cStatus) != SUCC)
    {
        return FAIL;
    }

    /* 判断状态 */
    if(cStatus == 'N')
    {
        *pcCommStatus = cStatus;
    }

    /* 将状态信息写入详细文件 */
    fprintf(fpStatusFile, "链接状态:\x0D\x0A");
    fprintf(fpStatusFile, "%s", szInfo);

    return SUCC;
}

/****************************************************************
** 功    能：检查双工长链客户端状态
** 输入参数：
**        szComments            通讯中文名称
**        szServIP              服务器IP地址
**        iServPort             服务器端口
** 输出参数：
**        pcCommStatus          链接状态 Y：正常 N：异常
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        1.检查与服务端链接状态
** 修改日志：
****************************************************************/
int ChkDKCStatus(char *szComments, char *szServIP, int iServPort, char *pcCommStatus)
{
    char    szInfo[1024+1];             /* 端口状态信息 */
    char    cStatus;                    /* 端口状态 */

    /* 检查与服务端链接状态 */
    memset(szInfo, 0, sizeof(szInfo));
    if(ChkServConnection(szServIP, iServPort, szInfo, &cStatus) != SUCC)
    {
        return FAIL;
    }

    /* 判断状态 */
    if(cStatus == 'N')
    {
        *pcCommStatus = cStatus;
    }

    /* 将状态信息写入详细文件 */
    fprintf(fpStatusFile, "%s 服务器地址:[%s:%d]\x0D\x0A", szComments, szServIP, iServPort);
    fprintf(fpStatusFile, "链接状态:\x0D\x0A");
    fprintf(fpStatusFile, "%s", szInfo);

    return SUCC;
}

/****************************************************************
** 功    能：检查单工长链状态
** 输入参数：
**        szComments            通讯中文名称
**        iLocalPort            本地监听端口
**        szServIP              服务器IP地址
**        iServPort             服务器端口
** 输出参数：
**        pcCommStatus          链接状态 Y：正常 N：异常
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        1.检查服务端端口监听状态
**        2.检查与服务端链接状态
**        3.检查客户端链接状态
** 修改日志：
****************************************************************/
int ChkSKStatus(char *szComments, int iLocalPort, char *szServIP, int iServPort, char *pcCommStatus)
{
    char    szInfo[1024+1];             /* 端口状态信息 */
    char    cStatus;                    /* 端口状态 */
    
    /* 检查监听状态 */
    memset(szInfo, 0, sizeof(szInfo));
    if(ChkServListen(iLocalPort, szInfo, &cStatus) != SUCC)
    {
        return FAIL;
    }

    /* 判断状态 */
    if(cStatus == 'N')
    {
        *pcCommStatus = cStatus;
    }

    /* 将状态信息写入详细文件 */
    fprintf(fpStatusFile, "%s 本地端口:[%d] 服务器地址:[%s:%d]\x0D\x0A", szComments, iLocalPort, szServIP, iServPort);
    fprintf(fpStatusFile, "监听状态:\x0D\x0A");
    fprintf(fpStatusFile, "%s", szInfo);

    /* 检查与服务端链接状态 */
    memset(szInfo, 0, sizeof(szInfo));
    if(ChkServConnection(szServIP, iServPort, szInfo, &cStatus) != SUCC)
    {
        return FAIL;
    }

    /* 判断状态 */
    if(cStatus == 'N')
    {
        *pcCommStatus = cStatus;
    }

    /* 将状态信息写入详细文件 */
    fprintf(fpStatusFile, "链接状态:\x0D\x0A");
    fprintf(fpStatusFile, "%s", szInfo);

    /* 检查客户端链接状态 */
    memset(szInfo, 0, sizeof(szInfo));
    if(ChkClitConnection(iLocalPort, szInfo, &cStatus) != SUCC)
    {
        return FAIL;
    }

    /* 判断状态 */
    if(cStatus == 'N')
    {
        *pcCommStatus = cStatus;
    }

    /* 将状态信息写入详细文件 */
    fprintf(fpStatusFile, "%s", szInfo);

    return SUCC;
}

/****************************************************************
** 功    能：检查双工短链服务端状态
** 输入参数：
**        szComments            通讯中文名称
**        iLocalPort            本地监听端口
** 输出参数：
**        pcCommStatus          链接状态 Y：正常 N：异常
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        1.检查服务端端口监听状态
** 修改日志：
****************************************************************/
int ChkDSStatus(char *szComments, int iLocalPort, char *pcCommStatus)
{
    char szInfo[1024+1];             /* 端口状态信息 */
    char cStatus;                    /* 端口状态 */
    
    /* 检查监听状态 */
    memset(szInfo, 0, sizeof(szInfo));
    if(ChkServListen(iLocalPort, szInfo, &cStatus) != SUCC)
    {
        return FAIL;
    }

    /* 判断状态 */
    if(cStatus == 'N')
    {
        *pcCommStatus = cStatus;
    }

    /* 将状态信息写入详细文件 */
    fprintf(fpStatusFile, "%s 监听端口:[%d]\x0D\x0A", szComments, iLocalPort);
    fprintf(fpStatusFile, "监听状态:\x0D\x0A");
    fprintf(fpStatusFile, "%s", szInfo);

    return SUCC;
}

/****************************************************************
** 功    能：检查服务端链接状态
** 输入参数：
**        iPort                 本地监听端口
** 输出参数：
**        szInfo                链接信息
**        pcStatus              链接状态 Y：正常 N：异常
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        当端口处于监听状态，且发送、接收均无数据阻塞，则状态为Y，否则为N
** 修改日志：
****************************************************************/
int ChkServListen(int iPort, char *szInfo, char *pcStatus)
{
    FILE *fp;
    char szCmd[256+1];
    char szField1[32+1], szField4[32+1], szField5[32+1], szField6[32+1];
    int iRecvQ, iSendQ;
    
    memset(szCmd, 0, sizeof(szCmd));
    
    sprintf(szCmd, "netstat -an | grep %d | awk '{if($4==\"%s%c%d\" && $6==\"LISTEN\") print $0}'",
        iPort, cnServListenIP, cnSplit, iPort);

    fp = popen(szCmd, "r");
    if(fp == NULL)
    {
        WriteLog(ERROR, "执行popen失败!CMD:[%s]", szCmd);
        return FAIL;
    }

    /* 读取shell执行结果 */
    memset(szField1, 0, sizeof(szField1));
    memset(szField4, 0, sizeof(szField4));
    memset(szField5, 0, sizeof(szField5));
    memset(szField6, 0, sizeof(szField6));
    
    /* 默认状态正常 */
    *pcStatus = STATUS_YES;

    if(fscanf(fp, "%s %d %d %s %s %s",
        szField1, &iRecvQ, &iSendQ, szField4, szField5, szField6) != EOF)
    {
        /* 判断是否有数据阻塞 */
        if(iRecvQ > 0 || iSendQ > 0)
        {
            *pcStatus = STATUS_NO;
        }
        
        sprintf(szInfo, "%s    %d    %d    %s                %s                %s\x0D\x0A",
            szField1, iRecvQ, iSendQ, szField4, szField5, szField6);
    }
    else
    {
        *pcStatus = STATUS_NO;
    }

    pclose(fp);

    return SUCC;
}

/****************************************************************
** 功    能：检查服务端链接状态
** 输入参数：
**        szIP              服务端IP地址
**        iPort             服务端监听端口
** 输出参数：
**        szInfo            链接信息
**        pcStatus          链接状态 Y：正常 N：异常
** 返 回 值：
**        SUCC              检查成功
**        FAIL              检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        当与服务端建立链接，且发送、接收均无数据阻塞，则链接状态为Y，否则为N
** 修改日志：
****************************************************************/
int ChkServConnection(char *szIP, int iPort, char *szInfo, char *pcStatus)
{
    FILE    *fp;                    /* 文件指针 */
    char    szCmd[256+1];           /* 端口状态查询命令 */
    char    szField1[32+1];         /* 结果第一域 协议类型 */
    int     iRecvQ;                 /* 通讯接收队列 */
    int     iSendQ;                 /* 通讯发送队列 */
    char    szField4[32+1];         /* 结果第四域 服务端IP、端口 */
    char    szField5[32+1];         /* 结果第五域 客户端IP、端口 */
    char    szField6[32+1];         /* 结果第六域 状态 */ 
    char    szTmpBuf[128+1];        /* 临时变量 */
    int     iConnCount;             /* 链接个数 */
    int     iIndex;                 /* 字符串索引 */

    memset(szCmd, 0, sizeof(szCmd));

    sprintf(szCmd,
            "netstat -an | grep %d | awk '{if($5==\"%s%c%d\" && $6==\"ESTABLISHED\") print $0}'",
            iPort, szIP, cnSplit, iPort);

    fp = popen(szCmd, "r");
    if(fp == NULL)
    {
        WriteLog(ERROR, "执行popen失败!CMD:[%s]", szCmd);

        return FAIL;
    }

    /* 读取shell执行结果 */
    memset(szField1, 0, sizeof(szField1));
    memset(szField4, 0, sizeof(szField4));
    memset(szField5, 0, sizeof(szField5));
    memset(szField6, 0, sizeof(szField6));

    /* 默认状态正常 */
    iConnCount = 0;
    iIndex = 0;
    *pcStatus = STATUS_YES;

    while(1)
    {
        if(fscanf(fp, "%s %d %d %s %s %s",
            szField1, &iRecvQ, &iSendQ, szField4, szField5, szField6) != EOF)
        {
            /* 判断是否有数据阻塞 */
            if(iRecvQ > 0 || iSendQ > 0)
            {
                *pcStatus = STATUS_NO;
            }

            sprintf(szTmpBuf, "%s    %d    %d    %s        %s        %s\x0D\x0A",
                    szField1, iRecvQ, iSendQ, szField4, szField5, szField6);

            strcpy(szInfo+iIndex, szTmpBuf);
            iIndex += strlen(szTmpBuf);

            iConnCount++;
        }
        else
        {
            break;
        }
    }

    if(iConnCount == 0)
    {
        *pcStatus = STATUS_NO;
    }

    pclose(fp);

    return SUCC;
}

/****************************************************************
** 功    能：检查客户端链接状态
** 输入参数：
**        iPort                 本地监听端口
** 输出参数：
**        szInfo                链接信息
**        pcStatus              链接状态 Y：正常 N：异常
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        当至少有一个客户端建立链接，且所有客户端链接
**        发送、接收均无数据阻塞，则链接状态为Y，否则为N
** 修改日志：
****************************************************************/
int ChkClitConnection(int iPort, char *szInfo, char *pcStatus)
{
    int     i;
    FILE    *fp;                    /* 文件指针 */
    char    szCmd[256+1];           /* 端口状态查询命令 */
    char    szField1[32+1];         /* 结果第一域 协议类型 */
    int     iRecvQ;                 /* 通讯接收队列 */
    int     iSendQ;                 /* 通讯发送队列 */
    char    szField4[32+1];         /* 结果第四域 服务端IP、端口 */
    char    szField5[32+1];         /* 结果第五域 客户端IP、端口 */
    char    szField6[32+1];         /* 结果第六域 状态 */ 
    char    szTmpBuf[128+1];        /* 临时变量 */
    int     iTmp;                   /* 临时变量 */
    int     iConnCount;             /* 链接个数 */
    int     iIndex;                 /* 字符串索引 */

    memset(szCmd, 0, sizeof(szCmd));

    sprintf(szCmd,
            "netstat -an | grep '%c%d ' | awk '{if($6==\"ESTABLISHED\") print $0}'", cnSplit, iPort);

    fp = popen(szCmd, "r");
    if(fp == NULL)
    {
        WriteLog(ERROR, "执行popen失败!CMD:[%s]", szCmd);

        return FAIL;
    }

    /* 默认状态正常 */
    *pcStatus = STATUS_YES;
    iConnCount = 0;
    iIndex = 0;

    while(1)
    {
        if(fscanf(fp, "%s %d %d %s %s %s\n",
                  szField1, &iRecvQ, &iSendQ, szField4, szField5, szField6) != EOF)
        {
            /* 判断监听端口是否正确 */
            for(i=(strlen(szField4)-1);i>=0;i--)
            {
                if(szField4[i] == cnSplit)
                {
                    break;
                }
            }

            iTmp = atoi(szField4+i+1);

            if(iTmp != iPort)
            {
                continue;
            }

            /* 判断是否有数据阻塞 */
            if(iRecvQ > 0 || iSendQ > 0)
            {
                *pcStatus = STATUS_NO;
            }

            memset(szTmpBuf, 0, sizeof(szTmpBuf));
            sprintf(szTmpBuf, "%s    %d    %d    %s        %s        %s\x0D\x0A",
                    szField1, iRecvQ, iSendQ, szField4, szField5, szField6);
            strcpy(szInfo+iIndex, szTmpBuf);
            iIndex += strlen(szTmpBuf);

            iConnCount++;
        }
        else
        {
            break;
        }
    }

    if(iConnCount == 0)
    {
        *pcStatus = STATUS_NO;
    }

    pclose(fp);

    return SUCC;
}
