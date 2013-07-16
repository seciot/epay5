
/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：读配置文件
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.11 $
 * $Log: ReadConf.c,v $
 * Revision 1.11  2013/06/17 03:10:39  fengw
 *
 * 1、判断条目是否存在时，增加判断条目后一个字节是否是结束符、空格、tab，防止查询相似条目时出现误判。
 * 2、拷贝条目值时，跳过条目与值之间的空格、tab，防止变量越界。
 *
 * Revision 1.10  2012/12/04 07:06:22  chenjr
 * 代码规范化
 *
 * Revision 1.9  2012/12/03 06:31:35  yezt
 *
 * 改“Writelog” 为"WriteLog“
 *
 * Revision 1.8  2012/11/29 02:15:35  linqil
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.7  2012/11/28 08:25:44  linqil
 * 去掉冗余头文件user.h
 *
 * Revision 1.6  2012/11/28 02:40:25  linqil
 * 修改日志函数
 *
 * Revision 1.5  2012/11/27 05:55:25  linqil
 * 去掉void函数的返回值
 *
 * Revision 1.4  2012/11/21 06:08:30  chenjr
 * *** empty log message ***
 *
 * Revision 1.3  2012/11/21 06:05:53  chenjr
 * 添加从WORKDIR环境变量读取配置路径功能
 *
 * Revision 1.2  2012/11/20 07:49:11  chenjr
 * 修改函数参数,去除默认值输入参数.
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pub.h"

extern char *DelAllSpace(char *szStr);

/* ----------------------------------------------------------------
 * 功    能：读配置文件
 * 输入参数：szFilenm    配置文件名
 *           szSection   章节名,一个配置文件可以有多个章节，每个章节下又
 *                       可以包含多个条目
 *           szItem      条目, 后面跟着该条目的具体值
 * 输出参数：szVal       条目值，与szItem条目同处一行，中间用空格或=分隔
 * 返 回 值：0  读取成功；  -1  读取失败
 * 作    者：
 * 日    期：
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */ 
int ReadConfig(char *szFilenm, char *szSection, char *szItem, char *szVal)
{
    int   findSection = 0;
    int   findItem = 0;
    FILE  *fp;
    char  acLine[200], acTmp[51], szPath[500], *pEnv;
    int   i;

    if (szFilenm == NULL || szSection == NULL || szItem == NULL ||
        szVal    == NULL)
    {
        WriteLog(ERROR, "Invalid argument\n");
        return  FAIL;
    }

    pEnv = getenv("WORKDIR");
    if (pEnv == NULL)
    {
        WriteLog(ERROR, "environment variable[WORKDIR] isn't set [%d-%s]", errno, strerror(errno));
        return  FAIL;
    }

    memset(szPath, 0, sizeof(szPath));
    sprintf(szPath, "%s/etc/%s", pEnv, szFilenm);

    fp = fopen(szPath, "r");
    if (fp == NULL)
    {
        WriteLog(ERROR, "Open file[%s] error[%d-%s]", szPath, errno, strerror(errno));
        return  FAIL;
    }

    while (!feof(fp))
    {
        memset(acLine, 0x00, sizeof(acLine));

        if (fgets(acLine, 80, fp) == NULL)
        {
            break;
        }

        acLine[strlen(acLine)-1] = 0;      /* 去掉换行符 '0x0A' */
        memset(acTmp, 0, sizeof(acTmp));

        if( acLine[0]=='#' )
        {
            continue;
        }

        if (acLine[0] == '[')
        {
            if (findSection == 1)     /* 已到达下一章节,说明未找到配置 */
            {
                break;
            }

        //    findSection = 0;
        
            memcpy(acTmp, acLine + 1, strlen(szSection) );

            if (memcmp(szSection, acTmp, strlen(acTmp) ) == 0)
            {
                findSection = 1;
            }
        }
        else
        {
            if (findSection == 0)
            {
                continue;
            }

            if (memcmp(acLine, szItem, strlen(szItem)) == 0)
            {
                if(acLine[strlen(szItem)] == 0x00)
                {
                    szVal[0] =0x00;
                    findItem=1;
                    break;
                }
                else if(acLine[strlen(szItem)] == 0x09 || acLine[strlen(szItem)] == 0x20)
                {
                    for(i=strlen(szItem)+1;i<strlen(acLine);i++)
                    {
                        if(acLine[i] != 0x20 && acLine[i] != 0x09)
                        {
                            break;
                        }
                    }
                    strcpy(szVal, acLine+i);
                    findItem=1;
                    break;
                }
            }
        }
    }

    fclose(fp);

    if (findSection == 0 || findItem==0 )
    {
/*
        if (szDefVal != NULL)
        {
            strcpy(szVal, szDefVal);
            return 0;
        }
*/
        WriteLog(ERROR, "Not found param[%s][%s] in file[%s]",
                         szSection, szItem, szFilenm);
        return  FAIL;
    }

    DelAllSpace(szVal);
    return  SUCC;
}
