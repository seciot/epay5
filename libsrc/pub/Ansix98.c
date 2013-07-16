
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：ansix98
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.8 $
 * $Log: Ansix98.c,v $
 * Revision 1.8  2012/12/04 03:20:37  chenjr
 * 代码规范化
 *
 * Revision 1.7  2012/11/27 07:00:43  linqil
 * 调整条件判断语句
 *
 * Revision 1.6  2012/11/26 09:09:45  linqil
 * 增加注释
 *
 * Revision 1.5  2012/11/26 09:08:08  linqil
 * 修改BCDToAsc为BcdToAsc
 *
 * Revision 1.4  2012/11/26 09:06:24  linqil
 * 修改AscToBCD为AscToBcd
 *
 * Revision 1.3  2012/11/26 08:40:13  linqil
 * 增加对pub.h的引用，0 -1 替换为SUCC、FAIL
 *
 * Revision 1.2  2012/11/20 03:27:37  chenjr
 * init
 *
 * Revision 1.1  2012/11/20 03:25:44  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "pub.h"


#define SINGLE_DES   1     /* DES */
#define TRIPLE_DES   2     /* 3DES */

/* ----------------------------------------------------------------
 * 功    能：ANSI X9.8    加密模块
 * 输入参数：uszKey       加密密钥明文
 *           szPan        账号
 *           szPasswd     密码明文
 *           iPwdLen      密码明文长度
 *           iFlag        算法标识: 采用DES或3DES
 * 输出参数：uszResult    采用ansi98加密后的密码密文
 * 返 回 值：-1  加密失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ANSIX98(unsigned char *uszKey, char *szPan,  char *szPwd,
            int iPwdLen, int iFlag, unsigned char *uszResult)
{
    int	i;
    int iRet;
    unsigned char  password[9];
    
    if (iPwdLen > 8)
    {
        return FAIL;
    }
    
    iRet = A_(szPwd, szPan, iPwdLen, uszResult);
    if (iRet < 0 )	
    {
        return FAIL;
    }
    
    if (iFlag == TRIPLE_DES)
    {
        TriDES( uszKey, uszResult, password );
    }
    else
    {
        DES(uszKey, uszResult, password);
    }
    
    memcpy((char*) uszResult, (char*) password, 8);
    uszResult[8] = '\0';
    
    return SUCC;
}

/* ----------------------------------------------------------------
 * 功    能：ANSI X9.8    解密模块
 * 输入参数：uszKey       解密密钥明文
 *           szPan        账号
 *           uszPasswd     密码密文
 *           iFlag        算法标识: 采用DES或3DES
 * 输出参数：uszResult    采用ansi98解密后的密码明文
 * 返 回 值：-1  解密失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int _ANSIX98(unsigned char *uszKey, char *szPan, unsigned char *uszPwd,
             int iFlag, unsigned char *uszResult)
{
    unsigned char  a_value [ 17 ];
    
    if (iFlag == TRIPLE_DES)
    {
        _TriDES(uszKey, uszPwd, a_value);
    }
    else
    {
        _DES(uszKey, uszPwd, a_value);
    }
    
    return(_A_(a_value, szPan, uszResult));
}

/* ----------------------------------------------------------------
 * 功    能：密码明文与账号异或处理
 * 输入参数：szPan        账号
 *           uszPasswd    密码明文
 *           iPwdLen      密码长度
 * 输出参数：uszResult    异或后结果
 * 返 回 值：SUCC  成功   FAIL  失败
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int A_(unsigned char *uszPasswd, char *szPan, int iPwdLen, unsigned char *uszResult)
{
    unsigned char  uszPasswd0[17], tmp[17], tmp1[10];
    int            i;
    	
    memcpy (uszPasswd0, uszPasswd, iPwdLen);
    
    for (i = iPwdLen; i < 16; i++)  
    {
        uszPasswd0 [ i ] = 'F'; 
    }
    
    AscToBcd(uszPasswd0, 16, 0, tmp1);
    memcpy (tmp, szPan + 3, 13);
    
    for (i = 0; i < 13; i++) 
    {
        if (tmp[i] < '0' || tmp[i] > '9')	
        {
            tmp [ i ] = 'F';
        }
    }

    AscToBcd(tmp, 12, 0, uszPasswd0);
    	
    sprintf((char *)tmp, "0%d", iPwdLen);
    AscToBcd(tmp, 2, 0, uszResult);
    
    * (++uszResult) = tmp1[0];
    
    uszResult ++;
    
    for (i = 0; i <= 5; i++) 
    {
        *(uszResult + i) = tmp1[i + 1] ^ uszPasswd0[i];
    }
    
    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：密码明文还原
 * 输入参数：uszValuse    被账号异或后的密码
 *           szPan        账号
 * 输出参数：uszResult    密码明文
 * 返 回 值：SUCC  成功   FAIL  失败
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int _A_(unsigned char *uszValue, char *szPan, unsigned char *uszResult)
{
    unsigned char  tmp[17], tmp1[17], passwd0[17];
    int            i, len;
    
    memcpy(tmp, szPan + 3, 13);
    
    for (i = 0; i < 13; i++) 
    {
        if (tmp[i] < '0' || tmp[i] > '9')	
        {
            tmp[i] = 'F';
        }
    }
    
    AscToBcd(tmp, 12, 0, tmp1);
    
    tmp[0] = uszValue[0];
    tmp[1] = uszValue[1];
    
    for (i = 0; i < 6; i++) 
    {
        tmp[i+2] = uszValue[2+i]^tmp1[i];
    }
    
    BcdToAsc(tmp, 16, 0, passwd0);
    
    memcpy((char *) tmp1, (char *)passwd0, 2);
    tmp1[2] = '\0';
    
    len = atoi (tmp1);
    
    if (len > 8)
    {
        return FAIL;
    }
    
    memcpy((char *)uszResult, (char *)passwd0 + 2, len);
    uszResult[len] = '\0';
    
    return SUCC;
}

