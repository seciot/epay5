/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：终端资料表相关操作函数
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.2 $
 * $Log: ChkTmsUpdate.ec,v $
 * Revision 1.2  2013/06/14 06:32:54  fengw
 *
 * 1、文件格式转换。
 *
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;

/*****************************************************************
** 功    能:增加检查TMS通知标识
** 输入参数:
           ptApp
** 输出参数:
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int ChkTmsUpdate(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[20+1];                 /* 商户号 */
        char    szPosNo[20+1];                  /* 终端号 */
        int     iCnt;                           /* 记录数 */
    EXEC SQL END DECLARE SECTION;

    /* 无效终端，不用下载 */
    if(memcmp(ptApp->szRetCode, ERR_INVALID_TERM, 2) == 0 ||
       memcmp(ptApp->szRetCode, ERR_DUPLICATE_PSAM_NO, 2) == 0 ||
       memcmp(ptApp->szRetCode, ERR_INVALID_MERCHANT, 2) == 0 ||
       memcmp(ptApp->szTransCode, DOWN_TMS_CODE, 8) == 0 ||
       strlen(ptApp->szNextTransCode) >= 8)
    {
        return SUCC;
    }

    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));

    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);

    EXEC SQL SELECT COUNT(*) 
        INTO :iCnt 
        FROM tm_vpos_info
        WHERE TRIM(shop_no) = :szShopNo AND
              TRIM(pos_no) = :szPosNo AND notice_flag = '1';
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询 商户号[%s] 终端号:[%s] TMS更新通知失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, szPosNo, SQLCODE, SQLERR);

        return FAIL;
    }

    if(iCnt == 1)
    {
        strcpy(ptApp->szNextTransCode, DOWN_TMS_CODE);

        return FAIL;   
    }    

    return SUCC;
}