/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 交易参数检查
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.4 $
** $Log: ChkEpayConf.ec,v $
** Revision 1.4  2013/03/06 01:19:51  fengw
**
** 1、统计当日交易累计金额、笔数时增加pos_date条件。
**
** Revision 1.3  2013/02/21 06:36:11  fengw
**
** 1、增加结构体处理化语句。
**
** Revision 1.2  2012/12/04 01:24:28  fengw
**
** 1、替换ErrorLog为WriteLog。
**
** Revision 1.1  2012/11/23 09:09:16  fengw
**
** 金融交易处理模块初始版本
**
** Revision 1.2  2012/11/22 08:59:27  fengw
**
** 1、修改交易限额检查部分代码
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
** 功    能：获取终端参数定义
** 输入参数：
**        ptApp            app结构指针
** 输出参数：
**        giFeeCalcType    手续费计算方式
** 返 回 值：
**        SUCC             参数检查成功
**        FAIL             参数检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/08
** 调用说明：
**
** 修改日志：
****************************************************************/
int ChkEpayConf(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        int     iBusinessType;                 /* 业务类型 */
        char    szPosDate[8+1];                /* 交易日期 */
        double  dAmountTraceSum;               /* 当日累计交易金额 */
        int     iTraceCount;                   /* 当日累计交易笔数 */
    EXEC SQL END DECLARE SECTION;

    int        iRet;            /* 函数返回值 */
    double     dAmount;         /* 交易金额 */
    T_EpayConf tEpayConf;       /* 交易参数结构体 */

    /* 业务类型 */
    iBusinessType = ptApp->iBusinessType;

    /* 交易日期 */
    memset(szPosDate, 0, sizeof(szPosDate));
    strcpy(szPosDate, ptApp->szPosDate);

    /* 交易金额 */
    dAmount = atol(ptApp->szAmount)/100.0;

    memset(&tEpayConf, 0, sizeof(tEpayConf));

    iRet = GetEpayConf(ptApp, &tEpayConf);
    if(iRet == CONF_NOT_FOUND)
    {
        /* 没有配置参数，返回检查成功 */
        return SUCC;
    }
    else if(iRet == CONF_GET_FAIL)
    {
        /* 获取参数时失败，返回检查失败 */
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    /* 逐项检查 */

    /* 单笔限额检查 */
    if(tEpayConf.dAmountSingle > 0.001 && dAmount - tEpayConf.dAmountSingle > 0.001)
    {
        strcpy(ptApp->szRetCode, ERR_EXCEED_SINGLE);

        WriteLog(ERROR, "单笔交易限额检查失败!交易金额:[%.2f] 单笔限额:[%.2f]",
                 dAmount, tEpayConf.dAmountSingle);

        return FAIL;
    }

    /* 当日累计限额检查，当日交易笔数检查 */
    if(tEpayConf.dAmountSum > 0.001 || tEpayConf.iMaxCount > 0)
    {
        EXEC SQL
            SELECT NVL(SUM(amount), 0), NVL(COUNT(amount), 0) INTO
                :dAmountTraceSum, :iTraceCount
            FROM posls
            WHERE business_type = :iBusinessType AND pos_date = :szPosDate AND return_code = '00'
                  AND cancel_flag = 'N' AND recover_flag = 'N';
        if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "统计当日交易类型[%d]累计交易金额失败!SQLCODE=%d SQLERR=%s",
                     iBusinessType, SQLCODE, SQLERR);

            return FAIL;
        }

        if(tEpayConf.dAmountSum > 0.001 && (dAmountTraceSum + dAmount - tEpayConf.dAmountSum) > 0.001)
        {
            strcpy(ptApp->szRetCode, ERR_EXCEED_TOTAL);

            WriteLog(ERROR, "累计交易限额检查失败!交易金额:[%.2f] 累计限额:[%.2f] 当日累计交易额:[%.2f]",
                     dAmount, tEpayConf.dAmountSum, dAmountTraceSum);

            return FAIL;
        }

        if(tEpayConf.iMaxCount > 0 && (iTraceCount + 1) > tEpayConf.iMaxCount)
        {
            strcpy(ptApp->szRetCode, ERR_EXCEED_TIMES);

            WriteLog(ERROR, "当日交易笔数限制检查失败!当日累计交易笔数:[%d] 当日笔数限制:[%d]",
                     iTraceCount, tEpayConf.iMaxCount);

            return FAIL;
        }
    }

    /* 信用卡类检查 */
    if(ptApp->cOutCardType == CREDIT_CARD)
    {
        strcpy(ptApp->szRetCode, ERR_EXCEED_SINGLE);

        /* 单笔限额检查 */
        if(tEpayConf.dCreditAmountSingle > 0.001 && dAmount - tEpayConf.dCreditAmountSingle > 0.001)
        {
            strcpy(ptApp->szRetCode, ERR_EXCEED_SINGLE);

            WriteLog(ERROR, "信用卡单笔交易限额检查失败!交易金额:[%.2f] 单笔限额:[%.2f]",
                     dAmount, tEpayConf.dCreditAmountSingle);

            return FAIL;
        }

        /* 当日累计限额检查，当日交易笔数检查 */
        if(tEpayConf.dCreditAmountSum > 0.001 || tEpayConf.iCreditMaxCount > 0)
        {
            EXEC SQL
                SELECT NVL(SUM(amount), 0), NVL(COUNT(amount), 0) INTO
                    :dAmountTraceSum, :iTraceCount
                FROM posls
                WHERE business_type = :iBusinessType AND pos_date = :szPosDate AND
                      card_type = '1' AND return_code = '00' AND cancel_flag = 'N'AND recover_flag = 'N';
            if(SQLCODE)
            {
                strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

                WriteLog(ERROR, "统计当日交易类型[%d]累计信用卡交易金额失败!SQLCODE=%d SQLERR=%s",
                         iBusinessType, SQLCODE, SQLERR);

                return FAIL;
            }

            if(tEpayConf.dCreditAmountSum > 0.001 &&
               (dAmountTraceSum + dAmount - tEpayConf.dCreditAmountSum) > 0.001)
            {
                strcpy(ptApp->szRetCode, ERR_EXCEED_TOTAL);

                WriteLog(ERROR, "累计信用卡交易限额检查失败!交易金额:[%.2f] 累计限额:[%.2f] 当日累计交易额:[%.2f]",
                         dAmount, tEpayConf.dCreditAmountSum, dAmountTraceSum);

                return FAIL;
            }

            if(tEpayConf.iCreditMaxCount > 0 && (iTraceCount + 1) > tEpayConf.iCreditMaxCount)
            {
                strcpy(ptApp->szRetCode, ERR_EXCEED_TIMES);

                WriteLog(ERROR, "当日信用卡交易笔数限制检查失败!当日累计交易笔数:[%d] 当日笔数限制:[%d]",
                         iTraceCount, tEpayConf.iCreditMaxCount);

                return FAIL;
            }
        }
    }

    /* 转出卡检查 */
    if(strlen(ptApp->szPan) > 0 &&
       ChkCardType(ptApp->cOutCardType, ptApp->iOutCardBelong, tEpayConf.szCardTypeOut) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_INVALID_CARD);

        WriteLog(ERROR, "转出卡许可卡种检查失败!卡种:[%c] 卡归属:[%d] 许可卡种类型:[%s]",
                 ptApp->cOutCardType, ptApp->iOutCardBelong, tEpayConf.szCardTypeOut);

        return FAIL;
    }

    /* 转入卡检查 */
    if(strlen(ptApp->szAccount2) > 0 &&
       ChkCardType(ptApp->cInCardType, ptApp->iInCardBelong, tEpayConf.szCardTypeIn) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_INVALID_CARD);

        WriteLog(ERROR, "转入卡许可卡种检查失败!卡种:[%c] 卡归属:[%d] 许可卡种类型:[%s]",
                 ptApp->cInCardType, ptApp->iInCardBelong, tEpayConf.szCardTypeIn);

        return FAIL;
    }

    /* 手续费计算方式保存至全局变量，供计算手续费时使用 */
    giFeeCalcType = tEpayConf.iFeeCalcType;

    return SUCC;
}
