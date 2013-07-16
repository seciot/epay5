/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块 向manatran发起交易请求
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.3 $
** $Log: SendWebReq.c,v $
** Revision 1.3  2012/12/21 02:05:32  fengw
**
** 1、将文件格式从DOS转为UNIX。
**
** Revision 1.2  2012/12/21 02:04:03  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#define _EXTERN_

#include "comweb.h"

/****************************************************************
** 功    能：向manatran发起交易请求，并接收交易应答
** 输入参数：
**        ptApp                 app结构指针
**        lTimeOut              交易超时时间
** 输出参数：
**        无
** 返 回 值：
**        SUCC                  成功
**        FAIL                  失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/18
** 调用说明：
**
** 修改日志：
****************************************************************/
int SendWebReq(T_App *ptApp, long lTimeOut)
{
    int     iRet;                           /* 函数结果 */
    int     lTransDataIdx;                  /* 交易数据索引 */

    /* 判断IP地址 */
    if(strlen(ptApp->szIp) < 7)
    {
        WriteLog(ERROR, "商户[%s] 终端[%s] 登记IP地址错误!",
                 ptApp->szShopNo, ptApp->szPosNo, ptApp->szIp);

        strcpy(ptApp->szRetCode, ERR_TERM_NOT_REGISTER);

        return FAIL;
    }

    /* 设置接收应答消息类型 */
    ptApp->lProcToAccessMsgType = getpid();

    /* 将app结构数据保存至共享内存 */
    if(SetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "保存app结构到共享内存失败!TransDataIdx:[%ld]",
                 ptApp->lTransDataIdx);

        return FAIL;
    }

    /* 发送交易请求 */
    iRet = SendAccessToProcQue(ptApp->lAccessToProcMsgType, ptApp->lTransDataIdx);
    if(iRet != SUCC)
    {
        WriteLog(ERROR, "发送交易请求失败!MsgType:[%ls] TransDataIdx:[%ld]",
                 ptApp->lAccessToProcMsgType, ptApp->lTransDataIdx);

        return FAIL;
    }

    /* 接收交易应答 */
    /* 循环接收应答，直至接收到正确应答或超时 */
    while(1)
    {
        iRet = RecvProcToAccessQue(ptApp->lProcToAccessMsgType, lTimeOut, &lTransDataIdx);
        if(iRet == TIMEOUT)
        {
            strcpy(ptApp->szRetCode, ERR_TIMEOUT);

            WriteLog(ERROR, "接收交易应答超时!MsgType:[%ls]", ptApp->lProcToAccessMsgType);

            return FAIL;
        }

        /* 判断数据索引值是否一致 */
        /* 不一致表示接收到错误应答包，继续等待接收 */
        if(ptApp->lTransDataIdx != lTransDataIdx)
        {
            WriteLog(ERROR, "交易应答匹配失败!TransDataIdx ToTrans:[%ls] TransDataIdx FromHost:[%ld]",
                     ptApp->lTransDataIdx, lTransDataIdx);

            continue;
        }

        break;
    }

    /* 将共享内存数据拷贝到app结构 */
    if(GetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "从共享内存拷贝app结构失败!TransDataIdx:[%ld]",
                 ptApp->lTransDataIdx);

        return FAIL;
    }

    return SUCC;
}