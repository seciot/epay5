/*****************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台系统状态监控模块 获取消息队列信息
** 创 建 人：冯炜
** 创建日期：2012/10/30
**
** $Revision: 1.4 $
** $Log: GetEpayStatus.c,v $
** Revision 1.4  2012/12/21 02:08:15  fengw
**
** 1、增加Revision、Log。
**
*******************************************************************/

#define _EXTERN_

#include "epaymoni.h"

/****************************************************************
** 功    能：检查进程状态
** 输入参数：
**        szFileName            详细信息文件名
** 输出参数：
**        szProcStatus          进程状态结果信息
**        szMsgStatus           消息队列状态结果信息
**        szCommStatus          通讯端口状态结果信息
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/10/31
** 调用说明：
**        状态详细信息直接写入文件
** 修改日志：
****************************************************************/
int GetEpayStatus(char *szFileName, char *szProcStatus, char *szMsgStatus, char *szCommStatus)
{
    /* 打开详细信息文件 */
    fpStatusFile = fopen(szFileName, "w");
    if(fpStatusFile == NULL)
    {
        WriteLog(ERROR, "创建详细信息文件[%s]失败!", szFileName);

        return FAIL;
    }

    /* 获取进程监控信息 */
    memset(szProcStatus, 0, sizeof(szProcStatus));
    if(GetProcStatus(szProcStatus) != SUCC)
    {
        WriteLog(ERROR, "生成进程状态信息失败!");

        fclose(fpStatusFile);

        return FAIL;
    }

    /* 获取消息队列监控信息 */
    memset(szMsgStatus, 0, sizeof(szMsgStatus));
    if(GetMsgStatus(szMsgStatus) != SUCC)
    {
        WriteLog(ERROR, "生成消息队列状态信息失败!");

        fclose(fpStatusFile);

        return FAIL;
    }

    /* 获取通讯端口监控信息 */
    memset(szCommStatus, 0, sizeof(szCommStatus));
    if(GetCommStatus(szCommStatus) != SUCC)
    {
        WriteLog(ERROR, "生成通讯端口状态信息失败!");

        fclose(fpStatusFile);

        return FAIL;
    }

    fclose(fpStatusFile);

    return SUCC;
}
