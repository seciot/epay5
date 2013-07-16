
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.3 $
 * $Log: ChgAmount.c,v $
 * Revision 1.3  2013/02/21 06:30:04  fengw
 *
 * 1、修改对角分位的处理，保留2位小数格式。
 *
 * Revision 1.2  2012/12/21 05:44:01  chenrb
 * *** empty log message ***
 *
 * Revision 1.1  2012/12/11 02:11:00  fengw
 *
 * 1、ChgAmout.c改名为ChgAmount.c。
 *
 * Revision 1.5  2012/12/04 06:21:35  chenjr
 * 代码规范化
 *
 * Revision 1.4  2012/11/27 03:20:01  linqil
 * *** empty log message ***
 *
 * Revision 1.3  2012/11/27 02:52:15  yezt
 * *** empty log message ***
 *
 * Revision 1.2  2012/11/27 02:45:19  linqil
 * 增加引用pub.h 修改return
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */


#include <stdio.h>
#include <string.h>
#include "pub.h"

extern char *DelAllSpace(char *szStr);

/* ----------------------------------------------------------------
 * 功    能：将形如" 1234.56 "(允许含负号)金额字符串转化为"000000123456"
 * 输入参数：szSrc    含点分隔符的数字串(可以含正负号)
 *           iOutLen  指定转换后的输出长度(不足前补0)
 *           iPreFlag 指明对被转换串前的"-/+"是否转换成"D/C",
 *                    =0，忽略，不处理
 *                    非0, "-"转成"D", "+"转成"C"
 * 输出参数：szDest   不含点分隔符的数字串(不足前补0)
 * 返 回 值：-1 转换失败;  0 转换成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ChgAmtDotToZero(char *szSrc, int iOutLen, int iPreFlag, char *szDest)
{
    char acDot[100], acZero[100], *pD;
    int iDotLen, iLOffset, iRealLen;

    if (szSrc == NULL || iOutLen < 0 || iOutLen > 100 || szDest == NULL)
    {
        return FAIL;
    }

    memset(acDot, 0, sizeof(acDot));
    memset(acZero, '\0', sizeof(acZero));

    /* 去掉串中所有空格 */
    strcpy(acDot, szSrc);
    DelAllSpace(acDot);
    iDotLen = strlen(acDot); 

    /* 算出向左偏移长度, 要向左偏移几位 */
    iLOffset = 0;
    pD = strchr(acDot, '.');
    if (iDotLen > 0)
    {
        iLOffset = (pD == NULL ? 2 : (strlen(pD) > 3 ? 0 : 3-strlen(pD)));
    }
    
    /* 算出真正存放数据的长度 */
    iRealLen = iDotLen + iLOffset;
    if (iOutLen > iRealLen)
    {
        iRealLen = iOutLen;
    }
    else
    {
        /* 如果真实长度包含点长度或符号长度，则需要减去 */
        if (pD != NULL)
        {
            iRealLen--;
        }

        if (iDotLen > 0 && (acDot[0] == '-' || acDot[0] == '+'))
        {
            iRealLen--;
        }
    }

    if (iPreFlag != 0)
    {
        iRealLen++;
    }

    memset(acZero, '0', iRealLen);
    iRealLen -= iLOffset;

    /* 符号位 */
    if (iPreFlag != 0 && iDotLen > 0)
    {
        if (acDot[0] == '-')
        {
            acZero[0] = 'D';
        }
        else
        {
            acZero[0] = 'C';
        }
    //    iRealLen++;
    }

    iDotLen--;
    iRealLen--;
    while (iDotLen >= 0)
    {
        if (acDot[iDotLen] == '.' || acDot[iDotLen] == '-' ||
            acDot[iDotLen] == '+')
        {
            iDotLen--;
            continue;
        } 

        acZero[iRealLen] = acDot[iDotLen];
        iDotLen--;
        iRealLen--;
    }

    strcpy(szDest, acZero);
    return strlen(acZero);
}


/* ----------------------------------------------------------------
 * 功    能：将形如"000000123456"金额字符串转换成 "1234.56"(实际长)
**           将形如"C000000123456"金额字符串转换成"1234.56"(实际长)
**           将形如"D000000123456"金额字符串转换成"-1234.56"(实际长)
 * 输入参数：szSrc    含C/D符合金额规范的金额域字符串
 *           iOutLen  转换后数字串需要输出长度（位数不足前补空格),
 *                    实际金额长度大于该值则按实际长度输出
 * 输出参数：szDest   含点分隔符的数字串
 * 返 回 值：转换后的数字串实际长度
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ChgAmtZeroToDot(char *szSrc, int iOutLen, char *szDest)
{
    int i, j, iOffset, isHeadZero, iSymbLen, iContLen;
    char szTmp[512], szBuf[512], szSymb[2];

    if (szSrc == NULL || iOutLen < 0 || szDest == NULL)
    {
        return FAIL;
    }


    memset(szTmp, 0, sizeof(szTmp));
    memset(szBuf, 0, sizeof(szBuf));
    memset(szSymb, 0, sizeof(szSymb));

    /* 取符号位 */
    i = 0;
    if (*(szSrc+i) == 'D' || *(szSrc+i) == 'd')
    {
        szSymb[0] = '-'; 
        i++; 
    }
    else if (*(szSrc+i) == 'C' || *(szSrc+i) == 'c')
    {
        i++;
    }
    
    /* 保存串前0之后的有效内容 */
    j = 0;
    isHeadZero = 1;
    while (*(szSrc+i) != '\0')
    {
        if (isHeadZero && *(szSrc+i) == '0')
        {
            i++;
            continue;
        } 

        isHeadZero = 0;
        szTmp[j] = *(szSrc+i);
        i++; j++;
    }

    /* 对保存的临时串分析补位 */
    switch (strlen(szTmp))
    {
    case 0:
        sprintf(szBuf, "0.00");
        break;

    case 1:
        sprintf(szBuf, "0.0%1.1s", szTmp);
        break;
 
    case 2:
        if (szTmp[1] != '0')
            sprintf(szBuf, "0.%2.2s", szTmp);
        else
            sprintf(szBuf, "0.%1.1s", szTmp);
        break;

    default:
        strncpy(szBuf, szTmp, strlen(szTmp) - 2);

        strcat(szBuf, ".");
        strncpy(szBuf + strlen(szBuf), szTmp + strlen(szTmp) - 2, 2);
            
        break;
    }

    iOffset = 0;
    memset(szDest, ' ', iOutLen);

    iSymbLen = strlen(szSymb);
    iContLen = strlen(szBuf);

    if (iOutLen > (iContLen + iSymbLen))
    {
        iOffset = iOutLen - iContLen - iSymbLen;
    }

    sprintf(szDest + iOffset, "%s%s", szSymb, szBuf);
    return (iOffset > 0 ? iOutLen : iContLen + iSymbLen);
}


