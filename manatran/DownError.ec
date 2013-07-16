/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.2 $
$Log: DownError.ec,v $
Revision 1.2  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.1  2012/12/18 10:04:56  wukj
*** empty log message ***
 
*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

int DownErrorOld( ptAppStru, iDownloadNew ) 
T_App    *ptAppStru;
int iDownloadNew;
{
    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}

/*****************************************************************
** 功    能: 错误信息下载
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        错误信息
           ptAppStru->iReservedLen    错误信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownError( ptAppStru, iDownloadNew ) 
T_App    *ptAppStru;
int iDownloadNew;
{
    EXEC SQL BEGIN DECLARE SECTION;
        
        struct T_ERROR_INFO {
            int     iErrorIndex;
            char    szOpFlag[2];
            int     iModuleNum;
            char    szInfo1Format[3];
            char    szInfo1[101];
            char    szInfo2Format[3];
            char    szInfo2[101];
            char    szInfo3Format[3];
            char    szInfo3[101];
            char    szUpdateDate[9];
        }tErrorInfo;
        char    szPsamNo[17], szUpdateDate[9];
        int    iBeginRecNo;
        int    iMaxRecNo, iTransType, iNeedDownMax;
    EXEC SQL END DECLARE SECTION;

    char szBuf[512], szData[512];
    int i, iCmdLen, iDataLen, iRet, iLastIndex;
    int iCurPos, iTotalRecNum, iModuNum, iInfo1Len, iInfo2Len, iInfo3Len;
    memset(&tErrorInfo, 0, sizeof(T_ERROR_INFO));
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
            set down_error = 'N', error_recno = 0
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update down_error fail %d", SQLCODE );
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
    if( ( 
          ptAppStru->iTransType == AUTODOWN_ERROR ||
          ptAppStru->iTransType == DOWN_ALL_ERROR ) &&
        memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {
        iTransType = DOWN_ALL_ERROR;

        EXEC SQL UPDATE terminal
        set all_transtype = :iTransType
        WHERE psam_no = :szPsamNo;
        if( SQLCODE )
        {
            WriteLog( ERROR, "update all_transtype fail %d", SQLCODE );
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            RollbackTran();
            return FAIL;
        }
        CommitTran();
    }

    for( i=254; i>=0; i-- )
    {
        if( ptAppStru->szReserved[i] == '1' )
        {
            iNeedDownMax = i+1;
            break;
        }
    }

    //首个下载包，需要根据control_code判断是断点续传，还是重新下载
    if( memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {
        //断点续传
        if( ptAppStru->szControlCode[1] == '1' )
        {
WriteLog( TRACE, "%s 断点续传", ptAppStru->szTransName );
            EXEC SQL SELECT NVL(error_recno,0) INTO :iBeginRecNo
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
                WriteLog( ERROR, "sel operate_recno fail %d", SQLCODE );
                return FAIL;
            }
        }
        /* 重新下载 */
        else
        {
            iBeginRecNo = 0;
        }
        //add by gaomx 20110415 for test
        //nBeginRecNo = 0;
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
            set error_recno = :iBeginRecNo
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update operate_recno fail %d", SQLCODE );
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
        if( ptAppStru->iTransType == CENDOWN_ERROR || ptAppStru->iTransType == AUTODOWN_ERROR )
        {
                iDownloadNew = 0;
        }

    //增量下载add by gaomx 20110415 for test
    //nDownloadNew =0;
    if( iDownloadNew == 1 )
    {
        /* 只需要下载应用版本号之后数据 */
        BcdToAsc( (uchar *)(ptAppStru->szAppVer), 8, 0 ,(uchar *)szUpdateDate);
        szUpdateDate[8] = 0;
    }
    //完全下载
    else
    {
        strcpy( szUpdateDate, "20000101" );
    }

    /*取最大记录号，用于判断是否需要通知
      终端进行后续下载，即决定终端的下一步操作*/
    EXEC SQL SELECT max(error_index) INTO :iMaxRecNo
    FROM error_info;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "count error fail %d", SQLCODE );
        return FAIL;
    }

    if( iMaxRecNo < iNeedDownMax )
    {
        iNeedDownMax = iMaxRecNo;
    }

    ptAppStru->iReservedLen = 0;
    
    EXEC SQL DECLARE err_cur cursor for
    SELECT
        ERROR_INDEX,
        OP_FLAG,
        MODULE_NUM,
        NVL(INFO1_FORMAT,' '),
        NVL(INFO1, ' '),
        NVL(INFO2_FORMAT,' '),
        NVL(INFO2, ' '),
        NVL(INFO3_FORMAT,' '),
        NVL(INFO3, ' '),
        NVL(UPDATE_DATE,' ')
    FROM  error_info
    WHERE error_index > :iBeginRecNo and 
         (update_date >= :szUpdateDate or error_index = :iNeedDownMax)
    order by error_index;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "delare err_cur fail %d", SQLCODE );
        return FAIL;
    }

    EXEC SQL OPEN err_cur;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "open err_cur fail %d", SQLCODE );
        EXEC SQL CLOSE err_cur;
        return FAIL;
    }

    iTotalRecNum = 0;
    iDataLen = 0;
    while(1)
    {
        EXEC SQL FETCH err_cur 
        INTO 
            :tErrorInfo.iErrorIndex,
            :tErrorInfo.szOpFlag,
            :tErrorInfo.iModuleNum,
            :tErrorInfo.szInfo1Format,
            :tErrorInfo.szInfo1,
            :tErrorInfo.szInfo2Format,
            :tErrorInfo.szInfo2,
            :tErrorInfo.szInfo3Format,
            :tErrorInfo.szInfo3,
            :tErrorInfo.szUpdateDate;
        if( SQLCODE == SQL_NO_RECORD )
        {
            EXEC SQL CLOSE err_cur;
            break;
        }
        else if( SQLCODE )
        {
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            WriteLog( ERROR, "fetch err_cur fail %d", SQLCODE );
            EXEC SQL CLOSE err_cur;
            return FAIL;
        }

        //后续记录不用下载
        if( tErrorInfo.iErrorIndex > iNeedDownMax )
        {
            EXEC SQL CLOSE err_cur;
            break;
        }

        //该条记录不需要下载
        if( ptAppStru->szReserved[tErrorInfo.iErrorIndex-1] == '0' && tErrorInfo.iErrorIndex != iNeedDownMax )
        {
            continue;
        }

        DelTailSpace( tErrorInfo.szInfo1 );
        DelTailSpace( tErrorInfo.szInfo2 );
        DelTailSpace( tErrorInfo.szInfo3 );

        iInfo1Len = strlen( tErrorInfo.szInfo1 );
        iInfo2Len = strlen( tErrorInfo.szInfo2 );
        iInfo3Len = strlen( tErrorInfo.szInfo3 );

        iModuNum = 0;
        if( memcmp( tErrorInfo.szInfo1Format, "FF", 2 ) != 0 )
        {
            iModuNum ++;
        }
        if( memcmp( tErrorInfo.szInfo2Format, "FF", 2 ) != 0 )
        {
            iModuNum ++;
        }
        if( memcmp( tErrorInfo.szInfo3Format, "FF", 2 ) != 0 )
        {
            iModuNum ++;
        }

        iCurPos = 0;
        //提示信息索引
        szBuf[iCurPos] = tErrorInfo.iErrorIndex;
        iCurPos ++;
        //信息操作标识
        szBuf[iCurPos] = tErrorInfo.szOpFlag[0];
        iCurPos ++;
        //信息内容长度
        szBuf[iCurPos] = iInfo1Len + iInfo2Len + iInfo3Len + iModuNum*2+1;
        iCurPos ++;
        //模板数
        szBuf[iCurPos] = iModuNum;
        iCurPos ++;
        
        //模板1数据
        if( memcmp( tErrorInfo.szInfo1Format, "FF", 2 ) != 0 )
        {
            //显示格式
            AscToBcd( (uchar*)(tErrorInfo.szInfo1Format), 2, 0 ,(uchar*)szBuf+iCurPos);
            iCurPos ++;
            //内容长度
            szBuf[iCurPos] = iInfo1Len;
            iCurPos ++;

            if( iInfo1Len > 0 )
            {
                memcpy(szBuf+iCurPos, tErrorInfo.szInfo1, iInfo1Len);
                iCurPos += iInfo1Len;
            }
        }

        //模板2数据
        if( memcmp( tErrorInfo.szInfo2Format, "FF", 2 ) != 0 )
        {
            //显示格式
            AscToBcd((uchar*)(tErrorInfo.szInfo2Format), 2, 0, (uchar*)szBuf+iCurPos);
            iCurPos ++;
            //内容长度
            szBuf[iCurPos] = iInfo2Len;
            iCurPos ++;

            if( iInfo2Len > 0 )
            {
                memcpy(szBuf+iCurPos, tErrorInfo.szInfo2, iInfo2Len);
                iCurPos += iInfo2Len;
            }
        }

        //模板3数据
        if( memcmp( tErrorInfo.szInfo3Format, "FF", 2 ) != 0 )
        {
            //显示格式
            AscToBcd( (uchar*)(tErrorInfo.szInfo3Format), 2, 0, (uchar*)szBuf+iCurPos);
            iCurPos ++;
            //内容长度
            szBuf[iCurPos] = iInfo3Len;
            iCurPos ++;

            if( iInfo3Len > 0 )
            {
                memcpy(szBuf+iCurPos, tErrorInfo.szInfo3, iInfo3Len);
                iCurPos += iInfo3Len;
            }
        }

        if( (iDataLen+iCurPos+1) <= 255 )
        {
            memcpy( szData+iDataLen, szBuf, iCurPos );
            iDataLen += iCurPos;
            iLastIndex = tErrorInfo.iErrorIndex;
            iTotalRecNum ++;
        }
        //数据过长，后续记录由下个包下载
        else
        {
            EXEC SQL CLOSE err_cur;
            break;
        }
    }

    ptAppStru->szReserved[0] = iTotalRecNum;
    memcpy( ptAppStru->szReserved+1, szData, iDataLen );
    ptAppStru->iReservedLen = iDataLen+1;    
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
        memcpy( ptAppStru->szCommand+iCmdLen, "\x1D\xFF", 2 );//更新操作提示
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

        //需要后续下载，不送comweb
        strcpy( ptAppStru->szAuthCode, "NO" );
    }
    else
    {
        if( ptAppStru->iTransType == DOWN_ALL_ERROR )
        {
            //需要后续下载，不送comweb
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


