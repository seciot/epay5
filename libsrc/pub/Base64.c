
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：base64编码及解码接口
 *  创 建 人：ChenLei
 *  创建日期：2010.10.14 10:58:10
 * ----------------------------------------------------------------
 * $Revision: 1.5 $
 * $Log: Base64.c,v $
 * Revision 1.5  2012/12/11 03:11:08  fengw
 *
 * 1、删除指针类型转换，消除编译警告。
 *
 * Revision 1.4  2012/12/04 06:18:55  chenjr
 * 代码规范化
 *
 * Revision 1.3  2012/11/27 03:20:01  linqil
 * *** empty log message ***
 *
 * Revision 1.2  2012/11/27 02:44:03  linqil
 * 增加pub.h 修改return
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "pub.h"

const char BASE64_ENCODE_TABLE[64] =
{
     65,  66,  67,  68,  69,  70,  71,  72,  /* 00 - 07 */
     73,  74,  75,  76,  77,  78,  79,  80,  /* 08 - 15 */
     81,  82,  83,  84,  85,  86,  87,  88,  /* 16 - 23 */
     89,  90,  97,  98,  99, 100, 101, 102,  /* 24 - 31 */
    103, 104, 105, 106, 107, 108, 109, 110,  /* 32 - 39 */
    111, 112, 113, 114, 115, 116, 117, 118,  /* 40 - 47 */
    119, 120, 121, 122,  48,  49,  50,  51,  /* 48 - 55 */
     52,  53,  54,  55,  56,  57,  43,  47   /* 56 - 63 */
};

const unsigned char BASE64_DECODE_TABLE[256] =
{
    255, 255, 255, 255, 255, 255, 255, 255, /*  00 -  07 */
    255, 255, 255, 255, 255, 255, 255, 255, /*  08 -  15 */
    255, 255, 255, 255, 255, 255, 255, 255, /*  16 -  23 */
    255, 255, 255, 255, 255, 255, 255, 255, /*  24 -  31 */
    255, 255, 255, 255, 255, 255, 255, 255, /*  32 -  39 */
    255, 255, 255,  62, 255, 255, 255,  63, /*  40 -  47 */
     52,  53,  54,  55,  56,  57,  58,  59, /*  48 -  55 */
     60,  61, 255, 255, 255, 255, 255, 255, /*  56 -  63 */
    255,   0,   1,   2,   3,   4,   5,   6, /*  64 -  71 */
      7,   8,   9,  10,  11,  12,  13,  14, /*  72 -  79 */
     15,  16,  17,  18,  19,  20,  21,  22, /*  80 -  87 */
     23,  24,  25, 255, 255, 255, 255, 255, /*  88 -  95 */
    255,  26,  27,  28,  29,  30,  31,  32, /*  96 - 103 */
     33,  34,  35,  36,  37,  38,  39,  40, /* 104 - 111 */
     41,  42,  43,  44,  45,  46,  47,  48, /* 112 - 119 */
     49,  50,  51, 255, 255, 255, 255, 255, /* 120 - 127 */
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255 
};

const int m_LineWidth=76;

/* ----------------------------------------------------------------
 * 功    能：base64编码接口
 * 输入参数：szSrc    待编码信息体
 *           nSize    信息体长度
 * 输出参数：szDest   BASE64编码后的码串
 * 返 回 值：码串长度
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int base64_encode(char *szSrc, unsigned int nSize, char *szDest)
{
    char         *pInPtr, *pInLimitPtr;
    char         *OutPtr ;
    unsigned int iB, iInMax3, Len;
    
    if ((szSrc == NULL) || (nSize <= 0))
    {
        return SUCC;
    }
    
    pInPtr  = szSrc;
    iInMax3 = nSize / 3 * 3;
    OutPtr  = szDest;
    pInLimitPtr = pInPtr + iInMax3;
    				
    while (pInPtr != pInLimitPtr)
    {
        Len = 0;
        while ((Len < m_LineWidth) && (pInPtr != pInLimitPtr))
        {
            iB = (unsigned char) *pInPtr++;
            iB = iB << 8;
            				
            iB = iB | (unsigned char) *pInPtr++;
            iB = iB << 8;
            						
            iB = iB | (unsigned char) *pInPtr++;
            						
            /* 以4 byte倒序写入输出缓冲 */
            OutPtr[3] = BASE64_ENCODE_TABLE[iB & 0x3F];
            iB = iB >> 6;
            OutPtr[2] = BASE64_ENCODE_TABLE[iB & 0x3F];
            iB = iB >> 6;
            OutPtr[1] = BASE64_ENCODE_TABLE[iB & 0x3F];
            iB = iB >> 6;
            OutPtr[0] = BASE64_ENCODE_TABLE[iB];
            OutPtr+=4;
            Len+=4;
        }
        
        if (Len >= m_LineWidth)
        {
            *OutPtr++ = '\r'; /* 加上回车换行符 */
            *OutPtr++ = '\n';
        }
    }
    
    /* 设置尾部 */
    switch (nSize - iInMax3)
    {
    case 1:
        iB = (unsigned char) *pInPtr;
        iB = iB << 4;
        OutPtr[1] = BASE64_ENCODE_TABLE[iB & 0x3F];
        iB = iB >> 6;
        OutPtr[0] = BASE64_ENCODE_TABLE[iB];
        OutPtr[2] = '=';  /* 用'='也就是64码填充剩余部分 */
        OutPtr[3] = '=';
        OutPtr+=4;
        break;
    
    case 2:
        iB = (unsigned char) *pInPtr++;
        iB = iB << 8;
        iB = iB | (unsigned char) *pInPtr;
        iB = iB << 2;
        OutPtr[2] = BASE64_ENCODE_TABLE[iB & 0x3F];
        iB = iB >> 6;
        OutPtr[1] = BASE64_ENCODE_TABLE[iB & 0x3F];
        iB = iB >> 6;
        OutPtr[0] = BASE64_ENCODE_TABLE[iB];
        OutPtr[3] = '='; 	/* Fill remaining byte. */
        OutPtr+=4;
        break;
    }
    
    return (unsigned int) (OutPtr - szDest);
}
			
/* ----------------------------------------------------------------
 * 功    能：base64解码接口
 * 输入参数：szSrc    待解码信息体
 *           nSize    信息体长度
 * 输出参数：szDest   BASE64解码后的串
 * 返 回 值：解码后串的长度
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int base64_decode(char *szSrc, unsigned int nSize, char *szDest)
{
    unsigned int 	lByteBuffer;
    unsigned int 	lByteBufferSpace;
    unsigned int 	C; 			/* 临时阅读变量 */
    int 	        reallen;
    char 		*InPtr, *InLimitPtr;
    char 		*OutPtr;
    				
    if ((szSrc == NULL) || (szDest == NULL) || (nSize <= 0))
    {
        return SUCC;
    }
    
    InPtr = szSrc;
    InLimitPtr= InPtr + nSize;
    OutPtr = szDest;
    
    lByteBuffer = 0;
    lByteBufferSpace = 4;
    				
    while (InPtr != InLimitPtr)
    {
        C = BASE64_DECODE_TABLE[*InPtr]; /* Read from InputBuffer. */
        InPtr++;
    
        if (C == 0xFF)
        {
            continue; 		 /* 读到255非法字符 */
        }
    
        lByteBuffer = lByteBuffer << 6 ;
        lByteBuffer = lByteBuffer | C ;
        lByteBufferSpace--;
        
        if (lByteBufferSpace != 0)
        {
            continue; 		/* 一次读入4个字节 */
        }
        
        /* 到序写入3个字节到缓冲 */
        OutPtr[2] = lByteBuffer;
        lByteBuffer = lByteBuffer >> 8;
        
        OutPtr[1] = lByteBuffer;
        lByteBuffer = lByteBuffer >> 8;
        
        OutPtr[0] = lByteBuffer;
        
        /* 准备写入后3位 */
        OutPtr+= 3;
        
        lByteBuffer = 0;
        lByteBufferSpace = 4;
    }

    reallen = OutPtr - szDest;

    /* 处理尾部 返回实际长度 */
    switch (lByteBufferSpace)
    {
    case 1:
        lByteBuffer = lByteBuffer >> 2;
        OutPtr[1] = lByteBuffer;
        lByteBuffer = lByteBuffer >> 8;
        OutPtr[0] = lByteBuffer;
        return reallen + 2;
    case 2:
        lByteBuffer = lByteBuffer >> 4;
        OutPtr[0] = lByteBuffer;
        return reallen + 1;
    
    default:
        return reallen;
    }

}
