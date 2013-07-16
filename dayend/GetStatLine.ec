/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：日终生成统计表
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: GetStatLine.ec,v $
 * Revision 1.4  2013/06/08 02:04:39  fengw
 *
 * 1、新增日期计算函数，统一流水清理函数的入参。
 *
 * Revision 1.3  2013/06/05 07:35:01  fengw
 *
 * 1、替换PrintLog函数为WriteLog。
 * 2、修改参数文件名、域名等。
 *
 * Revision 1.2  2013/04/07 05:52:15  linqil
 * 修改DECODE函数为CASE...WHEN...END语句；修改trim; 使其能够兼容DB2数据库。
 *
 * Revision 1.1  2012/12/03 05:30:43  linxiang
 * *** empty log message ***
 *
 * ----------------------------------------------------------------
 */

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    EXEC SQL INCLUDE "../incl/transtype.h";
EXEC SQL EnD DECLARE SECTION;
#else
    $include sqlca;
#endif

#include "dayend.h"
 
/* ----------------------------------------------------------------
 * 功    能：日终生成统计表
 * 输入参数：
 *           pszBeginDate    开始日期
 *           pszEndDate    结束日期
 * 输出参数：
 * 返 回 值：-1  失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */

int GetStatLine( char *pszBeginDate , char *pszEndDate)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char szBeginDate[8+1];
        char szEndDate[8+1];
        int iTransTypes[100]={PURCHASE, TRAN_IN, TRAN_OUT, PAY_CREDIT, 0,
                              0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0,
                              0,};
    EXEC SQL END DECLARE SECTION;
/*
        int iTransTypes[100]={PURCHASE, TRAN_IN, TRAN_OUT, PAY_CREDIT, TRAN_OUT_OTHER,
            CHINATELECOM_PREPAY, CHINATELECOM_PAY, CHINAUNICOM_PREPAY, CHINAUNICOM_PAY,
            CHINAMOBILE_PREPAY, CHINAMOBILE_PAY, ELECTRICITY_PAY, GAS_PAY, TRAFFIC_AMERCE_PAY,
            TRAFFIC_AMERCE_NO_PAY,};
*/
    WriteLog(TRACE, "开始统计从(%s)至(%s)交易数据", pszBeginDate, pszEndDate);

    memset(szBeginDate, 0, sizeof(szBeginDate));
    memset(szEndDate, 0, sizeof(szEndDate));
    memcpy( szBeginDate, pszBeginDate, 8 );
    memcpy( szEndDate, pszEndDate, 8 );

    /******
    删除stat_line里的流水
    *****/
    EXEC SQL DELETE FROM stat_line
    WHERE  trans_date between :szBeginDate AND :szEndDate; 
    if( SQLCODE && SQLCODE != SQL_NO_RECORD )
    {
        WriteLog( ERROR, "delete from t_stat_line fail [%d][%s]", SQLCODE, SQLERR );
        return ( FAIL );
    }
    
    EXEC SQL
        INSERT INTO stat_line VALUE
        (SELECT a.shop_no, a.pos_no, b.market_no, trans_date, c.pan,
            pur_count, pur_amount, pur_fee, pay_in_count, pay_in_amount, pay_in_fee,
            pay_out_count, pay_out_amount, pay_out_fee, type_1_count, type_1_amount, type_1_fee,
            type_2_count, type_2_amount, type_2_fee, type_3_count, type_3_amount, type_3_fee,
            '', b.dept_detail, b.shop_name, d.dept_name, e.market_name, '0'
        FROM
        (SELECT
            SUM(CASE WHEN trans_type = :iTransTypes[0] THEN 1 ELSE 0 END) pur_count,
            SUM(CASE WHEN trans_type = :iTransTypes[0] THEN amount ELSE 0 END) pur_amount,
            SUM(CASE WHEN trans_type = :iTransTypes[0] THEN addi_amount ELSE 0 END) pur_fee,
            SUM(CASE WHEN trans_type = :iTransTypes[1] THEN 1 ELSE 0 END) pay_in_count,
            SUM(CASE WHEN trans_type = :iTransTypes[1] THEN amount ELSE 0 END) pay_in_amount,
            SUM(CASE WHEN trans_type = :iTransTypes[1] THEN addi_amount ELSE 0 END) pay_in_fee,
            SUM(CASE WHEN trans_type = :iTransTypes[2] THEN 1 ELSE 0 END) pay_out_count,
            SUM(CASE WHEN trans_type = :iTransTypes[2] THEN amount ELSE 0 END) pay_out_amount,
            SUM(CASE WHEN trans_type = :iTransTypes[2] THEN addi_amount ELSE 0 END) pay_out_fee,
            SUM(CASE WHEN trans_type = :iTransTypes[3] THEN 1 ELSE 0 END) type_1_count,
            SUM(CASE WHEN trans_type = :iTransTypes[3] THEN amount ELSE 0 END) type_1_amount,
            SUM(CASE WHEN trans_type = :iTransTypes[3] THEN addi_amount ELSE 0 END) type_1_fee,
            SUM(CASE WHEN trans_type = :iTransTypes[4] THEN 1 ELSE 0 END) type_3_count,
            SUM(CASE WHEN trans_type = :iTransTypes[4] THEN amount ELSE 0 END) type_3_amount,
            SUM(CASE WHEN trans_type = :iTransTypes[4] THEN addi_amount ELSE 0 END) type_3_fee,
            SUM(CASE WHEN trans_type = :iTransTypes[5] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[6] THEN  1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[7] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[8] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[9] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[10] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[11] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[12] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[13] THEN 1 ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[14] THEN 1 ELSE 0 END) type_2_count,
            SUM(CASE WHEN trans_type = :iTransTypes[5] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[6] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[7] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[8] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[9] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[10] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[11] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[12] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[13] THEN amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[14] THEN amount ELSE 0 END) type_2_amount,
            SUM(CASE WHEN trans_type = :iTransTypes[5] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[6] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[7] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[8] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[9] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[10] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[11] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[12] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[13] THEN addi_amount ELSE 0 END)
            + SUM(CASE WHEN trans_type = :iTransTypes[14] THEN addi_amount ELSE 0 END) type_2_fee,
            shop_no, pos_no, host_date trans_date
            FROM posls WHERE 1=1
            AND return_code = '00' AND cancel_flag = 'N' AND recover_flag = 'N'
            AND host_date != ' ' AND host_date between '20111111' and '20122222'
            GROUP BY shop_no, pos_no, host_date)
        a, shop b, register_card c, dept d, market e
        WHERE a.shop_no = b.shop_no(+) AND a.shop_no = c.shop_no(+) AND a.pos_no = c.pos_no(+)
        AND c.status = '1' AND ltrim(rtrim(b.dept_no)) = ltrim(rtrim(d.dept_no(+)))
        AND ltrim(rtrim(d.dept_no)) = ltrim(rtrim(e.dept_no(+))) );

    if( SQLCODE!=0 && SQLCODE!=SQL_NO_RECORD )
    {
        WriteLog(ERROR,"取交易信息出错[%ld][%s]", SQLCODE, SQLERR);
        return (FAIL);
    }
        
    return SUCC;
}
