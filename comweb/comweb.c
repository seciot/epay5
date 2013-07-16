/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.4 $
** $Log: comweb.c,v $
** Revision 1.4  2012/12/26 08:33:21  fengw
**
** 1、增加通讯原始数据日志记录。
**
** Revision 1.3  2012/12/25 07:00:35  fengw
**
** 1、修改web交易监控通讯端口号变量类型为字符串。
**
** Revision 1.2  2012/12/21 02:04:03  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#include "comweb.h"

void _proc_exit(int iSigNo)
{
	CloseDB();

	exit(0);
}

int main(int argc, char *argv[])
{
    int     i;
    char    szListenPort[5+1];      /* 本地监听端口 */
    int     iServSockFd;            /* 服务端socket描述符 */
    int     iClitSockFd;            /* 客户端socket描述符 */
    char    szClitIP[15+1];         /* 客户端IP地址 */
    char    szTmpBuf[64+1];         /* 临时变量 */

    /* 获取启动参数 */
    if(argc != 2)
	{
		printf("parameter too few\n");

		printf("Usage:%s Port\n", argv[0]);

        exit(-1);
	}

    /* 本地监听端口 */
    memset(szListenPort, 0, sizeof(szListenPort));
	strcpy(szListenPort, argv[1]);

	/* 生成精灵进程, 使进程与终端脱节 */
	/* 与终端脱节后,scanf等函数将无法使用 */
	switch(fork())
	{
	    case 0:
            break;
        case -1:
            exit(-1);
        default:
		    exit(0);
	}

    /* 信号处理 */
	for(i=0;i<32;i++)
	{
        if(i == SIGALRM || i == SIGKILL ||
           i == SIGUSR1 || i == SIGUSR2)
        {
            continue;
        }

		signal(i, SIG_IGN);
	}

	signal(SIGUSR1, _proc_exit);

    /* Make a back process */
	setpgrp();

    if(GetEpayMsgId() != SUCC)
    {
        exit(-1);
    }

	/* 系统参数配置读取 */
    /* 获取web监控IP地址 */
    memset(gszMoniIP, 0 , sizeof(gszMoniIP));
    if(ReadConfig(CONFIG_FILENAME, SECTION_COMMUNICATION, "WEB_IP", gszMoniIP) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_COMMUNICATION, "WEB_IP");

        exit(-1);
    }

    /* 获取web监控端口号 */
    memset(gszMoniPort, 0 , sizeof(gszMoniPort));
    if(ReadConfig(CONFIG_FILENAME, SECTION_COMMUNICATION, "WEB_PORT", gszMoniPort) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_COMMUNICATION, "WEB_PORT");

        exit(-1);
    }

    /* 获取交易数据索引超时时间 */
    memset(szTmpBuf, 0 , sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_PUBLIC, "TIMEOUT_TDI", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_PUBLIC, "TIMEOUT_TDI");

        exit(-1);
    }
    giTdiTimeOut = atoi(szTmpBuf);

    /* 打开监听端口，连续5次失败后重启进程 */
    for(i=0;i<5;i++)
    {
        iServSockFd = CreateSrvSocket(szListenPort, "tcp", 5);
        if(iServSockFd > 0)
        {
            break;
        }
        else
        {
            WriteLog(ERROR, "打开监听端口[%s]第[%d]次失败!", szListenPort, i+1);

            sleep(10);
        }
    }

    if(i == 5)
    {
        close(iServSockFd);

        exit(-1);
    }

    WriteLog(TRACE, "comweb模块pid:[%ld] socket:[%ld]等待客户端链接!", getpid(), iServSockFd);

    /* 接收后台的链接，成功后与父进程同步 */
	while(1)
	{
        memset(szClitIP, 0, sizeof(szClitIP));
        iClitSockFd = SrvAccept(iServSockFd, szClitIP);
        if(iClitSockFd < 0)
        {
            WriteLog(ERROR, "comweb客户端接入失败!");

            continue;
        }

        WriteLog(TRACE, "comweb客户端IP:[%s]链接建立!", szClitIP);

		/* 生成一个子进程，让子进程进行交易处理，父进程继续守候端口 */
		switch(fork())
		{
		    /* fork失败 */
            case -1:
                WriteLog(ERROR, "comweb模块fork子进程失败!");

                close(iClitSockFd);

                break;
            /* 子进程进行交易处理 */
            case 0:
                /* 关掉继承的iServSockFd副本 */
			    close(iServSockFd);

			    WriteLog(TRACE, "comweb模块子进程pid:[%ld]运行!", getpid()); 
			    
			    if(OpenDB() != SUCC)
			    {
			        WriteLog(ERROR, "打开数据库失败!"); 

                    close(iClitSockFd);

			        return FAIL;
			    }

			    ChildProcess(iClitSockFd);

			    close(iClitSockFd);

			    CloseDB();

			    exit(0);
			/* 父进程继续守候端口，等待连接 */
	    	default: 
                /* 关掉iClitSockFd副本 */
			    close(iClitSockFd);

			    break;
		}
	}

	return SUCC;
}

int ChildProcess(int iSockFd)
{
    T_App   tApp;                               /* app结构 */
    int     iTransDataIndex;                    /* 交易数据索引 */
    char    szLenBuf[2+1];                      /* 报文长度Buf */
    int     iRet;                               /* 函数调用结果 */
    int     iLen;                               /* 报文长度 */
    char    szReqBuf[MAX_SOCKET_BUFLEN+1];      /* 请求报文 */

    memset(&tApp, 0, sizeof(T_App));

    /* 接收报文长度 */
	memset(szLenBuf, 0, sizeof(szLenBuf));

    iRet = ReadSockFixLen(iSockFd, 0, 2, szLenBuf);
    if(iRet != 2)
    {
        WriteLog(ERROR, "接收Web交易请求报文长度失败!iRet:[%d]", iRet);

        return FAIL;
    }

    iLen = szLenBuf[0] * 256 + szLenBuf[1];

    if(iLen <= 0 && iLen > MAX_SOCKET_BUFLEN)
    {
        WriteLog(ERROR, "Web交易请求报文长度错误!iLen:[%d]", iLen);

        return FAIL;
    }

    /* 接收报文 */
	memset(szReqBuf, 0, sizeof(szReqBuf));

    iRet = ReadSockFixLen(iSockFd, 0, iLen, szReqBuf);
    if(iRet != iLen)
    {
        WriteLog(ERROR, "接收Web交易请求报文失败!预期接收长度:[%d] 实际接收长度:[%d]", iLen, iRet);

        return FAIL;
    }

    /* 记录原始通讯日志 */
    WriteHdLog(szReqBuf, iLen, "comweb recv web req");

    /* 请求报文拆包 */
    if(UnpackWebReq(&tApp, szReqBuf, iLen) != SUCC)
    {
        WriteLog(ERROR, "Web交易请求报文拆包失败!报文:[%d][%s]", iLen, szReqBuf);

        return FAIL;
    }

    /* 获取交易数据索引 */
    if((iTransDataIndex = GetTransDataIndex(giTdiTimeOut)) == FAIL)
    {
        strcpy(tApp.szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    tApp.lTransDataIdx = iTransDataIndex;
    
    /* 记录app结构数据 */
    WriteAppStru(&tApp, "comweb recv from web");

    ProcWebTrans(&tApp);

    SendWebRsp(&tApp, iSockFd);

    /* 记录app结构数据 */
    WriteAppStru(&tApp, "comweb send to web");

    return SUCC;
}