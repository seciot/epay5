/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 终端合法性检查
** 创 建 人：冯炜
** 创建日期：2012-11-12
**
** $Revision: 1.3 $
** $Log: ChkPosValid.ec,v $
** Revision 1.3  2013/06/14 02:32:57  fengw
**
** 1、查询终端批次号时，增加空值判断。
**
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
** 功    能：终端合法性检查
** 输入参数：
**        ptApp->szShopNo       商户号
**        ptApp->szPosNo        终端号
** 输出参数：
**        ptApp->lBatchNo       终端批次号
**        gszTelephone          绑定电话号码
** 返 回 值：
**        SUCC                  检查成功
**        FAIL                  检查失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/12
** 调用说明：
**
** 修改日志：
****************************************************************/
int ChkPosValid(T_App *ptApp)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[15+1];                 /* 商户号 */
        char    szPosNo[15+1];                  /* 终端号 */
        int     iStatus;                        /* 终端状态 */
        int     iBatchNo;                       /* 终端当前批次号 */
        char    szTelephone[15+1];              /* 终端绑定电话号码 */
    EXEC SQL END DECLARE SECTION;

	/* 读取终端资料 */
	memset(szShopNo, 0, sizeof(szShopNo));
	memset(szPosNo, 0, sizeof(szPosNo));
    memset(szTelephone, 0, sizeof(szTelephone));

    strcpy(szShopNo, ptApp->szShopNo);
    strcpy(szPosNo, ptApp->szPosNo);

	EXEC SQL
	    SELECT telephone, status, NVL(cur_batch, 0)
        INTO :szTelephone, :iStatus, :iBatchNo
	  	FROM terminal
	  	WHERE shop_no = :szShopNo AND pos_no = :szPosNo;
	if(SQLCODE == SQL_NO_RECORD)
	{
	    strcpy(ptApp->szRetCode, ERR_INVALID_TERM);

		WriteLog(ERROR, "终端资料未登记!商户号:[%s] 终端号:[%s]", szShopNo, szPosNo);

		return FAIL;
	}
	else if(SQLCODE == SQL_SELECT_MUCH)
	{
	    strcpy(ptApp->szRetCode, ERR_DUPLICATE_TERM);

        WriteLog(ERROR, "终端资料重复登记!商户号:[%s] 终端号:[%s]", szShopNo, szPosNo);

		return FAIL;
	}
	else if(SQLCODE)
	{
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

		WriteLog(ERROR, "查询终端资料 商户号:[%s] 终端号:[%s] 失败!SQLCODE=%d SQLERR=%s",
		         szShopNo, szPosNo, SQLCODE, SQLERR);

		return FAIL;
	}
	
	/* 检查终端状态 */
	if(iStatus != 1)
	{
	    strcpy(ptApp->szRetCode, ERR_TERM_STATUS);

	    WriteLog(ERROR, "终端非正常状态!状态[%d]", iStatus);

		return FAIL;
	}

    /* 保存终端信息 */
	ptApp->lBatchNo = iBatchNo;
	strcpy(gszTelephone, szTelephone);

    return SUCC;
}