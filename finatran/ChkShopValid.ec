/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 商户合法性检查
** 创 建 人：冯炜
** 创建日期：2012-11-12
**
** $Revision: 1.5 $
** $Log: ChkShopValid.ec,v $
** Revision 1.5  2013/02/25 01:13:15  fengw
**
** 1、增加字符串末尾空格删除代码。
**
** Revision 1.4  2013/01/16 02:24:45  fengw
**
** 1、增加机构层级信息读取。
**
** Revision 1.3  2012/12/25 08:32:40  wukj
** 商户类型mcc_code加上NVL
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
** 功    能：商户合法性检查
** 输入参数：
**        ptApp->szShopNo       商户号
** 输出参数：
**        ptApp->szShopType     商户类型
**        ptApp->szShopName     商户名称
**        ptApp->szAcqBankId    收单行行号
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/12
** 调用说明：
**
** 修改日志：
****************************************************************/
int ChkShopValid(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[15+1];                 /* 商户号 */
        int     iMarketNo;                      /* 市场类别 */
        char    szShopName[40+1];               /* 商户名称 */
        char    szAcqBankID[11+1];              /* 收单行 */
        char    szMCCCode[4+1];                 /* 商户类型 */
        char    szIsBlack[1+1];                 /* 黑名单标志 */
        char    szDeptDetail[70+1];             /* 机构层级信息 */
        int     iSignFlag;                      /* 签约标志 */
    EXEC SQL END DECLARE SECTION;

    /* 读取商户资料 */
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szShopName, 0, sizeof(szShopName));
    memset(szAcqBankID, 0, sizeof(szAcqBankID));
    memset(szMCCCode, 0, sizeof(szMCCCode));
    memset(szIsBlack, 0, sizeof(szIsBlack));
    memset(szDeptDetail, 0, sizeof(szDeptDetail));

    strcpy(szShopNo, ptApp->szShopNo);

	EXEC SQL
        SELECT NVL(market_no, 0), shop_name, acq_bank, sign_flag, is_black, dept_detail, NVL(mcc_code,' ')
        INTO :iMarketNo, :szShopName, :szAcqBankID, :iSignFlag, :szIsBlack, :szDeptDetail, :szMCCCode
        FROM shop
        WHERE shop_no = :szShopNo;
	if(SQLCODE == SQL_NO_RECORD)
	{
        strcpy(ptApp->szRetCode, ERR_INVALID_MERCHANT);

		WriteLog(ERROR, "商户资料未登记!商户号:[%s]", szShopNo);

		return FAIL;
	}
	else if(SQLCODE)
	{
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

		WriteLog(ERROR, "查询商户资料 商户号:[%s] 失败!SQLCODE=%d SQLERR=%s", szShopNo, SQLCODE, SQLERR);

		return FAIL;
	}

    /* 检查商户状态 */
	if(szIsBlack[0] == '1' || iSignFlag == 1)
	{
        strcpy(ptApp->szRetCode, ERR_SHOP_STATUS);

        WriteLog(ERROR, "商户非正常状态!签约状态[%d] 黑名单标志[%c]", iSignFlag, szIsBlack[0]);

        return FAIL;
	}

    /* 保存商户信息 */
    DelTailSpace(szShopName);
    DelTailSpace(szDeptDetail);

	strcpy(ptApp->szShopType, szMCCCode);
	strcpy(ptApp->szShopName, szShopName);
	strcpy(ptApp->szAcqBankId, szAcqBankID);
    strcpy(ptApp->szDeptDetail, szDeptDetail);

    return SUCC;
}
