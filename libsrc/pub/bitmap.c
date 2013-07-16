
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：位图展开和压缩
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.2 $
 * $Log: bitmap.c,v $
 * Revision 1.2  2012/12/20 01:18:09  wukj
 * 增加注释
 *
 *
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "tools.h"

void
UncompressBitmap( char *szOutData, char *szAscBitmap, int nLen )
{
	int i, j = 0;
	unsigned char szBitmap[50], cChr;

	if( nLen%2 != 0 )
	{
		WriteLog( ERROR, "len error %ld", nLen );
		return;
	}

	if( nLen > 64 )
	{
		WriteLog( ERROR, "len too long%ld", nLen );
		return;
	}

	AscToBcd( (unsigned char*)szAscBitmap, nLen, 0 ,szBitmap);
	nLen = nLen/2;

	for( i=0; i<nLen; i++ )	
	{
		cChr = 0x80;	
		for( j=0; j<8; j++ )
		{
			if( (szBitmap[i]&cChr) == 0 )
			{
				szOutData[i*8+j] = '0';
			}
			else
			{
				szOutData[i*8+j] = '1';
			}
			cChr = cChr/2;
		}
	}
	szOutData[i*8+j] = 0;

	return;
}

void
CompressBitmap( char *szInData, char *szAscBitmap, int nLen )
{
	int i, j, nBytes;
	unsigned char	szBitmap[33], cChr;

	if( nLen%8 != 0 )
	{
		WriteLog( ERROR, "len error %ld", nLen );
		return;
	}

	if( nLen > 256 )
	{
		WriteLog( ERROR, "len too long %ld", nLen );
		return;
	}

	nBytes = nLen/8;

	memset( szBitmap, 0, 32 );
	for( i=0; i<nBytes; i++ )
	{
		cChr = 0x80;	
		for(j=0; j<8; j++)
		{
			if( szInData[i*8+j] == '1' )
			{
				szBitmap[i] = szBitmap[i]|cChr;
			}
			cChr = cChr/2;
		}
	}

	BcdToAsc( szBitmap, nBytes*2, 0 ,(unsigned char*)szAscBitmap);
	szAscBitmap[nBytes*2] = 0;

	return;
}
