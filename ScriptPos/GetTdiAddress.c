/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 根据报文头获取交易对应app结构指针
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.2 $
 * $Log: GetTdiAddress.c,v $
 * Revision 1.2  2013/06/14 06:32:54  fengw
 *
 * 1、文件格式转换。
 *
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/****************************************************************
** 功    能：根据报文头获取交易对应app结构指针
** 输入参数：
**        szInData                  报文
**        iLen                      报文长度
** 输出参数：
**        无
** 返 回 值：
**        ptApp                     app结构指针
**        NULL                      失败
** 作    者：
**        Robin
** 日    期：
**        2008/07/31
** 调用说明：
**
** 修改日志：
****************************************************************/
T_App* GetTdiAddress(char *szInData, int iLen)
{
    T_App   *ptApp;                     /* app结构指针 */
    int     iStepIndex;                 /* 报文同步序号索引 */
    int     iSteps;                     /* 报文同步序号 */
    int     iCallType;                  /* 呼叫类型 */
    long    lTdi;                       /* 交易数据索引号 */

    /* 判断是否上送电话号码 */
    if(memcmp(szInData+5, "\x4C\x52\x49\x00\x1C", 5) == 0)
    {
        iStepIndex = 38;
    }
    else
    {
        iStepIndex = 5;
    }

    /* 获取报文同步序号 */
    iSteps = (uchar)szInData[iStepIndex];
#ifdef DEBUG
    WriteLog(TRACE, "报文同步序号:[%d]", iSteps);
#endif

    /* 获取呼叫类型 */
    iCallType = (uchar)szInData[iStepIndex+3];
#ifdef DEBUG
    WriteLog(TRACE, "报文呼叫类型:[%d]", iCallType);
#endif
    if(iCallType != POS_CALLING && iCallType != EPAY_CALLING)
    {
        WriteLog(ERROR, "报文呼叫类型[%d]非法!终端主动:[%d] 平台主动:[%d]",
                 iCallType, POS_CALLING, EPAY_CALLING);

        return NULL;
    }

    /* 终端发起首个交易包，需要中心分配交易数据索引号 */
    if(iSteps == 1)
    {
        lTdi = GetTransDataIndex(giTimeoutTdi);
        if(lTdi < 0)
        {
            WriteLog(ERROR, "分配交易数据索引号失败!");

            return NULL;
        }
    }
    else
    {
        lTdi = (uchar)szInData[iStepIndex+1]*256 + (uchar)szInData[iStepIndex+2];

        /* 交易数据索引号非法，判定为包非法 */
        if(lTdi >= MAX_TRANS_DATA_INDEX)
        {
            WriteLog(ERROR, "交易数据索引号[%d]非法!许可范围[0-%ld]", lTdi, MAX_TRANS_DATA_INDEX-1);

            return NULL;
        }

        /* 重置TDI占用时间 */
        SetTdiTime(lTdi);
    }
#ifdef DEBUG
    WriteLog(TRACE, "交易数据索引号:[%ld]", lTdi);
#endif

    /* 根据交易数据索引获取结构指针 */
    ptApp = GetAppAddress(lTdi); 
    if(ptApp == NULL)
    {
        WriteLog(ERROR, "获取交易数据索引[%ld]对应结构指针失败!", lTdi);

        return NULL;
    }

    /* 终端发起交易第1步或中心发起交易的第2步，初始化公共数据结构 */
    if(iSteps == 1 || ((iCallType == EPAY_CALLING)&&(iSteps == 2)))
    {
        memset(ptApp, 0, APPSIZE);
    }
    /* 后续交易清空上回合交易指令数据 */
    else
    {
        memset(ptApp->szCommand, 0, sizeof(ptApp->szCommand));    
        ptApp->iCommandNum = 0;
        ptApp->iCommandLen = 0;
        
        memset(ptApp->szControlPara, 0, sizeof(ptApp->szControlPara));    
        ptApp->iControlLen = 0;
    }

    /* 初始赋值 */
    /* 报文同步序号 */
    ptApp->iSteps = iSteps;

    /* 呼叫类型 */
    ptApp->iCallType = iCallType;

    /* 交易数据索引 */
    ptApp->lTransDataIdx = lTdi;

    /* 交易日期、时间 */
    GetSysDate(ptApp->szPosDate);
    GetSysTime(ptApp->szPosTime);

    /* 默认响应码NN */
    strcpy(ptApp->szRetCode, "NN");
    strcpy(ptApp->szHostRetCode, "NN");
    memset(ptApp->szRetDesc, 0, sizeof(ptApp->szRetDesc));

    /* 收单行号 */
    strcpy(ptApp->szAcqBankId, gszAcqBankId);

    return ptApp;
}
