/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:
           银联专用加密机接口
** 创 建 人:Robin 
** 创建日期:2009/08/29


$Revision: 1.10 $
$Log: unionpayhsm.c,v $
Revision 1.10  2012/12/26 01:44:17  wukj
%s/commu_with_hsm/CommuWithHsm/g

Revision 1.9  2012/12/05 06:32:14  wukj
*** empty log message ***

Revision 1.8  2012/11/29 07:51:43  wukj
修改日志函数,修改ascbcd转换函数

Revision 1.7  2012/11/29 01:57:55  wukj
日志函数修改

Revision 1.6  2012/11/21 04:13:38  wukj
修改hsmincl.h 为hsm.h

Revision 1.5  2012/11/21 03:20:31  wukj
1:加密机类型名字修改 2: 全局变量移至hsmincl.h


*******************************************************************/

#include "hsm.h"

/*****************************************************************
** 功    能:加密机随机生成终端主密钥TMK，并将其密文和检查值返回给主机.
** 输入参数:
           无
** 输出参数:
           tInterface->szData   TMK(存放中心32Bytes)+CheckValue(4Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06UphsmGetTmk( T_Interface *tInterface, int iSekTmkIndex, int iTekIndex )
{
    char    szInData[1024], szOutData[1024], szRetCode[3];
    int     iLen, iRet, iSndLen, i;

    iLen = 0;
    /*命令*/
    memcpy( szInData, "K0", 2 );    
    iLen += 2;
    /* sek */
    sprintf( szInData+iLen, "S%04ld", iSekTmkIndex ); 
    iLen += 5;
    /* tek */
    sprintf( szInData+iLen, "T%04ld", iTekIndex ); 
    iLen += 5;
    szInData[iLen] = 'Y';
    iLen ++;
    szInData[iLen] = 0;
    memset( szOutData, 0, 1024 );
    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( memcmp(szOutData, "K1", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData, szOutData+4, 68 );
    tInterface->iDataLen = 68;
    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能:生成数据密钥PIK/MAK，并将其密文和检查值返回给主机.
** 输入参数:
           无
** 输出参数:
           tInterface->szData   
                  PIK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)+ 
                  MAK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(168Bytes)
                  MAG(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06UphsmGetWorkKey(T_Interface *tInterface, int iSekTmkIndex,
        int iSekWorkIndex )
{
    char    szInData[1024], szOutData[1024], szPsamNo[17], szRand[33];
    char    szEnTmk[33];
    int     iLen, iRet, iSndLen, i;
    char    cChr;

    memcpy( szEnTmk, tInterface->szData, 32 );
    
    for( i=0; i<3; i++ )
    {
        iLen = 0;
        /*命令*/
        memcpy( szInData, "K2", 2 );    
        iLen += 2;
        /*用于解密终端主密钥的存储加密密钥索引*/
        sprintf( szInData+iLen, "S%04ld", iSekTmkIndex ); 
        iLen += 5;
        /*用于加密工作密钥的存储加密密钥索引*/
        sprintf( szInData+iLen, "S%04ld", iSekWorkIndex ); 
        iLen += 5;
        /*终端主密钥长度标识，16位长*/
        szInData[iLen] = 'Y';            
        iLen ++;
        memcpy( szInData+iLen, szEnTmk, 32 );    
        iLen += 32;
        /*生成的工作密钥长度标识，16位长*/
        szInData[iLen] = 'Y';    
        iLen ++;
        szInData[iLen] = 0;
        memset( szOutData, 0, 1024 );
        iRet = CommuWithHsm( szInData, iLen, szOutData ); 
        if( iRet == FAIL )
        {
            WriteLog( ERROR, "commu with hsm fail" );
            strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
            return FAIL;
        }
        if( memcmp(szOutData, "K3", 2) != 0  ||
            memcmp(szOutData+2, "00", 2) != 0 )
        {
            DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
            WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
            return SUCC;
        }

        memcpy( tInterface->szData+i*80, szOutData+4, 80 );
    }

    tInterface->iDataLen = 240;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 用ANSI X9.9 MAC算法对数据做MAC. 
** 输入参数:
           tInterface->szData    参与MAC运算的数据
           tInterface-.datalen 长度
** 输出参数:
           tInterface->szData    MAC(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06UphsmCalcMac(T_Interface *tInterface, int iSekIndex)
{
    char    szInData[2048], szOutData[2048];
    int     iLen, iRet, iSndLen;

    iLen = 0;
    memcpy( szInData, "M0", 2 );    /* 命令 */
    iLen += 2;

    /* MAC算法 1-XOR 2-X9.9 3-X9.19 */
    if( tInterface->iAlog == X99_CALC_MAC )
    {
        szInData[iLen] = '2';        
    }
    else if( tInterface->iAlog == XOR_CALC_MAC )
    {
        szInData[iLen] = '1';
    }
    else
    {
        szInData[iLen] = '3';
    }
    iLen += 1;

    sprintf( szInData+iLen, "S%04ld", iSekIndex );    /* 加密密钥索引 */
    iLen += 5;

    szInData[iLen] = 'Y';            /* 密钥长度标识，16位长 */
    iLen ++;

    BcdToAsc( (uchar *)(tInterface->szMacKey), 32, 0, (uchar *)(szInData+iLen) );    /*MAC密钥密文*/
    iLen += 32;

    sprintf( szInData+iLen, "%04ld", tInterface->iDataLen );
    iLen += 4;

    memcpy( szInData+iLen, tInterface->szData, tInterface->iDataLen );
    iLen += tInterface->iDataLen;
    
    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( memcmp(szOutData, "M1", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm calc mac fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    AscToBcd( (uchar *)(szOutData+4), 16, 0 ,(uchar *)(tInterface->szData));
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 将源PIN密文用源PIK解密，进行PIN格式转换，然后用目的PIK加密输出.
** 输入参数:
           tInterface->szData    源帐号(16字节)+密码密文(8字节)+目的帐号(16字节) 
** 输出参数:
           tInterface->szData    转加密后的密码密文(8字节)
** 返 回 值: 
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06UphsmChangePin(T_Interface *tInterface, int iSekPosIndex, int iSekPppIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szTargetPan[17];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "0000%12.12s", tInterface->szData+3 );
    szPanBlock[16] = 0;

    sprintf( szTargetPan, "0000%12.12s", tInterface->szData+27 );
    szTargetPan[16] = 0;

    iLen = 0;
    memcpy( szInData, "P0", 2 );    /* 命令 */
    iLen += 2;

    /* 加密源PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "S%04ld", iSekPosIndex );
    iLen += 5;

    /* 加密目的PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "S%04ld", iSekPppIndex );
    iLen += 5;

    /* 源PIK密钥长度标识，16位长 */
    szInData[iLen] = 'Y';    
    iLen ++;

    /* 源PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szPinKey), 32, 0 ,(uchar *)(szInData+iLen));    
    iLen += 32;

    /* 目的PIK密钥长度标识，16位长 */
    szInData[iLen] = 'Y';    
    iLen ++;

    /* 目的PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szMacKey), 32, 0 ,(uchar *)(szInData+iLen));    
    iLen += 32;

    /* 源PinBlock格式 */
    memcpy( szInData+iLen, "01", 2 );    
    iLen += 2;

    /* 目的PinBlock格式 */
    memcpy( szInData+iLen, "01", 2 );    
    iLen += 2;

    /* 源PinBlock密文 */
    BcdToAsc( (uchar *)(tInterface->szData+16), 16, 0 ,(uchar *)(szInData+iLen));    
    iLen += 16;

    /* 源帐号 */
    memcpy( szInData+iLen, szPanBlock, 16 );    
    iLen += 16;

    /* 目的帐号 */
    memcpy( szInData+iLen, szTargetPan, 16 );    
    iLen += 16;

    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( memcmp(szOutData, "P1", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm pin change fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    AscToBcd( (uchar *)(szOutData+4), 16, 0 ,(uchar *)(tInterface->szData));
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:  验证终端上送的PIN是否与数据库中的PIN一致
              分2步:
              1.用PIK对PIN明文加密
              2.与终端上送的密文进行比较
** 输入参数:
           tInterface->szData     数据库中密码明文(8字节)+终端PIN密文(8字节)
** 输出参数:
           tInterface->szData     SUCC-一致  FAIL-不一致
** 返 回 值: 
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06UphsmVerifyPin(T_Interface *tInterface, int iSekPosIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szEncPin[17];
    int     iLen, iRet, iSndLen;

    memset( szPanBlock, '0', 16 );
    szPanBlock[16] = 0;

    iLen = 0;
    memcpy( szInData, "60", 2 );    /* 命令 */
    iLen += 2;

    /* 加密PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "S%04ld", iSekPosIndex );
    iLen += 5;

    /* PIK密钥长度标识，16位长 */
    szInData[iLen] = 'Y';    
    iLen ++;

    /* PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szPinKey), 32, 0 ,(uchar *)(szInData+iLen));    
    iLen += 32;

    /* PinBlock格式 */
    memcpy( szInData+iLen, "01", 2 );    
    iLen += 2;

    /* PIN明文 */
    memcpy( szInData+iLen, "08", 2 );
    iLen += 2;
    memcpy( szInData+iLen, tInterface->szData, 8 );
    iLen += 8;
    memcpy( szInData+iLen, "FFFFFFFF", 6 );
    iLen += 6;

    /* 帐号 */
    memcpy( szInData+iLen, szPanBlock, 16 );    
    iLen += 16;

    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( memcmp(szOutData, "61", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm encrypt pin fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    AscToBcd( (uchar *)(szOutData+4), 16, 0 ,(uchar *)szEncPin);
    if( memcmp( tInterface->szData+8, szEncPin, 8 ) == 0 )
    {
        strcpy( tInterface->szData, "SUCC" );
    }
    else
    {
        strcpy( tInterface->szData, "FAIL" );
    }
    tInterface->iDataLen = 4;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 生成密钥的校验值
** 输入参数:
           tInterface->szData   密钥密文(32字节)
** 输出参数:
           tInterface->szData   校验值(16)
** 返 回 值: 
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06UphsmCalcChkval(T_Interface *tInterface, int iSekIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17];
       int     iLen, iRet, iSndLen;

    iLen = 0;
    memcpy( szInData, "3A", 2 );    /* 命令 */
    iLen += 2;

    /* 加密PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "S%04ld", iSekIndex );
    iLen += 5;

    /* 密钥长度标识，16位长 */
    szInData[iLen] = 'Y';    
    iLen ++;

    /* 密钥密文 */
    memcpy( szInData+iLen, tInterface->szData, 32 );
    iLen += 32;

    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( memcmp(szOutData, "3B", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm encrypt pin fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData, szOutData+4, 8 );
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 加密机转加密工作密钥，即将由TMK加密的工作密钥还原成明文，然后再用指定的SEK加密
** 输入参数:
           tInterface->szData   TMK密文(32Bytes)+PIK密文(32Bytes)+MAK密文(32Bytes)，工作密钥由TMK加密
           iSekTmkIndex       加密TMK的SEK密钥索引
           iSekWorkKeyIndex   加密工作密钥的SEK密钥索引
** 输出参数:
           tInterface->szData   PIK密文(32Bytes)+MAK密文(32Bytes)+TMK密文(32Bytes)，工作密钥由指定SEK加密
** 返 回 值: 
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06UphsmChangeWorkKey( T_Interface *tInterface, int iSekTmkIndex, int iSekWorkIndex )
{
    char    szInData[1024], szOutData[1024], szRetCode[3], szTmk[33];
    char    szOutKey[256];
    int     iLen, iRet, iSndLen, i;

    memcpy( szTmk, tInterface->szData, 32 );
    for( i=1; i<=2; i++ )
    {
        iLen = 0;
        /*命令*/
        memcpy( szInData, "KI", 2 );    
        iLen += 2;
        /* sek1 */
        sprintf( szInData+iLen, "S%04ld", iSekWorkIndex ); 
        iLen += 5;
        /* sek2 */
        sprintf( szInData+iLen, "S%04ld", iSekTmkIndex); 
        iLen += 5;
        /* TMK密钥长度 */
        szInData[iLen] = 'Y';
        iLen ++;
        /* TMK密文 */
        memcpy( szInData+iLen, szTmk, 32 );
        iLen += 32;
        /* 转换标识 0：从SEK到TMK加密 1：从TMK到SEK加密 */
        szInData[iLen] = '1';
        iLen ++;
        /* PIK密钥长度 */
        szInData[iLen] = 'Y';
        iLen ++;
        /* PIK/MAC密文 */
        memcpy( szInData+iLen, tInterface->szData+32*i, 32 );
        iLen += 32;
        szInData[iLen] = 0;
        memset( szOutData, 0, 1024 );
        iRet = CommuWithHsm( szInData, iLen, szOutData ); 
        if( iRet == FAIL )
        {
            WriteLog( ERROR, "commu with hsm fail" );
            return FAIL;
        }
        if( memcmp(szOutData, "KJ", 2) != 0  ||
            memcmp(szOutData+2, "00", 2) != 0 )
        {
            DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
            WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
            return SUCC;
        }

        memcpy( szOutKey+32*(i-1), szOutData+4, 32 );
    }
    memcpy( tInterface->szData, szOutKey, 64 );
    memcpy( tInterface->szData+64, szTmk, 32 );
    tInterface->iDataLen = 96;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}
