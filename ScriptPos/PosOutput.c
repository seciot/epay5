/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 POS请求报文拆包
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.3 $
 * $Log: PosOutput.c,v $
 * Revision 1.3  2013/03/11 07:14:18  fengw
 *
 * 1、新增自定义指令处理代码。
 *
 * Revision 1.2  2013/02/21 06:49:38  fengw
 *
 * 1、修改08号、48号指令处理代码。
 *
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/****************************************************************
** 功    能：POS指令输出数据拆解
** 输入参数：
**        iCmdNum                   指令个数
**        szCmdData                 指令数据
**        iCmdLen                   指令数据长度
**        szData                    有效数据
**        iDataLen                  有效数据长度
** 输出参数：
**        ptApp                     app结构指针
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
int PosOutput(T_App *ptApp, int iCmdNum, char *szCmdData, int iCmdLen, char *szData, int iDataLen)
{
    int         i, j;
    T_TLVStru   tTlv;                           /* TLV数据结构 */
    int         iCmdIndex;                      /* 指令索引 */
    int         iDataIndex;                     /* 有效数据索引 */
    int         iCmdBytes;                      /* 指令长度 */
    uchar       ucOrgCmd;                       /* 原始指令 */
    char        szTmpBuf[1024+1];               /* 临时变量 */
    int         iTmp;                           /* 临时变量 */
    char        szTlvTag[3+1];                  /* TLV TAG */
    int         iTlvLen;                        /* TLV数据长度 */
    char        szTlvBuf[2048+1];               /* TLV数据 */
    int         iCopyIndex;                     /* 数据拷贝索引 */
    int         iControlNum;                    /* 流程控制码个数 */
    int         iMenuNum;                       /* 动态菜单个数 */
    int         iaTimes[MAX_COMMAND_NUM];       /* 记录指令出现的次数 */

    /* 记录POS指令解析日志 */
    WriteCmdData(ptApp->szTransName, ptApp->lPosTrace, "RCVBEGIN", ptApp->iTransType);

    /* 指令数据解析 */
    /* TLV初始化 */
    InitTLV(&tTlv, TAG_STANDARD, LEN_STANDARD, VALUE_NORMAL);

    iControlNum = 0;
    iMenuNum = 0;
    iCmdIndex = 0;
    iDataIndex = 0;

    for(i=0; i<MAX_COMMAND_NUM; i++)
    {
        iaTimes[i] = 0;
    }

    for(i=0;i<iCmdNum;i++)
    { 
        /* 判断是否是特殊指令 */
        /*
        新增操作码均为3个字节。
        第一个字节固定为C0。
        第二个字节表示操作指令号所对应的操作提示信息索引号（HEX），若为0表示无操作提示信息，为255表示使用临时操作提示信息（临时操作提示信息由中心返回）。在下文中用XX表示。
        第三个字节表示具体的指令类型。
        */
        if((szCmdData[iCmdIndex] & 0xFF) == SPECIAL_CMD_HEAD)
        {   
            if((iTmp = SpecialOutput(ptApp, szCmdData+iCmdIndex, szData+iDataIndex)) == FAIL)
            {
                WriteLog(ERROR, "解析自定义指令输出数据失败!");

                return FAIL;
            }

            if(iTmp == 0)
            {
                WriteCmdData(szCmdData+iCmdIndex, i, "FF", 2);
            }
            else
            {
                WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
            }

            iDataIndex += iTmp;
            iCmdIndex += SPECIAL_CMD_LEN;
        }
        else
        {
            iCmdBytes = CalcCmdBytes((uchar)szCmdData[iCmdIndex]);

            ucOrgCmd = szCmdData[iCmdIndex] & 0x3F;

            /* 记录指令出现次数 */
            iaTimes[ucOrgCmd]++;

            /* 保存当前数据索引值，用于拷贝TLV */
            iCopyIndex = iDataIndex;

            switch(ucOrgCmd)
            {
                /* 读取安全模块号 */
                case 0x02:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 16);
                    iDataIndex += 16;

                    break;
                /* 磁道明文数据 */
                case 0x03:
                    /* 卡序号 */
                    memset(szTmpBuf, 0, sizeof(szTmpBuf));
                    BcdToAsc(szData+iDataIndex, 2, 0, szTmpBuf);
                    memcpy(ptApp->szEmvCardNo, szTmpBuf, 3);
                    iDataIndex += 2;
#ifdef DEBUG
                    WriteLog(TRACE, "卡序号:[%s]", ptApp->szEmvCardNo);
#endif

                    if((iTmp = GetTrack(ptApp, szData+iDataIndex)) == FAIL)
                    {
                        return FAIL;
                    }
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        /* 根据磁道信息获取卡号 */
                        if(GetCardType(ptApp) != SUCC)
                        {
                            WriteLog(ERROR, "根据磁道信息获取卡号失败!");

                            return FAIL;
                        }
                    }

                    break;
                /* 磁道密文数据 */
                case 0x04:
                    /* 磁道数据密文长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 88)
                    {
                        WriteLog(ERROR, "磁道密文数据长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    /* /解密磁道数据 */
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        if(HsmDecryptTrack(ptApp, ptApp->szTrackKey,
                           szData+iDataIndex, iTmp) != SUCC)
                        {
                            WriteLog(ERROR, "解密磁道密文数据失败!");

                            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

                            return FAIL;
                        }

                        /* 根据磁道信息获取卡号 */
                        if(GetCardType(ptApp) != SUCC)
                        {
                            WriteLog(ERROR, "根据磁道信息获取卡号失败!");

                            return FAIL;
                        }
                    }

                    /* 记录指令数据 */
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                    break;
                /* 密码密文 */
                case 0x05:
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        memcpy(ptApp->szPasswd, szData+iDataIndex, 8);
                    }
                    else
                    {
                        memcpy(ptApp->szNewPasswd, szData+iDataIndex, 8);
                    }
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 8);
                    iDataIndex += 8;

                    break;
                /* 交易数量 */
                case 0x06:
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        memset(szTmpBuf, 0, sizeof(szTmpBuf));
                        BcdToAsc((uchar*)(szData+iDataIndex), 6, LEFT_ALIGN, (uchar*)szTmpBuf);

                        ptApp->iTransNum = atol(szTmpBuf);
                        ptApp->lOldPosTrace= atol(szTmpBuf);
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 3);
                    iDataIndex += 3;
    
                    break;
                /* 交易金额 */
                case 0x07:
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        BcdToAsc((uchar*)(szData+iDataIndex), 12, LEFT_ALIGN, ptApp->szAmount);
                    }  

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 6);
                    iDataIndex += 6;

                    break;
                /* 金融应用号 */
                case 0x08:
                    /* 金融应用号长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 40)
                    {
                        WriteLog(ERROR, "金融应用号长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        memcpy(ptApp->szFinancialCode, szData+iDataIndex, iTmp);
                        ptApp->szFinancialCode[iTmp] = 0;

                        strcpy(ptApp->szAccount2, ptApp->szFinancialCode);

                        /* 根据第二卡号获取卡信息 */
                        if(GetAcctType(ptApp) != SUCC)
                        {
                            WriteLog(ERROR, "根据第二卡号获取卡信息失败!");

                            return FAIL;
                        }
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                    break;
                /* 商务应用号 */
                case 0x09:
                    /* 商务应用号长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 40)
                    {
                        WriteLog(ERROR, "商务应用号长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        memcpy(ptApp->szBusinessCode, szData+iDataIndex, iTmp);
                        ptApp->szBusinessCode[iTmp] = 0;
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                    break;
                /* 日期YYYYMMDD */
                case 0x0A:
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        BcdToAsc((uchar*)(szData+iDataIndex), 8, LEFT_ALIGN, 
                                 (uchar*)ptApp->szInDate);
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 4);
                    iDataIndex += 4;

                    break;
                /* 年月YYYYMM */
                case 0x0B:
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        BcdToAsc((uchar*)(szData+iDataIndex), 6, LEFT_ALIGN, 
                                 (uchar*)ptApp->szInMonth);
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 3);
                    iDataIndex += 3;

                    break;
                /* 自定义数据 */
                case 0x0C:
                    /* 自定义数据长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 40)
                    {
                        WriteLog(ERROR, "自定义数据长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        memcpy(ptApp->szUserData, szData+iDataIndex, iTmp);
                        ptApp->szUserData[iTmp] = 0;
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                    break;
                /* MAC */
                case 0x0D:
                    if(ptApp->iTransType != AUTO_VOID)
                    {
                        memcpy(ptApp->szMac, szData+iDataIndex, 8);
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 8);
                    iDataIndex += 8;

                    break;
                /* 冲正信息[原交易流水号(3 bytes)+原交易MAC(8 bytes)] */
                case 0x0F:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 11);

                    memset(szTmpBuf, 0, sizeof(szTmpBuf));
                    BcdToAsc((uchar*)(szData+iDataIndex), 6, LEFT_ALIGN, (uchar*)szTmpBuf);

                    ptApp->lOldPosTrace = atol(szTmpBuf);
                    iDataIndex += 3;

                    memcpy(ptApp->szMac, szData+iDataIndex, 8);
                    iDataIndex += 8;

                    break;
                /* 报文版本号 */
                case 0x10:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 2);
                    iDataIndex += 2;

                    break;
                /* 终端应用脚本版本号 */
                case 0x11:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 4);
                    iDataIndex += 4;

                    break;
                /* 终端序列号 */
                case 0x12:
                    memcpy(ptApp->szTermSerialNo, szData+iDataIndex, 10);

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 10);
                    iDataIndex += 10;

                    break;
                /* 加密报文数据 */
                case 0x13:
                    /* 加密报文数据长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 256)
                    {
                        WriteLog(ERROR, "加密报文数据长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                     break;
                /* 解密报文数据 */
                case 0x14:
                    /* 解密报文数据长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 240)
                    {
                        WriteLog(ERROR, "解密报文数据长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                    break;
                /* 提取帐单支付数据 */
                case 0x15:
                    /* 账单支付数据长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 40)
                    {
                        WriteLog(ERROR, "账单支付数据长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    memcpy(ptApp->szBusinessCode, szData+iDataIndex, iTmp); 

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTmp);
                    iDataIndex += iTmp;

                    break;
                /* 更新终端参数 */
                case 0x16:
                /* 更新安全模块参数 */
                case 0x17:
                /* 更新菜单参数 */
                case 0x18:
                /* 更新功能提示 */
                case 0x19:
                /* 更新操作提示 */
                case 0x1A:
                /* 更新首页信息 */
                case 0x1B:
                /* 更新打印模板记录 */
                case 0x1C:
                /* 更新错误提示信息 */
                case 0x1D:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 2);

                    memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
                    iDataIndex += 2;

                    break;
                /* 存储帐单 */
                case 0x1E:
                    /* 账单支付数据长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 42)
                    {
                        WriteLog(ERROR, "账单支付数据长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex-1, iTmp+1);

                    /* 存储结果 */
                    memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
                    iDataIndex += 2;

                    memcpy(ptApp->szBusinessCode, szData+iDataIndex, iTmp-2);
                    iDataIndex = iDataIndex+iTmp-2;

                    break;    
                /* 记录日志 */
                case 0x1F:
                /* 存储短信 */
                case 0x20:
                /* 打印数据 */
                case 0x21:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 2);

                    memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
                    iDataIndex += 2;

                    break;
                /* 显示结果信息 */
                case 0x22:
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
                /* 验证MAC */
                case 0x28:
                /* 免提拨号 */
                case 0x29:
                /* 交易确认 */
                case 0x2A:
                    /* 指令0x22至0x2A没有输出数据 */
                    WriteCmdData(szCmdData+iCmdIndex, i, "FF", 2);

                    break;    
                /* 用户选择动态菜单 */
                case 0x2B:
                     WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 1);

                    /* 动态菜单项 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp >= 5)
                    {
                        WriteLog(ERROR, "动态菜单项[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    ptApp->iaMenuItem[iMenuNum] = iTmp;
                    iMenuNum++;

                    break;    
                /* 静态菜单更新 */
                case 0x2C:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 2);

                    memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
                    iDataIndex += 2;

                    break;    
                /* 静态菜单显示与选择 */
                case 0x2D:
                    /* 静态菜单ID */
                    ptApp->iStaticMenuId = (uchar)(szData[iDataIndex]);
                    iDataIndex += 1;

                    /* 静态菜单输出长度 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > 30)
                    {
                        WriteLog(ERROR, "静态菜单输出长度[%d]非法!", iTmp);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    memcpy(ptApp->szStaticMenuOut, szData+iDataIndex, iTmp);
                    ptApp->szStaticMenuOut[iTmp] = 0;

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex-2, iTmp+2);
                    iDataIndex += iTmp;

                    break;    
                /* 上传卡号 */
                case 0x2E:
                    /* 卡号记录数 */
                    iTmp = (uchar)(szData[iDataIndex]);    
                    if(iTmp > MAX_REC_CARD_NUM)
                    {
                        WriteLog(ERROR, "一次上传卡号条数[%d]太多!最大上传数:[%d]",
                                 iTmp, MAX_REC_CARD_NUM);

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }
                    iDataIndex += 1;

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex-1, 17*iTmp+1);
                    iDataIndex += 17*iTmp;

                    break;    
                /* 系统临时操作提示信息 */
                case 0x2F:
                    WriteCmdData(szCmdData+iCmdIndex, i, "FF", 2);

                    break;
                /* 获取流程控制码 */
                case 0x30:
                    if(iControlNum <= 5)
                    {
                        ptApp->szControlCode[iControlNum] = szData[iDataIndex];
                    }
                    else
                    {
                        WriteLog(ERROR, "流程控制码超过5个");

                        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

                        return FAIL;
                    }

                    /* 不执行下一个指令码，跳过 */
                    if(szData[iDataIndex] == '0')
                    {
                        WriteCmdData(szCmdData+iCmdIndex+iCmdBytes, i, "FF", 2);
                        iCmdIndex += CalcCmdBytes((uchar)szCmdData[iCmdIndex+iCmdBytes]);
                        i++;
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 1);
                    iDataIndex += 1;

                    iControlNum++;

                    break;
                /* IC卡指令(预留) */
                case 0x31:
                /* 文件数据处理 */
                case 0x32:
                    WriteCmdData(szCmdData+iCmdIndex, i, "FF", 2);

                    break;
                /* 读取二磁道卡号 */
                case 0x33:
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        memset(szTmpBuf, 0, sizeof(szTmpBuf));
                        BcdToAsc((uchar*)(szData+iDataIndex), 20, LEFT_ALIGN, (uchar*)szTmpBuf);

                        iDataIndex += 10;

                        for(j=0;j<20;j++)
                        {
                            if(szTmpBuf[j] == 'F')
                            {
                                szTmpBuf[j] = 0;
                                break;
                            }
                        }
                        strcpy(ptApp->szPan, szTmpBuf);
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 10);
                    iDataIndex += 10;

                    break;
                /* 上传交易日志 */
                case 0x34:
                    /* 暂不实现 */
                    break;
                /* 上传错误日志 */
                case 0x35:
                    /* 暂不实现 */
                    break;    
                /* 利率数据 */
                case 0x36:
                    if(iaTimes[ucOrgCmd] == 1)
                    {
                        memset(szTmpBuf, 0, sizeof(szTmpBuf));
                        BcdToAsc((uchar*)(szData+iDataIndex), 6, LEFT_ALIGN, (uchar*)szTmpBuf);
                        ptApp->lRate = atol(szTmpBuf);
                    }

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 3);
                    iDataIndex += 3;

                    break;
                /* 从串口接收数据 */
                case 0x37:
                    iTlvLen = (uchar)szData[iDataIndex];

                    szTlvTag[0] = 0xDF;
                    szTlvTag[1] = 0x80|ucOrgCmd;
                    szTlvTag[2] = iaTimes[ucOrgCmd];
                    SetTLV(&tTlv, szTlvTag, iTlvLen, szData+iDataIndex+1);

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTlvLen+1);
                    iDataIndex += (iTlvLen+1);

                    break;
                /* 发送数据给串口 */
                case 0x38:
                    iTlvLen = (uchar)szData[iDataIndex];

                    szTlvTag[0] = 0xDF;
                    szTlvTag[1] = 0x80|ucOrgCmd;
                    szTlvTag[2] = iaTimes[ucOrgCmd];
                    SetTLV(&tTlv, szTlvTag, iTlvLen, szData+iDataIndex+1);

                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, iTlvLen+1);
                    iDataIndex += (iTlvLen+1);

                    break;
                /* 签到更新密钥 */
                case 0x39:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 2);

                    memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
                    iDataIndex += 2;

                    break;
                /* 预拨号 */
                case 0x3A:
                /* 清空菜单 */
                case 0x3B:
                /* 冲正重发控制 */
                case 0x3C:
                    WriteCmdData(szCmdData+iCmdIndex, i, "FF", 2);

                    break;
                /* 显示后台返回数据(选择继续) */
                case 0x3D:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 1);
                    iDataIndex += 1;

                    break;    
                /* TMS参数下载 */
                case 0x3E:
                    WriteCmdData(szCmdData+iCmdIndex, i, szData+iDataIndex, 2);

                    memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
                    iDataIndex += 2;

                    break;
                /* 预留 */
                case 0x3F:
                    break;
                default:
                    break;
            }

            /* 第2次出现的指令数据，以TLV格式记录在szReserved字段中 */
            if(iaTimes[ucOrgCmd] >= 2)
            {
                szTlvTag[0] = 0xDF;
                szTlvTag[1] = 0x80|ucOrgCmd;
                szTlvTag[2] = iaTimes[ucOrgCmd];

                SetTLV(&tTlv, szTlvTag, iDataIndex-iCopyIndex, szData+iCopyIndex);
            }

            iCmdIndex = iCmdIndex+iCmdBytes;
        }
    }

    /* 记录POS指令解析日志 */
    WriteCmdData(ptApp->szTransName, ptApp->lPosTrace, "RCVEND", ptApp->iTransType);

    /* 保存TLV数据 */
    memset(szTlvBuf, 0, sizeof(szTlvBuf));
    iTlvLen = PackTLV(&tTlv, szTlvBuf);
    if(iTlvLen == FAIL)
    {
        WriteLog(ERROR, "TLV数据格式转换为字符串失败!");

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    if((iTlvLen+ptApp->iReservedLen) > sizeof(ptApp->szReserved))
    {
        WriteLog(ERROR,
                 "需要存入szReserved的数据长度太长!当前长度:[%d] 待保存数据长度:[%d] 最大数据长度:[%d]",
                 ptApp->iReservedLen, iTlvLen, sizeof(ptApp->szReserved));

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    memcpy(ptApp->szReserved+ptApp->iReservedLen, 
           szTlvBuf, iTlvLen);
    ptApp->iReservedLen += iTlvLen;

    return SUCC;
}
