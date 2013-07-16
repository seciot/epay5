/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：终端资料表相关操作函数
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.3 $
 * $Log: GetTermRec.ec,v $
 * Revision 1.3  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 * Revision 1.2  2012/12/21 07:05:35  wukj
 * 增加注释
 *
 * Revision 1.1  2012/12/18 09:14:02  wukj
 * *** empty log message ***
 *
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif
/*****************************************************************
** 功    能:取终端信息
** 输入参数:
           ptApp
** 输出参数:
           ptTerm   终端信息结构体
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int GetTermRec(T_App* ptApp, T_TERMINAL *ptTerm)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[15+1];             /* 商户号 */
        char    szPosNo[15+1];              /* 终端号 */
        char    szPsamNo[16+1];             /* 安全模块号 */
        char    szTelephone[15+1];          /* 电话号码 */
        int     iTermModule;                /* 终端参数模板 */
        int     iPsamModule;                /* 安全参数模板 */
        int     iAppType;                   /* 应用类型 */
        char    szDescribe[20+1];           /* 终端描述 */
        char    szPosType[40+1];            /* 终端型号 */
        char    szAddress[40+1];            /* 安装地址 */
        char    szPutDate[8+1];             /* 安装日期 */
        long    lCurTrace;                  /* 当前流水号 */
        char    szIp[15+1];                 /* 请求方IP */
        int     iPort;                      /* 端口号 */
        char    szDownMenu[1+1];            /* 是否需要下载菜单 */
        char    szDownTerm[1+1];            /* 是否需要下载终端参数 */
        char    szDownPsam[1+1];            /* 是否需要下载安全参数 */
        char    szDownPrint[1+1];           /* 是否需要下载打印模板 */
        char    szDownOperate[1+1];         /* 是否需要下载操作提示 */
        char    szDownFunction[1+1];        /* 是否需要下载功能提示 */
        char    szDownError[1+1];           /* 是否需要下载错误提示 */
        char    szDownAll[1+1];             /* 是否需要下载全部内容 */
        char    szDownPayList[1+1];         /* 是否需要下载账单 */
        int     iMenuRecNo;                 /* 已下载菜单记录号 */
        int     iPrintRecNo;                /* 已下载菜单记录号 */
        int     iOperateRecNo;              /* 已下载操作提示记录号 */
        int     iFunctionRecNo;             /* 已下载功能提示记录号 */
        int     iErrorRecNo;                /* 已下载错误提示记录号 */
        int     iAllTransType;              /* 当前下载功能 */
        char    szTermBitMap[8+1];          /* 终端参数下载位图 */
        char    szPsamBitMap[8+1];          /* 安全参数下载位图 */
        char    szPrintBitMap[64+1];        /* 打印位图 */
        char    szOperateBitMap[64+1];      /* 操作提示位图 */
        char    szFunctionBitMap[64+1];     /* 功能提示位图 */
        char    szErrorBitMap[64+1];        /* 错误提示位图 */
        int     iMsgRecNum;                 /* 需要下载短信记录数 */
        char    szMsgRecNo[256+1];          /* 需要下载短信记录号序号 */
        int     iFirstPage;                 /* 需要下载首页信息记录号 */
        int     iTStatus;                   /* 0-正常、1-停用 */
        long    lCurBatch;                  /* 当前批次号 */
    EXEC SQL END DECLARE SECTION;

    memset(szPsamNo, 0, sizeof(szPsamNo));
    strcpy(szPsamNo, ptApp->szPsamNo);

    EXEC SQL
        SELECT shop_no, pos_no, NVL(telephone, ' '), NVL(term_module, 0), NVL(psam_module, 0),
               NVL(app_type, 1), NVL(describe, ' '), NVL(pos_type, 'spp100'), NVL(address, ' '),
               NVL(put_date, ' '), NVL(cur_trace, 1), NVL(ip, '127.0.0.1'), NVL(port, 0),
               NVL(down_menu, 'N'), NVL(down_term, 'N'), NVL(down_psam, 'N'), NVL(down_print, 'N'),
               NVL(down_operate, 'N'), NVL(down_function, 'N'), NVL(Down_Error, 'N'), NVL(down_all, 'N'),
               NVL(down_paylist, 'N'), NVL(menu_recno, 0), NVL(print_recno, 0), NVL(operate_recno, 0),
               NVL(function_recno, 0), NVL(error_recno, 0), NVL(all_transtype, 3), NVL(term_bitmap, ' '),
               NVL(psam_bitmap, ' '), NVL(print_bitmap, ' '), NVL(operate_bitmap, ' '), NVL(function_bitmap, ' '),
               NVL(error_bitmap, '  '), NVL(msg_recnum, 0), NVL(msg_recno, ' '), NVL(first_page, 0),
               NVL(status, 0), NVL(cur_batch, 1)
        INTO :szShopNo, :szPosNo, :szTelephone, :iTermModule, :iPsamModule,
             :iAppType, :szDescribe, :szPosType, :szAddress,
             :szPutDate, :lCurTrace, :szIp, :iPort,
             :szDownMenu, :szDownTerm, :szDownPsam, :szDownPrint,
             :szDownOperate, :szDownFunction, :szDownError, :szDownAll,
             :szDownPayList, :iMenuRecNo, :iPrintRecNo, :iOperateRecNo,
             :iFunctionRecNo, :iErrorRecNo, :iAllTransType, :szTermBitMap,
             :szPsamBitMap, :szPrintBitMap, :szOperateBitMap, :szFunctionBitMap,
             :szErrorBitMap, :iMsgRecNum, :szMsgRecNo, :iFirstPage,
             :iTStatus, :lCurBatch
        FROM  terminal
        WHERE psam_no = :szPsamNo;
    if(SQLCODE == SQL_NO_RECORD)
    {
        memset(ptApp->szShopNo, 0, sizeof(ptApp->szShopNo));
        memcpy(ptApp->szShopNo, szPsamNo, 8);

        memset(ptApp->szPosNo, 0, sizeof(ptApp->szShopNo));
        memcpy(ptApp->szPosNo, szPsamNo+8, 8);

        WriteLog(ERROR, "安全模块号[%s]未登记", szPsamNo);

        strcpy(ptApp->szRetCode, ERR_INVALID_TERM);

        return FAIL;
    }
    else if(SQLCODE == SQL_SELECT_MUCH)
    {
        memset(ptApp->szShopNo, 0, sizeof(ptApp->szShopNo));
        memcpy(ptApp->szShopNo, szPsamNo, 8);

        memset(ptApp->szPosNo, 0, sizeof(ptApp->szShopNo));
        memcpy(ptApp->szPosNo, szPsamNo+8, 8);

        WriteLog(ERROR, "安全模块号[%s]记录重复", szPsamNo );

        strcpy(ptApp->szRetCode, ERR_DUPLICATE_PSAM_NO);

        return FAIL;
    }
    else if(SQLCODE)
    {
        memset(ptApp->szShopNo, 0, sizeof(ptApp->szShopNo));
        memcpy(ptApp->szShopNo, szPsamNo, 8);

        memset(ptApp->szPosNo, 0, sizeof(ptApp->szShopNo));
        memcpy(ptApp->szPosNo, szPsamNo+8, 8);

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询 安全模块号:[%s] 终端记录失败!SQLCODE=%d SQLERR=%s",
                 szPsamNo, SQLCODE, SQLERR);

        return FAIL;
    }

    /* 删除空格 */
    DelTailSpace(szShopNo);
    DelTailSpace(szPosNo);
    DelTailSpace(szTelephone);
    DelTailSpace(szDescribe);
    DelTailSpace(szPosType);
    DelTailSpace(szPosType);
        
    strcpy(ptTerm->szShopNo, szShopNo);
    strcpy(ptTerm->szPosNo, szPosNo);
    strcpy(ptTerm->szTelephone, szTelephone);
    ptTerm->iTermModule = iTermModule;
    ptTerm->iPsamModule = iPsamModule;
    ptTerm->iAppType = iAppType;
    strcpy(ptTerm->szDescribe, szDescribe);
    strcpy(ptTerm->szPosType, szPosType);
    strcpy(ptTerm->szAddress, szAddress);
    strcpy(ptTerm->szPutDate, szPutDate);
    ptTerm->lCurTrace = lCurTrace;
    strcpy(ptTerm->szIp, szIp);
    ptTerm->iPort = iPort;
    strcpy(ptTerm->szDownMenu, szDownMenu);
    strcpy(ptTerm->szDownTerm, szDownTerm);
    strcpy(ptTerm->szDownPsam, szDownPsam);
    strcpy(ptTerm->szDownPrint, szDownPrint);
    strcpy(ptTerm->szDownOperate, szDownOperate);
    strcpy(ptTerm->szDownFunction, szDownFunction);
    strcpy(ptTerm->szDownError, szDownError);
    strcpy(ptTerm->szDownAll, szDownAll);
    strcpy(ptTerm->szDownPayList, szDownPayList);
    ptTerm->iMenuRecNo = iMenuRecNo;
    ptTerm->iPrintRecNo = iPrintRecNo;
    ptTerm->iOperateRecNo = iOperateRecNo;
    ptTerm->iFunctionRecNo = iFunctionRecNo;
    ptTerm->iErrorRecNo = iErrorRecNo;
    ptTerm->iAllTransType = iAllTransType;
    strcpy(ptTerm->szTermBitMap, szTermBitMap);
    strcpy(ptTerm->szPsamBitMap, szPsamBitMap);
    strcpy(ptTerm->szPrintBitMap, szPrintBitMap);
    strcpy(ptTerm->szOperateBitMap, szOperateBitMap);
    strcpy(ptTerm->szFunctionBitMap, szFunctionBitMap);
    strcpy(ptTerm->szErrorBitMap, szErrorBitMap);
    ptTerm->iMsgRecNum = iMsgRecNum;
    strcpy(ptTerm->szMsgRecNo, szMsgRecNo);
    ptTerm->iFirstPage = iFirstPage;
    ptTerm->iTStatus = iTStatus;
    ptTerm->lCurBatch = lCurBatch;

    return SUCC;
}
