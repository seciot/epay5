/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端登记相关

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.2 $
$Log: Term.ec,v $
Revision 1.2  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.1  2012/12/18 10:25:53  wukj
*** empty log message ***

Revision 1.9  2012/12/18 04:57:16  wukj
*** empty log message ***

Revision 1.8  2012/12/14 02:09:24  wukj
*** empty log message ***

Revision 1.7  2012/12/05 06:32:01  wukj
*** empty log message ***

Revision 1.6  2012/12/03 03:25:08  wukj
int类型前缀修改为i

Revision 1.5  2012/11/29 10:09:04  wukj
日志,bcdasc转换等修改

Revision 1.4  2012/11/19 01:58:29  wukj
修改app结构变量,编译通过

Revision 1.3  2012/11/16 08:38:12  wukj
修改app结构变量名称

Revision 1.2  2012/11/16 03:25:05  wukj
新增CVS REVSION LOG注释

*******************************************************************/

#include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
        EXEC SQL INCLUDE SQLCA;
        EXEC SQL INCLUDE "../incl/DbStru.h";
EXEC SQL EnD DECLARE SECTION;
#endif



/*****************************************************************
** 功    能:终端登记申请
** 输入参数:
          ptAppStru
        
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
TermRegister( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[17];
        char    szTeleNo[40];
        char    szShopNo[16];
        char    szPosNo[16];
        char    szPutDate[9];
        int iCount;
    EXEC SQL END DECLARE SECTION;
    char szDate[8+1];
    int    iRet;
    memset( szPsamNo, 0, sizeof(szPsamNo));
    memset( szTeleNo, 0, sizeof(szTeleNo));
    memset( szShopNo, 0, sizeof(szShopNo));
    memset( szPosNo, 0, sizeof(szPosNo));
    memset( szDate , 0, sizeof(szDate));

    strcpy( szPsamNo, ptAppStru->szPsamNo);
    strcpy( szTeleNo, ptAppStru->szBusinessCode);
    strcpy( szShopNo, ptAppStru->szReserved);
    strcpy( szPosNo, ptAppStru->szFinancialCode);
    if( strlen(ptAppStru->szReserved) == 0 || strlen(ptAppStru->szFinancialCode) == 0)
    {
        WriteLog( ERROR, "请输入有效的商户号或终端号[%s],[%s]", szShopNo,szPosNo );
        strcpy( ptAppStru->szRetCode, ERR_INVALID_TERM );
        return FAIL;
    }
    if (strlen(ptAppStru->szBusinessCode) == 0)
    {
        strcpy(szTeleNo, "00000000");
    }
            
    /* 检查PSAM卡是否被登记 */
    EXEC SQL SELECT count(*) INTO :iCount
        FROM terminal
        WHERE psam_no = :szPsamNo;
    if( SQLCODE && SQLCODE != SQL_NO_RECORD)
    {
        WriteLog( ERROR, "select terminal fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }
    if (iCount > 0)
    {
        WriteLog( ERROR, "PSAM卡已经被登记，请核实[%s]",szPsamNo );
        strcpy( ptAppStru->szRetCode, PSAMNO_INVALID );
        return FAIL;
    }
    /* 检查商户号、终端号是否被登记 */
    EXEC SQL SELECT count(*) INTO :iCount
        FROM terminal
        WHERE shop_no = :szShopNo and pos_no = :szPosNo;
    if( SQLCODE && SQLCODE != SQL_NO_RECORD)
    {
        WriteLog( ERROR, "select terminal fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }
    if (iCount > 0)
    {
        WriteLog( ERROR, "商户号、终端号已经被登记，请核实[%s][%s]",szShopNo,szPosNo );
        strcpy( ptAppStru->szRetCode, SHOP_TERM_INVALID );
        return FAIL;
    }
    
    /* 保持终端信息 */
    GetSysDate( szDate );
    strcpy(szPutDate, szDate);
    EXEC SQL INSERT INTO TERMINAL 
             (SHOP_NO, POS_NO, PSAM_NO,
              TELEPHONE,TERM_MODULE,PSAM_MODULE,
              POS_TYPE,PUT_DATE,FIRST_PAGE,
              STATUS,APP_TYPE,ADDRESS)
         VALUES
              (:szShopNo, :szPosNo, :szPsamNo,
              :szTeleNo, 1, 1,
              'SPP-100',:szPutDate, 0,
              1,3,'客户地址');
    if( SQLCODE )
    {
        WriteLog( ERROR, "insert terminal fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }
    CommitTran();

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}

/*****************************************************************
** 功    能:终端绑定(话机登记)
** 输入参数:
          ptAppStru
        
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
int TermTelRegister( ptAppStru, iTeleLen ) 
T_App    *ptAppStru;
int iTeleLen;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[17], szTeleNo[40], szIp[16];
        char    szPin[9], szPinKey[33];
        int    iTermModu, iKeyIndex;
    EXEC SQL END DECLARE SECTION;
    char     szTmpStr[9], szBcdPinKey[17], szHsmRet[10];
    int    iRet;

    iTermModu = ptAppStru->iTermModule;
    strcpy( szIp, ptAppStru->szIp );
    if( strlen(ptAppStru->szBusinessCode) == 0 )
    {
        /*去掉电话号码前的区号*/
        memcpy( szTeleNo, ptAppStru->szCalledTelByTerm+strlen(ptAppStru->szCalledTelByTerm)-iTeleLen, iTeleLen );
        szTeleNo[iTeleLen] = 0;
    }
    else
    {
        strcpy( szTeleNo, ptAppStru->szBusinessCode);
    }
    strcpy( szPsamNo, ptAppStru->szPsamNo);

    memcpy( szTmpStr, szPsamNo+8, 8 );
    szTmpStr[8] = 0;
    iKeyIndex = atol(szTmpStr);
    
    /* 检查输入的终端管理密码是否正确 */
    EXEC SQL SELECT manager_pwd INTO :szPin
        FROM terminal_para
        WHERE module = :iTermModu;
    if( SQLCODE == SQL_NO_RECORD )
    {
        WriteLog( ERROR, "module %d not exist", iTermModu );
        strcpy( ptAppStru->szRetCode, ERR_TERM_MODULE );
        return FAIL;
    }
    else if( SQLCODE )
    {
        WriteLog( ERROR, "select manager_pwd fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    EXEC SQL SELECT pin_key INTO :szPinKey
        FROM pos_key
        WHERE key_index = :iKeyIndex;    
    if( SQLCODE )
    {
        WriteLog( ERROR, "select pin_key fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }
    
    AscToBcd( (uchar *)szPinKey, 32, 0, (uchar *)szBcdPinKey );

    iRet = HsmVerifyPin( ptAppStru, szPin, szBcdPinKey, szHsmRet );
        if( iRet != SUCC )
        {
            WriteLog( ERROR, "encrypt pin fail" );
                   strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
                   return ( FAIL );
        }

    /* 密码密文是否一致 */
    if( memcmp( szHsmRet, "SUCC", 4 ) != 0 )
    {
        WriteLog( ERROR, "passwd error" );
                   strcpy( ptAppStru->szRetCode, ERR_OPERPWD_ERROR );
                   return ( FAIL );
    }

    EXEC SQL UPDATE terminal set telephone = :szTeleNo, ip = :szIp
    where psam_no = :szPsamNo;
    if( SQLCODE )
    {
        WriteLog( ERROR, "update terminal fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        RollbackTran();
        return FAIL;
    }
    CommitTran();

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}

