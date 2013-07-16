/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：终端资料表相关操作函数
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.1 $
 * $Log: CheckFirstPage.ec,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

EXEC SQL BEGIN DECLARE SECTION;
        EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;

/*****************************************************************
** 功    能:判断终端是否需要更新首页信息
** 输入参数:
           ptAppStru
** 输出参数:
           iLen          信息长度
           szPagemsg     首页信息
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int CheckFirstPage( ptAppStru, iLen, szPageMsg )
T_App *ptAppStru;
int    *iLen;
char    *szPageMsg;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[17], szMsg[256], szDate[9];
        int    iFirstPage;
    EXEC SQL END DECLARE SECTION;

    *iLen = 0;

    if( ptAppStru->iTransType == CENDOWN_FIRST_PAGE ||
        ptAppStru->iTransType == DOWN_FIRST_PAGE )
    {
        return SUCC;
    }

    /* 无效终端，不用下载 */
    if( memcmp( ptAppStru->szRetCode, ERR_INVALID_TERM, 2 ) == 0 ||
        memcmp( ptAppStru->szRetCode, ERR_DUPLICATE_PSAM_NO, 2 ) == 0 ||
        memcmp( ptAppStru->szRetCode, ERR_INVALID_MERCHANT, 2 ) == 0 )
    {
        return SUCC;
    }

    strcpy( szPsamNo, ptAppStru->szPsamNo );

    EXEC SQL SELECT nvl(first_page,0) into :iFirstPage
    from  terminal
    where psam_no = :szPsamNo;
    if( SQLCODE == SQL_NO_RECORD )
    {
        WriteLog( ERROR, "terminal not find psam_no[%s]", szPsamNo );
        return( FAIL );
    }
    else if( SQLCODE )
    {
        WriteLog( ERROR, "select terminal error %d", SQLCODE);
        return( FAIL );
    }

    GetSysDate( szDate );

    //更新首页信息
    if( iFirstPage > 0 )
    {
        EXEC SQL SELECT DESCRIBE 
        INTO :szMsg
        FROM first_page
        WHERE rec_no = :iFirstPage and valid_date >= :szDate;
        if( SQLCODE )
        {
            WriteLog( ERROR, "get first page[%d] fail %ld", iFirstPage, SQLCODE );
        }
        else
        {
            DelTailSpace( szMsg );
            *iLen = strlen(szMsg);
            strcpy( szPageMsg, szMsg );

            InsertUpdateFirstPageCmd( ptAppStru );

            EXEC SQL UPDATE terminal
            set first_page = 0
            where psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update first_page fail[%ld]", SQLCODE );
                RollbackTran();
                return( FAIL );
            }
            CommitTran();
        }
    }

    return( SUCC );
}