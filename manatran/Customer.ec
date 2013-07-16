/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台 收款人管理
** 创 建 人：冯炜
** 创建日期：2013-02-19
**
** $Revision: 1.1 $
** $Log: Customer.ec,v $
** Revision 1.1  2013/02/21 06:24:14  fengw
**
** 1、增加收款人管理交易。
**
*******************************************************************/
#include "manatran.h"

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
    int     iRecCount;                      /* 记录个数 */
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：增加收款人信息
** 输入参数：
**        ptApp            app结构指针
** 输出参数：
**        无
** 返 回 值：
**        SUCC             参数检查成功
**        FAIL             参数检查失败
** 作    者：
**        fengwei
** 日    期：
**        2013/02/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int AddCustomer(T_App *ptApp) 
{
    T_TLVStru   tTlv;               /* TLV数据结构 */
    char        szTmpBuf[128+1];    /* 临时变量 */

    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));

    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);

    /* 查询我的收款人信息 */
    EXEC SQL
        SELECT COUNT(*), NVL(MAX(rec_no), 0)+1 INTO :iRecCount, :iRecNo
        FROM my_customer
        WHERE shop_no = :szShopNo AND pos_no = :szPosNo;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询 商户号[%s] 终端号[%s] 收款人信息失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, szPosNo, SQLCODE, SQLERR);

        return FAIL;
    }

    /* 最大支持9组收款人信息 */
    if(iRecCount >= MAX_STATIC_MENU_COUNT)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "商户号[%s] 终端号[%s] 收款人信息个数[%d]超限!",
                 szShopNo, szPosNo, iRecCount);

        return FAIL;
    }

    /* TLV初始化 */
    InitTLV(&tTlv, TAG_STANDARD, LEN_STANDARD, VALUE_NORMAL);

    if(UnpackTLV(&tTlv, ptApp->szReserved, ptApp->iReservedLen) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "字符串转换为TLV数据格式失败!");

        return FAIL;
    }

    memset(szPan, 0, sizeof(szPan));
    memset(szAcctName, 0, sizeof(szAcctName));
    memset(szExpireDate, 0, sizeof(szExpireDate));
    memset(szBankID, 0, sizeof(szBankID));
    memset(szBankName, 0, sizeof(szBankName));
    memset(szRegisterDate, 0, sizeof(szRegisterDate));

    strcpy(szPan, ptApp->szAccount2);
    strcpy(szAcctName, ptApp->szUserData);
    strcpy(szExpireDate, ptApp->szBusinessCode);

    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    if(GetValueByTag(&tTlv, "\xDF\x89\x02", szTmpBuf, sizeof(szTmpBuf)) != 13)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "获取联行行号数据失败!");

        return FAIL;
    }
    memcpy(szBankID, szTmpBuf+1, 12);

    strcpy(szBankName, ptApp->szInBankName);
    GetSysDate(szRegisterDate);

    BeginTran();

    EXEC SQL
        INSERT INTO my_customer
        (shop_no, pos_no, pan, acct_name, expire_date,
        bank_id, bank_name, register_date, rec_no)
        VALUES
        (:szShopNo, :szPosNo, :szPan, :szAcctName,
        :szExpireDate, :szBankID, :szBankName, :szRegisterDate, :iRecNo);
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "插入 商户号[%s] 终端号[%s] 收款人信息失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, szPosNo, SQLCODE, SQLERR);

        return FAIL;
    }

    CommitTran();

    strcpy(ptApp->szRetCode, TRANS_SUCC);

    return SUCC;
}

/****************************************************************
** 功    能：删除收款人信息
** 输入参数：
**        ptApp            app结构指针
** 输出参数：
**        无
** 返 回 值：
**        SUCC             参数检查成功
**        FAIL             参数检查失败
** 作    者：
**        fengwei
** 日    期：
**        2013/02/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int DelCustomer(T_App *ptApp) 
{
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));
    memset(szPan, 0, sizeof(szPan));

    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);
    strcpy(szPan, ptApp->szAccount2);

    /* 删除我的收款人信息 */

    BeginTran();

    EXEC SQL
        DELETE FROM my_customer
        WHERE shop_no = :szShopNo AND pos_no = :szPosNo AND pan = :szPan;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "删除 商户号[%s] 终端号[%s] 收款卡号[%s] 收款人信息失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, szPosNo, szPan, SQLCODE, SQLERR);

        return FAIL;
    }

    CommitTran();

    strcpy(ptApp->szRetCode, TRANS_SUCC);

    return SUCC;
}