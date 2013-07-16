/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台自动冲正模块
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.7 $
** $Log: autovoid.ec,v $
** Revision 1.7  2013/03/22 05:34:20  fengw
**
** 1、增加TPDU赋值语句，用于判断冲正交易由平台发起还是POS发起。
**
** Revision 1.6  2012/12/21 01:57:30  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#include "autovoid.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

void _proc_exit(int iSigNo)
{
	CloseDB();

	exit(0);
}

int main(int argc, char *argv[])
{
    int     i;
    char    szTmpBuf[64+1];

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

    /* 获取已有的消息队列 */
    if ( GetEpayMsgId() != SUCC )
    { 
        WriteLog(ERROR, "GetEpayMsgId() Error!");
        exit(-1);
    }    

	signal(SIGUSR1, _proc_exit);

    /* Make a back process */
	setpgrp();

	/* 系统参数配置读取 */
    /* 冲正超时时间 */
    memset(szTmpBuf, 0 , sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_AUTOVOID, "TIMEOUT_VOID", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_AUTOVOID, "TIMEOUT_VOID");

        exit(-1);
    }
    giVoidTimeOut = atoi(szTmpBuf);

    /* 冲正间隔时间 */
    memset(szTmpBuf, 0 , sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_AUTOVOID, "AUTO_SLEEP_TIME", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_AUTOVOID, "AUTO_SLEEP_TIME");

        exit(-1);
    }
    giSleepTime = atoi(szTmpBuf);

    /* 获取交易数据索引超时时间 */
    memset(szTmpBuf, 0 , sizeof(szTmpBuf));
    if(ReadConfig(CONFIG_FILENAME, SECTION_PUBLIC, "TIMEOUT_TDI", szTmpBuf) != SUCC)
    {
        WriteLog(ERROR, "读取参数文件[%s]SECTION:[%s] 参数项:[%s] 失败!",
                 CONFIG_FILENAME, SECTION_PUBLIC, "TIMEOUT_TDI");

        exit(-1);
    }
    giTdiTimeOut = atoi(szTmpBuf);

    glPid = getpid();

    /*进程启动先睡眠几秒*/
    sleep(giSleepTime);

    while(1)
    {
        /* 判断数据库是否打开 */
        if(ChkDBLink() != SUCC && OpenDB() != SUCC )
        {
            WriteLog(ERROR, "打开数据库失败!");
            
            sleep(giSleepTime);

            continue;
        }

        if(ProcVoid() == FAIL )
        {
            WriteLog(ERROR, "ProcVoid error");
        }

        sleep(giSleepTime);
    }

    CloseDB();

	return SUCC;
}

int ProcVoid() 
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[15+1];
        char    szPosNo[15+1];
        char    szPsamNo[16+1];
        char    szPosDate[8+1];
        long    lPosTrace;
        char    szMac[16+1];
    EXEC SQL END DECLARE SECTION;

    int     iTransDataIndex;
    T_App   tApp;

    /* 声明游标 */
	EXEC SQL
	    DECLARE cur_void CURSOR
	    FOR SELECT shop_no, pos_no, pos_date, psam_no, pos_trace, mac
	    FROM void_ls
	    WHERE recover_flag = 'N' AND
	    (return_code = '00' or return_code = '68' or return_code = '96' or return_code = 'Q9');
	if(SQLCODE)
	{  
        WriteLog(ERROR, "声明游标cur_void失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }
    
    /* 打开游标 */
    EXEC SQL OPEN cur_void;
    if(SQLCODE)
    {
        WriteLog(ERROR, "打开游标cur_void失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    while(1) 
    {
        memset(szShopNo, 0, sizeof(szShopNo));
        memset(szPosNo, 0, sizeof(szPosNo));
        memset(szPsamNo, 0, sizeof(szPsamNo));
        memset(szPosDate, 0, sizeof(szPosDate));
        memset(szMac, 0, sizeof(szMac));

        EXEC SQL
            FETCH cur_void INTO :szShopNo, :szPosNo, :szPosDate, :szPsamNo, lPosTrace, szMac;
        if(SQLCODE == SQL_NO_RECORD) 
        {
            EXEC SQL CLOSE cur_void;

            break;
        }
        else if(SQLCODE) 
        {
            WriteLog(ERROR, "读取游标cur_void失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            EXEC SQL CLOSE cur_void;

            return FAIL;
        }

        if((iTransDataIndex = GetTransDataIndex(giTdiTimeOut)) == FAIL)
        {
            EXEC SQL CLOSE cur_void;

            return FAIL;
        }

        memset(&tApp, 0, sizeof(T_App));
        tApp.lTransDataIdx = iTransDataIndex;

        tApp.iTransType = AUTO_VOID;
        strcpy(tApp.szTransName, "主机自冲");
        strcpy(tApp.szPsamNo, szPsamNo);
        strcpy(tApp.szShopNo, szShopNo);
        strcpy(tApp.szPosNo, szPosNo);
        tApp.lOldPosTrace= lPosTrace;
        tApp.lAccessToProcMsgType= 1;
        tApp.lProcToPresentMsgType= 1;
        AscToBcd(szMac, 16, 0, tApp.szMac);

        /* 设置tpdu地址，用于判断冲正交易由平台发起 */
        memcpy(tApp.szSourceTpdu, "\xFF\xFF", 2);
        memcpy(tApp.szTargetTpdu, "\xFF\xFF", 2);

        /* 设置呼叫号码为全F，跳过电话号码合法性检查 */
        strcpy(tApp.szCallingTel, "FFFFFFFF");
        strcpy(tApp.szCalledTelByTerm, "FFFFFFFF");
        strcpy(tApp.szCalledTelByNac, "FFFFFFFF");

        /* 接收消息类型 */
        tApp.lProcToAccessMsgType = glPid;

        /* 发送消息类型 */
        tApp.lAccessToProcMsgType = 1;
        
        /* 默认响应码为NN */
        strcpy(tApp.szHostRetCode, "NN");
        
        if(SendVoidReq(&tApp,giVoidTimeOut) != SUCC)
        {
            EXEC SQL CLOSE cur_void;

            return FAIL;
        }

        /* 冲正成功，修改冲正标识 */
        if(memcmp(tApp.szHostRetCode, "00", 2) == 0)
        {
            BeginTran();

            EXEC SQL
                UPDATE void_ls
                SET recover_flag = 'Y'
                WHERE shop_no = :szShopNo AND pos_no = :szPosNo AND 
                pos_date = :szPosDate AND pos_trace = :lPosTrace;
            if(SQLCODE)
            {
                WriteLog(ERROR, "更新冲正交易 ShopNo:[%s] PosNo:[%s] PosDate:[%s] PosTrace:[%ld] 标志失败失败!SQLCODE=%d SQLERR=%s",
                         szShopNo, szPosNo, szPosDate, lPosTrace, SQLCODE, SQLERR);

                RollbackTran();

                continue;
            }

            CommitTran();
        }
    }

    return SUCC;
}
