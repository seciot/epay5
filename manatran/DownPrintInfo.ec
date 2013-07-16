
/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.3 $
$Log: DownPrintInfo.ec,v $
Revision 1.3  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.2  2012/12/25 08:31:18  wukj
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
extern int gnPrintNum;

/*****************************************************************
** 功    能:打印信息下载
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        打印信息
           ptAppStru->iReservedLen    打印信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownPrintInfo( ptAppStru, iDownloadNew ) 
T_App    *ptAppStru;
int    iDownloadNew;
{
    EXEC SQL BEGIN DECLARE SECTION;
        
        struct T_PRINT_INFO {
             int     iRecNo;
             char    szInfo[61];
             int     iDataIndex;
             char    szUpdateDate[9];
        }tPrintInfo;
        char    szPsamNo[17], szUpdateDate[9];
        int    iBeginRecNo, iTransType;
        int    iMaxRecNo, iNeedDownMax;
    EXEC SQL END DECLARE SECTION;

    char szBuf[512], szData[512];
    int i, iCurPos, iTotalRecNum, iLastIndex, iCmdLen, iDataLen;
    int iPreCmdLen, iPreCmdNum, iRet;
    char szPreCmd[512], szFlag[2];

    strcpy( szPsamNo, ptAppStru->szPsamNo );

    //中心发起交易，用于判断是否送comweb
    strcpy( ptAppStru->szAuthCode, "YES" );

    //终端返回最后一个包交易结果，不用回送终端&comweb
    if( memcmp( ptAppStru->szTransCode, "FF", 2 ) == 0 )
    {
        strcpy( ptAppStru->szAuthCode, "NO" );
        if( memcmp( ptAppStru->szHostRetCode, "00", 2 ) == 0 )
        {
            EXEC SQL UPDATE terminal
            set down_print = 'N', print_recno = 0
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update down_print fail %d", SQLCODE );
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                RollbackTran();
                return FAIL;
            }
            CommitTran();
        }
        strcpy( ptAppStru->szRetCode, TRANS_SUCC );
        return SUCC;
    }

    //应用下载，更新当前下载步骤
    if( ptAppStru->iTransType == DOWN_ALL_PRINT &&
            memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {
        WriteLog( TRACE, "begin down %s", ptAppStru->szTransName );
        iTransType = DOWN_ALL_PRINT;

        //有更新结果&trans_code前两位为00，表明打印记录下载刚开始，
        //置起始记录为0
        if( memcmp(ptAppStru->szHostRetCode, "NN", 2) != 0 )
        {
            EXEC SQL UPDATE terminal
            set all_transtype = :iTransType, print_recno = 0,
                down_menu = 'N', menu_recno = 0
            WHERE psam_no = :szPsamNo;
        }
        //无更新结果&trans_code前两位为00，表明是打印记录下载的断点
        //续传，不需要重置起始记录号
        else
        {
            EXEC SQL UPDATE terminal
            set all_transtype = :iTransType
            WHERE psam_no = :szPsamNo;
        }

        if( SQLCODE )
        {
            WriteLog( ERROR, "update all_transtype fail %d", SQLCODE );
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            RollbackTran();
            return FAIL;
        }
        CommitTran();
    }

    ptAppStru->iReservedLen = 0;

    for( i=254; i>=0; i-- )
    {
        if( ptAppStru->szReserved[i] == '1' )
        {
            iNeedDownMax = i+1;
            break;
        }
    }

    //首个下载包，需要根据user_code2判断是断点续传，还是重新下载
    if( memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {
        //断点续传
        if( ptAppStru->szControlCode[1] == '1' )
        {
WriteLog( TRACE, "%s 断点续传", ptAppStru->szTransName );
            EXEC SQL SELECT NVL(print_recno,0) INTO :iBeginRecNo
            FROM terminal
            WHERE psam_no = :szPsamNo;
            if( SQLCODE == SQL_NO_RECORD )
            {
                strcpy( ptAppStru->szRetCode, ERR_INVALID_TERM );
                WriteLog( ERROR, "term[%s] not exist", szPsamNo );
                return FAIL;
            }
            else if( SQLCODE )
            {
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                WriteLog( ERROR, "sel print_recno fail %d", SQLCODE );
                return FAIL;
            }
        }
        /* 重新下载 */
        else
        {
            iBeginRecNo = 0;
        }
    }
    else
    {
        AscToBcd( (uchar*)(ptAppStru->szTransCode), 2, 0 , (uchar*)szBuf);
        szBuf[1] = 0;
        iBeginRecNo = (uchar)szBuf[0];

        //终端更新成功，更改已下载记录号
        if( memcmp(ptAppStru->szHostRetCode, "00", 2) == 0 )
        {
            EXEC SQL UPDATE terminal 
            set print_recno = :iBeginRecNo
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update print_recno fail %d", SQLCODE );
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                RollbackTran();
                return FAIL;
                
            }
            CommitTran();
        }
        else
        {
            WriteLog( ERROR, "终端[%s]更新失败[%s]", szPsamNo, ptAppStru->szHostRetCode );
            sprintf( ptAppStru->szPan, "终端更新结果" );
            return FAIL;
        }
    }

    /* 终端选择了下载方式，以终端选择为准，否则以平台设置为准 */
    if( ptAppStru->szControlCode[0] == '1' )
    {
        iDownloadNew = 1;
    }
    else if( ptAppStru->szControlCode[0] == '0' )
    {
        iDownloadNew = 0;
    }

    /* 中心主动发起或自动发起的下载，由位图控制下载的参数，采用完全下载 */
    if( ptAppStru->iTransType == CENDOWN_PRINT_INFO || ptAppStru->iTransType == AUTODOWN_PRINT_INFO )
    {
        iDownloadNew = 0;
    }

    //增量下载
    if( iDownloadNew == 1 )
    {
        /* 只需要下载应用版本号之后的数据 */
        BcdToAsc( (uchar *)(ptAppStru->szAppVer), 8, 0 ,(uchar *)szUpdateDate);
        szUpdateDate[8] = 0;
    }
    //完全下载
    else
    {
        strcpy( szUpdateDate, "20000101" );
    }

    /*取大记录号，用于判断是否需要通知
      终端进行后续下载，即决定终端的下一步操作*/
    EXEC SQL SELECT max(rec_no) INTO :iMaxRecNo
    FROM print_info;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "count print_cur fail %d", SQLCODE );
        return FAIL;
    }

    if( iMaxRecNo < iNeedDownMax )
    {
        iNeedDownMax = iMaxRecNo;
    }

    EXEC SQL DECLARE print_cur cursor for
    SELECT
        REC_NO,
        NVL(INFO, ' '),
        NVL(DATA_INDEX, 1),
        NVL(UPDATE_DATE,'20080101')        
    FROM print_info
    WHERE rec_no > :iBeginRecNo and
         (update_date >= :szUpdateDate or rec_no = :iNeedDownMax)
    ORDER BY rec_no;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "delare print_cur fail %d", SQLCODE );
        return FAIL;
    }

    EXEC SQL OPEN print_cur;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "open print_cur fail %d", SQLCODE );
        EXEC SQL CLOSE print_cur;
        return FAIL;
    }

    iTotalRecNum = 0;
    iDataLen = 0;
    while(1)
    {
        EXEC SQL FETCH print_cur 
        INTO 
        :tPrintInfo.iRecNo,
        :tPrintInfo.szInfo,
        :tPrintInfo.iDataIndex,
        :tPrintInfo.szUpdateDate;
        if( SQLCODE == SQL_NO_RECORD )
        {
            EXEC SQL CLOSE print_cur;
            break;
        }
        else if( SQLCODE )
        {
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            WriteLog( ERROR, "fetch print_cur fail %d", SQLCODE );
            EXEC SQL CLOSE print_cur;
            return FAIL;
        }

        /* 达到每包下载条数上限 */
        if( iTotalRecNum >= gnPrintNum )
        {
            EXEC SQL CLOSE print_cur;
            break;
        }

        //后续记录不用下载
        if( tPrintInfo.iRecNo > iNeedDownMax )
        {
            EXEC SQL CLOSE print_cur;
            break;
        }

        //该条记录不需要下载
        if( ptAppStru->szReserved[tPrintInfo.iRecNo-1] == '0' && tPrintInfo.iRecNo != iNeedDownMax )
        {
            continue;
        }

        iCurPos = 0;
        DelTailSpace( tPrintInfo.szInfo );
        szBuf[iCurPos] = tPrintInfo.iRecNo;
        iCurPos ++;

        /* 换行符 */
        if( tPrintInfo.iRecNo == PRINT_ENTER )
        {
            szBuf[iCurPos] = 1;
            iCurPos ++;
            memcpy( szBuf+iCurPos, "\x0A", 1 );
            iCurPos += 1;
        }
        /* 空白 */
        else if( tPrintInfo.iRecNo == PRINT_BLANK )
        {
            szBuf[iCurPos] = 0;
            iCurPos ++;
        }
        else
        {
            szBuf[iCurPos] = strlen(tPrintInfo.szInfo);
            iCurPos ++;
            memcpy( szBuf+iCurPos, tPrintInfo.szInfo, strlen(tPrintInfo.szInfo) );
            iCurPos += strlen(tPrintInfo.szInfo);
        }

        if( (iDataLen+iCurPos+1) <= 255 )
        {
            memcpy( szData+iDataLen, szBuf, iCurPos );
            iDataLen += iCurPos;
            iLastIndex = tPrintInfo.iRecNo;
            iTotalRecNum ++;
        }
        else
        {
            EXEC SQL CLOSE print_cur;
            break;
        }
    }

    ptAppStru->iReservedLen = iDataLen+1;    
    ptAppStru->szReserved[0] = iTotalRecNum;
    memcpy( ptAppStru->szReserved+1, szData, iDataLen );
    sprintf( ptAppStru->szPan, "下载%03d-%03d / %03d", iBeginRecNo+1, iLastIndex, iNeedDownMax );

    /* 需要进行后续下载 */
    if( iLastIndex < iNeedDownMax )
    {
        //后续交易代码前2位表示已下载记录最大记录号，后6位为当前交易
        //代码后6位
        szBuf[0] = iLastIndex;
        szBuf[1] = 0;
        BcdToAsc( (uchar*)szBuf, 2, 0 ,(uchar*)(ptAppStru->szNextTransCode));
        memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
        ptAppStru->szNextTransCode[8] = 0;
        
        iCmdLen = 0;
        ptAppStru->iCommandNum = 0;
        if( memcmp(ptAppStru->szTransCode, "00", 2) == 0 )
        {
            //临时提示信息
            memcpy( ptAppStru->szCommand+iCmdLen, "\xAF", 1 );    
            iCmdLen += 1;
            ptAppStru->iCommandNum ++;
        }
        memcpy( ptAppStru->szCommand+iCmdLen, "\x1C\xFF", 2 );//更新打印记录
        iCmdLen += 2;
        ptAppStru->iCommandNum ++;
        memcpy( ptAppStru->szCommand+iCmdLen, "\x8D", 1 );    //计算MAC
        iCmdLen += 1;
        ptAppStru->iCommandNum ++;
        memcpy( ptAppStru->szCommand+iCmdLen, "\x24\x03", 2 );//发送数据
        iCmdLen += 2;
        ptAppStru->iCommandNum ++;
        memcpy( ptAppStru->szCommand+iCmdLen, "\x25\x04", 2 );//接收数据
        iCmdLen += 2;
        ptAppStru->iCommandNum ++;

        ptAppStru->iCommandLen = iCmdLen;

        //需要进行后续下载，不送comweb
        strcpy( ptAppStru->szAuthCode, "NO" );
    }
    else
    {
        if( ptAppStru->iTransType == DOWN_ALL_PRINT )
        {
            //需要进行后续下载，不送comweb
            strcpy( ptAppStru->szAuthCode, "NO" );
        }
        else
        {
            memcpy( ptAppStru->szNextTransCode, "FF", 2 );
            memcpy( ptAppStru->szNextTransCode+2, 
                ptAppStru->szTransCode+2, 6 );
            ptAppStru->szNextTransCode[8] = 0;
        }
    }

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}

