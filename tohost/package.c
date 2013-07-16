/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：tohost打包、解包的封装程序
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: package.c,v $
 * Revision 1.3  2013/06/14 02:20:00  fengw
 *
 * 1、增加应答报文MAC校验代码。
 *
 * Revision 1.2  2013/06/14 02:03:31  fengw
 *
 * 1、增加签到、余额查询、消费交易报文组包、拆包处理。
 *
 * Revision 1.1  2012/12/11 07:16:20  linxiang
 * *** empty log message ***
 *
 * ----------------------------------------------------------------
 */

#define _EXTERN_

#include "tohost.h"
#include "def8583.h"

 PF GetFuncPoint(T_App *ptApp, int iFuncType);

/* ----------------------------------------------------------------
 * 功    能：   检查是否心跳包
 * 输入参数：
 *            szBuffer:收到的后台数据包
 * 输出参数：
 * 返 回 值：-1  否；  0  是
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int CheckWhetherHB(char *szBuffer)
{
    /*在此把心跳包的检查补充完成*/
    return FAIL;
}

/* ----------------------------------------------------------------
 * 功    能：   打心跳包
 * 输入参数：
 * 输出参数：
 *            szBuffer:打好的心跳包
 * 返 回 值：心跳包长度
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int PackHB(unsigned char *szBuffer)
{
     /*在此把心跳包的打包补充完成*/
    return strlen(szBuffer);
}

/* ----------------------------------------------------------------
 * 功    能：   打交易请求包
 * 输入参数：
 *            ptApp:交易处理层提交的数据结构
 * 输出参数：
 *            szBuffer:打好的交易请求包
 * 返 回 值：交易请求包长度
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int PackageRequest(T_App *ptApp, char* szBuffer)
{
    MsgRule     tMsgRule;
    ISO_data    tData;
    PF          pFuncPack;
    char        szBuf[1024+1];
    char        szTmpBuf[512+1];
    char        szMAC[8+1];
    int         iLen;
    int         iIndex;

    memset(&tMsgRule, 0, sizeof(tMsgRule));

    tMsgRule.iMidType = MSGIDTYPE_BCD;
    tMsgRule.iFieldLenType = FIELDLENTYPE_BCD;
    tMsgRule.ptISO = iso8583_YLPOSP;

    /* 初始化 */
    ClearBit(&tData);

    /* 根据交易类型，调用组包函数 */    
    if((pFuncPack = GetFuncPoint(ptApp, PACK_TYPE)) == NULL)
    {
        return FAIL;
    }

    if(pFuncPack(&tMsgRule, ptApp, &tData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]请求报文组包失败!", ptApp->iTransType);

        return FAIL;
    }

    /* 报文体打包 */
    memset(szBuf, 0, sizeof(szBuf));
    iLen = IsoToStr(&tMsgRule, &tData, szBuf);

    /* MAC计算 */
    if(ptApp->iTransType != LOGIN)
    {
        memset(szMAC, 0, sizeof(szMAC));
        if(CalcMac(szBuf, iLen-8, ptApp, szMAC) != SUCC)
        {
            return FAIL;
        }

        memcpy(szBuf+iLen-8, szMAC, 8);
    }

    /* 报文头打包 */
    iIndex = 2;

    /* TPDU */
    memcpy(szBuffer+iIndex, "\x60\x00\x00\x00\x00", 5);
    iIndex += 5;    

    /* 应用类别定义 */
    szBuffer[iIndex] = 0x60;
    iIndex += 1;

    /* 软件版本号 */
    szBuffer[iIndex] = 0x31;
    iIndex += 1;

    /* 终端状态及处理要求 */
    szBuffer[iIndex] = 0x00;
    iIndex += 1;

    /* 终端软件版本号 */
    memcpy(szBuffer+iIndex, "\x31\x00\x00", 3);
    iIndex += 3;

    /* 报文体拷贝 */
    memcpy(szBuffer+iIndex, szBuf, iLen);
    iIndex += iLen;

    /* 报文长度 */
    szBuffer[0] = (iIndex-2) / 256;
    szBuffer[1] = (iIndex-2) % 256;

    return iIndex;
}

/* ----------------------------------------------------------------
 * 功    能：   解交易响应包
 * 输入参数：
 *            szBuffer:后台返回的数据包
 *            iLength:后台返回的数据包长度
 * 输出参数：
 *            ptApp:返回给交易处理层的数据结构
 * 返 回 值：-1  失败；  0  成功
 * 作      者：
 * 日      期：
 * 调用说明：
 * 修改日志：修改日期      修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int UnpackageRespond(T_App *ptApp, char* szBuffer, int iLength)
{
    MsgRule     tMsgRule;
    ISO_data    tData;
    PF          pFuncUnpack;
    int         iHeadLen=13;
    char        szMAC[8+1];

    memset(&tMsgRule, 0, sizeof(tMsgRule));

    tMsgRule.iMidType = MSGIDTYPE_BCD;
    tMsgRule.iFieldLenType = FIELDLENTYPE_BCD;
    tMsgRule.ptISO = iso8583_YLPOSP;

    /* 初始化 */
    ClearBit(&tData);

    /* 报文解包 */
    if(StrToIso(&tMsgRule, szBuffer+iHeadLen, &tData) != SUCC)
    {
        return FAIL;
    }

    /* 根据消息类型和处理码获取交易类型 */
    if(GetTransType(&tMsgRule, &tData, ptApp) != SUCC)
    {
        return FAIL;
    }

    /* 根据交易类型，调用拆包函数 */    
    if((pFuncUnpack = GetFuncPoint(ptApp, UNPACK_TYPE)) == NULL)
    {
        return FAIL;
    }

    if(pFuncUnpack(&tMsgRule, ptApp, &tData) != SUCC)
    {
        WriteLog(ERROR, "交易类型[%d]应答报文拆包失败!", ptApp->iTransType);

        return FAIL;
    }

    strcpy(ptApp->szRetCode, ptApp->szHostRetCode);

    /* 验证MAC */
    if(ptApp->iTransType != LOGIN)
    {
        memset(szMAC, 0, sizeof(szMAC));
        if(CalcMac(szBuffer+iHeadLen, iLength-iHeadLen-8, ptApp, szMAC) != SUCC)
        {
            return FAIL;
        }

        if(memcmp(szBuffer+iLength-8, szMAC, 8) != 0)
        {
            WriteLog(ERROR, "MAC校验错!应答MAC:[%s] 计算MAC:[%s]", szBuffer+iLength-8, szMAC);
            strcpy(ptApp->szRetCode, ERR_RESP_MAC);

            return SUCC;
        }
    }

    return SUCC;
}

/****************************************************************
** 功    能：根据交易类型，报文处理类型获取报文处理函数指针
** 输入参数：
**        ptApp                 app结构指针
**        iFuncType             报文处理类型
** 输出参数：
**        无 
** 返 回 值：
**        pFunc                 报文处理函数指针 
**        FAIL                  失败
** 作    者：
**        fengwei
** 日    期：
**        2013/06/09
** 调用说明：
**
** 修改日志：
****************************************************************/
PF GetFuncPoint(T_App *ptApp, int iFuncType)
{
    int i=0;

    while(1)
    {
        if(gtaPkgProc[i].iTransType == 0)
        {
            /* 未定义交易 */
            strcpy(ptApp->szRetCode, ERR_INVALID_TRANS);

            WriteLog(ERROR, "交易类型[%d]请求报文组包函数未定义!", ptApp->iTransType);

            return NULL;
        }

        if(gtaPkgProc[i].iTransType == ptApp->iTransType)
        {
            if(iFuncType == PACK_TYPE)
            {
                return gtaPkgProc[i].pFuncPack;
            }
            else if(iFuncType == UNPACK_TYPE)
            {
                return gtaPkgProc[i].pFuncUnpack;
            }
            else
            {
                /* 未定义交易 */
                strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

                WriteLog(ERROR, "报文处理类型参数[%d]未定义!", iFuncType);

                return NULL;
            }
        }

        i++;
    }
}

/****************************************************************
** 功    能：根据消息类型和处理码获取交易类型
** 输入参数：
**        ptMsgRule             报文规则
**        iFuncType             报文处理类型
** 输出参数：
**        ptApp                 app结构指针
** 返 回 值：
**        SUCC                  成功
**        FAIL                  失败
** 作    者：
**        fengwei
** 日    期：
**        2013/06/09
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetTransType(MsgRule* ptMsgRule, ISO_data* ptData, T_App* ptApp)
{
    char    szTmpBuf[512+1];
    char    szMsgType[2+1];
    char    szNetCode[3+1];
    int     iRet;

    /* 0域 消息类型 */
    strcpy(ptApp->szMsgId, ptData->message_id);

    /* 3域 处理码 */	
	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	iRet = GetBit(ptMsgRule, ptData, PROC_CODE, szTmpBuf);
	if(iRet == 6)
	{
	    strcpy(ptApp->szProcCode, szTmpBuf);
    }

    /* 60域 消息类型 网络管理信息码 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    memset(szMsgType, 0, sizeof(szMsgType));
    memset(szNetCode, 0, sizeof(szNetCode));
    iRet = GetBit(ptMsgRule, ptData, FIELD60, szTmpBuf);
    if(iRet > 0)
    {
        memcpy(szMsgType, szTmpBuf, 2);

        if(iRet > 8)
        {
            memcpy(szNetCode, szTmpBuf+8, 3);
        }
    }

    /* 判断交易类型 */
    ptApp->iTransType = 0;
    strcpy(ptApp->szTransName, "未知交易");

    if(memcmp(ptApp->szMsgId, "0810", 4) == 0)
    {
        if(memcmp(szNetCode, "001", 3) == 0 || memcmp(szNetCode, "003", 3) == 0 ||
           memcmp(szNetCode, "004", 3) == 0) 
        {
            ptApp->iTransType = LOGIN;
            strcpy(ptApp->szTransName, "签到");
        }
    }
    else if(memcmp(ptApp->szMsgId, "0210", 4) == 0)
	{
	    if(memcmp(ptApp->szProcCode, "000000", 6) == 0)
        {
            if(memcmp(szMsgType, "22", 3) == 0)
            {
                ptApp->iTransType = PURCHASE;
                strcpy(ptApp->szTransName, "消费");
            }
            else if(memcmp(szMsgType, "20", 3) == 0)
            {
                ptApp->iTransType = CONFIRM;
                strcpy(ptApp->szTransName, "授权完成");
            }
        }
	    else if(memcmp(ptApp->szProcCode, "310000", 6) == 0)
        {
			ptApp->iTransType = INQUERY;
		    strcpy(ptApp->szTransName, "余额查询");
		}else if(memcmp(ptApp->szProcCode, "200000", 6) == 0)
        {
			ptApp->iTransType = PUR_CANCEL;
		    strcpy(ptApp->szTransName, "消费撤销");
        }
	}else if(memcmp(ptApp->szMsgId,"0410" ,4) == 0)
    {
	    if(memcmp(ptApp->szProcCode, "000000", 6) == 0)
        {
            ptApp->iOldTransType = PURCHASE;
            strcpy(ptApp->szTransName, "消费冲正");
        }
	    else if(memcmp(ptApp->szProcCode, "200000", 6) == 0)
        {
            /*预授权冲正撤销同交易码*/
			ptApp->iOldTransType = PUR_CANCEL;
		    strcpy(ptApp->szTransName, "撤销冲正");
		}else if(memcmp(ptApp->szProcCode, "030000", 6) == 0)
        {
			ptApp->iOldTransType = PRE_AUTH;
		    strcpy(ptApp->szTransName, "预授权冲正");
        }
        ptApp->iTransType = AUTO_VOID;

    }else if(memcmp(ptApp->szMsgId,"0230" ,4) == 0)
    {
	    ptApp->iTransType = REFUND;
	    strcpy(ptApp->szTransName, "退货");
    }else if(memcmp(ptApp->szMsgId,"0110" ,4) == 0)
    {
        if(memcmp(ptApp->szProcCode, "030000", 6) == 0)
        {
			ptApp->iTransType = PRE_AUTH;
		    strcpy(ptApp->szTransName, "预授权");
        }else if(memcmp(ptApp->szProcCode, "200000", 6) == 0)
        {
			ptApp->iTransType = PRE_CANCEL;
		    strcpy(ptApp->szTransName, "预授权撤销");
        }

    }

    if(ptApp->iTransType == 0)
	{
	    WriteLog(ERROR, "消息ID:[%s] 处理码:[%s] 消息类型:[%s] 网络管理信息码:[%s]未能判断出交易类型!",
	             ptApp->szMsgId, ptApp->szProcCode, szMsgType, szNetCode);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
	}

	return SUCC;
}
