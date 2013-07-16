/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台银联POS通讯模块 报文MAC计算
** 创 建 人：冯炜
** 创建日期：2013-06-09
**
** $Revision: 1.1 $
** $Log: CalcMac.c,v $
** Revision 1.1  2013/06/14 02:05:29  fengw
**
** 1、增加MAC计算函数。
**
*******************************************************************/

#define _EXTERN_

#include "tohost.h"

/****************************************************************
** 功    能：MAC计算
** 输入参数：
**        szData          参与MAC计算报文
**        iLen            报文长度
**        ptApp           app结构指针
** 输出参数：
**        szMAC           报文MAC
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2013/06/09
** 调用说明：
**
** 修改日志：
****************************************************************/
int CalcMac(char* szData, int iLen, T_App* ptApp, char* szMAC)
{
    char    szMacKey[32+1];
    char    szMAK[16+1];
    char    szTmpBuf1[8+1];
    char    szTmpBuf2[16+1];
    int     i;

    /* 获取后台MACKEY */
    memset(szMacKey, 0, sizeof(szMacKey));
    if(GetHostTermKey(ptApp, YLPOSP, MAC_KEY, szMacKey) != SUCC)
    {
        return FAIL;
    }
    AscToBcd(szMacKey, 32, 0, szMAK);

    /* 报文处理 */
    memset(szTmpBuf1, 0, sizeof(szTmpBuf1));
    memset(szTmpBuf2, 0, sizeof(szTmpBuf2));

    /* 每8字节循环异或后展开 */
    XOR(szData, iLen, szTmpBuf1);
    BcdToAsc(szTmpBuf1, 16, 0, szTmpBuf2);

    /* 取前8个字节加密 */
    if(HsmCalcMac(ptApp, X99_CALC_MAC, szMAK, szTmpBuf2, 8, szTmpBuf1) != SUCC)
    {
        WriteLog(ERROR, "计算报文MAC错误!");

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    /* 加密结果与后8个字节异或 */
    for(i=0;i<8;i++)
    {
        szTmpBuf1[i] = szTmpBuf1[i] ^ szTmpBuf2[8+i];
    }

    /* 异或结果加密 */
    if(HsmCalcMac(ptApp, X99_CALC_MAC, szMAK, szTmpBuf1, 8, szTmpBuf1) != SUCC)
    {
        WriteLog(ERROR, "计算报文MAC错误!");

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    /* 加密结果展开 */
    BcdToAsc(szTmpBuf1, 16, 0, szTmpBuf2);

    /* 取前8个字节做为MAC */
    memcpy(szMAC, szTmpBuf2, 8);

	return SUCC;
}