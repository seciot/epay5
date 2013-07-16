
/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.2 $
$Log: DownPsamPara.ec,v $
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

/*****************************************************************
** 功    能:PSAM卡参数下载
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        参数信息
           ptAppStru->iReservedLen    参数信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownPsamPara( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        
        struct T_PSAM_PARA {
            int     iModuleId;
            char    szDescribe[21];
            int     iPinKeyIndex;
            int     iMacKeyIndex;
            int     iFskTeleNum;
            char    szFskTeleNo1[16];
            char    szFskTeleNo2[16];
            char    szFskTeleNo3[16];
            int     iFskDownTeleNum;
            char    szFskDownTeleNo1[16];
            char    szFskDownTeleNo2[16];
            char    szFskDOwnTeleNo3[16];
            int     iHdlcTeleNum;
            char    szHdlcTeleNo1[16];
            char    szHdlcTeleNo2[16];
            char    szHdlcTeleNo3[16];
            int     iHdlcDownTeleNum;
            char    szHdlcDownTeleNo1[16];
            char    szHdlcDownTeleNo2[16];
            char    szHdlcDownTeleNo3[16];
            int     iFskBakTeleNum;
            char    szFskBakTeleNo1[16];
            char    szFskBakTeleNo2[16];
            char    szFskBakTeleNo3[16];
            int     iHdlcBakTeleNum;
            char    szHdlcBakTeleNo1[16];
            char    szHdlcBakTeleNo2[16];
            char    szHdlcBakTeleNo3[16];
        }tPsamPara;

        int iModuleId, iTransType;
        char    szPsamNo[17];
    EXEC SQL END DECLARE SECTION;

    char szBuf[512], szCheckVal[9], szKey[9], szRec[20];
    int i, j, iCurPos, iTotalRecNum, iLen, iTmpLen, iLenPos;

    strcpy( szPsamNo, ptAppStru->szPsamNo );

    //中心发起交易，用于判断是否送comweb
    strcpy( ptAppStru->szAuthCode, "YES" );

    //终端返回上次交易结果，不用回送终端&comweb
    if( memcmp( ptAppStru->szTransCode, "FF", 2 ) == 0 )
    {
        strcpy( ptAppStru->szAuthCode, "NO" );
        if( memcmp( ptAppStru->szHostRetCode, "00", 2 ) == 0 )
        {
            EXEC SQL UPDATE terminal
            set down_psam = 'N'
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update down_term fail %d", SQLCODE );
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
    if( ptAppStru->iTransType == DOWN_ALL_PSAM )
    {
        WriteLog( TRACE, "begin down %s", ptAppStru->szTransName );
        iTransType = DOWN_ALL_PSAM;
        EXEC SQL UPDATE terminal
        set all_transtype = :iTransType, down_term = 'N'
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
    
    ptAppStru->iReservedLen = 0;

    iModuleId = ptAppStru->iPsamModule;

    EXEC SQL SELECT 
            MODULE_ID,
              NVL(DESCRIBE,' '),
              NVL(PIN_KEY_INDEX ,0) ,
              NVL(MAC_KEY_INDEX,0),
              NVL(FSK_TELE_NUM,0),
              NVL(FSK_TELE_NO1,' '),
              NVL(FSK_TELE_NO2,' '),
              NVL(FSK_TELE_NO3,' '),
              NVL(FSK_DOWN_TELE_NUM,0),
              NVL(FSK_DOWN_TELE_NO1,' '),
              NVL(FSK_DOWN_TELE_NO2,' '),
              NVL(FSK_DOWN_TELE_NO3,' '),
              NVL(HDLC_TELE_NUM,0),
              NVL(HDLC_TELE_NO1,' '),
              NVL(HDLC_TELE_NO2,' '),
              NVL(HDLC_TELE_NO3,' '),
              NVL(HDLC_DOWN_TELE_NUM,0),
              NVL(HDLC_DOWN_TELE_NO1,' '),
              NVL(HDLC_DOWN_TELE_NO2,' '),
              NVL(HDLC_DOWN_TELE_NO3,' '),
              NVL(FSKBAK_TELE_NUM,0),
              NVL(FSKBAK_TELE_NO1,' '),
              NVL(FSKBAK_TELE_NO2,' '),
              NVL(FSKBAK_TELE_NO3,' '),
              NVL(HDLCBAK_TELE_NUM,0) ,
              NVL(HDLCBAK_TELE_NO1,' ') ,
              NVL(HDLCBAK_TELE_NO2,' '),
              NVL(HDLCBAK_TELE_NO3,' ') 
    INTO 
            :tPsamPara.iModuleId,
            :tPsamPara.szDescribe,
            :tPsamPara.iPinKeyIndex,
            :tPsamPara.iMacKeyIndex,
            :tPsamPara.iFskTeleNum,
            :tPsamPara.szFskTeleNo1,
            :tPsamPara.szFskTeleNo2,
            :tPsamPara.szFskTeleNo3,
            :tPsamPara.iFskDownTeleNum,
            :tPsamPara.szFskDownTeleNo1,
            :tPsamPara.szFskDownTeleNo2,
            :tPsamPara.szFskDOwnTeleNo3,
            :tPsamPara.iHdlcTeleNum,
            :tPsamPara.szHdlcTeleNo1,
            :tPsamPara.szHdlcTeleNo2,
            :tPsamPara.szHdlcTeleNo3,
            :tPsamPara.iHdlcDownTeleNum,
            :tPsamPara.szHdlcDownTeleNo1,
            :tPsamPara.szHdlcDownTeleNo2,
            :tPsamPara.szHdlcDownTeleNo3,
            :tPsamPara.iFskBakTeleNum,
            :tPsamPara.szFskBakTeleNo1,
            :tPsamPara.szFskBakTeleNo2,
            :tPsamPara.szFskBakTeleNo3,
            :tPsamPara.iHdlcBakTeleNum,
            :tPsamPara.szHdlcBakTeleNo1,
            :tPsamPara.szHdlcBakTeleNo2,
            :tPsamPara.szHdlcBakTeleNo3
    FROM PSAM_PARA
    WHERE module_id = :iModuleId;
    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptAppStru->szRetCode, ERR_PSAM_MODULE );
        WriteLog( ERROR, "psam_para [%ld] not exist", iModuleId );
        return FAIL;
    }
    else if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "SELECT psam_para fail %d", SQLCODE );
        return FAIL;
    }

    iCurPos = 0;
    iTotalRecNum = 0;
    strcpy( ptAppStru->szPan, "rec" );    
    for( i=1; i<=7; i++ )
    {
        /* 1-表示要更新 0-表示不要更新 */
        if( ptAppStru->szReserved[i-1] == '0' )
            continue;

        if( i <= 8 )
        {
            sprintf( szRec, " %d", i );
            strcat( ptAppStru->szPan, szRec );
        }
        
        szBuf[iCurPos] = i;    //PSAM卡中记录号
        iCurPos ++;
        switch (i){
        //密钥索引
        case 1:
            szBuf[iCurPos] = 2;    //数据长度
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iPinKeyIndex;
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iMacKeyIndex;
            iCurPos ++;
            iTotalRecNum ++;
            break;
        //FSK系统号码
        case 2:
            iLen = 0;
            iLenPos = iCurPos;    //记录长度所在位置
            szBuf[iCurPos] = iLen;    //数据长度(暂时赋值)
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iFskTeleNum;    //号码个数    
            iCurPos ++;
            iLen ++;
            for( j=1; j<=tPsamPara.iFskTeleNum; j++ )
            {
                switch( j ){
                case 1:
                    DelTailSpace(tPsamPara.szFskTeleNo1);
                    iTmpLen = strlen(tPsamPara.szFskTeleNo1);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskTeleNo1, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 2:
                    DelTailSpace(tPsamPara.szFskTeleNo2);
                    iTmpLen = strlen(tPsamPara.szFskTeleNo2);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskTeleNo2, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 3:
                    DelTailSpace(tPsamPara.szFskTeleNo3);
                    iTmpLen = strlen(tPsamPara.szFskTeleNo3);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskTeleNo3, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                }
            }
            szBuf[iLenPos] = iLen;    //数据长度(最终赋值)
            iTotalRecNum ++;
            break;
        //FSK下载系统号码
        case 3:
            iLen = 0;
            iLenPos = iCurPos;    //记录长度所在位置
            szBuf[iCurPos] = iLen;    //数据长度(暂时赋值)
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iFskDownTeleNum;    //号码个数    
            iCurPos ++;
            iLen ++;
            for( j=1; j<=tPsamPara.iFskDownTeleNum; j++ )
            {
                switch( j ){
                case 1:
                    DelTailSpace(tPsamPara.szFskDownTeleNo1);
                    iTmpLen = strlen(tPsamPara.szFskDownTeleNo1);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskDownTeleNo1, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 2:
                    DelTailSpace(tPsamPara.szFskDownTeleNo2);
                    iTmpLen = strlen(tPsamPara.szFskDownTeleNo2);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskDownTeleNo2, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 3:
                    DelTailSpace(tPsamPara.szFskDOwnTeleNo3);
                    iTmpLen = strlen(tPsamPara.szFskDOwnTeleNo3);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskDOwnTeleNo3, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                }
            }
            szBuf[iLenPos] = iLen;    //数据长度(最终赋值)
            iTotalRecNum ++;
            break;
        //HDLC系统号码
        case 4:
            iLen = 0;
            iLenPos = iCurPos;    //记录长度所在位置
            szBuf[iCurPos] = iLen;    //数据长度(暂时赋值)
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iHdlcTeleNum;    //号码个数    
            iCurPos ++;
            iLen ++;
            for( j=1; j<=tPsamPara.iHdlcTeleNum; j++ )
            {
                switch( j ){
                case 1:
                    DelTailSpace(tPsamPara.szHdlcTeleNo1);
                    iTmpLen = strlen(tPsamPara.szHdlcTeleNo1);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcTeleNo1, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 2:
                    DelTailSpace(tPsamPara.szHdlcTeleNo2);
                    iTmpLen = strlen(tPsamPara.szHdlcTeleNo2);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcTeleNo2, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 3:
                    DelTailSpace(tPsamPara.szHdlcTeleNo3);
                    iTmpLen = strlen(tPsamPara.szHdlcTeleNo3);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcTeleNo3, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                }
            }
            szBuf[iLenPos] = iLen;    //数据长度(最终赋值)
            iTotalRecNum ++;
            break;
        //HDLC下载系统号码
        case 5:
            iLen = 0;
            iLenPos = iCurPos;    //记录长度所在位置
            szBuf[iCurPos] = iLen;    //数据长度(暂时赋值)
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iHdlcDownTeleNum;    //号码个数    
            iCurPos ++;
            iLen ++;
            for( j=1; j<=tPsamPara.iHdlcDownTeleNum; j++ )
            {
                switch( j ){
                case 1:
                    DelTailSpace(tPsamPara.szHdlcDownTeleNo1);
                    iTmpLen = strlen(tPsamPara.szHdlcDownTeleNo1);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcDownTeleNo1, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 2:
                    DelTailSpace(tPsamPara.szHdlcDownTeleNo2);
                    iTmpLen = strlen(tPsamPara.szHdlcDownTeleNo2);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcDownTeleNo2, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 3:
                    DelTailSpace(tPsamPara.szHdlcDownTeleNo3);
                    iTmpLen = strlen(tPsamPara.szHdlcDownTeleNo3);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcDownTeleNo3, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                }
            }
            szBuf[iLenPos] = iLen;    //数据长度(最终赋值)
            iTotalRecNum ++;
            break;
        //FSK备份系统号码
        case 6:
            iLen = 0;
            iLenPos = iCurPos;    //记录长度所在位置
            szBuf[iCurPos] = iLen;    //数据长度(暂时赋值)
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iFskBakTeleNum;    //号码个数    
            iCurPos ++;
            iLen ++;
            for( j=1; j<=tPsamPara.iFskBakTeleNum; j++ )
            {
                switch( j ){
                case 1:
                    DelTailSpace(tPsamPara.szFskBakTeleNo1);
                    iTmpLen = strlen(tPsamPara.szFskBakTeleNo1);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskBakTeleNo1, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 2:
                    DelTailSpace(tPsamPara.szFskBakTeleNo2);
                    iTmpLen = strlen(tPsamPara.szFskBakTeleNo2);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskBakTeleNo2, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 3:
                    DelTailSpace(tPsamPara.szFskBakTeleNo3);
                    iTmpLen = strlen(tPsamPara.szFskBakTeleNo3);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szFskBakTeleNo3, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                }
            }
            szBuf[iLenPos] = iLen;    //数据长度(最终赋值)
            iTotalRecNum ++;
            break;
        //HDLC备份系统号码
        case 7:
            iLen = 0;
            iLenPos = iCurPos;    //记录长度所在位置
            szBuf[iCurPos] = iLen;    //数据长度(暂时赋值)
            iCurPos ++;
            szBuf[iCurPos] = tPsamPara.iHdlcBakTeleNum;    //号码个数    
            iCurPos ++;
            iLen ++;
            for( j=1; j<=tPsamPara.iHdlcBakTeleNum; j++ )
            {
                switch( j ){
                case 1:
                    DelTailSpace(tPsamPara.szHdlcBakTeleNo1);
                    iTmpLen = strlen(tPsamPara.szHdlcBakTeleNo1);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcBakTeleNo1, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 2:
                    DelTailSpace(tPsamPara.szHdlcBakTeleNo2);
                    iTmpLen = strlen(tPsamPara.szHdlcBakTeleNo2);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcBakTeleNo2, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                case 3:
                    DelTailSpace(tPsamPara.szHdlcBakTeleNo3);
                    iTmpLen = strlen(tPsamPara.szHdlcBakTeleNo3);
                    szBuf[iCurPos] = iTmpLen;
                    iCurPos ++;
                    iLen ++;
                    memcpy( szBuf+iCurPos, tPsamPara.szHdlcBakTeleNo3, iTmpLen );
                    iCurPos += iTmpLen;
                    iLen += iTmpLen;
                    break;
                }
            }
            szBuf[iLenPos] = iLen;    //数据长度(最终赋值)
            iTotalRecNum ++;
            break;
        }
    }

    if( (iCurPos+1) > 255 )
    {
        WriteLog( ERROR, "数据太长[%d]", iCurPos+1 );
        strcpy( ptAppStru->szRetCode, ERR_DATA_TOO_LONG );
        return FAIL;
    }

    if( ptAppStru->iTransType == DOWN_ALL_TERM )
    {
        //需要进行后续下载，不送comweb
        strcpy( ptAppStru->szAuthCode, "NO" );
    }
    else
    {
        strcpy( ptAppStru->szNextTransCode, "FF" );
        memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
        ptAppStru->szNextTransCode[8] = 0;
    }

    ptAppStru->iReservedLen = iCurPos+1;    
    ptAppStru->szReserved[0] = iTotalRecNum;    //记录条数
    memcpy( ptAppStru->szReserved+1, szBuf, iCurPos );

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}

