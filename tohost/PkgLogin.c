/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台银联POS通讯模块 签到交易组包、拆包
** 创 建 人：冯炜
** 创建日期：2013-06-08
**
** $Revision: 1.2 $
** $Log: PkgLogin.c,v $
** Revision 1.2  2013/06/17 05:19:45  fengw
**
** 1、修改应答报文第13域交易日期处理代码。
**
** Revision 1.1  2013/06/14 02:04:51  fengw
**
** 1、增加签到、余额查询、消费交易报文组包、拆包处理。
**
*******************************************************************/

#define _EXTERN_

#include "tohost.h"

/****************************************************************
** 功    能：签查询打包
** 输入参数：
**        ptMsgRule       报文规则
**        ptApp           app结构
** 输出参数：
**        ptData          ISO结构指针
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2013/06/08
** 调用说明：
**
** 修改日志：
****************************************************************/
int LoginPack(MsgRule *ptMsgRule, T_App *ptApp, ISO_data *ptData)
{
    char    szTmpBuf[512+1];
    int     iIndex;

	/* 0域 消息类型 */
    if(SetBit(ptMsgRule, "0800", MSG_ID, 4, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, MSG_ID, "0800");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	/* 11域 受卡方系统跟踪号 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	sprintf(szTmpBuf, "%06ld", ptApp->lSysTrace);
    if(SetBit(ptMsgRule, szTmpBuf, POS_TRACE, 6, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, POS_TRACE, szTmpBuf);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	/*41域 受卡方终端标识码*/
    if(SetBit(ptMsgRule, ptApp->szPosNo, POS_ID, strlen(ptApp->szPosNo), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, POS_ID, ptApp->szPosNo);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }
    
	/* 42域 受卡方标识码 */
    if(SetBit(ptMsgRule, ptApp->szShopNo, CUSTOM_ID, strlen(ptApp->szShopNo), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, CUSTOM_ID, ptApp->szShopNo);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 60域 自定义域 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    iIndex = 0;

	/* 60.1交易类型码 */
	sprintf(szTmpBuf, "00");
	iIndex += 2;

	/* 60.2 批次号 */
	sprintf(szTmpBuf+iIndex, "%06ld", ptApp->lBatchNo);
	iIndex += 6;

	/*60.3 网络管理码3*/
    sprintf(szTmpBuf+iIndex, "003");
    iIndex += 3;

    if(SetBit(ptMsgRule, szTmpBuf, FIELD60, iIndex, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, FIELD60, szTmpBuf);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 62域 终端信息 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    strcpy(szTmpBuf, "Sequence No12311611111111");

    if(SetBit(ptMsgRule, szTmpBuf, FIELD62, strlen(szTmpBuf), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, FIELD62, szTmpBuf);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	/* 63域 操作员代码 */
    if(SetBit(ptMsgRule, "000", FIELD63, 3, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, FIELD63, "000");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	return SUCC;
}

/****************************************************************
** 功    能：签到拆包
** 输入参数：
**        ptMsgRule       报文规则
**        ptData          ISO结构指针
** 输出参数：
**        ptApp           app结构
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        fengwei
** 日    期：
**        2013/06/08
** 调用说明：
**
** 修改日志：
****************************************************************/
int LoginUnpack(MsgRule *ptMsgRule, T_App *ptApp, ISO_data *ptData)
{
	char    szTmpBuf[512+1];
    char    szTmpBuf2[512+1];
	int     iRet;
	T_WorkKey   tWorkKey;

	/* 11域 流水号 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, POS_TRACE, szTmpBuf);
	if(iRet != 6)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, POS_TRACE, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
	ptApp->lSysTrace = atol(szTmpBuf);

	/* 12域 交易时间 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, LOCAL_TIME, szTmpBuf);
	if(iRet == 6)
	{
		strcpy(ptApp->szHostTime, szTmpBuf);
	}
	else
	{
       	GetSysTime(ptApp->szHostTime);
   	}

	/* 13域 交易日期 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    GetSysDate(ptApp->szHostDate);
    iRet = GetBit(ptMsgRule, ptData, LOCAL_DATE, szTmpBuf);
    if(iRet == 4)
    {   
        strcpy(ptApp->szHostDate+4, szTmpBuf);
    }  
    strcpy(ptApp->szSettleDate, ptApp->szHostDate);

	/* 32域 受理方标识码 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, ACQUIRER_ID, szTmpBuf);
    if(iRet > 11 || iRet < 1)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, ACQUIRER_ID, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}

	/* 37域 检索参考号 后六位是凭证号 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, RETR_NUM, szTmpBuf);
	if(iRet != 12)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, RETR_NUM, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
    sprintf(ptApp->szRetriRefNum, "%12.12s", szTmpBuf);

    /* 39域 响应码 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    iRet = GetBit(ptMsgRule, ptData, RET_CODE, szTmpBuf);
	if(iRet != 2)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, RET_CODE, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
	strcpy(ptApp->szHostRetCode, szTmpBuf);

	/* 41域 终端号 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, POS_ID, szTmpBuf);
	if(iRet != 8)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, POS_ID, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
    strcpy(ptApp->szPosNo, szTmpBuf);

	/* 42域 商户号 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, CUSTOM_ID, szTmpBuf);
	if(iRet != 15)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, CUSTOM_ID, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
    strcpy(ptApp->szShopNo, szTmpBuf);

	/* 60.2域 批次号 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, FIELD60, szTmpBuf);
    if(iRet < 1 || iRet > 11)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, FIELD60, iRet);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
	}
	memset(szTmpBuf2, 0, sizeof(szTmpBuf2));
	memcpy(szTmpBuf2, szTmpBuf+2, 6);
	ptApp->lBatchNo = atol(szTmpBuf2);

	/* 62域 工作密钥 */
	if(memcmp(ptApp->szHostRetCode, TRANS_SUCC, 2) == 0) 
	{
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
	    iRet = GetBit(ptMsgRule, ptData, FIELD62, szTmpBuf);
	    if(iRet != 40)
    	{
            WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                     ptApp->iTransType, FIELD62, iRet);

		    strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

	    	return FAIL;
	    }

        /* 更新工作密钥 */
        memset(&tWorkKey, 0, sizeof(tWorkKey));

        /* PinKey密文 */
        BcdToAsc(szTmpBuf, 32, 0, tWorkKey.szPinKey);

        /* PinKey校验值 */
        BcdToAsc(szTmpBuf+16, 8, 0, tWorkKey.szPIKChkVal);

        /* MacKey密文 */
        /* 如果MacKey后8字节为0，拷贝前8个字节 */
        if(memcmp(szTmpBuf+28, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0)
        {
            memcpy(szTmpBuf+28, szTmpBuf+20, 8);
        }
        BcdToAsc(szTmpBuf+20, 32, 0, tWorkKey.szMacKey);

        /* MacKey校验值 */
        BcdToAsc(szTmpBuf+36, 8, 0, tWorkKey.szMAKChkVal);

        if(ChangeWorkKey(ptApp, YLPOSP, &tWorkKey) != SUCC)
        {
            WriteLog(ERROR, "工作密钥转加密失败!");

            return FAIL;
        }
	}

	return SUCC;
}
