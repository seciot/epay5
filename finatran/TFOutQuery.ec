/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 转出转账前查询交易
** 创 建 人：冯炜
** 创建日期：2012-11-14
**
** $Revision: 1.1 $
** $Log: TFOutQuery.ec,v $
** Revision 1.1  2013/02/21 06:34:21  fengw
**
** 1、原转出转账交易更名。
**
** Revision 1.3  2013/01/14 09:17:45  fengw
**
** 1、增加卡号去空格处理。
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
    char    szShopNo[15+1];                 /* 商户号 */
    char    szPosNo[15+1];                  /* 终端号 */
    char    szPan[19+1];                    /* 转出卡号 */
    char    szAcctName[40+1];               /* 持卡人姓名 */
    char    szExpireDate[4+1];              /* 卡有效期 */
    char    szBankName[20+1];               /* 银行名称 */
    int     iStatus;                        /* 状态 */
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
**        2012/11/14
** 调用说明：
**
** 修改日志：
****************************************************************/
int TranOutQueryPreTreat(T_App *ptApp)
{
    /* 获取绑定卡信息 */
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));

    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);

    memset(szPan, 0, sizeof(szPan));
    memset(szAcctName, 0, sizeof(szAcctName));
    memset(szExpireDate, 0, sizeof(szExpireDate));
    memset(szBankName, 0, sizeof(szBankName));

	EXEC SQL
        SELECT pan, acct_name, expire_date, bank_name, status
        INTO :szPan, :szAcctName, :szExpireDate, :szBankName, :iStatus
        FROM register_card
        WHERE shop_no = :szShopNo AND pos_no = :szPosNo AND transtype = 0;
    if(SQLCODE == SQL_NO_RECORD)
	{
        strcpy(ptApp->szRetCode, ERR_OUT_CARD_NOT_REGISTER);

		WriteLog(ERROR, "商户[%s] 终端[%s] 转出绑定卡未登记!", szShopNo, szPosNo);

		return FAIL;
	}
	else if(SQLCODE)
	{
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

		WriteLog(ERROR, "查询商户[%s] 终端[%s] 转出绑定卡失败!SQLCODE=%d SQLERR=%s",
		         szShopNo, szPosNo, SQLCODE, SQLERR);

		return FAIL;
	}

    /* 检查记录状态 */
	if(iStatus != 1)
	{
		strcpy(ptApp->szRetCode, ERR_REG_CARD_NOT_APPROVE);

		WriteLog(ERROR, "商户[%s] 终端[%s] 转出绑定卡未复核", szShopNo, szPosNo);

        return FAIL;
	}

    DelTailSpace(szPan);
    DelTailSpace(szExpireDate);
    DelTailSpace(szBankName);
    DelTailSpace(szAcctName);

    /* 检查绑定卡与终端刷卡信息 */
    if(strcmp(szPan, ptApp->szPan) != 0)
    {
        strcpy(ptApp->szRetCode, ERR_SELF_CARD);

		WriteLog(ERROR, "转出卡[%s] 绑定卡[%s]不匹配", szPan, ptApp->szPan);

        return FAIL;
    }

    /* 结果赋值 */
    /* 卡有效期 */
    memset(ptApp->szExpireDate, 0, sizeof(ptApp->szExpireDate));
    strcpy(ptApp->szExpireDate, szExpireDate);

    /* 银行名称 */
    memset(ptApp->szOutBankName, 0, sizeof(ptApp->szOutBankName));
    strcpy(ptApp->szOutBankName, szBankName);

    /* 持卡人姓名 */
    memset(ptApp->szHolderName, 0, sizeof(ptApp->szHolderName));
    strcpy(ptApp->szHolderName, szAcctName);

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
**        2012/11/14
** 调用说明：
**
** 修改日志：
****************************************************************/
int TranOutQueryPostTreat(T_App *ptApp)
{
    int     iFeeType;           /* 费率种类 */

    /* 交易成功计算手续费 */
    if(memcmp(ptApp->szRetCode, TRANS_SUCC, 2) == 0)
    {
        /* 根据转出卡地区、转入卡地区判断费率种类 */
        /*
        待添加
        */
        iFeeType = 0;

        if(CalcFee(ptApp, iFeeType) != SUCC)
        {
            WriteLog(ERROR, "计算手续费失败!");

            return FAIL;
        }

        /* 回显手续费 */
        /*
        待添加
        */
    }

    return SUCC;
}
