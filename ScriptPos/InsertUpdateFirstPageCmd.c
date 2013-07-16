/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：终端资料表相关操作函数
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.1 $
 * $Log: InsertUpdateFirstPageCmd.c,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/*****************************************************************
** 功    能:如果原流程代码中存在计算MAC指令/发送数据指令，则将更新首页信息指令插入
            计算MAC指令/发送数据指令之前；否则将更新首页信息追加在原流程代码之后
** 输入参数:
           ptAppStru
** 输出参数:
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
void  InsertUpdateFirstPageCmd( ptAppStru )
T_App *ptAppStru;
{
    int i, iCmdLen, iCmdNum, iLen, iCmdBytes, iFlag;
    uchar ucCmd;
    char    szCmd[100], szTmpStr[256];

    iFlag = 0;

    memcpy( szCmd, ptAppStru->szCommand, ptAppStru->iCommandLen);
    iCmdNum = ptAppStru->iCommandNum;
    iCmdLen = ptAppStru->iCommandLen;

    //查找计算MAC指令/发送数据指令所在位置
    for( i=1, iLen=0; i<=iCmdNum; i++ )
    {
        iCmdBytes = CalcCmdBytes( (uchar)szCmd[iLen] );

        ucCmd = szCmd[iLen]&0x3F;

        //计算MAC指令
        if( ucCmd == 0x0D )
        {
            iFlag = 1;
            break;
        }
        //发送数据指令
        if( ucCmd == 0x24 )
        {
            iFlag = 1;
            break;
        }

        iLen = iLen+iCmdBytes;
    }

    //流程代码中存在计算MAC指令/发送数据指令
    if( iFlag == 1 )
    {
        memcpy( szTmpStr, szCmd, iLen );    
        memcpy( szTmpStr+iLen, "\x9B", 1 );
        memcpy( szTmpStr+iLen+1, szCmd+iLen, iCmdLen-iLen );
        szTmpStr[iCmdLen+1] = 0;
    }
    else
    {
        memcpy( szTmpStr, szCmd, iCmdLen );
        memcpy( szTmpStr+iCmdLen, "\x9B", 1 );
        szTmpStr[iCmdLen+1] = 0;
    }

    ptAppStru->iCommandLen = iCmdLen+1;
    ptAppStru->iCommandNum ++;

    memcpy( ptAppStru->szCommand, szTmpStr, iCmdLen+1 );

    return;
}