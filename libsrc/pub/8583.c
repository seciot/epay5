/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：8583打包拆包接口
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.13 $
 * $Log: 8583.c,v $
 * Revision 1.13  2012/12/17 02:27:05  chenrb
 * WriteERLog修改成WriteLog
 *
 * Revision 1.12  2012/12/04 02:05:30  chenjr
 * 代码规范化
 *
 * Revision 1.11  2012/11/29 02:15:35  linqil
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.10  2012/11/28 08:25:44  linqil
 * 去掉冗余头文件user.h
 *
 * Revision 1.9  2012/11/27 08:21:01  linqil
 * 修改日志函数
 *
 * Revision 1.8  2012/11/27 02:53:49  yezt
 * *** empty log message ***
 *
 * Revision 1.7  2012/11/26 08:53:53  yezt
 * *** empty log message ***
 *
 * Revision 1.6  2012/11/26 08:40:13  linqil
 * 增加对pub.h的引用，0 -1 替换为SUCC、FAIL
 *
 * Revision 1.5  2012/11/26 08:35:51  linqil
 * 增加头文件pub.h；对头文件pub.h的引用；修改return 0 return -1 为return SUCC return FAIL；
 *
 * Revision 1.4  2012/11/26 08:29:37  chenjr
 * 添加注释
 *
 * Revision 1.3  2012/11/26 08:27:53  chenjr
 * 创建
 *
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "8583.h"

#include "pub.h"
#include "libpub.h"

/* ASC BCD HEX 转DEC */
int Asc2Dec(unsigned char *, int, int *);
int Bcd2Dec(unsigned char *, int, int *);
int Hex2Dec(unsigned char *, int, int *);

/* DEC 转 ASC BCD HEX */
int Dec2Asc(unsigned char *, int, int);
int Dec2Bcd(unsigned char *, int, int);
int Dec2Hex(unsigned char *, int, int);


/* ----------------------------------------------------------------
 * 功    能：清空ISO_data存储空间
 * 输入参数：ptData   ISO结构体指针
 * 输出参数：无
 * 返 回 值：无
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void ClearBit(ISO_data *ptData)
{
    int i;

    for (i = 0; i < 128; i++)
    {
        ptData ->f[i].bitf = 0;
    }
    ptData->off = 0;
}

/* ----------------------------------------------------------------
 * 功    能： 获取8583包指定域内容
 * 输入参数： ptMR    ---- 报文规则
 *            ptData  ---- ISO结构体执政
 *            iNO     ---- 域号
 * 输出参数： szDest  ---- 输出字符串
 * 返 回 值： 返回内容长度
 * 作    者： 陈建荣
 * 日    期： 2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetBit(MsgRule *ptMR, ISO_data *ptData, int iNo, char *szDest)
{
    int iLen;
    unsigned char *pMsg;

    if (iNo == 0)
    {
        memcpy(szDest, ptData->message_id, 4);
        szDest[4] = '\0';
        return 4;
    }

    if (iNo <= 1 || iNo > 128 || ptData->f[iNo - 1].bitf == 0)
    {
        return SUCC;
    }

    iNo--;
    pMsg = (unsigned char*)&(ptData->dbuf[ptData->f[iNo].dbuf_addr]);
    iLen = ptData->f[iNo].len;

    if (ptMR->ptISO[iNo].type & 0x01)
    {
        *szDest++ = *pMsg++;
    }

    if (ptMR->ptISO[iNo].type & 0x07)
    {
        if (ptMR->ptISO[iNo].type & 0x03)
        {
            BcdToAsc(pMsg, iLen, 0, (unsigned char*)szDest);
        }
        else
        {
            BcdToAsc(pMsg, iLen, 1, (unsigned char*)szDest);
        }
    }
    else
    {
        memcpy(szDest, pMsg, iLen);
    }

    szDest[iLen] = '\0';
    return iLen;
}

/* ----------------------------------------------------------------
 * 功    能：置8583包指定域内容
 * 输入参数：ptMR    -----  报文规则
 *           szSrc   -----  指定域内容
 *           iNo     -----  域号
 *           iLen    -----  域内容长度
 * 输出参数：ptData  -----  ISO结构指针
 * 返 回 值：0 成功/-1 失败
 * 作    者：陈建荣
 * 日    期：2012/12/06
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int SetBit(MsgRule *ptMR, char *szSrc, int iNo, int iLen, ISO_data *ptData)
{
    int i, iSpecLen;   /* 域规定长度 */
    unsigned char *pDbuf, szTmp[DBUFSIZE], cTmp;
   
    if (iNo == 0)
    {
        memcpy(ptData->message_id, szSrc, 4);
        ptData->message_id[4] = '\0';
        return SUCC;
    }

    if (iLen == 0 || iNo <= 1 || iNo > 128)
    {
        return FAIL;
    }

    iNo--;

    /* 确定真实值(iLen)与规定值(iSpecLen) */
    if (iLen > ptMR->ptISO[iNo].len)
    {
        iLen = ptMR->ptISO[iNo].len;
    }

    iSpecLen = iLen;

    if (ptMR->ptISO[iNo].flag == 0)
    {
        iSpecLen = ptMR->ptISO[iNo].len;
    }

    /* 防存储溢出 */
    if (iSpecLen + ptData->off > DBUFSIZE)
    {
        return FAIL;
    }

    /* 置域有值标识 */
    ptData->f[iNo].bitf = 1;
    /* 置域值长度 */
    ptData->f[iNo].len = iSpecLen;
    /* 置域起始地址 */
    ptData->f[iNo].dbuf_addr =  ptData->off;
    pDbuf = (unsigned char*)&(ptData->dbuf[ptData->off]);
    /* 置存储区下一可用下标起始值 */
    ptData->off += iSpecLen;

    /* 域内容处理 */
    if (ptMR->ptISO[iNo].type & 0x01)  
    {
        *(pDbuf++) = *(szSrc++);
        ptData->off += 1;
    }

    i = 0;
    if (ptMR->ptISO[iNo].type & 0x03)
    {
        for (; i < iSpecLen - iLen; i++)
        {
            szTmp[i] = '0';
        }
    }

    memcpy(szTmp + i, szSrc, iLen);
    i += iLen;

    if (ptMR->ptISO[iNo].type & 0x08)
    {
        cTmp = 0;
    }
    else 
    {
        cTmp = ' '; 
    }

    for (; i < iSpecLen; i++)
    {
        szTmp[i] = cTmp;
    }
    
    /* 域值保存 */
    if (ptMR->ptISO[iNo].type & 0x07)
    {
        if (ptMR->ptISO[iNo].type & 0x03)
        {
            AscToBcd(szTmp, iSpecLen, 0, pDbuf);
        }
        else
        {
            AscToBcd(szTmp, iSpecLen, 1, pDbuf);
        }
    }
    else
    {
        memcpy(pDbuf, szTmp, iSpecLen);
    }

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：8583打包
 * 输入参数：ptMR     -----  报文规则
 *           ptData   -----  ISO结构体指针
 * 输出参数：szDest   -----  打包后的报文串
 * 返 回 值：8583包报文长度
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int IsoToStr(MsgRule *ptMR, ISO_data *ptData, unsigned char *szDest)
{
    int iBitNum = 8, iMidLen = 4, i, j, iFieldNo, iFieldLen, iOffset = 0;
    unsigned char *pMsg, cBitMask, cBitMap;
    int (*pfStorLen)(unsigned char *, int, int);

    switch (ptMR->iFieldLenType)
    {
    case FIELDLENTYPE_ASC:
        pfStorLen = Dec2Asc;
        break;

    case FIELDLENTYPE_BCD:
        pfStorLen = Dec2Bcd;
        break;

    default:
        pfStorLen = Dec2Hex;
        break;
    }

    if (ptMR->iMidType == MSGIDTYPE_ASC)
    {
        memcpy(szDest, ptData->message_id, 4);
    }
    else
    {
        AscToBcd((unsigned char*)ptData->message_id, 4, 0, 
                 (unsigned char*)szDest);
        iMidLen = 2;
    }
 
    for (iFieldNo = 64; iFieldNo < 128; iFieldNo++)
    {
        if (ptData->f[iFieldNo].bitf)
        {
            iBitNum = 16;
            break;
        }
    }

    pMsg = szDest + iMidLen + iBitNum;

    for (i=0; i<iBitNum; i++)
    {
        cBitMap = 0;
        cBitMask = 0x80;

        for (j=0; j<8; j++, cBitMask >>= 1)
        {
            iFieldNo = (i << 3) + j;
            if (ptData->f[iFieldNo].bitf == 0)
            {
                continue;
            }
 
            cBitMap |= cBitMask;
            iFieldLen = ptData->f[iFieldNo].len;

            if (ptMR->ptISO[iFieldNo].flag > 0)
            {
                /* 转换域长度(DEC)为相应编码后拼接在串后 */
                pMsg += (unsigned char)(*pfStorLen)(pMsg,
                                         ptMR->ptISO[iFieldNo].flag,
                                         iFieldLen);
            }
            iOffset = 0;
            if (ptMR->ptISO[iFieldNo].type & 0x01)
            {
                (*pMsg++) = ptData->dbuf[ptData->f[iFieldNo].dbuf_addr + 
                                        iOffset];
                iOffset++;
                iFieldLen++;
            }

            if (ptMR->ptISO[iFieldNo].type & 0x07)
            {
                iFieldLen++;
                iFieldLen >>= 1;
            }

            for (; iOffset < iFieldLen; iOffset++)
            {
                (*pMsg++) = ptData->dbuf[ptData->f[iFieldNo].dbuf_addr +
                                        iOffset];
            }
        } /* for j [bit] */

        szDest[i + iMidLen] = cBitMap;
    }  /* for i [Byte] */

    if (iBitNum == 16)
    {
        szDest[iMidLen] |= 0x80;
    }

    return (pMsg - szDest);
}

/* ----------------------------------------------------------------
 * 功    能：8583拆包
 * 输入参数：ptMR   ----  报文规则
 *           szSrc  ----  8583报文串
 * 输出参数：ptData ----  ISO结构体指针
 * 返 回 值：-1   失败/0 成功
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int StrToIso(MsgRule *ptMR, unsigned char *szSrc, ISO_data *ptData)
{
    int iBitNum = 8, iMidLen = 4, i, j, iFieldNo, iFieldLen, iOffset = 0;
    unsigned char *pMsg, cBitMask;
    int (*pfCaclLen)(unsigned char *, int, int *);

    /* 初始化 */
    ClearBit(ptData);

    switch (ptMR->iFieldLenType)
    {
    case FIELDLENTYPE_ASC:
        pfCaclLen = Asc2Dec;
        break;

    case FIELDLENTYPE_BCD:
        pfCaclLen = Bcd2Dec;
        break;

    default:
        pfCaclLen = Hex2Dec;
        break;
    }

    if (ptMR->iMidType == MSGIDTYPE_ASC)
    {
        memcpy(ptData->message_id, szSrc, 4);
    }
    else
    {
        BcdToAsc((unsigned char*)szSrc, 4, 0, 
                 (unsigned char*)ptData->message_id);
        iMidLen = 2;
    }
    ptData->message_id[4] = '\0';

    /* 判断是否有扩展域 */
    if (szSrc[iMidLen] & 0x80)
    {
        iBitNum = 16;
    }

    /* pMsg指向报文域实体起始处 */
    pMsg = szSrc + iMidLen + iBitNum;

    /* 解析报文串 */
    for (i = 0; i < iBitNum; i++)
    {
        cBitMask = 0x80;
        for (j = 0; j < 8; j++, cBitMask >>= 1)
        {
            if (i == 0 && cBitMask == 0x80)
            {
                continue;
            }

            if ((szSrc[iMidLen + i] & cBitMask) == 0)
            {
                continue;
            }
    
            iFieldNo = (i << 3) + j;

            /* 取指定域有效长度 */
            if (ptMR->ptISO[iFieldNo].flag > 0)
            {
                /* 调用相应函数完成有效长度计算 */
                pMsg += (unsigned char)(*pfCaclLen)(pMsg, 
                                         ptMR->ptISO[iFieldNo].flag, 
                                         &iFieldLen);

                if (iFieldLen > ptMR->ptISO[iFieldNo].len)
                {
                    WriteLog(ERROR, "field[%d] too long[Max:%d-Cur%d]",
                          iFieldNo, ptMR->ptISO[iFieldNo].len, iFieldLen);
                    return FAIL;
                }
            }
            else
            {
                iFieldLen = ptMR->ptISO[iFieldNo].len;
            }

            ptData->f[iFieldNo].len       = iFieldLen;
            ptData->f[iFieldNo].dbuf_addr = iOffset;

            /*  Credit or Debit char isn't include in the len */
            if (ptMR->ptISO[iFieldNo].type  & 0x01)
            {
                ptData->dbuf[iOffset++] = *pMsg++;
            }

            /* BCD field Bytes = len / 2 [bit1, bit2]*/
            if (ptMR->ptISO[iFieldNo].type  & 0x07)
            {
                iFieldLen++;
                iFieldLen >>= 1;
            }

            /* 防止越界 */
            if (iFieldLen + iOffset >= DBUFSIZE)
            {
                WriteLog(ERROR, "the total length is too long[%d]\n",
                           iFieldLen + iOffset);
                return FAIL;
            }

            while (iFieldLen > 0)
            {
                ptData->dbuf[iOffset++] = *pMsg++;
                iFieldLen--;
            }
            ptData->f[iFieldNo].bitf = 1;
        } /* for j (bit)*/
    } /*for i (Byte)*/

    ptData->off = iOffset;
    return SUCC;
} /*StrToIso */


/* ----------------------------------------------------------------
 * 功    能： 组装指定域内容，包括该域的域号，长度，bcd码内容，asc码内容等
 * 输入参数： szSrc    ----- 报文串
 *            iFieldNo ----- 域号
 *            iLen     ----- 域长度内容
 * 输出参数： szDest   ----- 拼装后的串
 * 返 回 值： 输出数据长度
 * 作    者： 陈建荣
 * 日    期： 2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
static int PrtField(unsigned char *szSrc, int iFieldNo, int iLen, char *szDest)
{
    int l, j, k;
    char *pHead;

    pHead = szDest;

    sprintf(szDest, "field[%d] len=[%d]\n%n", 
            iFieldNo == 0 ? 0 : iFieldNo + 1, iLen, &l);

    for (j = 0; j < iLen / 16; j++)
    {
        sprintf(szDest += l, "%04xh:%n", j*16, &l);

        for (k = 0; k < 16; k++)
        {
            sprintf(szDest += l, "%02x %n", szSrc[j*16+k], &l);
        }

        sprintf(szDest += l, "|%n", &l);

        for ( k = 0; k < 16; k ++ )
        {
            if (szSrc[j*16+k] >= 0x30 && szSrc[j*16+k] <= 0x7e )
            {
                sprintf(szDest += l, "%c%n", 
                        (unsigned char)szSrc[j*16+k], &l);
            }
            else
            {
                sprintf(szDest += l, ".%n", &l);
            }
        }

        sprintf(szDest += l, "\n%n", &l);
    }

    if (iLen % 16 != 0 )
    {
        sprintf(szDest += l, "%04xh:%n", j*16, &l);
        for( k = 0; k< iLen % 16; k++ )
        {
            sprintf(szDest += l, "%02x %n", szSrc[j*16+k], &l);
        }

        for (k = 0; k< (48 - (iLen % 16) * 3); k ++)
        {
            sprintf(szDest += l, "%s%n", " ", &l);
        }

        sprintf(szDest += l, "|%n", &l);

        for (k = 0; k < 0 + iLen%16; k ++)
        {
            if (szSrc[j*16+k] >= 0x30 && szSrc[j*16+k] <= 0x7e )
            {
                sprintf(szDest += l, "%c%n", 
                        (unsigned char )szSrc[j*16+k], &l);
            }
            else
            {
                sprintf(szDest += l, ".%n", &l);
            }
        }
        sprintf(szDest += l, "\n%n", &l);
    }

    szDest += l;
    return (szDest - pHead);
}


/* ----------------------------------------------------------------
 * 功    能：输出8583包的debug日志
 * 输入参数：ptMR    ----- 报文规则
 *           ptData  ----- ISO结构体指针
 * 输出参数：szDest  ----- 日志输出字符串指针
 * 返 回 值：0 成功
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int DebugIso8583(MsgRule *ptMR, ISO_data *ptData, char *szDest)
{
    int            i, len;
    unsigned char  *pt, str[500];

    sprintf(szDest, "========================================\n%n", &len);

    AscToBcd((unsigned char*)ptData->message_id, 4, 0, str);

    szDest += len;
    szDest += PrtField(str, 0, 2, szDest);

    for (i = 0; i < 128; i ++)
    {
        if (ptData ->f[i].bitf != 1)
        {
            continue;
        }

        pt = (unsigned char *)&ptData->dbuf[ptData->f[i].dbuf_addr];

        if (ptMR->ptISO[i].type & 0x07 )
        {
            len = ( ptData->f[i].len + 1 )/ 2;
        }
        else
        {
            len = ptData-> f[i].len;
        }

        memcpy( (char *)str, (char *)pt, len );
        str[len] = 0;

        szDest += PrtField(str, i, len, szDest);
    }

    sprintf(szDest, "========================================\n%n", &len);
    sprintf( szDest += len, "\n\n%n", &len );

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：计算Asc码域的有效长度 
 * 输入参数：szSrc    -----  字符串指针   
 *           iLenType -----  报文长度类型
 * 输出参数：iLen     -----  报文实际长度 
 * 返 回 值：长度所占字节数
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int Asc2Dec(unsigned char *szSrc, int iLenType, int *piLen)
{
    int iOffSet, iVal = 0, i = 0;

    iOffSet = (iLenType == 1 ? 2 : 3);

    while (i < iOffSet)
    {
        iVal = iVal * 10 + ( (*szSrc) - '0');
        szSrc++;
        i++;
    }

    *piLen = iVal;

    return (iOffSet);
}

/* ----------------------------------------------------------------
 * 功    能：计算Hex码域的有效长度 
 * 输入参数：szSrc    -----  字符串指针   
 *           iLenType -----  报文长度类型
 * 输出参数：iLen     -----  报文实际长度 
 * 返 回 值：长度所占字节数
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int Hex2Dec(unsigned char *szSrc, int iLenType, int *piLen)
{
    int iOffSet, iVal = 0, i = 0;

    iOffSet = (iLenType == 1 ? 1 : 2);

    while (i < iOffSet)
    {
        iVal <<= 8;
        iVal += *(szSrc + i);
        i++;
    }

    *piLen = iVal;
    return (iOffSet);
}


/* ----------------------------------------------------------------
 * 功    能：计算Bcd码域的有效长度 
 * 输入参数：szSrc    -----  字符串指针   
 *           iLenType -----  报文长度类型
 * 输出参数：piLen    -----  报文实际长度 
 * 返 回 值：长度所占字节数
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int Bcd2Dec(unsigned char *szSrc, int iLenType, int *piLen)
{
    int iOffSet, iVal = 0, i = 0;

    iOffSet = (iLenType == 1 ? 1 : 2);

    while (i < iOffSet)
    {
        iVal = iVal * 100 + (*szSrc) - (*szSrc>>4) * 6;
        szSrc++;
        i++;
    }
    *piLen = iVal;
    return (iOffSet);
}

/* ----------------------------------------------------------------
 * 功    能：将有效域长度转换为Asc码
 * 输入参数：iLenType  ----- 报文长度类型
 *           iLen      ----- 报文实际长度
 * 输出参数：szSrc     ----- 字符串指针 (实际长度存储该字符串中)
 * 返 回 值：长度所占字节数
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int Dec2Asc(unsigned char *szSrc, int iLenType, int iLen)
{
    int iOffSet;

    if (iLenType == 1)
    {
        iOffSet = 2;
        (*szSrc++) = (unsigned char)(iLen / 10) + '0';
        (*szSrc++) = (unsigned char)(iLen % 10) + '0';
    }
    else
    {
        iOffSet = 3;
        (*szSrc++) = (unsigned char)(iLen / 100) + '0';
        iLen %= 100;
        (*szSrc++) = (unsigned char)(iLen / 10) + '0';
        iLen %= 10;
        (*szSrc++) = (unsigned char)iLen + '0';
    }

    return iOffSet;
}

/* ----------------------------------------------------------------
 * 功    能：将有效域长度转换为Hex码
 * 输入参数：iLenType  ----- 报文长度类型
 *           iLen      ----- 报文实际长度
 * 输出参数：szSrc     ----- 字符串指针 (实际长度存储该字符串中)
 * 返 回 值：长度所占字节数
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int Dec2Hex(unsigned char *szSrc, int iLenType, int iLen)
{
    int iOffSet;

    if (iLenType == 1)
    {
        iOffSet = 1;
        (*szSrc++) = (unsigned char)iLen;
    }
    else
    {
        iOffSet = 2;
        (*szSrc++) = (unsigned char)(iLen / 256);
        (*szSrc++) = (unsigned char)(iLen % 256);
    }

    return iOffSet;
}

/* ----------------------------------------------------------------
 * 功    能：将有效域长度转换为bcd码
 * 输入参数：iLenType  ----- 报文长度类型
 *           iLen      ----- 报文实际长度
 * 输出参数：szSrc     ----- 字符串指针 (实际长度存储该字符串中)
 * 返 回 值：长度所占字节数
 * 作    者：陈建荣
 * 日    期：2012/12/26
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int Dec2Bcd(unsigned char *szSrc, int iLenType, int iLen)
{
    int iOffSet;

    if (iLenType == 1)
    {
        iOffSet = 1;
        (*szSrc++) = (unsigned char)((iLen % 10) + (iLen / 10) * 16);
    }
    else
    {
        iOffSet = 2;
        (*szSrc++) = (unsigned char)(iLen / 100);
        (*szSrc++) = (unsigned char)(((iLen % 100) / 10) * 16 + (iLen % 100) % 10);
    }

    return iOffSet;
}
