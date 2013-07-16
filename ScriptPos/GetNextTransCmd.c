/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 POS应答报文组包
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.1 $
 * $Log: GetNextTransCmd.c,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"
  
int GetNextTransCmd(T_App *ptApp)
{
    int     iTransType;                 /* 后续交易类型 */
    char    szCmd[512+1];               /* 后续指令 */
    int     iCmdCount;                  /* 指令个数 */
    int     iLenCmd;                    /* 指令长度 */
    char    szDataSource[32+1];         /* 指令数据源 */
    int     iDSCount;                   /* 指令数据源个数 */
    int     iCtlLen;                    /* 控制参数长度 */
    int     szCtlPara[128+1];           /* 控制参数 */

    /* 查找后续交易类型 */
    if(GetTranType(ptApp->szNextTransCode, &iTransType) != SUCC)
    {
        WriteLog( ERROR, "get next trans type fail" );

        return FAIL;
    }

    /* 查找后续交易指令 */
    memset(szCmd, 0, sizeof(szCmd));
    memset(szDataSource, 0, sizeof(szDataSource));
    memset(szCtlPara, 0, sizeof(szCtlPara));
    iCmdCount = 0;
    iLenCmd = 0;
    iDSCount = 0;
    iCtlLen = 0;

    if(GetCommands(iTransType, '0', szCmd, &iCmdCount,
                   &iLenCmd, szDataSource, &iDSCount, &iCtlLen, szCtlPara) != SUCC)
    {
        WriteLog(ERROR, "获取交易类型[%d]后续指令失败", iTransType);

        return FAIL;
    }

    /* 后续指令 */
    memcpy(ptApp->szCommand+ptApp->iCommandLen, szCmd, iLenCmd);
    ptApp->iCommandLen += iLenCmd;
    ptApp->iCommandNum += iCmdCount;

    /* 控制参数 */
    memcpy(ptApp->szControlPara + ptApp->iControlLen, szCtlPara, iCtlLen);
    ptApp->iControlLen += iCtlLen;
    
    /* 数据来源 */
    memcpy(ptApp->szDataSource+ptApp->iDataNum, szDataSource, iDSCount);
    ptApp->iDataNum += iDSCount;
    
    return SUCC;
}