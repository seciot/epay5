/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.8 $
** $Log: finatran.c,v $
** Revision 1.8  2013/06/07 02:14:40  fengw
**
** 1、增加是否检查号码绑定判断相关代码。
**
** Revision 1.7  2012/12/25 06:54:43  fengw
**
** 1、修改web交易监控通讯端口号变量类型为字符串。
**
** Revision 1.6  2012/12/14 06:32:34  fengw
**
** 1、增加“绑定电话号码检查长度”参数项读取。
**
** Revision 1.5  2012/12/07 05:58:04  fengw
**
** 1、增加获取消息队列ID函数调用。
**
** Revision 1.4  2012/12/07 02:02:21  fengw
**
** 1、增加web监控参数配置读取。
**
** Revision 1.3  2012/11/23 09:09:48  fengw
**
** 金融交易处理模块初始版本
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#include "finatran.h"

void _proc_exit(int iSigNo)
{
	CloseDB();

	exit(0);
}

int main(int argc, char *argv[])
{
    int     i;
    int     iMaxCount;          /* 处理模块最大进程数 */
    long    lMsgType;           /* 交易处理模块接收消息类型 */
    char    szTmpBuf[64+1];     /* 临时变量 */

    /* 获取启动参数 */
    if(argc != 3)
	{
		printf("parameter too few\n");

		printf("Usage:%s MaxCount MsgType\n", argv[0]);

        exit(-1);
	}

    /* 金融交易处理模块最大进程个数 */
	iMaxCount = atoi(argv[1]);

    /* 金融交易处理模块接收请求消息类型 */
	lMsgType = atoi(argv[2]);

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

    /* 判断是否检查号码绑定 */
    memset(szTmpBuf, 0 , sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_PUBLIC, "CHECK_PHONE", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_PUBLIC, "CHECK_PHONE");

        exit(-1);
    }
    giTeleChkType = atoi(szTmpBuf);

    /* 绑定电话号码检查位数 */
    memset(szTmpBuf, 0 , sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_PUBLIC, "TELE_NO_LEN", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_PUBLIC, "TELE_NO_LEN");

        exit(-1);
    }
    giTeleChkLen = atoi(szTmpBuf);

	/* fork处理进程 */
    for(i=0;i<iMaxCount;i++)
	{
		switch(fork ())
		{
		    case 0:
			    ProcTrans(lMsgType);
			    break;
		    case -1:
			    exit(-1);
		    default:
			    break;
		}
	}

	return SUCC;
}
