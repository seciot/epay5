/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：定义本系统信号量的宏以及函数定义等
** 创 建 人：冯炜
** 创建日期：2012/11/29
** ----------------------------------------------------------------
** $Revision:
** $Log:
*******************************************************************/

#ifndef	_EPAY_SEM_
#define _EPAY_SEM_

#include <stdlib.h>

#include "user.h"
#include "dbtool.h"
#include "transtype.h"
#include "libpub.h"

#define SEM_FILE            "/etc/SEMFILE"

/* 信号量标识 */
#define SEM_ACCESS_ID       1               /* 接入客户端信息共享内存读写信号量 */
#define SEM_TDI_ID          2               /* 交易数据索引共享内存读写信号量 */
#define SEM_HOST_ID         3               /* 后台通讯状态共享内存读写信号量 */

int     giSemAccessID;
int     giSemTdiID;
int     giSemHostID;

#else
    extern int      giSemAccessID;
    extern int      giSemTdiID;
    extern int      giSemHostID;
#endif

