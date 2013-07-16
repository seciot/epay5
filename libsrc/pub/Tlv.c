/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：TLV数据格式处理
 *  创 建 人：fengwei
 *  创建日期：2012-9-24
 * ----------------------------------------------------------------
 * $Revision: 1.8 $
 * $Log: Tlv.c,v $
 * Revision 1.8  2012/12/21 05:40:31  wukj
 * *** empty log message ***
 *
 * Revision 1.7  2012/12/20 06:08:21  chenrb
 * 修正DebugTLV函数中错误调用BcdToAsc的问题
 *
 * Revision 1.6  2012/12/17 07:16:53  fengw
 *
 * 1、修改DebugTLV函数，将TLV数据直接输入到TRACE日志。
 *
 * Revision 1.5  2012/12/14 09:17:55  fengw
 *
 * 1、修改GetValueByTag函数返回值。
 * 2、修改PackTLV函数注释。
 *
 * Revision 1.4  2012/12/13 05:06:45  fengw
 *
 * 1、命名规范化。
 *
 * Revision 1.3  2012/11/27 06:51:08  linqil
 * 增加引用pub.h 修改return语句
 *
 * Revision 1.2  2012/11/20 05:39:22  chenjr
 * modi interface (str_ASCTOBCD、str_BCDTOASC-->AscToBcd BcdToAsc)
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#include <string.h>
#include <stdio.h>

#include "Tlv.h"
#include "pub.h"
#include "user.h"

/* ----------------------------------------------------------------
 * 功    能：计算Tag占用字节数
 * 输入参数：szTag     Tag标签
 *           iTagType  Tag(标签)类型;
 *                     0 普通类型,一个Tag占用一个字节
 *                     1 标准类型,若Tag标签的第一个字节(字节按从左往右排序
 *                       最左边的为第一个字节，bit顺序正好相反)的bit1-bit5
 *                       为"11111",则说明Tag长度不止一个字节,具体有几字节
 *                       参考下一字节的bit8,如果bit8为1,再看下一字节的bit8
 *                       直至下一字节的bit8为0止算是Tag的最后一个字节.
 * 输出参数：
 * 返 回 值：>0 Tag长度(单位字节)；0 计算失败
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
static int _calc_tag_len(char* szTag, int iTagType)
{
    int iTagLen;
    int i;

    switch (iTagType)
    {
        case TAG_NORMAL:
            /* Tag长度固定为一字节 */
            iTagLen = 1;
            break;
        case TAG_STANDARD:
            /* 第一个字节bit5-bit1为11111时，Tag长度参照下一个字节 */
            if ((szTag[0] & 0x1F) == 0x1F)
            {
                /* 后续字节bit8为1,表示后续还有一个字节,为0表示最后一个字节 */
                for (i=1; i<MAX_TAG_LEN;i++)
                {
                    if ((szTag[i] & 0x80) != 0x80)
                    {
                        break;
                    }
                }

                iTagLen = 1 + i;
            }
            /* 否则Tag长度为一字节 */
            else
            {
                iTagLen = 1;
            }
            break;
        default:
            iTagLen = 0;
            break;
    }

    return iTagLen;
}

/*****************************************************************
** 功    能：设置TLV中Tag值
** 输入参数：
**		szTag		Tag
**		iTagType	Tag类型，说明参考_calc_tag_len函数参数说明
** 输出参数：
**		szTagBuf	存放Tag值Buf
** 返 回 值： 
**		>0		Tag占用字节数
**		0		设置失败
** 作    者：
**		fengwei
** 日    期：
**		2012/09/24
** 调用说明：
**		仅供tlv库函数内部调用，外部不调用
** 修改日志：
****************************************************************/
static int _set_tag(char* szTag, int iTagType, char* szTagBuf)
{
	int iTagLen;

	iTagLen = _calc_tag_len(szTag, iTagType);

	if(iTagLen > 0 && iTagLen <= MAX_TAG_LEN)
	{
		memcpy(szTagBuf, szTag, iTagLen);
	}

	return iTagLen;
}

/*****************************************************************
** 功    能：从TLV格式数据中获取Tag值
** 输入参数：
**		szBuf		TLV格式数据
**		iTagType	Tag类型，说明参考_calc_tag_len函数参数说明
** 输出参数：
**		szTagBuf	存放Tag值Buf
** 返 回 值： 
**		>0		Tag占用字节数
**		0		读取失败
** 作    者：
**		fengwei
** 日    期：
**		2012/09/24
** 调用说明：
**		仅供tlv库函数内部调用，外部不调用
** 修改日志：
****************************************************************/
static int _get_tag(char* szBuf, int iTagType, char* szTagBuf)
{
	int iTagLen;

	iTagLen = _calc_tag_len(szBuf, iTagType);

	if(iTagLen > 0 && iTagLen <= MAX_TAG_LEN)
	{
		memcpy(szTagBuf, szBuf, iTagLen);
	}

	return iTagLen;
}

/*****************************************************************
 * 功    能：设置TLV中Len值
 * 输入参数：iLen		Len值
 *           iLenType	Len类型
 *                      0：普通类型，Len值占用一个字节(HEX格式)，
 *                         取值范围0-255
 *                      1：标准类型，若Len最左边的bit位值为0时，
 *                         Len值占用一个字节，bit7-bit1代表长度，取值
 *                         范围为0-127；若Len最左边的bit位值为1时，
 *                         Len值占用2-3个字节，bit7-bit1代表Len值占用字节
 *                         数，例如，若最左字节为10000010，表示L字段除该
 *                         字节外，后面还有两个字节。其后续字节的十进
 *                         制取值表示子域取值的长度。
 *                         备注：如增加新类型，则需要在tlv.h中增加类型定
 *                         义及增加函数中switch的case选项
 * 输出参数：szLenBuf	存放Len值Buf
 * 返 回 值： >0		Len占用字节数
 *            0		设置失败
 * 作    者：fengwei
 * 日    期：2012/09/24
 * 调用说明：仅供tlv库函数内部调用，外部不调用
 * 修改日志：
 ***************************************************************/
static int _set_len(int iLen, int iLenType, char* szLenBuf)
{
	char szTmpBuf[MAX_LEN_LEN+1];
	int iLenLen;

	iLenLen = 0;

	memset(szTmpBuf, 0, sizeof(szTmpBuf));

	switch(iLenType)
	{
		case LEN_NORMAL:
			if(iLen >= 0 && iLen <= 255)
			{
				szTmpBuf[0] = iLen;
				iLenLen = 1;
			}
			break;
		case LEN_STANDARD:
			if(iLen <= 127)
			{
				szTmpBuf[0] = iLen;
				iLenLen = 1;
			}
			else if(iLen <= 256)
			{
				szTmpBuf[0] = 0x81;
				szTmpBuf[1] = iLen;
				iLenLen = 2;
			}
			else
			{
				szTmpBuf[0] = 0x82;
				szTmpBuf[1] = iLen / 256;
				szTmpBuf[2] = iLen % 256;
				iLenLen = 3;
			}
			break;
		default:
			iLenLen = 0;
			break;
	}

	if (iLenLen > 0 && iLenLen <= MAX_LEN_LEN)
	{
		memcpy(szLenBuf, szTmpBuf, iLenLen);
	}

	return iLenLen;
}

/*****************************************************************
** 功    能：从TLV格式数据中获取Len值
** 输入参数：
**		szBuf		TLV格式数据
**		iLenType	Len类型，说明参考_set_len函数参数说明
** 输出参数：
**		iLen		Len值
** 返 回 值： 
**		>0		Len占用字节数
**		0		读取失败
** 作    者：
**		fengwei
** 日    期：
**		2012/09/24
** 调用说明：
**		仅供tlv库函数内部调用，外部不调用
** 修改日志：
****************************************************************/
static int _get_len(char* szBuf, int iLenType, int* iLen)
{
	int iLenLen;
	char szTmpBuf[MAX_LEN_LEN+1];

	memset(szTmpBuf, 0, sizeof(szTmpBuf));

	switch(iLenType)
	{
		case LEN_NORMAL:
			*iLen = szBuf[0];
			iLenLen = 1;
			break;
		case LEN_STANDARD:
			if ((szBuf[0] & 0x80) == 0x80)
			{
				iLenLen = (szBuf[0] & 0x7F);
				
				if(iLenLen == 1)
				{
					*iLen = szBuf[1];
					iLenLen++;;
				}
				else if(iLenLen == 2)
				{
					*iLen = szBuf[1] * 256 + szBuf[2];
					iLenLen++;
				}
			}
			else
			{
				*iLen = szBuf[0];
				iLenLen = 1;
			}
			break;
		default:
			iLenLen = 0;
			break;
	}

	return iLenLen;	
}

/*****************************************************************
** 功    能：设置TLV中Value值
** 输入参数：
**		szValue		Value值
**		iLen		Value长度
**		iValueType	Value类型
**				0：普通类型，数据原样拷贝，不做处理
**				1：BCD码压缩(右对齐)，数据以BCD码压缩，长度为奇数时，左补0对齐
**				2：BCD码压缩(左对齐)，数据以BCD码压缩，长度为奇数时，右补0对齐
** 输出参数：
**		szValueBuf	格式化后Value值
** 返 回 值： 
**		>0		Len占用字节数
**		0		读取失败
** 作    者：
**		fengwei
** 日    期：
**		2012/09/24
** 调用说明：
**		仅供tlv库函数内部调用，外部不调用
** 修改日志：
****************************************************************/
static int _set_value(char* szValue, int iLen, int iValueType, char* szValueBuf)
{
	char szTmpBuf[MAX_VALUE_LEN+1];
	int iValueLen;

	memset(szTmpBuf, 0, sizeof(szTmpBuf));

	switch(iValueType)
	{
		case VALUE_NORMAL:
			memcpy(szTmpBuf, szValue, iLen);
			iValueLen = iLen;
			break;
		case VALUE_BCD_RIGHT:
			AscToBcd(szValue, iLen, 1, szTmpBuf);	
			if(iLen%2 == 0 )
			{
				iValueLen = iLen/2;
			}
			else
			{
				iValueLen = iLen/2 + 1;
			}
			break;
		case VALUE_BCD_LEFT:
			AscToBcd(szValue, iLen, 0, szTmpBuf);	
			if(iLen%2 == 0 )
			{
				iValueLen = iLen/2;
			}
			else
			{
				iValueLen = iLen/2 + 1;
			}
			break;
		default:
			iValueLen = -1;
			break;
	}

	if (iValueLen > 0 && iValueLen <= MAX_VALUE_LEN)
	{
		memcpy(szValueBuf, szTmpBuf, iValueLen);
	}

	return iValueLen;
}

/*****************************************************************
** 功    能：从TLV格式数据中获取Value值
** 输入参数：
**		szBuf		TLV格式数据
**		iLen		Value值占用字节数
**		iValueType	Value类型，说明参考_set_value函数参数说明
** 输出参数：
**		szValueBuf	存放Value值Buf
** 返 回 值： 
**		>0		Value长度
**		-1		读取失败
** 作    者：
**		fengwei
** 日    期：
**		2012/09/24
** 调用说明：
**		仅供tlv库函数内部调用，外部不调用
** 修改日志：
****************************************************************/
int _get_value(char* szBuf, int iLen, int iValueType, char* szValueBuf)
{
	int iValueLen;

	switch(iValueType)
	{
		case VALUE_NORMAL:
			memcpy(szValueBuf, szBuf, iLen);
			iValueLen = iLen;
			break;
		case VALUE_BCD_RIGHT:
			BcdToAsc(szBuf, iLen, 1, szValueBuf);	
			if(iLen%2 == 0)
			{
				iValueLen = iLen/2;
			}
			else
			{
				iValueLen = iLen/2 + 1;
			}
			break;
		case VALUE_BCD_LEFT:
			BcdToAsc(szBuf, iLen, 0, szValueBuf);	
			if(iLen%2 == 0)
			{
				iValueLen = iLen/2;
			}
			else
			{
				iValueLen = iLen/2 + 1;
			}
			break;
		default:
			iValueLen = -1;
			break;
	}

	return iValueLen;
}

/* ----------------------------------------------------------------
 * 功    能：TLV配置初始化，设置Tag、Len、Value格式类型
 * 输入参数：pTLV            TLV格式数据
 *           iTagType        Tag类型，说明参考_calc_tag_len函数参数说明
 *           iLenType        Len类型，说明参考_set_len函数参数说明
 *           iValueType      Value类型，说明参考_set_value函数参数说明
 * 输出参数：
 * 返 回 值：
 * 作    者：fengwei
 * 日    期：2012-9-24
 * 调用说明：InitTLV(pTLV, TAG_NORMAL, LEN_NORMAL, VALUE_NORMAL)
 * 修改日志：修改日期    修改者      修改内容简述
 *           2012-11-14  ryan.chan   调整格式、
 * ----------------------------------------------------------------
 */
void InitTLV(T_TLVStru *pTLV, int iTagType, int iLenType, int iValueType)
{
    int i;

    for(i=0; i<MAX_TLV_NUM; i++)
    {
        memset(&(pTLV->tTLVData[i]), 0, sizeof(T_TLVData));
    }

    pTLV->iTagType = iTagType;
    pTLV->iLenType = iLenType;
    pTLV->iValueType = iValueType;

    return;
}

/* ----------------------------------------------------------------
 * 功    能：添加TLV格式数据
 * 输入参数：pTLV            TLV格式数据
 *           szTag           标签
 *           iLen            数据长度
 *           szValueBuf      数据
 * 输出参数：
 * 返 回 值：0  成功；    -1 失败
 * 作    者：fengwei
 * 日    期：2012-9-24
 * 调用说明：SetTLV(pTLV, "\x01", 4, "1234")
 * 修改日志：修改日期    修改者      修改内容简述
 *           2012-11-14  ryan.chan   调整格式、
 * ----------------------------------------------------------------
 */
int SetTLV(T_TLVStru *pTLV, char *szTag, int iLen, char* szValue)
{
    int i, iTagLen;
    T_TLVData *pData;

    for (i = 0; i < MAX_TLV_NUM; i++)
    {
        pData = pTLV->tTLVData + i;

        if (pData->iFlag == DATA_NULL)
        {
            iTagLen = _calc_tag_len(szTag, pTLV->iTagType);
            if (iTagLen > 0)
            {
                memcpy(pData->szTag, szTag, iTagLen);
            }
            else
            {
                WriteLog(ERROR, "set tlv Tag error");
                return FAIL;
            }

            pData->iLen = iLen;

            if (iLen > 0)
            {
                memcpy(pData->szValue, szValue, 
                       iLen>MAX_VALUE_LEN?MAX_VALUE_LEN:iLen);
            }
            pData->iFlag = DATA_NOTNULL;

            return SUCC;
        }
    }

    return FAIL;
}

/* ----------------------------------------------------------------
 * 功    能：根据Tag值读取Value值
 * 输入参数：pTLV            TLV格式数据
 *           szTag           标签
 *           iBufSize        Buf大小
 * 输出参数：szValueBuf      保存Value值Buf
 * 返 回 值：>=0 Value长度 -1 失败
 * 作    者：fengwei
 * 日    期：2012-9-24
 * 调用说明：GetValueByTag(pTLV, "\x01", szValueBuf, sizeof(szValueBuf))
 * 修改日志：修改日期    修改者      修改内容简述
 *           2012-11-14  ryan.chan   调整格式、
 * ----------------------------------------------------------------
 */
int GetValueByTag(T_TLVStru *pTLV, char* szTag, char* szValueBuf, int iBufSize)
{
    int i;
    int iTagLen;
    int iValueLen;
    T_TLVData *pData;

    iTagLen = _calc_tag_len(szTag, pTLV->iTagType);
    if (iTagLen <= 0)
    {
        WriteLog(ERROR, "iTagType:[%d] def error", pTLV->iTagType);
        return FAIL;
    }

    for (i = 0; i < MAX_TLV_NUM; i++)
    {
        pData = pTLV->tTLVData + i;

        if (memcmp(pData->szTag, szTag, iTagLen) == 0 
           && pData->iFlag == DATA_NOTNULL)
        {
            iValueLen = pData->iLen > iBufSize ? iBufSize : pData->iLen;

            memcpy(szValueBuf, pData->szValue, iValueLen);

            return iValueLen;
        }
    }

    return FAIL;
}

/* ----------------------------------------------------------------
 * 功    能：根据索引值读取Value值
 * 输入参数：pTLV            TLV格式数据
 *           iIndex          索引值
 *           iBufSize        Buf大小
 * 输出参数：szValueBuf      保存Value值Buf
 * 返 回 值：0  成功；    -1 失败
 * 作    者：fengwei
 * 日    期：2012-9-24
 * 调用说明：GetValueByIdx(pTLV, 1, szValueBuf, sizeof(szValueBuf))
 * 修改日志：修改日期    修改者      修改内容简述
 *           2012-11-14  ryan.chan   调整格式、
 * ----------------------------------------------------------------
 */
int GetValueByIdx(T_TLVStru *pTLV, int iIndex, char* szValueBuf, 
                  int iBufSize)
{
    int iTagLen;
    T_TLVData *pData;

    pData = pTLV->tTLVData + iIndex;

    if (pData->iFlag == DATA_NOTNULL)
    {
        memcpy(szValueBuf, pData->szValue, 
               pData->iLen>iBufSize?iBufSize:pData->iLen);

        return SUCC;
    }

    return FAIL;	
}

/* ----------------------------------------------------------------
 * 功    能：将TLV格式数据打包成字符串
 * 输入参数：pTLV            TLV格式数据
 * 输出参数：szBuf           输出字符串
 * 返 回 值：>=0 打包字符串长度  -1 失败
 * 作    者：fengwei
 * 日    期：2012-9-24
 * 调用说明：PackTLV(pTLV, szBuf)
 * 修改日志：修改日期    修改者      修改内容简述
 *           2012-11-14  ryan.chan   调整格式、
 * ----------------------------------------------------------------
 */
int PackTLV(T_TLVStru *pTLV, char* szBuf)
{
    int i;
    int iIndex, iLen;
    T_TLVData *pData;

    iIndex = 0;

    for (i=0;i<MAX_TLV_NUM;i++)
    {
        pData = pTLV->tTLVData + i;

        if (pData->iFlag == DATA_NOTNULL)
        {
            iLen = _set_tag(pData->szTag, pTLV->iTagType, szBuf+iIndex);
            if (iLen <= 0)
            {
                WriteLog(ERROR, "pack tlv Tag error");
                return FAIL;
            }
            iIndex += iLen;

            iLen = _set_len(pData->iLen, pTLV->iLenType, szBuf+iIndex);
            if (iLen <= 0)
            {
                WriteLog(ERROR, "pack tlv len error");
                return FAIL;
            }
            iIndex += iLen;

            iLen = _set_value(pData->szValue, pData->iLen, 
                              pTLV->iValueType, szBuf+iIndex);
            if (iLen < 0)
            {
                WriteLog(ERROR, "pack tlv value error");
                return FAIL;
            }
            iIndex += iLen;
        }
    }

    return iIndex;
}


/* ----------------------------------------------------------------
 * 功    能：将字符串解包为TLV格式数据
 * 输入参数：szBuf           输入字符串
 *           iBufLen         输入字符串长度
 * 输出参数：pTLV            TLV格式数据
 * 返 回 值：0  成功；    -1 失败
 * 作    者：fengwei
 * 日    期：2012-9-24
 * 调用说明：UnpackTLV(pTLV, szBuf, iLen)
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int UnpackTLV(T_TLVStru *pTLV, char* szBuf, int iBufLen)
{
    int  i, iIndex, iLen;
    char szTagBuf[MAX_TAG_LEN+1];
    T_TLVData *pData;

    iIndex = 0;
    i = 0;

    while (iIndex < iBufLen)
    {
        pData = pTLV->tTLVData + i;
        
        iLen = _get_tag(szBuf+iIndex, pTLV->iTagType, pData->szTag);
        if (iLen == 0)
        {
            WriteLog(ERROR, "unpack tlv Tag error");
            return FAIL;
        }
        iIndex += iLen;

        iLen = _get_len(szBuf+iIndex, pTLV->iLenType, &(pData->iLen));
        if (iLen == 0)
        {
            WriteLog(ERROR, "unpack tlv len error");
            return FAIL;
        }
        iIndex += iLen;

        iLen = _get_value(szBuf+iIndex, pData->iLen, pTLV->iValueType, 
                          pData->szValue);
        if (iLen < 0)
        {
            WriteLog(ERROR, "unpack tlv value error");
            return FAIL;
        }
        iIndex += iLen;

        pData->iFlag = DATA_NOTNULL;

        i++;
    }

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：TLV库Debug函数
 * 输入参数：pTLV      TLV格式数据
 * 输出参数：无
 * 返 回 值：无
 * 作    者：fengwei
 * 日    期：2012-9-24
 * 调用说明：DebugTLV(pTLV)
 * 修改日志：修改日期    修改者      修改内容简述
 *
 * ----------------------------------------------------------------
 */
void DebugTLV(T_TLVStru *pTLV)
{
    int i,j;
    int iTagLen;
    T_TLVData *pData;
    char szTagBuf[64+1];
    char szValueBuf[MAX_VALUE_LEN*2+1];

    if (pTLV == NULL)
    {
        return;
    }

    for (i=0; i<MAX_TLV_NUM; i++)
    {
        pData = pTLV->tTLVData + i;

        memset(szTagBuf, 0, sizeof(szTagBuf));
        memset(szValueBuf, 0, sizeof(szValueBuf));

        if (pData->iFlag == DATA_NOTNULL)
        {
            
            iTagLen = _calc_tag_len(pData->szTag, pTLV->iTagType);

            for (j=0;j<iTagLen;j++)
            {
                sprintf(szTagBuf+j*4, "0x%02x", pData->szTag[j]);
            }

            BcdToAsc(pData->szValue, pData->iLen*2, LEFT_ALIGN, szValueBuf);

            WriteLog(TRACE, "TLV:[%d] Tag:[%s] Len:[%d] Value:[%s]", 
                            i, szTagBuf, pData->iLen, szValueBuf);
        } /*if*/
    } /*for*/

    return;
}

