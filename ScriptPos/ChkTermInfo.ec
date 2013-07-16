/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：终端资料表相关操作函数
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.3 $
 * $Log: ChkTermInfo.ec,v $
 * Revision 1.3  2013/03/01 05:15:28  fengw
 *
 * 1、修正签到更新版本BUG。
 *
 * Revision 1.2  2013/02/21 06:50:58  fengw
 *
 * 1、增加错误应答码赋值代码。
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

/*******************************************************************************
 * 函数功能：根据安全模块号获取终端资料以及密钥
 * 输入参数：
 *           ptApp  - 公共数据结构
 * 输出参数：
 *           
 * 返 回 值： 
 *           无
 *
 * 作    者：Robin
 * 日    期：2012/11/20
 *
 ******************************************************************************/
int ChkTermInfo(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[16+1];             /* 安全模块号 */
        long    lTrace;                     /* 终端流水号 */
        long    lKeyIndex;                  /* 密钥索引值 */
        char    szPinKey[32+1];             /* PINKEY */
        char    szMacKey[32+1];             /* MACKEY */
        char    szMagKey[32+1];             /* MAGKEY */
    EXEC SQL END DECLARE SECTION;

    char    szTmpBuf[128+1];                /* 临时变量 */
    T_TERMINAL  tTerm;                      /* 终端结构 */

    memset(&tTerm, 0, sizeof(T_TERMINAL));

    if(GetTermRec(ptApp, &tTerm) != SUCC)
    {
        WriteLog(ERROR, "获取终端 安全模块号:[%s] 记录失败!", ptApp->szPsamNo);

        return FAIL;
    }

    /* 终端信息赋值 */
    /* 商户号 */
    strcpy(ptApp->szShopNo, tTerm.szShopNo);

    /* 终端号 */
    strcpy(ptApp->szPosNo, tTerm.szPosNo);

    /* 终端参数模板 */
    ptApp->iTermModule = tTerm.iTermModule;

    /* 安全参数模板 */
    ptApp->iPsamModule = tTerm.iPsamModule;

    /* 当前批次号 */
    ptApp->lBatchNo = tTerm.lCurBatch;

    /* 下载位图 */
    switch (ptApp->iTransType)
    {
        case DOWN_ALL_FUNCTION:    
        case DOWN_ALL_OPERATION:    
        case DOWN_ALL_PRINT:    
        case DOWN_ALL_ERROR:    
        case DOWN_FUNCTION_INFO:    
        case DOWN_OPERATION_INFO:    
        case AUTODOWN_ALL_OPERATION:    
        case DOWN_PRINT_INFO:    
        case DOWN_ERROR:    
            memset(ptApp->szReserved, '1', 256);
            break;
        case DOWN_ALL_PSAM:
        case DOWN_ALL_TERM:
        case DOWN_PSAM_PARA:
        case DOWN_TERM_PARA:
            memset(ptApp->szReserved, '1', 32);
            break;
        case CENDOWN_FUNCTION_INFO:
        case AUTODOWN_FUNCTION_INFO:
            UncompressBitmap(ptApp->szReserved, tTerm.szFunctionBitMap, 64);
            break;
        case CENDOWN_OPERATION_INFO:
        case AUTODOWN_OPERATION_INFO:
            UncompressBitmap(ptApp->szReserved, tTerm.szOperateBitMap, 64);
            break;
        case CENDOWN_PRINT_INFO:
        case AUTODOWN_PRINT_INFO:
            UncompressBitmap(ptApp->szReserved, tTerm.szPrintBitMap, 64);
            break;
        case CENDOWN_ERROR:
        case AUTODOWN_ERROR:
            UncompressBitmap(ptApp->szReserved, tTerm.szErrorBitMap, 64);
            break;
        case CENDOWN_TERM_PARA:
        case AUTODOWN_TERM_PARA:
            UncompressBitmap(ptApp->szReserved, tTerm.szTermBitMap, 8);
            break;
        case CENDOWN_PSAM_PARA:
        case AUTODOWN_PSAM_PARA:
            UncompressBitmap(ptApp->szReserved, tTerm.szPsamBitMap, 8);
            break;
    }

    /* 如果POS上传流水号大于系统记录的该终端流水号，更新终端记录当前流水号字段 */
    if(ptApp->lPosTrace > tTerm.lCurTrace)
    {
        lTrace = ptApp->lPosTrace + 1;
    }
    else
    {
        lTrace = tTerm.lCurTrace + 1;
    }

    if(lTrace >= 1000000l)
    {
        lTrace = 1l;
    }

    memset(szPsamNo, 0, sizeof(szPsamNo));
    strcpy(szPsamNo, ptApp->szPsamNo);

    EXEC SQL
        UPDATE terminal
        SET cur_trace = :lTrace
        WHERE psam_no = :szPsamNo;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "更新终端 安全模块号:[%s] 当前流水号失败!SQLCODE=%d SQLERR=%s",
                 szPsamNo, SQLCODE, SQLERR);

        RollbackTran();

        return(FAIL);
    }

    CommitTran();

    /* 查询工作密钥 */
    if(ptApp->iTransType != LOGIN && ptApp->iTransType != ECHO_TEST &&
       ptApp->iTransType != OPER_LOGIN)
    {
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        memset(szPinKey, 0, sizeof(szPinKey));
        memset(szMacKey, 0, sizeof(szMacKey));
        memset(szMagKey, 0, sizeof(szMagKey));

        memcpy(szTmpBuf, ptApp->szPsamNo+8, 8);
        lKeyIndex = atol(szTmpBuf);

        EXEC SQL
            SELECT NVL(pin_key, ' '), NVL(mac_key, ' '), NVL(mag_key, ' ')
            INTO :szPinKey, :szMacKey, :szMagKey
            FROM pos_key
            WHERE key_index = :lKeyIndex;
        if(SQLCODE == SQL_NO_RECORD)
        {
            strcpy(ptApp->szRetCode, ERR_INVALID_TERM);

            WriteLog(ERROR, "查询 密钥索引值[%ld] 对应密钥记录失败!SQLCODE=%d SQLERR=%s",
                     lKeyIndex, SQLCODE, SQLERR);

            return FAIL;
        }
        else if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "查询 密钥索引值[%ld] 对应密钥记录失败!SQLCODE=%d SQLERR=%s",
                     lKeyIndex, SQLCODE, SQLERR);

            return FAIL;
        }

        AscToBcd((uchar*)(szPinKey), 32, 0 ,(uchar*)(ptApp->szPinKey));
        AscToBcd((uchar*)(szMacKey), 32, 0 ,(uchar*)(ptApp->szMacKey));
        AscToBcd((uchar*)(szMagKey), 32, 0 ,(uchar*)(ptApp->szTrackKey));
    }

    /* 签到交易在交易返回时检查是否需要更新 */
    if(ptApp->iTransType == LOGIN)
    {
        return SUCC;
    }

#ifdef DEBUG
    WriteLog(TRACE, "Check TMS Downlaod Begin!");
#endif
    /* 检查终端是否需要更新TMS */
    if(ChkTmsUpdate(ptApp) != SUCC)
    {
#ifdef DEBUG
        WriteLog(TRACE, "TMS Need Update!");
#endif
        memcpy(ptApp->szRetCode, ERR_NEED_DOWN_APP, 2);

        return FAIL;
    }

#ifdef DEBUG
    WriteLog(TRACE, "Check App Downlaod Begin!");
#endif
    /* 检查终端是否需要更新应用 */
    if(ChkAppUpdate(ptApp, &tTerm) != SUCC)
    {
#ifdef DEBUG
        WriteLog(TRACE, "App Need Update!");
#endif
        memcpy(ptApp->szRetCode, ERR_NEED_DOWN_APP, 2);

        return FAIL;
    }

    return SUCC;
}
