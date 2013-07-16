/******************************************************************
 ** Copyright(C)2009－2012 福建联迪商用设备有限公司
 ** 主要内容：日志打印头文件，包含文件句柄，一些宏变量等内容 
 ** 创 建 人：zhangwm
 ** 创建日期：2012/12/03
 **
 ** ---------------------------------------------------------------
 **   $Revision: 1.1 $
 **   $Log: LogPrint.h,v $
 **   Revision 1.1  2012/12/17 07:18:58  fengw
 **
 **   1、将基础库、EPAY库中头文件移至$WORKDIR/incl目录。
 **
 **   Revision 1.4  2012/11/29 07:02:31  zhangwm
 **
 **   增加是否打印日志控制
 **
 **   Revision 1.3  2012/11/27 06:13:41  zhangwm
 **
 **   将写APP日志功能移除
 **
 **   Revision 1.2  2012/11/26 06:45:35  zhangwm
 **
 **   	将错误日志打印移植到公共库中
 **
 **   Revision 1.1  2012/11/20 03:27:37  chenjr
 **   init
 **
 ** ---------------------------------------------------------------
 **
 *******************************************************************/
#ifndef __LOGPRINT_H_
#define __LOGPRINT_H_


/* 日志文件名定义 */
#define E_LOG "/log/E_LOG"
#define T_LOG "/log/T_LOG"
#define H_LOG "/log/H_LOG"
#define M_LOG "/log/M_LOG"

#define E_TYPE 0
#define T_TYPE 1
#define H_TYPE 2
#define M_TYPE 3

/* 全局文件句柄 */
FILE *fpTLog;
FILE* fpHLog;
FILE* fpMLog;

/* 日志打印相关数据变量长度定义 */
#define PATH_LEN 80
#define DATA_LEN 5120

#define E_ERROR     0
#define T_TRACE     1

#define ERROR       __FILE__, __LINE__, E_ERROR
#define TRACE       __FILE__, __LINE__, T_TRACE

#endif /* end __LOGPRINT_H */
