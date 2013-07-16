/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：模拟后台程序
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: simhost.ec,v $
 * Revision 1.1  2012/12/13 01:52:19  linxiang
 * *** empty log message ***
 *
 * ----------------------------------------------------------------
 */
# include "simhost.h" 


void HostProc(T_App *ptApp);

/* ----------------------------------------------------------------
 * 功    能：进程根据收到信号退出运行
 * 输入参数：
 *           iSigNo    收到的信号
 * 输出参数：
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void GracefulExit ( int iSigNo ) 
{
    alarm ( 0 ) ;
    exit (iSigNo);
}

main (int argc, char *argv[])
{
    int iRet, i, iTimeOut, iProcNum, iHostId, iCommNum;

    if( argc != 5 )
    {
        printf("Usage : simhost HostId CommNum SleepTime ProcNum\n");
        WriteLog(ERROR, "Usage : simhost HostId CommNum SleepTime ProcNum");
        return (FAIL);
    }
    iHostId = atoi(argv[1]);
    iCommNum = atoi(argv[2]);
    iTimeOut = atoi(argv[3]);
    iProcNum = atoi(argv[4]);

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
    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    signal ( SIGUSR1, GracefulExit ) ;

    setpgrp (); /* Make a back process */

    /*获取消息队列标识*/
    if( GetEpayMsgId() != SUCC )
    {
        printf( "获取消息队列标识错，可能是没有进行系统初始化\n" );
        WriteLog( ERROR, "can't get msg");
        exit(0);
    }

    if( GetEpayShm() != SUCC )
    {
        printf( "映射共享内存失败，可能是没有进行系统初始化\n" );
        WriteLog( ERROR, "GetSystemShmCtrl fail");
        exit(0);
    }

    iRet = SetHost( iHostId, iCommNum, 'Y', 'Y' );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "SetHost fail");
        exit( 0 );
    }

    WriteLog (TRACE, "simhost ready..." );

    for( i=0; i<iProcNum; i++ )
    {
        switch ( fork ( ) ) {
        case 0 : 
            SingleProcess( iTimeOut, iHostId );
            break;
        case -1 :
            exit ( -1 );
        default :
            break;
        }
    }
}


/* ----------------------------------------------------------------
 * 功    能：模拟后台通讯接口
 * 输入参数：
 *           iTimeout 超时时间
 *           iHostId 后台主机ID
 * 输出参数：
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int SingleProcess( int iTimeOut, int iHostId )
{
    int i, iRet;
    T_App *ptApp;
    int iTdi, iRetTdi;
    int iShmExpired = 30;

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    signal( SIGUSR1, GracefulExit ) ;

    while(1)
    {
        iRet = RecvProcToPresentQue(iHostId, 0, &iTdi);
        ptApp = GetAppAddress(iTdi);
        if( ptApp == NULL )
        {
            WriteLog( ERROR, "GetApp [%d]error", iHostId );
            continue;
        }

        WriteAppStru( ptApp, "simhost read request" );

        if( iTimeOut > 0 && ptApp->iTransType != LOGIN )
        {
//            sleep(nTimeOut);
            usleep(iTimeOut*1000);
        }

        HostProc( ptApp );

        iRetTdi = GetTdiMatch(ptApp->szShopNo, ptApp->szPosNo, ptApp->lSysTrace, iShmExpired,ptApp->iTransType);
        if( iRetTdi == FAIL || iRetTdi != iTdi)
        {
            WriteLog( ERROR, "GetTdiMatch [%d]error", iHostId );
            continue;
        }
                            
        iRet = SendPresentToProcQue(ptApp->lPresentToProcMsgType, iTdi);
        if( iRet != SUCC )
        {
            WriteLog( ERROR, "SendPresentToProcQue[%03ld] error", iHostId );
            continue;
        }
        WriteAppStru( ptApp, "tohost send response" );
    }
}


/* ----------------------------------------------------------------
 * 功    能：生成后台的模拟报文
 * 输入参数：
 *           ptApp 从共享内存读出的请求数据
 * 输出参数：
 *           ptApp 向共享内存写入的响应数据
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void HostProc( T_App *ptApp )
{
    FILE *fp;
    int fd;
    long lAuthCode, lRrn;

    fp = fopen("~/etc/simhost.inf", "r+");
    if(fp == NULL)
    {
        WriteLog(ERROR, "simhost.int not exist");
        return;
    }
    fd = fileno(fp);
    flock(fd, LOCK_EX);
    fscanf(fp, "%d %d", &lAuthCode, &lRrn);
    lAuthCode++;
    lRrn++;
    fprintf(fp, "%d %d", lAuthCode, &lRrn);
    flock(fd, LOCK_UN);
    fclose(fp);
    
    sprintf( ptApp->szRetriRefNum, "%012ld", lRrn );
    sprintf( ptApp->szAuthCode, "%06ld", lAuthCode );

    if( ptApp->iTransType == INQUERY ) //|| ptApp->iTransType == ICBC_INQUERY )
    {
        sprintf( ptApp->szAddiAmount, "D%012ld", 1000000 );
    }
#if 0
    else if( ptApp->iTransType == CHINAUNICOM_INQ ||
        ptApp->iTransType == CHINATELECOM_INQ ||
        ptApp->iTransType == ELECTRICITY_INQ ||
        ptApp->iTransType == CHINAMOBILE_INQ )
    {
        if( memcmp( ptApp->szBusinessCode, "13305919562", 11 ) == 0 )
        {
            sprintf( ptApp->szAmount, "%012ld", 88888 );
        }
        else if( memcmp( ptApp->szBusinessCode, "13305919563", 11 ) == 0 )
        {
            sprintf( ptApp->szAmount, "%012ld", 0 );
        }
        else
        {
            sprintf( ptApp->szAmount, "%012ld", 66666 );
        }
    }
    else if( ptApp->iTransType == CHINAUNICOM_QUERY ||
             ptApp->iTransType == CHINATELECOM_QUERY ||
             ptApp->iTransType == CHINAMOBILE_QUERY )
    {
        if( memcmp( ptApp->szBusinessCode, "13305919562", 11 ) == 0 )
        {
            sprintf( ptApp->szAddiAmount, "D%012ld", 88888 );
        }
        else
        {
            sprintf( ptApp->szAddiAmount, "%012ld", 77777 );
        }
    }
#endif

    GetSysDate( ptApp->szHostDate );
    GetSysTime( ptApp->szHostTime );

#if 0
    if( ( ptApp->iTransType == CHINAUNICOM_INQ || 
          ptApp->iTransType == CHINAMOBILE_INQ ) &&
        memcmp( ptApp->szAmount, "000000000000", 12 ) == 0 )
    {
        strcpy( ptApp->szRetCode, ERR_NOT_PAY );
    }
    else
#endif
    {
        strcpy( ptApp->szRetCode, TRANS_SUCC );
    }

#if 0
    if( ptApp->iTransType == TRAN_OUT_OTHER_CALC_FEE )
    {
        /* 未输入转入行联行行号 */
        if( strlen(ptApp->szFinancialCode) == 0 )
        {
            if( strcmp(ptApp->szAccount2, "4512891711209101" ) == 0 )
            {
                strcpy( ptApp->szFinancialCode, "123456789012" );
                strcpy( ptApp->szHolderName, "李四" );
                strcpy( ptApp->szInBankName, "兴业银行福州分行" );
                strcpy( ptApp->szHostRetCode, "00000" );
            }
            else
            {
                strcpy( ptApp->szRetCode, ERR_NO_OTHER_BANK );
                strcpy( ptApp->szHostRetCode, ERR_ZJYW_NO_OTHER_BANK );
            }
        }
        /* 输入 */
        else
        {
            if( strcmp(ptApp->szBusinessCode, "111111111111" ) == 0 )
            {
                strcpy( ptApp->szHolderName, "李四" );
                strcpy( ptApp->szInBankName, "兴业银行福州分行" );
                strcpy( ptApp->szHostRetCode, "00000" );
            }
            else if( strcmp(ptApp->szBusinessCode, "222222222222" ) == 0 )
            {
                strcpy( ptApp->szInBankName, "兴业银行福州分行" );
                strcpy( ptApp->szHostRetCode, "00000" );
            }
            else
            {
                strcpy( ptApp->szRetCode, ERR_BANK_CODE );
                strcpy( ptApp->szHostRetCode, ERR_ZJYW_BANK_CODE );
            }
        }
    }
    else if( ptApp->iTransType == TRAN_OUT_OTHER )
    {
        WriteLog( TRACE, "收款人:%s", ptApp->szHolderName );
        WriteLog( TRACE, "付款卡:%s", ptApp->szPan );
        WriteLog( TRACE, "收款卡:%s", ptApp->szAccount2 );
        WriteLog( TRACE, "发卡行:%s", ptApp->szOutBankName );
        WriteLog( TRACE, "联行行号:%s", ptApp->szInBankId );
        WriteLog( TRACE, "金额:%s", ptApp->szAmount );
        WriteLog( TRACE, "手续费:%s", ptApp->szAddiAmount );
    }
    else if( ptApp->iTransType == TRAFFIC_AMERCE_INQ ||
             ptApp->iTransType == TRAFFIC_AMERCE_NO_INQ )
    {
        GetSysDate( ptApp->szHostDate );
        strcpy( ptApp->szBusinessCode, "88000001" );
        strcpy( ptApp->szAmount, "000000020000" );
        strcpy( ptApp->szAddiAmount, "000000001000" );
        sprintf( ptApp->szRetriRefNum, "%012ld", lRrn );
        strcpy( ptApp->szHolderName, "张三" );
        strcpy( ptApp->szHostRetCode, "00" );
    }
    else
#endif
    {
        strcpy( ptApp->szHolderName, "张三" );
        strcpy( ptApp->szHostRetCode, "00" );
    }

    ptApp->iReservedLen = 0;

    return;
}
