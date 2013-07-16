
/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.4 $
$Log: DownMsg.ec,v $
Revision 1.4  2013/06/18 06:04:09  fengw

1、数据库表中，短信编号存储方式修改为6位编号 + '.'分隔符，修改相应代码。

Revision 1.3  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.2  2013/01/05 06:41:53  fengw

1、修正SQL语句中字段名错误。

Revision 1.1  2012/12/18 10:04:56  wukj
*** empty log message ***

$Date: 2013/06/18 06:04:09 $
*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

/*****************************************************************
** 功    能:短信通知下发(一次下载一条)
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        短信信息
           ptAppStru->iReservedLen    短信信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownMsg( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[17], szRecNo[61], szMsgRec[61], szMessage[256];
        char    szDate[9];
        int    iTransType, iRecNum, iSmsNo;
    EXEC SQL END DECLARE SECTION;

    int    i, iCurPos, iCmdLen, iCmdNum, iRet;
    int    iPreCmdLen, iPreCmdNum;
    char    szTmpStr[50], szBuf[512], szCmd[512];
    char    szPreCmd[50];

    //中心发起交易，用于判断是否送comweb
    strcpy( ptAppStru->szAuthCode, "YES" );

    ptAppStru->iReservedLen = 0;

    strcpy( szPsamNo, ptAppStru->szPsamNo );

    EXEC SQL SELECT NVL(msg_recnum,0), NVL(msg_recno,' ') 
    INTO :iRecNum, :szRecNo
    FROM terminal
    WHERE psam_no = :szPsamNo;
    if( SQLCODE )
    {
        WriteLog( ERROR, "SELECT term fail %d", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }
    DelTailSpace(szRecNo);

    //终端返回上次下载结果
    if( memcmp( ptAppStru->szTransCode, "00", 2 ) != 0 )
    {
        //下载成功，更新短信下载信息
        if( memcmp( ptAppStru->szHostRetCode, "00", 2 ) == 0 )
        {
            if( iRecNum >= 1 )
            {
                iRecNum --;
                memcpy( szMsgRec, szRecNo+7, iRecNum*7 );
                szMsgRec[iRecNum*7] = 0;

                EXEC SQL UPDATE terminal 
                set msg_recnum = :iRecNum,
                        msg_recno = :szMsgRec
                WHERE psam_no = :szPsamNo;
                if( SQLCODE )
                {
                    strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                    WriteLog( ERROR, "update term fail %d", SQLCODE );
                    RollbackTran();
                    return FAIL;
                }
                CommitTran();
            }
            else
            {    
                WriteLog( ERROR, "msg_recnum=0" );
            }
        }
    }
    else
    {
        memcpy( szMsgRec, szRecNo, iRecNum*7 );
    }

    //终端返回上次交易结果，不用回送终端&&comweb
    if( memcmp( ptAppStru->szTransCode, "FF", 2 ) == 0 )
    {
        strcpy( ptAppStru->szAuthCode, "NO" );
        strcpy( ptAppStru->szRetCode, TRANS_SUCC );
        return SUCC;
    }

    //无短信
    if( iRecNum == 0 )
    {
        strcpy( ptAppStru->szRetCode, ERR_DOWN_FINISH );
        WriteLog( ERROR, "not short_message" );
        return FAIL;
    }

    GetSysDate( szDate );

    for( i=0; i<iRecNum; i++ )
    {
        memcpy( szTmpStr, szMsgRec+i*7, 6 );
        szTmpStr[6] = 0;
        iSmsNo = atol(szTmpStr);

        EXEC SQL SELECT msg_content INTO :szMessage
        FROM short_message
        WHERE rec_no = :iSmsNo and valid_date >= :szDate;
        //对应记录短信已经被删除或已过有效期
        if( SQLCODE == SQL_NO_RECORD )
        {
            WriteLog( TRACE, "sms[%d] has been deleted or invalid", iSmsNo );
            continue;
        }
        else if( SQLCODE )
        {
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            WriteLog( ERROR, "SELECT sms fail %d", SQLCODE );
            return FAIL;
        }
        break;
    }

    //有些短信记录已经被删除或已过期
    if( i > 0 )
    {
        iRecNum -= i;
        memcpy( szRecNo, szMsgRec+i*7, iRecNum*7 );
        szRecNo[iRecNum*7] = 0;

        EXEC SQL UPDATE terminal 
        set msg_recnum = :iRecNum,
            msg_recno = :szRecNo
        WHERE psam_no = :szPsamNo;
        if( SQLCODE )
        {
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            WriteLog( ERROR, "update term fail %d", SQLCODE );
            RollbackTran();
            return FAIL;
        }
        CommitTran();
    }

    //无短信
    if( iRecNum == 0 )
    {
        strcpy( ptAppStru->szRetCode, ERR_DOWN_FINISH );
        WriteLog( ERROR, "not short_message" );
        return FAIL;
    }

    sprintf( ptAppStru->szPan, "记录号%06ld", iSmsNo );
    DelTailSpace( szMessage );
    ptAppStru->iReservedLen = strlen( szMessage );
    memcpy( ptAppStru->szReserved, szMessage, ptAppStru->iReservedLen );

    /* 需要进行后续下载 */
    if( iRecNum > 1 )
    {
        //后续交易代码前2位01表示需要进行后续下载，FF表示不用
        strcpy( ptAppStru->szNextTransCode, "01" );
        memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
        ptAppStru->szNextTransCode[8] = 0;
        
        iPreCmdLen = 5;
        memcpy( szPreCmd, "\x8D\x24\x03\x25\x04", iPreCmdLen );
        iPreCmdNum = 3;

        //后续终端流程代码(指令码集)
        iCmdLen = 0;
        memcpy( ptAppStru->szCommand+iCmdLen, "\xA0", 1 );    //存储短信
        iCmdLen += 1;

        memcpy( ptAppStru->szCommand+iCmdLen, szPreCmd, iPreCmdLen );
        iCmdLen += iPreCmdLen;

        ptAppStru->iCommandNum = iPreCmdNum+1;
        ptAppStru->iCommandLen = iCmdLen;

        //需要进行后续下载，不送comweb
        strcpy( ptAppStru->szAuthCode, "NO" );
    }
    else
    {
        strcpy( ptAppStru->szNextTransCode, "FF" );
        memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
        ptAppStru->szNextTransCode[8] = 0;
    }

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}

