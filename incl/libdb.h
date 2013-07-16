/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.5 $
 * $Log: libdb.h,v $
 * Revision 1.5  2012/12/03 07:46:04  yezt
 *
 * 整理.h文件代码，修改连接状态检验函数
 *
 * Revision 1.4  2012/12/03 05:47:54  yezt
 *
 * 增加引用libpub.h
 *
 * Revision 1.3  2012/11/29 06:12:49  yezt
 *
 * 增加数据库连接状态判断函数
 *
 * Revision 1.2  2012/11/28 01:43:42  chenjr
 * 去除dbop接口入参
 *
 * Revision 1.1  2012/11/20 07:25:03  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#ifndef _LIBDB_H_
#define _LIBDB_H_

    /* 建立数据库链接 */
    extern int OpenDB();

    /* 断开数据库链接 */
    extern void CloseDB();

    /* 开始一个事务 */
    extern int BeginTran();

    /* 提交一个事务 */
    extern int CommitTran();

    /* 事务回滚 */
    extern int RollbackTran();
    
    /* 数据库连接状态判断 */
    extern int ChkDBLink();


#endif  /*_LIBDB_H_ */ 
