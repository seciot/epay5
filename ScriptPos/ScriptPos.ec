/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：脚本POS 接入模块。作为服务端，接受网控器、FSK接入网关、路
 *           由器等以长连接双工方式接入，实现脚本POS与交易处理系统数据
 *           交换，报文合法性验证，以及脚本POS报文数据与内部公共数据结
 *           构数据的格式互转。
 * 创 建 人：Robin
 * 创建日期：2012/11/19
 *
 * $Revision: 1.6 $
 * $Log: ScriptPos.ec,v $
 * Revision 1.6  2013/06/14 06:33:26  fengw
 *
 * 1、增加SetEnvTransId调用代码。
 *
 * Revision 1.5  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 * Revision 1.4  2012/12/26 01:15:43  wukj
 * 执行SrvAccept失败后将return FAIl修改为continue
 *
 * Revision 1.3  2012/12/21 03:35:33  chenrb
 * *** empty log message ***
 *
 * Revision 1.2  2012/12/17 09:20:19  chenrb
 * *** empty log message ***
 *
 * Revision 1.1  2012/12/12 02:23:18  chenrb
 * 初始版本
 *
 ******************************************************************************/

#include "ScriptPos.h"

/*******************************************************************************
 * 函数功能：进程优雅退出。进程退出前做些善后处理，包括关闭数据库、关闭socket等。
 * 输入参数：
 *           iExitStatus - 进程退出时状态，被exit作为参数调用
 * 输出参数：
 *           无
 * 返 回 值： 
 *           无
 *
 * 作    者：Robin
 * 日    期：2012/11/20
 *
 ******************************************************************************/
void  GracefulExit( iExitStatus )
int iExitStatus;
{
    WriteLog( TRACE, "ScriptPos receive signal USR1 and exit" );
    CloseDB( );
    if( giSock > 0 )
    {
        close( giSock );
        WriteLog( TRACE, "close Sock %ld", giSock );
    }

    exit( iExitStatus );
}

/*******************************************************************************
 * 函数功能：收到用户信号USR2后，做跳转
 *           除TDI_SEM信号量等。
 * 输入参数：
 *           iNoUse - 无用参数
 * 输出参数：
 *           无
 * 返 回 值： 
 *           无
 *
 * 作    者：Robin
 * 日    期：2012/11/20
 *
 ******************************************************************************/
void RecUSR2Proc( int nNoUse )
{
    siglongjmp( env, 1 );
}

/*******************************************************************************
 * 函数功能：监听服务端口，等待客户端发起连接请求。收到连接请求后fork一对服务
 *           进程，父进程继续监听等待其他客户端的连接请求。这一对服务进程：
 *           1、其中一个服务进程守护在socket端口，等待接收终端发起的交易请求，
 *              并进行拆包处理，然后通过ACCESS_TO_PROC消息队列发送给交易处理层
 *              处理。
 *           2、另外一个进程守护在PROC_TO_ACCESS消息队列中，等待接收交易应答，
 *              并进行组包处理，然后通过socket发送给终端。
 * 输入参数：
 *           argv[1]  -  服务端口
 * 输出参数：
 *           无
 * 返 回 值： 
 *           FAIL  -  失败
 *
 * 作    者：Robin
 * 日    期：2012/11/20
 *
 ******************************************************************************/
int main (argc, argv)
int    argc;
char   *argv[ ];
{
    int     i, j, k, iSock, iNewSock, iRet;
    long    lPid, lChildPid, lPrePid;
    char    szSvrPort[5+1];
    char    szIp[16], szTmpStr[200], szSectionName[256], szItemName[100], szTmpBuf[200];
    int     iMaxLinkNum;
    char    szDate[9], szTime[7];

    if( argc < 2 ) 
    {
        WriteLog( ERROR, "Usage : ScriptPos SvrPort" );
        return( FAIL );
	}

    memset(szSvrPort, 0, sizeof(szSvrPort));
    strcpy(szSvrPort, argv[1]);

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }

    /*生成精灵进程, 使进程与终端脱节*/
    /*与终端脱节后,scanf等函数将无法使用*/
    switch ( fork ( ) ) 
    {
    case 0 : 
        break;
    case -1 :
        exit ( -1 );
    default :
        exit ( 0 );        
    }

    /* Make a back process */
    setpgrp (); 

    if( GetEpayMsgId( ) != SUCC ) 
    {
        WriteLog( ERROR, "获取消息队列标识错，可能是没有进行系统初始化" );
        return ( FAIL );
    }

    if( GetEpayShm() != SUCC )
    {
        WriteLog( ERROR, "映射公共数据结构共享内存失败，可能是没有进行系统初始化" );
        return ( FAIL );
    }

    iRet = GetMasterKey( gszAuthKey );
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "read key fail" );
        return ( FAIL );
    }

    /*读取公共系统参数*/
    strcpy( szSectionName, "SECTION_PUBLIC" );
    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "EACH_PACK_MAX_BYTES" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {    
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    giEachPackMaxBytes = atoi( szTmpBuf );

    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "DISP_MODE" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    giDispMode = atol(szTmpBuf);    

    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "ACQUIRER_ID" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    strcpy( gszAcqBankId, szTmpBuf );

    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "HOLDER_NAME_MODE" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    giHolderNameMode = atol(szTmpBuf);    

    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "MAC_CHK" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    giMacChk = atol(szTmpBuf);

    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "TIMEOUT_TDI" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    giTimeoutTdi = atol(szTmpBuf);

    /* 开始读取通讯参数 */
    strcpy( szSectionName, "SECTION_COMMUNICATION" );
    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "WEB_IP" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {    
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    strcpy( gszWebIp, szTmpBuf );

    memset(gszWebPort, 0, sizeof(gszWebPort));
    strcpy( szItemName, "WEB_PORT" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, gszWebPort );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }

    memset( szTmpBuf, 0, 80 );
    strcpy( szItemName, "MAX_LINK_NUM" );
    iRet = ReadConfig( "Setup.ini", szSectionName, szItemName, szTmpBuf );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Setup参数配置错误[%s]", szItemName );
        return ( FAIL );
    }
    iMaxLinkNum = atol(szTmpBuf);    

    signal(SIGUSR1, GracefulExit);

    for( i=0; i<5; i++ )
    {
        iSock = CreateSrvSocket( szSvrPort, SOCK_TCP, 5 );
        if( iSock == FAIL )
        {
            WriteLog( ERROR, "can not open socket[%s] times[%d]!!", szSvrPort, i);
            sleep(30);
        }
        else
        {
            break;
        }
    }
    if( i == 5 )
    {
        WriteLog( ERROR, "can not open socket[%ld]!!", szSvrPort);
        return ( FAIL );
    }
    giSock = iSock;

    while(1)
    {
        WriteLog( TRACE, "tcpcom father[%ld] waiting connect...", getpid() ); 
        iNewSock = SrvAccept( iSock, szIp );
        if( iNewSock < 0 )
        {
            WriteLog( ERROR, "can not open socket!");
            continue;
        }

        /* 检查IP地址链接条数 */
        iRet = GetAccessLinkNum( szIp, szSvrPort );
        /* 超过指定链接条数，认为异常，关闭链接，通知原有处理进程退出 */
        if( iRet > iMaxLinkNum )
        {
            while( 1 )
            {
                lPrePid = GetAccessPid( szIp, szSvrPort );
                if( lPrePid <= 0 )
                {
                    break;
                }
                kill(lPrePid, SIGUSR2);
            }
            WriteLog( ERROR, "接入[IP:%s PORT:%s]连接数异常，请及时处理", szIp, szSvrPort );
            close( iNewSock );
            continue;
        }

        /*生成一个子进程，让子进程进行交易处理，父进程继续守候端口*/
        lPid = fork();
        switch ( lPid )
        {
        case -1:
            WriteLog(ERROR, "fork error");
            GracefulExit (-2);

        case 0: /*子进程进行交易处理*/
            for( i = 0; i < 32; i++ )
            {
                if( i == SIGALRM || i == SIGKILL || 
                    i == SIGUSR1 || i == SIGUSR2 )
                     continue;
                signal( i, SIG_IGN );
            }

            /*关掉继承的sock副本*/
            close( iSock );    

            lChildPid = getpid();

            switch (fork ()) 
            {
            case -1:
                WriteLog(ERROR, "fork error");
                GracefulExit (-2);

            case 0:
                ChildProcess( iNewSock, lChildPid, szIp, szSvrPort );
                break;

            default:

                ParentProcess( iNewSock, lChildPid, szIp, szSvrPort );
                break;
            }
        default: /*父进程继续守候端口，等待连接*/
            close( iNewSock );    /*关掉iNewSock副本*/
            break;
        }
    }
}

/*******************************************************************************
 * 函数功能：守护在socket端口，等待接收终端发起的交易请求，并进行拆包处理，然后
 *           通过ACCESS_TO_PROC消息队列发送给交易处理层处理。
 * 输入参数：
 *           iNewSock   -  sock端口
 *           lParentPid -  接入层进程号，作为交易处理层向接入层发送应答时的消息类型
 *           szIp       -  请求方IP地址
 *           szSrvPort  -  服务端口号
 * 输出参数：
 *           无
 * 返 回 值： 
 *           FAIL  -  失败
 *
 * 作    者：Robin
 * 日    期：2012/11/20
 *
 ******************************************************************************/
int ChildProcess( int iNewSock, long lParentPid, char *szIp, char *szSrvPort )
{
    T_App   *ptApp;
    uchar   szRcvBuf[BUFFSIZE], szSndBuf[BUFFSIZE];
    int     iLen, iRet, i, iLenLen;

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    signal(SIGUSR1, GracefulExit);

    if( OpenDB() )
    {
        WriteLog( ERROR, "database fail" );
        exit( 0 );
    }

    WriteLog( TRACE, "tcpcom child child[%ld] ready read sock", getpid() ); 

    iLenLen = 2;
    while (1) 
    {
        memset( szRcvBuf, 0, BUFFSIZE );
        iLen = ReadSockDelLenField( iNewSock, 0, iLenLen, HEX_DATA, szRcvBuf );
        /*读sock错误，通知父进程退出，自己关闭sock后也退出*/
        if( iLen < 0 )
        {
            WriteLog(ERROR, "ReadSock ERROR");
            /* 睡眠5秒钟，确保父进程调用sigsetjmp成功 */
            sleep(5);
            kill( lParentPid, SIGUSR2 );
            close( iNewSock );
            CloseDB();
            exit( 0 );
        }
        /*读到心跳报文或其他非法报文(例如电话机直接拨号，网控器上送的主被叫号码信息*/
        else if( iLen < 50 )
        {
            continue;
        }

        WriteHdLog( szRcvBuf, iLen, "ScriptPos receive from pos" );
        
        /* 根据报文头获取App结构指针 */
        ptApp = GetTdiAddress(szRcvBuf, iLen);
        if(ptApp == NULL)
        {
            WriteLog(ERROR, "获取App结构指针失败!");
            WriteHdLog(szRcvBuf, iLen, "ScriptPos receive invalid message from pos");
            continue;
        }

        /*将终端报文解析成内部公共数据结构*/
        iRet = UnpackScriptPos(ptApp, szRcvBuf, iLen);
        if(iRet == INVALID_PACK)
        {
            WriteLog(ERROR, "invalid pack or get tdi fail");
            WriteHdLog( szRcvBuf, iLen, "ScriptPos receive invalid message from pos" );
            FreeTdi(ptApp->lTransDataIdx);
            continue;
        }
        else if( iRet != SUCC ) 
        {
            /* 不是终端未登记引起的拆包错误，记录错误包 */
            if( memcmp(ptApp->szRetCode, ERR_INVALID_TERM, 2) != 0 &&
                memcmp(ptApp->szRetCode, ERR_NEED_DOWN_APP, 2) != 0 &&
                memcmp(ptApp->szRetCode, ERR_DUPLICATE_PSAM_NO, 2) != 0 &&
                memcmp(ptApp->szRetCode, ERR_AUTHCODE, 2) != 0 )
            {
                WriteLog(ERROR, "UnpackScriptPos fail");
                WriteHdLog( szRcvBuf, iLen, "ScriptPos receive invalid message from pos" );
            }

            WriteMoniLog( ptApp, NULL );
            WebDispMoni( ptApp, NULL, gszWebIp, gszWebPort );

            /* 重置返回指令 */
            /* 挂机 */
            memcpy(ptApp->szCommand, "\xA6", 1);

            /* 显示结果信息 */
            memcpy(ptApp->szCommand+1, "\xA2", 1);

            ptApp->iCommandNum = 2;
            ptApp->iCommandLen = 2;

            if(strlen(ptApp->szNextTransCode) == 8)
            {
                GetNextTransCmd(ptApp);
            }

            SendOutToPos(ptApp, iNewSock);

            FreeTdi(ptApp->lTransDataIdx);

            continue;
        }

        /* IP地址 */
        strcpy(ptApp->szIp, szIp);

        /* 接收应答消息类型 进程号 */
        ptApp->lProcToAccessMsgType = lParentPid;

        /* 设置日志记录的安全模块号 */
        SetEnvTransId(ptApp->szPsamNo);

        /*发送交易数据索引号给处理层，通知处理层进行交易后续处理*/
        if( SendAccessToProcQue( ptApp->lAccessToProcMsgType, ptApp->lTransDataIdx ) != SUCC )
        {
            WriteLog(ERROR, "send to proc fail");

            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR );

            SendOutToPos(ptApp, iNewSock);

            FreeTdi(ptApp->lTransDataIdx);

            continue;
        }
    }
}

/*******************************************************************************
 * 函数功能： 守护在PROC_TO_ACCESS消息队列中，等待接收交易应答，并进行组包处理，
 *            然后通过socket发送给终端。
 * 输入参数：
 *            iNewSock  -  sock端口
 *            lParentPid-  接入层进程号，作为接收的消息类型
 *            szIp      -  请求方IP地址
 *            szSrvPort -  服务端口号
 * 输出参数：
 *           无
 * 返 回 值： 
 *           FAIL  -  失败
 *
 * 作    者：Robin
 * 日    期：2012/11/20
 *
 ******************************************************************************/
int ParentProcess( int iNewSock, long lParentPid, char *szIp, char *szSrvPort )
{
    T_App   *ptApp;
    uchar   szSndBuf[BUFFSIZE], szTmpBuf[BUFFSIZE];
    int     iLen, i, iRet;
    long    lMsgType, lTDI;
    T_TERMINAL      tTerm;

    lMsgType = lParentPid;

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    
    if( sigsetjmp(env,1) != 0 )
    {
        WriteLog( ERROR, "receive usr2 and exit" );
        /*删除共享内存中记录的接入层pid号*/
        DelAccessPid( lParentPid );
        shutdown( iNewSock, 2 );
        exit( 0 );
    }
    
    signal( SIGUSR2, RecUSR2Proc );

    WriteLog(TRACE, "Parent OpenDB begin..." );
    if( OpenDB() )
    {
        WriteLog( ERROR, "open database fail" );
        exit( 0 );
    }
    WriteLog(TRACE, "Parent OpenDB succ" );

    /* 登记本进程的进程号 */
    SetAccessPid( szIp, atoi(szSrvPort), lParentPid );
        
    WriteLog(TRACE, "ScriptPos child father[%ld] ready read msg", lMsgType); 
    i = 1;
    while (1) {
        if( RecvProcToAccessQue( lMsgType, 0, &lTDI ) != SUCC ) 
        {
            /*防止消息队列被误删，日志增长太快，采用i计数器控制
            每10000次记一次错误日志*/
            if( i == 1 )
                WriteLog(ERROR, "ScirptPos recv from processing layer fail!");
            i ++;
            if( i == 10000 )
                i = 1;
            continue;
        }
    
        ptApp = GetAppAddress( lTDI );
        if( ptApp == NULL )
        {
            WriteLog( ERROR, "GetApp fail, TDI=%d", lTDI );
            continue;
        }

        WriteAppStru( ptApp, "ScriptPos recv from processing layer" );

        /* 终端更新交易最后一步(更新结果通知)不需要返回给终端，并主动释放TDI */
        if( memcmp( ptApp->szTransCode, "FF", 2) == 0 )
        {
            FreeTdi( lTDI );
            continue;
        }

        /* 交易失败 */
        if(memcmp(ptApp->szRetCode, TRANS_SUCC, 2) != 0)
        {
            /* 重置返回指令 */
            /* 挂机 */
            memcpy(ptApp->szCommand, "\xA6", 1);

            /* 显示结果信息 */
            memcpy(ptApp->szCommand+1, "\xA2", 1);

            ptApp->iCommandNum = 2;
            ptApp->iCommandLen = 2;
        }

        /* 检查终端是否需要更新TMS */
        if(ChkTmsUpdate(ptApp) != SUCC)
        {
            GetNextTransCmd(ptApp);

            SendOutToPos(ptApp, iNewSock);

            continue;
        }

        /* 检查终端是否需要更新应用 */
        memset(&tTerm, 0, sizeof(T_TERMINAL));

        if(GetTermRec(ptApp, &tTerm) == SUCC && ChkAppUpdate(ptApp, &tTerm) != SUCC)
        {
            GetNextTransCmd(ptApp);

            SendOutToPos(ptApp, iNewSock);

            continue;
        }

        SendOutToPos(ptApp, iNewSock);

        continue;
    }
}
