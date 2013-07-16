/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：日终备份
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: DayendBackup.ec,v $
 * Revision 1.2  2013/06/05 07:35:01  fengw
 *
 * 1、替换PrintLog函数为WriteLog。
 * 2、修改参数文件名、域名等。
 *
 * Revision 1.1  2012/12/03 05:30:43  linxiang
 * *** empty log message ***
 *
 * ----------------------------------------------------------------
 */

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#else
    $include sqlca;
#endif

#include "dayend.h"
 
/* ----------------------------------------------------------------
 * 功    能：日终备份
 * 输入参数：
 *           pszDate    备份日期
 * 输出参数：
 * 返 回 值：-1  失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int DayendBackup( char *pszDate )
{
    EXEC SQL BEGIN DECLARE SECTION;
        char     szDate[8+1];
    EXEC SQL END DECLARE SECTION;

    char szCmd[255];

    strncpy(szDate, pszDate, 8);

    /* 删除之前进程匹配记录，这些记录视为后台未返回的交易的进程记录 */
    EXEC SQL DELETE FROM pid_match WHERE local_date <= :szDate;
    CommitTran();

    WriteLog(TRACE, "数据库备份开始..." );
    memset( szCmd, 0, sizeof( szCmd ) );
    sprintf( szCmd, "backup_db %s 1>/dev/null 2>/dev/null", szDate );
    system( szCmd );
    WriteLog(TRACE, "数据库备份完成..." );

    WriteLog(TRACE, "交易日志备份开始..." );
    memset( szCmd, 0, sizeof( szCmd ) );
    sprintf( szCmd, "backlog %s 1>/dev/null 2>/dev/null", szDate );
    system( szCmd );
    WriteLog(TRACE, "交易日志备份完成..." );

    return SUCC;
}
