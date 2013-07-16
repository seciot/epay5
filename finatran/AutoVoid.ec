/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 冲正交易
** 创 建 人：冯炜
** 创建日期：2012-11-09
**
** $Revision: 1.4 $
** $Log: AutoVoid.ec,v $
** Revision 1.4  2013/03/22 05:33:18  fengw
**
** 1、修正冲正交易业务逻辑处理BUG。
**
** Revision 1.3  2013/03/11 07:09:03  fengw
**
** 1、修改BcdToAsc函数参数。
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
    char    szRetriRefNum[12+1];            /* 后台检索参考号 */
    char    szOldRetriRefNum[12+1];         /* 原后台检索参考号 */
    char    szReturnCode[2+1];              /* 平台返回码 */
    char    szHostRetCode[6+1];             /* 后台返回码 */
    char    szHostRetMsg[40+1];             /* 后台返回错误信息 */
    char    szAuthCode[6+1];                /* 授权码 */
    char    szHostDate[8+1];                /* 平台交易日期 */
    char    szHostTime[6+1];                /* 平台交易时间 */
    char    szSettleDate[8+1];              /* 结算日期 */
    int     iBatchNo;                       /* 批次号 */
    double  dAmount;                        /* 交易金额 */
    char    szBankID[11+1];                 /* 银行标识号 */
    char    szShopNo[15+1];                 /* 商户号 */
    char    szPosNo[15+1];                  /* 终端号 */
    int     iPosTrace;                      /* 终端流水号 */
    int     iOldPosTrace;                   /* 原终端流水号 */
    int     iSysTrace;                      /* 平台流水号 */
    int     iTransType;                     /* 交易类型 */
    char    szPan[19+1];                    /* 转出卡号 */
    char    szAccount2[19+1];               /* 转入卡号 */
    char    szPosDate[8+1];                 /* POS交易日期 */
    char    szCancelFlag[1+1];              /* 撤销标志 */
    char    szRecoverFlag[1+1];             /* 冲正标志 */
    char    szPosSettle[1+1];               /* 结算标志 */
    char    szBusinessCode[40+1];           /* 商务应用号 */
    char    szCardType[1+1];                /* 卡类型 */
    char    szPsamNo[16+1];                 /* 安全模块号 */
    char    szMAC[16+1];                    /* MAC */
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
int AutoVoidPreTreat(T_App *ptApp)
{
    long long llAmt;

    /* 参数赋值 */
    memset(szPsamNo, 0, sizeof(szPsamNo));
    memset(szMAC, 0, sizeof(szMAC));

    strcpy(szPsamNo, ptApp->szPsamNo);
    BcdToAsc(ptApp->szMac, 16, LEFT_ALIGN, szMAC);
    iPosTrace = ptApp->lOldPosTrace;

    /* 查询原流水记录 */
    memset(szRetriRefNum, 0, sizeof(szRetriRefNum));
    memset(szOldRetriRefNum, 0, sizeof(szOldRetriRefNum));
    memset(szAuthCode, 0, sizeof(szAuthCode));
    memset(szHostDate, 0, sizeof(szHostDate));
    memset(szHostTime, 0, sizeof(szHostTime));
    memset(szAccount2, 0, sizeof(szAccount2));
    memset(szPan, 0, sizeof(szPan));
    memset(szReturnCode, 0, sizeof(szReturnCode));
    memset(szCancelFlag, 0, sizeof(szCancelFlag));
    memset(szRecoverFlag, 0, sizeof(szRecoverFlag));
    memset(szPosSettle, 0, sizeof(szPosSettle));
    memset(szBusinessCode, 0, sizeof(szBusinessCode));
    memset(szCardType, 0, sizeof(szCardType));
    memset(szBankID, 0, sizeof(szBankID));

    EXEC SQL
        SELECT retri_ref_num, old_retri_ref_num, sys_trace, auth_code, amount,
               host_time, host_date, trans_type, account2, pan, return_code,
               cancel_flag, recover_flag, pos_settle, business_code, card_type, bank_id
        INTO :szRetriRefNum, :szOldRetriRefNum, :iSysTrace, :szAuthCode, :dAmount,
             :szHostDate, :szHostTime, :iTransType, :szAccount2, :szPan, :szReturnCode,
             :szCancelFlag, :szRecoverFlag, :szPosSettle, :szBusinessCode, :szCardType, :szBankID
        FROM posls
        WHERE psam_no = :szPsamNo AND pos_trace = :iPosTrace AND mac = :szMAC AND
              recover_flag = 'N';
    if(SQLCODE == SQL_NO_RECORD)
    {
        /* 退出交易处理，应答码设置为成功，返回POS冲正成功 */
        strcpy(ptApp->szRetCode, TRANS_SUCC);
        strcpy(ptApp->szHostRetCode, TRANS_SUCC);

        WriteLog(TRACE, "原交易流水 psam_no:[%s] Trace:[%ld] Mac:[%s]不存在或已冲正，无需冲正，返回成功",
                 szPsamNo, iPosTrace, szMAC);

        return FAIL;
    }
    else if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询原交易流水 psam_no:[%s] Trace:[%ld] Mac:[%s]失败!SQLCODE=%d SQLERR=%s",
                 szPsamNo, iPosTrace, szMAC,  SQLCODE, SQLERR);

        return FAIL;
    }

    /* 判断原交易状态是否为NN */
    if(memcmp(szReturnCode, "NN", 2) == 0)
    {
        /* 如果冲正交易发起方为终端，登记冲正流水，返回成功 */
        if(memcmp(ptApp->szSourceTpdu, "\xFF\xFF", 2) != 0 &&
           memcmp(ptApp->szTargetTpdu, "\xFF\xFF", 2) != 0)
        {
            /* 登记冲正流水 */
            InsertVoidls(ptApp);

            strcpy(ptApp->szRetCode, TRANS_SUCC);

            return FAIL;
        }
        /* 如果冲正交易发起方为平台，返回失败，等待下次冲正 */
        else
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
            WriteLog(TRACE, "return_code[NN] posp_trans[%d]",iPosTrace);

            return FAIL;
        }
    }
    /* 如果状态不为以下情况，不需要冲正，返回成功 */
    else if(memcmp(szReturnCode, "00", 2) != 0 &&
            memcmp(szReturnCode, "68", 2) != 0 &&
            memcmp(szReturnCode, "TO", 2) != 0 &&
            memcmp(szReturnCode, "Q9", 2) != 0)
    {
        /* 无需冲正 */
        strcpy(ptApp->szRetCode, TRANS_SUCC);
        strcpy(ptApp->szHostRetCode, TRANS_SUCC);
WriteLog(ERROR, "not need void");
        return FAIL;
    }

    /* 结果赋值 */
    DelTailSpace(szRetriRefNum);
    DelTailSpace(szOldRetriRefNum);
    DelTailSpace(szAuthCode);
    DelTailSpace(szPan);
    DelTailSpace(szBankID);
    DelTailSpace(szAccount2);
    DelTailSpace(szHostDate);
    DelTailSpace(szHostTime);
    DelTailSpace(szBusinessCode);

    /* 交易参考号 */
    strcpy(ptApp->szRetriRefNum, szRetriRefNum);

    /* 原交易参考号 */
    strcpy(ptApp->szOldRetriRefNum, szOldRetriRefNum);

    /* 授权码 */
    strcpy(ptApp->szAuthCode, szAuthCode);

    /* 交易类型 */
    ptApp->iOldTransType = iTransType;

    /* 卡类型 */
    ptApp->cOutCardType = szCardType[0];

    /* 转出卡号 */
    strcpy(ptApp->szPan, szPan);

    /* 银行标识 */
    strcpy(ptApp->szAcqBankId, szBankID);

    /* 转入账号 */
    strcpy(ptApp->szAccount2, szAccount2);

    /* 交易金额 */
    memset(ptApp->szAmount, 0, sizeof(ptApp->szAmount));
    sprintf(ptApp->szAmount, "%012ld", (long long)(dAmount*100.0+0.5));

    /* 主机交易日期、时间 */
    strcpy(ptApp->szHostDate, szHostDate);
    strcpy(ptApp->szHostTime, szHostTime);

    /* 原系统流水号 */
    ptApp->lOldSysTrace = iSysTrace;

    /* 冲正使用原有的的流水号 */
    ptApp->lSysTrace = iSysTrace;

    /* 商务应用号 */
    strcpy(ptApp->szBusinessCode, szBusinessCode);

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
int AutoVoidPostTreat(T_App *ptApp)
{
    /* 更新流水信息 */

    memset(szOldRetriRefNum, 0, sizeof(szOldRetriRefNum));
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
    strcpy(szOldRetriRefNum, ptApp->szOldRetriRefNum);
    strcpy(szRetriRefNum, ptApp->szRetriRefNum);
    strcpy(szReturnCode, ptApp->szRetCode);
    strcpy(szHostRetCode, ptApp->szHostRetCode);
    strcpy(szHostRetMsg, ptApp->szHostRetMsg);
    strcpy(szAuthCode, ptApp->szAuthCode);
    strcpy(szHostDate, ptApp->szHostDate);
    strcpy(szHostTime, ptApp->szHostTime);
    strcpy(szSettleDate, ptApp->szSettleDate);
    strcpy(szBankID, ptApp->szAcqBankId);
    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);
    strcpy(szPan, ptApp->szPan);
    strcpy(szPosDate, ptApp->szPosDate);

    iOldPosTrace = ptApp->lOldPosTrace;
    iPosTrace = ptApp->lPosTrace;
    iBatchNo = ptApp->lBatchNo;

    /* 事务处理 */
    BeginTran();

    /* 更新原交易流水撤销标志、冲正标志 */
    if(memcmp(ptApp->szRetCode, TRANS_SUCC, 2) == 0)
    {
        if(ptApp->iOldTransType == PUR_CANCEL ||
           ptApp->iOldTransType == PRE_CANCEL ||
           ptApp->iOldTransType == CON_CANCEL ||
           ptApp->iOldTransType == TRAN_CANCEL ||
           ptApp->iOldTransType == TRAN_OUT_CANCEL ||
           ptApp->iOldTransType == TRAN_IN_CANCEL)
        {
            EXEC SQL
                UPDATE posls SET cancel_flag = 'N'
                WHERE shop_no = :szShopNo AND pos_no = :szPosNo AND
                    retri_ref_num = :szOldRetriRefNum AND pan = :szPan AND
                    pos_date = :szPosDate AND recover_flag = 'N' AND pos_settle = 'N';
            if(SQLCODE)
            {
                RollbackTran();

                strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

                WriteLog(ERROR, "更新原撤销流水 商户[%s] 终端[%s] 原系统参考号[%s] 卡号:[%s] POS交易日期[%s] 失败!SQLCODE=%d SQLERR=%s",
                         szShopNo, szPosNo, szOldRetriRefNum, szPan, szPosDate, SQLCODE, SQLERR);

                return FAIL;
            }
        }

		EXEC SQL
		    UPDATE posls SET recover_flag = 'Y'
    	 	WHERE psam_no = :szPsamNo AND pos_trace = :iOldPosTrace AND
                  mac = :szMAC AND recover_flag = 'N';
        if(SQLCODE)
        {
            RollbackTran();

            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "更新流水 安全模块号[%s] 原POS流水[%d] MAC:[%s] 失败!SQLCODE=%d SQLERR=%s",
                     szPsamNo, iOldPosTrace, szMAC, SQLCODE, SQLERR);

            return FAIL;
        }

        /* 提交更新 */
        CommitTran();
    }

    return SUCC;
}
