/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：双工短连接
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: dup_st.c,v $
 * Revision 1.3  2013/06/14 02:02:14  fengw
 *
 * 1、增加报文日志打印代码。
 * 2、修改短链接通讯处理机制。
 *
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

enum eActAfterSig { ExitSig = 1, ResumeSig};
static const MAX_DATA_LENGTH = 1024;
    
int giHostId, giCommNum;
static jmp_buf gProEnv;

/* ----------------------------------------------------------------
 * 功    能：进程根据收到的信号进行跳转
 * 输入参数：
 *         iSigNo    收到的信号
 * 输出参数：
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void ProcessAction( int iSigNo )
{
    WriteLog( TRACE, "recv child signal %d", iSigNo );
    if(iSigNo == SIGUSR1)
        siglongjmp(gProEnv, ExitSig);
    else if( iSigNo == SIGUSR2)
        siglongjmp(gProEnv, ResumeSig);
    return;
}

int main(int argc, char *argv[])
{
    int i, iRet, iPid;
    int iTdi, iInterval;
    long lMsgType;
    char szArgItem[100];
    char szBuffer[MAX_DATA_LENGTH];
  
    if(argc != 3)
    {
        printf("usage: to_host host_id comm_num\n");
        exit(0);
    }
    giHostId = atoi(argv[1]);
    giCommNum = atoi(argv[2]);
    if(giHostId <= 0 || giCommNum <= 0)
    {
        printf("host_id和comm_num必须大于0\n");
        exit(0);
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

    iRet = SetHost( giHostId, giCommNum, 'Y', 'Y' );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "SetHost fail");
        exit( 0 );
    }

    signal(SIGUSR1, ProcessAction);

    /* 根据jmp的值处理信号 */
    iRet = sigsetjmp(gProEnv, 1);
    if(iRet == ExitSig)
    {
        iRet = SetHost( giHostId, giCommNum, 'N', 'N' );
        exit(0);
    }
    
    lMsgType = giHostId;

    while(1) 
    {
        memset( szBuffer, 0, sizeof( szBuffer ) );

        iRet = RecvProcToPresentQue(lMsgType, 0, &iTdi);
        if(iRet != SUCC)
        {
            sleep(5);

            continue;
        }
        else
        {
            iPid = fork();
            if(iPid == -1)
            {
                 exit(0);
            }
            else if(iPid > 0)
            {
                continue;
            }

            ProcessTrans(szBuffer, iTdi);

            exit(0);
        }
    }
}
 
/* ----------------------------------------------------------------
 * 功    能：    1、从消息队列中读取交易请求信息，将内部公用数据结构
 *  数据按各主机系统的报文格式转换，并发送到相应主机系统。
 *                          2、从socket中读取主机系统的响应信息，读到后将各成
 * 员机构的报文转换成内部公用数据结构，将交易响应信息回  送交易请求方。
 * 输入参数：
 *            szBuffer:消息队列中的数据
 *            iTdi:交易索引
 * 输出参数：
 * 返 回 值：
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ProcessTrans(char *szBuffer, int iTdi)
{
    int i, iRet, iLength;
    T_App tApp, *ptApp;
    char szRemoteIp[20], szRemotePort[10];
    long lMsgType;
    int iSockId, fd;
    int iTimeout, iShmExpired;
    int iRetTdi;
    char szArgItem[100];

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    
    /*读共享内存*/
    ptApp = GetAppAddress(iTdi);
    if( ptApp == NULL )
    {
        WriteLog( ERROR, "GetApp [%d]error", giHostId );
        return FAIL;
    }

    iRet = OpenDB();
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "parent open database fail" );
        return FAIL;
    }

    /*与后台建立链接*/
    sprintf(szArgItem, "REMOTE_IP_%d", giHostId);
    GetCharParam(szArgItem, szRemoteIp);
    sprintf(szArgItem, "REMOTE_PORT_%d", giHostId);
    GetCharParam(szArgItem, szRemotePort);

    iSockId = CreateCliSocket( szRemoteIp, szRemotePort );
    if(iSockId < 0)
    {
        WriteLog(ERROR, "connect host [%d]err %s", giHostId, strerror(errno));
        /*出错，发送响应信息给请求方*/
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
        SendPresentToProcQue( ptApp->lPresentToProcMsgType, iTdi );
        return FAIL;
    }

    /*将内部公用数据结构数据转换成后台报文*/
    iLength = PackageRequest(ptApp, szBuffer);
    if( iLength < 0 )
    {
        WriteLog( ERROR, "pack Host[%03d] fail", giHostId );
        close(iSockId);
        /*出错，发送响应信息给请求方*/
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
        SendPresentToProcQue( ptApp->lPresentToProcMsgType, iTdi );
        return FAIL;
    }

    WriteHdLog(szBuffer, iLength, "send to host");

    iRet = WriteSock( iSockId, szBuffer, iLength, 0 );
    if ( iRet <= 0 )
    {
        WriteLog( ERROR, "Host[%03d][%d] write idle pack errno[%ld]", giHostId, giCommNum, errno );
        close(iSockId);
        /*出错，发送响应信息给请求方*/
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
        SendPresentToProcQue( ptApp->lPresentToProcMsgType, iTdi );
        return FAIL;
    }

    /* 发送交易请求成功，记录pid_match表 */
    iRet = SetTdiMatch( iTdi, ptApp->szShopNo, ptApp->szPosNo, ptApp->lSysTrace ,ptApp->iTransType);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "insert pid_match fail" );
        close(iSockId);
        return FAIL;
    }
   
    sprintf(szArgItem, "REMOTE_TIMEOUT_%d", giHostId);
    GetIntParam(szArgItem, iTimeout);
    GetIntParam("ShmExpired", iShmExpired);
    iLength = ReadSockVarLen(iSockId, iTimeout, szBuffer);
    if( iLength <= 0 )
    {
        WriteLog( ERROR, "read length fail %ld, nLength=[%d]", errno, iLength );
        close(iSockId);
        /*查询匹配记录*/
        iRetTdi = GetTdiMatch(ptApp->szShopNo, ptApp->szPosNo, ptApp->lSysTrace, iShmExpired,ptApp->iTransType);
        if( iRetTdi == FAIL || iRetTdi != iTdi)
        {
            WriteLog( ERROR, "GetTdiMatch [%d]error", giHostId );
            return FAIL;
        }
        /*出错，发送响应信息给请求方*/
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
        SendPresentToProcQue( ptApp->lPresentToProcMsgType, iTdi );
        WriteLog( ERROR, "time out begin return" );
        return FAIL;
    }

    WriteHdLog(szBuffer, iLength, "recv from host");
            
    /*拆数据包 */
    iRet = UnpackageRespond(ptApp, szBuffer, iLength);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "Unpack Host[%03ld] error", giHostId );
        close(iSockId);
        /*查询匹配记录*/
        iRetTdi = GetTdiMatch(ptApp->szShopNo, ptApp->szPosNo, ptApp->lSysTrace, iShmExpired,ptApp->iTransType);
        if( iRetTdi == FAIL || iRetTdi != iTdi)
        {
            WriteLog( ERROR, "GetTdiMatch [%d]error", giHostId );
            return FAIL;
        }
        /*出错，发送响应信息给请求方*/
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
        SendPresentToProcQue( ptApp->lPresentToProcMsgType, iTdi );
        return FAIL;    
    }

    /*查询匹配记录*/
    iRetTdi = GetTdiMatch(ptApp->szShopNo, ptApp->szPosNo, ptApp->lSysTrace, iShmExpired,ptApp->iTransType);
    if( iRetTdi == FAIL || iRetTdi != iTdi)
    {
        WriteLog( ERROR, "GetTdiMatch [%d]error", giHostId );
        close(iSockId);
        return FAIL;
    }

    /*发送响应信息给交易发起进程*/
    iRet = SendPresentToProcQue(ptApp->lPresentToProcMsgType, iTdi);
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "SendPresentToProcQue[%03ld] error", giHostId );
        close(iSockId);
        return FAIL;
    }

    close(iSockId);
    return SUCC;
}

/* ----------------------------------------------------------------
 * 功    能：   按各主机系统的报文格式发送心跳包
 * 输入参数：
 * 输出参数：
 * 返 回 值：
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ProcessHB()
{
    int i, iRet, iLength;
    T_App tApp, *ptApp;
    char szRemoteIp[20], szRemotePort[10];
    long lMsgType, iTdi;
    int iSockId, fd;
    int iTimeout;
    int iRetTdi;
    char szArgItem[100];
    char szBuffer[MAX_DATA_LENGTH];

    for( i = 0; i < 32; i++ )
    {
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
             continue;
        signal( i, SIG_IGN );
    }
    
    /*与后台建立链接*/
    sprintf(szArgItem, "REMOTE_IP_%d", giHostId);
    GetCharParam(szArgItem, szRemoteIp);
    sprintf(szArgItem, "REMOTE_PORT_%d", giHostId);
    GetCharParam(szArgItem, szRemotePort);
    iSockId = CreateCliSocket( szRemoteIp, szRemotePort );
    if(iSockId < 0)
    {
        return FAIL;
    }
    
    iLength = PackHB(szBuffer);            
    if( iLength < 0 )
    {
        close(iSockId);
        return FAIL;
    }
    iRet = WriteSock( iSockId, szBuffer, iLength, 0 );
    if ( iRet <= 0 )
    {
        close(iSockId);
        return FAIL;
    }

       close(iSockId);
    return SUCC;
}
