/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台系统状态监控模块
** 创 建 人：冯炜
** 创建日期：2012-10-30
**
** $Revision: 1.5 $
** $Log: epaymoni.h,v $
** Revision 1.5  2012/12/21 02:08:15  fengw
**
** 1、增加Revision、Log。
**
*******************************************************************/

#ifndef _EPAYMONI_H_
#define _EPAYMONI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oci.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "app.h"
#include "dbtool.h"
#include "transtype.h"
#include "user.h"
#include "errcode.h"

/* 参数文件宏定义 */
#define CONFIG_FILENAME                 "Setup.ini"         /* 系统参数文件名称 */
#define SECTION_PUBLIC                  "SECTION_PUBLIC"    /* 公共SECTION名称 */
#define SECTION_EPAYMONI                "SECTION_EPAYMONI"  /* 监控SECTION名称 */

/* 监控最小间隔宏定义 */
#define MIN_MONI_INTERVAL	            30                  /* 最小系统监控间隔时间，单位秒 */

/* 通讯类型宏定义 */
#define DUPLEX_KEEPALIVE_SERV           1                   /* 双工长链服务端 */
#define DUPLEX_KEEPALIVE_CLIT           2                   /* 双工长链客户端 */
#define SIMPLEX_KEEPALIVE               3                   /* 单工长链 */
#define DUPLEX_SERVER                   4                   /* 短链服务端 */

/* 状态宏定义 */
#define STATUS_YES                      'Y'                 /* 正常 */
#define STATUS_NO                       'N'                 /* 异常 */

#ifndef _EXTERN_

/* 定义netstat命令IP地址与端口分隔符 */
#ifdef AIX
    const char cnServListenIP[] = "*";
    const char cnSplit = '.';
    const char cnPSStat[] = "status";
    const char cnProcStatus = 'A';
#else
    const char cnServListenIP[] = "0.0.0.0";
    const char cnSplit = ':';
    const char cnPSStat[] = "stat";
    const char cnProcStatus = 'S';
#endif

FILE *fpStatusFile;

#else
    extern char cnServListenIP[];
    extern char cnSplit;
    extern char cnPSStat[];
    extern char cnProcStatus;
    extern FILE *fpStatusFile;
#endif

#endif
