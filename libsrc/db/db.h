/* ---------------------------------------
 * Copyright(C)2006 - 2013 联迪商用设备有限公司
 *
 * FileName  :  db.h
 *
 * 主要内容  :  定义db库的公共宏定义，主要是为了不引用user.h
 *
 * CreateInfo:  LinQili@ 2012.11.28 16:26:38
 *
 * ---------------------------------------
 *
 * $Revision: 1.2 $
 *
 * $Log: db.h,v $
 * Revision 1.2  2012/12/03 07:46:04  yezt
 *
 * 整理.h文件代码，修改连接状态检验函数
 *
 * Revision 1.1  2012/11/28 08:33:30  linqil
 * init
 *
 * --------------------------------------------*/
 
#ifndef _DB_H_
#define _DB_H_

#define SQLCODE           sqlca.sqlcode
#define SQLERR            sqlca.sqlerrm.sqlerrmc
#define SQLROW            sqlca.sqlerrd[2]

#define SQL_NO_RECORD     1403
#define SQL_SELECT_MUCH   -2112
#define SQL_DUPLICATE     -1

#define DB_ORACLE         1


/*函数返回*/


/* 日志打印相关宏定义 */
#define E_ERROR     0
#define T_TRACE     1

#define ERROR       __FILE__, __LINE__, E_ERROR
#define TRACE       __FILE__, __LINE__, T_TRACE

#endif /*  _DB_H_ */


