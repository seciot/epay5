/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 获取系统流水号
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.2 $
** $Log: GetSysTrace.ec,v $
** Revision 1.2  2012/12/04 01:24:28  fengw
**
** 1、替换ErrorLog为WriteLog。
**
** Revision 1.1  2012/11/23 09:09:16  fengw
**
** 金融交易处理模块初始版本
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：获取系统流水号
** 输入参数：
**        ptApp                 app结构指针
** 输出参数：
**        ptApp->lSysTrace      系统流水号
** 返 回 值：
**        SUCC                  获取流水号成功
**        FAIL                  获取流水号失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/13
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetSysTrace(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        int     iSysTrace;             /* 系统流水号 */
    EXEC SQL END DECLARE SECTION;
	
	/* 开始事务 */
	BeginTran();
	
	/* 查询系统表加锁，控制并发 */
	EXEC SQL
	    SELECT sys_trace INTO :iSysTrace
	    FROM system_parameter FOR UPDATE;
	if(SQLCODE)
	{
	    strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询系统参数表失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

		RollbackTran();

		return FAIL;
	}

    if(iSysTrace >= 999999)
    {
        iSysTrace = 1;
    }
    else
    {
        iSysTrace++;
    }

    EXEC SQL
        UPDATE system_parameter SET sys_trace = :iSysTrace;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "更新系统参数表失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        RollbackTran();

        return FAIL;
    }

    /* 保存流水号到app结构 */
    ptApp->lSysTrace = iSysTrace;

    CommitTran();

    return SUCC;
}
