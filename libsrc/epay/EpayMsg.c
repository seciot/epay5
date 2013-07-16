/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：epay库文件,主要操作消息队列
** 创 建 人：高明鑫
** 创建日期：2012/11/8
** $Revision: 1.5 $
** $Log: EpayMsg.c,v $
** Revision 1.5  2013/06/28 08:35:16  fengw
**
** 1、添加创建、删除时记录TRACE日志，出现故障时便于跟踪确定问题。
**
** Revision 1.4  2012/11/29 08:04:19  chenrb
** 修订出错提示信息,以反映实际情况
**
** Revision 1.3  2012/11/29 02:28:52  gaomx
** *** empty log message ***
**
** Revision 1.2  2012/11/29 02:23:56  gaomx
** *** empty log message ***
**
** Revision 1.1  2012/11/28 01:38:17  gaomx
** add by gaomx
**
** Revision 1.6  2012/11/28 01:20:59  gaomx
** 修改消息队列的文件为绝对路径
**
** Revision 1.5  2012/11/27 07:02:39  gaomx
** *** empty log message ***
**
** Revision 1.4  2012/11/27 06:13:47  epay5
**
** dos转unix格式
**
** Revision 1.3  2012/11/27 05:21:47  epay5
** modified by gaomx
**
*******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include "errno.h"
#include "../../incl/user.h"
#include "../../incl/app.h"
#include "EpayMsg.h"

int	giGetMsgFlag = 0;

static	jmp_buf		env1, env_hsm;
static	void	TimeoutProcess1( int nouse );
static	void	TimeoutProcHsm( int nouse );

/*****************************************************************
** 功    能:创建新的消息队列或者获取已有的消息队列
** 输入参数:
           无
** 输出参数:
           无
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by gaomx 20121119规范命名及排版修订
**
****************************************************************/
int	GetEpayMsgId( )
{
	char szPath[80];
	
	sprintf( szPath, "%s%s", getenv("WORKDIR"),MSGFILE );
	giProcToPresent = GetMsgQue( szPath, PROC_TO_PRESENT_QUEUE );
	if ( giProcToPresent < 0 )
    {
        return ( FAIL );
    }
    giPresentToProc = GetMsgQue( szPath, PRESENT_TO_PROC_QUEUE );
	if ( giPresentToProc < 0 )
    {
        return ( FAIL );
    }
    giProcToAccess = GetMsgQue( szPath, PROC_TO_ACCESS_QUEUE );
	if ( giProcToAccess < 0 )
    {
        return ( FAIL );
    }
    giAccessToProc = GetMsgQue( szPath, ACCESS_TO_PORC_QUEUE );
	if ( giAccessToProc < 0 )
    {
        return ( FAIL );
    }
    giToHsm = GetMsgQue(szPath, TO_HSM_QUEUE);
	if ( giToHsm < 0 )
    {
        return ( FAIL );
    }
    giFromHsm = GetMsgQue(szPath, FROM_HSM_QUEUE);
	if ( giFromHsm < 0 )
    {
        return ( FAIL );
    }

	giGetMsgFlag = 1;

	return( SUCC );
}


/*****************************************************************
** 功    能:创建消息队列
** 输入参数:
           无
** 输出参数:
           无
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by gaomx 20121119规范命名及排版修订
**
****************************************************************/
int CreateEpayMsg ( )
{
	char szPath[80];	
	
	WriteLog(TRACE, "创建EPAY消息队列开始");
    
    sprintf( szPath, "%s%s", getenv("WORKDIR"),MSGFILE );
	if(CreateMsgQue(szPath, ACCESS_TO_PORC_QUEUE) <0 )
    {
		WriteLog(ERROR,"creat msg ACCESS_TO_PORC_QUEUE err!");
		return FAIL;
	}	
	
	if(CreateMsgQue(szPath, PROC_TO_ACCESS_QUEUE) <0 )
    {
		WriteLog(ERROR,"creat msg PROC_TO_ACCESS_QUEUE err!");
		return FAIL;
	}	

	if(CreateMsgQue(szPath, PRESENT_TO_PROC_QUEUE) <0 )
    {
		WriteLog(ERROR,"creat msg PRESENT_TO_PROC_QUEUE err!");
		return FAIL;
	}	

	if(CreateMsgQue(szPath, PROC_TO_PRESENT_QUEUE) <0 )
    {
		WriteLog(ERROR,"creat msg PROC_TO_PRESENT_QUEUE err!");
		return FAIL;
	}	
	
	if(CreateMsgQue(szPath, TO_HSM_QUEUE) <0 )
    {
		WriteLog(ERROR,"creat msg TO_HSM_QUEUE err!");
		return FAIL;
	}	

	if(CreateMsgQue(szPath, FROM_HSM_QUEUE) <0 )
    {
		WriteLog(ERROR,"creat msg FROM_HSM_QUEUE err!");
		return FAIL;
	}

	WriteLog(TRACE, "创建EPAY消息队列成功");

	return ( SUCC );
}

/*****************************************************************
** 功    能:删除消息队列
** 输入参数:
           无
** 输出参数:
           无
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by gaomx 20121119规范命名及排版修订
**
****************************************************************/
int RmEpayMsg ()
{
	int iFlag , iRet ;
	
    iRet = FAIL;
	iFlag = SUCC;

    WriteLog(TRACE, "删除EPAY消息队列开始");
		
	/* 获取已有的消息队列 */
	iRet = GetEpayMsgId();
	if ( iRet != SUCC )
	{
		return iRet;	
	}
	
	if( RmMsgQue( giAccessToProc ) != SUCC )
	{
		WriteLog( ERROR, "rm msg ACCESS_TO_PORC_QUEUE fail" );
		iFlag = FAIL;
	}

	if( RmMsgQue( giProcToAccess ) != SUCC )
	{
		WriteLog( ERROR, "rm msg PROC_TO_ACCESS_QUEUE fail" );
		iFlag = FAIL;
	}
	
	if( RmMsgQue( giProcToPresent ) != SUCC )
	{
		WriteLog( ERROR, "rm msg PRESENT_TO_PROC_QUEUE fail" );
		iFlag = FAIL;
	}
	
	if( RmMsgQue( giPresentToProc ) != SUCC )
	{
		WriteLog( ERROR, "rm msg PRESENT_TO_PROC_QUEUE fail" );
		iFlag = FAIL;
	}
	
	if( RmMsgQue( giToHsm ) != SUCC )
	{
		WriteLog( ERROR, "rm msg TO_HSM_QUEUE fail" );
		iFlag = FAIL;
	}
	
	if( RmMsgQue( giFromHsm ) != SUCC )
	{
		WriteLog( ERROR, "rm msg FROM_HSM_QUEUE fail" );
		iFlag = FAIL;
	}

    WriteLog(TRACE, "删除EPAY消息队列成功");

	return iFlag;
}

/*****************************************************************
** 功    能:向消息队列中发送消息
** 输入参数:
           msgid		消息队列的标识符
 *		lMsgType	消息的类型
 *		pszSendData	发送数据
** 输出参数:
           无
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by gaomx 20121119规范命名及排版修订
**
****************************************************************/
int SendMessage(iMsgId, lMsgType, pszSendData)
int	iMsgId;
long 	lMsgType;
char	*pszSendData;
{
	int	iRet;
	T_MessageStru tMsgStru;
	struct msqid_ds msInfo;

	tMsgStru.lMsgType = lMsgType;
	memcpy(tMsgStru.szMsgText, pszSendData, MAX_MSG_SIZE);

	iRet = msgsnd(iMsgId, &tMsgStru, MAX_MSG_SIZE, ~IPC_NOWAIT);
	if( iRet == FAIL )
	{
		WriteLog(ERROR, "send msg error[%d-%s] MsgType[%ld] qnum[%ld]", errno,strerror(errno), tMsgStru.lMsgType, msInfo.msg_qnum);
		return( FAIL );
	}

	return( SUCC );
}


/*****************************************************************
** 功    能:从消息队列中读取消息
** 输入参数:
        lMsgType	消息的类型
 *		timeout		0：无限等待接收消息
 *				>0：超时时间
** 输出参数:
            pszReadData	收到的数据
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by gaomx 20121119规范命名及排版修订
**
****************************************************************/
int 	ReceiveMessage(iMsgId, lMsgType, iTimeOut, pszReadData)
int	iMsgId;
long 	lMsgType;
int 	iTimeOut;
char	*pszReadData;
{
	int	iRet;
	T_MessageStru tMsgStru;

	if( iTimeOut != 0 )
    {
		signal(SIGALRM, TimeoutProcess1);
	
		if( sigsetjmp(env1,1) != 0 )
		{
			WriteLog( TRACE, "read msg timeout!" );
			return( TIMEOUT );
		}
	
		alarm(iTimeOut);
	}

	iRet = msgrcv(iMsgId, (char *)&tMsgStru, MAX_MSG_SIZE, lMsgType, 0);
	if( iRet == FAIL )
    {
		alarm(0);
		WriteLog(ERROR, "read msg error msgid=%d lMsgType=%ld error=[%d-%s]", iMsgId, lMsgType, errno,strerror(errno));
		return( FAIL );
	}
	alarm(0);

	memcpy(pszReadData, tMsgStru.szMsgText, MAX_MSG_SIZE);

	return( SUCC );
}

/*****************************************************************
** 功    能:向消息队列中发送消息
** 输入参数:
        msgid		消息队列的标识符
 *		lMsgType	消息的类型
 *		pszSendData	发送数据，大小为HSMSIZE
** 输出参数:
            无
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by gaomx 20121119规范命名及排版修订
**
****************************************************************/
int SendHsmMessage(iMsgId, lMsgType, pszSendData)
int	iMsgId;
long 	lMsgType;
char	*pszSendData;
{
	int	iRet;
	T_HsmStru tMsgStru;
	struct msqid_ds msInfo;

	tMsgStru.lMsgType = lMsgType;
	memcpy(tMsgStru.szMsgText, pszSendData, HSMSIZE);

	iRet = msgsnd(iMsgId, &tMsgStru, HSMSIZE, ~IPC_NOWAIT);
	if( iRet == FAIL )
	{
		WriteLog(ERROR, "send msg error[%d-%s] MsgType[%ld] qnum[%ld]", errno,strerror(errno), tMsgStru.lMsgType, msInfo.msg_qnum);
		return( FAIL );
	}

	return( SUCC );
}

/*****************************************************************
** 功    能:从消息队列中读取消息
** 输入参数:
        lMsgType	消息的类型
 *		timeout		0：无限等待接收消息
 *				>0：超时时间
** 输出参数:
            pszReadData	收到的数据
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by gaomx 20121119规范命名及排版修订
**
****************************************************************/
int 	ReceiveHsmMessage(iMsgId, lMsgType, iTimeOut, pszReadData)
int	iMsgId;
long 	lMsgType;
int 	iTimeOut;
char	*pszReadData;
{
	int	iRet;
	T_HsmStru tMsgStru;

	if( iTimeOut != 0 )
	{
	    signal(SIGALRM, TimeoutProcHsm);
	
		if( sigsetjmp(env_hsm, 1) != 0 )
		{
			return( TIMEOUT );
		}

		alarm(iTimeOut);
	}

	iRet = msgrcv(iMsgId, (char *)&tMsgStru, HSMSIZE, lMsgType, 0);
	if( iRet == FAIL )
	{
		alarm(0);
		WriteLog(ERROR, "read msg error msgid=%d lMsgType=%ld error=[%d-%s]", iMsgId, lMsgType, errno,strerror(errno));
		return( FAIL );
	}

	alarm(0);

	memcpy(pszReadData, tMsgStru.szMsgText, HSMSIZE);

	return( SUCC );
}

/*************************************************************
*  功    能: 	
*		接入层向交易处理层发送交易请求。有判断消息队列是否已满，
*		如果已满，则等待一段时间再发送。
*  输入参数: 
*		lMsgType		发送消息类型，取自公共数据结构中
*						的lToTransMsgType
*  		lTransDataIdx	交易数据索引号
*  输出参数: 	无
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int SendAccessToProcQue ( lMsgType, lTransDataIdx )
long 	lMsgType;
long lTransDataIdx ;
{	
	char szSendData[MAX_MSG_SIZE+1];

	sprintf(szSendData,"%ld",lTransDataIdx);
	return(SendMessage(giAccessToProc, lMsgType, szSendData));
}

/*************************************************************
*  功    能: 	
*		交易处理层向接入层发送交易应答。有判断消息队列是否
*		已满，如果已满，则等待一段时间再发送。
*  输入参数: 
*		lMsgType		发送消息类型，取自公共数据结构中的
*						lFromTransMsgType(在接入层发送请求之前
*						赋值)
*  		lTransDataIdx	交易数据索引号
*  输出参数: 	无
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int SendProcToAccessQue( lMsgType, lTransDataIdx )
long 	lMsgType;
long lTransDataIdx ;
{
	char szSendData[MAX_MSG_SIZE+1];
	
    sprintf(szSendData,"%ld",lTransDataIdx);
	return(SendMessage(giProcToAccess, lMsgType, szSendData));
}



/*************************************************************
*  功    能: 	
*		安全服务请求方向hsm发送交易请求。有判断消息队列是否
*		已满，如果已满，则等待一段时间再发送。
*		被$HOME/libepay/hsmcli.c文件中各安全服务函数调用。
*  输入参数: 
*		lMsgType		发送消息类型，取自公共数据结构中
*						的lTransDataIdx
*  		tFace			发送的数据结构
*  输出参数: 	无
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int SendToHsmQue( lMsgType, tFace )
long 	lMsgType;
T_Interface *tFace;
{
	char szSendData[HSMSIZE+1];

	if( !giGetMsgFlag )
	{
		if( GetEpayMsgId( ) == FAIL )
		{
			WriteLog( ERROR, "gmsg error!" );
			return( FAIL );
		}
		giGetMsgFlag = 1;
	}

	memcpy(szSendData, (char *)tFace, HSMSIZE);
	return(SendHsmMessage(giToHsm, lMsgType, szSendData));
}

/*************************************************************
*  功    能: 	
*		hsm加密服务模块向安全服务请求方发送交易应答。有判断
*		消息队列是否已满，如果已满，则等待一段时间再发送。
*  输入参数: 
*		lMsgType		发送消息类型，取自tFace结构中
*						的lSoureMsgType
*  		tFace			发送的数据结构
*  输出参数: 	无
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int SendFromHsmQue( lMsgType, tFace )
long 	lMsgType;
T_Interface *tFace;
{
	char szSendData[HSMSIZE+1];

	memcpy(szSendData,(char *)tFace, HSMSIZE);
	return(SendHsmMessage(giFromHsm, lMsgType, szSendData));
}

/*************************************************************
*  功    能: 	
*		交易处理层向业务提交层发送交易请求。有判断消息队列是否
*		已满，如果已满，则等待一段时间再发送。
*  输入参数: 
*		lMsgType		发送消息类型，取自公共数据结构中
*						的lToHostMsgType
*  		lTransDataIdx	交易数据索引号
*  输出参数: 	无
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int SendProcToPresentQue ( lMsgType ,lTransDataIdx )
long 	lMsgType;
long lTransDataIdx ;
{
	char szSendData[MAX_MSG_SIZE+1];
	
    sprintf(szSendData,"%ld",lTransDataIdx);
	return(SendMessage(giProcToPresent, lMsgType, szSendData));
}

/*************************************************************
*  功    能: 	
*		业务提交层向交易处理层发送交易应答。有判断消息队列是否
*		已满，如果已满，则等待一段时间再发送。
*  输入参数: 
*		lMsgType		发送消息类型，取自公共数据结构中的
*						lFromHostMsgType(在交易处理层发送请求之前
*						赋值)
*  		lTransDataIdx	交易数据索引号
*  输出参数: 	无
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int SendPresentToProcQue( lMsgType, lTransDataIdx )
long 	lMsgType;
long lTransDataIdx ;
{
	char szSendData[MAX_MSG_SIZE+1];

	sprintf(szSendData,"%ld",lTransDataIdx);
	return(SendMessage(giPresentToProc, lMsgType, szSendData));
}


/*************************************************************
*  功    能: 	
*		接入层从交易处理层接收交易应答。
*  输入参数: 
*		lMsgType		接收消息类型，接收进程的进程号 
*		lTimeOut		超时时间，0表示无限等待
*  输出参数: 	
*  		lTransDataIdx	交易数据索引号
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int RecvProcToAccessQue( lMsgType, lTimeOut, lTransDataIdx )
long	lMsgType;
long lTimeOut;
long *lTransDataIdx;
{
	char szReadData[MAX_MSG_SIZE+1];
	int iRet;
	
	iRet = ReceiveMessage(giProcToAccess, lMsgType, lTimeOut, szReadData);
	if( iRet != SUCC )
    {
		return iRet;
    }
	*lTransDataIdx = atol(szReadData);
	
	return( SUCC );
}

/*************************************************************
*  功    能: 	
*		交易处理层从接入层接收交易请求。
*  输入参数: 
*		lMsgType		发送消息类型，根据模块名称取自module表
*						中msg_type字段
*		lTimeOut		超时时间，0表示无限等待
*  输出参数: 	
*  		lTransDataIdx	交易数据索引号
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int RecvAccessToProcQue( lMsgType, lTimeOut, lTransDataIdx )
long	lMsgType;
long lTimeOut;
long *lTransDataIdx;
{
	char szReadData[MAX_MSG_SIZE+1];
	int iRet;
	
	iRet = ReceiveMessage(giAccessToProc, lMsgType, lTimeOut, szReadData);
	if( iRet != SUCC )
    {
		return iRet;
    }
	*lTransDataIdx = atol(szReadData);
	
	return( SUCC );
}

/*************************************************************
*  功    能: 	
*		交易处理层从业务提交层接收交易应答。
*  输入参数: 
*		lMsgType		接收消息类型，接收进程的进程号 
*		lTimeOut		超时时间，0表示无限等待
*  输出参数: 	
*  		lTransDataIdx	交易数据索引号
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int RecvPresentToProcQue( lMsgType, lTimeOut, lTransDataIdx )
long	lMsgType;
long lTimeOut;
long *lTransDataIdx;
{
	char szReadData[MAX_MSG_SIZE+1];
	int iRet;
	
	iRet = ReceiveMessage(giPresentToProc, lMsgType, lTimeOut, szReadData);
	if( iRet != SUCC )
	{
	    return iRet;
    }
	*lTransDataIdx = atol(szReadData);
	
	return( SUCC );
}

/*************************************************************
*  功    能: 	
*		业务提交层从交易处理层接收交易请求。
*  输入参数: 
*		lMsgType		发送消息类型，根据模块名称取自module表
*						中msg_type字段
*		lTimeOut		超时时间，0表示无限等待
*  输出参数: 	
*  		lTransDataIdx	交易数据索引号
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int RecvProcToPresentQue( lMsgType, lTimeOut, lTransDataIdx  )
long	lMsgType;
long lTimeOut;
long *lTransDataIdx;
{
	char szReadData[MAX_MSG_SIZE+1];
	int iRet;
	
	iRet = ReceiveMessage(giProcToPresent, lMsgType, lTimeOut, szReadData);
	if( iRet != SUCC )
	{
        return iRet;
    }
	*lTransDataIdx = atol(szReadData);
	
	return( SUCC );
}
/*************************************************************
*  功    能: 	
*		交易处理层从接入层接收交易请求。
*		被$HOME/libepay/hsmcli.c文件中各安全服务函数调用。
*  输入参数: 
*		lMsgType		发送消息类型，取自tFace结构
*						中lSourceMsgType字段
*		lTimeOut		超时时间，0表示无限等待
*  输出参数: 	
*  		tFace		接收数据结构
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int RecvFromHsmQue( lMsgType, lTimeOut, tFace )
long	lMsgType;
long	lTimeOut;
T_Interface *tFace;
{
	char szReadData[HSMSIZE+1];
	int iRet;

	iRet = ReceiveHsmMessage(giFromHsm, lMsgType, lTimeOut, szReadData);
	if( iRet != SUCC )
	{
    	return iRet;
    }
	memcpy((char *)tFace, szReadData, HSMSIZE);

	return SUCC;
}

/*************************************************************
*  功    能: 	
*		交易处理层从接入层接收交易请求。
*  输入参数: 
*		lMsgType		接收消息类型
*		lTimeOut		超时时间，0表示无限等待
*  输出参数: 	
*  		tFace		接收数据结构
*  返    回：	SUCC	成功
*				FAIL	失败
*************************************************************/
int RecvToHsmQue ( lMsgType, lTimeOut, tFace )
long	lMsgType;
long	lTimeOut;
T_Interface *tFace;
{
	char szReadData[HSMSIZE+1];
	int iRet;

	iRet = ReceiveHsmMessage(giToHsm, lMsgType, lTimeOut, szReadData);
	if( iRet != SUCC )
	{
    	return iRet;
    }
	memcpy((char *)tFace, szReadData, HSMSIZE);

	return SUCC;
}


/* 在函数间跳转 */
static	void	TimeoutProcess1( int nouse )
{
	siglongjmp( env1, 1 );
}

 /* 在函数间跳转 */
static	void	TimeoutProcHsm( int nouse )
{
	siglongjmp( env_hsm, 1 );
}

/**************************************************************************
 *  功    能: 清空消息队列
 *
 *  输入参数: iReadType ---  队列标识
 *      ROUTE MONITOR TO_TRANS FROM_TRANS TO_HOST FROM_HOST
 *      TO_HSM FROM_HSM
 *
 *  输出参数: 
 *
 *  返    回：0
 *
 *  作    者: Robin
 *
 *  日    期: 2001/06/23
 *
 *  修 改 人:
 *
 *  修改日期:
 *************************************************************************/
int
ClearMessage( int iReadType )
{
 	int iRet, iMsgId, iNum;
    long lMsgType, lTimeOut, lTransDataIdx;
    struct msqid_ds msInfo;
    T_MessageStru tMsgStru;
    T_Interface tHsmFace;
    char  szBuffer[2000], szTitle[512], szTmpBuf[256];

    if( GetEpayMsgId( ) == FAIL )
    {
        WriteLog( ERROR, "gmsg error!" );
        return( FAIL );
    }

    lTimeOut = 0;
    switch ( iReadType )
    {	    
	    case ACCESS_TO_PORC_QUEUE:
	        iMsgId = giAccessToProc;
	        sprintf( szTitle, "ScriptPos/ComWeb/AutoVoid->FinaTran/ManaTran" );
	        break;
	    case PROC_TO_ACCESS_QUEUE:
	        iMsgId = giProcToAccess;
	        sprintf( szTitle, "FinaTran/ManaTran->ScriptPos/ComWeb/AutoVoid" );
	        break;
	    case PROC_TO_PRESENT_QUEUE:
	        iMsgId = giProcToPresent;
	        sprintf( szTitle, "FinaTran->PayGate/ServiceGate" );
	        break;
	    case PRESENT_TO_PROC_QUEUE:
	        iMsgId = giPresentToProc;
	        sprintf( szTitle, "PayGate/ServiceGate->FinaTran" );
	        break;
	    case TO_HSM_QUEUE:
	        iMsgId = giToHsm;
	        sprintf( szTitle, "--->Hsm" );
	        break;
	    case FROM_HSM_QUEUE:
	        iMsgId = giFromHsm;
	        sprintf( szTitle, "Hsm--->" );
	        break;
		default:
		    WriteLog( ERROR, "error read type %ld", iReadType );
		return FAIL;
    }
    iRet = msgctl(iMsgId, IPC_STAT, &msInfo);
    if( iRet == FAIL)
    {
        WriteLog( ERROR, "msgctl fail [%ld-%s]", errno,strerror(errno) );
        return FAIL;
    }
    iNum = msInfo.msg_qnum;
    WriteLog(TRACE, "msg_q[%s] qnum[%ld]", szTitle, iNum );
    sprintf( szTmpBuf, " qnum[%d] ", iNum );
    strcat( szTitle, szTmpBuf );
    lMsgType = 0;
    for( ; iNum > 0; iNum --)
   	{
        if( iReadType == FROM_HSM_QUEUE )
        {
            iRet = RecvFromHsmQue( lMsgType, lTimeOut, &tHsmFace );
            if( iRet != SUCC )
            {
                WriteLog( ERROR, "read from hsm fail" );
                return FAIL;
            }
            WriteHsmStru( &tHsmFace, szTitle );
	    }
	    else if( iReadType == TO_HSM_QUEUE )
	    {
            iRet = RecvToHsmQue( lMsgType, lTimeOut, &tHsmFace );
            if( iRet != SUCC )
            {
                WriteLog( ERROR, "read to hsm fail" );
                return FAIL;
            }
            WriteHsmStru( &tHsmFace, szTitle );
        }
        else
        {
            iRet = msgrcv(iMsgId, (char *)&tMsgStru, MAX_MSG_SIZE, lMsgType, IPC_NOWAIT);
            if( iRet == FAIL )
            {
                WriteLog(ERROR, "read msg error error=[%d-%s]",errno,strerror(errno));
                return FAIL;
            }
            lTransDataIdx = atol(tMsgStru.szMsgText);
            WriteLog( TRACE, "lTransDataIdx = [%ld]" ,lTransDataIdx);
        }
    }
    return SUCC;
}



/**************************************************************************
 *  功    能： 清空trans->tohost中某机构的交易请求信息 
 *
 *  输入参数:  lMsgType		消息的类型，我们采用机构类型+1作为消息类型
 *		0：所有消息；其他：指定消息
 *
 *  返    回:  无
 *
 *  作    者:  Robin
 *
 *  日    期:  2007/06/21
 *
 *  修 改 人:
 *
 *  修改日期:
 *
 *  说    明: 
 *************************************************************************/
void ClearTransRequ( lMsgType )
long	lMsgType;
{
	char szReadData[MAX_MSG_SIZE+1];
	int iRet, i;

	for( i=0; i<30; i++ )
	{
		iRet = ReceiveMessage(giProcToPresent, lMsgType, 1, szReadData);
		if( iRet != SUCC )
			return;
	}
}


