/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 交易处理
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.7 $
** $Log: ProcTrans.ec,v $
** Revision 1.7  2013/06/14 06:26:08  fengw
**
** 1、增加SetEnvTransId调用代码。
**
** Revision 1.6  2013/03/11 07:11:31  fengw
**
** 1、根据标志位，判断交易是否需要发送至后台。
**
** Revision 1.5  2013/01/18 08:25:22  fengw
**
** 1、删除未使用变量定义。
**
** Revision 1.4  2012/12/24 08:22:15  fengw
**
** 1、修改交易处理失败后登记自动冲正流水处理。
**
** Revision 1.3  2012/12/07 02:03:23  fengw
**
** 1、增加app结构日志记录。
** 2、增加异常自动登记冲正流水。
**
** Revision 1.2  2012/12/04 01:24:28  fengw
**
** 1、替换ErrorLog为WriteLog。
**
** Revision 1.1  2012/11/23 09:09:16  fengw
**
** 金融交易处理模块初始版本
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：交易处理
** 输入参数：
**        lMsgType              接收消息类型
**        lTimeOut              交易超时时间
** 输出参数：
**        无
** 返 回 值：
**        无                    循环处理交易，直至模块进程退出
** 作    者：
**        fengwei
** 日    期：
**        2012/11/13
** 调用说明：
**
** 修改日志：
****************************************************************/
void ProcTrans(long lMsgType)
{
    int     i;
    long    lPid;                       /* 当前进程号 */
    long    lTransDataIdx;              /* 交易数据索引号 */
    T_App   tApp;                       /* app结构 */
    int     (*pFuncPre)(T_App*);        /* 交易预处理函数指针 */
    int     (*pFuncPost)(T_App*);       /* 交易后处理函数指针 */
    int     iSendToHost;                /* 发送后台标志 */

    /* 获取当前进程号 */
    lPid = getpid();

    while(1)
	{
	    /* 等待接收交易请求 */
		if(RecvAccessToProcQue(lMsgType, 0, &lTransDataIdx) != SUCC)
		{
			WriteLog(ERROR, "接收交易请求失败!MsgType:[%ld]", lMsgType);

			continue;
		}

        /* 判断数据库是否打开*/
        if(ChkDBLink() != SUCC && OpenDB() != SUCC)
        {
            WriteLog(ERROR, "打开数据库失败!");

            continue;
        }

		/* 将app结构指针指向共享内存 */
        if(GetApp(lTransDataIdx, &tApp) != SUCC)
        {
            SendRspToPos(&tApp);

            continue;
        }

        /* 设置日志记录的安全模块号 */
        SetEnvTransId(tApp.szPsamNo);

        /* 设置接收消息类型 */
        tApp.lPresentToProcMsgType = lPid;;

        /* 获取系统流水号 */
        if(GetSysTrace(&tApp) != SUCC)
        {
            SendRspToPos(&tApp);

            continue;
        }

        /* 合法性检查 */
        if(ChkValid(&tApp) != SUCC)
        {
            SendRspToPos(&tApp);

            continue;
        }

        /* 交易参数检查 */
        if(ChkEpayConf(&tApp) != SUCC)
        {
            SendRspToPos(&tApp);

            continue;
        }

        /* 获取交易处理函数指针 */
        /* 默认无处理 */
        pFuncPre = NULL;
        pFuncPost = NULL;
        i = 0;

        while(1)
        {
            if(gtaTransProc[i].iTransType == 0)
            {
                /* 未定义交易 */
                strcpy(tApp.szRetCode, ERR_INVALID_TRANS);

                WriteLog(ERROR, "交易类型[%d]未定义!", tApp.iTransType);

                SendRspToPos(&tApp);

                continue;
            }

            if(gtaTransProc[i].iTransType == tApp.iTransType)
            {
                pFuncPre = gtaTransProc[i].pFuncPre;

                pFuncPost = gtaTransProc[i].pFuncPost;

                iSendToHost = gtaTransProc[i].iSendToHost;

                break;
            }

            i++;
        }

        /* 交易预处理 */
        if(pFuncPre != NULL && pFuncPre(&tApp) != SUCC)
        {

             SendRspToPos(&tApp);

             continue;
        }

        /* 记录app结构信息 */
        WriteAppStru(&tApp, "finatran send to tohost");

        /* 发送交易请求到后台 */
        if(iSendToHost == SEND && SendReqToHost(&tApp) != SUCC)
        {
            if(memcmp(tApp.szRetCode, ERR_TIMEOUT, 2) == 0 &&
            tApp.cExcepHandle == POS_MUST_VOID &&
            InsertVoidls(&tApp) != SUCC )
            {   
                    WriteLog(ERROR, "登记冲正流水记录失败!");
                    continue;
            }   
            WriteLog( ERROR, "time out recv return");
            SendRspToPos(&tApp);

            continue;
        }

        /* 记录app结构信息 */
        WriteAppStru(&tApp, "finatran recv from tohost");

        /* 交易后处理 */
        if(pFuncPost != NULL && pFuncPost(&tApp) != SUCC &&
           tApp.cExcepHandle == POS_MUST_VOID && InsertVoidls(&tApp) != SUCC)
        {
            /* 处理失败，如果交易定义中需要冲正，则登记自动冲正流水 */
            WriteLog(ERROR, "登记冲正流水记录失败!");

            continue;
        }

        SendRspToPos(&tApp);
    }
}
