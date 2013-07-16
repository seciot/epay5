/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：系统软加密主密钥合成工具
 *  创 建 人：chenjr
 *  创建日期：2012/12/7
 * ----------------------------------------------------------------
 * $Revision: 1.2 $
 * $Log: genMasterKey.c,v $
 * Revision 1.2  2012/12/24 04:34:17  chenjr
 * 添加密钥校验值功能
 *
 * Revision 1.1  2012/12/07 06:18:24  chenjr
 * init
 *
 * Revision 1.1  2012/12/07 06:11:21  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */


#include "genMasterKey.h"

int iKeyLen = 0;                /* 密钥长度 */
int iComponent = 0;             /* 密钥分量 */
unsigned char uszaKeyComp[49];  /* 密钥明文 */

/*
 * 打印密钥长度选择菜单
 */
void PrtKEYLTMenu(void)
{
    system("clear");
    printf(MODELTITLE);
    printf(KEYLT_MENU);
}

/*
 * 打印密钥分量选择菜单
 */
void PrtKeyCompMenu(void)
{
    system("clear");
    printf(MODELTITLE);
    printf(KEYLT_LEN, iKeyLen);
    printf(KEYCOM_MENU);
}

/*
 * 打印"输入密钥分量"提示头
 */
void PrtComConHead(void)
{
    system("clear");
    printf(COMCON_HEAD, iKeyLen, iComponent);
}

/*
 * 打印密钥保存提示头
 */
void PrtSaveComHead(void)
{
    fflush(NULL);
    system("clear");
    printf(COMCON_HEAD, iKeyLen, iComponent);
}

/* ----------------------------------------------------------------
 * 功    能：设置密钥参数(包括长度类型与分量值)
 * 输入参数：pfun   指具体函数
 * 输出参数：
 * 返 回 值：菜单对应值
 * 作    者：陈建荣
 * 日    期：2012/12/7
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int setKeyArgu(void(*pfun)(void))
{
    int iTmp = 0;

    do{
        pfun();

        printf("\t");
        iTmp = getchar();
        getchar();  /* 接收回车,丢弃 */

        if (iTmp == 'Q' || iTmp == 'q')
        {
            exit(0);
        }
    }while (iTmp != '1' && iTmp != '2' && iTmp != '3');

    return iTmp - '0';
}

/* ----------------------------------------------------------------
 * 功    能：字符范围判断与处理
 *           字符只能是0-9、a-f、A-F,如果a-f就转换成A-F
 * 输入参数：piCh   字符值
 * 输出参数：
 * 返 回 值：-1  无法字符   >0 所读字符ASC值
 * 作    者：陈建荣
 * 日    期：2012/12/7
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int chkChar(int *piCh)
{
    if ( (*piCh >= '0' && *piCh <= '9') ||
         (*piCh >= 'a' && *piCh <= 'f') ||
         (*piCh >= 'A' && *piCh <= 'F') 
       )
    {
        if ( *piCh >= 97 && *piCh <= 102 )
        {
            *piCh -= ('a'-'A');
        }

        return *piCh;
    }
    else
    {
        return -1;
    }
}

/* ----------------------------------------------------------------
 * 功    能：从标准输入读取一个字符，并对所读取的字符进行处理
 * 输入参数：无
 * 输出参数：iVal 所读的字符 （字符范围0-9、A-F)
 * 返 回 值：-1  系统错误   >0 所读字符ASC值
 * 作    者：陈建荣
 * 日    期：2012/12/7
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
int getch(unsigned char *iVal)
{
    struct termios tm, tm_old;
    int fd = 1, c;

    if (tcgetattr(fd, &tm) < 0)
    {
        return -1;
    }

    tm_old = tm;
    cfmakeraw(&tm);

    if (tcsetattr(fd, TCSANOW, &tm) < 0)
    {
        return -1;
    }

    do{
        c = fgetc(stdin);
    }while (chkChar(&c) == -1);

    if (tcsetattr(fd, TCSANOW, &tm_old) < 0)
    {
        return -1;
    }

    *iVal = c;

    return c; 
}

/* ----------------------------------------------------------------
 * 功    能：读取密钥分量并合成
 * 输入参数：iComp   分量值（第几组分量)
 * 输出参数：uszKeyComp  合成后的密钥明文
 * 返 回 值：无
 * 作    者：陈建荣
 * 日    期：2012/12/7
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
void setKeyComp(int iComp,  unsigned char *uszKeyComp)
{
    int iredo = 0, inum, iInAgain, i;
    unsigned char usaComp[2][100];
    unsigned char tmp[100];

    do{
        iInAgain = 0;
        memset(usaComp, 0, sizeof(usaComp));

        do{
            fflush(NULL);
            PrtComConHead();
    
            iredo == 0 ?  printf(COMCON_INPUT, iComp + 1)
                       :  printf(COMCON_REIN, iComp + 1);
    
            inum = 0;
            printf(COMCON_INLEN, inum);
            
            /* 根据密钥长度从标准输入读取密钥分量 */
            while (inum < iKeyLen)
            {
                if (getch(&usaComp[iredo][inum]) == -1)
                {
                    exit(0);
                }
    
                inum++;
                inum < 11 ? printf("\b") : printf("\b\b");
                printf("%d", inum);
            }
    
            iredo++;
        }while(iredo < 2);
    
        /* 两次输入一致性判断 */
        if (memcmp(usaComp[0], usaComp[1], iKeyLen) != 0)
        {
            printf(COMCON_ICON);
            getchar();
            iredo = 0;
            iInAgain = 1;
        }
    }while (iInAgain);

    /* 合成分量 */
    AscToBcd(usaComp[0], iKeyLen, 0, tmp);
    for (i=0; i< (iKeyLen+1) / 2; i++)
    {
        uszKeyComp[i] ^= tmp[i]; 
    }
}

void getChkVal(unsigned char *uszChk)
{
    DES(uszaKeyComp, CHKVALEELEM, uszChk);
}


/* 主函数 */
int main(void)
{
    int i, icomp = 0;
    char szKeyText[100], szChkVal[10];
    unsigned char uszChkV[5];

    /* 选择密钥长度(单倍长、双倍长) */
    iKeyLen = setKeyArgu(PrtKEYLTMenu) * 16;

    /* 选择密钥分量数(一个分量、两个分量、三个分量) */
    iComponent = setKeyArgu(PrtKeyCompMenu);

    /* 根据密钥长度及分量数读取分量并合成密钥 */
    memset(uszaKeyComp, 0, sizeof(uszaKeyComp));
    while (icomp < iComponent)
    {
        setKeyComp(icomp, uszaKeyComp);
        icomp++;
    }

    memset(szKeyText, 0, sizeof(szKeyText));
    BcdToAsc(uszaKeyComp, iKeyLen, 0, szKeyText);

    /* 保存密钥 */
    PrtSaveComHead();
    if (SaveMasterKey(szKeyText, iKeyLen) != FAIL)
    {
        getChkVal(uszChkV);
        memset(szChkVal, 0, sizeof(szChkVal));
        BcdToAsc(uszChkV, 8, 0, szChkVal);
        printf(SAVECOM_SUCC, szChkVal);
    }
    else
    {
        printf(SAVECOM_SUCC);
    }
}

