
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：时间日期类接口函数
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.5 $
 * $Log: DateTime.c,v $
 * Revision 1.5  2012/12/04 06:26:21  chenjr
 * 代码规范化
 *
 * Revision 1.4  2012/11/27 03:20:01  linqil
 * *** empty log message ***
 *
 * Revision 1.3  2012/11/27 02:47:40  linqil
 * 增加引用pub.h 修改return
 *
 * Revision 1.2  2012/11/21 06:06:59  chenjr
 * 格式转换
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#include <time.h>
#include <string.h>
#include <stdio.h>
#include "pub.h"

#define ONEDAYCLOCKS (24 * 60 * 60)
#define MAXSIZE      60

extern int IsNumber(char *szStr);
static struct tm *GetLocalTime(long lSec);
static int IsLeapYear(int iYear);
static int GetMonMaxDay(int iYear, int iMon);

/* ----------------------------------------------------------------
 * 功    能：取系统当前时间日期 (格式自定义)
 * 输入参数：szFmt     输出时间日期格式 
 * 输出参数：szDTStr   输出当前系统时间日期,格式自定义
 * 返 回 值：-1   失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetSysDTFmt(const char *szFmt, char *szDTStr)
{
    struct tm *ptTm;

    if (szFmt == NULL || szDTStr == NULL)
    {
        return FAIL;
    }

    ptTm = GetLocalTime(0);
    strftime(szDTStr, MAXSIZE, szFmt, ptTm);

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：取系统当前日期(格式YYYYMMDD)
 * 输入参数：无
 * 输出参数：szDateStr  输出当前系统日期,格式为YYYYMMDD
 * 返 回 值：-1   失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetSysDate(char *szDateStr)
{
    struct tm *ptTm;

    if (szDateStr == NULL) 
    {
        return FAIL;
    }

    ptTm = GetLocalTime(0);
    GetSysDTFmt("%Y%m%d", szDateStr);

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：取系统当前时间(格式HHMMSS)
 * 输入参数：无
 * 输出参数：szTimeStr  输出当前系统时间,格式为HHMMSS
 * 返 回 值：-1   失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetSysTime(char *szTimeStr)
{
    struct tm *ptTm;

    if (szTimeStr == NULL) 
    {
        return FAIL;
    }

    ptTm = GetLocalTime(0);
    GetSysDTFmt("%H%M%S", szTimeStr);

    return SUCC;
}


/* ----------------------------------------------------------------
 * 功    能：取系统当前日期往前(后)几天的日期(格式YYYYMMDD)
 * 输入参数：iDays 距离当前日期的天数
 *           正数, 表示当前系统日期往后iDay天的日期
 *           0   , 当前系统日期
 *           负数, 表示当前系统日期往前iDay天的日期
 * 输出参数：szDateStr  输出当前系统日期,格式为YYYYMMDD
 * 返 回 值：-1   失败；  0  成功
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetDateSinceCur(int iDays, char *szDateStr)
{
    long      lSec;
    struct tm *ptTm;

    if (szDateStr == NULL)
    {
        return FAIL;
    }

    lSec = iDays * ONEDAYCLOCKS;
    ptTm = GetLocalTime(lSec);
    strftime(szDateStr, MAXSIZE, "%Y%m%d", ptTm);

    return SUCC;
}



/* ----------------------------------------------------------------
 * 功    能：检验日期格式是否合法(合法格式YYYYMMDD)
 * 输入参数：szDateStr
 * 输出参数：无
 * 返 回 值：-1   格式非法；  0  格式合法
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int ChkDateFmt(char *szDateStr)
{
    int iYear, iMon, iDay;

    if (szDateStr == NULL)
    {
        return FAIL;
    }

    /* 必须是8个字节 */
    if (strlen(szDateStr) != 8)
    {
        return FAIL;
    }
 
    /* 必须是全数字 */
    if (IsNumber(szDateStr) == -1)
    {
        return FAIL;
    }

    /* 年必须在有效范围内 */
    iYear = (szDateStr[0] - '0') * 1000 +
            (szDateStr[1] - '0') * 100  +
            (szDateStr[2] - '0') * 10   +
            (szDateStr[3] - '0'); 
    if (iYear < 1900 || iYear > 2500)
    {
        return FAIL;
    }

    /* 月必须在有效范围内 */
    iMon = (szDateStr[4] - '0') * 10   +
           (szDateStr[5] - '0'); 
    if (iMon < 1 || iMon >= 13)
    {
        return FAIL;
    }

    /* 日必须在有效范围内(注意平闰年二月份的天数） */
    iDay = (szDateStr[6] - '0') * 10   +
           (szDateStr[7] - '0');

    if (iDay < 1 || iDay >  GetMonMaxDay(iYear, iMon))
    {
        return FAIL;
    }

    return SUCC;
}


/* 获取指定月的最大天数 */
static int GetMonMaxDay(int iYear, int iMon)
{
    int iDays = 0;
    int mon_maxdayset[]={0,31,28,31,30,31,30,31,31,30,31,30,31};

    iDays = mon_maxdayset[iMon];
    if (!IsLeapYear(iYear) && iMon == 2)
    {
        iDays += 1;
    }

    return iDays;
}



/* 判断平闰年 */
static int IsLeapYear(int iYear)
{
    if (iYear <= 0)
    {
        return FAIL;
    }

    if ((iYear % 4 == 0 && iYear % 100 != 0) ||
        iYear % 400 == 0)
    {
        return SUCC;
    }

    return FAIL;
}


/* 返回指定clock的tm结构 */
static struct tm *GetLocalTime(long lSec)
{
    long       lClock;

    time(&lClock);
    lClock += lSec;

    return localtime(&lClock);
}
