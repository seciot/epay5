/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:

** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.3 $
$Log: QueryTotal.ec,v $
Revision 1.3  2013/02/21 06:20:01  fengw

1、修改SQL语句中错误的商户号、终端号字段名。

Revision 1.2  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.1  2013/01/21 05:53:31  wukj
新增明细查询函数


*******************************************************************/
# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    EXEC SQL INCLUDE "../incl/DbStru.h";
EXEC SQL EnD DECLARE SECTION;
#endif

/*****************************************************************
** 功    能:查询本终端交易统计数据 
** 输入参数:
** 输出参数:
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int
QueryTotal( ptAppStru ) 
T_App	*ptAppStru;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	szTermId[17], szMerchId[17];
		char	szBeginDate[9], szEndDate[9];
		int	iTotal, iTransType, iTransType1, iTransType2, iTransType3, iTransType4, iTransType5;
		int	iTransType6, iTransType7;
		double	dAmount;
	EXEC SQL END DECLARE SECTION;

	char szTmpStr[100], szData[2048];
	int  iCurPos, iLine;

	strcpy( szMerchId, ptAppStru->szShopNo);
	strcpy( szTermId, ptAppStru->szPosNo);

       if( strlen(ptAppStru->szInDate) == 0 ||
	    memcmp(ptAppStru->szInDate, "FFFFFFFF", 8) == 0 )
	{
		strcpy( szBeginDate, "20090101" );
	}
	else
	{
		strcpy( szBeginDate, ptAppStru->szInDate);
	}

	if( strlen(ptAppStru->szHostDate) == 0 ||
	    memcmp(ptAppStru->szHostDate, "FFFFFFFF", 8) == 0 )
	{
		GetSysDate( szEndDate );
		GetSysDate( ptAppStru->szHostDate);
	}
	else
	{
                strcpy( szEndDate, ptAppStru->szHostDate);
        }

	/* 消费 */
	iTransType1 = PURCHASE;

	EXEC SQL SELECT count(*), nvl(sum(amount), 0) INTO :iTotal, :dAmount
	FROM posls
	WHERE shop_no = :szMerchId AND pos_no = :szTermId AND
              return_code = '00' AND recover_flag = 'N' AND cancel_flag = 'N' AND
	      pos_date >= :szBeginDate AND pos_date <= :szEndDate AND
	      trans_type = :iTransType1;
	if( SQLCODE )
	{
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		WriteLog( ERROR, "count posls fail %d", SQLCODE );
		return FAIL;
	}
	if( iTotal == 0 )
	{
		dAmount = 0.0;
	}
	iCurPos = 0;	
	iLine = 0;
	/* 标题栏 */
	strcpy( szTmpStr, "交易 总笔数 总金额" );
	sprintf( szData+iCurPos, "%-20.20s", szTmpStr );
	iCurPos += 20;
	memcpy( szData+iCurPos, "\x0A", 1 );
	iCurPos ++;
	iLine ++;
	
	sprintf( szTmpStr, "消费%3ld %12.2f", iTotal, dAmount );
	sprintf( szData+iCurPos, "%-20.20s", szTmpStr );
	iCurPos += 20;
	memcpy( szData+iCurPos, "\x0A", 1 );
	iCurPos ++;
	iLine ++;

	iTransType = REFUND;
	//iTransType1 = TRAN_OUT_OTHER;
	/* 退货总笔数、总金额 */
	EXEC SQL SELECT count(*), nvl(sum(amount), 0) INTO :iTotal, :dAmount
	FROM posls
	WHERE SHOP_NO = :szMerchId AND POS_NO = :szTermId AND
              return_code = '00' AND recover_flag = 'N' AND cancel_flag = 'N' AND
	      pos_date >= :szBeginDate AND pos_date <= :szEndDate AND
	      trans_type = :iTransType;
	if( SQLCODE )
	{
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		WriteLog( ERROR, "count posls fail %d", SQLCODE );
		return FAIL;
	}
	if( iTotal == 0 )
	{
		dAmount = 0.0;
	}

	

	/* 退货 */
	sprintf( szTmpStr, "退货%3ld %12.2f", iTotal, dAmount );
	sprintf( szData+iCurPos, "%-20.20s", szTmpStr );
	iCurPos += 20;
	memcpy( szData+iCurPos, "\x0A", 1 );
	iCurPos ++;
	iLine ++;

	if( iCurPos > sizeof(ptAppStru->szReserved) || iCurPos > 255 )
	{
		WriteLog( ERROR, "data too long %ld", iCurPos );
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}
	ptAppStru->iReservedLen = iCurPos;
	memcpy( ptAppStru->szReserved, szData, ptAppStru->iReservedLen );

	strcpy( ptAppStru->szRetCode, TRANS_SUCC );
	return ( SUCC );
}
