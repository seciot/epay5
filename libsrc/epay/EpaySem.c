/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 信号量操作
** 创 建 人：冯炜
** 创建日期：2012-11-27
**
** $Revision: 1.6 $
** $Log: EpaySem.c,v $
** Revision 1.6  2013/06/28 08:35:16  fengw
**
** 1、添加创建、删除时记录TRACE日志，出现故障时便于跟踪确定问题。
**
** Revision 1.5  2013/06/25 01:57:46  fengw
**
** 1、增加创建成功后记录TRACE日志。
**
** Revision 1.4  2012/12/21 02:11:36  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#include "EpaySem.h"

int CreateEpaySem()
{
    char    szFileName[128+1];

    memset(szFileName, 0, sizeof(szFileName));

    WriteLog(TRACE, "创建EPAY信号量开始");

    GetFullName("WORKDIR", SEM_FILE, szFileName); 

    /* 创建ACCESS信号量 */
    if((giSemAccessID = CreateSem(szFileName, SEM_ACCESS_ID, 1)) == FAIL)
    {
        WriteLog(ERROR, "创建ACCESS信号量失败");
        
        RmSem(giSemAccessID);

        return FAIL;
    }

    /* 创建TDI信号量 */
    if((giSemTdiID = CreateSem(szFileName, SEM_TDI_ID, 1)) == FAIL)
    {
        WriteLog(ERROR, "创建TDI信号量失败");
        
        RmSem(giSemAccessID);
        RmSem(giSemTdiID);

        return FAIL;
    }

    /* 创建HOST信号量 */
    if((giSemHostID = CreateSem(szFileName, SEM_HOST_ID, 1)) == FAIL)
    {
        WriteLog(ERROR, "创建HOST信号量失败");

        RmSem(giSemAccessID);
        RmSem(giSemTdiID);
        RmSem(giSemHostID);

        return FAIL;
    }

    WriteLog(TRACE, "创建EPAY信号量成功");

    return SUCC;
}

int GetEpaySem(int iSemType)
{
    char    szFileName[128+1];

    memset(szFileName, 0, sizeof(szFileName));

    GetFullName("WORKDIR", SEM_FILE, szFileName); 

    switch(iSemType)
    {
        case SEM_ACCESS_ID:
            if(giSemAccessID <= 0 &&
               (giSemAccessID = GetSem(szFileName, SEM_ACCESS_ID)) == FAIL)
            {
                WriteLog(ERROR, "获取ACCESS信号量失败!");

                return FAIL;
            }

            return giSemAccessID;
        case SEM_TDI_ID:
            if(giSemTdiID <= 0 &&
               (giSemTdiID = GetSem(szFileName, SEM_TDI_ID)) == FAIL)
            {
                WriteLog(ERROR, "获取TDI信号量失败!");

                return FAIL;
            }

            return giSemTdiID;
        case SEM_HOST_ID:
            if(giSemHostID <= 0 &&
               (giSemHostID = GetSem(szFileName, SEM_HOST_ID)) == FAIL)
            {
                WriteLog(ERROR, "获取HOSE信号量失败!");

                return FAIL;
            }

            return giSemHostID;
        default:
            WriteLog(ERROR, "信号量类型:[%d]未定义!", iSemType);
            
            return FAIL;
    }
}

int RmEpaySem()
{
    WriteLog(TRACE, "删除EPAY信号量开始");

    /* 删除ACCESS信号量 */
    if(GetEpaySem(SEM_ACCESS_ID) == FAIL)
    {
        WriteLog(ERROR, "获取ACCESS信号量失败!");

        return FAIL;
    }

    if(RmSem(giSemAccessID) != SUCC)
    {
        WriteLog(ERROR, "删除ACCESS信号量失败!");

        return FAIL;
    }

    /* 删除TDI信号量 */
    if(GetEpaySem(SEM_TDI_ID) == FAIL)
    {
        WriteLog(ERROR, "获取TDI信号量失败!");

        return FAIL;
    }

    if(RmSem(giSemTdiID) != SUCC)
    {
        WriteLog(ERROR, "删除TDI信号量失败!");

        return FAIL;
    }

    /* 删除HOST信号量 */
    if(GetEpaySem(SEM_HOST_ID) == FAIL)
    {
        WriteLog(ERROR, "获取HOST信号量失败!");

        return FAIL;
    }

    if(RmSem(giSemHostID) != SUCC)
    {
        WriteLog(ERROR, "删除HOST信号量失败!");

        return FAIL;
    }

    WriteLog(TRACE, "删除EPAY信号量成功");

    return SUCC;
}

int PSem(int iSemType, int iResource)
{
    int     iSemID;

    if((iSemID = GetEpaySem(iSemType)) == FAIL)
    {
        return FAIL;
    }

    return P(iSemID, iResource);
}

int VSem(int iSemType, int iResource)
{
    int     iSemID;

    if((iSemID = GetEpaySem(iSemType)) == FAIL)
    {
        return FAIL;
    }

    return V(iSemID, iResource);
}
