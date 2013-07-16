/******************************************************************
 ** Copyright(C)2009－2012 福建联迪商用设备有限公司
 ** 主要内容：日志打印。 
 ** 创 建 人：zhangwm
 ** 创建日期：2012/12/03
 **
 ** ---------------------------------------------------------------
 **   $Revision: 1.5 $
 **   $Log: EpayLog.c,v $
 **   Revision 1.5  2012/11/29 07:09:22  zhangwm
 **
 **   增加判断是否打印日志
 **
 **   Revision 1.4  2012/11/28 07:21:54  zhangwm
 **
 **   修改监控日志显示终端号而非商户号
 **
 **   Revision 1.3  2012/11/28 03:01:57  zhangwm
 **
 **   修改时间处理函数以及金额处理函数为新版本
 **
 **   Revision 1.2  2012/11/27 06:13:41  zhangwm
 **
 **   将写APP日志功能移除
 **
 **   Revision 1.1  2012/11/27 03:49:52  zhangwm
 **
 **   增加epay日志打印函数
 **
 ** ---------------------------------------------------------------
 **
 *******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef LINUX
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include "EpayLog.h"
#include "user.h"
#include "app.h"

/*****************************************************************
 ** 功    能：打印16进制数据
 ** 输入参数：
 **        pszLogData 要打印的内容
 **        iLen 打印内容的长度 
 **        pszTitle 打印的内容头 
 ** 输出参数：
 **        无
 ** 返 回 值：
 **        无 
 ** 作    者：zhangwm
 ** 日    期：2012/12/03
 ** 修改日志：
 **          1、2012/12/03 初始创建 
 ****************************************************************/
void WriteHdLog(char* pszLogData, int iLen, char* pszTitle)
{
    char szTemp[100];
    char szTime[7], szLogData[DATA_LEN];
    int i, j, iPos, iStep;

    if (IsPrint(DEBUG_HLOG) == NO)
    {
        return;
    }

    iPos = 0;
    /* 该函数后期使用按C语言规范命名后的函数 */
    GetSysTime(szTime);

    sprintf(szTemp, "%s  %s\n", pszTitle, szTime);
    sprintf(szLogData + iPos, "%s", szTemp);
    iPos += strlen(szTemp);

    sprintf(szTemp, "%s", "===== =1==2==3==4==5==6==7==8=Hex=0==1==2==3==4==5==6 ====Asc Value====\n");
    sprintf(szLogData + iPos, "%s", szTemp);
    iPos += strlen(szTemp);

    for (i=0; i<iLen/16; i++)
    {
        iStep = 0;
        sprintf(szTemp + iStep, "%04xh:", i*16);
        iStep += 6;

        for (j=0; j<16; j++)
        {
            sprintf(szTemp + iStep, "%02x ", (unsigned char )pszLogData[i*16+j]);
            iStep += 3;
        }
        sprintf(szTemp + iStep, "%s", "|");
        iStep += 1;

        for (j=0; j<16; j++)
        {
            if (pszLogData[i*16+j] >= 0x30 && pszLogData[i*16+j] <= 0x7e)
            {
                sprintf(szTemp + iStep, "%c", (unsigned char )pszLogData[i*16+j]);
            }
            else
            {
                sprintf(szTemp + iStep, "%s", ".");
            }
            iStep += 1;
        }

        sprintf(szTemp + iStep, "%s", "\n");
        iStep += 1;

        sprintf(szLogData + iPos, "%s", szTemp);
        iPos += iStep;
    }

    /* 数据长度不是16的倍数，最后一行不足补空格 */    
    if (iLen % 16 != 0)
    {
        iStep = 0;
        sprintf(szTemp + iStep, "%04xh:", i*16);
        iStep += 6;

        for (j=0; j< iLen%16; j++)
        {
            sprintf(szTemp + iStep, "%02x ", (unsigned char )pszLogData[i*16+j]);
            iStep += 3;
        }
        for (j=0; j<(48-(iLen%16)*3); j++)
        {
            sprintf(szTemp + iStep, "%s", " ");
            iStep += 1;
        }

        sprintf(szTemp + iStep, "%s", "|");
        iStep += 1;

        for (j=0; j<iLen%16; j++)
        {
            if (pszLogData[i*16+j] >= 0x30 && pszLogData[i*16+j] <= 0x7e)
            {
                sprintf(szTemp + iStep, "%c", (unsigned char )pszLogData[i*16+j]);
            }
            else
            {
                sprintf(szTemp + iStep, "%s", ".");
            }
            iStep += 1;
        }

        sprintf(szTemp + iStep, "%s", "\n");
        iStep += 1;

        sprintf(szLogData + iPos, "%s", szTemp);
        iPos += iStep;
    }

    sprintf(szTemp, "%s", "===== =============================================== =================\n\n");
    sprintf(szLogData + iPos, "%s", szTemp);
    iPos += strlen(szTemp);

    PrintLog(szLogData, H_TYPE);

    return;
}

/*****************************************************************
 ** 功    能：打印交易监控数据
 ** 输入参数：
 **        tpApp 交易结构体
 ** 输出参数：
 **        无
 ** 返 回 值：
 **        无 
 ** 作    者：zhangwm
 ** 日    期：2012/12/03
 ** 修改日志：
 **          1、2012/12/03 初始创建 
 ****************************************************************/
void WriteMoniLog(T_App* tpApp, char* pszTransName)
{
    char szLogData[DATA_LEN], szAmount[14], szTransName[9];
    char szResult[23], szTime[9];
    unsigned long lAmount;

    if (IsPrint(DEBUG_MLOG) == NO)
    {
        return;
    }

    memset(szResult, 0, sizeof(szResult));
    memset(szLogData, 0, sizeof(szLogData));

    if (strcmp(tpApp->szRetCode, "00") != 0)
    {
        strcpy(szResult, tpApp->szRetCode);
        if (strlen(tpApp->szRetDesc) != 0)
        {    
            strcat(szResult, tpApp->szRetDesc);
        }
        else
        {
            strcat(szResult, "交易失败");
        }
    }
    else
    {
        strcpy(szResult, "00");
        strcat(szResult, "交易成功");
    }

    strcat(szResult, tpApp->szHostRetCode);

    GetSysDTFmt("%T", szTime);

    if (pszTransName == NULL)
    {
        strcpy(szTransName, tpApp->szTransName);
    }
    else
    {
        strcpy(szTransName, pszTransName);
    }

    lAmount = atol(tpApp->szAmount);
    if (lAmount != 0)
    {    
        ChgAmtZeroToDot(tpApp->szAmount, 0, szAmount);
        sprintf(szLogData, "  %8.8s %-22.22s %-8.8s %13s %-16.16s %8.8s\n", tpApp->szPosNo, tpApp->szPan, szTransName, szAmount, szResult, szTime );
    }
    else if (strlen(tpApp->szAddiAmount) > 0)
    {
        ChgAmtZeroToDot(tpApp->szAddiAmount, 0, szAmount);
        sprintf(szLogData, "  %8.8s %-22.22s %-8.8s %13s %-16.16s %8.8s\n", tpApp->szPosNo, tpApp->szPan, szTransName, szAmount, szResult, szTime );
    }
    else
    {
        sprintf(szLogData, "  %8.8s %-22.22s %-8.8s %13s %-16.16s %8.8s\n", tpApp->szPosNo, tpApp->szPan, szTransName, " ", szResult, szTime );
    }

    PrintLog(szLogData, M_TYPE);

    return;
}
