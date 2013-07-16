
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.8 $
 * $Log: Shm.c,v $
 * Revision 1.8  2012/12/04 07:11:14  chenjr
 * 代码规范化
 *
 * Revision 1.7  2012/11/29 02:15:35  linqil
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.6  2012/11/28 08:25:44  linqil
 * 去掉冗余头文件user.h
 *
 * Revision 1.5  2012/11/28 02:58:33  linqil
 * 修改日志函数
 *
 * Revision 1.4  2012/11/28 01:33:20  chenjr
 * 添加创建接口
 *
 * Revision 1.3  2012/11/27 07:38:25  yezt
 * *** empty log message ***
 *
 * Revision 1.2  2012/11/27 06:08:39  linqil
 * 增加引用pub.h 修改return 修改条件判断方式
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
#include <sys/shm.h>

#include "pub.h"

/* ----------------------------------------------------------------
 * 功    能： 创建共享内存
 * 输入参数：
 *            szFile       已经存在的文件名(一般在etc文件夹下)
 *            iId          子序号
 *            iShmSize     新建的共享内存大小
 * 输出参数： 无
 * 返 回 值： -1   失败 /shmid   共享内存标识符
 * 作    者：
 * 日    期： 2012/11/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int CreateShm(char *szFile,  int iId, int iShmSize)
{
    int    shmid;
    key_t  key;

    if (szFile == NULL || iId == 0 || iShmSize < 0)
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

    shmid = shmget(key, iShmSize, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        WriteLog(ERROR, "call shmget(%ld, %d) fail[%d-%s]",
                         key, iShmSize, errno, strerror(errno));
        return FAIL;
    }

    return shmid;
}

/* ----------------------------------------------------------------
 * 功    能： 获取共享内存标识
 * 输入参数：
 *            szFile       已经存在的文件名(一般在etc文件夹下)
 *            iId          子序号
 *            iShmSize     新建的共享内存大小
 * 输出参数： 无
 * 返 回 值： -1   失败 /shmid   共享内存标识符
 * 作    者：
 * 日    期： 2012/11/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetShm(char *szFile,  int iId, int iShmSize)
{
    int    shmid;
    key_t  key;

    if (szFile == NULL || iId == 0 || iShmSize < 0)
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

    shmid = shmget(key, iShmSize, 0);
    if (shmid == -1)
    {
        WriteLog(ERROR, "call shmget(%ld, %d) fail[%d-%s]",
                         key, iShmSize, errno, strerror(errno));
        return FAIL;
    }

    return shmid;
}

/* ----------------------------------------------------------------
 * 功    能：把共享内存区对象映射到调用进程的地址空间(读写模式)
 * 输入参数：iShmid       共享内存标识符
 * 输出参数：无
 * 返 回 值：NULL  空指针 / addr      地址指针
 * 作    者：
 * 日    期：2012/11/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
char *AtShm(int iShmid)
{
    char   *addr;

    if (iShmid < 0)
    {
        return (NULL);
    }

    addr = (char*)shmat(iShmid, 0, SHM_RND);
    if (addr == (void*)-1)
    {
        WriteLog(ERROR, "call shmat(shmid=%d) fail[%d-%s]",
                         iShmid, errno, strerror(errno));
        return (NULL);
    }

    return (addr);
}


/* ----------------------------------------------------------------
 * 功    能：将指定的共享内存删除
 * 输入参数：iShmid       共享内存标识符
 * 输出参数：无
 * 返 回 值：FAIL  失败/SUCC   成功
 * 作    者：
 * 日    期：2012/11/27
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int RmShm(int iShmid)
{
    int     iRet;

    if (iShmid <= 0)
    {
        WriteLog(ERROR, "RmShm input-para[iShmid=%d] error", iShmid);
        return FAIL;
    }

    iRet = shmctl(iShmid, IPC_RMID, NULL);
    if (iRet < 0)
    {
        WriteLog(ERROR, "call shmctl IPC_RMID(shmid=%d) fail[%d-%s]", 
                         iShmid, errno, strerror(errno));
        return FAIL;
    }

    return SUCC;
}

