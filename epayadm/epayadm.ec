/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台IPC控制模块
** 创 建 人：冯炜
** 创建日期：2012-11-19
**
** $Revision: 1.3 $
** $Log: epayadm.ec,v $
** Revision 1.3  2013/06/28 08:36:34  fengw
**
** 1、检测IPC时增加提示日志，避免报错信息造成错误提示。
**
** Revision 1.2  2012/12/10 02:55:21  fengw
**
** 1、增加TestIPC函数，用于测试平台IPC是否已经创建。
** 2、修改启动参数判断。
**
** Revision 1.1  2012/12/07 01:23:02  fengw
**
** 1、易收付平台启动、关闭程序初始版本。
**
*******************************************************************/

#include "epayadm.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

int main(int argc, char *argv[])
{
    int     iFlag;          /* 启动参数 */

    /* 获取启动参数 */
    /* 无参数返回共享内存状态 */
    /* 参数说明 */
    /* 无参数返回共享内存状态 */
    /* 0 创建IPC */
    /* 1 关闭IPC */
    /* 2 重载卡表数据 */
    if(argc == 1)
    {
        return TestIPC();
    }
    else if(argc == 2)
	{
        iFlag = atoi(argv[1]);

        switch(iFlag)
        {
            /* 创建IPC */
            case 0:
                return CreateIPC();
            /* 删除IPC */
            case 1:
                return DeleteIPC();
            /* 重载卡表数据 */
            case 2:
                return ReloadCards();
            default:
                printf("参数错误\n");
    
                printf("示例:%s [0(启动)|1(关闭)|2(重载卡表)]\n", argv[0]);
    
                exit(-1);
        }
    }
    else
    {
        printf("参数错误\n");

        printf("示例:%s [0(启动)|1(关闭)|2(重载卡表)]\n", argv[0]);

        exit(-1);
    }
}

/****************************************************************
** 功    能：判断IPC是否已经创建
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      IPC已经创建
**        FAIL                      IPC未创建
** 作    者：
**        fengwei
** 日    期：
**        2012/12/10
** 调用说明：
**
** 修改日志：
****************************************************************/
int TestIPC()
{
    WriteLog(ERROR, "********************IPC测试，可忽略报错信息********************");

    if(GetEpayMsgId() == SUCC  || GetEpayShm() == SUCC ||
       GetEpaySem() == SUCC)
    {
        WriteLog(ERROR, "********************IPC测试结束，IPC已创建********************");

        return SUCC;
    }
    else
    {
        WriteLog(ERROR, "********************IPC测试结束，IPC未创建********************");

        return FAIL;
    }
}

/****************************************************************
** 功    能：创建共享内存、消息队列等IPC
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      IPC创建成功
**        FAIL                      IPC创建失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int CreateIPC()
{
    /* 创建消息队列 */
    if(CreateEpayMsg() != SUCC)
    {
        printf("创建EPAY消息队列失败!\n");

        /* 删除消息队列 */
        RmEpayMsg();

        return FAIL;
    }

    printf("创建EPAY消息队列成功!\n");

    /* 创建信号量 */
    if(CreateEpaySem() != SUCC)
    {
        printf("创建EPAY信号量失败!\n");

        /* 删除消息队列 */
        RmEpayMsg();

        /* 删除信号量 */
        RmEpaySem();

        return FAIL;
    }

    printf("创建EPAY信号量成功!\n");

    /* 打开数据库 */
    if(OpenDB() != SUCC)
    {
        printf("打开数据库失败!\n");

        return FAIL;
    }

    /* 创建共享内存 */
    if(CreateEpayShm() != SUCC)
    {
        printf("创建EPAY共享内存失败!\n");

        /* 删除共享内存 */
        RmEpayShm();

        /* 删除消息队列 */
        RmEpayMsg();

        /* 删除信号量 */
        RmEpaySem();

        CloseDB();

        return FAIL;
    }

    printf("创建EPAY共享内存成功!\n");

    /* 关闭数据库 */
    CloseDB();

    return SUCC;
}

/****************************************************************
** 功    能：删除共享内存、消息队列等IPC
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      IPC删除成功
**        FAIL                      IPC删除失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int DeleteIPC()
{
    /* 删除共享内存 */
    if(RmEpayShm() != SUCC)
    {
        printf("删除共享内存失败!\n");
    }

    /* 删除消息队列 */
    if(RmEpayMsg() != SUCC)
    {
        printf("删除消息队列失败!\n");
    }

    /* 删除信号量 */
    if(RmEpaySem() != SUCC)
    {
        printf("删除信号量失败!\n");
    }

    return SUCC;
}

/****************************************************************
** 功    能：重载卡表数据到共享内存
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      卡表数据重载成功
**        FAIL                      卡表数据重载失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int ReloadCards()
{
    /* 打开数据库 */
    if(OpenDB() != SUCC)
    {
        printf("打开数据库失败!\n");

        return FAIL;
    }

    /* 载入卡表数据 */
    if(LoadCardToShm() != SUCC)
    {
        printf("载入卡表数据失败!\n");

        CloseDB();

        return FAIL;
    }

    /* 关闭数据库 */
    CloseDB();

    return SUCC;
}
