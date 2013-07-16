/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:

** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.3 $
$Log: QueryDetail.ec,v $
Revision 1.3  2013/02/21 06:22:14  fengw

1、修改SQL语句。

Revision 1.2  2013/01/24 07:32:36  fengw
*** empty log message ***

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
** 功    能:根据商户号、终端号、起止交易日期、卡号查询交易明细
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
QueryDetail( ptAppStru, iQueVoid ) 
T_App	*ptAppStru;
int iQueVoid;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	szTermId[17], szMerchId[17], szTransName[21], szPsamNo[17];
		char	szBuf[2048];
		char	szBeginDate[9], szEndDate[9], szPan[21], szCancel[5];
		int	iTotal, iTransType, lTrace;
		int	iTransType1, iTransType2, iTransType3, iTransType4;
		int	iTransType5;
		struct T_POSLS{
    			char    szHostDate[9];
    			char    szHostTime[7];
    			char    szPan[20];
    			double  dAmount;
    			char    szCardType[2];
    			int     iTransType;
    			int     szBusinessType;
    			char    szRetriRefNum[13];
    			char    szAuthCode[7];
    			char    szPosNo[16];
    			char    szShopNo[16];
    			char    szAccount2[19];
    			double  dAddiAmount;
    			int     iBatchNo;
    			char    szPsamNo[17];
    			int     iInvoice;
    			char    szRetCode[3];
    			char    szHostRetCode[7];
    			char    szCancelFlag[2];
    			char    szRecoverFlag[2];
    			char    szPosSettle[2];
    			char    szPosBatch[2];
    			char    szHostSettle[2];
    			int     iSysTrace;
    			char    szOldRetriRefNum[13];
    			char    szPosDate[9];
    			char    szPosTime[7];
    			char    szFinancialCode[41];
    			char    szBusinessCode[41];
    			char    szBankId[12];
    			char    szSettleDate[9];
    			char    szOperNo[5];
    			char    szMac[17];
			    int     iPosTrace;
			    char    szDeptDetail[71];
		} tPosLs;
		struct T_QUERY_CONDITION{
    			char    szPsamNo[17];
    			char    szPan[20];
    			char    szBeginDate[9];
    			char    szEndDate[9];
    			char    szShopNo[17];
    			char    szPosNo[17];
    			double  dAmount;
		}tQueryCon;
	EXEC SQL END DECLARE SECTION;

	char szTmpStr[100], szData[2048], szCmd[100];
	int  iCmdLen, iCurPos, iDataLen, iBeginRecNo, iEndRecNo, iCurRecNum, iRecNo, iLine;
	int  iDateFlag;
	long	lAmt;

	strcpy( szPsamNo, ptAppStru->szPsamNo);
	lTrace = ptAppStru->lPosTrace;
	iTransType1 = TRAN_CANCEL;
	iTransType2 = TRAN_IN_CANCEL;
	iTransType3 = TRAN_OUT_CANCEL;
	iTransType4 = PUR_CANCEL;

	/* 查询交易之后续查询 */
	if( memcmp( ptAppStru->szTransCode, "00", 2 ) != 0 )
	{
		AscToBcd( (uchar*)(ptAppStru->szTransCode), 2, 0 ,(uchar*)szBuf);
		szBuf[1] = 0;
		iBeginRecNo = (uchar)szBuf[0];

		/* 从数据库中查询之前输入的查询条件 */
		EXEC SQL SELECT 
			PSAM_NO,
  			NVL(PAN,' '),
  			NVL(BEGIN_DATE,' '),
  			NVL(END_DATE  ,' '),
  			NVL(SHOP_NO ,' '),  
  			NVL(POS_NO  ,' '),  
  			NVL(AMOUNT ,0)   
		INTO 
			:tQueryCon.szPsamNo,
			:tQueryCon.szPan,
			:tQueryCon.szBeginDate,
			:tQueryCon.szEndDate,
			:tQueryCon.szShopNo,
			:tQueryCon.szPosNo,
			:tQueryCon.dAmount
		FROM query_condition
		WHERE psam_no = :szPsamNo;
		if( SQLCODE )
		{
			WriteLog( ERROR, "select condition fail %d", SQLCODE );
			strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
			return FAIL;
		}
		DelTailSpace( tQueryCon.szPan);
		DelTailSpace( tQueryCon.szBeginDate);
		DelTailSpace( tQueryCon.szEndDate);
		DelTailSpace( tQueryCon.szShopNo);
		DelTailSpace( tQueryCon.szPosNo);

		strcpy( szPan, tQueryCon.szPan);
		strcpy( szBeginDate, tQueryCon.szBeginDate);
		strcpy( szEndDate, tQueryCon.szEndDate);
		strcpy( szMerchId, tQueryCon.szShopNo);
		strcpy( szTermId, tQueryCon.szPosNo);
	}
	else
	{
		iBeginRecNo = 0;

		/* 前端输入查询条件 */

		/* 查询本终端 */
		if( ptAppStru->iTransType== QUERY_DETAIL_SELF )
		{
			strcpy( szMerchId, ptAppStru->szShopNo);
			strcat( szMerchId, "%" );
			strcpy( szTermId, ptAppStru->szPosNo);
			strcat( szTermId, "%" );
		}
		/* 查询指定终端 */
		else
		{
			strcpy( szMerchId, "%" );
			strcpy( szTermId, ptAppStru->szBusinessCode);
			strcat( szTermId, "%" );
		}

		strcpy( szPan, ptAppStru->szFinancialCode);
		strcat( szPan, "%" );

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

		memset( (char *)&tQueryCon, 0, sizeof(struct T_QUERY_CONDITION) );
		strcpy( tQueryCon.szPsamNo, szPsamNo );
		strcpy( tQueryCon.szBeginDate, szBeginDate );
		strcpy( tQueryCon.szEndDate, szEndDate );
		strcpy( tQueryCon.szPan, szPan );
		strcpy( tQueryCon.szShopNo, szMerchId );
		strcpy( tQueryCon.szPosNo, szTermId );

		/* 删除原有查询条件，插入新的查询条件 */
		EXEC SQL DELETE
		FROM query_condition
		WHERE psam_no = :szPsamNo;
		if( SQLCODE != SQL_NO_RECORD && SQLCODE != 0 )
		{
			WriteLog( ERROR, "select condition fail %d", SQLCODE );
			strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
			RollbackTran();
			return FAIL;
		}
		CommitTran();

		EXEC SQL
            INSERT INTO query_condition
            (psam_no, pan, begin_date, end_date, shop_no, pos_no)
			VALUES
            (:tQueryCon.szPsamNo, :tQueryCon.szPan, :tQueryCon.szBeginDate,
            :tQueryCon.szEndDate, :tQueryCon.szShopNo, :tQueryCon.szPosNo);
		if( SQLCODE )
		{
			WriteLog( ERROR, "insert condition fail %d", SQLCODE );
			strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
			RollbackTran();
			return FAIL;
		}
		CommitTran();
	}

	if( iQueVoid == YES )
	{
		strcpy( szCancel, "%" );
	}
	else
	{
		strcpy( szCancel, "N%" );
	}

	/*取符合查询条件的流水笔数 */
	EXEC SQL SELECT count(*) INTO :iTotal
	FROM posls
	WHERE shop_no like :szMerchId AND shop_no like :szTermId AND
              return_code = '00' AND recover_flag = 'N' AND cancel_flag like :szCancel AND
	      pos_date >= :szBeginDate AND pos_date <= :szEndDate AND pan like :szPan AND
	      trans_type != :iTransType1 AND trans_type != :iTransType2 AND
	      trans_type != :iTransType3 AND trans_type != :iTransType4 ;
	if( SQLCODE )
	{
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		WriteLog( ERROR, "count posls fail %d", SQLCODE );
		return FAIL;
	}
	if( iTotal == 0 )
	{
		strcpy( ptAppStru->szRetCode, ERR_NO_TRACE );
		return FAIL;
	}

	EXEC SQL DECLARE LsTerm_cur cursor FOR
	SELECT 
        	NVL(HOST_DATE,' '),
        	NVL(HOST_TIME,' '),
        	NVL(PAN,' '),
        	NVL(AMOUNT,0.00),
        	NVL(CARD_TYPE,'1'),
        	NVL(TRANS_TYPE, 0), 
        	NVL(BUSINESS_TYPE, 0), 
        	NVL(RETRI_REF_NUM,' '),
        	NVL(AUTH_CODE, ' '),
        	NVL(POS_NO,' '),
        	NVL(SHOP_NO, ' '),
        	NVL(ACCOUNT2, ' '),
        	NVL(ADDI_AMOUNT, 0.00),
        	NVL(BATCH_NO, 1), 
        	NVL(PSAM_NO,' '),
        	NVL(INVOICE, 1), 
        	NVL(RETURN_CODE, ' '),
        	NVL(HOST_RET_CODE,' '),
        	NVL(CANCEL_FLAG, 'N'),
        	NVL(RECOVER_FLAG, 'N'),
        	NVL(POS_SETTLE,'N'),
        	NVL(POS_BATCH, 'N'),
        	NVL(HOST_SETTLE,'N'),
        	NVL(SYS_TRACE,1),
        	NVL(OLD_RETRI_REF_NUM,' '),
        	NVL(POS_DATE,' '),
        	NVL(POS_TIME,' '),
        	NVL(FINANCIAL_CODE,' '),
        	NVL(BUSINESS_CODE, ' '),
        	NVL(BANK_ID, ' '),
        	NVL(SETTLE_DATE,' '),
        	NVL(OPER_NO, '0001'),
        	NVL(MAC,' '),
        	NVL(POS_TRACE, 1)
	FROM posls
	WHERE SHOP_NO like :szMerchId AND POS_NO like :szTermId AND
              return_code = '00' AND recover_flag = 'N' AND cancel_flag like :szCancel AND
	      pos_date >= :szBeginDate AND pos_date <= :szEndDate AND pan like :szPan AND
	      trans_type != :iTransType1 AND trans_type != :iTransType2 AND
	      trans_type != :iTransType3 AND trans_type != :iTransType4 
	ORDER BY sys_trace;

	if( SQLCODE )
	{
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		WriteLog( ERROR, "delare LsTerm_cur fail %d", SQLCODE );
		return FAIL;
	}


	EXEC SQL OPEN LsTerm_cur;
	if( SQLCODE )
	{
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		WriteLog( ERROR, "open LsTerm_cur fail %d", SQLCODE );
		EXEC SQL CLOSE LsTerm_cur;
		return FAIL;
	}

	iRecNo = 0;
	iCurRecNum = 0;
	iDataLen = 0;

	while(1)
	{
		EXEC SQL FETCH LsTerm_cur 
		INTO 
        		:tPosLs.szHostDate,
        		:tPosLs.szHostTime,
        		:tPosLs.szPan,
        		:tPosLs.dAmount,
        		:tPosLs.szCardType,
        		:tPosLs.iTransType,
        		:tPosLs.szBusinessType,
        		:tPosLs.szRetriRefNum,
        		:tPosLs.szAuthCode,
        		:tPosLs.szPosNo,
        		:tPosLs.szShopNo,
        		:tPosLs.szAccount2,
        		:tPosLs.dAddiAmount,
        		:tPosLs.iBatchNo,
        		:tPosLs.szPsamNo,
        		:tPosLs.iInvoice,
        		:tPosLs.szRetCode,
        		:tPosLs.szHostRetCode,
        		:tPosLs.szCancelFlag,
        		:tPosLs.szRecoverFlag,
        		:tPosLs.szPosSettle,
        		:tPosLs.szPosBatch,
        		:tPosLs.szHostSettle,
        		:tPosLs.iSysTrace,
        		:tPosLs.szOldRetriRefNum,
        		:tPosLs.szPosDate,
        		:tPosLs.szPosTime,
        		:tPosLs.szFinancialCode,
        		:tPosLs.szBusinessCode,
        		:tPosLs.szBankId,
        		:tPosLs.szSettleDate,
        		:tPosLs.szOperNo,
        		:tPosLs.szMac,
        		:tPosLs.iPosTrace;
		if( SQLCODE == SQL_NO_RECORD )
		{
			EXEC SQL CLOSE LsTerm_cur;
			break;
		}
		else if( SQLCODE )
		{
			strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
			WriteLog( ERROR, "fetch LsTerm_cur fail %d", SQLCODE );
			EXEC SQL CLOSE LsTerm_cur;
			return FAIL;
		}
		iRecNo ++;

		/* 已下载，取下一条 */
		if( iRecNo <= iBeginRecNo )
		{
			continue;
		}

		DelTailSpace( tPosLs.szPan);
		DelTailSpace( tPosLs.szAccount2);
		DelTailSpace( tPosLs.szBusinessCode);
		DelTailSpace( tPosLs.szFinancialCode);
		DelTailSpace( tPosLs.szShopNo);
		DelTailSpace( tPosLs.szPosNo);

		iTransType = tPosLs.iTransType;

		/* 取交易名称 */
		EXEC SQL SELECT trans_name INTO :szTransName
		FROM trans_def
		WHERE trans_type = :iTransType;
		if( SQLCODE )
		{
			WriteLog( ERROR, "get trans_name fail %d", SQLCODE );
			strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
			EXEC SQL CLOSE LsTerm_cur;
			return FAIL;
		}
		DelTailSpace( szTransName );

		iCurPos = 0;
		iLine = 0;

		/* 交易名称 */
		if( tPosLs.szCancelFlag[0] == 'Y' )
		{
			sprintf( szTmpStr, "%s 已撤销|", szTransName );
		}
		else
		{
			sprintf( szTmpStr, "%s|", szTransName );
		}
		memcpy( szBuf+iCurPos, szTmpStr, strlen(szTmpStr) );
		iCurPos += strlen(szTmpStr);
		iLine ++;

		/* 第一卡号 */
		if( iTransType == TRANS || iTransType == TRAN_IN || iTransType == TRAN_OUT ||
		    iTransType == PAY_CREDIT )
		{
			sprintf( szTmpStr, "付%s|", tPosLs.szPan);
		}	
		else
		{
			sprintf( szTmpStr, "卡号%s|", tPosLs.szPan);
		}

		memcpy( szBuf+iCurPos, szTmpStr, strlen(szTmpStr) );
		iCurPos += strlen(szTmpStr);
		iLine ++;

		/* 第二卡号或用户号 */
		if( iTransType == TRANS || iTransType == TRAN_IN || iTransType == TRAN_OUT ||
		    iTransType == PAY_CREDIT )
		{
			sprintf( szTmpStr, "收%s|", tPosLs.szAccount2);
		}	
		else if( iTransType == PURCHASE )
		{
			sprintf( szTmpStr, "授权码:%s|", tPosLs.szAuthCode);
		}
		else if( iTransType == REFUND )
		{
			sprintf( szTmpStr, "原参考号:%s|", tPosLs.szFinancialCode);
		}
		else
		{
			sprintf( szTmpStr, "用户号:%s|", tPosLs.szFinancialCode);
		}
		memcpy( szBuf+iCurPos, szTmpStr, strlen(szTmpStr) );
		iCurPos += strlen(szTmpStr);;
		iLine ++;

		/* 交易金额 */
		/*add by gaomx 20100921 大金额溢出问题*/
		sprintf( szTmpStr, "金额:%.2f元|", tPosLs.dAmount);
		/*add end*/
		memcpy( szBuf+iCurPos, szTmpStr, strlen(szTmpStr) );
		iCurPos += strlen(szTmpStr);
		iLine ++;

		/* 交易时间 */
		sprintf( szTmpStr, "%4.4s-%2.2s-%2.2s %2.2s:%2.2s:%2.2s|", tPosLs.szPosDate, tPosLs.szPosDate+4, 
			tPosLs.szPosDate+6, tPosLs.szPosTime, tPosLs.szPosTime+2, tPosLs.szPosTime+4 );
		memcpy( szBuf+iCurPos, szTmpStr, strlen(szTmpStr) );
		iCurPos += strlen(szTmpStr);
		iLine ++;

		/* 交易流水号 */
		sprintf( szTmpStr, "流水号:%ld|", tPosLs.iPosTrace);
		memcpy( szBuf+iCurPos, szTmpStr, strlen(szTmpStr) );
		iCurPos += strlen(szTmpStr);
		iLine ++;

		/* 查询其他终端，需要显示商户号、终端号 */
		if( ptAppStru->iTransType == QUERY_DETAIL_OTHER )
		{
			sprintf( szTmpStr, "终端:%s|", tPosLs.szPosNo);
			memcpy( szBuf+iCurPos, szTmpStr, strlen(szTmpStr) );
			iCurPos += strlen(szTmpStr);
			iLine ++;
		}

		/* 空行，打印所有流水时每笔交易之间空一行 */
		memcpy( szBuf+iCurPos, " |", 2 );
		iCurPos += 2;
		iLine ++;

		if( (iDataLen+iCurPos+4) <= MAX_QUERY_LEN )
		{
			memcpy( szData+iDataLen, szBuf, iCurPos );
			iDataLen += iCurPos;
			iCurRecNum ++;
		}
		else
		{
			EXEC SQL CLOSE LsTerm_cur;
			break;
		}
	}
	iEndRecNo = iBeginRecNo+iCurRecNum;

	sprintf( ptAppStru->szReserved, "%06ld%06ld%06ld%06ld%06ld", iDataLen+4, iTotal, iBeginRecNo+1, iEndRecNo, iLine );
	ptAppStru->iReservedLen = 30;	
	szBuf[0] = iTotal;
	szBuf[1] = iBeginRecNo+1;
	szBuf[2] = iEndRecNo;
	szBuf[3] = iLine;
	memcpy( szBuf+4, szData, iDataLen );
	szBuf[iDataLen+4] = 0;
	
	/* 删除该终端以往查询结果 */
	EXEC SQL DELETE FROM query_result
	WHERE psam_no = :szPsamNo;
	if( SQLCODE != SQL_NO_RECORD && SQLCODE != 0 )
	{
		WriteLog( ERROR, "delete query_result fail %d", SQLCODE );
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		RollbackTran();
		return FAIL;
	}
	CommitTran();

	/* 插入本次查询结果，以便tcpcompos模块取出，送往终端 */
	EXEC sQL INSERT INTO query_result VALUES( :szPsamNo, :lTrace, :szBuf );
	if( SQLCODE )
	{
		WriteLog( ERROR, "insert query_result fail %d", SQLCODE );
		strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
		RollbackTran();
		return FAIL;
	}
	CommitTran();
	
	sprintf( ptAppStru->szPan, "下载%03d-%03d / %03d", iBeginRecNo+1, iEndRecNo, iTotal );

	/* 需要进行后续下载 */
	if( iEndRecNo < iTotal )
	{
		//后续交易代码前2位表示已下载记录最大记录号，后6位为当前交易
		//代码后6位
		szBuf[0] = iEndRecNo;
		szBuf[1] = 0;
		BcdToAsc( (uchar*)szBuf, 2, 0 ,(uchar*)(ptAppStru->szNextTransCode));
		memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
		ptAppStru->szNextTransCode[8] = 0;
		
		iCmdLen = ptAppStru->iCommandLen;
		memcpy( ptAppStru->szCommand+iCmdLen, "\x8D", 1 );	//计算MAC
		iCmdLen += 1;
		ptAppStru->iCommandNum++;
		memcpy( ptAppStru->szCommand+iCmdLen, "\x23\x02", 2 );//连接系统
		iCmdLen += 2;
		ptAppStru->iCommandNum ++;
		memcpy( ptAppStru->szCommand+iCmdLen, "\x24\x03", 2 );//发送数据
		iCmdLen += 2;
		ptAppStru->iCommandNum++;
		memcpy( ptAppStru->szCommand+iCmdLen, "\x25\x04", 2 );//接收数据
		iCmdLen += 2;
		ptAppStru->iCommandNum++;

		ptAppStru->iCommandLen= iCmdLen;
	}
	else
	{
		memcpy( ptAppStru->szNextTransCode, "FF", 2 );
		memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
		ptAppStru->szNextTransCode[8] = 0;
	}

	strcpy( ptAppStru->szRetCode, TRANS_SUCC );
	return ( SUCC );
}

