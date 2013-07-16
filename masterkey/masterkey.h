/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台终端主密钥管理模块头文件
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.2 $
** $Log: masterkey.h,v $
** Revision 1.2  2012/12/07 06:00:18  fengw
**
** 1、删除public.h的引用，增加stdio.h、string.h的引用。
**
** Revision 1.1  2012/12/07 01:19:47  fengw
**
** 1、终端主密钥生成程序初始版本。
**
*******************************************************************/

#ifndef _MASTERKEYH_
#define _MASTERKEYH_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"
#include "dbtool.h"
#include "transtype.h"
#include "errcode.h"

#define MAX_MASTERKEY_GENERATE              10000       /* 终端主密钥单次生成最大个数 */

#ifndef _EXTERN_

#else

#endif

#endif
