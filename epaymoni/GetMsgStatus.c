/*****************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台系统状态监控模块 获取消息队列信息
** 创 建 人：冯炜
** 创建日期：2012/10/30
**
** $Revision: 1.7 $
** $Log: GetMsgStatus.c,v $
** Revision 1.7  2013/03/26 07:20:45  fengw
**
** 1、修改GetMsgQue函数返回值判断。
**
** Revision 1.6  2012/12/21 02:08:15  fengw
**
** 1、增加Revision、Log。
**
*******************************************************************/

#define _EXTERN_

#include "epaymoni.h"

/****************************************************************
** 功    能：检查消息队列状态
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
int GetMsgStatus(char *szChkStatus)
{
    int     i;
    char    szTmpBuf[64+1];                 /* 临时变量 */
    int     iMsgCount;                      /* 待监控消息队列个数 */
    char    szMsgFileName[64+1];            /* 创建消息队列文件名 */
    char    szID[16+1];                     /* 创建消息队列ID */
    int     iID;                            /* 创建消息队列ID */
    char    szMsgComments[32+1];            /* 消息队列中文名 */
    int     iMsgID;                         /* 消息队列ID */
    char    szStatus[64+1];                 /* 消息队列状态 */
    int     iIndex;                         /* 字符串索引 */
    struct msqid_ds msInfo;                 /* 消息队列信息结构 */

    /* 读取参数 */
    /* 获取待监控消息队列个数 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, "MSG_MON_COUNT", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_EPAYMONI, "MSG_MON_COUNT");

        return FAIL;
    }

    iMsgCount = atoi(szTmpBuf);

    /* 判断待监控消息队列个数，小于等于0，返回错误 */
    if(iMsgCount <=0)
    {
        WriteLog(ERROR, "MSG_MON_COUNT参数值[%d]不正确!", iMsgCount);

        return FAIL;
    }

    /* 获取消息队列文件名 */
    memset(szMsgFileName, 0, sizeof(szMsgFileName));
    if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, "MSG_FILE", szMsgFileName) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_EPAYMONI, "MSG_FILE");

        return FAIL;
    }

    /* 写详细信息文件，数据库sysinfo字段 */
    fprintf(fpStatusFile, "消息队列当前状态\x0D\x0A");
    fprintf(fpStatusFile, "********************************************************************************\x0D\x0A");

    /* 写状态信息，数据库msg_status字段 */
    iIndex = 0;

    sprintf(szChkStatus, "%d|", iMsgCount);
    iIndex += strlen(szChkStatus);

    /* 循环获取进程信息 */
    for(i=1;i<=iMsgCount;i++)
    {
        /* 获取消息队列ID */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szID, 0, sizeof(szID));
        sprintf(szTmpBuf, "MSG_ID_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szID) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }
        iID = atoi(szID);

        /* 获取消息队列中文名 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szMsgComments, 0, sizeof(szMsgComments));
        sprintf(szTmpBuf, "MSG_COMMENTS_%d", i);
        if(ReadConfig(CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf, szMsgComments) != SUCC)
        {
            WriteLog(ERROR, "读取参数文件[%s] SECTION:[%s] 参数项:[%s] 失败!",
                     CONFIG_FILENAME, SECTION_EPAYMONI, szTmpBuf);

            return FAIL;
        }

        /* 获取消息队列ID */
        if((iMsgID = GetMsgQue(szMsgFileName, iID)) == FAIL)
        {
            WriteLog(ERROR, "获取消息队列MsgID失败！MsgFile:[%s] ID:[%d]", szMsgFileName, iID);

            /* 状态信息 2.消息队列状态 */
            memset(szStatus, 0, sizeof(szStatus));
            sprintf(szStatus, "N,%s,-1#", szMsgComments);

            memcpy(szChkStatus+iIndex, szStatus, strlen(szStatus));
            iIndex += strlen(szStatus);

            /* 详细信息文件 */
            fprintf(fpStatusFile, "%s 消息队列异常\x0D\x0A", szMsgComments);

            continue;
        }

        /* 获取消息队列状态 */
        if(GetMsgQueStat(iMsgID, &msInfo) != SUCC)
        {
            WriteLog(ERROR, "获取消息队列信息失败!MsgID:[%d]", iMsgID);

            /* 状态信息 2.消息队列状态 */
            memset(szStatus, 0, sizeof(szStatus));
            sprintf(szStatus, "N,%s,-1#", szMsgComments);

            memcpy(szChkStatus+iIndex, szStatus, strlen(szStatus));
            iIndex += strlen(szStatus);

            /* 详细信息文件 */
            fprintf(fpStatusFile, "%s 消息队列异常\x0D\x0A", szMsgComments);

            continue;
        }

        /* 状态信息 2.消息队列状态 */
        memset(szStatus, 0, sizeof(szStatus));
        sprintf(szStatus, "%c,%s,%d#", msInfo.msg_qnum==0?STATUS_YES:STATUS_NO,
                szMsgComments, msInfo.msg_qnum);
            
        memcpy(szChkStatus+iIndex, szStatus, strlen(szStatus));
        iIndex += strlen(szStatus);

        /* 详细信息文件 */
        fprintf(fpStatusFile, "%s 消息个数:%d\x0D\x0A", szMsgComments, msInfo.msg_qnum);
    }

    /* 状态信息 结束标志 */
    szChkStatus[iIndex] = '|';
    iIndex += 1;

    /* 详细信息文件  结束标志 */
    fprintf(fpStatusFile, "********************************************************************************\x0D\x0A");

    return SUCC;
}
