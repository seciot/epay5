/* --------------------------------------------
 * Copyright(C)2006 - 2013 联迪商用设备有限公司
 *
 * FileName  :  pub.h
 *
 * 主要内容  :  定义pub库的公共宏定义
 *
 * CreateInfo:  LinQili@ 2012.11.26 16:26:38
 *
 * ---------------------------------------
 *
 * $Revision: 1.1 $
 *
 * $Log: pub.h,v $
 * Revision 1.1  2012/12/17 07:18:58  fengw
 *
 * 1、将基础库、EPAY库中头文件移至$WORKDIR/incl目录。
 *
 * Revision 1.2  2012/11/28 08:25:44  linqil
 * 去掉冗余头文件user.h
 *
 * Revision 1.1  2012/11/26 08:35:51  linqil
 * 增加头文件pub.h；增加各个文件对头文件的引用；修改return 0 return -1 为return SUCC return FAIL；
 *
 *
 * --------------------------------------------*/
 
#ifndef _PUB_H_
#define _PUB_H_

/*函数返回*/
#define SUCC             0
#define FAIL            -1
#define DUPLICATE       -2
#define TIMEOUT         -3
#define INVALID_PACK    -4


/* 日志打印相关宏定义 */
#define E_ERROR     0
#define T_TRACE     1

#define ERROR       __FILE__, __LINE__, E_ERROR
#define TRACE       __FILE__, __LINE__, T_TRACE

#endif /*  _PUB_H_ */


