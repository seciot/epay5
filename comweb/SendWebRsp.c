/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块 向web发送交易应答
** 创 建 人：冯炜
** 创建日期：2012-12-19
**
** $Revision: 1.4 $
** $Log: SendWebRsp.c,v $
** Revision 1.4  2012/12/26 08:33:21  fengw
**
** 1、增加通讯原始数据日志记录。
**
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
** 功    能：向web发送交易应答
** 输入参数：
**        ptApp                 app结构指针
**        iSockFd               socket描述符
** 输出参数：
**        无
** 返 回 值：
**        SUCC                  成功
**        FAIL                  失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int SendWebRsp(T_App *ptApp, int iSockFd)
{
    char    szLenBuf[2+1];                      /* 报文长度Buf */
    int     iRet;                               /* 函数调用结果 */
    int     iLen;                               /* 报文长度 */
    char    szRspBuf[MAX_SOCKET_BUFLEN+1];      /* 应答报文 */

    /* 应答报文组包 */
    memset(szRspBuf, 0, sizeof(szRspBuf));
    iLen = PackWebRsp(ptApp, szRspBuf);

    /* 将app结构数据保存至共享内存 */
    if(SetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "保存app结构到共享内存失败!TransDataIdx:[%ld]",
                 ptApp->lTransDataIdx);

        return FAIL;
    }

    /* 发送应答报文 */
	memset(szLenBuf, 0, sizeof(szLenBuf));
	szLenBuf[0] = iLen / 256;
	szLenBuf[1] = iLen % 256;

    iRet = WriteSock(iSockFd, szLenBuf, 2, 0);
    if(iRet != 2)
    {
        WriteLog(ERROR, "发送Web交易应答报文长度失败!iRet:[%d]", iRet);

        return FAIL;
    }

    iRet = WriteSock(iSockFd, szRspBuf, iLen , 0);
    if(iRet != iLen)
    {
        WriteLog(ERROR, "发送Web交易应答报文失败!预期发送长度:[%d] 实际发送长度:[%d]", iLen, iRet);

        return FAIL;
    }

    /* 记录原始通讯日志 */
    WriteHdLog(szRspBuf, iLen, "comweb send web rsp");

    return SUCC;
}