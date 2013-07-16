/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:
** 创 建 人:
** 创建日期:


$Revision: 1.2 $
$Log: ChkOperValid.ec,v $
Revision 1.2  2012/12/20 06:43:05  wukj
*** empty log message ***

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
** 功    能:检查操作员是否存在，并校验密码。如果是szOperNo为管理员"0001"的操作员第一次登陆，则直接插入表terminal_oper。
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
*****************************************************************/
int ChkOperValid(ptAppStru)
T_App *ptAppStru;
{
    int iRet = 0;
    EXEC SQL BEGIN DECLARE SECTION;
        
        char  szMsShopNo[15+1];
        char  szMsPosNo[15+1];
        char  szMsOperNo[4+1];
        char  szMsOperPwd[6+1];

        char  szOperPwd[6+1];
        char  szOperName[20];
        int   iDelFlag;
        int   iLoginStatus;
    EXEC SQL END DECLARE SECTION;
     

    memset( szMsShopNo, 0, sizeof(szMsShopNo)    );
    memset( szMsPosNo, 0, sizeof( szMsPosNo) );
    memset( szMsOperNo, 0, sizeof( szMsOperNo) );
    memset( szMsOperPwd, 0, sizeof(szMsOperPwd) );

    strcpy( szMsShopNo,  ptAppStru->szShopNo);
    strcpy( szMsPosNo,  ptAppStru->szPosNo);
    memcpy( szMsOperNo,  ptAppStru->szFinancialCode, 4);
    memcpy( szMsOperPwd,  ptAppStru->szBusinessCode, 6);

    EXEC SQL SELECT     
        NVL(OPER_PWD,' '),
        NVL(OPER_NAME,' '),
        NVL(DEL_FLAG, 0),
        NVL(LOGIN_STATUS,0)
    INTO 
        :szOperPwd,
        :szOperName,
        :iDelFlag,
        :iLoginStatus
    
    FROM
        terminal_oper t
    WHERE
        t.shop_no=:szMsShopNo
        and t.oper_no=:szMsOperNo 
        and t.pos_no =:szMsPosNo
        and t.del_flag=0 ;
    if( SQLCODE )
    {
        WriteLog(ERROR,"商户[%s]操作员[%s]不存在,SQLCODE[%d]",szMsShopNo,szMsOperNo,SQLCODE );
        strcpy(ptAppStru->szRetCode, ERR_EPOS_OPERPWD_ERROR);
        return SQL_NO_RECORD ;
    }
    iRet = strcmp( ptAppStru->szBusinessCode, szOperPwd);
    if( iRet != 0 )
    {
        WriteLog(ERROR,"商户[%s]操作员[%s]密码不符",szMsShopNo,szMsOperNo);
        strcpy(ptAppStru->szRetCode, ERR_EPOS_OPERPWD_ERROR);
        return FAIL;
    }
    
    return SUCC;
}
