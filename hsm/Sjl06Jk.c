/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:SJL06金卡指令集
           
** 创 建 人:Robin 
** 创建日期:2009/08/29


$Revision: 1.12 $
$Log: Sjl06Jk.c,v $
Revision 1.12  2012/12/26 01:44:17  wukj
%s/commu_with_hsm/CommuWithHsm/g

Revision 1.11  2012/12/05 06:32:13  wukj
*** empty log message ***

Revision 1.10  2012/12/03 03:24:46  wukj
int类型前缀修改为i

Revision 1.9  2012/11/29 07:51:43  wukj
修改日志函数,修改ascbcd转换函数

Revision 1.8  2012/11/29 01:57:55  wukj
日志函数修改

Revision 1.7  2012/11/21 04:13:38  wukj
修改hsmincl.h 为hsm.h

Revision 1.6  2012/11/21 03:20:31  wukj
1:加密机类型名字修改 2: 全局变量移至hsmincl.h


*******************************************************************/

#include "hsm.h"

/*****************************************************************
** 功    能:加密机随机生成终端主密钥TMK，并将其密文和检查值返回给主机。
** 输入参数:
           无
** 输出参数:
           tInterface->szData  TMK(存放中心32Bytes)+TMK(分发给终端32Bytes)+CheckValue(4Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkGetTmk( T_Interface *tInterface, int iSekTmkIndex, int iTekIndex )
{
    char    szInData[1024], szOutData[1024], szRetCode[3];
    int     iLen, iRet, iSndLen, i;

    iLen = 0;
    /*命令*/
    memcpy( szInData, "11", 2 );    
    iLen += 2;
    /* sek */
    sprintf( szInData+iLen, "%03ld", iSekTmkIndex ); 
    iLen += 3;
    szInData[iLen] = 0;
    memset( szOutData, 0, 1024 );
    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( memcmp(szOutData, "12", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    /* SEK加密的密钥 */
    memcpy( tInterface->szData, szOutData+4, 32 );
    memcpy( tInterface->szData+32, szOutData+4, 32 );
    /* 密钥校验值 */
    memcpy( tInterface->szData+64, "0000", 4 );
    tInterface->iDataLen = 68;
    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能:生成数据密钥PIK/MAK，并将其密文和检查值返回给主机。
** 输入参数:
           tInterface->szData  终端主密钥密文(32)
           iTempBMKIndex     指定的银行主密钥索引,用于加密生成终端工作密钥
           iSekWorkIndex     本地配置,用于加密工作密钥
** 输出参数:
           tInterface->szData  PIK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)+
                             MAK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(168Bytes)+
                             MAG(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkGetWorkKey(T_Interface *tInterface, int iSekTmkIndex,
                int iSekWorkIndex, int iTempBMKIndex )
{
    char    szInData[1024], szOutData[1024], szPsamNo[17], szRand[33];
    char    szEnTmk[33], szChkVal[17];
    int     iLen, iRet, iSndLen, i;
    char    cChr;

    memcpy( szEnTmk, tInterface->szData, 32 );

    /* 将终端主密钥存放在银行主密钥区域的指定索引，用于加密生成终端工作密钥 */
    iLen = 0;
    /*命令*/
    memcpy( szInData, "2A", 2 );    
    iLen += 2;
    /* 指定银行主密钥索引 */
    sprintf( szInData+iLen, "%03ld", iTempBMKIndex ); 
    iLen += 3;

    /*密钥标识 无或Y，128bit; Z，192bit
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;
    */

    /*终端主密钥密文*/
    memcpy( szInData+iLen, szEnTmk, 32 );
    iLen += 32;
    szInData[iLen] = 0;
    memset( szOutData, 0, 1024 );
    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( memcmp(szOutData, "2B", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }
    

    /* 产生工作密钥及校验值 */
    for( i=0; i<3; i++ )
    {
        /*==================产生由终端主密钥加密的工作密钥===================*/
        iLen = 0;
        /*命令*/
        memcpy( szInData, "16", 2 );    
        iLen += 2;
        /*密钥标识 无或X, 64bit; Y，128bit; Z，192bit*/
        memcpy( szInData+iLen, "Y", 1 );    
        iLen += 1;
        /*银行主密钥索引*/
        sprintf( szInData+iLen, "%03ld", iTempBMKIndex ); 
        iLen += 3;
        szInData[iLen] = 0;
        memset( szOutData, 0, 1024 );
        iRet = CommuWithHsm( szInData, iLen, szOutData ); 
        if( iRet == FAIL )
        {
            WriteLog( ERROR, "commu with hsm fail" );
            strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
            return FAIL;
        }
        if( memcmp(szOutData, "17", 2) != 0  ||
            memcmp(szOutData+2, "00", 2) != 0 )
        {
            DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
            WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
            return SUCC;
        }

        memcpy( tInterface->szData+i*80+32, szOutData+4, 32 );
        memcpy( szChkVal, szOutData+36, 16 );

        /*==================工作密钥由终端主密钥加密转换成本地主密钥加密(固定索引)===================*/
        iLen = 0;
        /*命令*/
        memcpy( szInData, "17", 2 );    
        iLen += 2;
        /*银行主密钥1索引*/
        sprintf( szInData+iLen, "%03ld", iTempBMKIndex ); 
        iLen += 3;
        /*银行主密钥2索引 --- 工作密钥加密密钥索引 */
        sprintf( szInData+iLen, "%03ld", iSekWorkIndex ); 
        iLen += 3;
        /*密钥标识 无或X, 64bit; Y，128bit; Z，192bit*/
        memcpy( szInData+iLen, "Y", 1 );    
        iLen += 1;
        /*工作密钥密文(终端主密钥加密)*/
        memcpy( szInData+iLen, szOutData+4, 32 );
        iLen += 32;
        szInData[iLen] = 0;
        memset( szOutData, 0, 1024 );
        iRet = CommuWithHsm( szInData, iLen, szOutData ); 
        if( iRet == FAIL )
        {
            WriteLog( ERROR, "commu with hsm fail" );
            strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
            return FAIL;
        }
        if( memcmp(szOutData, "18", 2) != 0  ||
            memcmp(szOutData+2, "00", 2) != 0 )
        {
            DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
            WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
            return SUCC;
        }

        memcpy( tInterface->szData+i*80, szOutData+4, 32 );
        memcpy( tInterface->szData+i*80+64, szChkVal, 16 );
    }

    tInterface->iDataLen = 240;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:用ANSI X9.9 MAC算法对数据做MAC。 
** 输入参数:
           tInterface->szData 参与MAC运算的数据，长度由data_len指定
           iSekIndex        存储加密密钥索引
** 输出参数:
           tInterface->szData MAC(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkCalcMac(T_Interface *tInterface, int iSekIndex)
{
    char    szInData[2048], szOutData[2048], szMacData[17];
    int     iLen, iRet, iSndLen;

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "80", 2 );    
    iLen += 2;
    /*存储加密密钥索引*/
    sprintf( szInData+iLen, "%03ld", iSekIndex ); 
    iLen += 3;

    /*密钥标识 无或X, 64bit; Y，128bit; Z，192bit*/
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;

    /*MAC密钥密文*/
    BcdToAsc( (uchar *)(tInterface->szMacKey), 32, 0 , (uchar *)(szInData+iLen));
    iLen += 32;
    
    if( tInterface->iAlog == XOR_CALC_MAC )
    {
        XOR( tInterface->szData, tInterface->iDataLen, szMacData );
        sprintf( szInData+iLen, "%03ld", 8 );
        iLen += 3;

        memcpy( szInData+iLen, szMacData, 8 );
        iLen += 8;
    }
    else
    {
        sprintf( szInData+iLen, "%03ld", tInterface->iDataLen );
        iLen += 3;

        memcpy( szInData+iLen, tInterface->szData, tInterface->iDataLen );
        iLen += tInterface->iDataLen;
    }
    
    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( memcmp(szOutData, "81", 2) != 0 ||
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
** 功    能:将源PIN密文用源PIK解密，进行PIN格式转换，然后用目的PIK加密输出。
** 输入参数:
           tInterface->szData 源帐号(16字节)+密码密文(8字节)+目的帐号(16字节)
           iSekPosIndex     加密源PIK的存储加密密钥索引
           iSekHostIndex    加密目的PIK的存储加密密钥索引
** 输出参数:
           tInterface->szData 转加密后的密码密文(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkChangePin(T_Interface *tInterface, int iSekPosIndex, int iSekHostIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szTargetPan[17];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "0000%12.12s", tInterface->szData+3 );
    szPanBlock[16] = 0;

    sprintf( szTargetPan, "0000%12.12s", tInterface->szData+27 );
    szTargetPan[16] = 0;

    iLen = 0;
    memcpy( szInData, "69", 2 );    /* 命令 */
    iLen += 2;

    /* 加密源PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "%03ld", iSekPosIndex );
    iLen += 3;

    /*密钥标识 无或X, 64bit; Y，128bit; Z，192bit*/
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;

    /* 源PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szPinKey),32, 0 , (uchar *)(szInData+iLen));    
    iLen += 32;

    /* 加密目的PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "%03ld", iSekHostIndex );
    iLen += 3;

    /*密钥标识 无或X, 64bit; Y，128bit; Z，192bit*/
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;

    /* 目的PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szMacKey), 32, 0 , (uchar *)(szInData+iLen));    
    iLen += 32;

    /* 源PinBlock密文 */
    BcdToAsc( (uchar *)(tInterface->szData+16), 16, 0 , (uchar *)(szInData+iLen));    
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

    if( memcmp(szOutData, "6A", 2) != 0 ||
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
** 功    能:验证终端上送的PIN是否与数据库中的PIN一致
            分2步:
            1、用PIK对PIN明文加密
            2、与终端上送的密文进行比较
** 输入参数:
           tInterface->szData 数据库中密码明文(8字节)+终端PIN密文(8字节)
           iSekPosIndex     加密PIK的存储加密密钥索引
** 输出参数:
           tInterface->szData SUCC-一致  FAIL-不一致
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkVerifyPin(T_Interface *tInterface, int iSekPosIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szEncPin[17];
    int     iLen, iRet, iSndLen;

    memset( szPanBlock, '0', 16 );
    szPanBlock[16] = 0;

    iLen = 0;
    memcpy( szInData, "60", 2 );    /* 命令 */
    iLen += 2;

    /* 加密PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "%03ld", iSekPosIndex );
    iLen += 3;

    /*PIN类型 ANSIX9.8算法*/ 
    memcpy( szInData+iLen, "3", 1 );    
    iLen += 1;

    /*密钥标识 无或X, 64bit; Y，128bit; Z，192bit*/
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;

    /* PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szPinKey), 32, 0 , (uchar *)(szInData+iLen));    
    iLen += 32;

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

    AscToBcd( (uchar *)(szOutData+4), 16, 0 , (uchar *)szEncPin);
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
** 功    能:生成密钥的校验值
** 输入参数:
           tInterface->szData 密钥密文(32字节)
           iSekIndex        加密PIK的存储加密密钥索引
** 输出参数:
           tInterface->szData 校验值(16)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkCalcChkval(T_Interface *tInterface, int iSekIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17];
    int     iLen, iRet, iSndLen;

    iLen = 0;
    memcpy( szInData, "13", 2 );    /* 命令 */
    iLen += 2;

    /* 加密PIK的存储加密密钥索引 */
    sprintf( szInData+iLen, "%03ld", iSekIndex );
    iLen += 3;

    /*密钥标识 无或X, 64bit; Y，128bit; Z，192bit*/
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;

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

    if( memcmp(szOutData, "14", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm encrypt pin fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData, szOutData+4, 4 );
    memcpy( tInterface->szData+4, "0000", 4 );
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:加密机转加密工作密钥，即将由TMK加密的工作密钥还原成明文，然后再用指定的SEK加密
** 输入参数:
           tInterface->szData       TMK密文(32Bytes)+PIK密文(32Bytes)+MAK密文(32Bytes)，工作密钥由TMK加密
           iSekTmkIndex           加密TMK的SEK密钥索引
           iSekWorkIndex          加密工作密钥的SEK密钥索引
** 输出参数:
           tInterface->szData       PIK密文(32Bytes)+MAK密文(32Bytes)+TMK密文(32Bytes)，工作密钥由指定SEK加密
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkChangeWorkKey( T_Interface *tInterface, int iSekTmkIndex, int iSekWorkIndex, int iTempBMKIndex )
{
    char    szInData[1024], szOutData[1024], szRetCode[3], szTmk[33];
    char    szOutKey[256];
    int     iLen, iRet, iSndLen, i;

    memcpy( szTmk, tInterface->szData, 32 );

    /* 将终端主密钥存放在银行主密钥区域的指定索引(当作银行主密钥1使用)，用于转加密终端工作密钥 */
    iLen = 0;
    /*命令*/
    memcpy( szInData, "2A", 2 );    
    iLen += 2;
    /* 指定银行主密钥索引 */
    sprintf( szInData+iLen, "%03ld", iTempBMKIndex ); 
    iLen += 3;
    /*密钥标识 无或Y，128bit; Z，192bit*/
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;
    /*终端主密钥密文*/
    memcpy( szInData+iLen, szTmk, 32 );
    iLen += 32;
    szInData[iLen] = 0;
    memset( szOutData, 0, 1024 );
    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( memcmp(szOutData, "2B", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispUphsmErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    for( i=1; i<=2; i++ )
    {
        iLen = 0;
        /*命令*/
        memcpy( szInData, "17", 2 );    
        iLen += 2;
        /* sek1 */
        sprintf( szInData+iLen, "%03ld", iTempBMKIndex ); 
        iLen += 3;
        /* sek2 */
        sprintf( szInData+iLen, "%03ld", iSekWorkIndex); 
        iLen += 3;
        /*密钥标识 无或Y，128bit; Z，192bit*/
        memcpy( szInData+iLen, "Y", 1 );    
        iLen += 1;
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
        if( memcmp(szOutData, "18", 2) != 0  ||
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

/*****************************************************************
** 功    能:存储一个索引的银行主密钥
** 输入参数:
            iKeyIndex   存储索引
            szEnBMK     密钥值
** 输出参数:
            无
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06JkSaveBMK( int iKeyIndex, char *szEnBMK )
{
    char    szInData[1024], szOutData[1024];
    int     iLen, iRet;

    /* 将终端主密钥存放在银行主密钥区域的指定索引，用于加密生成终端工作密钥 */
    iLen = 0;
    /*命令*/
    memcpy( szInData, "2A", 2 );    
    iLen += 2;
    /* 指定银行主密钥索引 */
    sprintf( szInData+iLen, "%03ld", iKeyIndex ); 
    iLen += 3;

    /*密钥标识 无或Y，128bit; Z，192bit
    memcpy( szInData+iLen, "Y", 1 );    
    iLen += 1;
    */

    /*终端主密钥密文*/
    memcpy( szInData+iLen, szEnBMK, 32 );
    iLen += 32;
    szInData[iLen] = 0;
    memset( szOutData, 0, 1024 );
    iRet = CommuWithHsm( szInData, iLen, szOutData ); 
    if( iRet == FAIL )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( memcmp(szOutData, "2B", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }
    
    return SUCC;
}
