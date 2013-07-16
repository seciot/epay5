/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：系统软加密主密钥保存与获取接口
 *  创 建 人：chenjr
 *  创建日期：2012/12/7
 * ----------------------------------------------------------------
 * $Revision: 1.2 $
 * $Log: SoftMasterKey.c,v $
 * Revision 1.2  2012/12/27 07:20:12  fengw
 *
 * 1、解密后密钥以BCD码输出。
 *
 * Revision 1.1  2012/12/07 06:22:45  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "libpub.h"
#include "user.h"


#define ENC_KEY "\xE0\x35\x21\xA4\x5B\x4C\x89\x67"
#define FILEPATH "etc/MKEY.dat"


/* ----------------------------------------------------------------
 * 功    能：对主密钥明文加密后保存进文件
 * 输入参数：szKeyText  主密钥明文
 *           iKeyLen    主密钥明文长度
 * 输出参数：无
 * 返 回 值：FAIL 保存失败    SUCC保存成功
 * 作    者：陈建荣
 * 日    期：2012/12/7
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int SaveMasterKey(char *szKeyText, int iKeyLen)
{
    int i;
    FILE *fp;
    unsigned char szMKey[48], szEnMKey[48], szAscMKey[48];
    char szFileName[100], *pEnv;

    AscToBcd(szKeyText, iKeyLen, 0, szMKey);

    for (i=0; i < iKeyLen / 2; i+=8)
    {
        DES(ENC_KEY, szMKey + i, szEnMKey + i);
    }

    BcdToAsc(szEnMKey, iKeyLen, 0, szAscMKey );

    pEnv = getenv("WORKDIR");
    if (pEnv == NULL)
    {
        WriteLog(ERROR, "env var[WORKDIR] isn't set [%d-%s]"
                , errno, strerror(errno));
        return  FAIL;
    }    

    memset(szFileName, '\0', sizeof(szFileName));
    sprintf(szFileName, "%s/%s", pEnv, FILEPATH);

    if( (fp=fopen(szFileName, "w+")) == NULL)
    {
        WriteLog(ERROR, "open file e");
        return( FAIL);
    }

    fprintf(fp, "%s\n", szAscMKey);

    fclose(fp);

    return(SUCC);
}

/* ----------------------------------------------------------------
 * 功    能：从密钥文件取出主密钥密文并解密返回
 * 输入参数：无
 * 输出参数：szKeyText   解密后的主密钥明文
 * 返 回 值：FAIL  提取失败   >0  主密钥明文长度
 * 作    者：陈建荣
 * 日    期：2012/12/7
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int GetMasterKey(char *szKeyText)
{
    int iKeyLen, i;
    FILE    *fp;
    unsigned char   szTmpBuf[100], szTmpKey[25], szDeMKey[25];
    char    szFileName[100], *pEnv;

    pEnv = getenv("WORKDIR");
    if (pEnv == NULL)
    {
        WriteLog(ERROR, "env var[WORKDIR] isn't set [%d-%s]"
                , errno, strerror(errno));
        return  FAIL;
    }

    memset(szFileName, '\0', sizeof(szFileName));
    sprintf(szFileName, "%s/%s", pEnv, FILEPATH);

    if( (fp=fopen(szFileName, "r")) == NULL)
    {
        WriteLog(ERROR, "open file err");
        return( FAIL );
    }
   
    memset(szTmpBuf, 0, sizeof(szTmpBuf));
    fscanf(fp, "%s", szTmpBuf);
    fclose(fp);
    iKeyLen = strlen(szTmpBuf);

    AscToBcd(szTmpBuf, iKeyLen, 0, szTmpKey);

    for (i=0; i < iKeyLen / 2; i+=8)
    {
        _DES(ENC_KEY, szTmpKey + i, szDeMKey + i);
    }

    memcpy(szKeyText, szDeMKey, iKeyLen/2);

    return (iKeyLen);
}

