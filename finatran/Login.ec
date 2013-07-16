/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 签到交易
** 创 建 人：冯炜
** 创建日期：2013-06-13
**
** $Revision: 1.1 $
** $Log: Login.ec,v $
** Revision 1.1  2013/06/14 02:33:14  fengw
**
** 1、增加签到交易。
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
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
**        2012/11/09
** 调用说明：
**
** 修改日志：
****************************************************************/
int LoginPreTreat(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[16+1];
        long    lPosTrace;
        int     iKeyIndex;
        char    szTMK[32+1];
        char    szPinKey[32+1];
        char    szMacKey[32+1];
        char    szMagKey[32+1];
    EXEC SQL END DECLARE SECTION;

    char    szTmpBuf[512+1];
    char    szKeyData[256+1];
    int     iIndex;
    int     iCount;
    int     i;

    /* 取POS当前流水号，以便终端更新当前流水号 */
    memset(szPsamNo, 0, sizeof(szPsamNo));
    strcpy(szPsamNo, ptApp->szPsamNo);

    EXEC SQL
        SELECT cur_trace INTO :lPosTrace
        FROM terminal
        WHERE psam_no = :szPsamNo;
    if(SQLCODE)
    {
        WriteLog(ERROR, "查询终端 安全模块号[%s] 当前流水失败!SQLCODE=%d SQLERR=%s",
                 szPsamNo, SQLCODE, SQLERR);

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    ptApp->lPosTrace = lPosTrace;

    /* 取终端主密钥密文 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    memcpy(szTmpBuf, ptApp->szPsamNo+8, 8);
    iKeyIndex = atol(szTmpBuf);

    memset(szTMK, 0, sizeof(szTMK));

    EXEC SQL
        SELECT master_key_lmk INTO :szTMK
        FROM pos_key 
        WHERE key_index = :iKeyIndex;
    if(SQLCODE)
    {
        WriteLog(ERROR, "查询密钥索引[%d]TMK密文失败!SQLCODE=%d SQLERR=%s",
                 iKeyIndex, SQLCODE, SQLERR);

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    /* 加密机随机生成工作密钥 */
    memset(szKeyData, 0, sizeof(szKeyData));
    memcpy(szKeyData, szTMK, 32);

    if(HsmGetWorkKey(ptApp, szKeyData) != SUCC)
    {
        WriteLog(ERROR, "生成工作密钥失败!");

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    /* 本地密钥加密的工作密钥密文保存到数据库 */
    memset(szPinKey, 0, sizeof(szPinKey));
    memset(szMacKey, 0, sizeof(szMacKey));
    memset(szMagKey, 0, sizeof(szMagKey));

    memcpy(szPinKey, szKeyData, 32);
    memcpy(szMacKey, szKeyData+80, 32);
    memcpy(szMagKey, szKeyData+160, 32);

    EXEC SQL
        UPDATE pos_key 
        SET pin_key = :szPinKey, mac_key = :szMacKey,
            mag_key = :szMagKey
        WHERE key_index = :iKeyIndex;
    if(SQLCODE)
    {
        WriteLog(ERROR, "更新密钥索引[%d]工作密钥密文失败!SQLCODE=%d SQLERR=%s",
                 iKeyIndex, SQLCODE, SQLERR);

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        RollbackTran();

        return FAIL;
    }

    CommitTran();

    /* 终端主密钥加密的工作密钥密文返回给终端 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    iCount = 0;
    iIndex = 0;

    for(i=8;i<=14;i++)
    {
        switch(i)
        {
            case 8:
                /* 安全模块号状态 */
                szTmpBuf[iIndex] = i;
                iIndex += 1;
            
                /* 数据长度 */
                szTmpBuf[iIndex] = 1;
                iIndex += 1;
            
                /* 数据 */
                szTmpBuf[iIndex] = '0';
                iIndex += 1;

                iCount += 1;

                break;
            case 9:
            case 10:
            case 14:
                /* 保留 */
                break;
            case 11:
            case 12:
            case 13:
                /* 工作密钥 */
                /* 密钥类型 */
                szTmpBuf[iIndex] = i;
                iIndex += 1;

                /* 密钥长度 16字节密文+4字节密钥校验值 */
                szTmpBuf[iIndex] = 20;
                iIndex += 1;

                /* 密钥密文 + 校验值 */
                AscToBcd((uchar *)(szKeyData+32+(i-11)*80), 40, 0 ,(uchar *)(szTmpBuf+iIndex));
                iIndex += 20;

                iCount += 1;

                break;
            default:
                break;
        }
    }

    ptApp->szReserved[0] = iCount;
    memcpy(ptApp->szReserved+1, szTmpBuf, iIndex);
    ptApp->iReservedLen = iIndex+1;

    strcpy(ptApp->szRetCode, TRANS_SUCC);

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
**        2012/11/09
** 调用说明：
**
** 修改日志：
****************************************************************/
int LoginPostTreat(T_App *ptApp)
{
    return SUCC;
}
