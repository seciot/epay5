/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 发送交易请求
** 创 建 人：冯炜
** 创建日期：2012-11-13
**
** $Revision: 1.3 $
** $Log: SendVoidReq.c,v $
** Revision 1.3  2012/12/21 01:57:30  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#define _EXTERN_

#include "autovoid.h"

int SendVoidReq(T_App *ptApp, int iTimeOut)
{
    long    lTransDataIndex;

    if(SetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        return FAIL;
    }

    /* 发送交易请求 */
    if(SendAccessToProcQue(ptApp->lAccessToProcMsgType, ptApp->lTransDataIdx) != SUCC) 
    {
        WriteLog(ERROR, "发送自动冲正交易请求失败!");

        return FAIL;
    }

    /* 等待后台返回应答 */
    /* 循环接收后台应答，直至接收到正确应答或超时 */
    while(1)
    {
        if(RecvProcToAccessQue(ptApp->lProcToAccessMsgType, iTimeOut, &lTransDataIndex) != SUCC)
        {
            WriteLog(ERROR, "冲正交易 PsamNo:[%s] PosTrace:[%ld] 超时，等待下次冲正!",
                     ptApp->szPsamNo, ptApp->lPosTrace);

            return SUCC;
        }

        /* 判断数据索引值是否一致 */
        /* 不一致表示接收到错误应答包，继续等待接收 */
        if(ptApp->lTransDataIdx != lTransDataIndex)
        {
            WriteLog(ERROR, "冲正交易应答匹配失败!TransDataIdx ToTrans:[%ls] TransDataIdx FromHost:[%ld]",
                     ptApp->lTransDataIdx, lTransDataIndex);

            continue;
        }

        break;
    }

    /* 将共享内存数据拷贝到app结构 */
    if(GetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        WriteLog(ERROR, "从共享内存拷贝app结构失败!TransDataIdx:[%ld]",
                 ptApp->lTransDataIdx);

        return FAIL;
    }

    return SUCC;
}
