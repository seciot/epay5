/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：单工长连接
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: sig_lt.c,v $
 * Revision 1.2  2012/12/11 07:16:20  linxiang
 * *** empty log message ***
 *
 * Revision 1.1  2012/12/10 01:19:21  linxiang
 * *** empty log message ***
 *
 * ----------------------------------------------------------------
 */

#include "tohost.h"

#define CP_APP_CHAR(dp, sp, f) \
    do{\
        strncpy((dp)->f, (sp)->f, sizeof((dp)->f) - 1);\
    }while(0)
    
enum eActAfterSig { ResumeSig = 1, ExitSig};
static const MAX_DATA_LENGTH = 1024;

#define FIFO_WAKEUP_C "/tmp/sig_lt.wakeup_c.fifo"
#define FIFO_WAKEUP_P "/tmp/sig_lt.wakeup_p.fifo"

static jmp_buf gSrvEnv, gCltEnv;

int giHostId, giCommNum;

void ChildProc();
void ParentProc();

/* ----------------------------------------------------------------
 * 功    能：父进程根据收到的子进程发出的信号进行跳转
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
void ParentAction( int iSigNo )
{    
    WriteLog( TRACE, "recv child signal %d", iSigNo );
    if(iSigNo == SIGUSR1)
        siglongjmp(gSrvEnv, ExitSig);
    else if( iSigNo == SIGUSR2)
        siglongjmp(gSrvEnv, ResumeSig);
    return;
}

/* ----------------------------------------------------------------
 * 功    能：子进程根据收到的父进程发出的信号进行跳转
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
void ChildAction( int iSigNo )
{    
    WriteLog( TRACE, "recv parent signal %d", iSigNo );
    if(iSigNo == SIGUSR1)
        siglongjmp(gCltEnv, ExitSig);
    else if( iSigNo == SIGUSR2)
        siglongjmp(gCltEnv, ResumeSig);
    return;
}

int main(int argc, char *argv[])
{
    int i, iRet, iChildPid;
  
    giHostId = UNIONPAY_PPP;
    giCommNum = 1;

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

    iRet = SetHost( giHostId, giCommNum, 'N', 'N' );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "SetHost fail");
        exit( 0 );
    }
    
    while( 1 )
    {
        if(access(FIFO_WAKEUP_C, F_OK) == 0)
            unlink(FIFO_WAKEUP_C);
        if(mkfifo(FIFO_WAKEUP_C, O_CREAT|O_EXCL) < 0)
        {
            WriteLog(ERROR, "mkfifo err %d %d", errno, strerror(errno));
            exit(0);
        }
        if(access(FIFO_WAKEUP_P, F_OK) == 0)
            unlink(FIFO_WAKEUP_P);

        if(mkfifo(FIFO_WAKEUP_P, O_CREAT|O_EXCL) < 0)
        {
            WriteLog(ERROR, "mkfifo err %d %d", errno, strerror(errno));
            exit(0);
        }
        iChildPid = fork( );
        switch( iChildPid ){
        case  -1:
            WriteLog( ERROR, "fork error!!!" );
            exit( -1 );
        case   0:
            /*子进程处理*/
            ChildProc();
            exit(0);
        default:
            /*父进程处理*/
            ParentProc( iChildPid );
            sleep(60);
            break;
        }
    }
}
 
/* ----------------------------------------------------------------
 * 功    能：    子进程负责从socket中读取主机系统的响应信息，读到后将各成
 * 员机构的报文转换成内部公用数据结构，将交易响应信息回  送交易请求方。
 * 输入参数：
 * 输出参数：
 * 返 回 值：
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void ChildProc()
{
    int i, iParentPid, iLength, iRet;
    unsigned char szReadData[MAX_DATA_LENGTH];
    T_App tApp, *ptApp;
    int iSockId, iNewSockId, fd;
    char szCltIp[20], szLocalPort[10];
    int iInterval, iShmExpired;
    int iTdi;
    
    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    signal( SIGUSR1, ChildAction );
    signal( SIGUSR2, ChildAction );

    iParentPid = getppid();

    /* 子进程等待父进程的同步信号后连接数据库，防止与父进程同时去连。*/
    /* 同时去连会导致父进程连接数据库连不上，不知什么原因 */
    fd = open(FIFO_WAKEUP_C, O_RDONLY, 0);
    read(fd, szReadData, 16);
    close(fd);
    
    if(OpenDB() != SUCC)
    {
        WriteLog(ERROR, "child open database fail");
        kill(iParentPid, SIGUSR2);
        return;
    }

    /* 根据jmp的值处理信号 */
    iRet = sigsetjmp(gCltEnv, 1);
    if(iRet == ResumeSig) 
    {
        close(iSockId);
        close(iNewSockId);
    }
    else if(iRet == ExitSig)
    {
        close(iSockId);
        close(iNewSockId);
        CloseDB();
        return;
    }

    /* 打开监听端口，连续5次失败后重启进程 */
    GetCharParam("Local_Port", szLocalPort);
    for( i=0; i<5; i++ )
    {
        iSockId = CreateSrvSocket( szLocalPort, "tcp", 5 );
        if( iSockId > 0 )
        {
            break;
        }
        else
        {
            WriteLog( ERROR, "can not open socket[%s] times[%d]!!", szLocalPort, i);
            sleep(30);
        }
    }
    if(i == 5)
    {
        close(iSockId);
        CloseDB();
        kill(iParentPid, SIGUSR2);
        return;
    }

    /* 接收后台的链接，成功后与父进程同步 */
    iNewSockId = SrvAccept( iSockId, szCltIp );
    if(iNewSockId < 0)
    {
        close(iSockId);
        CloseDB();
        kill(iParentPid, SIGUSR2);
        return;
    }
    fd = open(FIFO_WAKEUP_P, O_WRONLY|O_NONBLOCK, 0);
    write(fd, "0000", 4);
    close(fd);

    GetIntParam("HostRcvInterval", iInterval);
    GetIntParam("ShmExpired", iShmExpired);
    do{
        iLength = ReadSockVarLen(iNewSockId, iInterval, szReadData);
        /*读数据出错或超时，关闭sock，并通知父进程退出*/
        if( iLength <= 0 )
        {
            WriteLog( ERROR, "read length fail %ld, nLength=[%d]", errno, iLength );
            close( iSockId );
            close(iNewSockId);
        
            /*向父进程发送信息，通知父进程退出*/
            kill( iParentPid, SIGUSR2 );

            /*将主机网络状态标志置为"N" */
            SetHost( giHostId, giCommNum, 'N', 'N' );

            CloseDB();
            return;
        }
        /* 如果设置心跳包时间，要判断是否心跳包 */
        if(iInterval != 0)
        {
            if(CheckWhetherHB(szReadData) == SUCC)
                continue;
        }

        /*拆数据包 */
        iRet = UnpackageRespond(szReadData, iLength, &tApp );
        if( iRet != SUCC )
        {
            WriteLog( ERROR, "Unpack Host[%03ld] error", giHostId );
            continue;
        }
        /*查询匹配记录*/
        iTdi = GetTdiMatch(tApp.szShopNo, tApp.szPosNo, tApp.lSysTrace, iShmExpired,tApp.iTransType);
        if( iTdi == FAIL )
        {
            WriteLog( ERROR, "GetTdiMatch [%d]error", giHostId );
            continue;
        }
        /*修改共享内存*/
        ptApp = GetAppAddress(iTdi);
        if( ptApp == NULL )
        {
            WriteLog( ERROR, "GetApp [%d]error", giHostId );
            continue;
        }
        CP_APP_CHAR(ptApp, &tApp, szRetriRefNum);
        CP_APP_CHAR(ptApp, &tApp, szAuthCode);
        CP_APP_CHAR(ptApp, &tApp, szHostRetCode);
        CP_APP_CHAR(ptApp, &tApp, szHostRetMsg);
        
        /*发送响应信息给交易发起进程*/
        iRet = SendPresentToProcQue(ptApp->lPresentToProcMsgType, iTdi);
        if( iRet != SUCC )
        {
            WriteLog( ERROR, "SendPresentToProcQue[%03ld] error", giHostId );
            continue;
        }
    }while( 1 );
}

 /* ----------------------------------------------------------------
 * 功    能：    父进程负责从消息队列中读取交易请求信息，将内部公用数据结构
 *  数据按各主机系统的报文格式转换，并发送到相应主机系统。
 * 输入参数：
 *            iChildPid    子进程PID
 * 输出参数：
 * 返 回 值：
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void ParentProc( int iChildPid )
{
    int i, iRet, iLength;
    T_App tApp, *ptApp;
    char szRemoteIp[20], szRemotePort[10];
    long lMsgType, iTdi;
    int iSockId, fd;
    int iTimeout;
    unsigned char szBuffer[MAX_DATA_LENGTH];

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    signal( SIGUSR1, ParentAction );
    signal( SIGUSR2, ParentAction );

    /*连上数据库后向子进程同步*/
    iRet = OpenDB();
    fd = open(FIFO_WAKEUP_C, O_WRONLY|O_NONBLOCK, 0);
    write(fd, "0000", 4);
    close(fd);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "parent open database fail" );
        exit( 0 );
    }
    
    iRet = sigsetjmp(gSrvEnv, 1);
    if(iRet == ResumeSig) 
    {
        close(iSockId);
    }
    else if(iRet == ExitSig)
    {
        SetHost( giHostId, giCommNum, 'N', 'N' );
        close(iSockId);
        CloseDB();
        return;
    }
    /*与后台建立链接*/
    GetCharParam("HOST_IP", szRemoteIp);
    GetCharParam("HOST_PORT", szRemotePort);
    while(1)
    {
        iSockId = CreateCliSocket( szRemoteIp, szRemotePort );
        if(iSockId > 0)
            break;
        kill(iChildPid, SIGUSR1);
        sleep(5);
    }
    /* 收到子进程的同步信号后，取队列中的数据 */
    fd = open(FIFO_WAKEUP_P, O_RDONLY, 0);
    read(fd, szBuffer, 16);
    close(fd);
    iRet = SetHost( giHostId, giCommNum, 'Y', 'Y' );
    
    lMsgType = giHostId + 1;
    GetIntParam("ReadProcTimeout", iTimeout);
    do{
        memset( szBuffer, 0, sizeof( szBuffer ) );

        iRet = RecvProcToPresentQue(lMsgType, iTimeout, &iTdi);
        /* 空闲nTimeOut秒，发送空闲报文给后台系统 */
        if( iRet == TIMEOUT )
        {
            iLength = PackHB(szBuffer);
            iRet = WriteSock( iSockId, szBuffer, iLength, 0 );
            if ( iRet <= 0 )
            {
                close( iSockId );
                WriteLog( ERROR, "Host[%03d][%d] write idle pack errno[%ld]", giHostId, giCommNum, errno );

                /* 通知子进程退出，父进程重新建立链接 */    
                kill( iChildPid, SIGUSR2 );

                CloseDB();
                SetHost( giHostId, giCommNum, 'N', 'N' );
                return;
            }
            continue;
        }
        else if( iRet != SUCC )
        {
            WriteLog (ERROR, "Read from ToHost error");
             continue;
        }


        /*读共享内存*/
        ptApp = GetAppAddress(iTdi);
        if( ptApp == NULL )
        {
            WriteLog( ERROR, "GetApp [%d]error", giHostId );
            continue;
        }        
        /*将内部公用数据结构数据转换成后台报文*/
        iLength = PackageRequest(ptApp, szBuffer);
        if( iLength < 0 ){
            WriteLog( ERROR, "pack Host[%03d] fail", giHostId );
            /*出错，发送响应信息给请求方*/
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
            SendPresentToProcQue( ptApp->lPresentToProcMsgType, iTdi );
            continue;
        }

        iRet = WriteSock( iSockId, szBuffer, iLength, 0 );
        if ( iRet <= 0 )
        {
            close( iSockId );
            WriteLog( ERROR, "Host[%03d][%d] write idle pack errno[%ld]", giHostId, giCommNum, errno );
            /* 通知子进程退出，父进程重新建立链接 */    
            kill( iChildPid, SIGUSR2 );

            CloseDB();
            SetHost( giHostId, giCommNum, 'N', 'N' );
            return;
        }

        /* 发送交易请求成功，记录pid_match表 */
        iRet = SetTdiMatch( iTdi, ptApp->szShopNo, ptApp->szPosNo, ptApp->lSysTrace, ptApp->iTransType );
        if( iRet != SUCC )
        {
            WriteLog( ERROR, "insert pid_match fail" );
        }
    }while( 1 );
}
