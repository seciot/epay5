/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：返回终端应答报文
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.1 $
 * $Log: SendOutToPos.c,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/*******************************************************************************
 * 函数功能：返回终端应答报文
 * 输入参数：
 *           ptApp  - 公共数据结构
 * 输出参数：
 *           无
 * 返 回 值： 
 *           SUCC               成功
 *           FAIL               失败 
 * 作    者：Robin
 * 日    期：2012/11/20
 *
 ******************************************************************************/

int SendOutToPos(T_App *ptApp, int iSockFd)
{
    uchar   szSndBuf[BUFFSIZE];         /* 应答报文 */
    int     iLen;                       /* 应答报文长度 */
    char    szFirstPage[256+1];         /* 首页信息 */
    int     iFirstPageLen;              /* 首页信息长度 */

    /* 检查首页信息 */
    memset(szFirstPage, 0, sizeof(szFirstPage));
    iFirstPageLen = 0;
    CheckFirstPage(ptApp, &iFirstPageLen, szFirstPage);

    iLen = PackScriptPos(ptApp, szFirstPage, iFirstPageLen, szSndBuf);
    if(iLen == FAIL)
    {
        WriteLog(ERROR, "POS应答报文组包失败!");

        FreeTdi(ptApp->lTransDataIdx);

        return FAIL;
    }

    WriteHdLog(szSndBuf, iLen, "ScriptPos send to pos");

    if(WriteSockAddLenField(iSockFd, szSndBuf, iLen, 0, 2, HEX_DATA) == FAIL)
    {
        WriteLog(ERROR, "发送POS应答报文组包失败!");

        FreeTdi(ptApp->lTransDataIdx);

        return FAIL;
    }

    if(strlen(ptApp->szNextTransCode) == 0)
    {
        FreeTdi(ptApp->lTransDataIdx);
    }

    return SUCC;
}