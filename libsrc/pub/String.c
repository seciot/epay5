
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：字符串处理类接口函数
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.4 $
 * $Log: String.c,v $
 * Revision 1.4  2012/12/20 06:01:14  wukj
 * 删除GetField函数,用GetStrData函数替换
 *
 * Revision 1.3  2012/12/04 07:26:35  chenjr
 * 代码规范化
 *
 * Revision 1.2  2012/11/27 06:44:55  linqil
 * 增加引用pub.h；修改return语句
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */


#include <string.h>
#include "pub.h"

/* ----------------------------------------------------------------
 * 功    能：删除字符串首的所有空格
 * 输入参数：szStr  被删除空格前的字符串
 * 输出参数：szStr  删除串前所有空格后的字符串
 * 返 回 值：szStr  删除串前所有空格后的字符串
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
char *DelHeadSpace(char *szStr)
{
    char szBuf[1024];
    int  i = 0;

    if (szStr == NULL)
    {
        return NULL;
    }
 
    memset(szBuf, 0, sizeof(szBuf));
    strcpy(szBuf, szStr);

    while (szBuf[i] == ' ')
    {
        i++;
    }

    strcpy(szStr, szBuf + i);
    return szStr;
}

/* ----------------------------------------------------------------
 * 功    能：删除字符串末的所有空格
 * 输入参数：szStr  被删除空格前的字符串
 * 输出参数：szStr  删除串末所有空格后的字符串
 * 返 回 值：szStr  删除串末所有空格后的字符串
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
char *DelTailSpace(char *szStr)
{
    int  l;

    if (szStr == NULL)
    {
        return NULL;
    }

    l = strlen(szStr) - 1;
    while (l >= 0 && szStr[l] == ' ')
    {
        l--;
    }

    l++;
    szStr[l] = '\0';
    return szStr;
}

/* ----------------------------------------------------------------
 * 功    能：删除字符串中的所有空格
 * 输入参数：szStr  被删除空格前的字符串
 * 输出参数：szStr  删除所有空格后的字符串
 * 返 回 值：szStr  删除所有空格后的字符串
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
char *DelAllSpace(char *szStr)
{
    char szBuf[1024], *pStr;
    int i = 0;

    if (szStr == NULL)
    {
        return NULL;
    }

    pStr = szBuf;
    while (*(szStr+i) != '\0')
    {
        if (*(szStr+i) != ' ')
        {
            *pStr++ = *(szStr+i); 
        }
        i++;
    }

    *pStr='\0';
    strcpy(szStr, szBuf);
    return(szStr);
}


/* ----------------------------------------------------------------
 * 功    能：转换字符串所有小写字符为大写
 * 输入参数：szStr  转换前字符串
 * 输出参数：szStr  转换后字符串
 * 返 回 值：szStr  转换后字符串
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
char *ToUpper(char *szStr)
{
    int i = 0;

    if (szStr == NULL)
    {
        return NULL;
    }

    while (*(szStr+i) != '\0')
    {
        if (*(szStr+i) >= 'a' && *(szStr+i) <= 'z')
        {
            *(szStr+i) -= ('a' - 'A');
        }
        i++;
    }
    
    return szStr;
}

/* ----------------------------------------------------------------
 * 功    能：转换字符串所有大写字符为小写
 * 输入参数：szStr  转换前字符串
 * 输出参数：szStr  转换后字符串
 * 返 回 值：szStr  转换后字符串
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
char *ToLower(char *szStr)
{
    int i = 0;

    if (szStr == NULL)
    {
        return NULL;
    }

    while (*(szStr+i) != '\0')
    {
        if (*(szStr+i) >= 'A' && *(szStr+i) <= 'Z')
        {
            *(szStr+i) += ('a' - 'A');
        }
        i++;
    }
    
    return szStr;

}

/* ----------------------------------------------------------------
 * 功    能：判断字符串是否由全数字组成
 * 输入参数：szStr  被校验串
 * 输出参数：
 * 返 回 值：0  全数字；  -1  非全数字
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int IsNumber(char *szStr)
{
    int i = 0;

    if (szStr == NULL)
    {
        return FAIL;
    }

    while ( *(szStr+i) != '\0')
    {
        if (*(szStr+i) < '0' || *(szStr+i) > '9')
        {
            return FAIL;
        }
        i++;
    }

    return SUCC;
}


/***********************************************************************
 *      函 数 名:int    GetStrData(
 *                      char * pszInBuf
                        ,int iPosition
			,char *szDelimiter
 *                      ,char * pszOutData)
 *      入口参数: 
 *                pszInBuf:输入串
 *                iPosition:取出位置
 *		  pszDelimiter:分隔符,支持1到多个字节作为分隔符
 *      返回参数: 函数返回>=0表示成功
 *                pszOutData:输出串
 *
 *      引用全局变量:
 *      修改全局变量:
 *      父 函 数:
 *      子 函 数:
 *      访问数据:
 *      函数功能: 读取以pszDelimiter分隔的数据中的某个数据
 ***********************************************************************/
int GetStrData(char *szInBuf,int iPosition,char *szDelimiter,char *szOutData)
{
	int i,j,k;
	j=0;
	k=0;
	while(k<iPosition)
	{
		if(szInBuf[j]=='\0')return(-1);
		else if(memcmp(szInBuf+j,szDelimiter,strlen(szDelimiter)) == 0 )
		{
			k++;
			j += strlen(szDelimiter);
			continue;
		}
		j++;
	}
	i=0;
	while(memcmp(szInBuf+j+i,szDelimiter,strlen(szDelimiter)) != 0)
	{
		if(szInBuf[j+i]=='\0')break;
		szOutData[i]=szInBuf[j+i];
		i++;
	}
	szOutData[i]='\0';
	return(i);
}
