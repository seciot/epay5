/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台终端主密钥管理模块
** 创 建 人：冯炜
** 创建日期：2012-11-19
**
** $Revision: 1.1 $
** $Log: masterkey.ec,v $
** Revision 1.1  2012/12/07 01:19:47  fengw
**
** 1、终端主密钥生成程序初始版本。
**
*******************************************************************/

#include "masterkey.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

int main(int argc, char *argv[])
{
    EXEC SQL BEGIN DECLARE SECTION;
        int     iKeyIndex;                      /* 密钥索引 */
        char    szMasterKey[32+1];              /* 分发给终端的终端主密钥 */
        char    szMasterKeyLMK[32+1];           /* LKM加密后的终端主密钥 */
        char    szMasterKeyChk[4+1];            /* 终端主密钥校验值 */
    EXEC SQL END DECLARE SECTION;

    int     i;
    int     iKeyCount;                          /* 生成终端主密钥数 */
    int     iCount;                             /* 成功计数 */
    char    szKeyData[128+1];                   /* 密钥数据 */

    /* 获取启动参数 */
    if(argc < 2)
	{
		printf("参数错误\n");

		printf("示例:%s 需要生成密钥个数(0-%d)\n", argv[0], MAX_MASTERKEY_GENERATE);

        exit(-1);
	}

	iKeyCount = atoi(argv[1]);

	/* 检查密钥个数 */
	if(iKeyCount <=0 || iKeyCount > MAX_MASTERKEY_GENERATE)
	{
		printf("参数错误\n");

		printf("示例:%s 需要生成密钥个数(0-%d)\n", argv[0], MAX_MASTERKEY_GENERATE);

        exit(-1);
	}

    /* 打开数据库 */
    if(OpenDB() != SUCC)
    {
        printf("打开数据库失败!");

        exit(-1);
    }

    /* 读取当前密钥索引号 */
	EXEC SQL
	    SELECT NVL(MAX(key_index), 0)
	    INTO :iKeyIndex FROM pos_key;
	if(SQLCODE)
	{
	    printf("取当前密钥索引号失败!SQLCODE=%d SQLERR=%s\n", SQLCODE, SQLERR);

	    CloseDB();

	    exit(-1);
	}

	iCount = 0;

	for(i=1;i<=iKeyCount;i++)
	{
        /* 开始事务 */
        BeginTran();

	    memset(szKeyData, 0, sizeof(szKeyData));
		if(HsmGetMasterKey(szKeyData) != SUCC)
		{
			printf("获取终端主密钥失败!\n");

            break;
		}

        /* 从当前密钥索引号加一开始生成新密钥 */
        iKeyIndex++;

        memset(szMasterKey, 0, sizeof(szMasterKey));
        memset(szMasterKeyLMK, 0, sizeof(szMasterKeyLMK));
        memset(szMasterKeyChk, 0, sizeof(szMasterKeyChk));

		memcpy(szMasterKeyLMK, szKeyData, 32);
		memcpy(szMasterKey, szKeyData+32, 32);
		memcpy(szMasterKeyChk, szKeyData+64, 4);

		EXEC SQL
		    INSERT INTO pos_key (key_index, master_key, master_key_lmk, master_chk)
		    VALUES(:iKeyIndex, :szMasterKey, :szMasterKeyLMK, :szMasterKeyChk);
		if(SQLCODE)
		{
			printf("插入终端主密钥失败!SQLCODE=%d SQLERR=%s\n", SQLCODE, SQLERR);

			RollbackTran();

            break;
		}

		CommitTran();

		iCount++;
	}

	CloseDB();
	
	if(iKeyCount > iCount)
	{
	    printf("生成终端主密钥故障，已生成%d个终端主密钥\n", iCount);
    }
    else
    {
        printf("%d个终端主密钥生成完毕!\n", iCount);
    }

	exit(0);
}