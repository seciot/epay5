/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块 应答报文组包
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.3 $
** $Log: PackWebRsp.c,v $
** Revision 1.3  2012/12/21 07:33:09  fengw
**
** 1、响应信息长度改为16进制格式。
**
** Revision 1.2  2012/12/21 02:04:03  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#define _EXTERN_

#include "comweb.h"

/****************************************************************
** 功    能：应答报文组包
** 输入参数：
**        ptApp                 app结构指针
** 输出参数：
**        szRspBuf              交易应答报文
** 返 回 值：
**        >0                    报文长度
**        FAIL                  失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/18
** 调用说明：
**
** 修改日志：
****************************************************************/
int PackWebRsp(T_App *ptApp, char *szRspBuf)
{
    int     iIndex;                 /* buf索引 */
    int     iMsgCount;              /* 短信记录数 */
    int     iRetDescLen;            /* 响应信息长度 */

    iIndex = 0;

    /* 交易代码 */
    memcpy(szRspBuf+iIndex, ptApp->szTransCode, 8);
    iIndex += 8;

    /* 响应码 */
    memcpy(szRspBuf+iIndex, ptApp->szRetCode, 2);
    iIndex += 2;

    /* 根据返回码取返回信息 */
	if(strlen(ptApp->szRetDesc) == 0)
	{
		GetResult(ptApp->szRetCode, ptApp->szRetDesc);
	}

    /* 响应信息长度 */
    iRetDescLen = strlen(ptApp->szRetDesc);
    szRspBuf[iIndex] = iRetDescLen;
    iIndex += 1;

    /* 响应信息 */
	memcpy(szRspBuf+iIndex, ptApp->szRetDesc, iRetDescLen);
    iIndex += iRetDescLen;

    return iIndex;
}