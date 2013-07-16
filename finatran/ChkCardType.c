/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 卡种许可检查
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.4 $
** $Log: ChkCardType.c,v $
** Revision 1.4  2012/12/04 01:24:28  fengw
**
** 1、替换ErrorLog为WriteLog。
**
** Revision 1.3  2012/11/26 01:33:05  fengw
**
** 1、修改文件名后缀为ec，重新上传该模块代码
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

/****************************************************************
** 功    能：检查卡种是否许可
** 输入参数：
**        cCardType             卡类型 '0'-借记卡 '1'-贷记卡 '3'-准贷记卡
**        iOutCardBelong        卡归属 0-本行本地 1-本行异地 2-他行
**        szCardAllowedType     许可卡种类型，每位代表某种卡是否允许交易，1为允许，0为不允许。
**                              第一位：他行借记卡
**                              第二位：他行贷记卡
**                              第三位：他行准贷记卡
**                              第四位：本行借记卡
**                              第五位：本行贷记卡
**                              第六位：本行准贷记卡
**                              第七位：本地借记卡
**                              第八位：本地贷记卡
**                              第九位：本地准贷记卡
** 输出参数：
**        无
** 返 回 值：
**        SUCC                  允许该卡种交易
**        FAIL                  禁止该卡种交易
** 作    者：
**        fengwei
** 日    期：
**        2012/11/08
** 调用说明：
**
** 修改日志：
****************************************************************/
int ChkCardType(char cCardType, int iCardBelong, char *szCardAllowedType)
{
    int iIndex;

    /* 全1表示所有卡都支持，直接返回检查成功 */
    if(memcmp(szCardAllowedType, "111111111", 9) == 0)
    {
        return SUCC;
    }

    /* 根据卡归属及卡类型获取许可标志位索引值 */
    iIndex = 0;

    switch(iCardBelong)
    {
        /* 本行本地 */
        case LOCAL_BANK_LOCAL_CITY:
            iIndex += 6;
            break;
        /* 本行异地 */
        case LOCAL_BANK_OTHER_CITY:
            iIndex += 3;
            break;
        /* 他行 */
        case OTHER_BANK:
            iIndex += 0;
            break;
        default:
            WriteLog(ERROR, "卡归属类型[%d]未定义", iCardBelong);
            return FAIL;
    }

    switch(cCardType)
    {
        /* 借记卡 */
        case DEBIT_CARD:
            iIndex += 0;
            break;
        /* 贷记卡 */
        case CREDIT_CARD:
            iIndex += 1;
            break;
        /* 准贷记卡 */
        case PRECREDIT_CARD:
            iIndex += 2;
            break;
        default:
            WriteLog(ERROR, "卡类型[%c]未定义", cCardType);
            return FAIL;
    }

    if(szCardAllowedType[iIndex] == '1')
    {
        return SUCC;
    }

    return FAIL;
}
