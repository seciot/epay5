/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 解析自定义指令输入数据
** 创 建 人：fengwei
** 创建日期：2013/03/06
**
** $Revision: 1.1 $
** $Log: SpecialInput.c,v $
** Revision 1.1  2013/03/11 07:12:49  fengw
**
** 1、新增自定义指令输入、输出数据处理。
**
*******************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/****************************************************************
** 功    能：解析自定义指令输入数据
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
int SpecialInput(T_App *ptApp, char *szCmdData, char *szData)
{
    uchar   uCmd;                       /* 指令号 */
    int     iDataLen;                   /* 有效数据索引 */

    iDataLen = 0;
    uCmd = szCmdData[SPECIAL_CMD_LEN-1];

    switch(uCmd)
    {
        /* EMV参数版本号 */
        case 0x01:
            break;
        /* 保存EMV参数 */
        case 0x02:
            /* 两位长度 */
            szData[iDataLen] = (12 + ptApp->iEmvParaLen)/256;
            iDataLen += 1;

            szData[iDataLen] = (12 + ptApp->iEmvParaLen)%256;
            iDataLen += 1;

            /* EMV参数版本号 */
            memcpy(szData+iDataLen, ptApp->szEmvParaVer, 12);
            iDataLen += 12;

            /* EMV参数 */
            if(ptApp->iEmvParaLen > 0)
            {
                memcpy(szData+iDataLen, ptApp->szEmvPara, ptApp->iEmvParaLen);
                iDataLen +=  ptApp->iEmvParaLen;
            }

            break;
        /* 读取终端当前公钥版本号 */
        case 0x03:
            break;
        /* 保存EMV公钥 */
        case 0x04:
            /* 两位长度 */
            szData[iDataLen] = (8 + ptApp->iEmvKeyLen)/256;
            iDataLen += 1;
        
            szData[iDataLen] = (8 + ptApp->iEmvKeyLen)%256;
            iDataLen += 1;

            /* EMV公钥版本号 */
            memcpy(szData+iDataLen, ptApp->szEmvKeyVer, 8);
            iDataLen += 8;

            /* EMV公钥 */
            if(ptApp->iEmvKeyLen > 0)
            {
                memcpy(szData+iDataLen, ptApp->szEmvKey, ptApp->iEmvKeyLen);
                iDataLen += ptApp->iEmvKeyLen;
            }

            break;
        /* EMV完整流程处理 */
        case 0x05:
            break;
        /* EMV联机数据处理 */
        case 0x06:
            /* EMV数据长度 */
            szData[iDataLen] = ptApp->iEmvDataLen;
            iDataLen += 1;

            /* EMV数据 */
            if(ptApp->iEmvDataLen > 0)
            {
                memcpy(szData+iDataLen, ptApp->szEmvData, ptApp->iEmvDataLen);
                iDataLen += ptApp->iEmvDataLen;
            }

            break;
        default:
            WriteLog(ERROR, "非法自定义指令[%d]", uCmd);

            return FAIL;
    }

    return iDataLen;
}