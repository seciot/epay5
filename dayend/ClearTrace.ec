/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：日终流水清理
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: ClearTrace.ec,v $
 * Revision 1.4  2013/06/08 02:04:39  fengw
 *
 * 1、新增日期计算函数，统一流水清理函数的入参。
 *
 * Revision 1.3  2013/06/05 07:35:01  fengw
 *
 * 1、替换PrintLog函数为WriteLog。
 * 2、修改参数文件名、域名等。
 *
 * Revision 1.2  2013/06/05 02:15:38  fengw
 *
 * 1、增加系统状态监控表数据清理代码。
 *
 * Revision 1.1  2012/12/03 05:30:43  linxiang
 * *** empty log message ***
 *
 * ----------------------------------------------------------------
 */
#ifdef DB_ORACLE
     EXEC SQL BEGIN DECLARE SECTION;
        EXEC SQL INCLUDE SQLCA;
        char    szSettleDate[8+1]; 
        char    szMoniTime[14+1];
     EXEC SQL EnD DECLARE SECTION;
#else
         $include sqlca;
#endif

#include "dayend.h"
      
/* ----------------------------------------------------------------
 * 功      能：日终当前流水清理
 * 输入参数：
 *          szDate      流水清理日期
 * 输出参数：
 * 返 回 值：-1  失败；  0  成功
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int Posls2History(char* szDate) 
{
    memset(szSettleDate, 0, sizeof(szSettleDate));
    memcpy(szSettleDate, szDate, 8);

    WriteLog(TRACE, "将当前流水表中(%s)以前流水移到历史表", szSettleDate);

    EXEC SQL INSERT INTO history_ls VALUE
        (SELECT * FROM posls WHERE settle_date <= :szSettleDate);
    if(SQLCODE && SQLCODE != SQL_NO_RECORD)
    {
        WriteLog(ERROR, "insert into history_ls fail %ld %s", SQLCODE, SQLERR );

        return FAIL;
    }

    EXEC SQL DELETE FROM posls WHERE settle_date <= :szSettleDate;
    if(SQLCODE && SQLCODE != SQL_NO_RECORD)
    {
        WriteLog(ERROR, "delete posls fail %ld %s", SQLCODE, SQLERR );

        return FAIL;
    }

    WriteLog(TRACE, "当前流水表移到历史表成功");

    return SUCC;
}

/* ----------------------------------------------------------------
 * 功      能：日终历史流水清理
 * 输入参数：
 *          szDate          历史流水清理日期
 * 输出参数：
 * 返 回 值：-1  失败；  0  成功
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ClearHistory(char* szDate) 
{
    memset(szSettleDate, 0, sizeof(szSettleDate));
    memcpy(szSettleDate, szDate, 8);

    WriteLog(TRACE, "清除历史流水表中(%s)以前的流水", szSettleDate);

    EXEC SQL DELETE FROM history_ls WHERE settle_date <= :szSettleDate;
    if(SQLCODE && SQLCODE != SQL_NO_RECORD)
    {
        WriteLog(ERROR, "delete history_ls fail %ld %s", SQLCODE, SQLERR );

        return FAIL;
    }

    WriteLog(TRACE, "历史流水表数据清理成功" );

    return SUCC;
}

/* ----------------------------------------------------------------
 * 功      能：系统状态监控记录清除
 * 输入参数：
 *          szDate      监控记录清理日期
 * 输出参数：
 * 返 回 值：-1  失败；  0  成功
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ClearEpayMoni(char* szDate) 
{
    memset(szMoniTime, 0, sizeof(szMoniTime));
    memcpy(szMoniTime, szDate, 8);
    memcpy(szMoniTime+8, "000000", 6);

    WriteLog(TRACE, "清除系统状态监控表(%s)以前的记录", szMoniTime);

    EXEC SQL
        DELETE FROM epay_moni
        WHERE moni_time < :szMoniTime;
    if(SQLCODE && SQLCODE != SQL_NO_RECORD)
    {
        WriteLog(ERROR, "清除系统状态监控表记录失败!SQLCODE=%d SQLERR=%s",
                 SQLCODE, SQLERR);

        return FAIL;
    }

    WriteLog(TRACE, "系统状态监控表数据清理成功");

    return SUCC;
}
