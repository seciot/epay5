/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 后台终端密钥处理函数
** 创 建 人：冯炜
** 创建日期：2013-06-13
**
** $Revision: 1.1 $
** $Log: HostTermKey.ec,v $
** Revision 1.1  2013/06/14 02:23:30  fengw
**
** 1、增加后台密钥查询、更新函数。
**
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "errcode.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    int     iHostNo;
    char    szShopNo[15+1];
    char    szPosNo[15+1];
    char    szTermKey[32+1];
    char    szPinKey[32+1];
    char    szMacKey[32+1];
    char    szMagKey[32+1];
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：获取主机密钥信息
** 输入参数：
**        ptApp           app结构指针
**        iHost           主机号
** 输出参数：
**        szTMK           终端主密钥
**        szPinKey        PinKey
**        szMacKey        MacKey
**        szMagKey        MagKey
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2013/06/13
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetHostTermKey(T_App* ptApp, int iHost, int iKeyType, char* szKey)
{
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));

    iHostNo = iHost;
    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);

    memset(szTermKey, 0, sizeof(szTermKey));
    memset(szPinKey, 0, sizeof(szPinKey));
    memset(szMacKey, 0, sizeof(szMacKey));
    memset(szMagKey, 0, sizeof(szMagKey));

    EXEC SQL
        SELECT
            NVL(MASTER_KEY, ' '), NVL(PIN_KEY, ' '), NVL(MAC_KEY, ' '), NVL(MAG_KEY, ' ')
        INTO
            :szTermKey, :szPinKey, :szMacKey, :szMagKey
        FROM host_term_key
        WHERE host_no = :iHostNo AND shop_no = :szShopNo AND pos_no = :szPosNo;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询 主机号:[%d] 商户号:[%s] 终端号:[%s]密钥信息失败!SQLCODE=%d SQLERR=%s",
                 iHostNo, szShopNo, szPosNo, SQLCODE, SQLERR); 

        return FAIL;
    }

    switch(iKeyType)
    {
        case TERM_KEY:
            strcpy(szKey, szTermKey);
            break;
        case PIN_KEY:
            strcpy(szKey, szPinKey);
            break;
        case MAC_KEY:
            strcpy(szKey, szMacKey);
            break;
        case MAG_KEY:
            strcpy(szKey, szMagKey);
            break;
        default:
            WriteLog(ERROR, "密钥类型:[%d]未定义!", iKeyType);
            return FAIL;
    }

    return SUCC;
}

/****************************************************************
** 功    能：更新主机工作密钥
** 输入参数：
**        ptApp           app结构指针
**        iHost           主机号
**        szPinKey        PinKey
**        szMacKey        MacKey
**        szMagKey        MagKey
** 输出参数：
**        无
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2013/06/13
** 调用说明：
**
** 修改日志：
****************************************************************/
int UpdHostTermKey(T_App* ptApp, int iHost, char* szPIK,
                   char* szMAK, char* szMGK)
{
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));
    memset(szPinKey, 0, sizeof(szPinKey));
    memset(szMacKey, 0, sizeof(szMacKey));
    memset(szMagKey, 0, sizeof(szMagKey));

    iHostNo = iHost;
    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);
    strcpy(szPinKey, szPIK);
    strcpy(szMacKey, szMAK);
    strcpy(szMagKey, szMGK);

	EXEC SQL 
	    UPDATE host_term_key 
        SET pin_key = :szPinKey, mac_key = :szMacKey, mag_key = :szMagKey
        WHERE host_no = :iHostNo AND shop_no = :szShopNo AND pos_no = :szPosNo;
	if(SQLCODE)
	{
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "更新 主机号:[%d] 商户号:[%s] 终端号:[%s]密钥信息失败!SQLCODE=%d SQLERR=%s",
                 iHostNo, szShopNo, szPosNo, SQLCODE, SQLERR); 

		RollbackTran();

		return FAIL;
	}

	CommitTran();

	return SUCC;
}