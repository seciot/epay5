/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：终端下载类交易 下载EMV参数
** 创 建 人：fengwei
** 创建日期：2013/03/06
**
** $Revision: 1.1 $
** $Log: DownEmvPara.ec,v $
** Revision 1.1  2013/03/11 07:04:57  fengw
**
** 1、新增EMV参数下载交易。
**
*******************************************************************/

#include "manatran.h"

/****************************************************************
** 功    能：下载EMV参数
** 输入参数：
**        ptApp                     app结构指针
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2013/03/06
** 调用说明：
**
** 修改日志：
****************************************************************/

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

int DownEmvPara(T_App *ptApp) 
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szEmvParaVer[12+1];                 /* EMV参数版本号 */
        char    szEmvParaMaxVer[12+1];              /* EMV参数最大版本号 */
        char    szEmvPara[1024+1];                  /* EMV参数数据 */
    EXEC SQL END DECLARE SECTION;

    char    szTmpBuf[128+1];                        /* 临时变量 */

    /* 中心发起交易，用于判断是否送comweb */
    strcpy(ptApp->szAuthCode, "YES" );

    memset(szEmvParaVer, 0, sizeof(szEmvParaVer));
    memset(szEmvParaMaxVer, 0, sizeof(szEmvParaMaxVer));
    memset(szEmvPara, 0, sizeof(szEmvPara));

    if(strlen(ptApp->szEmvParaVer) == 0)
    {
        strcpy(szEmvParaVer, "000000000000");
    }
    else
    {
        strcpy(szEmvParaVer, ptApp->szEmvParaVer);
    }

    EXEC SQL
        SELECT MAX(Para_ver) INTO :szEmvParaMaxVer
        FROM emv_para;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询EMV参数最大版本号失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }
#ifdef DEBUG
    WriteLog(TRACE, "MAX EMV Para Ver:[%s]", szEmvParaMaxVer);
#endif

    EXEC SQL
        SELECT para_ver, para_data
        INTO :szEmvParaVer, :szEmvPara
        FROM
            (SELECT para_ver, para_data FROM emv_para
            WHERE para_ver > :szEmvParaVer OR para_ver = :szEmvParaMaxVer
            ORDER BY para_ver ASC)
        WHERE ROWNUM = 1;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询版本号大于[%s]的EMV参数记录失败!SQLCODE=%d SQLERR=%s",
                 szEmvParaVer, SQLCODE, SQLERR);

        return FAIL;
    }
    else
    {
        DelTailSpace(szEmvPara);

        memset(ptApp->szEmvParaVer, 0, sizeof(ptApp->szEmvParaVer));
        strcpy(ptApp->szEmvParaVer, szEmvParaVer);
        ptApp->iEmvParaLen = strlen(szEmvPara) / 2;
        AscToBcd(szEmvPara, strlen(szEmvPara), LEFT_ALIGN, ptApp->szEmvPara);
#ifdef DEBUG
        WriteLog(TRACE, "szEmvParaVer:[%s]", ptApp->szEmvParaVer);
        WriteLog(TRACE, "iEmvParaLen:[%d]", ptApp->iEmvParaLen);
        WriteLog(TRACE, "szEmvPara:[%s]", szEmvPara);
#endif

        /* 判断是否需要后续更新 */
        if(memcmp(szEmvParaVer, szEmvParaMaxVer, 12) < 0)
        {
            /* 清空后续指令 */
            memset(ptApp->szCommand, 0, sizeof(ptApp->szCommand));
            ptApp->iCommandLen = 0;
            ptApp->iCommandNum = 0;
            
            /* 后续交易代码前2位表示已下载记录最大记录号，后6位为当前交易 */
            memset(szTmpBuf, 0, sizeof(szTmpBuf));
            memcpy(szTmpBuf, ptApp->szTransCode, 2);

            memset(ptApp->szNextTransCode, 0, sizeof(ptApp->szNextTransCode));
            sprintf(ptApp->szNextTransCode, "%02d", atoi(szTmpBuf)+1);
            memcpy(ptApp->szNextTransCode+2, ptApp->szTransCode+2, 6);

            /* 更新EMV参数 */
            memcpy(ptApp->szCommand+ptApp->iCommandLen, "\xC0\x00\x02", 3);
            ptApp->iCommandLen += 3;
            ptApp->iCommandNum += 1;

            /* 读取EMV参数版本号 */
            memcpy(ptApp->szCommand+ptApp->iCommandLen, "\xC0\x00\x01", 3);
            ptApp->iCommandLen += 3;
            ptApp->iCommandNum += 1;

            /* 计算MAC */
            memcpy(ptApp->szCommand+ptApp->iCommandLen, "\x8D", 1);
            ptApp->iCommandLen += 1;
            ptApp->iCommandNum += 1;

            /* 发送数据 */
            memcpy(ptApp->szCommand+ptApp->iCommandLen, "\x24\x03", 2);
            ptApp->iCommandLen += 2;
            ptApp->iCommandNum += 1;

            /* 接收数据 */
            memcpy(ptApp->szCommand+ptApp->iCommandLen, "\x25\x04", 2);
            ptApp->iCommandLen += 2;
            ptApp->iCommandNum += 1;

            /* 需要后续下载，不送comweb */
            strcpy(ptApp->szAuthCode, "NO");
        }
    }

    /* 无后续交易，挂机 */
    if(strlen(ptApp->szNextTransCode) == 0 || memcmp(ptApp->szNextTransCode, "00000000", 8) == 0)
    {
        /* 挂机 */
        memcpy(ptApp->szCommand+ptApp->iCommandLen, "\xA6", 1);
        ptApp->iCommandLen += 1;
        ptApp->iCommandNum += 1;
    }
#ifdef DEBUG
    WriteLog(TRACE, "Next TransCode:[%s]", ptApp->szNextTransCode);
#endif

    strcpy(ptApp->szRetCode, TRANS_SUCC);

    return SUCC;
}