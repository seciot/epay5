/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台自动冲正模块头文件
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.4 $
** $Log: autovoid.h,v $
** Revision 1.4  2012/12/21 01:55:45  fengw
**
** 1、将文件格式从DOS转为UNIX。
** 2、修改Revision、Log格式。
**
*******************************************************************/

#ifndef _AUTOVOID_H_
#define _AUTOVOID_H_

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "transtype.h"
#include "libpub.h"

#define CONFIG_FILENAME             "Setup.ini"
#define SECTION_AUTOVOID            "SECTION_AUTOVOID"
#define SECTION_PUBLIC              "SECTION_PUBLIC"

#ifndef _EXTERN_
    int     glPid;
    int     giVoidTimeOut;
    int     giTdiTimeOut;
    int     giSleepTime;
#else
    extern int  glPid;
    extern int  giVoidTimeOut;
    extern int  giTdiTimeOut;
    extern int  giSleepTime;
#endif

#endif
