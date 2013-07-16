
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：套接口读写
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.8 $
 * $Log: SockRdWr.c,v $
 * Revision 1.8  2013/06/14 06:25:28  fengw
 *
 * 1、SendToUdpSrv函数增加setsockopt调用代码。
 *
 * Revision 1.7  2012/12/21 05:44:01  chenrb
 * *** empty log message ***
 *
 * Revision 1.6  2012/12/04 07:24:05  chenjr
 * 代码规范化
 *
 * Revision 1.5  2012/11/29 02:15:35  linqil
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.4  2012/11/28 08:25:44  linqil
 * 去掉冗余头文件user.h
 *
 * Revision 1.3  2012/11/28 02:58:33  linqil
 * 修改日志函数
 *
 * Revision 1.2  2012/11/27 06:42:35  linqil
 * 增加对pub.h的引用；修改return；增加条件判断语句
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "pub.h"
#include "user.h"

/* 变长读取的最大长度 */
#define VARLENFORREAD   1024
#define RDSOCKTIMEOUT   -3
#define WRSOCKTIMEOUT   -3

/* 从套接口读定长数据超时跳转 */
jmp_buf  RdFixLen;
static void RdFixLenTimeOut(int n)
{
    siglongjmp(RdFixLen, 1);
}

/* 从套接口读变长数据超时跳转 */
jmp_buf  RdVarLen;
static void RdVarLenTimeOut(int n)
{
    siglongjmp(RdVarLen, 1);
}

/* 往套接口写数据超时跳转 */
jmp_buf  WrSock;
static void WrSockTimeOut(int n)
{
    siglongjmp(WrSock, 1);
}



/* ----------------------------------------------------------------
 * 功    能：从指定套接口读取指定长度数据流(带超时)
 * 输入参数：iSockFd   套接口句柄
 *           iTimeOut  超时时间(秒)
 *           iLen      指定读取数据流长度
 * 输出参数：szBuf     读取到的数据流内容
 * 返 回 值：-1  调用参数错或读出错; 
 *           RDSOCKTIMEOUT 读超时
 *           =0  对端关闭；
 *           >0  成功，所读报文长度;
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ReadSockFixLen(int iSockFd, int iTimeOut, int iLen, char *szBuf)
{
    int iNvrRead, iHveRead;

    if (iSockFd <= 0 || iTimeOut < 0 || iLen <= 0 || szBuf == NULL)
    {
        WriteLog(ERROR, "输入数据非法iSockFd[%d], iTimeOut[%d] iLen[%d]",
                 iSockFd, iTimeOut, iLen);
        return FAIL;
    }

    iNvrRead = iLen;

    if( iTimeOut > 0 )
    {
        signal(SIGALRM, RdFixLenTimeOut); 
        if (sigsetjmp(RdFixLen, 1) != 0)
        {
            WriteLog(ERROR, "ReadSockFixLen timeout");
            alarm(0);
            return(RDSOCKTIMEOUT);
        }
        alarm(iTimeOut);
    }

    while (iNvrRead > 0)
    {
        iHveRead = read(iSockFd, szBuf, iNvrRead);
        if (iHveRead < 0)
        {
            if( iTimeOut > 0 )
            {
                alarm(0);
            }
            return iHveRead;
        }
        else if (iHveRead == 0)
        {
            break;
        }

        iNvrRead -= iHveRead;
        szBuf += iHveRead;
    }

    if( iTimeOut > 0 )
    {
        alarm(0);
    }

    return(iLen - iNvrRead);
}

/* ----------------------------------------------------------------
 * 功    能：从套接口读不定长数据(一次读取的内容)
 * 输入参数：iSockFd   套接口
 *           iTimeOut  超时时间(秒)
 * 输出参数：szBuf     读取报文内容
 * 返 回 值：>0  报文内容长度； 
 *           =0  对端关闭;
 *           -1 读取失败; 
 *           RDSOCKTIMEOUT 读超时
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ReadSockVarLen(int iSockFd, int iTimeOut, char *szBuf)
{
    int iHveRead = 0;

    signal(SIGALRM, RdVarLenTimeOut);
    if (sigsetjmp(RdVarLen, 1) != 0)
    {
        WriteLog(ERROR, "ReadSockVarLen timeout");
        alarm(0);
        return (RDSOCKTIMEOUT);
    }
    alarm(iTimeOut);

    iHveRead = read(iSockFd, szBuf, VARLENFORREAD);
    if (iHveRead < 0)
    {
        alarm(0);
        return FAIL;
    }

    alarm(0);
    return iHveRead;
}

/******************************************************************************
 * 功    能：从指定套接口读取含长度域的数据，去除长度域后输出
 * 输入参数：
 *           iSockFd    套接口句柄
 *           iTimeOut   超时时间(秒)，为0表示阻塞读
 *           iLenLen    长度域长度
 *           iLenType   长度域类型  HEX_DATA-十六进制 ASC_DATA-ASC十进制 BCD_DATA-BCD
 * 输出参数：
 *           szOutData  读取到的数据流内容(不包括长度)
 * 返 回 值：
 *           FAIL           读取失败
 *           RDSOCKTIMEOUT  读超时
 *           >0  成功，所读报文长度
 * 作    者：Robin
 * 日    期：2012/12/19
 * 修改日志：修改日期    修改者      修改内容简述
 *
 * **************************************************************************/
int ReadSockDelLenField( iSockFd, iTimeOut, iLenLen, iLenType, szOutData )
int iSockFd;
int iTimeOut;
int iLenLen;
uchar *szOutData;
{
    int i, iLen, iRet;
    uchar szBuf[2048], szTmpStr[256];

    if( iLenLen <= 0 || iLenLen > 4 )
    {
        WriteLog( ERROR, "iLenLen非法[%d]", iLenLen );
        return FAIL;
    }

    //读取长度域
    memset( szBuf, 0, sizeof(szBuf) );
    iLen = ReadSockFixLen( iSockFd, iTimeOut, iLenLen, szBuf );
    if( iLen != iLenLen )
    {
        WriteLog( ERROR, "读长度域失败。读到长度[%d] 期望长度[%d]", iLen, iLenLen );
        return FAIL;
    }

    //计算长度
    iLen = 0;
    switch (iLenType){
    case HEX_DATA:
        for( i=0; i<iLenLen; i++ )
        {
            iLen = iLen*256 + szBuf[i];
        }
        break;
    case ASC_DATA:
        iLen = atol(szBuf);
        break;
    case BCD_DATA:
        BcdToAsc( szBuf, iLenLen*2, LEFT_ALIGN, szTmpStr );
        iLen = atol(szTmpStr);
        break;
    default:
        WriteLog( ERROR, "iLenType非法[%d]", iLenType );
        return FAIL;
    }

    if( iLen > sizeof(szBuf) )
    {
        WriteLog( ERROR, "长度太长，怀疑为非法报文。len=[%d]", iLen );
        return FAIL;
    }

    //根据长度读取内容
    memset( szBuf, 0, sizeof(szBuf) );
    iRet = ReadSockFixLen( iSockFd, 1, iLen, szBuf );
    if( iRet != iLen )
    {
        WriteLog( ERROR, "读数据内容失败。读到长度[%d] 期望长度[%d]", iLen, iLenLen );
        return FAIL;
    }

    memcpy( szOutData, szBuf, iLen );

    return iLen;
}

/* ----------------------------------------------------------------
 * 功    能：往套接口写数据
 * 输入参数：iSockFd   套接口
 *           uszBuf    发送内容
 *           iLen      发送内容长度
 *           iTimeOut  超时时间(秒)
 * 输出参数：
 * 返 回 值：>=0           成功； 
 *           =-1           失败;  
 *          WRSOCKTIMEOUT 写超时
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int WriteSock(int iSockFd, uchar *uszBuf, int iLen, int iTimeOut)
{
    int iLeft, iWrit;

    if (iSockFd <= 0 || uszBuf == NULL || iLen <= 0 || iTimeOut < 0)
    {
        return FAIL;
    }

    iLeft = iLen;

    signal(SIGALRM, WrSockTimeOut);
    if (sigsetjmp(WrSock, 1) != 0)
    {
        WriteLog(ERROR, "WriteSock timeout");
        alarm(0);
        return (WRSOCKTIMEOUT);
    }
    alarm(iTimeOut);

    while (iLeft > 0)
    {
        iWrit = write(iSockFd, uszBuf, iLeft);
        if (iWrit <= 0)
        {
            alarm(0);
            return iWrit;
        }

        iLeft  -= iWrit;
        uszBuf += iWrit;
    }

    alarm(0);
    return iLen - iLeft;
}

/* ----------------------------------------------------------------
 * 功    能：往套接口写数据，自动在数据前面添加长度域
 * 输入参数：iSockFd    套接口
 *           szInData   发送内容
 *           iLen       发送内容长度
 *           iTimeOut   超时时间(秒)
 *           iLenLen    添加长度域长度
 *           iLenType   长度域类型
 * 输出参数：
 * 返 回 值：>=0           成功； 
 *           =-1           失败;  
 *          WRSOCKTIMEOUT 写超时
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int WriteSockAddLenField(int iSockFd, uchar *szInData, int iLen, int iTimeOut, int iLenLen, int iLenType )
{
    int     iRet;
    uchar   szBuf[2048], szTmpStr[256];

    if( iLenLen <= 0 || iLenLen > 4 )
    {
        WriteLog( ERROR, "iLenLen非法[%d]", iLenLen );
        return FAIL;
    }

    if( iLen > sizeof(szBuf)-4 )
    {
        WriteLog( ERROR, "发送数据太长[%d]", iLen );
        return FAIL;
    }

    //组织长度域
    switch (iLenType){
    case HEX_DATA:
        if( iLenLen == 2 )
        {
            if( iLen > 65535 )
            {
                WriteLog( ERROR, "发送数据太长，2字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                szBuf[0] = iLen/256;    
                szBuf[1] = iLen%256;        
            }
        }
        else if( iLenLen == 1 )
        {
            if( iLen > 256 )
            {
                WriteLog( ERROR, "发送数据太长，1字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                szBuf[0] = iLen;        
            }
        }
        else
        {
            WriteLog( ERROR, "十六进制长度，长度域没必要大于2。长度域长度[%ld]", iLenLen );
            return FAIL;
        }
        break;
    case ASC_DATA:
        if( iLenLen == 4 )
        {
            if( iLen > 9999 )
            {
                WriteLog( ERROR, "发送数据太长，4字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                sprintf( szBuf, "%04ld", iLen );
            }
        }
        else if( iLenLen == 3 )
        {
            if( iLen > 999 )
            {
                WriteLog( ERROR, "发送数据太长，3字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                sprintf( szBuf, "%03ld", iLen );
            }
        }
        else if( iLenLen == 2 )
        {
            if( iLen > 99 )
            {
                WriteLog( ERROR, "发送数据太长，2字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                sprintf( szBuf, "%02ld", iLen );
            }
        }
        else if( iLenLen == 1 )
        {
            if( iLen > 9 )
            {
                WriteLog( ERROR, "发送数据太长，1字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                sprintf( szBuf, "%01ld", iLen );
            }
        }
        break;
    case BCD_DATA:
        if( iLenLen == 2 )
        {
            if( iLen > 9999 )
            {
                WriteLog( ERROR, "发送数据太长，2字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                sprintf( szTmpStr, "%04ld", iLen );
                AscToBcd( szTmpStr, 4, LEFT_ALIGN, szBuf );
            }
        }
        else if( iLenLen == 1 )
        {
            if( iLen > 99 )
            {
                WriteLog( ERROR, "发送数据太长，1字节长度域表示不了。长度[%ld]", iLen );
                return FAIL;
            }
            else
            {
                sprintf( szTmpStr, "%04ld", iLen );
                AscToBcd( szTmpStr, 4, LEFT_ALIGN, szBuf );
            }
        }
        else
        {
            WriteLog( ERROR, "十六进制长度，长度域没必要大于2。长度域长度[%ld]", iLenLen );
            return FAIL;
        }
        break;
    default:
        WriteLog( ERROR, "iLenType非法[%d]", iLenType );
        return FAIL;
    }

    memcpy( szBuf+iLenLen, szInData, iLen );

    iRet = WriteSock( iSockFd, szBuf, iLen+iLenLen, iTimeOut );
    if( iRet != (iLen+iLenLen) )
    {
        WriteLog( ERROR, "发送数据失败。发送长度[%d] 期望长度[%d]", iRet, iLen+iLenLen );
        return FAIL;
    }

    return SUCC;
}

/* ----------------------------------------------------------------
 * 功    能：向UDP服务端发送数据
 * 输入参数：szIp   UDP服务端IP地址
 *           szPort UDP服务端端口
 *           szBuf  发送内容
 *           iLen   发送内容长度
 * 输出参数：
 * 返 回 值：0 发送成功；  -1  发送失败
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int SendToUdpSrv(char *szIp, char *szPort, char *szBuf, int iLen)
{
    int                 sockfd;
    struct sockaddr_in  srv_addr,cli_addr;
    int                 iRet;
    int                 on=1;

    /* 创建套接口 */
    sockfd = socket(AF_INET,SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        WriteLog(ERROR, "call socket error[%d-%s]"
                 , errno, strerror(errno));
        return FAIL;
    }

    setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR | SO_BROADCAST, &on, sizeof(on));

    memset((char *)&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(szIp);
    srv_addr.sin_port = htons(atoi(szPort));

    memset((char *)&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cli_addr.sin_port = htons(0);

    iRet = bind(sockfd,(struct sockaddr *)&cli_addr,sizeof(cli_addr));
    if (iRet < 0)
    {
        WriteLog(ERROR, "call bind error[%d-%s]", errno, strerror(errno));
        close(sockfd);
        return FAIL;
    }

    iRet = sendto(sockfd, szBuf, iLen, 0,
                 (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (iRet < 0) 
    {
        WriteLog(ERROR, "call sendto error[%d-%s]\n"
                 , errno, strerror(errno));
        close(sockfd);
        return FAIL;
    }

    close(sockfd);

    return SUCC;
}
