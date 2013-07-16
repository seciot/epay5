/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.3 $
$Log: DownFirstPage.ec,v $
Revision 1.3  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.2  2012/12/20 06:43:05  wukj
*** empty log message ***

Revision 1.1  2012/12/18 10:04:56  wukj
*** empty log message ***

*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

/*****************************************************************
** 功    能: 首页信息下载
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        首页信息
           ptAppStru->iReservedLen    首页信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownFirstPage( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[17], szMessage[256];
        char    szDate[9];
        int    iFirstPage;
    EXEC SQL END DECLARE SECTION;

    strcpy( szPsamNo, ptAppStru->szPsamNo );

    //中心发起交易，用于判断是否送comweb
    strcpy( ptAppStru->szAuthCode, "YES" );

    ptAppStru->iReservedLen = 0;

    //终端返回上次下载结果，不用回送终端&comweb
    if( memcmp( ptAppStru->szTransCode, "FF", 2 ) == 0 )
    {
        strcpy( ptAppStru->szAuthCode, "NO" );

        //下载成功，更新首页信息记录为0
        if( memcmp( ptAppStru->szHostRetCode, "00", 2 ) == 0 )
        {
            EXEC SQL UPDATE terminal 
            set first_page = 0
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                strcpy(ptAppStru->szRetCode, ERR_SYSTEM_ERROR);
                WriteLog(ERROR, "update term fail %d", SQLCODE );
                RollbackTran();
                return FAIL;
            }
            CommitTran();
        }

        strcpy( ptAppStru->szRetCode, TRANS_SUCC );
        return SUCC;
    }

    EXEC SQL SELECT NVL(first_page,0)
    INTO :iFirstPage
    FROM terminal
    WHERE psam_no = :szPsamNo;
    if( SQLCODE )
    {
        WriteLog( ERROR, "SELECT term fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    GetSysDate( szDate );

    EXEC SQL SELECT NVL(message,' ') 
    INTO :szMessage
    FROM first_page 
    WHERE recno = :iFirstPage and valid_date >= :szDate;
    //对应首页信息已经被删除或已过有效期
    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptAppStru->szRetCode, ERR_INVALID_FIRST_PAGE );
        WriteLog( ERROR, "first page[%d] has been deleted or invalid", iFirstPage);
        return FAIL;
    }
    else if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "SELECT sms fail %d", SQLCODE );
        return FAIL;
    }
    
    DelTailSpace( szMessage );
    ptAppStru->iReservedLen = strlen( szMessage );
    memcpy( ptAppStru->szReserved, szMessage, ptAppStru->iReservedLen );

    strcpy( ptAppStru->szNextTransCode, "FF" );
    memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
    ptAppStru->szNextTransCode[8] = 0;

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}
