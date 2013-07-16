/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 根据响应错误码获取错误信息
** 创 建 人：冯炜
** 创建日期：2012-12-19
**
** $Revision: 1.4 $
** $Log: GetResult.ec,v $
** Revision 1.4  2012/12/20 09:22:33  wukj
** *** empty log message ***
**
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：根据响应错误码获取错误信息
** 输入参数：
**        ptApp                     app结构指针
** 输出参数：
**        szRetDesc                 错误信息
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetResult(char *szRetCode, char *szRetDesc)
{
	EXEC SQL BEGIN DECLARE SECTION;
        char    szReturnCode[2+1];
        char    szReturnName[12+1];
	EXEC SQL END DECLARE SECTION;

    memset(szReturnCode, 0, sizeof(szReturnCode));
	strcpy(szReturnCode, szRetCode);

    memset(szReturnName, 0, sizeof(szReturnName));

	EXEC SQL 
        SELECT return_name 
        INTO :szReturnName 
        FROM error_code 
        WHERE return_code = :szReturnCode;
	if(SQLCODE) 
	{
	    WriteLog(ERROR, "查询错误码[%s]对应错误信息失败!SQLCODE=%d SQLERR=%s",
	             szReturnCode, SQLCODE, SQLERR);

        strcpy(szRetDesc, "未知错误");

		return FAIL;
	}

    DelTailSpace(szReturnName);
	strcpy(szRetDesc, szReturnName);

	return SUCC;
}
