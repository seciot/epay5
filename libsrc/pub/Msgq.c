
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.10 $
 * $Log: Msgq.c,v $
 * Revision 1.10  2012/12/04 06:50:14  chenjr
 * 代码规范化
 *
 * Revision 1.9  2012/11/29 02:15:35  linqil
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.8  2012/11/28 08:25:44  linqil
 * 去掉冗余头文件user.h
 *
 * Revision 1.7  2012/11/28 02:40:25  linqil
 * 修改日志函数
 *
 * Revision 1.6  2012/11/27 07:08:04  linqil
 * 修改条件判断语句
 *
 * Revision 1.5  2012/11/27 03:26:05  linqil
 * 增加引用pub.h 修改return
 *
 * Revision 1.4  2012/11/27 03:07:45  yezt
 * *** empty log message ***
 *
 * Revision 1.3  2012/11/26 02:43:03  chenjr
 * 把消息队列创建与获取分解成两个接口，添加获取消息队列信息接口
 *
 * Revision 1.2  2012/11/22 02:04:51  chenjr
 * *** empty log message ***
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <setjmp.h>

#include "pub.h"

#define MSGSIZE    8092 * 5

struct MsgBuf
{
    long mtype;
    char mbuf[MSGSIZE];
};


/* 从消息队列读消息超时跳转 */
static jmp_buf  RcvMsgTimeOut;
static void RcvMsgTimeOutProc(int n)
{
    siglongjmp(RcvMsgTimeOut, 1);
}

/* ----------------------------------------------------------------
 * 功    能：创建消息队列
 * 输入参数：szFile     已经存在的文件名(一般在etc文件夹下)
 *           iId        子序号
 * 输出参数：无
 * 返 回 值：-1 失败/消息队列的标识符
 * 作    者：
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int CreateMsgQue(char *szFile,  int iId)
{
    int    msgid;
    key_t  key;

    if (szFile == NULL || iId == 0)
    {
        return FAIL;
    }
   
    key = ftok(szFile, iId);
    if (key == -1)
    {
        WriteLog(ERROR, "call ftok(\"%s\",%d) fail[%d-%s]", 
                         szFile, iId, errno, strerror(errno));
        return FAIL;
    }

    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        WriteLog(ERROR, "call msgget IPC_CREAT(%ld) fail[%d-%s]", 
                         key, errno, strerror(errno));
        return FAIL;
    }

    return (msgid);
}


/* ----------------------------------------------------------------
 * 功    能：获取一个已存在消息队列的标识
 * 输入参数：
 *           szFile     已经存在的文件名(一般在etc文件夹下)
 *           iId        子序号
 * 输出参数：无
 * 返 回 值：-1 失败/消息队列的标识符
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetMsgQue(char *szFile,  int iId)
{
    int    msgid;
    key_t  key;

    if (szFile == NULL || iId == 0)
    {
        return FAIL;
    }

    key = ftok(szFile, iId);
    if (key == -1)
    {
        WriteLog(ERROR, "call ftok(\"%s\",%d) fail[%d-%s]", 
                         szFile, iId, errno, strerror(errno));
        return FAIL;
    }

    msgid = msgget(key, 0);
    if (msgid == -1)
    {
        WriteLog(ERROR, "call msgget 0(%ld) fail[%d-%s]", 
                         key, errno, strerror(errno));
        return FAIL;
    }

    return (msgid);
}


/* ----------------------------------------------------------------
 * 功    能：从系统内核中移除消息队列
 * 输入参数：iMsgid     消息队列的标识符
 * 输出参数：无
 * 返 回 值：0  成功/-1 失败
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int RmMsgQue(int iMsgid)
{

    if (iMsgid <= 0)
    {
        WriteLog(ERROR, "RmMsgQue input-para[iMsgid=%d] error", iMsgid); 
        return FAIL;
    }

    if (msgctl(iMsgid, IPC_RMID, NULL) < 0)
    {
        WriteLog(ERROR, "call msgctl IPC_RMID(msgid=%d) fail[%d-%s]", 
                         iMsgid, errno, strerror(errno));
        return FAIL;
    }

    return SUCC;
}

/* ----------------------------------------------------------------
 * 功    能： 取出消息队列的msqid_ds数据并返回
 *            的机构中
 * 输入参数： iMsgid     消息队列的标识符
 * 输出参数： ptDs       消息队列的msqid_ds结构
 * 返 回 值： 0  成功/-1 失败
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetMsgQueStat(int iMsgid, struct msqid_ds *ptDs)
{
    if (iMsgid <= 0)
    {
        WriteLog(ERROR, "GetMsgQueStat input-para[iMsgid=%d] error", iMsgid);
        return FAIL;
    }

    if (msgctl(iMsgid, IPC_STAT, ptDs) < 0)
    {
        WriteLog(ERROR, "call msgctl IPC_STAT(msgid=%d) fail[%d-%s]",
                         iMsgid, errno, strerror(errno));
        return FAIL;
    }

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：向消息队列中发送消息，消息大小为iSndLen
 * 输入参数：
 *          iMsgid     消息队列的标识符
 *          lMsgType   消息的类型
 *          szSndBuf   发送数据
 *          iSndLen    发送的数据大小
 * 输出参数：无
 * 返 回 值：0  成功 /-1 失败
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int SndMsgToMQ(int iMsgid, long lMsgType, char *szSndBuf, int iSndLen)
{
    struct MsgBuf mb;

    if (iMsgid <= 0 || lMsgType <= 0 || szSndBuf == NULL || 
        iSndLen < 0 || iSndLen > MSGSIZE - 1)
    {
        return FAIL;
    }
    
    memset(&mb, 0, sizeof(mb));
    mb.mtype = lMsgType;
    memcpy(mb.mbuf, szSndBuf, iSndLen);

    if (msgsnd(iMsgid, &mb, iSndLen, ~IPC_NOWAIT) == -1)
    {
        WriteLog(ERROR, "call msgsnd(msgid=%d) fail[%d-%s]", 
                        iMsgid, errno, strerror(errno));
        return FAIL;
    }

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：从消息队列中读取消息，消息最大为MSGSIZE
 * 输入参数：
 *           iMsgid    消息队列的标识符
 *           lMsgType  消息的类型
 *           iTimeOut  超时时间(0：无限等待接收消息,>0：超时时间)
 * 输出参数：szRcvBuf  收到的数据
 * 返 回 值： 0   成功/-1  失败
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int RcvMsgFromMQ(int iMsgid, long lMsgType, int iTimeOut, char *szRcvBuf)
{
    int rcvlen;
    struct MsgBuf mb;

    if (iMsgid <= 0 || lMsgType <= 0 || iTimeOut < 0 || szRcvBuf == NULL)
    {
        return FAIL;
    }

    if (iTimeOut > 0)
    {
        signal(SIGALRM, RcvMsgTimeOutProc);
        if (sigsetjmp(RcvMsgTimeOut, 1) != 0)
        {
            WriteLog(ERROR, "RcvMsgFromMQ(msgid=%d) Timeout", iMsgid);
            return SUCC;
        }
        alarm(iTimeOut);
    }

    memset(&mb, 0, sizeof(mb));
    rcvlen = msgrcv(iMsgid, &mb, MSGSIZE, lMsgType, 0);
    if (rcvlen == -1)
    {
        WriteLog(ERROR, "call msgrcv(msgid=%d) fail[%d-%s]", 
                        iMsgid, errno, strerror(errno));
        return FAIL;
    }
    alarm(0);

    memcpy(szRcvBuf, mb.mbuf, rcvlen);

    return (rcvlen);
}


