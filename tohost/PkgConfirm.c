#define _EXTERN_

#include "tohost.h"

/****************************************************************
** 功    能：  预授权完成打包 
** 输入参数：
**        ptMsgRule       报文规则
**        ptApp           app结构
** 输出参数：
**        ptData          ISO结构指针
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        linqil 
** 日    期：
**        2013/07/01
** 调用说明：
**
** 修改日志：
****************************************************************/

int ConfirmPack(MsgRule *ptMsgRule, T_App *ptApp, ISO_data *ptData)
{
    char    szTmpBuf[512+1];
    int     iIndex;

	/* 0域 消息类型 */
    if(SetBit(ptMsgRule, "0200", MSG_ID, 4, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, MSG_ID, "0200");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	/* 2域 卡号 */
	if(strlen(ptApp->szPan) > 0 &&
	   SetBit(ptMsgRule, ptApp->szPan, PAN, strlen(ptApp->szPan), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, PAN, ptApp->szPan);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
	}

	/* 3域 交易处理码 */
    if(SetBit(ptMsgRule,"000000" , PROC_CODE, 6, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, PROC_CODE, "310000");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /*4域 交易金额*/
    if(SetBit(ptMsgRule, ptApp->szAmount, AMOUNT, 12, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, AMOUNT, "310000");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }    

	/* 11域 原受卡方系统跟踪号 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	sprintf(szTmpBuf, "%06ld", ptApp->lSysTrace);
    if(SetBit(ptMsgRule, szTmpBuf, POS_TRACE, 6, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, POS_TRACE, szTmpBuf);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 14域 卡有效期 */
    if(strlen(ptApp->szExpireDate) == 4 &&
       SetBit(ptMsgRule, ptApp->szExpireDate, EXPIRY, strlen(ptApp->szExpireDate), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, EXPIRY, ptApp->szExpireDate);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	/* 25域 服务点条件码 */
    if(SetBit(ptMsgRule, "00", SERVER_CODE, 2, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, SERVER_CODE, "00");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 35域 二磁道 */
    if(strlen(ptApp->szTrack2) > 0 &&
       SetBit(ptMsgRule, ptApp->szTrack2, TRACK_2, strlen(ptApp->szTrack2), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, TRACK_2, ptApp->szTrack2);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 36域 三磁道 */
    if(strlen(ptApp->szTrack3) > 0 &&
       SetBit(ptMsgRule, ptApp->szTrack3, TRACK_3, strlen(ptApp->szTrack3), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, TRACK_3, ptApp->szTrack3);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }   


	/* 37域 原消费交易参考号 */
    if(SetBit(ptMsgRule, ptApp->szOldRetriRefNum,RETR_NUM, 12, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, RETR_NUM, ptApp->szRetCode);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 38域 授权码 */
    if (SetBit(ptMsgRule, ptApp->szAuthCode, AUTH_ID, 6, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, RETR_NUM, ptApp->szRetCode);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	/* 39域 应答码 */
    if(SetBit(ptMsgRule, ptApp->szRetCode, RET_CODE, 2, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, RET_CODE, ptApp->szRetCode);

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

	/* 49域 人民币货币代码 */
    if(SetBit(ptMsgRule, "156", FUND_TYPE, 3, ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, FUND_TYPE, "156");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 个人密码 */
    if(memcmp(ptApp->szPasswd, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8) != 0 &&
       memcmp(ptApp->szPasswd, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) != 0)
    {   
        /* 密码转加密 */
        if(ConvertPin(ptApp, YLPOSP) != SUCC)
        {   
            return FAIL;
        }   

        /* 22域 服务点输入方式 */
        if(strlen(ptApp->szTrack2))
        {   
            /* 有刷卡 */    
            strcpy(ptApp->szEntryMode, "021");
        }   
        else
        {   
            /* 未刷卡 */
            strcpy(ptApp->szEntryMode, "011");
        }   

        if(SetBit(ptMsgRule, ptApp->szEntryMode, MODE, strlen(ptApp->szEntryMode), ptData) != SUCC)
        {   
            WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                     ptApp->iTransType, MODE, ptApp->szEntryMode);

            strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

            return FAIL;
        }   

        /* 52个人标识码数据 密码 */
        if(SetBit(ptMsgRule, ptApp->szPasswd, PIN_DATA, 8, ptData) != SUCC)
        {   
            WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                     ptApp->iTransType, PIN_DATA, "********");

            strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

            return FAIL;
        }

        /* 53域 安全控制信息 
         * PIN-FORMAT-USED: 
         * 2 ANSI X9.8 Format（带主账号信息）
         *
         * ENCRYPTION-METHOD-USED 加密算法标志:
         * 0：单倍长密钥算法
         * 6：双倍长密钥算法
         * */
        if(SetBit(ptMsgRule, "2600000000000000", SEC_CTRL_CODE, 16, ptData) != SUCC)
        {
            WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                     ptApp->iTransType, SEC_CTRL_CODE, "2600000000000000");

            strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

            return FAIL;
        }

        /* 26域 服务点PIN获取码(服务点设备所允许输入的个人密码明文的最大长度) */
        if(SetBit(ptMsgRule, "06", PIN_MODE, 2, ptData) != SUCC)
        {
            WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                     ptApp->iTransType, PIN_MODE, "06");

            strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

            return FAIL;
        }
    }
    else
    {
        /* 22域 服务点输入方式 */
        if(strlen(ptApp->szTrack2))
        {
            /* 有刷卡 */
            strcpy(ptApp->szEntryMode, "022");
        }
        else
        {
            /* 未刷卡 */
            strcpy(ptApp->szEntryMode, "012");
        }

        if(SetBit(ptMsgRule, ptApp->szEntryMode, MODE, strlen(ptApp->szEntryMode), ptData) != SUCC)
        {
            WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                     ptApp->iTransType, MODE, ptApp->szEntryMode);

            strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

            return FAIL;
        }
    }



    /* 60域 自定义域 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    iIndex = 0;

	/* 60.1交易类型码 */
	sprintf(szTmpBuf, "22");
	iIndex += 2;

	/* 60.2 批次号 */
	sprintf(szTmpBuf+iIndex, "%06ld", ptApp->lBatchNo);
	iIndex += 6;

	/*60.3 网络管理码3*/
    sprintf(szTmpBuf+iIndex, "000");
    iIndex += 3;

    if(SetBit(ptMsgRule, szTmpBuf, FIELD60, strlen(szTmpBuf), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, FIELD60, szTmpBuf);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

    /* 61域 原始信息域 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    iIndex = 0;

    /* 61.1域 原批次号 */
	sprintf(szTmpBuf+iIndex, "%06ld", ptApp->lBatchNo);
	iIndex += 6;

    /* 61.2域 原POS流水号 */
	sprintf(szTmpBuf+iIndex, "%06ld", ptApp->lOldPosTrace);
	iIndex += 6;
    
    if(SetBit(ptMsgRule, szTmpBuf, HOLDER_ID, strlen(szTmpBuf), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, HOLDER_ID, szTmpBuf);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	/* 64域 预置mac信息 */
    if(SetBit(ptMsgRule, "        ", 64, strlen(szTmpBuf), ptData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文第[%d]域组包内容[%s]设置失败!",
                 ptApp->iTransType, 64, "        ");

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }

	return SUCC;

}

/****************************************************************
** 功    能：预授权完成拆包
** 输入参数：
**        ptMsgRule       报文规则
**        ptData          ISO结构指针
** 输出参数：
**        ptApp           app结构
** 返 回 值：
**        SUCC            处理成功
**        FAIL            处理失败
** 作    者：
**        linqil
** 日    期：
**        2013/07/01
** 调用说明：
**
** 修改日志：
****************************************************************/
int ConfirmUnpack(MsgRule *ptMsgRule, T_App *ptApp, ISO_data *ptData)
{
	char    szTmpBuf[512+1];
    char    szTmpBuf2[512+1];
	int     iRet;

	/* 2域 主账号 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, PAN, szTmpBuf);
	if(iRet < 0 || iRet > 19)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, PAN, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
	strcpy(ptApp->szPan, szTmpBuf);

    /* 4域 交易金额 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, AMOUNT, szTmpBuf);
	if(iRet != 12)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, AMOUNT, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
    strcpy(ptApp->szAmount, szTmpBuf);

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
	
	/* 14域 有效期 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, EXPIRY, szTmpBuf);
	if(iRet == 4)
	{
		strcpy(ptApp->szExpireDate, szTmpBuf);
	}

    /* 15域 清算日期 */
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, SETTLE_DATE, szTmpBuf);
	if(iRet != 4)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, SETTLE_DATE, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
	strcpy(ptApp->szSettleDate, szTmpBuf);

	/* 25域 服务点条件码 */
	/* 已解析 */

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
    
    /* 38域 授权码 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    iRet = GetBit(ptMsgRule, ptData, AUTH_ID, szTmpBuf);
    if (iRet == 6)
    {
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, RETR_NUM, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
    }


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

	/* 44域 FIELD44 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData,FIELD44, szTmpBuf);
    /*
	if(iRet != 15)
	{
        WriteLog(ERROR, "交易类型[%d]应到报文第[%d]域拆解失败iRet:[%d]!",
                 ptApp->iTransType, CUSTOM_ID, iRet);

		strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

		return FAIL;
	}
    */

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

	return SUCC;
}
