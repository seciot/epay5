/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：管理类交易主控进程
** 创 建 人：高明鑫
** 创建日期：2012/11/8

$Revision: 1.13 $
$Log: Manatran.ec,v $
Revision 1.13  2013/06/14 06:24:14  fengw

1、修改web交易监控端口变量类型为字符串。

Revision 1.12  2013/03/11 07:06:29  fengw

1、新增EMV公钥下载、EMV参数下载交易。
2、原IC卡测试交易改为串口数据读写测试交易。

Revision 1.11  2013/02/21 06:23:34  fengw

1、修改清除冲正标志指令格式。

Revision 1.10  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.9  2013/01/24 07:32:36  fengw
*** empty log message ***

Revision 1.8  2013/01/21 05:53:31  wukj
新增明细查询函数

Revision 1.7  2012/12/27 07:21:40  fengw

1、修正SQL语句中字段名错误。

Revision 1.6  2012/12/25 07:10:06  fengw

1、修改参数个数判断。

Revision 1.5  2012/12/24 08:34:27  wukj
catch_exit修改为CatchExit

Revision 1.4  2012/12/24 08:32:36  wukj
manatran读消息类型改为从启动参数获取

Revision 1.3  2012/12/24 06:49:37  wukj
删除gnRecTimeOut

Revision 1.2  2012/12/24 06:47:27  fengw

1、修改参数配置文件为Setup.ini。
2、补齐调用ReadConfig函数后判断语句。

Revision 1.1  2012/12/18 10:25:53  wukj
*** empty log message ***

Revision 1.15  2012/12/18 04:29:59  wukj
*** empty log message ***

Revision 1.14  2012/12/18 02:21:06  wukj
*** empty log message ***

Revision 1.13  2012/12/12 01:39:51  wukj
*** empty log message ***

Revision 1.12  2012/12/10 05:32:12  wukj
*** empty log message ***

Revision 1.11  2012/12/06 01:42:57  wukj
*** empty log message ***

Revision 1.10  2012/12/05 06:32:01  wukj
*** empty log message ***

Revision 1.9  2012/12/03 03:25:08  wukj
int类型前缀修改为i

Revision 1.8  2012/11/29 10:09:03  wukj
日志,bcdasc转换等修改

Revision 1.7  2012/11/21 03:28:44  wukj
将全局变量移至manatran.h

Revision 1.6  2012/11/20 07:45:39  wukj
替换\t为空格对齐

Revision 1.5  2012/11/19 01:58:29  wukj
修改app结构变量,编译通过

Revision 1.4  2012/11/16 08:44:33  wukj
删除fee.ec文件以及manatran中的调用,修改Makefile

Revision 1.3  2012/11/16 08:38:12  wukj
修改app结构变量名称

Revision 1.2  2012/11/16 03:25:05  wukj
新增CVS REVSION LOG注释


******************************************************************/

#ifndef _MAIN_
#define _MAIN_
#endif 

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    EXEC SQL INCLUDE "../incl/DbStru.h";
EXEC SQL EnD DECLARE SECTION;
#endif

T_TMS_PARA gtTmsPara;

void    CatchExit (int nie );

void
CatchExit (int nie) 
{
    WriteLog ( TRACE, "manatran exit on SIGUSR1" );
    CloseDB();
    exit ( 0 );
}

main ( argc, argv )
int argc;
char * argv [ ];
{
    int     iRet, i, iHostId;
    T_App   *ptAppStru;
    int     iTransDataIdx;
    char    szReadData[100], szTmpBuf[200];
    long lAccessToProcQue;
    memset(&gtTmsPara, 0, sizeof(T_TMS_PARA));

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }

    /*生成精灵进程, 使进程与终端脱节*/
    /*与终端脱节后,scanf等函数将无法使用*/
    switch ( fork ( ) ) {
    case 0 : 
        break;
    case -1 :
        exit ( -1 );
    default :
        exit ( 0 );        

    }

    setpgrp (); /* Make a back process */
    //取manatran读接入模块消息类型
    if(argc != 2)
    {
        WriteLog(ERROR,"启动参数缺失: manatran MSGTYPE ");
        return (FAIL);
    }
    lAccessToProcQue =  atol(argv[1]);

    if( GetEpayMsgId() != SUCC )
    {
        WriteLog (ERROR, "GetEpayMsgId() Error !");
        return (FAIL);
    }

    if( GetEpayShm( ) != SUCC )
    {
        WriteLog(ERROR, "GetEpayShm fail!");
        return (FAIL);
    }

    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "WEB_IP" );
    iRet = ReadConfig( "Setup.ini", "SECTION_COMMUNICATION", szReadData, szTmpBuf);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        return FAIL;
    }
    strcpy( gszWebIp, szTmpBuf );

    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "WEB_PORT" );
    iRet = ReadConfig( "Setup.ini","SECTION_COMMUNICATION", szReadData, szTmpBuf);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        return FAIL;
    }
    strcpy(gszWebPort, szTmpBuf);

    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "PRINT_NUM" );
    iRet = ReadConfig( "Setup.ini","SECTION_PUBLIC", szReadData, szTmpBuf);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        return FAIL;
    }
    gnPrintNum = atol(szTmpBuf);    

    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "DOWNLOAD_NEW" );
    iRet = ReadConfig( "Setup.ini","SECTION_PUBLIC", szReadData, szTmpBuf);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        return FAIL;
    }
    gnDownloadNew = atoi( szTmpBuf );

    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "TELE_NO_LEN" );
    iRet = ReadConfig( "Setup.ini","SECTION_PUBLIC", szReadData, szTmpBuf);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        return FAIL;
    }
    gnTeleLen = atoi( szTmpBuf );

    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "QUERY_VOID_TRACE" );
    iRet = ReadConfig( "Setup.ini","SECTION_PUBLIC", szReadData, szTmpBuf);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        return FAIL;
    }
    gnQueVoidTrace = atoi( szTmpBuf );
    if( gnQueVoidTrace != YES && gnQueVoidTrace != NO )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]",  szReadData );
        return FAIL;
    }

    memset( szReadData, 0, 80 );
    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "OWN_BANK_ID" );
    if( ReadConfig( "Setup.ini","SECTION_PUBLIC", szReadData, szTmpBuf) != SUCC)
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        exit( 0 );
    }
    strcpy( gszOwnBankId, szTmpBuf );


    memset( szReadData, 0, 80 );
    memset( szTmpBuf, 0, 80 );
    strcpy( szReadData, "HOST_NO" );
    if( ReadConfig( "Setup.ini","SECTION_PUBLIC", szReadData, szTmpBuf) != SUCC)
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", 
            szReadData );
        exit( 0 );
    }
    iHostId = atol(szTmpBuf);

    signal ( SIGUSR1, CatchExit );

    WriteLog( TRACE, "Ready read message, msgtype[%d]\n", lAccessToProcQue);
    while ( 1 ) {
        if(RecvAccessToProcQue( lAccessToProcQue,0,&iTransDataIdx) == FAIL)
        {
            WriteLog ( ERROR, "Read from Proc error" );
            continue;
        }

        ptAppStru = (T_App*)GetAppAddress(iTransDataIdx);
        if( ptAppStru == NULL )
        {
            WriteLog( ERROR, "根据iTransDataIdx获取APP结构地址失败");
            continue;
        }

        switch ( fork () ) {
        case -1 :
            if( OpenDB() )
            {
                WriteLog( ERROR, "database fail" );
                exit( 0 );
            }
            WriteLog ( ERROR, "can not fork" );
            strcpy ( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            SendOut( ptAppStru );
            CloseDB( );
             break;
        case 0 :
            for( i = 0; i < 32; i++ )
            {
                if( i == SIGALRM || i == SIGKILL || 
                    i == SIGUSR1 || i == SIGUSR2 )
                     continue;
                signal( i, SIG_IGN );
            }
            signal ( SIGUSR1, CatchExit );
            if( OpenDB() )
            {
                WriteLog( ERROR, "database fail " );
                exit( 0 );
            }
            process( ptAppStru );
            SendOut( ptAppStru );
            CloseDB( );
            exit ( 0 );
        default :
            for( i = 0; i < 32; i++ )
            {
                if( i == SIGALRM || i == SIGKILL || 
                    i == SIGUSR1 || i == SIGUSR2 )
                     continue;
                signal( i, SIG_IGN );
            }
            signal ( SIGUSR1, CatchExit );
            break;
        }
    }
}

int
GetComwebPid( T_App *ptAppStru, long *lWebcomPid )
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[16], szPosNo[16];
        long    lPid;
    EXEC SQL END DECLARE SECTION;

    *lWebcomPid = -1;

    strcpy( szShopNo, ptAppStru->szShopNo );
    strcpy( szPosNo, ptAppStru->szPosNo );

    EXEC SQL SELECT pid INTO :lPid
    FROM  comweb_pid
    WHERE shop_no = :szShopNo AND pos_no = :szPosNo;
    if( SQLCODE )
    {
        WriteLog( ERROR, "sel comweb_pid fail %d", SQLCODE );
        return FAIL;
    }

    EXEC SQL DELETE FROM comweb_pid
    WHERE shop_no = :szShopNo AND pos_no = :szPosNo;
    if( SQLCODE )
    {
        WriteLog( ERROR, "del comweb_pid fail %d", SQLCODE );
        RollbackTran();
        return FAIL;
    }
    CommitTran();

    *lWebcomPid = lPid;

    return SUCC;
}

int
SendOut ( T_App *ptAppStru)
{
    long    lProcToAccessMsgType, lWebMsgType;
    int    iRet;
    pid_t    lPid;
    if( ptAppStru->iTransType == CENDOWN_FUNCTION_INFO ||
        ptAppStru->iTransType == CENDOWN_OPERATION_INFO ||
        ptAppStru->iTransType == CENDOWN_PRINT_INFO ||
        ptAppStru->iTransType == CENDOWN_TERM_PARA ||
        ptAppStru->iTransType == CENDOWN_PSAM_PARA ||
        ptAppStru->iTransType == CENDOWN_MENU ||
        ptAppStru->iTransType == CENDOWN_ERROR ||
        ptAppStru->iTransType == CENDOWN_PAYLIST ||
        ptAppStru->iTransType == CENDOWN_MSG ||
        ptAppStru->iTransType == CENDOWN_FIRST_PAGE ||
        ptAppStru->iTransType == CENDOWN_ALL_OPERATION )
    {
        //下载完毕，通知comweb已下载
        if( memcmp( ptAppStru->szAuthCode, "YES", 3 ) == 0 )
        {
            iRet = GetComwebPid( ptAppStru, &lWebMsgType );
            if( iRet != SUCC )
            {
                WriteLog( ERROR, "get comweb_pid fail" );
            }
            
            if( lWebMsgType > 0 )
            {
                //send the result data to comweb
                //iRet = SendFromTrans(lWebMsgType, ptAppStru);
                iRet = SendProcToAccessQue( lWebMsgType,ptAppStru->lTransDataIdx);
                if( iRet != SUCC )
                {
                    WriteLog( ERROR, "send to comweb Error" );
                }
            }
        }

        if( memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
        {
            //中心发起的首个交易包，中心处理不成功，没必要
            //发送到终端
            if( memcmp(ptAppStru->szRetCode, "00", 2) != 0 )
            {
                lProcToAccessMsgType = -1;
            }
            else
            {
                lProcToAccessMsgType = ptAppStru->lProcToAccessMsgType;
            }
        }
        else
        {
            lProcToAccessMsgType = ptAppStru->lProcToAccessMsgType;
        }
    }
    else
    {
        lProcToAccessMsgType = (unsigned long)ptAppStru->lProcToAccessMsgType;
    }
    //终端返回上次交易结果，不用回送终端
    if( memcmp( ptAppStru->szTransCode, "FF", 2 ) == 0 )
    {
        lProcToAccessMsgType = -1;
    }
    /* send data to tcpcompos */
    if( lProcToAccessMsgType > 0 )
    {
        //iRet = SendFromTrans( lProcToAccessMsgType, ptAppStru );
        iRet = SendProcToAccessQue( lProcToAccessMsgType,ptAppStru->lTransDataIdx);
        if( iRet == FAIL )
        {
            WriteLog( ERROR, "Send to tcpcompos Error" );
        }
        WriteLog(TRACE,"sendFromTrans succ");
    }

    WebDispMoni( ptAppStru, ptAppStru->szTransName, gszWebIp, gszWebPort );

    return SUCC;
}

int
process( T_App *ptAppStru ) 
{
    int iRet;
    EXEC SQL BEGIN DECLARE SECTION;

        char    szShopNo[16];
        int     iMarketNo;
        char    szShopName[41];
        char    szAcqBank[12];
        char    szContactor[11];
        char    szTelephone[26];
        char    szAddr[48];
        int     iFee;
        char    szFaxNum[26];
        int     iSignFlag;  /*0-签约，1-撤销*/
        char    szSignDate[9];   /*签约日期*/
        char    szUnSignDate[9]; /*撤销日期*/
        char    szDeptNo[15+1]; /*所属机构*/
        char    szCardKind[9+1]; /*卡应用类型*/
    EXEC SQL END DECLARE SECTION;

    strcpy( szShopNo, ptAppStru->szShopNo );
    strcpy( szShopNo, ptAppStru->szShopNo );
    EXEC SQL SELECT 
        SHOP_NO, 
        NVL(market_no, 0),
        NVL(SHOP_NAME,' '),
        NVL(ACQ_BANK,' '),
        NVL(CONTACTOR,' '),
        NVL(TELEPHONE,' '),
        NVL(ADDR,' '),
        NVL(FEE, 0),
        NVL(FAX_NUM,' '),
        NVL(SIGN_FLAG,0),
        NVL(SIGN_DATE,' '),
        NVL(UNSIGN_DATE,' '),
        NVL(DEPT_NO,' ')
    INTO 
        :szShopNo,
        :iMarketNo,
        :szShopName,
        :szAcqBank,
        :szContactor,
        :szTelephone,
        :szAddr,
        :iFee,
        :szFaxNum,
        :szSignDate,
        :szSignDate,
        :szUnSignDate,
        :szDeptNo
    FROM shop
    WHERE shop_no = :szShopNo;

    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptAppStru->szRetCode, ERR_INVALID_MERCHANT );    
        WriteLog( ERROR, "SHOP[%s] not found", szShopNo );
        return ( FAIL );

    }
    else if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );    
        WriteLog( ERROR, "select shop[%s] fail [%d]", szShopNo, SQLCODE );
        return ( FAIL );

    }
    DelTailSpace( szShopName);
    strcpy( ptAppStru->szShopName, szShopName);

    if( ptAppStru->iTransType == DOWN_ALL_OPERATION )
    {
        //断点续传，取上次下载步骤(即上次进行中的下载交易)
        iRet = GetDownAllTransType( ptAppStru );
        if( iRet != SUCC )
        {
            return FAIL;
        }
        WriteLog( TRACE, "GetDownAllTransType %d %s", ptAppStru->iTransType, ptAppStru->szTransName);
    }
    
    switch ( ptAppStru->iTransType ) {
    case ECHO_TEST:
        ProcEchoTest( ptAppStru );
        break;
    case CENDOWN_PAYLIST:
    case AUTODOWN_PAYLIST:
    case DOWN_PAYLIST:
        DownPaylist( ptAppStru );
        break;
    case ADD_OPER:
        iRet = EposOperAdd( ptAppStru );
            break;
    case DEL_OPER:
        iRet = EposOperDel( ptAppStru );
        break;
    case OPER_PWD:
        iRet = EposOperUpdatePwd( ptAppStru );
        break;

    case LOGIN:
        if( memcmp(ptAppStru->szTransCode ,"10000005",8) == 0 )
        {
            iRet = EposOperLogin(ptAppStru);
            if( iRet != SUCC )
            {
                break;
            }
        }
        Login( ptAppStru );
        break;
    case CENDOWN_FIRST_PAGE:
    case DOWN_FIRST_PAGE:
        DownFirstPage( ptAppStru );
        break;
    case CENDOWN_MSG:
    case AUTODOWN_MSG:
    case DOWN_MSG:
        DownMsg( ptAppStru );
        break;
    case SEND_INFO:
        TermSendInfo( ptAppStru );
        break;
    case REGISTER:
        TermTelRegister( ptAppStru, gnTeleLen );
        break;
    case SEND_TRACE:
        SendTrace( ptAppStru );
        break;
    case SEND_ERR_LOG:
        SendErrLog( ptAppStru );
        break;
    case CENDOWN_TERM_PARA:
    case AUTODOWN_TERM_PARA:
    case DOWN_TERM_PARA:
    case DOWN_ALL_TERM:
        DownTermPara( ptAppStru );
        break;
    case CENDOWN_PSAM_PARA:
    case AUTODOWN_PSAM_PARA:
    case DOWN_PSAM_PARA:
    case DOWN_ALL_PSAM:
        DownPsamPara( ptAppStru );
        break;
    case CENDOWN_FUNCTION_INFO:
    case AUTODOWN_FUNCTION_INFO:
    case DOWN_FUNCTION_INFO:
    case DOWN_ALL_FUNCTION:
        DownFunctionInfo( ptAppStru, gnDownloadNew );
        break;
    case AUTODOWN_ALL_OPERATION:
    case CENDOWN_ALL_OPERATION:
    case DOWN_ALL_OPERATION:
    case CENDOWN_OPERATION_INFO:
    case AUTODOWN_OPERATION_INFO:
    case DOWN_OPERATION_INFO:
        DownOperationInfo( ptAppStru, gnDownloadNew );
        break;
    case CENDOWN_PRINT_INFO:
    case AUTODOWN_PRINT_INFO:
    case DOWN_PRINT_INFO:
    case DOWN_ALL_PRINT:
        DownPrintInfo( ptAppStru, gnDownloadNew );
        break;
    case CENDOWN_MENU:
    case AUTODOWN_MENU:
    case DOWN_MENU:
    case DOWN_ALL_MENU:
        DownMenu( ptAppStru );
        break;
    case CENDOWN_ERROR:
    case AUTODOWN_ERROR:
    case DOWN_ERROR:
    case DOWN_ALL_ERROR:
        DownError( ptAppStru, gnDownloadNew );
        break;
    case DYNAMIC_CONTR:
        DynamicControl( ptAppStru );
        break;
    case TEST_NORMAL_INPUT:
        WriteTermData( ptAppStru );
        break;
    case TEST_NORMAL_DISP_INPUT:
    case TEST_TWO_INPUT:
    case TEST_TWO_DISP_INPUT:
        WriteHandInputData( ptAppStru );
        break;
    case TEST_DIG_CHK_INPUT:
    case TEST_DIG_CHK_TWO_INPUT:
        WriteDigCheckData( ptAppStru );
        break;
    case TEST_SEND_CARDNO:
        TestSendCardno( ptAppStru );
        break;
    case TEST_PRINT:
        TestPrint( ptAppStru );
        break;
    case TEST_INQ:
        TestInq( ptAppStru );
        break;
    case TEST_PAY:
        TestPay( ptAppStru );
        break;
    case TEST_DISP_OPER_INFO:
        DispOperationInfo( ptAppStru );
        break;
    case TEST_PAYLIST:
        TestPayList( ptAppStru );
        break;
    case TEST_OTHER:
        /*
        WriteIcTrack( ptAppStru );
        */
        TestSerial(ptAppStru);
        break;
    case REPRINT:
    case QUERY_LAST_DETAIL:
        Reprint( ptAppStru );
        break;
    case QUERY_DETAIL_SELF:
    case QUERY_DETAIL_OTHER:
        QueryDetail(ptAppStru);
        break;
    case QUERY_TOTAL:
        QueryTotal(ptAppStru);
        break;
    case QUERY_TODAY_DETAIL:
        QueryTodayDetail(ptAppStru);
        break;
    case GET_DYMENU_66:
    case GET_DYMENU_67:
    case GET_DYMENU_68:
    case GET_DYMENU_69:
        GetDynamicMenu( ptAppStru );
        break;
    /*add by gaomx 20120426增加TMS通知参数下载*/
    case DOWN_TMS:
        DownTmsDataUp( ptAppStru );
    WriteLog(TRACE, "tms通知数据包==%s", ptAppStru->szTmsData );
        break;
    /*add end*/
    /*add by gaomx 20120926 增加静态菜单的下载*/
    case DOWN_STATIC_MENU:
        DownStaticMenu( ptAppStru, gnDownloadNew );
        break;
    /*add end*/

    case ADD_CUSTOMER:
        AddCustomer(ptAppStru);
        break;
    case DEL_CUSTOMER:
        DelCustomer(ptAppStru);
        break;
    case DOWN_EMV_KEY:
        DownEmvKey(ptAppStru);
        break;
    case DOWN_EMV_PARA:
        DownEmvPara(ptAppStru);
        break;
    default :
        WriteLog( ERROR, "invalid trans %d", ptAppStru->iTransType );
        strcpy( ptAppStru->szRetCode, ERR_INVALID_TRANS );
        break;
    }
    if( strlen(ptAppStru->szHostDate) == 0 )
    {
        GetSysDate( ptAppStru->szHostDate );
    }
    GetSysTime( ptAppStru->szHostTime );
    /*add by gaomx 20120425 最后一个更新包，增加清冲正标识*/
    if(memcmp(ptAppStru->szNextTransCode, "FF", 2) == 0)
    {
        memcpy(ptAppStru->szCommand+ptAppStru->iCommandLen, "\xFC\x00\x00", 3);
        ptAppStru->iCommandLen += 3;
        ptAppStru->iCommandNum += 1;
    }
    /*add end*/

    WriteLog(TRACE,"manatran return succ");
    return( SUCC );
}
