/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 获取完整文件名
** 创 建 人：冯炜
** 创建日期：2012-12-10
**
** $Revision: 1.3 $
** $Log: GetFullName.c,v $
** Revision 1.3  2012/12/20 09:25:54  wukj
** Revision后的美元符
**
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/****************************************************************
** 功    能：获取完整文件名
** 输入参数：
**        szEnvName                 环境变量名
**        szFileName                短文件名
** 输出参数：
**        szFullFileName            完整文件名
** 返 回 值：
**        char*                     完整文件名
** 作    者：
**        fengwei
** 日    期：
**        2012/12/10
** 调用说明：
**
** 修改日志：
****************************************************************/
char* GetFullName(char* szEnvName, char* szFileName, char* szFullFileName)
{
    char    szEnv[64+1];

    memset(szEnv, 0, sizeof(szEnv));
    if(getenv(szEnvName) != NULL)
    {
        strcpy(szEnv, getenv(szEnvName));
    }
    else
    {
        strcpy(szEnv, "/");
    }

    sprintf(szFullFileName, "%s%s", szEnv, szFileName);

    return szFullFileName;
}
