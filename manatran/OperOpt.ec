/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端操作员相关交易
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.1 $
$Log: OperOpt.ec,v $
Revision 1.1  2012/12/18 10:25:53  wukj
*** empty log message ***

Revision 1.10  2012/12/18 04:29:59  wukj
*** empty log message ***

Revision 1.9  2012/12/10 05:32:12  wukj
*** empty log message ***

Revision 1.8  2012/12/05 06:32:01  wukj
*** empty log message ***

Revision 1.7  2012/12/03 03:25:08  wukj
int类型前缀修改为i

Revision 1.6  2012/11/29 10:09:03  wukj
日志,bcdasc转换等修改

Revision 1.5  2012/11/20 07:45:39  wukj
替换\t为空格对齐

Revision 1.4  2012/11/19 01:58:29  wukj
修改app结构变量,编译通过

Revision 1.3  2012/11/16 08:38:12  wukj
修改app结构变量名称

Revision 1.2  2012/11/16 03:25:05  wukj
新增CVS REVSION LOG注释

*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
        EXEC SQL INCLUDE SQLCA;
        EXEC SQL INCLUDE "../incl/DbStru.h";
EXEC SQL EnD DECLARE SECTION;
#endif

int GetEposLoginOperNo(T_App *ptAppStru,char *szOperNo);

/*****************************************************************
** 功    能:终端操作员登陆签到交易
               交易规则：
               数据库表terminal_oper中无此终端的操作员信息
               1、如果终端上送操作员号为“0001”、操作员密码为“000000”，
                  则在terminal_oper表中添加该终端的初始管理员，
                  操作员号为“0001”，初始密码为“000000”，姓名为“管理员”。
               2、否则返回“无此操作员或密码错误”。
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
int EposOperLogin( ptAppStru )
T_App *ptAppStru;
{
    int iRet = 0;
    
    EXEC SQL BEGIN DECLARE SECTION;
        char  szMsOperNo[4+1];
        char  szMsOperPwd[6+1];
        char  szMsShopNo[15+1];
    EXEC SQL END DECLARE SECTION;
    
    memset(    szMsOperNo, 0, sizeof( szMsOperNo) ) ;
    memset(    szMsOperPwd, 0,    sizeof(szMsOperPwd) );    
    memset( szMsShopNo , 0, sizeof(szMsShopNo) );
    
    /*密码长度校验*/
    if(strlen(ptAppStru->szBusinessCode) != 6 )
    {
        strcpy(ptAppStru->szRetCode, ERR_EPOS_OPERPWD_ERROR);
        WriteLog(ERROR,"密码长度不合法");
        return FAIL;
    }
    
    /*拷贝操作员编号 */
    memcpy(    szMsOperNo, ptAppStru->szFinancialCode, 4 ); 
    /*拷贝操作员密码*/
    memcpy(    szMsOperPwd, ptAppStru->szBusinessCode, 6 );
    /*拷贝商户号*/
    memcpy( szMsShopNo, ptAppStru->szShopNo, 15 );
    
    /*在表terminal_oper中校验操作员是否合法:1、操作员是否存在；2、密码是否正确
    *chk_oper_vaild()已对上述两种情况赋值相应错误代码
    */
    WriteLog(TRACE,"操作员登陆交易开始 shop[%s] operno[%s]……",szMsShopNo,szMsOperNo);
    
    BeginTran();
    
    iRet = ChkOperValid( ptAppStru );
    if( iRet  !=  SUCC )
    {
        /*若为商户管理员第一次登陆，则增加该操作员*/
        if( memcmp( ptAppStru->szFinancialCode, "0001", 4) == 0 && 
                memcmp( ptAppStru->szBusinessCode, "000000", 6) == 0 &&
            iRet == SQL_NO_RECORD )
        {
            iRet = Insert2TerminalOper( ptAppStru, "管理员", "000000" );
            if(iRet != SUCC )
            {
                strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
                WriteLog(ERROR,"商户[%s]操作员[%s]增加管理员失败",szMsShopNo,szMsOperNo);
                return FAIL;
            }
            
        }
        else
        {    
            WriteLog(ERROR,"商户[%s]操作员[%s]非法",ptAppStru->szShopNo,ptAppStru->szFinancialCode);
            return FAIL;
        }
    }
    
    WriteLog(TRACE,"操作员合法性检查完毕，状态合法…… shop[%s] operno[%s]……",szMsShopNo,szMsOperNo);
    
    /*修改对应EPOS上其它已登陆操作员得状态为签退*/
    iRet = OtherOperLoginOut( ptAppStru );
    if( iRet != SUCC && iRet != SQL_NO_RECORD )
    {
        RollbackTran();
        WriteLog(ERROR,"签退EPOS【%s】上所有操作员失败！SQLCODE[%d]",ptAppStru->szPosNo,SQLCODE);
        return FAIL;
    }
    
    WriteLog(TRACE,"签退POS上其他操作员……");
    
    /*修改terminal_oper表中对应操作员的pos_no为当前终端编号,*/
    iRet = UpdateOperStatusLogin( ptAppStru );
    if( iRet != SUCC )
    {
        RollbackTran();
        WriteLog(ERROR, "修改商户[%s]操作员[%s]登陆失败",ptAppStru->szShopNo,ptAppStru->szFinancialCode );
        return FAIL;
    }
    
    CommitTran();
    WriteLog(TRACE,"操作员登陆操作完成…… shop[%s] operno[%s]……",szMsShopNo,szMsOperNo);
    
    strcpy(ptAppStru->szRetCode, TRANS_SUCC);
    return SUCC;
}

/*****************************************************************
** 功    能:增加EPOS操作员,该功能只能有编号为"0001"的操作员使用
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
int EposOperAdd(ptAppStru)
T_App *ptAppStru;
{
    int iRet = 0;
    EXEC SQL BEGIN DECLARE SECTION;
        T_TERMINAL_OPER tTerminalOper;
        char  szMsShopNo[15+1];
        char  szMsOperNo[4+1];
        char  szMsOperName[20+1];
    EXEC SQL END DECLARE SECTION;
    
    memset( &tTerminalOper, 0, sizeof( T_TERMINAL_OPER) );
    memset(    szMsOperNo, 0, sizeof( szMsOperNo) );
    memset( szMsOperName, 0, sizeof(szMsOperName) );

    memcpy( szMsOperName, ptAppStru->szBusinessCode, 20 );
    memcpy( szMsShopNo, ptAppStru->szShopNo, 15);
    
    WriteLog(TRACE,"增加操作员交易开始 ");
    
    BeginTran();
    
    /*取出终端登录的操作员编号*/
    iRet = GetEposLoginOperNo(ptAppStru,szMsOperNo);
    if( iRet != SUCC )
    {
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        WriteLog(ERROR," 当前终端无操作员登录，请登录");
        return FAIL;
    }
    
    if( strcmp( szMsOperNo, "0001") != 0 )
    {
        strcpy(ptAppStru->szRetCode, ERR_EPOS_OPT_INVALID);
        WriteLog(ERROR,"商户[%s]操作员[%s]无权增加操作员",ptAppStru->szShopNo ,szMsOperNo);
        return FAIL;
    }
            
    WriteLog(TRACE,"增加操作员交易…… shop[%s] 管理员[%s]……",szMsShopNo,szMsOperNo);    
    
    memset( szMsOperNo, 0, sizeof(szMsOperNo) );
    memcpy( szMsOperNo, ptAppStru->szFinancialCode , 4);

    /*增加操作员*/
    iRet = Insert2TerminalOper( ptAppStru, szMsOperName , "000000" );
    if( SUCC != iRet )
    {
        RollbackTran();
        WriteLog(ERROR,"商户[%s]增加操作员[%s]失败",szMsShopNo,szMsOperNo);
        return FAIL;
    }
    
    CommitTran();
    
    WriteLog(TRACE,"增加操作员交易结束 新增shop[%s] operno[%s]……",szMsShopNo,szMsOperNo);
    
    strcpy(ptAppStru->szRetCode, TRANS_SUCC);
    
    return SUCC;
    
}

/*****************************************************************
** 功    能:删除终端操作员，本交易只能由管理员'0001'完成且'0001'的操作员不能被删除
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
int EposOperDel( ptAppStru )
T_App *ptAppStru;
{
    int iRet = 0;
    EXEC SQL BEGIN DECLARE SECTION;
        T_TERMINAL_OPER tTerminalOper;
    
        char  szMsShopNo[15+1];
        char  szMsOperNo[4+1];
        char  szMsDelOperNo[4+1];
        char  szMsPosNo[15+1];
    EXEC SQL END DECLARE SECTION;
    
    memset( &tTerminalOper, 0, sizeof( T_TERMINAL_OPER) );
    memset(    szMsShopNo, 0, sizeof(szMsShopNo) );
    memset(    szMsOperNo, 0, sizeof( szMsOperNo) );
    memset(    szMsDelOperNo, 0, sizeof( szMsDelOperNo) );
    memset( szMsPosNo, 0, sizeof(szMsPosNo) );

    strcpy(    szMsShopNo,  ptAppStru->szShopNo);
    strcpy( szMsPosNo, ptAppStru->szPosNo );
    memcpy( szMsDelOperNo, ptAppStru->szFinancialCode , 4);
    

    WriteLog(TRACE,"删除操作员交易开始 shop[%s] del_operno[%s]……",szMsShopNo, szMsDelOperNo);
    
    BeginTran();
    
    if( strcmp(szMsDelOperNo, "0001") == 0 )
    {
        strcpy(ptAppStru->szRetCode, ERR_DEL_ADMIN_ERROR);
        WriteLog(ERROR,"商户[%s]终端[%s]管理员不允许删除",szMsShopNo,szMsPosNo);
        return FAIL;
    }
    
    /*取出终端登录的操作员*/
    iRet = GetEposLoginOperNo( ptAppStru, szMsOperNo );
    if( iRet != SUCC )
    {
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        WriteLog(ERROR," 当前终端无操作员登录，请登录");
        return FAIL;
    }
    
    if( strcmp( szMsOperNo, "0001" ) != 0 )
    {
        strcpy( ptAppStru->szRetCode, ERR_EPOS_OPT_INVALID );
        WriteLog(ERROR,"商户[%s]终端[%s]当前操作员[%s]无此权限",szMsPosNo,szMsOperNo);
        return FAIL;
    }
            
    WriteLog(TRACE,"删除操作员交易…… shop[%s] pos[%s]adminno[%s]……",szMsShopNo,szMsPosNo,szMsOperNo);
    
    /*删除操作员*/
    EXEC SQL UPDATE terminal_oper t
                set 
                    t.del_flag =1,
                    t.login_status =0
                WHERE
                    t.shop_no =:szMsShopNo
                and t.pos_no =:szMsPosNo
                and t.oper_no =:szMsDelOperNo;
    if( SQLCODE && ( SQLCODE != SQL_NO_RECORD ) )
    {
        RollbackTran();
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        return FAIL;
    }
    
    CommitTran();
    
    WriteLog(TRACE,"删除操作员交易结束 shop[%s] del_operno[%s]……",szMsShopNo,szMsDelOperNo);
    
    strcpy(ptAppStru->szRetCode, TRANS_SUCC);
    
    return SUCC;
    
}

/*****************************************************************
** 功    能:修改操作员密码
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
int EposOperUpdatePwd( ptAppStru )
T_App *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char szMsOldPwd[6+1];
        char szMsNewPwd[7+1];
        char szMsShopNo[15+1];
        char szMsPosNo[15+1];
        char szMsOperNo[4+1];
    EXEC SQL END DECLARE SECTION;
    
    memset( szMsOldPwd, 0, sizeof(szMsOldPwd) );
    memset( szMsNewPwd, 0, sizeof(szMsNewPwd) );
    memset( szMsShopNo, 0, sizeof(szMsShopNo) );
    memset( szMsPosNo, 0, sizeof(szMsPosNo) );
    memset( szMsOperNo, 0, sizeof(szMsOperNo) );


    
    /*密码长度校验*/
    if(  strlen(ptAppStru->szFinancialCode)!= 6 
      || strlen(ptAppStru->szBusinessCode)!= 6  )
     {
         strcpy(ptAppStru->szRetCode, ERR_EPOS_OPERPWD_ERROR);
         WriteLog(ERROR,"密码长度不合法");
        return FAIL;
    }
    
    BeginTran();
    
    /*取出当前登录EPOS的操作员号*/
    GetEposLoginOperNo( ptAppStru,szMsOperNo );

    WriteLog(TRACE,"修改操作员密码交易开始 shop[%s] operno[%s]……",szMsShopNo,szMsOperNo);            
    
    memcpy(szMsOldPwd, ptAppStru->szFinancialCode, 6);
    memcpy(szMsNewPwd, ptAppStru->szBusinessCode, 6);
    memcpy(szMsShopNo, ptAppStru->szShopNo, 15);
    memcpy(szMsPosNo, ptAppStru->szPosNo,15);
    
    EXEC SQL UPDATE terminal_oper t 
                set 
                    t.oper_pwd =:szMsNewPwd
                WHERE
                     t.shop_no =:szMsShopNo 
                 and t.pos_no =:szMsPosNo 
                 and t.oper_pwd =:szMsOldPwd
                 and t.oper_no = :szMsOperNo
                 and t.del_flag =0 
                 and t.login_status =1;
    if( SQLCODE )
    {
        RollbackTran();
        strcpy( ptAppStru->szRetCode, ERR_EPOS_OPERPWD_ERROR );
        WriteLog( ERROR,"商户[%s]操作员[%s]输入密码错误",szMsShopNo,szMsPosNo );
        return FAIL;
    }
    
    WriteLog(TRACE,"修改操作员密码交易完成 shop[%s] pos[%s] operno[%s]……", \
                szMsShopNo,szMsPosNo,szMsOperNo );    
    CommitTran();
    
    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    
    return SUCC;
}


/*****************************************************************
** 功    能:修改表terminal_oper中pos_no为ptAppStru->szPosNo的记录，将该记录中的操作员状态标为签退。
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
int OtherOperLoginOut( ptAppStru )
T_App *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char szMsPosNo[15+1];
        char szMsShopNo[15+1];
    EXEC SQL END DECLARE SECTION;


    memset( szMsPosNo, 0, sizeof(szMsPosNo)    );
    memset( szMsShopNo, 0, sizeof(szMsShopNo) );

    strcpy( szMsShopNo,  ptAppStru->szShopNo);
    strcpy( szMsPosNo,   ptAppStru->szPosNo);

    EXEC SQL UPDATE terminal_oper t
        set
            t.login_status=0 
        WHERE
            t.shop_no =:szMsShopNo
          and t.pos_no=:szMsPosNo ;
    if( SQLCODE == SQL_NO_RECORD )
    {
             return SQL_NO_RECORD;
    }
    if( SQLCODE )
    {
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        return FAIL;
    }

    return SUCC;

}

/*****************************************************************
** 功    能:在termianl_oper表中，修改shop_no和szOperNo为当前和EOPS编号的记录，将记录中pos_no置为ptAppStru->szPosNo,并标记login_status为登陆
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
int UpdateOperStatusLogin(ptAppStru)
T_App *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char szMsShopNo[15+1];
        char szMsPosNo[15+1];
        char szMsOperNo[4+1];
    EXEC SQL END DECLARE SECTION;

    memset( szMsShopNo, 0, sizeof(szMsShopNo) );
    memset( szMsOperNo, 0, sizeof(szMsOperNo) );
    memset( szMsPosNo, 0, sizeof(szMsPosNo)    );

    strcpy( szMsShopNo, ptAppStru->szShopNo );
    memcpy( szMsOperNo, ptAppStru->szFinancialCode , 4 );
    strcpy( szMsPosNo, ptAppStru->szPosNo );

    EXEC SQL UPDATE    terminal_oper  t
                set
                    t.login_status=1
                WHERE
                    t.shop_no=:szMsShopNo
                    and t.pos_no =:szMsPosNo
                    and t.oper_no=:szMsOperNo 
                    and t.del_flag =0 ;
    if( SQLCODE )
    {
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        WriteLog(ERROR,"修改terminal_oper错误,SQLCODE[%d]",SQLCODE);
        return FAIL;
    }
    return SUCC;
}


/*****************************************************************
** 功    能:取出终端当前登录的操作员号
** 输入参数:
           ptAppStru
** 输出参数:
           szOperNo 操作员号
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
*****************************************************************/
int GetEposLoginOperNo(ptAppStru,szOperNo)
T_App *ptAppStru;
char *szOperNo ;
{
    int iRet = 0;
    EXEC SQL BEGIN DECLARE SECTION;
        char szMsOperNo[4+1];
        char szMsShopNo[15+1];
        char szMsPosNo[15+1];
    EXEC SQL END DECLARE SECTION;
    
    memset(    szMsShopNo, 0, sizeof(szMsShopNo) );
    memset(    szMsOperNo, 0, sizeof(szMsOperNo) );
    memset(    szMsPosNo, 0, sizeof(szMsPosNo)    );

    strcpy(    szMsShopNo, ptAppStru->szShopNo );
    strcpy(    szMsPosNo, ptAppStru->szPosNo );
    
    EXEC SQL SELECT oper_no 
    INTO :szMsOperNo
    FROM terminal_oper t
    WHERE t.shop_no=:szMsShopNo
        and t.pos_no=:szMsPosNo
        and t.del_flag=0
        and t.login_status=1;
    if( SQLCODE )
    {
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        return FAIL;
    }
    
    strcpy( szOperNo, szMsOperNo );
    
    return SUCC;
}

/*****************************************************************
** 功    能:向terminal_oper表中增加一条记录
** 输入参数:
       oper_name  操作员姓名
       oper_pwd   操作员密码
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
int Insert2TerminalOper( ptAppStru, szOperName, szOperPwd)
T_App *ptAppStru;
char *szOperName;
char *szOperPwd;
{
    int iRet = 0;
    EXEC SQL BEGIN DECLARE SECTION;
        char szMsShopNo[15+1];
        char szMsOperNo[4+1];
        char szMsOperPwd[6+1];
        char szMsOperName[20+1];
        char szMsPosNo[15+1];
        
    EXEC SQL END DECLARE SECTION;
    T_TERMINAL_OPER tTerminalOper;

    memset( szMsShopNo, 0, sizeof(szMsShopNo) );
    memset( szMsOperNo, 0, sizeof(szMsOperNo) );
    memset( szMsOperPwd, 0, sizeof(szMsOperPwd) );
    memset( szMsOperName, 0, sizeof(szMsOperName));
    memset( szMsPosNo, 0, sizeof(szMsPosNo) );
    
    strcpy( szMsShopNo, ptAppStru->szShopNo );
    memcpy( szMsOperNo, ptAppStru->szFinancialCode, 4 );
    strcpy( szMsPosNo, ptAppStru->szPosNo );
    strcpy( szMsOperPwd, szOperPwd);
    strcpy( szMsOperName, szOperName);
    
    /*取出终端操作员表中的对应信息，并根据不同情况加以判断*/
    iRet = GetOperInfo(ptAppStru, &tTerminalOper);    
    if( iRet == SQL_NO_RECORD )    
    {    
        EXEC SQL insert INTO 
                terminal_oper 
                    t(shop_no,pos_no,oper_no,oper_pwd,oper_name,del_flag,login_status)
                values
                    (:szMsShopNo,:szMsPosNo,:szMsOperNo,:szMsOperPwd,:szMsOperName,0,0) ;    
        if( SQLCODE )
        {    
            strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
            WriteLog(ERROR,"商户[%s]终端[%s]增加操作员[%s]失败", \
                    szMsShopNo,szMsPosNo,szMsOperNo);
            return FAIL;
        }
    }
    else if( iRet == SUCC && tTerminalOper.iDelFlag)
    {
        /*记录存在，但del_flag状态为删除：1*/
        EXEC SQL UPDATE 
                        terminal_oper t
                    set 
                        t.del_flag = 0 ,
                        t.login_status = 0,
                        t.oper_name = :szMsOperName,
                        t.oper_pwd = :szMsOperPwd
                    WHERE
                         t.pos_no =:szMsPosNo
                     and t.shop_no =:szMsShopNo
                     and t.oper_no =:szMsOperNo ;
        if( SQLCODE )
        {
            strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
            WriteLog(ERROR,"商户[%s]终端[%s]增加操作员[%s]失败", \
                    szMsShopNo,szMsPosNo,szMsOperNo);
            return FAIL;
        }
    }
    else if( iRet == SUCC && !tTerminalOper.iDelFlag)    
    {
        /*记录存在，且del_flag状态为未!删除：0*/
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        WriteLog(ERROR,"商户[%s]终端[%s]操作员[%s]已存在", \
                    szMsShopNo,szMsPosNo,szMsOperNo);
        return FAIL;
    }    
    else if( iRet == FAIL )
    {
        strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
        WriteLog(ERROR,"商户[%s]终端[%s]增加操作员[%s]失败", \
                    szMsShopNo,szMsPosNo,szMsOperNo);
        return FAIL;
    }    
    
    return SUCC ;

}

/*****************************************************************
** 功    能:给ptAppStru结构体赋值终端操作员。
            交易规则：
            如果是EPOS_OPER_LOGIN交易，直接设置ptAppStru->OperNo = AppStru->user_code1
            否则，从terminal_oper表中取出szOperNo并赋值给ptAppStru->OperNo，
            若无则设置ptAppStru->szOperNo = "0001"
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
*****************************************************************/
int SetOper2ptAppStru( ptAppStru )
T_App *ptAppStru ;
{
        int iRet = 0;
        EXEC SQL BEGIN DECLARE SECTION;
                char szOperNo[4+1];
        EXEC SQL END DECLARE SECTION;

        memset( szOperNo, 0, sizeof( szOperNo) );

        if( 5 == ptAppStru->iTransType
            && ( memcmp(ptAppStru->szTransCode,"10000005", 8) == 0 )
           )
        {
                memcpy( ptAppStru->szOperNo, ptAppStru->szFinancialCode, 4 );
                return SUCC;
        }

        iRet = GetEposLoginOperNo( ptAppStru, szOperNo );
        if( iRet == SUCC )
        {
                strcpy( ptAppStru->szOperNo, szOperNo );
                return SUCC;
        }
        else
        {
                memcpy( ptAppStru->szOperNo, "0001", 4 );
                return SUCC;
        }

        return SUCC ;

}

/*****************************************************************
** 功    能:判断操作员的密码是否已修改初始密码 ：
** 输入参数:
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
int IsBeginPwd( ptAppStru )
T_App *ptAppStru ;
{
    T_TERMINAL_OPER tTerminalOper;
    
    if( GetLoginOperInfo( ptAppStru, &tTerminalOper) == SUCC )
    {
        if( memcmp(tTerminalOper.szOperPwd,"000000",6) == 0 )
        {
            strcpy(ptAppStru->szRetCode, ERR_EPOS_OPERPWD_NOCHG);
            WriteLog(ERROR,"请先修改初始密码，再交易");
            return SUCC ;
        }
        else
        {
            return FAIL;
        }
    }
    else
    {
        WriteLog(ERROR,"取操作员信息失败!" );
        return FAIL;
    }
}

