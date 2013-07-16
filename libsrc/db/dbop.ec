/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：数据库操作接口
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.14 $
 * $Log: dbop.ec,v $
 * Revision 1.14  2012/12/06 02:31:54  yezt
 * 增加修改注释
 *
 * Revision 1.13  2012/12/06 01:58:17  yezt
 *
 * 使用用户建立的epay5的用户表判断，不同数据库都可以使用
 *
 * Revision 1.12  2012/12/03 08:04:58  yezt
 * 更改ChkDBLink函数
 *
 * Revision 1.11  2012/12/03 07:46:04  yezt
 *
 * 整理.h文件代码，修改连接状态检验函数
 *
 * Revision 1.10  2012/12/03 05:34:39  yezt
 *
 * 修改数据库连接状态验证函数
 *
 * Revision 1.9  2012/11/30 05:45:44  yezt
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.8  2012/11/29 06:12:49  yezt
 *
 * 增加数据库连接状态判断函数
 *
 * Revision 1.7  2012/11/29 06:06:41  linqil
 * *** empty log message ***
 *
 * Revision 1.6  2012/11/28 08:33:53  linqil
 * 去掉对user.h的引用
 *
 * Revision 1.5  2012/11/28 05:48:38  linqil
 * 修改日志函数
 *
 * Revision 1.4  2012/11/26 05:11:54  yezt
 * *** empty log message ***
 *
 * Revision 1.3  2012/11/21 06:01:39  chenjr
 * 修改系统函数拼写错(fprintf)
 *
 * Revision 1.2  2012/11/20 07:26:48  chenjr
 * format conv
 *
 * Revision 1.1  2012/11/20 07:25:03  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#define _DB_EXTERN_

#include <stdio.h>
#include <stdlib.h>
#include "libpub.h"
#include "db.h"

#ifdef DB_ORACLE
    EXEC SQL INCLUDE SQLCA;
#endif

#ifdef DB_INFORMIX
    $include sqlca;
#endif

/* 以下几项可根据项目具体情况修改 */
#define CONFPATH   "Setup.ini"       /*配置文件名 */
#define SECTION    "SECTION_DB"      /*数据库配置章节名(对应于配置文件)*/
#define DBUSER     "DB_USER"         /*用户名称标签名(对应于配置文件)*/
#define DBPWD      "DB_PWD"          /*用户密码标签名(对应于配置文件) */
#define DBSERVICE  "DB_SERVICES"     /*数据库名标签名(对应于配置文件)*/

#define SUCC       0
#define FAIL       -1

EXEC SQL BEGIN DECLARE SECTION;
    char  gszPwd[200];     /* 用户名 */
    char  gszUsr[200];     /* 密码 */
    char  gszSrv[300];     /* 数据库名 */
EXEC SQL END DECLARE SECTION;

#define GET_DBCONF(tag, val) \
        do    \
        { \
            if (-1 == ReadConfig(CONFPATH, SECTION, tag, val)) \
            { \
                WriteLog(ERROR, "打开数据库配置文件出错"); \
                return FAIL; \
            } \
        }while (0) 


/* ----------------------------------------------------------------
 * 功    能：建立数据库链接
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：SUCC 打开成功；  FAIL 打开失败
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：系统启动时候，连接数据库
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int OpenDB()
{
#ifdef DB_INFORMIX
    sqldetach();

    $database pos;
    if (SQLCODE)
    {
        WriteLog(ERROR, "database fail. SQLCODE[%ld-%.*s]", SQLCODE, SQLERRM.SQLERRML, SQLERRM.SQLERRMC);
        return(FAIL);
    }

    return SUCC;
#endif

#ifdef DB_ORACLE
    memset(gszPwd, 0, sizeof(gszPwd));
    memset(gszUsr, 0, sizeof(gszUsr));
    memset(gszSrv, 0, sizeof(gszSrv));


    /* 从配置文件提取数据库用户名、密码等 */
    GET_DBCONF(DBUSER,    gszUsr);
    GET_DBCONF(DBPWD,     gszPwd);
    GET_DBCONF(DBSERVICE, gszSrv);

    DelAllSpace(gszUsr);
    DelAllSpace(gszPwd);
    DelAllSpace(gszSrv);

    if (strlen(gszSrv) > 0)
    {
        EXEC SQL
        CONNECT :gszUsr IDENTIFIED BY :gszPwd USING  :gszSrv;
    }
    else
    {
        EXEC SQL
        CONNECT :gszUsr IDENTIFIED BY :gszPwd;
    }

    if (SQLCODE)
    {
        WriteLog(ERROR, "Connect DB As USR[%s]PWD[%s]SRV[%s] Fail. SQLCODE[%ld]",
             gszUsr, gszPwd, gszSrv, SQLCODE);
        return(FAIL);
    }

    return(SUCC); 
#endif

}

/* ----------------------------------------------------------------
 * 功    能：断开数据库连接
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：SUCC  成功；   FAIL  失败
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：系统关闭时候，断开数据库
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void CloseDB()
{
#ifdef DB_INFORMIX
    $close database;
#endif
    
#ifdef DB_ORACEL
    EXEC SQL COMMIT WORK RELEASE;
    if (SQLCODE)
    {
        WriteLog(ERROR, "Close DB As USR[%s] Fail[SQLCODE=%ld]", 
                gszUsr, SQLCODE);
    }
#endif
}

/* ----------------------------------------------------------------
 * 功    能：开始一个事务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：SUCC  成功；   FAIL  失败
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int BeginTran()
{
#ifdef DB_ORACLE
    return SUCC;
#endif

#ifdef DB_INFORMIX
    $begin work;
    if (SQLCODE)
    {
        WriteLog(ERROR, "Failure to begin work[SQLCODE=%d]", SQLCODE);
        return (FAIL);    
    }
    
    return (SUCC);
#endif
}

/* ----------------------------------------------------------------
 * 功    能：提交一个事务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：SUCC  成功；   FAIL  失败
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int CommitTran()
{
    EXEC SQL COMMIT WORK;
    if (SQLCODE)
    {
        WriteLog(ERROR, "Failure to commit work[SQLCODE=%d]", SQLCODE);
        return( FAIL );
    }
    return (SUCC);
}

/* ----------------------------------------------------------------
 * 功    能：回滚数据库事务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：SUCC  成功；   FAIL  失败
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：数据库表更新\插入\删除等失败时候回滚
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int RollbackTran()
{
    EXEC SQL ROLLBACK WORK;
    if( SQLCODE)
    {
        WriteLog(ERROR, "Failure to rollback work[SQLCODE=%d]", SQLCODE);
        return (FAIL);    
    }
    return( SUCC );
}

/* ----------------------------------------------------------------
 * 功    能：判断数据库连接是否正常
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：SUCC  正常连接 /  FAIL  断开连接
 * 作    者：叶铸挺
 * 日    期：2012/11/29
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 *           2012/12/06  yezt
 * 修改内容简述:使用用户建立的epay5的用户表判断，不同数据库都可以使用
 * ----------------------------------------------------------------
 */
int ChkDBLink()
{
    EXEC SQL
         SELECT count(*) FROM module;
    if(SQLCODE)
    {
        WriteLog(ERROR, "Disconnect DB[SQLCODE=%d]", SQLCODE);
        return (FAIL);
    }
    return( SUCC );
}
