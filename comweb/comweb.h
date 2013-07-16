/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块头文件
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.3 $
** $Log: comweb.h,v $
** Revision 1.3  2012/12/25 07:00:35  fengw
**
** 1、修改web交易监控通讯端口号变量类型为字符串。
**
** Revision 1.2  2012/12/21 02:04:03  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#ifndef _COMWEB_H_
#define _COMWEB_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "transtype.h"
#include "errcode.h"
#include "EpayLog.h"

/* 宏定义 */

#define CONFIG_FILENAME                 "Setup.ini"
#define SECTION_COMMUNICATION           "SECTION_COMMUNICATION"
#define SECTION_PUBLIC                  "SECTION_PUBLIC"

/* 下载模式 */
#define DOWN_MODE_IMMEDIATE             1                   /* 立即下载 */
#define DOWN_MODE_ONLINE                2                   /* 联机触发 */

/* 下载方式 */
#define DOWN_SPECIFY_POS                1                   /* 更新指定终端 */
#define DOWN_SPECIFY_SHOP               2                   /* 更新指定商户终端 */
#define DOWN_ALL                        3                   /* 更新所有终端 */
#define DOWN_SPECIFY_TYPE               4                   /* 更新指定应用类型终端 */
#define DOWN_SPECIFY_DEPT               5                   /* 更新指定机构下终端 */

/* 呼叫类型 */
#define CALLTYPE_POS                    1                   /* 终端发起 */
#define CALLTYPE_CENTER                 2                   /* 平台发起 */

/* 通讯缓存最大长度 */
#define MAX_SOCKET_BUFLEN               1024

#ifndef _EXTERN_
    /* finatran模块全局变量 */
    int     giTdiTimeOut;                       /* TDI索引最长访问时间 */
    long    glTimeOut;                          /* 交易超时时间 */
    char    gszMoniIP[15+1];                    /* web监控IP地址 */
    int     gszMoniPort[5+1];                   /* web监控端口号 */
    int     giDownMode;                         /* 下载模式 */
    int     giDownType;                         /* 下载方式 */
    char    gszBitmap[256+1];                   /* 下载位图 */
#else
    extern int          giTdiTimeOut;
    extern long         glTimeOut;
    extern char         gszMoniIP[15+1];
    extern int          gszMoniPort[5+1];
    extern int          giDownMode;
    extern int          giDownType;
    extern char         gszBitmap[256+1];
#endif

#endif
