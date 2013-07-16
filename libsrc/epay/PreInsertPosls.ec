/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 登记交易流水
** 创 建 人：冯炜
** 创建日期：2012-12-04
**
** $Revision: 1.6 $
** $Log: PreInsertPosls.ec,v $
** Revision 1.6  2013/06/28 01:20:47  fengw
**
** 1、预登记POS交易流水，初始化主机交易日期、主机交易时间、结算日期为系统日期、时间。
**
** Revision 1.5  2013/01/16 02:22:25  fengw
**
** 1、修改交易金额转换代码。
** 2、新增流水记录字段。
**
** Revision 1.4  2012/12/28 03:36:30  fengw
**
** 1、数据库表增加dept_detail字段。
**
** Revision 1.3  2012/12/20 09:25:54  wukj
** Revision后的美元符
**
*******************************************************************/

#include <stdio.h>
#include <string.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：登记交易流水
** 输入参数：
**        ptApp           app结构
** 输出参数：
**        无
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/04
** 调用说明：
**
** 修改日志：
****************************************************************/
int PreInsertPosls(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[15+1];                 /* 商户号 */
        char    szPosNo[15+1];                  /* 终端号 */
        int     iPosTrace;                      /* 终端流水号 */
        char    szPsamNo[16+1];                 /* 安全模块号 */
        int     iBatchNo;                       /* 批次号 */
        int     iSysTrace;                      /* 平台流水号 */
        int     iTransType;                     /* 交易类型 */
        int     iBusinessType;                  /* 业务类型 */
        char    szPan[19+1];                    /* 转出卡号 */
        char    szCardType[1+1];                /* 卡类型 */
        double  dAmount;                        /* 交易金额 */
        double  dAddiAmount;                    /* 手续费金额 */
        char    szAccount2[19+1];               /* 转入卡号 */
        char    szPosDate[8+1];                 /* POS交易日期 */
        char    szPosTime[6+1];                 /* POS交易时间 */
        char    szHostDate[8+1];                /* 主机交易日期 */
        char    szHostTime[6+1];                /* 主机交易时间 */
        char    szSettleDate[8+1];              /* 结算日期 */
        char    szFinancialCode[40+1];          /* 金融应用号 */
        char    szBusinessCode[40+1];           /* 商户应用号 */
        char    szAcqBankId[11+1];              /* 收单行行号 */
        char    szReturnCode[2+1];              /* 平台返回码 */
        char    szCancelFlag[1+1];              /* 撤销标志 */
        char    szRecoverFlag[1+1];             /* 冲正标志 */
        char    szPosSettle[1+1];               /* 结算标志 */
        char    szMac[16+1];                    /* MAC */
        char    szDeptDetail[70+1];             /* 机构层级信息 */
    EXEC SQL END DECLARE SECTION;

    /* 参数赋值 */
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));
    memset(szPsamNo, 0, sizeof(szPsamNo));
    memset(szPan, 0, sizeof(szPan));
    memset(szCardType, 0, sizeof(szCardType));
    memset(szAccount2, 0, sizeof(szAccount2));
    memset(szPosDate, 0, sizeof(szPosDate));
    memset(szPosTime, 0, sizeof(szPosTime));
    memset(szHostDate, 0, sizeof(szHostDate));
    memset(szHostTime, 0, sizeof(szHostTime));
    memset(szSettleDate, 0, sizeof(szSettleDate));
    memset(szFinancialCode, 0, sizeof(szFinancialCode));
    memset(szBusinessCode, 0, sizeof(szBusinessCode));
    memset(szAcqBankId, 0, sizeof(szAcqBankId));
    memset(szReturnCode, 0, sizeof(szReturnCode));
    memset(szCancelFlag, 0, sizeof(szCancelFlag));
    memset(szRecoverFlag, 0, sizeof(szRecoverFlag));
    memset(szPosSettle, 0, sizeof(szPosSettle));
    memset(szMac, 0, sizeof(szMac));
    memset(szDeptDetail, 0, sizeof(szDeptDetail));

    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);
    strcpy(szPsamNo, ptApp->szPsamNo);
    iPosTrace = ptApp->lPosTrace;
    iBatchNo = ptApp->lBatchNo;
    iSysTrace = ptApp->lSysTrace;
    iTransType = ptApp->iTransType;
    iBusinessType = ptApp->iBusinessType;
    strcpy(szPosDate, ptApp->szPosDate);
    strcpy(szPosTime, ptApp->szPosTime);

    if(strlen(ptApp->szHostDate) == 8)
    {
        strcpy(szHostDate, ptApp->szHostDate);
    }
    else
    {
        GetSysDate(szHostDate);
    }

    if(strlen(ptApp->szHostTime) == 6)
    {
        strcpy(szHostTime, ptApp->szHostTime);
    }
    else
    {
        GetSysTime(szHostTime);
    }
    
    if(strlen(ptApp->szSettleDate) == 8)
    {
        strcpy(szSettleDate, ptApp->szSettleDate);
    }
    else
    {
        GetSysDate(szSettleDate);
    }

    strcpy(szFinancialCode, ptApp->szFinancialCode);
    strcpy(szBusinessCode, ptApp->szBusinessCode);
    strcpy(szAcqBankId, ptApp->szAcqBankId);
    strcpy(szPan, ptApp->szPan);
    szCardType[0] = ptApp->cOutCardType;
    dAmount = atoll(ptApp->szAmount) / 100.00f;
    dAddiAmount = atoll(ptApp->szAddiAmount) / 100.00f;
    strcpy(szAccount2, ptApp->szAccount2);
    strcpy(szReturnCode, "NN");
    szCancelFlag[0] = 'N';
    szRecoverFlag[0] = 'N';
    szPosSettle[0] = 'N';
    BcdToAsc(ptApp->szMac, 16, 0, szMac);
    strcpy(szDeptDetail, ptApp->szDeptDetail);

    BeginTran();

    EXEC SQL
        INSERT INTO posls (shop_no, pos_no, psam_no, pos_trace, batch_no, sys_trace,
                           trans_type, business_type, pan, card_type, amount, account2,
                           pos_date, pos_time, financial_code, business_code, bank_id,
                           return_code, cancel_flag, recover_flag, pos_settle, mac, dept_detail)
        VALUES(:szShopNo, :szPosNo, :szPsamNo, :iPosTrace, :iBatchNo, :iSysTrace,
               :iTransType, :iBusinessType, :szPan, :szCardType, :dAmount, :szAccount2,
               :szPosDate, :szPosTime, :szFinancialCode, :szBusinessCode, :szAcqBankId,
               :szReturnCode, :szCancelFlag, :szRecoverFlag, :szPosSettle, :szMac, :szDeptDetail);
    if(SQLCODE)
    {
        WriteLog(ERROR, "插入POS流水记录失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        RollbackTran();

        return FAIL;
    }

    CommitTran();

    return SUCC;
}
