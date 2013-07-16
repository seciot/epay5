/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 商户、终端合法性检查
** 创 建 人：冯炜
** 创建日期：2012-11-13
**
** $Revision: 1.2 $
** $Log: ChkValid.ec,v $
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
** 功    能：商户、终端合法性检查
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
**        2012/11/13
** 调用说明：
**
** 修改日志：
****************************************************************/
int ChkValid(T_App *ptApp)
{
    /* 检查商户合法性 */
    if(ChkShopValid(ptApp) != SUCC)
    {
        return FAIL;
    }

    /* 检查终端合法性 */
    if(ChkPosValid(ptApp) != SUCC)
    {
        return FAIL;
    }

    /* 检查电话号码合法性 */
    if(ChkTeleValid(ptApp) != SUCC)
    {
        return FAIL;
    }

    /* 公共合法性检查 */
    /* 平台交易限额设置为10亿 */
	if(memcmp(ptApp->szAmount, "100000000000", 12) > 0)
	{
        strcpy(ptApp->szRetCode, ERR_EXCEED_SINGLE);

		WriteLog(ERROR, "交易金额[%s]过大", ptApp->szAmount);

		return FAIL;
	}

	/* 转出卡与转入卡相同 */
    if(strlen(ptApp->szPan) > 0 && strcmp(ptApp->szPan, ptApp->szAccount2) == 0)
    {
        strcpy(ptApp->szRetCode, ERR_ONE_CARD);

		WriteLog(ERROR, "转出卡[%s]与转入卡[%s]相同", ptApp->szPan, ptApp->szAccount2);

		return FAIL;
    }

    return SUCC;
}
