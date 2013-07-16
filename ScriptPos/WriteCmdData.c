/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 记录POS指令数据
** 创 建 人：Robin
** 创建日期：2008/07/31
**
** $Revision: 1.2 $
** $Log: WriteCmdData.c,v $
** Revision 1.2  2013/03/11 07:15:40  fengw
**
** 1、新增自定义指令判断处理。
**
** Revision 1.1  2013/01/06 05:29:25  fengw
**
** 1、将WriteCmdData函数分离为单独文件。
**
*******************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/****************************************************************
** 功    能：在文件$HOME/log/POSYYYYMMDD.log中记录拆包后数据
** 输入参数：
**        szCmd                     指令代码
**        iStep                     指令步骤
**        szData                    输入数据
**        iLen                      数据长度
** 输出参数：
**        无
** 返 回 值：
**        无
** 作    者：
**        Robin
** 日    期：
**        2008/07/31
** 调用说明：
**
** 修改日志：
****************************************************************/
void WriteCmdData(char *szCmd, int iStep, char *szData, int iLen)
{
    FILE    *fp;                            /* 文件指针 */
    char    szFileName[128+1];              /* 日志文件名 */
    char    szDate[8+1];                    /* 日期 */
    char    cOrgCmd;                        /* 原始指令 */
    char    szCmdName[MAX_CMD_NAME+1];      /* 指令名称 */
    char    szCryptType[3+1];               /* 加密类型 */
    char    szCryptName[64+1];              /* 加密类型名称 */
    int     iCmdBytes;                      /* 指令字节数 */
    char    szTmpBuf[1024+1];               /* 临时变量 */
    int     i, j;

    memset(szDate , 0 ,sizeof(szDate));
    memset(szFileName, 0, sizeof(szFileName));

    GetSysDate(szDate);
    sprintf(szFileName, "%s/log/POS%s", getenv("WORKDIR"), szDate);
    fp = fopen(szFileName, "a+");
    if(fp == NULL)
    {
        WriteLog(ERROR, "打开POS指令日志文件[%s]失败!", szFileName);

        return;
    }

    /* 指令起始、结尾分割 */
    if(memcmp(szData, "RCVBEGIN", 8) == 0)
    {
        fprintf(fp, "from pos begin trace[%ld] TransType[%ld][%s]=======>\n", iStep, iLen, szCmd);
        fclose(fp);
        return;
    }
    else if(memcmp(szData, "RCVEND", 6) == 0)
    {
        fprintf(fp, "from pos end trace[%ld] TransType[%ld][%s]=======>\n", iStep, iLen, szCmd);
        fclose(fp);
        return;
    }
    else if(memcmp(szData, "SNDBEGIN", 8) == 0)
    {
        fprintf(fp, "to pos begin trace[%ld] TransType[%ld][%s]+++++++>\n", iStep, iLen, szCmd);
        fclose(fp);
        return;
    }
    else if(memcmp(szData, "SNDEND", 6) == 0)
    {
        fprintf(fp, "to pos end trace[%ld] TransType[%ld][%s]+++++++>\n\n", iStep, iLen, szCmd);
        fclose(fp);
        return;
    }

    if((szCmd[0] & 0xFF) == SPECIAL_CMD_HEAD)
    {
        iCmdBytes = SPECIAL_CMD_LEN;

        strcpy(szCryptName, "未加密");

        cOrgCmd = (szCmd[SPECIAL_CMD_LEN-1] & 0xFF);

        /* 指令名称 */
        memset(szCmdName, 0, sizeof(szCmdName));
        if(cOrgCmd > 0 && cOrgCmd <= MAX_SP_CMD_INDEX)
        {
            strcpy(szCmdName, gszaSpecialCmdName[cOrgCmd-1]);
        }
        else
        {
            strcpy(szCmdName, "未知指令");
        }
    }
    else
    {
        /* 计算指令字节数 */
        iCmdBytes = CalcCmdBytes((unsigned char)szCmd[0]);

        /* 获取加密类型 */
        memset(szCryptName, 0, sizeof(szCryptName));
        memset(szCryptType, 0, sizeof(szCryptType));

        if(iCmdBytes == 3)
        {
            /* 未加密-比特位1 */
            if((szCmd[2]&0x80) == 0) 
            {
                strcpy(szCryptName, "未加密"); 
            }
            /* 有加密-比特位1 */
            else
            {
                strcpy(szCryptName, "加密");
            }
        }
        else
        {
            strcpy(szCryptName, "未加密");
        }

        /* 指令名称 */
        cOrgCmd = szCmd[0]&0x3F;
        memset(szCmdName, 0, sizeof(szCmdName));
        if(cOrgCmd > 0 && cOrgCmd <= MAX_CMD_INDEX)
        {
            strcpy(szCmdName, gszaCmdName[cOrgCmd-1]);
        }
        else
        {
            strcpy(szCmdName, "未知指令");
        }
    }

    /* 打印指令 */
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    for(i=0;i<iCmdBytes;i++)
    {
        sprintf(szTmpBuf+i*3, "%02x ", szCmd[i]&0xFF);
    }

    /* 打印指令日志 */
    fprintf(fp, "%2d %s 指令长度[%d] 指令[%s] 加密算法[%s]\n", iStep, szCmdName, iCmdBytes, szTmpBuf, szCryptName);

    if(memcmp(szData, "FF", 2) == 0 && iLen == 2)
    {
        fprintf(fp, "无数据\n");
        fclose(fp);
        return;
    }

    /* 打印数据日志 */
    for(i=0;i<iLen;i+=25)
    {
        memset(szTmpBuf, 0, sizeof(szTmpBuf));
        for(j=0;j<25&&(i+j)<iLen;j++)
        {
            sprintf(szTmpBuf+3*j, "%02x ", szData[i+j]&0xff);
        }
        fprintf(fp, "%s\n", szTmpBuf);
    }

    fclose(fp);

    return;
}