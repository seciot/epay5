/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:
** 创 建 人:
** 创建日期:


$Revision: 1.1 $
$Log: GetOperInfo.ec,v $
Revision 1.1  2012/12/18 04:29:59  wukj
*** empty log message ***


*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
        EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif
/*****************************************************************
** 功    能:取终端操作员信息
** 输入参数:
           oper_no 操作员编号
** 输出参数:
           tTermOperStru  终端操作员表结构体
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
*****************************************************************/
int GetOperInfo( ptAppStru , ptTerminalOper)
T_App *ptAppStru ;
T_TERMINAL_OPER *ptTerminalOper;
{
    EXEC SQL BEGIN DECLARE SECTION;
        
        char    szMsOperNo[4+1];
        char     szMsShopNo[15+1];
        char    szMsPosNo[15+1];

        char  szShopNo[15+1] ;
        char  szPosNo[15+1];
        char  szOperNo[4+1];
        char  szOperPwd[6+1];
        char  szOperName[20];
        int   iDelFlag;
        int   iLoginStatus;
    EXEC SQL END DECLARE SECTION;
    
    memset( &ptTerminalOper, 0, sizeof(T_TERMINAL_OPER) );
    memset( szMsOperNo, 0, sizeof(szMsOperNo) );
    memset( szMsShopNo, 0, sizeof(szMsShopNo) );
    memset( szMsPosNo, 0, sizeof(szMsPosNo) );
    
    memcpy( szMsOperNo, ptAppStru->szFinancialCode, 4);
    strcpy( szMsShopNo, ptAppStru->szShopNo );
    strcpy( szMsPosNo, ptAppStru->szPosNo );
        
    EXEC SQL SELECT 
        NVL(SHOP_NO,' '),
        NVL(POS_NO,' '),
        NVL(OPER_NO,' '),
        NVL(OPER_PWD,' '),
        NVL(OPER_NAME,' '),
        NVL(DEL_FLAG, 0),
        NVL(LOGIN_STATUS,0)
    
    INTO 
        :szShopNo,
        :szPosNo,
        :szOperNo,
        :szOperPwd,
        :szOperName,
        :iDelFlag,
        :iLoginStatus
    FROM
         terminal_oper t
    WHERE
        t.shop_no =:szMsShopNo
        and t.pos_no =:szMsPosNo  
        and t.oper_no =:szMsOperNo ;
    if(SQLCODE == SQL_NO_RECORD)
    {
        return SQL_NO_RECORD;
    }
    if( SQLCODE )
    {
        WriteLog(ERROR,"取操作员信息失败，SQLCODE[%d]", SQLCODE );
        return FAIL;
    }
            
    strcpy(ptTerminalOper->szShopNo,szShopNo);
    strcpy(ptTerminalOper->szPosNo,szPosNo);
    strcpy(ptTerminalOper->szOperNo,szOperNo);
    strcpy(ptTerminalOper->szOperPwd,szOperPwd);
    strcpy(ptTerminalOper->szOperName,szOperName);
    ptTerminalOper->iDelFlag =  iDelFlag;
    ptTerminalOper->iLoginStatus = iLoginStatus;
    return SUCC;
}

