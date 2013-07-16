/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 发送交易请求
** 创 建 人：冯炜
** 创建日期：2012-11-13
**
** $Revision: 1.3 $
** $Log: SendReqToHost.ec,v $
** Revision 1.3  2012/12/11 07:00:58  fengw
**
** 1、往后台发送请求时，先调用ChkHostStatus判断通讯状态，非正常直接返回系统故障。
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
** 功    能：返回POS应答或重新发起请求
** 输入参数：
**        ptApp                 app结构指针
**        lTimeOut              交易超时时间
** 输出参数：
**        无
** 返 回 值：
**        无
** 作    者：
**        fengwei
** 日    期：
**        2012/11/13
** 调用说明：
**
** 修改日志：
****************************************************************/
int SendReqToHost(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[15+1];         /* 商户号 */
        char    szPosNo[15+1];          /* 终端号 */
        int     iPosTrace;              /* 终端流水号 */
        char    szPosDate[8+1];         /* POS交易日期 */
    EXEC SQL END DECLARE SECTION;

    int     iRet;                       /* 函数执行结果 */
    long    lTransDataIdx;              /* 交易数据索引号 */

    /* 将app结构数据保存至共享内存 */
    if(SetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "保存app结构到共享内存失败!TransDataIdx:[%ld]",
                 ptApp->lTransDataIdx);

        return FAIL;
    }
    
    /* 判断后台通讯状态是否正常 */
    if(ChkHostStatus(ptApp->lProcToPresentMsgType) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "后台 MsgType:[%d] 通讯状态异常!", ptApp->lProcToPresentMsgType);

        return FAIL;
    }

    /* 发送交易请求至后台 */
    if(SendProcToPresentQue(ptApp->lProcToPresentMsgType, ptApp->lTransDataIdx) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "发送交易请求失败!MsgType:[%ls] TransDataIdx:[%ld]",
            ptApp->lProcToPresentMsgType, ptApp->lTransDataIdx);

        return FAIL;
    }

    /* **********调用SendToHost结束后至RecvFromHost调用结束之前严禁写任何对app结构操作代码********** */

    /*请在etc/setup.ini下设置交易处理超时时间*/
    glTimeOut=30;
    /* 等待后台返回应答 */
    /* 循环接收后台应答，直至接收到正确应答或超时 */
    while(1)
    {
        iRet = RecvPresentToProcQue(ptApp->lPresentToProcMsgType, glTimeOut, &lTransDataIdx);
        if(iRet == TIMEOUT)
        {
            strcpy(ptApp->szRetCode, ERR_TIMEOUT);

            WriteLog(ERROR, "接收交易应答超时!MsgType:[%ld]", ptApp->lPresentToProcMsgType);

            /* 更新流水状态为TO */
            memset(szShopNo, 0, sizeof(szShopNo));
            memset(szPosNo, 0, sizeof(szPosNo));
            memset(szPosDate, 0, sizeof(szPosDate));

            strcpy(szShopNo, ptApp->szShopNo);
            strcpy(szPosNo, ptApp->szPosNo);
            iPosTrace = ptApp->lPosTrace;
            strcpy(szPosDate, ptApp->szPosDate);

            EXEC SQL
                UPDATE posls SET return_code = 'TO'
                WHERE shop_no = :szShopNo AND pos_no = :szPosNo AND
                      pos_trace = :iPosTrace AND pos_date = :szPosDate AND
                      cancel_flag = 'N' AND recover_flag = 'N' AND pos_settle = 'N' AND
                      return_code = 'NN';
            if(SQLCODE && SQLCODE != SQL_NO_RECORD)
            {
                WriteLog(ERROR, "更新流水状态 商户[%s] 终端[%s] POS流水[%d] 失败!SQLCODE=%d SQLERR=%s",
                         szShopNo, szPosNo, iPosTrace, SQLCODE, SQLERR);
            }
            CommitTran();

            return FAIL;
        }

        /* 判断数据索引值是否一致 */
        /* 不一致表示接收到错误应答包，继续等待接收 */
        if(ptApp->lTransDataIdx != lTransDataIdx)
        {
            WriteLog(ERROR, "交易应答匹配失败!TransDataIdx ToTrans:[%ls] TransDataIdx FromHost:[%ld]",
                     ptApp->lTransDataIdx, lTransDataIdx);

            continue;
        }

        break;
    }

    /* 将共享内存数据拷贝到app结构 */
    if(GetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "从共享内存拷贝app结构失败!TransDataIdx:[%ld]",
                 ptApp->lTransDataIdx);

        return FAIL;
    }

    return SUCC;
}
