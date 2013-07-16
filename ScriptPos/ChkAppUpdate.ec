/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：检查终端是否需要下载
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.2 $
 * $Log: ChkAppUpdate.ec,v $
 * Revision 1.2  2013/02/21 06:46:28  fengw
 *
 * 1、修改检查应用更新时对交易类型判断条件。
 * 2、增加DEBUG代码，便于调试。
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
** 功    能:检查终端是否需要下载
** 输入参数:
**          ptApp                   app结构指针
**          ptTerm                  terminal结构指针
** 输出参数:
** 返 回 值:
**          成功 - SUCC
**          失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int ChkAppUpdate(T_App *ptApp, T_TERMINAL *ptTerm)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[16+1];             /* 安全模块号 */
        char    szCenAppVer[8+1];           /* 平台应用版本号 */
        int     iCnt;                       /* 记录数 */
        int     iAppType;                   /* 应用类型 */
    EXEC SQL END DECLARE SECTION;

    char        szTmpBuf[128+1];            /* 临时变量 */

    /* 不检查更新情况 */
    /* 1.多回合交易中非首回合交易，保证当前交易完整结束 */
    /* 2.交易类型小于100(非金融交易)，防止终端更新过程中被打断 */
    /* 3.交易类型大于100(金融交易)且应答码为成功，保证交易凭条打印成功 */
    /* 4.应答码为无效终端、无效商户等 */
    if(ptApp->iSteps > 1 || (ptApp->iTransType < 100 && ptApp->iTransType != LOGIN) ||
       (ptApp->iTransType >= 100 && memcmp(ptApp->szRetCode, TRANS_SUCC, 2) == 0) ||
       memcmp(ptApp->szRetCode, ERR_INVALID_TERM, 2) == 0 ||
       memcmp(ptApp->szRetCode, ERR_DUPLICATE_PSAM_NO, 2) == 0 ||
       memcmp(ptApp->szRetCode, ERR_INVALID_MERCHANT, 2) == 0)
    {
        return SUCC;
    }

    /* 检查是否需要更新应用 */
    if(ptTerm->szDownAll[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_ALL_OPERATION_CODE);
#endif

        strcpy(ptApp->szNextTransCode, AUTODOWN_ALL_OPERATION_CODE);

        return FAIL;
    }

    /* 检查终端应用版本号 */
    iAppType = ptTerm->iAppType;
    memset(szCenAppVer, 0, sizeof(szCenAppVer));

    EXEC SQL SELECT app_ver 
        INTO :szCenAppVer
        FROM app_def
        WHERE app_type = :iAppType;
    if(SQLCODE == SQL_NO_RECORD)
    {
        strcpy(ptApp->szRetCode, ERR_INVALID_APP);

        WriteLog(ERROR, "应用类型[%d]未定义", ptTerm->iAppType);

        return FAIL;
    }
    else if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询应用类型[%d]版本号失败!SQLCODE=%d SQLERR=%s",
                 iAppType, SQLCODE, SQLERR);

        return FAIL;
    }

    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    BcdToAsc((uchar*)(ptApp->szAppVer), 8, 0 ,(uchar*)szTmpBuf);

    if(memcmp(szTmpBuf, szCenAppVer, 8) != 0)
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_ALL_OPERATION_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_ALL_OPERATION_CODE);

        return FAIL;
    }

    /* 检查终端单项更新 */
    /* 更新菜单 */
    if(ptTerm->szDownMenu[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_MENU_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_MENU_CODE);

        return FAIL;
    }
    /* 更新终端参数 */
    else if(ptTerm->szDownTerm[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_TERM_PARA_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_TERM_PARA_CODE);

        return FAIL;
    }
    /* 更新安全参数 */
    else if(ptTerm->szDownPsam[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_PSAM_PARA_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_PSAM_PARA_CODE);

        return FAIL;
    }
    /* 更新打印记录 */
    else if(ptTerm->szDownPrint[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_PRINT_INFO_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_PRINT_INFO_CODE);

        return FAIL;
    }
    /* 更新功能提示信息 */
    else if(ptTerm->szDownFunction[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_FUNCTION_INFO_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_FUNCTION_INFO_CODE);

        return FAIL;
    }
    /* 更新操作提示信息 */
    else if(ptTerm->szDownOperate[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_OPERATION_INFO_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_OPERATION_INFO_CODE);

        return FAIL;
    }
    /* 更新错误信息 */
    else if(ptTerm->szDownError[0] == 'Y')
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_ERROR_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_ERROR_CODE);

        return FAIL;
    }
    /* 更新短信 */
    else if(ptTerm->iMsgRecNum > 0)
    {
#ifdef DEBUG
        WriteLog(TRACE, "Next TransCode:[%s]", AUTODOWN_MSG_CODE);
#endif
        strcpy(ptApp->szNextTransCode, AUTODOWN_MSG_CODE);

        return FAIL;
    }

    /* 检查是否需要更新账单 */
    memset(szPsamNo, 0, sizeof(szPsamNo));
    strcpy(szPsamNo, ptApp->szPsamNo);

    EXEC SQL
        SELECT COUNT(*) INTO :iCnt
        FROM  pay_list
        WHERE psam_no = :szPsamNo AND
              down_flag = 'N' AND pay_status = 'N';
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询终端 安全模块号[%s] 账单数据失败!SQLCODE=%d SQLERR=%s",
                 szPsamNo, SQLCODE, SQLERR);

        return FAIL;
    }

    if(iCnt > 0)
    {
        strcpy(ptApp->szNextTransCode, AUTODOWN_PAYLIST_CODE);

        return FAIL;
    }

    return SUCC;
}
