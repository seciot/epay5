
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.10 $
 * $Log: Sem.c,v $
 * Revision 1.10  2012/12/04 07:09:53  chenjr
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
 * Revision 1.6  2012/11/28 01:36:45  chenjr
 * *** empty log message ***
 *
 * Revision 1.5  2012/11/28 01:34:54  chenjr
 * 修改GetSem接口入参判断
 *
 * Revision 1.4  2012/11/28 01:33:20  chenjr
 * 添加创建接口
 *
 * Revision 1.3  2012/11/27 06:23:57  yezt
 * *** empty log message ***
 *
 * Revision 1.2  2012/11/27 06:04:53  linqil
 * 增加引用pub.h 修改return 修改条件判断
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
#include <sys/sem.h>

#include "pub.h"

/* ----------------------------------------------------------------
 * 功    能： 创建信号量
 * 输入参数： 
 *            szFile       已经存在的文件名(一般在etc文件夹下)
 *            iId          iId        子序号
 *            iResource    信号量个数
 * 输出参数： 无
 * 返 回 值： -1  失败/ semid 信号量标识符
 * 作    者：
 * 日    期： 2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int CreateSem(char *szFile,  int iId, int iResource)
{
    int     semid;
    key_t   key;
    int     iRet;

    if (szFile == NULL || iId == 0 || iResource<= 0)
    {
        WriteLog(ERROR, "GetSem Invalid argument[%s, %d, %d]", 
                        szFile, iId, iResource);
        return FAIL;
    }

    key = ftok(szFile, iId);
    if (key == -1)
    {
        WriteLog(ERROR, "call ftok(\"%s\",%d) fail[%d-%s]",
                         szFile, iId, errno, strerror(errno));
        return FAIL;
    }

    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1)
    {
        WriteLog(ERROR, "call semget(key=%ld) fail[%d-%s]",
                         key, errno, strerror(errno));
        return FAIL;
    }

    iRet = semctl(semid, 0, SETVAL, iResource);
    if (iRet == -1)
    {
        WriteLog(ERROR, "call semctl(semid=%d, rs=%d) fail[%d-%s]",
                         semid, iResource, errno, strerror(errno));
        return FAIL;
    }

    return semid;
}

/* ----------------------------------------------------------------
 * 功    能： 获取一个已存在的信号量
 * 输入参数： szFile       已经存在的文件名(一般在etc文件夹下)
 *            iId          iId        子序号
 * 输出参数： 无
 * 返 回 值： -1  失败/ semid 信号量标识符
 * 作    者：
 * 日    期： 2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetSem(char *szFile,  int iId)
{
    int     semid;
    key_t   key;
    int     iRet;

    if (szFile == NULL || iId == 0)
    {
        WriteLog(ERROR, "GetSem Invalid argument[%s, %d]", szFile, iId);
        return FAIL;
    }

    key = ftok(szFile, iId);
    if (key == -1)
    {
        WriteLog(ERROR, "call ftok(\"%s\",%d) fail[%d-%s]",
                         szFile, iId, errno, strerror(errno));
        return FAIL;
    }

    semid = semget(key, 1, 0);
    if (semid == -1)
    {
        WriteLog(ERROR, "call semget(key=%ld) fail[%d-%s]",
                         key, errno, strerror(errno));
        return FAIL;
    }

    return semid;
}

/* ----------------------------------------------------------------
 * 功    能：对信号量标示符指定的信号量进行操作
 * 输入参数：
 *           semid          信号量标识符
 *           iResource      信号资源  >0 释放控制资源
 *                                    <0 占用控制资源
 * 输出参数：无
 * 返 回 值：-1  失败 /0   成功
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int SemOpera(int iSemid, int iResource)
{
    struct sembuf sop;
    int     iRet;

    if (iSemid <= 0 || iResource == 0)
    {
        WriteLog(ERROR, "SemOpera Invalid argument[%d, %d]"
                      , iSemid, iResource);
        return FAIL;
    }
 
    sop.sem_num = 0;
    sop.sem_op  = iResource;
    sop.sem_flg = SEM_UNDO;

    iRet = semop(iSemid, &sop, 1);
    if (iRet == -1)
    {
        WriteLog(ERROR, "call semop(semid=%d) fail[%d-%s]",
                         iSemid, errno, strerror(errno));
        return FAIL;
    }

    return SUCC;
}

/* ----------------------------------------------------------------
 * 功    能：P操作
 * 输入参数：
 *           semid          信号量标识符
 *           iResource      增加资源的个数  
 * 输出参数：无
 * 返 回 值：-1    失败/0       成功
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int P(int iSemid, int iResource)
{
    int     iRet;
    iRet = SemOpera(iSemid, 0 - iResource);
    return iRet;
}


/* ----------------------------------------------------------------
 * 功    能：V操作
 * 输入参数： 
 *           semid          信号量标识符
 *           iResource      增加资源的个数  
 * 输出参数：无
 * 返 回 值：-1    失败/0       成功
 * 作    者：
 * 日    期：2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int V(int iSemid, int iResource)
{
    int     iRet;
    iRet = SemOpera(iSemid, iResource);
    return iRet;
}


/* ----------------------------------------------------------------
 * 功    能： 将指定的信号量集从内存中删除
 * 输入参数： semid          信号量标识符
 * 输出参数： 无
 * 返 回 值： -1    成功/0      失败
 * 作    者：
 * 日    期： 2012/12/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int RmSem(int iSemid)
{
    int     iRet;

    if (iSemid <= 0)
    {
        WriteLog(ERROR, "RmSem Invalid argument[%d]", iSemid);
        return FAIL;
    }

    iRet = semctl(iSemid, 0, IPC_RMID, (struct semid_ds*)0);
    if (iRet < 0)
    {
        WriteLog(ERROR, "call semctl IPC_RMID(semid=%d) fail[%d-%s]", 
                         iSemid, errno, strerror(errno));
        return FAIL;
    }

    return SUCC;
}

