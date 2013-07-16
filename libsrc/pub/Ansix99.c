/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：ansix99
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.10 $
 * $Log: Ansix99.c,v $
 * Revision 1.10  2012/12/10 02:57:33  fengw
 *
 * 1、替换WriteERLog函数为WriteLog。
 *
 * Revision 1.9  2012/12/04 03:33:18  chenjr
 * 代码规范化
 *
 * Revision 1.8  2012/11/29 02:15:35  linqil
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.7  2012/11/28 08:25:44  linqil
 * 去掉冗余头文件user.h
 *
 * Revision 1.6  2012/11/27 08:22:52  linqil
 * 修改日志函数
 *
 * Revision 1.5  2012/11/27 05:54:09  linqil
 * 去掉void 的返回值
 *
 * Revision 1.4  2012/11/27 05:46:48  linqil
 * 增加引用pub.h 修改return
 *
 * Revision 1.3  2012/11/27 02:41:45  linqil
 * 增加引用pub.h 修改return
 *
 * Revision 1.2  2012/11/26 09:06:24  linqil
 * 修改AscToBCD为AscToBcd
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libpub.h"
#include "pub.h"

#define SINGLE_DES   1     /* DES */
#define TRIPLE_DES   2     /* 3DES */

/* ----------------------------------------------------------------
 * 功    能：ANSI X9.9    计算MAc
 *           DES ( DES ( A ) ^ ( A + 8 ) ... ) )
 *           TriDES ( TriDES ( A ) ^ ( A + 8 ) ... ) )
 * 输入参数：uuszMacKey     计算MAC的密钥
 *           uszBuf        用于计算MAc的报文
 *           iLen         报文长度
 *           iAlg         算法标识: 采用SINGLE_DES or TRIPLE_DES
 * 输出参数：uszMac        计算结果MAc值(64bit)
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void ANSIX99(unsigned char *uuszMacKey, unsigned char *uszBuf, int iLen, 
             int iAlg, unsigned char *uszMac)
{
    int   i, j;
    char  tmp[20];
    
    if (iLen <= 0)
    {
        WriteLog(ERROR, "Invalid argument[iLen=%d]\n", iLen);
        return ;
    }
    
    memset(uszMac, '\0', 8);
    
    for (i = 0; i < iLen; i += 8)
    {
        /* right-justified with append 0x00 */
        if ((iLen - i) < 8)
        {
            memset( tmp, '\0', 8 );
            memcpy( tmp, uszBuf + i, iLen - i );
            for (j = 0; j < 8; j ++) 
            {
                uszMac[j] ^= tmp[j];
            }
        } 
        else
        {
            for ( j = 0; j < 8; j ++ ) 
            {
                uszMac [ j ] ^= uszBuf [ i + j ];
            }
        }
        
        if (iAlg == TRIPLE_DES)
        {
            TriDES(uuszMacKey, uszMac, uszMac);
        }
        else
        {
            DES(uuszMacKey, uszMac, uszMac);
        }
    }
    
    return ;
}


/* ----------------------------------------------------------------
 * 功    能：ANSI X9.19    计算MAc
 *  TMP1 = DES ( DES (A, KeyL ) ^ ( A + 8 ) ... ), KeyL )
 *  TMP2 = _DES( TMP1, KeyR )
 *  MAC  = DES( TMP2, KeyL )
 * 输入参数：uszMacKey      计算MAC的密钥
 *           uszBuf         用于计算MAc的报文
 *           iLen          报文长度
 * 输出参数：uszMac         计算结果MAc值(64bit)
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void ANSIX919(unsigned char *uszMacKey, unsigned char *uszBuf, int iLen, 
              unsigned char *uszMac)
{
    int   i, j;
    char  tmp[20];

    if (iLen <= 0)
    {
        WriteLog(ERROR, "Invalid argument[iLen=%d]", iLen);
        return ;
    }

    memset(uszMac, '\0', 8);
    
    for (i = 0; i < iLen; i += 8) 
    {
        /* right-justified with append 0x00 */
        if ((iLen - i) < 8) 
        {
            memset(tmp, '\0', 8);
            memcpy(tmp, uszBuf + i, iLen - i);
            for (j = 0; j < 8; j ++) 
            {
                uszMac[j] ^= tmp[j];
            }
        }
        else 
        {
            for (j = 0; j < 8; j ++) 
            {
                uszMac[j] ^= uszBuf[i+j];
            }
        }
        
        DES(uszMacKey, uszMac, uszMac);
    }
    
    _DES(uszMacKey+8, uszMac, uszMac);
    DES(uszMacKey, uszMac, uszMac);
    
    return ;
}


/* ----------------------------------------------------------------
 * 功    能：简单MAc算法 DES ( A ^ ( A + 8 ) ... )
 * 输入参数：uszMacKey    计算MAC的密钥
 *           uszBuf       用于计算MAc的报文
 *           iLen         报文长度
 *           iAlg         算法标识 TRIPLE_DES or SINGLE_DES
 * 输出参数：uszMac       计算结果MAc值(64bit)
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void Mac_Normal(unsigned char *uszMacKey, unsigned char *uszBuf, 
               int iLen, int iAlg, unsigned char *uszMac)
{
    int   i, j;
    char  tmp[20];

    if (iLen <= 0)
    {
        WriteLog(ERROR, "Invalid argument[iLen=%d]", iLen);
        return ;
    }

    memset(uszMac, '\0', 8);
    
    for (i = 0; i < iLen; i += 8) 
    {
        /* right-justified with append 0x00 */
        if ((iLen - i) < 8) 
        {
            memset ( tmp, '\0', 8 );
            memcpy ( tmp, uszBuf + i, iLen - i );
            for ( j = 0; j < 8; j ++ ) 
            {
                uszMac [ j ] ^= tmp [ j ];
            }
        } 
        else 
        {
            for (j = 0; j < 8; j ++) 
            {
                uszMac[j] ^= uszBuf[i + j];
            }
        }
    }
    
    if (iAlg == TRIPLE_DES)
    {
        TriDES(uszMacKey, uszMac, uszMac);
    }
    else
    {
        DES(uszMacKey, uszMac, uszMac);
    }

    return ;
}


/* ----------------------------------------------------------------
 * 功    能：将输入字符串按每8个字节分组，最后不足8位补数字0，
 *           然后从第1组开始逐组异或;( A ^ ( A + 8 ) ^ ( A + 16 ) ... )
 * 输入参数：uszInData     输入数据
 *           nLen          输入数据长度
 * 输出参数：uszOutData    异或后的数据(8 byte)
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void XOR(unsigned char *uszInData, int iLen, unsigned char *uszOutData )
{
    int   i, j;
    char  tmp[20];
 
    memset( uszOutData, 0, 8 );
    
    for (i = 0; i < iLen; i += 8) 
    {
        /* 不足8位右补 0x00 */
        if ( ( iLen - i ) < 8 ) 
        {
            memset(tmp, '\0', 8);
            memcpy(tmp, uszInData+i, iLen-i);
            for (j = 0; j < 8; j ++) 
            {
                uszOutData[j] ^= tmp[j];
            }
        }
        else
        {
            for (j = 0; j < 8; j ++) 
            {
                uszOutData[j] ^= uszInData[i+j];
            }
        }
    }
 
    return ;
}

