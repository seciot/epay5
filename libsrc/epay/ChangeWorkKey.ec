/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 工作密钥转加密
** 创 建 人：冯炜
** 创建日期：2013-06-13
**
** $Revision: 1.1 $
** $Log: ChangeWorkKey.ec,v $
** Revision 1.1  2013/06/14 02:22:43  fengw
**
** 1、增加工作密钥转加密函数。
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
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：工作密钥转加密
** 输入参数：
**        ptApp           app结构指针
**        iHost           主机号
**        ptWorkKey       工作密钥结构指针
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
int ChangeWorkKey(T_App* ptApp, int iHost, T_WorkKey *ptWorkKey)
{
    EXEC SQL BEGIN DECLARE SECTION;
        int     iHostNo;
        char    szShopNo[15+1];
        char    szPosNO[15+1];
        char    szTermKey[32+1];
        char    szPinKey[32+1];
        char    szMacKey[32+1];
        char    szMagKey[32+1];
    EXEC SQL END DECLARE SECTION;

    char    szKeyData[256+1];
    int     iIndex;

    memset(szTermKey, 0, sizeof(szTermKey));
    memset(szPinKey, 0, sizeof(szPinKey));
    memset(szMacKey, 0, sizeof(szMacKey));
    memset(szMagKey, 0, sizeof(szMagKey));

    /* 获取终端主密钥 */
    if(GetHostTermKey(ptApp, iHost, TERM_KEY, szTermKey) != SUCC)
    {
        return FAIL;
    }
    
    /* 终端未获取到终端主密钥，提示进行签到 */
    if(strcmp(szTermKey, " ") == 0)
    {
        strcpy(ptApp->szRetCode, ERR_NOT_HOST_KEY);

        WriteLog(ERROR, "主机号:[%d] 商户号:[%s] 终端号:[%s]终端主密钥未配置",
                 iHostNo, ptApp->szShopNo, ptApp->szPosNo);

        return FAIL;
    }

    /* 密钥转加密 */
    memset(szKeyData, 0, sizeof(szKeyData));
    iIndex = 0;

    /* 终端主密钥 */
    memcpy(szKeyData+iIndex, szTermKey, 32);
    iIndex += 32;

    /* PinKey密文 */
    memcpy(szKeyData+iIndex, ptWorkKey->szPinKey, 32);
    iIndex += 32;

    /* MacKey密文 */
    memcpy(szKeyData+iIndex, ptWorkKey->szMacKey, 32);
    iIndex += 32;

	if(HsmChangeWorkKey(szKeyData, szKeyData) != SUCC)
	{
		WriteLog(ERROR, "HsmChangeWorkKey error");

		strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

	   	return FAIL;
	}

	memset(szPinKey, 0, sizeof(szPinKey));
	memset(szMacKey, 0, sizeof(szMacKey));

	/* 获取转换后的PinKey, MacKey */
	memcpy(szPinKey, szKeyData, 32);
	memcpy(szMacKey, szKeyData+32, 32);

	/***********************校验部分************************/
	/* PinKey校验 */
	memset(szKeyData, 0, sizeof(szKeyData));
	if(HsmCalcChkval(szPinKey, szKeyData, 0) != SUCC)
	{
		WriteLog(ERROR, "计算PinKey密钥校验值失败!");

		strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

	   	return FAIL;
	}

	if(memcmp(ptWorkKey->szPIKChkVal, szKeyData, 8) != 0)
	{
		WriteLog(ERROR, "PinKey密钥校验失败!后台应答:[%s] 本地计算:[%s]", ptWorkKey->szPIKChkVal, szKeyData);

		strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

	   	return FAIL;
	}
	
	/* MacKey校验 */
	memset(szKeyData, 0, sizeof(szKeyData));
	if(HsmCalcChkval(szMacKey, szKeyData, 0) != SUCC)
	{
		WriteLog(ERROR, "计算MacKey密钥校验值失败!");

		strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

	   	return FAIL;
	}

	if(memcmp(ptWorkKey->szMAKChkVal, szKeyData, 8) != 0)
	{
		WriteLog(ERROR, "MacKey密钥校验失败!后台应答:[%s] 本地计算:[%s]", ptWorkKey->szMAKChkVal, szKeyData);

		strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

	   	return FAIL;
	}

    /* 更新工作密钥 */
    if(UpdHostTermKey(ptApp, iHost, szPinKey, szMacKey, "") != SUCC)
	{
	    WriteLog(ERROR, "更新工作密钥失败!");

		return FAIL;
	}
	
	return SUCC;
}