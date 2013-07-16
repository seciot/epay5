/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 跨行转账前查询交易
** 创 建 人：冯炜
** 创建日期：2013-02-20
**
** $Revision: 1.1 $
** $Log: TFOtherQuery.ec,v $
** Revision 1.1  2013/02/21 06:33:45  fengw
**
** 1、增加跨行汇款查询、跨行汇款交易。
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    char    szShopNo[15+1];                 /* 商户号 */
    char    szPosNo[15+1];                  /* 终端号 */
    char    szPan[19+1];                    /* 卡号 */
    char    szAcctName[40+1];               /* 持卡人姓名 */
    char    szExpireDate[4+1];              /* 有效期 */
    char    szBankID[12+1];                 /* 银行联行行号 */
    char    szBankName[40+1];               /* 银行名称 */
    char    szRegisterDate[8+1];            /* 登记时间 */
    int     iRecNo;                         /* 记录编号 */
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
int TranOtherQueryPreTreat(T_App *ptApp)
{
    /* 获取收款人账户信息 */
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));
    memset(szPan, 0, sizeof(szPan));

    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);
    strcpy(szPan, ptApp->szStaticMenuOut);

    memset(szAcctName, 0, sizeof(szAcctName));
    memset(szExpireDate, 0, sizeof(szExpireDate));
    memset(szBankID, 0, sizeof(szBankID));
    memset(szBankName, 0, sizeof(szBankName));
    memset(szRegisterDate, 0, sizeof(szRegisterDate));

    EXEC SQL
        SELECT acct_name, expire_date, bank_id,
        bank_name, register_date, rec_no
        INTO :szAcctName, :szExpireDate, :szBankID,
        :szBankName, :szRegisterDate, :iRecNo
        FROM my_customer WHERE shop_no = :szShopNo AND
        pos_no = :szPosNo AND pan = :szPan;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询 商户号[%s] 终端号[%s] 卡号[%s] 收款人信息失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, szPosNo, szPan, SQLCODE, SQLERR);

        return FAIL;
	}

    DelTailSpace(szExpireDate);
    DelTailSpace(szAcctName);
    DelTailSpace(szBankID);
    DelTailSpace(szBankName);

    /* 结果赋值 */
    /* 收款人账号 */
    memset(ptApp->szAccount2, 0, sizeof(ptApp->szAccount2));
    strcpy(ptApp->szAccount2, szPan);

    /* 银行名称 */
    memset(ptApp->szInBankName, 0, sizeof(ptApp->szInBankName));
    strcpy(ptApp->szInBankName, szBankName);

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
**        2013/02/20
** 调用说明：
**
** 修改日志：
****************************************************************/
int TranOtherQueryPostTreat(T_App *ptApp)
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