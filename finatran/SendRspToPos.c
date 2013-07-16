/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 返回交易应答
** 创 建 人：冯炜
** 创建日期：2012-11-13
**
** $Revision: 1.7 $
** $Log: SendRspToPos.c,v $
** Revision 1.7  2013/06/14 06:26:30  fengw
**
** 1、增加监控日志记录代码。
**
** Revision 1.6  2013/01/05 06:37:25  fengw
**
** 1、添加SetApp函数调用。
**
** Revision 1.5  2012/12/25 06:54:43  fengw
**
** 1、修改web交易监控通讯端口号变量类型为字符串。
**
** Revision 1.4  2012/12/07 02:04:10  fengw
**
** 1、修改消息队列发送函数。
** 2、增加发送web监控。
**
** Revision 1.3  2012/11/26 01:33:05  fengw
**
** 1、修改文件名后缀为ec，重新上传该模块代码
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

/****************************************************************
** 功    能：返回交易应答
** 输入参数：
**        ptApp                app结构指针
** 输出参数：
**        无
** 返 回 值：
**        无
** 作    者：
**        fengwei
** 日    期：
**        2012/11/13
** 调用说明：
**
** 修改日志：
****************************************************************/
void SendRspToPos(T_App *ptApp)
{
    /* 将app结构数据保存至共享内存 */
    if(SetApp(ptApp->lTransDataIdx, ptApp) != SUCC)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "保存app结构到共享内存失败!TransDataIdx:[%ld]",
                 ptApp->lTransDataIdx);

        return;
    }

    SendProcToAccessQue(ptApp->lProcToAccessMsgType, ptApp->lTransDataIdx);

    /* 登记监控信息 */

    WebDispMoni(ptApp, ptApp->szTransName, gszMoniIP, gszMoniPort);

    WriteMoniLog(ptApp, ptApp->szTransName);

    return;
}
