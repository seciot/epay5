
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：32位的SINGLE_DES与TRIPLE_DES接口
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.3 $
 * $Log: qdes32.c,v $
 * Revision 1.3  2012/12/04 07:01:44  chenjr
 * 代码规范化
 *
 * Revision 1.2  2012/11/27 05:46:49  linqil
 * 增加引用pub.h 修改return
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#include <string.h>
#include "qdes32.h"
#include "pub.h"

/* ----------------------------------------------------------------
 * 功    能：TRIPLE_DES加密
 * 输入参数：uszKey     加密密钥
 *           uszSrc     加密前数据
 * 输出参数：uszDest    加密后数据
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void TriDES(unsigned char *uszKey, unsigned char *uszSrc, 
            unsigned char *uszDest)
{
    char  szTmpStr[9], szBuf[9];

    DES(uszKey, uszSrc, szTmpStr);
    _DES(uszKey+8, szTmpStr, szBuf);
    DES(uszKey, szBuf, uszDest);
    return;
}

/* ----------------------------------------------------------------
 * 功    能：TRIPLE_DES解密
 * 输入参数：uszKey     解密密钥
 *           uszSrc     解密前数据
 * 输出参数：uszDest    解密后数据
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void _TriDES(unsigned char *uszKey, unsigned char *uszSrc, 
             unsigned char *uszDest)
{
    char   szTmpStr[9], szBuf[9];

    _DES( uszKey, uszSrc, szTmpStr );
    DES( uszKey+8, szTmpStr, szBuf );
    _DES( uszKey, szBuf, uszDest );
    return;
}

/* ----------------------------------------------------------------
 * 功    能：SINGLE_DES加密
 * 输入参数：uszKey    加密密钥
 *           uszSrc    加密前数据
 * 输出参数：uszDest   加密后数据
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void DES(unsigned char *uszKey, unsigned char *uszSrc,
         unsigned char *uszDest)
{
    des_key_schedule sch;
    des_set_key((des_cblock *)uszKey,sch);
    des_ecb_encrypt((des_cblock *)uszSrc,(des_cblock *)uszDest,&(sch[0]),1);
}

/* ----------------------------------------------------------------
 * 功    能：SINGLE_DES解密
 * 输入参数：uszKey    解密密钥
 *           uszSrc    解密前数据
 * 输出参数：uszDest   解密后数据
 * 返 回 值：
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void _DES(unsigned char *uszKey, unsigned char *uszSrc,
          unsigned char *uszDest)
{
    des_key_schedule sch;
    des_set_key((des_cblock *)uszKey,sch);
    des_ecb_encrypt((des_cblock *)uszSrc,(des_cblock *)uszDest,&(sch[0]),0);
}




int des_set_key(key,schedule)
des_cblock *key;
des_key_schedule schedule;
{
	register DES_LONG c,d,t,s;
	register unsigned char *in;
	register DES_LONG *k;
	register int i;

	k=(DES_LONG *)schedule;
	in=(unsigned char *)key;

	c2l(in,c);
	c2l(in,d);

	PERM_OP (d,c,t,4,0x0f0f0f0fL);
	HPERM_OP(c,t,-2,0xcccc0000L);
	HPERM_OP(d,t,-2,0xcccc0000L);
	PERM_OP (d,c,t,1,0x55555555L);
	PERM_OP (c,d,t,8,0x00ff00ffL);
	PERM_OP (d,c,t,1,0x55555555L);
	d=	(((d&0x000000ffL)<<16L)| (d&0x0000ff00L)     |
		 ((d&0x00ff0000L)>>16L)|((c&0xf0000000L)>>4L));
	c&=0x0fffffffL;

	for (i=0; i<16; i++)
		{
		if (shifts2[i])
			{ c=((c>>2L)|(c<<26L)); d=((d>>2L)|(d<<26L)); }
		else
			{ c=((c>>1L)|(c<<27L)); d=((d>>1L)|(d<<27L)); }
		c&=0x0fffffffL;
		d&=0x0fffffffL;

		s=	des_skb[0][ (c    )&0x3f                ]|
			des_skb[1][((c>> 6)&0x03)|((c>> 7L)&0x3c)]|
			des_skb[2][((c>>13)&0x0f)|((c>>14L)&0x30)]|
			des_skb[3][((c>>20)&0x01)|((c>>21L)&0x06) |
						  ((c>>22L)&0x38)];
		t=	des_skb[4][ (d    )&0x3f                ]|
			des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|
			des_skb[6][ (d>>15L)&0x3f                ]|
			des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];

		/* table contained 0213 4657 */
		*(k++)=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;
		s=     ((s>>16L)|(t&0xffff0000L));
		
		s=(s<<4L)|(s>>28L);
		*(k++)=s&0xffffffffL;
		}
	return SUCC;
}

void des_ecb_encrypt(input, output, ks,encrypt)
des_cblock *input,*output;
des_key_schedule ks;
int encrypt;
{
	register DES_LONG l;
	register unsigned char *in,*out;
	DES_LONG ll[2];

	in=(unsigned char *)input;
	out=(unsigned char *)output;
	c2l(in,l); ll[0]=l;
	c2l(in,l); ll[1]=l;
	des_encrypt(ll,ks,encrypt);
	l=ll[0]; l2c(l,out);
	l=ll[1]; l2c(l,out);
	l=ll[0]=ll[1]=0;
}

void des_encrypt(data, ks, encrypt)
DES_LONG *data;
des_key_schedule ks;
int encrypt;
{
	register DES_LONG l,r,u;

/* for XENIX delay */

	char delay_str [2];

	union fudge {
		DES_LONG  l;
		unsigned short s[2];
		unsigned char  c[4];
		} U,T;
	register int i;
	register DES_LONG *s;

	u=data[0];
	r=data[1];

	IP(u,r);

	l=(r<<1)|(r>>31);
	r=(u<<1)|(u>>31);

	/* clear the top bits on machines with 8byte longs */
	l&=0xffffffffL;
	r&=0xffffffffL;

	s=(DES_LONG *)ks;
	if (encrypt)
		{
		for (i=0; i<32; i+=4)
			{
			D_ENCRYPT(l,r,i+0); /*  1 */
/* for XENIX delay */
			strcpy(delay_str,"");
			D_ENCRYPT(r,l,i+2); /*  2 */
			}
		}
	else
		{
		for (i=30; i>0; i-=4)
			{
			D_ENCRYPT(l,r,i-0); /* 16 */
/* for XENIX delay */
			strcpy(delay_str,"");
			D_ENCRYPT(r,l,i-2); /* 15 */
			}
		}
	l=(l>>1)|(l<<31);
	r=(r>>1)|(r<<31);
	/* clear the top bits on machines with 8byte longs */
	l&=0xffffffffL;
	r&=0xffffffffL;

	FP(r,l);
	data[0]=l;
	data[1]=r;
}
int swapPasswd64to72(unsigned char *p_cp_passwd, int p_i_size, unsigned char *p_cp_dbkey)
{

	unsigned char pchtempStr[100];
	unsigned char code16Str[20];
	unsigned char code6Str[16];
	int	i,k;

	memcpy(pchtempStr, p_cp_passwd, p_i_size);
	pchtempStr[p_i_size] = 32;
	pchtempStr[p_i_size+1] = 0;

	for ( i=0; i<p_i_size+1; i++)
	{
		code16Str[i*2] = (pchtempStr[i] & 0xf0) >> 4;
		code16Str[i*2+1] = pchtempStr[i] & 0x0f;
	}
	code16Str[(p_i_size+1)*2] = 0;

	for (k=0; k<6; k++)
	{
		code6Str[2*k] = ( (code16Str[3*k]<<2)|(code16Str[3*k+1]>>2) ) + 0x30;
		code6Str[2*k+1] = ( ((code16Str[3*k+1]&0x03)<<4)|code16Str[3*k+2] ) + 0x30;
	}
	code6Str[12] = 0;
	memcpy(p_cp_dbkey, code6Str, strlen((char *)code6Str));

	return SUCC;
}

int swapPasswd72to64(unsigned char *p_cp_dbkey, int p_i_size, unsigned char *p_cp_passwd)
{

	unsigned char code6Str[16];
	unsigned char code16Str[20];
	unsigned char pchtempStr[100];
	int	i,k;

	memcpy(code6Str, p_cp_dbkey, 12);
	code6Str[12] = 0;

	for ( k=0; k<6; k++)
	{
//		code16Str[3*k] = ( (code6Str[2*k]-0x30) & 0x3c) >> 2;
		code16Str[3*k] = (code6Str[2*k]-0x30) >> 2;
		code16Str[3*k+1] = ( ((code6Str[2*k]-0x30)&0x03) << 2) + ( (code6Str[2*k+1]-0x30) >> 4);
		code16Str[3*k+2] = (code6Str[2*k+1]-0x30) & 0x0f;
	}
	code16Str[18] = 0;

	for ( i=0; i<p_i_size+1; i++)
	{
		pchtempStr[i] = (code16Str[i*2]<<4) | (code16Str[i*2+1]);
	}

	memcpy(p_cp_passwd, pchtempStr, p_i_size);
	p_cp_passwd[p_i_size] = 0;
	return SUCC;
}

