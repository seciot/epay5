
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：ASCII码与BCD码间互相转换接口
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.4 $
 * $Log: AscBcd.c,v $
 * Revision 1.4  2013/01/04 06:11:39  fengw
 *
 * 1、修正BcdToAsc函数字符串结束符赋值越界BUG。
 *
 * Revision 1.3  2012/12/14 08:06:16  chenrb
 * BcdToAsc输出的asc串增加结束符
 *
 * Revision 1.2  2012/11/27 02:43:00  linqil
 * 增加引用pub.h 修改return
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */


#include <string.h>
#include "pub.h"
#define  CONVSEAT_EVEN_FLAG     0x55     /* 转换位置处于偶数标识 */
#define  ODD_CHKFLAG            0x01     /* 奇数校验标识　*/


/* ----------------------------------------------------------------
 * 功    能：把ASC码转换成BCD码; 如果被转换的ASC串长度为奇数，接口支持
 *           在转换后的BCD串最前或最后的4bit补0
 * 输入参数：uszAscBuf  即将被转换的ASCII串
 *           iAscLen    ASCII串长度
 *           ucType     转换方式;只在转换长度iAscLen为奇数时生效.
 *                      0,  转换出的BCD串最后4bit补0
 *                      非0,转换出的BCD串最前4bit补0
 * 输出参数：uszBcdBuf  转换出的BCD串
 * 返 回 值：-1   输入参数异常
 *           >0   转换后uszBcdBuf的长度
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */ 
int AscToBcd(unsigned char  *uszAscBuf, int iAscLen,  
             unsigned char   ucType,    unsigned char *uszBcdBuf)
{
    int i;
    unsigned char ucLVal, ucRVal;

    /* 参数合法性检查 */
    if (uszAscBuf == NULL ||
        iAscLen   <= 0    ||
        uszBcdBuf == NULL)
    {
        return FAIL;
    }
        
    /* 填充方式判断 */
    if (iAscLen & ODD_CHKFLAG && ucType)
    {
        /* 转换长度为奇数字节长且转换类型非零，
           表示在串前填充4bit的0,  */
        ucLVal = 0;
    }
    else
    {
        ucLVal = CONVSEAT_EVEN_FLAG;
    }

    /* 逐字节 ASC->BCD转换 */
    for (i = 0; i < iAscLen; i++, uszAscBuf++)
    {
        if (*uszAscBuf >= 'a')
        {
            ucRVal = *uszAscBuf - 'a' + 10;
        }
        else if (*uszAscBuf >= 'A')
        {
            ucRVal = *uszAscBuf - 'A' + 10;
        }
        else if (*uszAscBuf >= '0')
        {
            ucRVal = *uszAscBuf - '0';
        }
        else
        {
            ucRVal = 0;
        }

        if (ucLVal == CONVSEAT_EVEN_FLAG)
        {
            ucLVal = ucRVal;
        }
        else
        {
            *uszBcdBuf++ = (ucLVal << 4) | ucRVal;
            ucLVal = CONVSEAT_EVEN_FLAG;
        }
    }

    /* 全部转换完毕, 判断转换长度是否为奇数 */
    if (ucLVal != CONVSEAT_EVEN_FLAG)
    {
        /* 为奇数, 在串后补4bit的0 */
        *uszBcdBuf = ucLVal << 4;
    }

    return (iAscLen + 1) / 2;
}



/* ----------------------------------------------------------------
 * 功    能：把BCD码转换成ASC码; 如果需要转换出奇数字节的ASC串，
 *           接口支持跳过BCD串最前或最后的4bit
 * 输入参数：uszBcdBuf  即将被转换的BCD串
 *           iAscLen    需要转换出的ASC串长度
 *           ucType     转换方式;只在转换长度iAscLen为奇数时生效.
 *                      0,  从头开始转换
 *                      非0,跳过BCD串最前4bit后再转换
 * 输出参数：uszAscBuf  转换出的Asc串
 * 返 回 值：-1   输入参数异常
 *           >0   转换后uszAscBuf的长度
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */ 
int BcdToAsc(unsigned char  *uszBcdBuf, int iAscLen,
             unsigned char   ucType,    unsigned char *uszAscBuf)
{
    int i, iConvLen;

    /* 参数合法性检查 */
    if (uszAscBuf == NULL ||
        iAscLen   <= 0    ||
        uszBcdBuf == NULL)
    {
        return FAIL;
    }

    /* 转换起始位置判断 */
    iConvLen = iAscLen;
    if (iAscLen & ODD_CHKFLAG && ucType)
    {
        /* 转出长度为奇数字节长且转换类型非零，
           表示跳过串前4bit  */
        i = 1;
        iConvLen++;
    }
    else
    {
        i = 0;
    }

    for (; i < iConvLen; i++, uszAscBuf++)
    {
        *uszAscBuf = ( (i & 0x01) ? (*uszBcdBuf++ & 0x0f) : 
                                    (*uszBcdBuf >> 4) );
        *uszAscBuf += ( (*uszAscBuf > 9) ? ('A' - 10) : '0' );
    }
    uszAscBuf[0] = 0;

    return iAscLen;

}



