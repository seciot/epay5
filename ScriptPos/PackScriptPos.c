
/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 POS应答报文组包
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.1 $
 * $Log: PackScriptPos.c,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/*******************************************************************************
 * 函数功能：将公共数据结构数据按固网支付规范报文(脚本POS报文)组包
 * 输入参数：
 *           ptApp          -  公共数据结构指针
 *           szFirstPage    -  首页信息
 *           iFirstPageLen  -  首页信息长度
 * 输出参数：
 *           szOutData      -  组好的报文
 * 返 回 值： 
 *           FAIL           -  拆包失败
 *           >0             -  组好的报文长度
 * 作    者：Robin
 * 日    期：2012/12/14
 * 修订日志：
 *
 ******************************************************************************/
int PackScriptPos(T_App *ptApp, char* szFirstPage, int iFirstPageLen, char *szOutData)
{
    int     iIndex;                             /* 报文数据索引 */
    int     iMsgIndex;                          /* 消息内容数据开始索引 */
    char    szValidData[2048+1];                /* 有效数据 */
    int     iValidDataLen;                      /* 有效数据长度 */
    int     iCalcMac;                           /* 计算MAC标志 */
    char    szMac[8+1];                         /* MAC */
    char    szTmpBuf[512+1];                    /* 临时变量 */

    /* 初始化 */
    iIndex = 0;

    /* 报文组包 */
    /* TPDU */
    szOutData[iIndex] = 0x60;
    iIndex += 1;

    /* 源地址 */
    memcpy(szOutData+iIndex, ptApp->szSourceTpdu, 2);
    iIndex += 2;
   
    /* 目的地址 */
    memcpy(szOutData+iIndex, ptApp->szTargetTpdu, 2);
    iIndex += 2;   

    /* 报文同步序号 */
    szOutData[iIndex] = ptApp->iSteps;
    iIndex += 1;

    /* 交易数据索引号 */    
    szOutData[iIndex] = (ptApp->lTransDataIdx) / 256;  
    szOutData[iIndex+1] = (ptApp->lTransDataIdx) % 256;
    iIndex += 2;

    /* 呼叫类型 */
    szOutData[iIndex] = ptApp->iCallType;
    iIndex += 1;
    
    /* 接入网关编号 */
    szOutData[iIndex] = ptApp->iFskId;
    iIndex += 1;

    /* 模块号 */
    szOutData[iIndex] = ptApp->iModuleId;
    iIndex += 1;

    /* 通道号 */
    szOutData[iIndex] = ptApp->iChannelId;
    iIndex += 1;

    /******************************消息内容数据组包******************************/
    /* 消息内容长度 */
    iIndex += 2;

    /* 记录消息内容开始索引，用于计算MAC */
    iMsgIndex = iIndex;

    /* 报文类型 */
    if(ptApp->iTransType == RESEND)
    {
        szOutData[iIndex] = 0x03;    
    }
    else
    {
        szOutData[iIndex] = 0x02;
    }
    iIndex += 1;

    /* 结束标志 */
    szOutData[iIndex] = 0x00;
    iIndex += 1;

    /* 电话号码 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    sprintf(szTmpBuf, "%15.15s", ptApp->szCallingTel);
    memcpy(szOutData+iIndex, szTmpBuf, 15);    
    iIndex += 15;

    /* 安全模块号 */
    memcpy(szOutData+iIndex, ptApp->szPsamNo, 16);    
    iIndex += 16;

    /* 系统日期 */
    AscToBcd((uchar*)(ptApp->szPosDate), 8, LEFT_ALIGN, szOutData+iIndex);
    iIndex += 4;

    /* 系统时间 */
    AscToBcd( (uchar*)(ptApp->szPosTime), 6, LEFT_ALIGN, szOutData+iIndex);
    iIndex += 3;

    /* 流水号 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    sprintf(szTmpBuf, "%06ld", ptApp->lPosTrace);
    AscToBcd((uchar*)szTmpBuf, 6, LEFT_ALIGN, szOutData+iIndex);
    iIndex += 3;

    /* POS指令数据组包 */
    if(PosInput(ptApp, szFirstPage, iFirstPageLen,
                szValidData, &iValidDataLen, &iCalcMac) != SUCC)
    {
        WriteLog(ERROR, "POS指令输入数据组包失败!");

        return FAIL;
    }

    /* 交易代码 */
    if(strlen(ptApp->szNextTransCode) == 8)
    {
        AscToBcd((uchar*)(ptApp->szNextTransCode), 8, LEFT_ALIGN, szOutData+iIndex);
    }
    else
    {
        AscToBcd((uchar*)(ptApp->szTransCode), 8, LEFT_ALIGN, szOutData+iIndex);
        FreeTdi(ptApp->lTransDataIdx);
    }
    iIndex += 4;

    /* 操作码个数 */
    szOutData[iIndex] = ptApp->iCommandNum;
    iIndex += 1;

    /* 操作码集 */
    memcpy(szOutData+iIndex, ptApp->szCommand, ptApp->iCommandLen);
    iIndex += ptApp->iCommandLen;

    /* 控制参数长度 */
    szOutData[iIndex] = ptApp->iControlLen / 256;
    szOutData[iIndex+1] = ptApp->iControlLen % 256;
    iIndex += 2;

    /* 控制参数 */
    memcpy(szOutData+iIndex, ptApp->szControlPara, ptApp->iControlLen);
    iIndex += ptApp->iControlLen;

    /* 有效数据长度，包括8个字节MACS */
    szOutData[iIndex] = (iValidDataLen + 8) / 256;
    szOutData[iIndex+1] = (iValidDataLen + 8) % 256;
    iIndex += 2;

    /* 有效数据 */
    memcpy(szOutData+iIndex, szValidData, iValidDataLen);
    iIndex += iValidDataLen;

     /* 分包发送，待修改 */
    /* 计算分成多少个包下传给POS */
    /*
    if( iMsgLen%giEachPackMaxBytes == 0 )
    {
        iPackNum = iMsgLen/giEachPackMaxBytes;
    }
    else
    {
        iPackNum = iMsgLen/giEachPackMaxBytes+1;
    }

    if( iPackNum == 1 )
    {
        szMsg[1] = 0;
    }
    else
    {
        szMsg[1] = iPackNum;
    }
    */

    /* 计算MAC */
    memset(szMac, 0, sizeof(szMac));

    if(iCalcMac == 1 && giMacChk == 1)
    {
        if(HsmCalcMac(ptApp, XOR_CALC_MAC, ptApp->szMacKey, 
                      szOutData+iMsgIndex, iIndex-iMsgIndex, szMac) != SUCC)
        {
            WriteLog(ERROR, "计算MAC失败!");

            memset(szMac, 'A', 8);
        }
    }
    else
    {
        memset(szMac, 0x00, 8);
    }

    memcpy(szOutData+iIndex, szMac, 8);
    iIndex += 8;

    /* 消息内容长度 */
    szOutData[iMsgIndex-2] = (iIndex-iMsgIndex+8) / 256;
    szOutData[iMsgIndex-2+1] = (iIndex-iMsgIndex+8) % 256;

    return iIndex;    
}