/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 POS请求报文拆包
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.2 $
 * $Log: UnpackScriptPos.c,v $
 * Revision 1.2  2013/06/08 02:06:31  fengw
 *
 * 1、将文件格式从DOS转换为UNIX。
 *
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/*******************************************************************************
 * 函数功能：将固网支付规范报文(脚本POS报文)解析到公共数据结构。
 *           指令被多次使用，则从第2次起输出数据解析到szReserved自定义字段中，这些
 *           数据按TLV格式组织。需要用到这些数据的模块按TLV格式自行解析。TAG编码
 *           规则如下：
 *           a) Tag为三字节
 *           b) 第一位0xDF
 *           c) 第二位 bit8为1，表示TAG有后续位，bit7-bit1表示指令编号，例如9号
 *              指令，读取商务应用号为 0x89 
 *           d) 第三位 bit8为0，bit7-bit1表示指令出现次数，例如第二次执行，则为
 *              0x02
 *           e) 例如第二次执行商务应用号时，数据保存Tag为 \xDF\x89\x02
 * 输入参数：
 *           ptApp      -  公共数据结构指针
 *           szInData   -  接收到的数据
 *           iInLen     -  接收到的数据长度
 * 输出参数：
 *           ptApp      -  公共数据结构指针
 * 返 回 值： 
 *           FAIL       -  拆包失败
 *           SUCC       -  拆包成功
 *           INVALID_PACK - 非法报文
 * 作    者：Robin
 * 日    期：2012/12/11
 * 修订日志：
 *
 ******************************************************************************/
int UnpackScriptPos(T_App *ptApp, char *szInData, int iInLen)
{
    int     i;
    int     iIndex;                     /* 报文拆解索引值 */
    char    szTmpBuf[1024+1];           /* 临时变量 */
    int     iMsgLen;                    /* 消息内容长度 */
    int     iMsgIndex;                  /* 消息内容开始索引值 */
    int     iCmdNum;                    /* 流程代码个数 */
    int     iCmdLen;                    /* 流程代码长度 */
    char    szCmdData[512+1];           /* 流程代码内容 */
    int     iDataLen;                   /* 有效数据长度 */
    int     iDataIndex;                 /* 有效数据开始索引值 */
    char    szAuthCode[4];              /* 网关认证码 */

    /* 报文拆解 */
    iIndex = 0;

    /* TPDU */
    /* 跳过60 */
    iIndex += 1;

    /* 目的地址 */
    memcpy(ptApp->szTargetTpdu, szInData+iIndex, 2);
    iIndex += 2;
#ifdef DEBUG
    WriteLog(TRACE, "TPDU 目的地址:[%02x %02x]",
             ptApp->szTargetTpdu[0], ptApp->szTargetTpdu[1]);
#endif

    /* 源地址 */
    memcpy(ptApp->szSourceTpdu, szInData+iIndex, 2);
    iIndex += 2;
#ifdef DEBUG
    WriteLog(TRACE, "TPDU 源地址:[%02x %02x]",
             ptApp->szSourceTpdu[0], ptApp->szSourceTpdu[1]);
#endif

    /* 上送电话号码 */
    if(memcmp(szInData+iIndex, "\x4C\x52\x49\x00\x1C", 5) == 0)
    {
        /* 电话号码标识头 */
        iIndex += 5;

        /* 主叫号码 */
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        BcdToAsc(szInData+iIndex, 16, LEFT_ALIGN, (uchar*)szTmpBuf);

        for(i=0;i<16;i++)
        {
            if(szTmpBuf[i] != '0')
            {
                break;
            }
        }
        memcpy(ptApp->szCallingTel, szTmpBuf+i, 16-i);
        iIndex += 8;
#ifdef DEBUG
        WriteLog(TRACE, "主叫号码:[%s]", ptApp->szCallingTel);
#endif

        /* 被叫号码 */        
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        BcdToAsc(szInData+iIndex, 16, LEFT_ALIGN, (uchar*)szTmpBuf);

        for(i=0;i<16;i++)
        {
            if(szTmpBuf[i] != '0')
            {
                break;
            }
        }
        memcpy(ptApp->szCalledTelByNac, szTmpBuf+i, 16-i);
        iIndex += 8;
#ifdef DEBUG
        WriteLog(TRACE, "被叫号码:[%s]", ptApp->szCalledTelByNac);
#endif

        /* 电话号码标识尾 */
        iIndex += 12;
    }

    /* 报文同步序号 */
    iIndex += 1;

    /* 交易数据索引号 */
    iIndex += 2;

    /* 呼叫类型 */
    iIndex += 1;

    /* 接入网关编号 */
    ptApp->iFskId = (uchar)szInData[iIndex];
    iIndex ++;
#ifdef DEBUG
    WriteLog(TRACE, "接入网关编号:[%d]", ptApp->iFskId);
#endif

    /* 接入网关模块号 */
    ptApp->iModuleId = (uchar)szInData[iIndex];
    iIndex ++;
#ifdef DEBUG
    WriteLog(TRACE, "接入网关模块号:[%d]", ptApp->iModuleId);
#endif

    /* 接入网关通道号 */
    ptApp->iChannelId = (uchar)szInData[iIndex];
    iIndex ++;
#ifdef DEBUG
    WriteLog(TRACE, "接入网关通道号:[%d]", ptApp->iChannelId);
#endif

    /******************************消息内容数据拆包******************************/
    /* 消息内容长度 */
    iMsgLen = ((uchar)szInData[iIndex])*256 + (uchar)szInData[iIndex+1];
    iIndex += 2;

    /* 扣除MAC长度 */
    iMsgLen = iMsgLen - 8;
#ifdef DEBUG
    WriteLog(TRACE, "消息内容长度:[%d]", iMsgLen);
#endif

    /* 判断消息内容长度是否超出 */
    if(iMsgLen <= 0 || iMsgLen > 1024 || iMsgLen > (iInLen-iIndex-8))
	{
		WriteLog(ERROR, "消息内容长度[%d]非法!实际消息内容长度[%d]",
		         iMsgLen, iInLen-iIndex-8);

		return INVALID_PACK;
	}

    /* 消息内容 */
    /* 记录消息内容开始索引值，用于MAC计算 */
    iMsgIndex = iIndex;

    /* 报文类型 */
#ifdef DEBUG
    WriteLog(TRACE, "报文类型:[%02x]", szInData[iIndex]);
#endif
    iIndex += 1;

    /* 结束标志 */
#ifdef DEBUG
    WriteLog(TRACE, "结束标志:[%02x]", szInData[iIndex]);
#endif
    iIndex += 1; 

    /* 报文版本 */
    memcpy(ptApp->szMsgVer, szInData+iIndex, 2);    
    iIndex += 2;
#ifdef DEBUG
    WriteLog(TRACE, "报文版本:[%02x%02x]", ptApp->szMsgVer[0], ptApp->szMsgVer[1]);
#endif

    /* 应用脚本版本 */
    memcpy(ptApp->szAppVer, szInData+iIndex, 4);    
    iIndex += 4;
#ifdef DEBUG
    WriteLog(TRACE, "应用脚本版本:[%02x%02x%02x%02x]",
             ptApp->szAppVer[0], ptApp->szAppVer[1],
             ptApp->szAppVer[2], ptApp->szAppVer[3]);
#endif

    /* 终端程序版本 */
    BcdToAsc(szInData+iIndex, 8, LEFT_ALIGN, ptApp->szPosCodeVer);
    iIndex += 4;
#ifdef DEBUG
    WriteLog(TRACE, "终端程序版本:[%s]", ptApp->szPosCodeVer);
#endif

    /* 终端类型 */
    memcpy(ptApp->szPosType, szInData+iIndex, 10);
    iIndex += 10;
#ifdef DEBUG
    WriteLog(TRACE, "终端类型:[%s]", ptApp->szPosType);
#endif

    /* 网关认证码 */
    memcpy(szAuthCode, szInData+iIndex, 4);
    iIndex += 4;
#ifdef DEBUG
    WriteLog(TRACE, "网关认证码:[%02x%02x%02x%02x]",
             szAuthCode[0] & 0xFF, szAuthCode[1] & 0xFF,
             szAuthCode[2] & 0xFF, szAuthCode[3] & 0xFF);
#endif

    /* POS上送被叫号码 */
    memcpy(ptApp->szCalledTelByTerm, szInData+iIndex, 15);
    iIndex += 15;
    DelTailSpace(ptApp->szCalledTelByTerm);
#ifdef DEBUG
    WriteLog(TRACE, "POS上送被叫号码:[%s]", ptApp->szCalledTelByTerm);
#endif

    /* 来电显示标志 */
#ifdef DEBUG
    WriteLog(TRACE, "来电显示标志:[%c]", szInData[iIndex]);
#endif
    iIndex += 1;    

    /* 安全模块号 */
    memcpy(ptApp->szPsamNo, szInData+iIndex, 16);
    iIndex += 16;
#ifdef DEBUG
    WriteLog(TRACE, "安全模块号:[%s]", ptApp->szPsamNo);
#endif

    /* POS流水号 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    BcdToAsc(szInData+iIndex, 6, LEFT_ALIGN, (uchar*)szTmpBuf);
    ptApp->lPosTrace = atol(szTmpBuf);
    iIndex += 3;
#ifdef DEBUG
    WriteLog(TRACE, "POS流水号:[%ld]", ptApp->lPosTrace);
#endif

    /* 交易代码 */
    BcdToAsc(szInData+iIndex, 8, LEFT_ALIGN, ptApp->szTransCode);
    iIndex += 4;
#ifdef DEBUG
    WriteLog(TRACE, "交易代码:[%s]", ptApp->szTransCode);
#endif

    /* 根据szTransCode获取交易信息 */
    if(GetTranInfo(ptApp) != SUCC)
    {
        WriteLog(ERROR, "获取交易码[%s]对应交易定义失败!", ptApp->szTransCode);

        return FAIL;
    }

    /* trans_def交易定义表中trans_code标识终端交易，trans_type标识平台交易。并约定trans_type低4位相同，
       对平台而言，这些交易对应平台同一个交易，可以采用完全相同的交易流程。即在此trans_type做模10000处
       理，这样对于平台的所有模块都只需要按一个交易处理。*/
    ptApp->iTransType = ptApp->iTransType % 10000;

    /* 交易重发报文 */
    if(szInData[iMsgIndex] == 0x03)
    {
        ptApp->iOldTransType = ptApp->iTransType;

        ptApp->iTransType = RESEND;
    }

    /* 获取终端信息，检查终端是否需要更新 */
    if(ChkTermInfo(ptApp) != SUCC)
    {
        return FAIL;
    }

    /******************************流程代码数据拆包******************************/
    /* 流程代码个数 */
    iCmdNum = (uchar)szInData[iIndex];    
    iIndex ++;
#ifdef DEBUG
    WriteLog(TRACE, "流程代码个数:[%d]", iCmdNum);
#endif

    /* 计算流程代码长度 */
    iCmdLen = 0;

    for(i=0;i<iCmdNum;i++)
    {
        iCmdLen += CalcCmdBytes((uchar)szInData[iIndex+iCmdLen]);
    }
#ifdef DEBUG
    WriteLog(TRACE, "流程代码长度:[%d]", iCmdLen);
#endif

    /* 流程代码 */
    memset(szCmdData, 0, sizeof(szCmdData));
    memcpy(szCmdData, szInData+iIndex, iCmdLen);    
    iIndex += iCmdLen;

    /* 有效数据长度 */
    iDataLen = ((uchar)szInData[iIndex])*256 + (uchar)szInData[iIndex+1];
    iIndex += 2;

    /* 判断有效数据长度是否超出 */
	if(iDataLen > 1024 || iDataLen > (iInLen-iIndex))
	{
        WriteLog(ERROR, "有效数据长度[%d]非法!实际消息内容长度[%d]",
                 iDataLen, iInLen-iIndex);

		return INVALID_PACK;
	}

    /******************************指令数据解析******************************/
    /* 记录有效数据开始索引值 */
    iDataIndex = iIndex;
    
    if(PosOutput(ptApp, iCmdNum, szCmdData, iCmdLen, szInData+iDataIndex, iDataLen) != SUCC)
    {
		return INVALID_PACK;
    }

    /* 除了签到、回响测试、柜员签到交易以外，都需要进行MAC校验 */
    if((ptApp->iTransType != LOGIN &&
        ptApp->iTransType != ECHO_TEST &&
        ptApp->iTransType != DOWN_ALL_FUNCTION &&
        ptApp->iTransType != DOWN_ALL_MENU &&
        ptApp->iTransType != DOWN_ALL_PRINT &&
        ptApp->iTransType != DOWN_ALL_TERM &&
        ptApp->iTransType != DOWN_ALL_PSAM &&
        ptApp->iTransType != DOWN_ALL_ERROR &&
        ptApp->iTransType != TEST_DISP_OPER_INFO &&
        ptApp->iTransType != TEST_PRINT &&
        ptApp->iTransType != DOWN_ALL_OPERATION &&
        ptApp->iTransType != CENDOWN_ALL_OPERATION &&
        ptApp->iTransType != REGISTER &&
        ptApp->iTransType != TERM_REGISTER &&
        ptApp->iTransType != AUTO_VOID &&
        ptApp->iTransType != OPER_LOGIN) && giMacChk == 1)
    {
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        if(HsmCalcMac(ptApp, XOR_CALC_MAC, ptApp->szMacKey, 
                      szInData+iMsgIndex, iMsgLen, szTmpBuf) != SUCC)
        {
            WriteLog(ERROR, "计算报文MAC错误!");

            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            return FAIL;
        }

        if(memcmp(ptApp->szMac, szTmpBuf, 8) != 0) 
        {
            WriteLog(ERROR, "报文MAC错误!");

            strcpy(ptApp->szRetCode, ERR_MAC);
#ifdef DEBUG
            WriteLog(TRACE, "报文上送MAC:[%02x%02x%02x%02x] 计算MAC:[%02x%02x%02x%02x]",
            ptApp->szMac[0] & 0xFF, ptApp->szMac[1] & 0xFF,
            ptApp->szMac[2] & 0xFF, ptApp->szMac[3] & 0xFF,
            szTmpBuf[0] & 0xFF, szTmpBuf[1] & 0xFF,
            szTmpBuf[2] & 0xFF, szTmpBuf[3] & 0xFF);
#endif
            return FAIL;
        }
    }

    WriteAppStru(ptApp, "Read from ePos");

    /* 校验网关认证码 */
    if(CheckAuthCode(szAuthCode, ptApp->szPsamNo) != SUCC)
    {     
        WriteLog(ERROR, "验证网关认证码失败!");

        strcpy(ptApp->szRetCode, ERR_AUTHCODE);

        return FAIL; 
    }

    return SUCC;
}
