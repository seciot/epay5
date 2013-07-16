/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.18 $
$Log: download.ec,v $
Revision 1.18  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.17  2012/12/21 01:38:58  wukj
*** empty log message ***

Revision 1.16  2012/12/18 10:04:56  wukj
*** empty log message ***

*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

/*****************************************************************
** 功    能: 断点续传，取上次下载步骤(即上次进行中的下载交易)
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
int GetDownAllTransType( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[17];
        int    iTransType;
    EXEC SQL END DECLARE SECTION;

    int    iRet;

    strcpy( szPsamNo, ptAppStru->szPsamNo );
    //首个下载包，且要求断点续传，取上次下载步骤(即上次进行的下载交易)
    if( memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 &&
        ptAppStru->szControlCode[1] == '1' )
    {
WriteLog( TRACE, "%s 断点续传", ptAppStru->szTransName );
        EXEC SQL SELECT NVL(all_transtype,0) INTO :iTransType
        FROM terminal
        WHERE psam_no = :szPsamNo;
        if( SQLCODE )
        {
            WriteLog( ERROR, "get all_transtype fail %d", SQLCODE );
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            return FAIL;
        }

        //根据nTransType获取交易信息
        iRet = GetTranInfo( ptAppStru );
        if( iRet != SUCC )
        {
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            WriteLog( ERROR, "get trans_def fail" );
            return FAIL;
        }    

        memset( ptAppStru->szReserved, '1', 255 );
    }
        
    return SUCC;
}

