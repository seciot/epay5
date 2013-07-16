/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 消费交易
** 创 建 人：冯炜
** 创建日期：2012-11-09
**
** $Revision: 1.3 $
** $Log: Purchase.ec,v $
** Revision 1.3  2013/06/18 07:42:19  fengw
**
** 1、更新POS流水记录时，增加对后台交易日期、后台交易时间、结算日期的判断，如果为空，则取系统日期时间。
**
** Revision 1.2  2012/12/04 01:24:28  fengw
**
** 1、替换ErrorLog为WriteLog。
**
** Revision 1.1  2012/11/23 09:09:16  fengw
**
** 金融交易处理模块初始版本
**
** Revision 1.2  2012/11/22 09:01:55  fengw
**
** 1、增加事务处理语句
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
    char    szRetriRefNum[12+1];            /* 后台检索参考号 */
    char    szReturnCode[2+1];              /* 平台返回码 */
    char    szHostRetCode[6+1];             /* 后台返回码 */
    char    szHostRetMsg[40+1];             /* 后台返回错误信息 */
    char    szAuthCode[6+1];                /* 授权码 */
    char    szHostDate[8+1];                /* 平台交易日期 */
    char    szHostTime[6+1];                /* 平台交易时间 */
    char    szSettleDate[8+1];              /* 结算日期 */
    int     iBatchNo;                       /* 批次号 */
    char    szBankID[11+1];                 /* 银行标识号 */
    char    szShopNo[15+1];                 /* 商户号 */
    char    szPosNo[15+1];                  /* 终端号 */
    int     iPosTrace;                      /* 终端流水号 */
    char    szPan[19+1];                    /* 转出卡号 */
    char    szPosDate[8+1];                 /* POS交易日期 */
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：交易预处理
** 输入参数：
**        ptApp           app结构
** 输出参数：
**        ptApp           app结构
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/09
** 调用说明：
**
** 修改日志：
****************************************************************/
int PurchasePreTreat(T_App *ptApp)
{
    /* 预登记流水 */
    if(PreInsertPosls(ptApp) != SUCC)
    {
        return FAIL;
    }

    return SUCC;
}

/****************************************************************
** 功    能：交易后处理
** 输入参数：
**        ptApp           app结构
** 输出参数：
**        ptApp           app结构
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/09
** 调用说明：
**
** 修改日志：
****************************************************************/
int PurchasePostTreat(T_App *ptApp)
{
    /* 更新流水信息 */

    memset(szRetriRefNum, 0, sizeof(szRetriRefNum));
    memset(szReturnCode, 0, sizeof(szReturnCode));
    memset(szHostRetCode, 0, sizeof(szHostRetCode));
    memset(szHostRetMsg, 0, sizeof(szHostRetMsg));
    memset(szAuthCode, 0, sizeof(szAuthCode));
    memset(szHostDate, 0, sizeof(szHostDate));
    memset(szHostTime, 0, sizeof(szHostTime));
    memset(szSettleDate, 0, sizeof(szSettleDate));
    memset(szBankID, 0, sizeof(szBankID));
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));
    memset(szPan, 0, sizeof(szPan));
    memset(szPosDate, 0, sizeof(szPosDate));

    /* 参数赋值 */
    strcpy(szRetriRefNum, ptApp->szRetriRefNum);
    strcpy(szReturnCode, ptApp->szRetCode);
    strcpy(szHostRetCode, ptApp->szHostRetCode);
    strcpy(szHostRetMsg, ptApp->szHostRetMsg);
    strcpy(szAuthCode, ptApp->szAuthCode);
    if(strlen(ptApp->szHostDate) == 8)
    {
        strcpy(szHostDate, ptApp->szHostDate);
        strcpy(szHostTime, ptApp->szHostTime);
    }
    else
    {
        GetSysDate(szHostDate);
        GetSysTime(szHostTime);
    }
    if(strlen(ptApp->szSettleDate) == 8)
    {
        strcpy(szSettleDate, ptApp->szSettleDate);
    }
    else
    {
        strcpy(szSettleDate, szHostDate);
    }
    strcpy(szBankID, ptApp->szAcqBankId);
    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);
    strcpy(szPan, ptApp->szPan);
    strcpy(szPosDate, ptApp->szPosDate);

    iPosTrace = ptApp->lPosTrace;
    iBatchNo = ptApp->lBatchNo;

    EXEC SQL
        UPDATE posls
        SET retri_ref_num = :szRetriRefNum, return_code = :szReturnCode,
            host_ret_code = :szHostRetCode, host_ret_msg = :szHostRetMsg,
            auth_code = :szAuthCode, host_date = :szHostDate, host_time = :szHostTime,
            settle_date = :szSettleDate, batch_no = :iBatchNo, bank_id = :szBankID
        WHERE shop_no = :szShopNo AND pos_no = :szPosNo AND
              pos_trace = :iPosTrace AND pos_date = :szPosDate AND
              pan = :szPan AND recover_flag = 'N' AND pos_settle = 'N';
    if(SQLCODE)
    {
        RollbackTran();

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "更新流水 商户[%s] 终端[%s] POS流水[%d] POS交易日期[%s] 卡号:[%s] 失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, szPosNo, iPosTrace, szPosDate, szPan, SQLCODE, SQLERR);

        return FAIL;
    }

    CommitTran();

    return SUCC;
}
