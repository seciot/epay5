/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 解析自定义指令输出数据
** 创 建 人：fengwei
** 创建日期：2013/03/06
**
** $Revision: 1.2 $
** $Log: SpecialOutput.c,v $
** Revision 1.2  2013/03/28 07:59:40  fengw
**
** 1、修改自定义指令06号数据解析。
**
** Revision 1.1  2013/03/11 07:12:49  fengw
**
** 1、新增自定义指令输入、输出数据处理。
**
*******************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/****************************************************************
** 功    能：解析自定义指令输出数据
** 输入参数：
**        ptApp                     app结构指针
**        szCmdData                 指令数据
**        szData                    有效数据
** 输出参数：
**        无
** 返 回 值：
**        >0                        数据长度
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2013/03/06
** 调用说明：
**
** 修改日志：
****************************************************************/
int SpecialOutput(T_App *ptApp, char *szCmdData, char *szData)
{
    uchar   uCmd;                       /* 指令号 */
    int     iDataIndex;                 /* 有效数据索引 */
    char    szTmpBuf[1024+1];           /* 临时变量 */
    int     iTmp;                       /* 临时变量 */

    iDataIndex = 0;
    uCmd = szCmdData[SPECIAL_CMD_LEN-1];

    switch(uCmd)
    {
        /* EMV参数版本号 */
        case 0x01:
            memcpy(ptApp->szEmvParaVer, szData+iDataIndex, 12);
            iDataIndex += 12;
#ifdef DEBUG
            WriteLog(TRACE, "EMV参数版本号:[%s]", ptApp->szEmvParaVer);
#endif

            break;
        /* 保存EMV参数 */
        case 0x02:
            /* EMV参数版本号 */
            memcpy(ptApp->szEmvParaVer, szData+iDataIndex, 12);
            iDataIndex += 12;
#ifdef DEBUG
            WriteLog(TRACE, "EMV参数版本号:[%s]", ptApp->szEmvParaVer);
#endif

            /* 保存结果 */
            memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
            iDataIndex += 2;
#ifdef DEBUG
            WriteLog(TRACE, "保存结果:[%s]", ptApp->szHostRetCode);
#endif

            break;
        /* 读取终端当前公钥版本号 */
        case 0x03:
            /* EMV公钥版本号 */
            memcpy(ptApp->szEmvKeyVer, szData+iDataIndex, 8);
            iDataIndex += 8;
#ifdef DEBUG
            WriteLog(TRACE, "EMV公钥版本号:[%s]", ptApp->szEmvKeyVer);
#endif

            break;
        /* 保存EMV公钥 */
        case 0x04:
            /* EMV参数版本号 */
            memcpy(ptApp->szEmvKeyVer, szData+iDataIndex, 8);
            iDataIndex += 8;
#ifdef DEBUG
            WriteLog(TRACE, "EMV公钥版本号:[%s]", ptApp->szEmvKeyVer);
#endif

            /* 保存结果 */
            memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
            iDataIndex += 2;
#ifdef DEBUG
            WriteLog(TRACE, "保存结果:[%s]", ptApp->szHostRetCode);
#endif

            break;
        /* EMV完整流程处理 */
        case 0x05:
            /* 交易金额 */
            memset(szTmpBuf, 0, sizeof(szTmpBuf));
            BcdToAsc((uchar*)(szData+iDataIndex), 12, LEFT_ALIGN, szTmpBuf);

            memcpy(ptApp->szAmount, szTmpBuf, 12);
            iDataIndex += 6;
#ifdef DEBUG
            WriteLog(TRACE, "交易金额:[%s]", ptApp->szAmount);
#endif

            /* 密码密文 */
            memcpy(ptApp->szPasswd, szData+iDataIndex, 8);
            iDataIndex += 8;

            /* 卡序号 */
            memset(szTmpBuf, 0, sizeof(szTmpBuf));
            BcdToAsc((uchar*)(szData+iDataIndex), 4, LEFT_ALIGN, szTmpBuf);
            iDataIndex += 2;
#ifdef DEBUG
            WriteLog(TRACE, "卡序号:[%s]", ptApp->szEmvCardNo);
#endif

            /* 持卡人姓名长度 */
            iTmp = (uchar)(szData[iDataIndex]);	
            iDataIndex += 1;

            /* 持卡人姓名 */
            if(iTmp > 0)
            {
                memcpy(ptApp->szHolderName, szData+iDataIndex, iTmp>40?40:iTmp);
                iDataIndex += iTmp;
            }
#ifdef DEBUG
            WriteLog(TRACE, "持卡人姓名:[%s]", ptApp->szHolderName);
#endif

            /* 磁道信息 */
            if((iTmp = GetTrack(ptApp, szData+iDataIndex)) == FAIL)
            {
                return FAIL;
            }
            iDataIndex += iTmp;

            /* 根据磁道信息获取卡号 */
            if(GetCardType(ptApp) != SUCC)
            {
                WriteLog(ERROR, "根据磁道信息获取卡号失败!");

                return FAIL;
            }

            /* EMV数据长度 */
            ptApp->iEmvDataLen = (uchar)(szData[iDataIndex]);	
    	    iDataIndex += 1;

            /* EMV数据 */
            if(ptApp->iEmvDataLen > 0)
            {
                memcpy(ptApp->szEmvData, szData+iDataIndex, ptApp->iEmvDataLen);
                iDataIndex += ptApp->iEmvDataLen;
            }

            break;
        /* EMV联机数据处理 */
        case 0x06:
            /* 交易结果 */
            memcpy(ptApp->szHostRetCode, szData+iDataIndex, 2);
            iDataIndex += 2;

            /* 以下数据根据实际情况处理 */
            /* 原交易日期 */
            iDataIndex += 4;

            /* 原交易流水号 */
            memset(szTmpBuf, 0, sizeof(szTmpBuf));
            BcdToAsc(szData+iDataIndex, 6, 0, szTmpBuf);
            iDataIndex += 3;
            ptApp->lOldPosTrace = atol(szTmpBuf);

            /* EMV证书长度 */
            ptApp->iEmvTcLen  = (uchar)(szData[iDataIndex]);
    		iDataIndex += 1;
    
            /* EMV证书 */
            if(ptApp->iEmvTcLen > 0)
            {
                memcpy(ptApp->szEmvTc, szData+iDataIndex, ptApp->iEmvTcLen);
            }
            iDataIndex += ptApp->iEmvTcLen;
    
            /* EMV脚本长度 */
            ptApp->iEmvScriptLen  = (uchar)(szData[iDataIndex]);
    		iDataIndex += 1;
    
            /* EMV脚本 */
            if(ptApp->iEmvScriptLen > 0)
            {
                memcpy(ptApp->szEmvScript, szData+iDataIndex, ptApp->iEmvScriptLen);
            }
            iDataIndex += ptApp->iEmvScriptLen;
            
            break;
        default:
            WriteLog(ERROR, "非法自定义指令[%d]", uCmd);

            return FAIL;
    }

    return iDataIndex;
}