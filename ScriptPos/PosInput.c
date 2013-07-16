/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 POS应答报文组包
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.2 $
 * $Log: PosInput.c,v $
 * Revision 1.2  2013/03/11 07:14:18  fengw
 *
 * 1、新增自定义指令处理代码。
 *
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/****************************************************************
** 功    能：POS指令输入数据组包
** 输入参数：
**        ptApp                     app结构指针
**        szFirstPage               首页信息
**        iFirstPageLen             首页信息长度
** 输出参数：
**        szData                    有效数据
**        piDataLen                 有效数据长度
**        piCalcMac                 是否校验MAC
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        Robin
** 日    期：
**        2008/07/31
** 调用说明：
**
** 修改日志：
****************************************************************/
int PosInput(T_App *ptApp, char* szFirstPage, int iFirstPageLen,
             char *szData, int *piDataLen, int *piCalcMac)
{

    int         i;
    uchar       ucOrgCmd;                       /* 原始指令 */
    int         iCmdIndex;                      /* 指令索引 */
    int         iCmdBytes;                      /* 指令长度 */
    int         iValidDataLen;                  /* 有效数据长度 */
    char        szValidData[1024+1];            /* 有效数据 */
    int         iCtrlIndex;                     /* 控制参数索引 */
    int         iDataSourceIndex;               /* 数据来源索引 */
    int         iPrintCount;                    /* 打印次数 */
    int         iMenuRecNo;                     /* 动态菜单ID */
    int         iValueLen;                      /* 数据长度 */
    char        szValue[1024+1];                /* 数据(首页信息、打印数据) */
    char        szTmpBuf[512+1];                /* 临时变量 */
    int         iTmp;                           /* 临时变量 */

    /* 记录POS指令解析日志 */
    WriteCmdData(ptApp->szTransName, ptApp->lPosTrace, "SNDBEGIN", ptApp->iTransType);

    /* 初始化 */
    iValidDataLen = 0;
    memset(szValidData, 0, sizeof(szValidData));

    iCmdIndex = 0;
    iCtrlIndex = 0;
    iDataSourceIndex = 0;
    iPrintCount = 0;

    for(i=0;i<ptApp->iCommandNum;i++)
    {
        /* 判断是否是特殊指令 */
        /*
        新增操作码均为3个字节。
        第一个字节固定为C0。
        第二个字节表示操作指令号所对应的操作提示信息索引号（HEX），若为0表示无操作提示信息，为255表示使用临时操作提示信息（临时操作提示信息由中心返回）。在下文中用XX表示。
        第三个字节表示具体的指令类型。
        */
        if((ptApp->szCommand[iCmdIndex] & 0xFF) == SPECIAL_CMD_HEAD)
        {
            if((iTmp = SpecialInput(ptApp, ptApp->szCommand+iCmdIndex, szValidData+iValidDataLen)) == FAIL)
            {
                WriteLog(ERROR, "组织自定义指令输入数据失败!");

                return FAIL;
            }

            if(iTmp == 0)
            {
                WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);
            }
            else
            {
                WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen, iTmp);
            }

            iValidDataLen += iTmp;
            iCmdIndex += SPECIAL_CMD_LEN;
        }
        else
        {
            iCmdBytes = CalcCmdBytes((uchar)ptApp->szCommand[iCmdIndex]);

            ucOrgCmd = ptApp->szCommand[iCmdIndex] & 0x3F;

            switch(ucOrgCmd)
            {
                /* 读取安全模块号 */
                case 0x02:
                /* 磁道明文数据 */
                case 0x03:
                /* 磁道密文数据 */
                case 0x04:
                /* 密码密文 */
                case 0x05:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 交易数量 */
                case 0x06:
                    /* 修改回显数量 */
                    if(ptApp->iTransNum > 0)
                    {
                        /* 控制参数长度(1Byte) + 模式标识(1Byte) */
                        iCtrlIndex += 2;

                        /* 回显数值 */
                        memset(szTmpBuf, 0, sizeof(szTmpBuf));
                        sprintf(szTmpBuf, "%06ld", ptApp->iTransNum);
                        AscToBcd(szTmpBuf, 6, LEFT_ALIGN, ptApp->szControlPara+iCtrlIndex);
                        iCtrlIndex += 3;

                        /* 最小数值(3Bytes) + 最大数值(3Bytes) */
                        iCtrlIndex += 6;
                    }

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 交易金额 */
                case 0x07:
                    /* 后台有返回金额(一般为应缴费金额)，要求输入金额时，作为回显金额 */
                    if(strlen(ptApp->szAmount) == 12)
                    {
                        /* 控制参数长度(1Byte) + 模式标识(1Byte) + 小数点位数(1Byte) */
                        iCtrlIndex += 3;

                        /* 回显金额(6Bytes) */
                        AscToBcd(ptApp->szAmount, 12, LEFT_ALIGN, ptApp->szControlPara+iCtrlIndex);
                        iCtrlIndex += 6;

                        /* 最小金额(6Bytes) + 最大金额(6Bytes) + 输密码是否回显(1Byte) */
                        iCtrlIndex += 13;
                    }

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 金融应用号 */
                case 0x08:
                    if(strlen(ptApp->szFinancialCode) > 0)
                    {
                        /* 控制参数长度(1Byte) + 最小长度(1Byte) + 最大长度(1Byte) + 
                           模式标识(1Byte) + 显示标识(1Byte) + 数据类型(1Byte) */
                        iCtrlIndex += 6;

                        /* 回显数据长度 */
                        ptApp->szControlPara[iCtrlIndex] = strlen(ptApp->szFinancialCode);
                        iCtrlIndex += 1;

                        /* 回显数据 */
                        memcpy(ptApp->szControlPara+iCtrlIndex, ptApp->szFinancialCode, 
                               strlen(ptApp->szFinancialCode));
                        iCtrlIndex += strlen(ptApp->szFinancialCode);
                    }

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 商务应用号 */
                case 0x09:
                    if(strlen(ptApp->szBusinessCode) > 0)
                    {
                        /* 控制参数长度(1Byte) + 最小长度(1Byte) + 最大长度(1Byte) + 
                           模式标识(1Byte) + 显示标识(1Byte) + 数据类型(1Byte) */
                        iCtrlIndex += 6;

                        /* 回显数据长度 */
                        ptApp->szControlPara[iCtrlIndex] = strlen(ptApp->szBusinessCode);
                        iCtrlIndex += 1;

                        /* 回显数据 */
                        memcpy(ptApp->szControlPara+iCtrlIndex, ptApp->szBusinessCode, 
                               strlen(ptApp->szBusinessCode));
                        iCtrlIndex += strlen(ptApp->szBusinessCode);
                    }

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 日期 */
                case 0x0A:
                /* 年月 */
                case 0x0B:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 自定义数据 */
                case 0x0C:
                    if(strlen(ptApp->szBusinessCode) > 0)
                    {
                        /* 控制参数长度(1Byte) + 最小长度(1Byte)+最大长度(1Byte)+模式标识(1Byte)*/
                        iCtrlIndex += 4;

                        /* 回显数据长度 */
                        ptApp->szControlPara[iCtrlIndex] = strlen(ptApp->szUserData);
                        iCtrlIndex += 1;

                        /* 回显数据 */
                        memcpy(ptApp->szControlPara+iCtrlIndex, ptApp->szUserData, 
                               strlen(ptApp->szUserData));
                        iCtrlIndex += strlen(ptApp->szUserData);
                    }

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* MAC */
                case 0x0D:
                /* 数字签名 */
                case 0x0E:
                /* 冲正信息 */
                case 0x0F:
                /* 报文版本号 */
                case 0x10:
                /* 终端应用脚本版本号 */
                case 0x11:
                /* 终端序列号 */
                case 0x12:
                /* 加密报文数据 */
                case 0x13:
                /* 解密报文数据 */
                case 0x14:
                /* 提取帐单支付数据 */
                case 0x15:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 更新终端参数 */
                case 0x16:
                /* 更新PSAM卡参数 */
                case 0x17:
                /* 更新菜单参数 */
                case 0x18:
                /* 更新功能提示 */
                case 0x19:
                /* 更新操作提示 */
                case 0x1A:
                /* 更新打印模板记录 */
                case 0x1C:
                /* 更新错误提示信息 */
                case 0x1D:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, ptApp->szReserved, ptApp->iReservedLen);

                    /* 有效数据长度 */
                    szValidData[iValidDataLen] = ptApp->iReservedLen;
                    iValidDataLen += 1;

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, ptApp->szReserved, ptApp->iReservedLen);
                    iValidDataLen += ptApp->iReservedLen;

                    break;    
                /* 更新首页信息 */
                case 0x1B:
                    if(iFirstPageLen > 0)
                    {
                        WriteCmdData(ptApp->szCommand+iCmdIndex, i, szFirstPage, iFirstPageLen);

                        /* 有效数据长度 */
                        szValidData[iValidDataLen] = iFirstPageLen;
                        iValidDataLen += 1;

                        /* 数据内容 */
                        memcpy(szValidData+iValidDataLen, szFirstPage, iFirstPageLen);
                        iValidDataLen += iFirstPageLen;
                    }
                    else
                    {
                        WriteCmdData(ptApp->szCommand+iCmdIndex, i, "\x00", 1);

                        /* 有效数据长度 */
                        szValidData[iValidDataLen] = 0;
                        iValidDataLen += 1;
                    }

                    break;
                /* 存储帐单 */
                case 0x1E:
                    /* 发送日期 */
                    AscToBcd((uchar*)(ptApp->szHostDate), 8, LEFT_ALIGN, szValidData+iValidDataLen);
                    iValidDataLen += 4;

                    /* 发送时间 */
                    AscToBcd((uchar*)(ptApp->szHostTime), 6, LEFT_ALIGN, szValidData+iValidDataLen);
                    iValidDataLen += 3;

                    /* 有效数据长度 */
                    szValidData[iValidDataLen] = ptApp->iReservedLen;
                    iValidDataLen ++;

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, ptApp->szReserved, ptApp->iReservedLen);
                    iValidDataLen += ptApp->iReservedLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-ptApp->iReservedLen-8,
                                 ptApp->iReservedLen+8);

                    break;
                /* 记录日志 */
                case 0x1F:
                    /* 交易日期 */
                    AscToBcd((uchar*)(ptApp->szHostDate), 8, LEFT_ALIGN, szValidData+iValidDataLen);
                    iValidDataLen += 4;

                    /* 交易时间 */
                    AscToBcd((uchar*)(ptApp->szHostTime), 6, LEFT_ALIGN, szValidData+iValidDataLen);
                    iValidDataLen += 3;

                    /* 流水号 */
                    memset(szTmpBuf, 0, sizeof(szTmpBuf));
                    sprintf(szTmpBuf, "%06ld", ptApp->lPosTrace);
                    AscToBcd((uchar*)szTmpBuf, 6, LEFT_ALIGN, szValidData+iValidDataLen);
                    iValidDataLen += 3;

                    /* MAC */
                    memcpy(szValidData+iValidDataLen, ptApp->szMac, 8);
                    iValidDataLen += 8;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-18, 18);

                    break;    
                /* 存储短信 */
                case 0x20:
                    /* 发送日期 */
                    AscToBcd((uchar*)(ptApp->szHostDate), 8, LEFT_ALIGN, szValidData+iValidDataLen);
                    iValidDataLen += 4;

                    /* 发送时间 */
                    AscToBcd((uchar*)(ptApp->szHostTime), 6, LEFT_ALIGN, szValidData+iValidDataLen);
                    iValidDataLen += 3;

                    /* 短信数据长度 */
                    szValidData[iValidDataLen] = ptApp->iReservedLen;
                    iValidDataLen ++;

                    /* 短信内容 */
                    memcpy(szValidData+iValidDataLen, ptApp->szReserved, ptApp->iReservedLen);
                    iValidDataLen += ptApp->iReservedLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-ptApp->iReservedLen-8, 
                                 ptApp->iReservedLen+8);

                    break;    
                /* 打印数据 */
                case 0x21:
                    iValueLen = 0;
                    iPrintCount = 0;
                    memset(szValue, 0, sizeof(szValue));

                    iValueLen = GetPrintData(ptApp, iDataSourceIndex, szValue, &iPrintCount);
                    iDataSourceIndex++;

                    /* 打印数据长度 */
                    szValidData[iValidDataLen] = (iValueLen+1) / 256;
                    szValidData[iValidDataLen+1] = (iValueLen+1) % 256;
                    iValidDataLen += 2;

                    /* 打印份数 */
                    szValidData[iValidDataLen] = iPrintCount + '0';
                    iValidDataLen += 1;

                    /* 打印数据 */
                    memcpy(szValidData+iValidDataLen, szValue, iValueLen);
                    iValidDataLen += iValueLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-3-iValueLen, iValueLen+3);

                    break;    
                /* 显示结果信息 */
                case 0x22:
                    iValueLen = 0;
                    memset(szValue, 0, sizeof(szValue));

                    iValueLen = GetReturnData(ptApp, szValue);

                    /* 结果信息长度 */
                    szValidData[iValidDataLen] = iValueLen;
                    iValidDataLen += 1;

                    memcpy(szValidData+iValidDataLen, szValue, iValueLen);
                    iValidDataLen += iValueLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-iValueLen-1, iValueLen+1);

                    break;    
                /* 连接系统 */
                case 0x23: 
                /* 发送数据 */
                case 0x24:   
                /* 接收数据 */
                case 0x25:   
                /* 挂机 */
                case 0x26:  
                /* 验证固网支付密码 */
                case 0x27:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;    
                /* 验证MAC */
                case 0x28:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    *piCalcMac = 1;

                    break;    
                /* 免提拨号 */
                case 0x29:  
                /* 交易确认 */
                case 0x2A:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;    
                /* 用户选择动态菜单 */
                case 0x2B:
                    iValueLen = 0;
                    memset(szValue, 0, sizeof(szValue));

                    iValueLen = GetMenuData(ptApp, iDataSourceIndex, &iMenuRecNo, szValue);
                    if(iValueLen == FAIL)
                    {
                        WriteLog(ERROR, "获取动态菜单失败!");

                        memset(ptApp->szRetDesc, 0, sizeof(ptApp->szRetDesc));

                        return FAIL;
                    }
                    iDataSourceIndex += 1;

                    ptApp->iMenuRecNo[ptApp->iMenuNum] = iMenuRecNo;
                    ptApp->iMenuNum += 1;

                    /* 有效数据长度 */
                    szValidData[iValidDataLen] = iValueLen;
                    iValidDataLen += 1;

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, szValue, iValueLen);
                    iValidDataLen += iValueLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i,
                                 szValidData+iValidDataLen-iValueLen-1, iValueLen+1);

                    break;    
                /* 静态菜单更新 */
                case 0x2C:
                    /* 有效数据长度 */
                    szValidData[iValidDataLen] = ptApp->iReservedLen / 256;
                    szValidData[iValidDataLen+1] = ptApp->iReservedLen % 256;
                    iValidDataLen += 2;        

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, ptApp->szReserved, ptApp->iReservedLen);
                    iValidDataLen += ptApp->iReservedLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-ptApp->iReservedLen-2, 
                                 ptApp->iReservedLen+2);

                    break;    
                /* 存储卡号 */
                case 0x2D:
                /* 上传卡号 */
                case 0x2E:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;    
                /* 系统临时操作提示信息 */
                case 0x2F:
                    iValueLen = 0;
                    memset(szValue, 0, sizeof(szValue));

                    iValueLen = GetTmpOperationInfo(ptApp, iDataSourceIndex, szValue);
                    if(iValueLen == FAIL)
                    {
                        WriteLog(ERROR, "获取临时操作提示失败!");

                        memset(ptApp->szRetDesc, 0, sizeof(ptApp->szRetDesc));

                        return FAIL;
                    }
                    iDataSourceIndex += 1;

                    /* 有效数据长度 */
                    szValidData[iValidDataLen] = iValueLen;
                    iValidDataLen ++;

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, szValue, iValueLen);
                    iValidDataLen += iValueLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-iValueLen-1, iValueLen+1);

                    break;
                /* 获取流程控制码 */
                case 0x30:
                /* IC卡指令 */
                case 0x31:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 文件数据处理(待补充) */
                case 0x32:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;    
                /* 卡号 */
                case 0x33:
                /* 上传交易日志 */
                case 0x34:   
                /* 上传错误日志 */
                case 0x35:  
                /* 利率数据 */
                case 0x36:
                /* 从PC串口接收数据 */
                case 0x37:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 发送数据给PC串口 */
                case 0x38:
                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, ptApp->szReserved, ptApp->iReservedLen);
                    iValidDataLen += ptApp->iReservedLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-ptApp->iReservedLen, 
                                 ptApp->iReservedLen);

                    break;
                /* 更新工作密钥 */
                case 0x39:
                    /* 有效数据长度 */
                    szValidData[iValidDataLen] = ptApp->iReservedLen;
                    iValidDataLen += 1;

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen,ptApp->szReserved, ptApp->iReservedLen);
                    iValidDataLen += ptApp->iReservedLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-ptApp->iReservedLen-1, 
                                 ptApp->iReservedLen+1);

                    break;
                /* 预拨号 */
                case 0x3A:
                /* 清空菜单 */
                case 0x3B:
                /* 冲正重发控制 */
                case 0x3C:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                /* 显示信息数据 */
                case 0x3D:
                    iValueLen = 0;
                    memset(szValue, 0, sizeof(szValue));

                    iValueLen = GetQueryResult(ptApp, szValue);
                    if(iValueLen == FAIL)
                    {
                        WriteLog(ERROR, "获取显示信息数据失败!");

                        memset(ptApp->szRetDesc, 0, sizeof(ptApp->szRetDesc));

                        return FAIL;
                    }

                    /* 数据长度 */
                    szValidData[iValidDataLen] = iValueLen / 256L;
                    szValidData[iValidDataLen+1] = iValueLen % 256L;
                    iValidDataLen += 2;

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, szValue, iValueLen);
                    iValidDataLen += iValueLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-iValueLen-2, iValueLen+2);

                    break;            
                /* TMS参数下载 */
                case 0x3E:
                    /* 数据长度 */
                    szValidData[iValidDataLen] = (ptApp->iTmsLen) / 256L;
                    szValidData[iValidDataLen+1] = (ptApp->iTmsLen) % 256L;
                    iValidDataLen += 2;

                    /* 数据内容 */
                    memcpy(szValidData+iValidDataLen, ptApp->szTmsData, ptApp->iTmsLen);
                    iValidDataLen += ptApp->iTmsLen;

                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, szValidData+iValidDataLen-ptApp->iTmsLen-2,
                                 ptApp->iTmsLen+2);

                    break;
                /* 预留 */
                case 0x3F:
                    WriteCmdData(ptApp->szCommand+iCmdIndex, i, "FF", 2);

                    break;
                default:
                    WriteLog(ERROR, "未知指令 %02x", ucOrgCmd);

                    break;
            }

            iCmdIndex = iCmdIndex+iCmdBytes;
        }
    }

    /* 记录POS指令解析日志 */
    WriteCmdData(ptApp->szTransName, ptApp->lPosTrace, "SNDEND", ptApp->iTransType);

    memcpy(szData, szValidData, iValidDataLen);
    *piDataLen = iValidDataLen;

    return SUCC;
}