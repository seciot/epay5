/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 电话号码合法性检查
** 创 建 人：冯炜
** 创建日期：2012-11-12
**
** $Revision: 1.6 $
** $Log: ChkTeleValid.c,v $
** Revision 1.6  2013/06/28 06:03:30  fengw
**
** 1、修正终端主叫号码检查代码BUG。
**
** Revision 1.5  2013/06/07 02:14:40  fengw
**
** 1、增加是否检查号码绑定判断相关代码。
**
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
** 功    能：电话号码合法性检查
** 输入参数：
**        gszTelephone          绑定电话号码
**        giTeleChkLen          电话号码检查位数
** 输出参数：
**        无
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/12
** 调用说明：
**        1.检查终端主叫号码是否与终端资料终端登记号码一致。当
**          登记资料为00000000时或上送号码为FFFFFFFF时，不做检查。
**        2.检查终端上送被叫号码与网控器上送被叫号码是否一致。
** 修改日志：
****************************************************************/
int ChkTeleValid(T_App *ptApp)
{
    int     iChkLen;        /* 电话号码检查位数 */
    int     iTeleLen;       /* 终端上送电话号码长度 */
    int     iLen;           /* 登记资料电话号码长度 */

    /* 判断是否检查号码绑定 */
    if(giTeleChkType == 0)
    {
        return SUCC;
    }

    /* 终端主叫号码检查 */
    /* 终端资料中电话号码为00000000时，不检查主叫号码 */
    /* 终端上送主叫号码为FFFFFFFF时，不检查主叫号码，用于平台发起交易 */
    if(memcmp(gszTelephone, "00000000", 8) != 0 &&
       memcmp(ptApp->szCallingTel, "FFFFFFFF", 8) != 0)
    {
        iTeleLen = strlen(ptApp->szCallingTel);
        iLen = strlen(gszTelephone);

        /* 当设置的电话号码检查位数大于上送电话号码长度或登记资料号码长度 */
        /* 检查长度以两号码中长度最小的那个号码长度为准 */
        /* 否则以设置的检查位数为准 */
        if(giTeleChkLen > iLen || giTeleChkLen > iTeleLen)
        {
            iChkLen = iLen>iTeleLen?iTeleLen:iLen;
        }
        else
        {
            iChkLen = giTeleChkLen;
        }

        if(memcmp(ptApp->szCallingTel + iTeleLen - iChkLen,
                  gszTelephone + iLen - iChkLen,
                  iChkLen) != 0)
        {
            strcpy(ptApp->szRetCode, ERR_INVALID_PHONE);

            WriteLog(ERROR, "终端[%s]主叫号码非法，终端上送:[%s] 资料登记:[%s]",
                     ptApp->szPsamNo, ptApp->szCallingTel, giTeleChkLen);

            return FAIL;
        }
	}

	/* 被叫号码检查 */
    if(strcmp(ptApp->szCalledTelByTerm, ptApp->szCalledTelByNac) != 0)
    {
        strcpy(ptApp->szRetCode, ERR_INVALID_PHONE);

        WriteLog(ERROR, "终端[%s]被叫叫号码非法，终端上送:[%s] 网控上送:[%s]",
                 ptApp->szPsamNo, ptApp->szCalledTelByTerm, ptApp->szCalledTelByNac);

        return FAIL;
    }

	return SUCC;
}
