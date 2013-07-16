/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 PinBlock转加密
** 创 建 人：冯炜
** 创建日期：2013-06-13
**
** $Revision: 1.1 $
** $Log: ConvertPin.c,v $
** Revision 1.1  2013/06/14 02:23:06  fengw
**
** 1、增加PinBlock转加密函数。
**
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "user.h"
#include "errcode.h"

#define TRIPLE_DES      9
#define SINGLE_DES      8

/****************************************************************
** 功    能：PinBlock转加密
** 输入参数：
**        ptApp           app结构指针
**        iHost           主机号
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
int ConvertPin(T_App* ptApp, int iHost)
{
    char    szPinKey[32+1];
    char    szPIK[16+1];
	char    szSourcePan[20];
	char    szTargetPan[20];

    /* 获取PinKey */
    memset(szPinKey, 0, sizeof(szPinKey));
    if(GetHostTermKey(ptApp, iHost, PIN_KEY, szPinKey) != SUCC)
    {
        return FAIL;
    }

	memset(szPIK, 0, sizeof(szPIK));
	AscToBcd(szPinKey, 32, 0, szPIK);

    memset(szSourcePan, 0, sizeof(szSourcePan));
    memset(szTargetPan, 0, sizeof(szTargetPan));

	if(strlen(ptApp->szPan) < 16)
	{
		/*右对齐左补零*/
		memset(szSourcePan, '0', 16);
		memcpy(szSourcePan+16-strlen(ptApp->szPan), ptApp->szPan, strlen(ptApp->szPan));
	}
	else
	{
		/*后16位*/
		memset(szSourcePan, '0', 16);
		memcpy(szSourcePan, ptApp->szPan+strlen(ptApp->szPan)-16, 16);
	}
	strcpy(szTargetPan, szSourcePan);

	if(HsmChangePin(ptApp, 1, TRIPLE_DES, ptApp->szPinKey, szPIK, szSourcePan, szTargetPan) != SUCC)
	{
		WriteLog(ERROR, "PinBlock转加密失败!");

		strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

		return FAIL;
	}

	return SUCC;
}
