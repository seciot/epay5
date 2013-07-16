/******************************************************************
 ** Copyright(C)2009－2012 福建联迪商用设备有限公司
 ** 主要内容：日志打印头文件，包含文件句柄，一些宏变量等内容 
 ** 创 建 人：zhangwm
 ** 创建日期：2012/12/03
 **
 ** ---------------------------------------------------------------
 **   $Revision: 1.3 $
 **   $Log: libepay.h,v $
 **   Revision 1.3  2012/12/07 05:47:02  fengw
 **
 **   1、添加libepay库下头文件。
 **
 **   Revision 1.2  2012/11/29 09:02:54  zhangwm
 **
 **   增加extern关键字
 **
 **   Revision 1.1  2012/11/28 07:20:40  zhangwm
 **
 **   增加LIBEPAY头文件
 **
 **
 ** ---------------------------------------------------------------
 **
 *******************************************************************/
#ifndef __LIBEPAY_H_
#define __LIBEPAY_H_
#include "app.h"

/* --------------------
 * [日志记录类接口]
 * --------------------*/
    extern void WriteHdLog(char* pszLogData, int iLen, char* pszTitle);
    extern void WriteMoniLog(T_App* tpApp, char* pszTransName);

#endif /*end __LIBEPAY_H_*/


