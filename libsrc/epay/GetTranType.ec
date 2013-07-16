/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 根据交易码获取交易类型
** 创 建 人：冯炜
** 创建日期：2012-12-20
**
** $Revision: 1.6 $
** $Log: GetTranType.ec,v $
** Revision 1.6  2012/12/25 07:04:52  fengw
**
** 1、参数szTransCode类型游址拚氡嘈碆UG。
**
** Revision 1.5  2012/12/20 09:25:54  wukj
** Revision后的美元符
**
** Revision 1.4  2012/12/20 09:20:59  wukj
** *** empty log message ***
**
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "errcode.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：根据交易码获取交易类型
** 输入参数：
**        szTransCode               交易码
** 输出参数：
**        piTransType               交易类型
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/20
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetTranType(char *szTransCode, int *piTransType)
{
	EXEC SQL BEGIN DECLARE SECTION;
        char    szHostTransCode[8+1];           /* 交易码 */
		int     iTransType;                     /* 交易类型 */
	EXEC SQL END DECLARE SECTION;

    memset(szHostTransCode, 0, sizeof(szHostTransCode));
    memcpy(szHostTransCode, "__", 2);
    memcpy(szHostTransCode+2, szTransCode+2, 6);

	EXEC SQL
	    SELECT trans_type INTO :iTransType
        FROM trans_def 
        WHERE trans_code LIKE :szHostTransCode;
    if(SQLCODE == SQL_NO_RECORD)
	{
        WriteLog(ERROR, "交易码[%s]未定义!SQLCODE=%d SQLERR=%s", szHostTransCode, SQLCODE, SQLERR);

		return FAIL;
	}
	else if(SQLCODE)
	{
		WriteLog(ERROR, "查询 交易码[%s] 交易定义失败!SQLCODE=%d SQLERR=%s", szHostTransCode, SQLCODE, SQLERR);

		return FAIL;
	}

	*piTransType = iTransType;

	return SUCC;
}
