/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:成都卫士通硬件加密机加密接口
            加密机版本号:WT101C-v1.9.4-JRIC1-060707
** 创 建 人:Robin 
** 创建日期:2009/08/29


$Revision: 1.11 $
$Log: sjl05.c,v $
Revision 1.11  2012/12/10 07:52:07  wukj
ANS相关函数调用修正

Revision 1.10  2012/12/05 06:32:14  wukj
*** empty log message ***

Revision 1.9  2012/12/03 03:24:46  wukj
int类型前缀修改为i

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
** 功    能:加密机随机生成终端主密钥TMK,并将其密文(用ZMK加密)和检查值返回给主机
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
int Sjl05GetTmk0404( T_Interface *tInterface, int iSekTmkIndex, int iTekIndex )
{
    char    szInData[1024], szOutData[1024], szTmpStr[100];
    int     iLen, iRet, iSndLen, i;

    iLen = 0;
    /*命令*/
    memcpy( szInData, "\x04\x04", 2 );    
    iLen += 2;
    /*区域主密钥索引号,用于加密终端主密钥*/
    sprintf( szTmpStr, "%04x", iSekTmkIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen)); 
    iLen += 2;
    /*加密算法,3DES*/
    szInData[iLen] = 0x00;
    /*密钥长度*/
    szInData[iLen] = 8;
    iLen ++;
    szInData[iLen] = 0;
    memset( szOutData, 0, 1024 );
    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
        return SUCC;
    }

    BcdToAsc((uchar *)szOutData+2, 32, 0 , (uchar *)tInterface->szData);
    BcdToAsc((uchar *)szOutData+18, 4, 0 , (uchar *)(tInterface->szData+32));
    tInterface->iDataLen = 36;
    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能:加密机随机生成终端主密钥TMK,并将其密文(用ZMK加密)和检查值返回给主机
** 输入参数:
           无
** 输出参数:
           tInterface->szData  TMK(存放中心32Bytes)+CheckValue(4Bytes) 
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05GetTmk( T_Interface *tInterface, int iSekTmkIndex, int iTekIndex )
{
    char    szInData[1024], szOutData[1024], szTmpStr[100];
    int     iLen, iRet, iSndLen, i;

    iLen = 0;
    /*命令*/
    memcpy( szInData, "\x04\x20", 2 );    
    iLen += 2;
    /*区域主密钥1索引号,用于加密终端主密钥*/
    sprintf( szTmpStr, "%04x", iSekTmkIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0,(uchar *)(szInData+iLen) ); 
    iLen += 2;
    /*区域主密钥2索引号,用于加密终端主密钥*/
    sprintf( szTmpStr, "%04x", iTekIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen)); 
    iLen += 2;
    /*密钥长度*/
    szInData[iLen] = 1;
    iLen ++;
    szInData[iLen] = 0;
    memset( szOutData, 0, 1024 );
    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
        return SUCC;
    }

    BcdToAsc((uchar *)szOutData+1, 68, 0 ,(uchar *)tInterface->szData);
    tInterface->iDataLen = 68;
    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能: 生成数据密钥PIK/MAK/MGK,并将其密文和检查值返回给主机
** 输入参数:
           tInterface->szData 终端主密钥密文(32)
** 输出参数:
           tInterface->szData 
                PIK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)+
                MAC(存放中心32Bytes)+MAC(下传POS32Bytes)+CheckValue(168Bytes) 
                MAG(存放中心32Bytes)+MAG(下传POS32Bytes)+CheckValue(16Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05GetWorkKey(T_Interface *tInterface, int iSekTmkIndex, 
        int iSekWorkIndex )
{
    char    szInData[1024], szOutData[1024], szTmpStr[100];
    char    szEnTmk[33];
    int     iLen, iRet, iSndLen, i;
    char    cChr;

    memcpy( szEnTmk, tInterface->szData, 32 );
    
    /* 一次生成两个工作密钥,由于需要三个工作密钥,因此需要调用2次 */
    for( i=0; i<2; i++ )
    {
        iLen = 0;
        /*命令*/
        memcpy( szInData, "\x04\x21", 2 );    
        iLen += 2;
        /*用于加密工作密钥的区域主密钥1*/
        sprintf( szTmpStr, "%04x", iSekWorkIndex );
        AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen)); 
        iLen += 2;
        /*用于解密终端主密钥的区域主密钥2*/
        sprintf( szTmpStr, "%04x", iSekTmkIndex );
        AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen)); 
        iLen += 2;
        /*数据密钥1长度*/
        szInData[iLen] = 1;            
        iLen ++;
        /*数据密钥2长度*/
        szInData[iLen] = 1;            
        iLen ++;
        /*终端主密钥长度*/
        szInData[iLen] = 1;            
        iLen ++;
        /*终端主密钥密文*/
        AscToBcd( (uchar *)szEnTmk, 32, 0 ,(uchar *)(szInData+iLen));    
        iLen += 16;
        szInData[iLen] = 0;
        memset( szOutData, 0, 1024 );
        iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
        if( iRet != SUCC )
        {
            WriteLog( ERROR, "commu with hsm fail" );
            strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
            return FAIL;
        }
        if( szOutData[0] == 'E' )
        {
            DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
            WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
            return SUCC;
        }

        if( i == 0 )
        {
            BcdToAsc(szOutData+1, 160, 0 ,tInterface->szData);
        }
        else
        {
            BcdToAsc(szOutData+1,  80, 0 ,tInterface->szData+160);
        }
    }

    tInterface->iDataLen = 240;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}
/*****************************************************************
** 功    能:生成数据密钥PIK/MAK/MGK,并将其密文和检查值返回给主机
** 注    意:密钥长度为16位,但左8位与右8位一样,以便到福建工行中间业务平台可以调用
            Sjl05ChangePin_PIK2TMK()函数
** 输入参数:
           tInterface->szData 终端主密钥密文(32)
** 输出参数:
           tInterface->szData 
                PIK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)+
                MAC(存放中心32Bytes)+MAC(下传POS32Bytes)+CheckValue(168Bytes) 
                MAG(存放中心32Bytes)+MAG(下传POS32Bytes)+CheckValue(16Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05GetWorkKey_FJICBC(T_Interface *tInterface, int iSekTmkIndex, 
        int iSekWorkIndex )
{
    char    szInData[1024], szOutData[1024], szTmpStr[100];
    char    szEnTmk[33];
    int     iLen, iRet, iSndLen, i;
    char    cChr;

    memcpy( szEnTmk, tInterface->szData, 32 );
    
    /* 一次生成两个工作密钥,由于需要三个工作密钥,因此需要调用2次 */
    for( i=0; i<2; i++ )
    {
        iLen = 0;
        /*命令*/
        memcpy( szInData, "\x04\x21", 2 );    
        iLen += 2;
        /*用于加密工作密钥的区域主密钥1*/
        sprintf( szTmpStr, "%04x", iSekWorkIndex );
        AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen)); 
        iLen += 2;
        /*用于解密终端主密钥的区域主密钥2*/
        sprintf( szTmpStr, "%04x", iSekTmkIndex );
        AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen)); 
        iLen += 2;
        /*数据密钥1长度*/
        szInData[iLen] = 0;            
        iLen ++;
        /*数据密钥2长度*/
        szInData[iLen] = 0;            
        iLen ++;
        /*终端主密钥长度*/
        szInData[iLen] = 1;            
        iLen ++;
        /*终端主密钥密文*/
        AscToBcd( (uchar *)szEnTmk, 32, 0,(uchar *)(szInData+iLen) );    
        iLen += 16;
        szInData[iLen] = 0;
        memset( szOutData, 0, 1024 );
        iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
        if( iRet != SUCC )
        {
            WriteLog( ERROR, "commu with hsm fail" );
            strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
            return FAIL;
        }
        if( szOutData[0] == 'E' )
        {
            DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
            WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
            return SUCC;
        }

        if( i == 0 )
        {
            //数据密钥1的密文1左8位(由区域主密钥1加密)
            BcdToAsc(  szOutData+1, 16, 0 ,tInterface->szData);

            //数据密钥1的密文1右8位(由区域主密钥1加密)
            BcdToAsc( szOutData+1, 16, 0 ,tInterface->szData+16);

            //数据密钥1的密文2左8位(由终端主密钥加密)
            BcdToAsc( szOutData+9, 16, 0 ,tInterface->szData+32);

            //数据密钥1的密文2右8位(由终端主密钥加密)
            BcdToAsc( szOutData+9, 16, 0 ,tInterface->szData+48);

            //数据密钥1的chkval
            BcdToAsc( szOutData+17, 16, 0 ,tInterface->szData+64);


            //数据密钥2的密文1左8位(由区域主密钥1加密)
            BcdToAsc( szOutData+25, 16, 0 ,tInterface->szData+80);

            //数据密钥2的密文1右8位(由区域主密钥1加密)
            BcdToAsc( szOutData+25, 16, 0 ,tInterface->szData+96);

            //数据密钥2的密文2左8位(由终端主密钥加密)
            BcdToAsc( szOutData+33, 16, 0 ,tInterface->szData+112);

            //数据密钥2的密文2右8位(由终端主密钥加密)
            BcdToAsc( szOutData+33, 16, 0 ,tInterface->szData+128);

            //数据密钥2的chkval
            BcdToAsc( szOutData+41, 16, 0 ,tInterface->szData+144);

        }
        else
        {
            //数据密钥1的密文1左8位(由区域主密钥1加密)
            BcdToAsc( szOutData+1, 16, 0 ,tInterface->szData+160);

            //数据密钥1的密文1右8位(由区域主密钥1加密)
            BcdToAsc( szOutData+1, 16, 0,tInterface->szData+176 );

            //数据密钥1的密文2左8位(由终端主密钥加密)
            BcdToAsc( szOutData+9, 16, 0 ,tInterface->szData+192);

            //数据密钥1的密文2右8位(由终端主密钥加密)
            BcdToAsc( szOutData+9, 16, 0 ,tInterface->szData+208);

            //数据密钥1的chkval
            BcdToAsc( szOutData+17, 16, 0 ,tInterface->szData+224);
        }
    }

    tInterface->iDataLen = 240;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:用ANSI X9.9 MAC算法对数据做MAC 
** 输入参数:
           tInterface->szData 参与MAC运算的数据,长度由data_len指定
** 输出参数:
           tInterface->szData MAC(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05CalcMac(T_Interface *tInterface, int iSekIndex)
{
    char    szInData[2048], szOutData[2048], szTmpStr[100];
    int     iLen, iRet, iSndLen;

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x04\x10", 2 );    
    iLen += 2;

    /*传输主密钥类型 1-区域主密钥 2-终端主密钥 0-本地主密钥*/
    szInData[iLen] = 1;
    iLen ++;

    /*传输主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen)); 
    iLen += 2;

    /*MAC密钥长度*/
    szInData[iLen] = 16;
    iLen ++;
    
    /* MAC算法 1-X9.9 2-X9.19 3-XOR*/
    if( tInterface->iAlog == X99_CALC_MAC )
    {
        szInData[iLen] = 1;        
    }
    else if( tInterface->iAlog == X919_CALC_MAC )
    {
        szInData[iLen] = 2;
    }
    else
    {
        szInData[iLen] = 3;
    }
    iLen += 1;

    /*MAC密钥密文*/
    memcpy( szInData+iLen, tInterface->szMacKey, 16 );    
    iLen += 16;

    /*初始向量*/
    memcpy( szInData+iLen, "\x00\x00\x00\x00\x00\x00\x00\x00", 8 );    
    iLen += 8;

    /*数据长度*/
    sprintf( szTmpStr, "%04x", tInterface->iDataLen );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /*数据*/
       memcpy( szInData+iLen, tInterface->szData, tInterface->iDataLen );
    iLen += tInterface->iDataLen;
    szInData[iLen] = 0;
    
    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }

       memcpy( tInterface->szData, szOutData+1, 8 );
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:将源PIN密文用源PIK解密,进行PIN格式转换,然后用目的PIK加密输出
** 注    意:当第1帐号为全0时,转加密不对,存在BUG,建议采用0406指令
** 输入参数:
           tInterface->szData 源帐号(16字节)+密码密文(8字节)+目的帐号(16字节)
** 输出参数:
           tInterface->szData 转加密后的密码密文(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05ChangePin0402(T_Interface *tInterface, int iSekPosIndex, int iSekHostIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szTargetPan[17], szTmpStr[100];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "0000%12.12s", tInterface->szData+3 );
    szPanBlock[16] = 0;

    /*加密机的一个BUG*/
    if( memcmp( szPanBlock, "0000000000000000", 16 ) == 0 )
    {
        sprintf( szTargetPan, "000%13.13s", tInterface->szData+27 );
        szTargetPan[16] = 0;
    }
    else
    {
        sprintf( szTargetPan, "0000%12.12s", tInterface->szData+27 );
        szTargetPan[16] = 0;
    }

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x04\x02", 2 );    
    iLen += 2;

    /* 加密源PIK的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekPosIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0,(uchar *)(szInData+iLen) );
    iLen += 2;

    /* 源PIK加密算法 */
    szInData[iLen] = 1;    
    iLen ++;

    /* 源PIK密钥长度 */
    szInData[iLen] = 16;    
    iLen ++;

    /* 源PIK密钥密文 */
    memcpy( szInData+iLen, tInterface->szPinKey, 16 );
    iLen += 16;

    /* 源PinBlock加密算法 */
    szInData[iLen] = 1;    
    iLen ++;

    /* 源PinBlock密文 */
    memcpy( szInData+iLen, tInterface->szData+16, 8 ); 
    iLen += 8;

    /* 源帐号 */
    AscToBcd( szPanBlock, 16, 0, (uchar *)(szInData+iLen));    
    iLen += 8;

    /* 加密目的PIK的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekHostIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 目的PIK加密算法 */
    szInData[iLen] = 1;    
    iLen ++;

    /* 目的PIK密钥长度 */
    szInData[iLen] = 16;    
    iLen ++;

    /* 目的PIK密钥密文 */
    memcpy( szInData+iLen, tInterface->szMacKey, 16 );
    iLen += 16;

    /* 目的PinBlock加密算法 */
    szInData[iLen] = 1;    
    iLen ++;

    /* 目的帐号 */
    AscToBcd( szTargetPan, 16, 0 ,(uchar *)(szInData+iLen));    
    iLen += 8;

    szInData[iLen] = 0;

    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }

       memcpy( tInterface->szData, szOutData+1, 8 );
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 将源PIN密文用源PIK解密,进行PIN格式转换,然后用目的PIK加密输出
** 输入参数:
           tInterface->szData 源帐号(16字节)+密码密文(8字节)+目的帐号(16字节)
** 输出参数:
           tInterface->szData 转加密后的密码密文(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05ChangePin(T_Interface *tInterface, int iSekPosIndex, int iSekHostIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szTargetPan[17], szTmpStr[100];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "%16.16s", tInterface->szData );
    szPanBlock[16] = 0;

    sprintf( szTargetPan, "%16.16s", tInterface->szData+24 );
    szTargetPan[16] = 0;

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x04\x06", 2 );    
    iLen += 2;

    /* 加密源PIK的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekPosIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 源PIK密钥长度 */
    szInData[iLen] = 16;    
    iLen ++;

    /* 源PIK密钥密文 */
    memcpy( szInData+iLen, tInterface->szPinKey, 16 );
    iLen += 16;

    /* 加密目的PIK的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekHostIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0,(uchar *)(szInData+iLen) );
    iLen += 2;

    /* 目的PIK密钥长度 */
    szInData[iLen] = 16;    
    iLen ++;

    /* 目的PIK密钥密文 */
    memcpy( szInData+iLen, tInterface->szMacKey, 16 );
    iLen += 16;

    /* 源PinBlock格式 */
    szInData[iLen] = 1;    
    iLen ++;

    /* 目的PinBlock格式 */
    szInData[iLen] = 1;    
    iLen ++;

    /* 源PinBlock密文 */
    memcpy( szInData+iLen, tInterface->szData+16, 8 ); 
    iLen += 8;

    /* 源帐号 */
    memcpy( szInData+iLen, szPanBlock, 16 );    
    iLen += 16;

    /* 分隔符 */
    szInData[iLen] = ';';    
    iLen ++;

    /* 目的帐号 */
    memcpy( szInData+iLen, szTargetPan, 16 );    
    iLen += 16;

    /* 分隔符 */
    szInData[iLen] = ';';    
    iLen ++;

    szInData[iLen] = 0;

    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }

       memcpy( tInterface->szData, szOutData+1, 8 );
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}
/*****************************************************************
** 功    能: 验证终端上送的PIN是否与数据库中的PIN一致
             分2步:
             1、用PIK对PIN明文加密
             2、与终端上送的密文进行比较
** 输入参数:
           tInterface->szData  数据库中密码明文(8字节)+终端PIN密文(8字节)
** 输出参数:
           tInterface->szData  SUCC-一致  FAIL-不一致
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05VerifyPin(T_Interface *tInterface, int iSekPosIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szPinBlock[17], szEncPin[17], szTmpStr[100];
    int     iLen, i, iRet, iSndLen;

    memset( szPanBlock, 0x00, 8 );

    sprintf( szTmpStr, "08%8.8sFFFFFF", tInterface->szData );
    AscToBcd( szTmpStr, 16, 0 ,(uchar *)szPinBlock);
    
    for( i=0; i<8; i++ )
    {
        szTmpStr[i] = szPinBlock[i]^szPanBlock[i];
    }
    memcpy( szPinBlock, szTmpStr, 8 );

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x04\x05", 2 );    
    iLen += 2;

    /* 加密PIK的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekPosIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0,(uchar *)(szInData+iLen) );
    iLen += 2;

    /* PIK加密算法 */
    szInData[iLen] = 1;    
    iLen ++;

    /* PIK密钥长度 */
    szInData[iLen] = 16;    
    iLen ++;

    /* PIK密钥密文 */
    memcpy( szInData+iLen, tInterface->szPinKey, 16 );
    iLen += 16;

    /* PinBlock */
    memcpy( szInData+iLen, szPinBlock, 8 );
    iLen += 8;

    szInData[iLen] = 0;

    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }

       memcpy( szEncPin, szOutData+1, 8 );
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
** 功    能: 解密PIN
** 输入参数:
           tInterface->szData   帐号(16字节)+终端PIN密文(8字节)
** 输出参数:
           tInterface->szData   密码明文
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05DecryptPin(T_Interface *tInterface, int iSekPosIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szBcdPanBlock[17], szEncPin[17], szTmpStr[100];
    int     iLen, i, iRet, iSndLen;

    sprintf( szPanBlock, "0000%12.12s", tInterface->szData+3 );
    AscToBcd( (uchar *)szPanBlock, 16, 0 , (uchar *)szBcdPanBlock);

    memcpy( szEncPin, tInterface->szData+16, 8 );
    
    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x04\x22", 2 );    
    iLen += 2;

    /* 加密PIK的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekPosIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 密码密文 */
    memcpy( szInData+iLen, szEncPin, 8 );
    iLen += 8;

    /* 主帐号 */
    memcpy( szInData+iLen, szBcdPanBlock, 8 );
    iLen += 8;

    /* PIK长度 */
    memcpy( szInData+iLen, "\x10", 1 );    
    iLen += 1;

    /* PIK密钥密文 */
    memcpy( szInData+iLen, tInterface->szPinKey, 16 );
    iLen += 16;

    szInData[iLen] = 0;

    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }

    iLen = (uchar)szOutData[1];
    if( iLen < 4 || iLen > 8 )
    {
        WriteLog( ERROR, "pin_len error 解密密码长度错误" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }
    BcdToAsc( szOutData+2, 14, 0 ,(uchar *)szTmpStr);
    memcpy( tInterface->szData, szTmpStr, iLen );
    tInterface->szData[iLen] = 0;
    tInterface->iDataLen = iLen;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 生成密钥的校验值
** 输入参数:
           tInterface->szData  密钥密文(32字节)
** 输出参数:
           tInterface->szData  校验值(16)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05CalcChkval(T_Interface *tInterface, int iSekIndex)
{
    char    szInData[1024], szOutData[1024], szTmpStr[100];
    int     iLen, iRet, iSndLen;

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x04\x23", 2 );    
    iLen += 2;

    /* 加密PIK的存储加密密钥索引 */
    sprintf( szTmpStr, "%04x", iSekIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 密钥长度 */
    szInData[iLen] = 16;    
    iLen ++;

    /* 密钥密文 */
    AscToBcd( (uchar *)tInterface->szData, 32, 0 ,(uchar *)(szInData+iLen));
    iLen += 16;

    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }

    BcdToAsc( (uchar *)(szOutData+1), 8, 0 , (uchar *)tInterface->szData);
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 加密机转加密工作密钥,即将由TMK加密的工作密钥还原成明文,然后再用指定的SEK加密
** 输入参数:
           tInterface->szData    TMK密文(32Bytes)+PIK密文(32Bytes)+MAK密文(32Bytes),工作
           iSekTmkIndex        加密TMK的SEK密钥索引
           iSekWorkKeyIndex    加密工作密钥的SEK密钥索引
** 输出参数:
           tInterface->szData    PIK密文(32Bytes)+MAK密文(32Bytes)+TMK密文(32Bytes),工作密钥由指定SEK加密
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05ChangeWorkKey( T_Interface *tInterface, int iSekTmkIndex, int iSekWorkIndex )
{
    char    szInData[1024], szOutData[1024], szTmk[33], szEnKey[256];
    char    szOutKey[256], szTmpStr[100];
    int     iLen, iRet, iSndLen, i;

    AscToBcd( (uchar *)tInterface->szData, 32, 0 ,(uchar *)szTmk);

    AscToBcd( (uchar *)tInterface->szData+32, 64, 0,(uchar *)szEnKey );

    /* =================用终端主密钥解密数据密钥====================== */
    iLen = 0;
    /*命令*/
    memcpy( szInData, "\x71", 1 );    
    iLen += 1;

    /* 加密终端主密钥的区域主密钥索引 */
    sprintf( szTmpStr, "%04x", iSekTmkIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* TMK密文 */
    memcpy( szInData+iLen, szTmk, 16 );
    iLen += 16;

    /* 初始向量 */
    memcpy( szInData+iLen, "\x00\x00\x00\x00\x00\x00\x00\x00", 8 );    
    iLen += 8;

    /* 加解密标志,1-加密 0-解密 */
    szInData[iLen] = 0;
    iLen += 1;

    /* 算法标识 */
    szInData[iLen] = 0;
    iLen += 1;

    /* 数据密钥长度 */
    sprintf( szTmpStr, "%04x", 32 );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 数据密钥密文 */
    memcpy( szInData+iLen, szEnKey, 32 );
    iLen += 32;
    szInData[iLen] = 0;

    memset( szOutData, 0, 1024 );
    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
        return SUCC;
    }
    memcpy( szOutKey, szOutData+3, 32 );


    /* ===================用区域主密钥加密数据密钥==================== */
    iLen = 0;
    /*命令*/
    memcpy( szInData, "\x72", 1 );    
    iLen += 1;

    /* 加密数据密钥的区域主密钥 */
    sprintf( szTmpStr, "%04x", iSekWorkIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 初始向量 */
    memcpy( szInData+iLen, "\x00\x00\x00\x00\x00\x00\x00\x00", 8 );    
    iLen += 8;

    /* 加解密标志,1-加密 0-解密 */
    szInData[iLen] = 1;
    iLen += 1;

    /* 算法标识 */
    szInData[iLen] = 0;
    iLen += 1;

    /* 数据密钥长度 */
    sprintf( szTmpStr, "%04x", 32 );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 数据密钥明文 */
    memcpy( szInData+iLen, szOutKey, 32 );
    iLen += 32;
    szInData[iLen] = 0;

    memset( szOutData, 0, 1024 );
    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
        return SUCC;
    }

    BcdToAsc(szOutData+3, 64, 0 ,(uchar *)tInterface->szData);
    BcdToAsc( szTmk, 32, 0 ,(uchar *)tInterface->szData+64);
    tInterface->iDataLen = 96;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能: 将源PIN密文用源PIK解密,进行PIN格式转换,然后用TMK加密输出转换PIN从PIK加密到TMK加密
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
int Sjl05ChangePin_PIK2TMK(T_Interface *tInterface, int iSekPosIndex, int iTmkIndex)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szTargetPan[17], szTmpStr[100];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "%16.16s", tInterface->szData );
    szPanBlock[16] = 0;

    sprintf( szTargetPan, "%16.16s", tInterface->szData+24 );
    szTargetPan[16] = 0;

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x6E", 1 );    
    iLen += 1;

    /* 加密源PIK的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekPosIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0,(uchar *)(szInData+iLen) );
    iLen += 2;

    /* TMK密钥索引 */
    sprintf( szTmpStr, "%04x", iTmkIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)(szInData+iLen));
    iLen += 2;

    /* 源PIK密钥密文 */
    memcpy( szInData+iLen, tInterface->szPinKey, 8 );
    iLen += 8;

    /* 源PinBlock密文 */
    memcpy( szInData+iLen, tInterface->szData+16, 8 ); 
    iLen += 8;

    /* 源帐号 */
    AscToBcd( (uchar *)szPanBlock, 16, 0 ,(uchar *)(szInData+iLen));    
    iLen += 8;

    /* 目的帐号 */
    AscToBcd( (uchar *)szTargetPan, 16, 0 ,(uchar *)szInData+iLen);    
    iLen += 8;

    szInData[iLen] = 0;

    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }

       memcpy( tInterface->szData, szOutData+1, 8 );
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 解密磁道数据
** 输入参数:
           tInterface->szData    磁道密文
** 输出参数:
           tInterface->szData    磁道明文明文
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/29 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl05DecryptTrack(T_Interface *tInterface, int iSekPosIndex)
{
    char    szInData[1024], szOutData[1024], szTargetPan[17], szTmpStr[100];
    int     iLen ,iRet, iSndLen;
    unsigned char    szMagKey[17], szEnMagKey[17], szTrack2[40];
    char    szTrack3[105], szTmk[17];
    int     i, j, iMacAlog, iAlog, iLenTmp, iTrack2Len, iTrack3Len;
    char szBuf[200];
    memset(szBuf, 0, sizeof(szBuf));
    iAlog = tInterface->iAlog;
    
    memcpy( szEnMagKey, tInterface->szPinKey, 16 );

    iLen = 0;
    /* 命令 */
    memcpy( szInData, "\x71", 1 );    
    iLen += 1;

    /* 加密KEY的区域主密钥索引*/
    sprintf( szTmpStr, "%04x", iSekPosIndex );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)szInData+iLen);
    iLen += 2;
    /* 加解密的KEY密文*/
    
    memcpy( szInData+iLen, tInterface->szPinKey, 16 );
    iLen += 16;
    
    /*CBC加密的初始向量*/
    memcpy( szInData+iLen, "\x00\x00\x00\x00\x00\x00\x00\x00", 8 );
    iLen += 8;
    
    /*解密标识*/
    memcpy( szInData+iLen, "\x00", 1 ); 
    iLen += 1;
    /* 算法标识 */
    memcpy( szInData+iLen, "\x00", 1 ); 
    iLen += 1;
    /*处理数据的长度*/
    memset(szTmpStr, 0, sizeof(szTmpStr));
    sprintf( szTmpStr, "%04x", tInterface->iDataLen );
    AscToBcd( (uchar *)szTmpStr, 4, 0 ,(uchar *)szInData+iLen);
    iLen += 2;
    
    /*处理的数据*/
    memcpy( szInData+iLen, tInterface->szData, tInterface->iDataLen  );
    iLen += tInterface->iDataLen;
    

    szInData[iLen] = 0;
    iRet = CommuWithSjl05hsm( szInData, iLen, szOutData ); 
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "commu with hsm fail" );
        strcpy( tInterface->szReturnCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }
    if( szOutData[0] == 'E' )
    {
        DispSjl05ErrorMsg( szOutData+1, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%02x]", szOutData[1]&0xFF );
              return SUCC;
       }
       tInterface->iDataLen = szOutData[1]*256 + szOutData[2];
       memcpy( szTmpStr, szOutData+1+2, tInterface->iDataLen );
       iLenTmp = tInterface->iDataLen;
    

    BcdToAsc( szTmpStr, iLenTmp*2, 0 ,szBuf);
    //取第2磁道
    for( i=0; i<iLenTmp*2 && i<37; i++ )
    {
        if( szBuf[i] == 'D' )
        {
            szBuf[i] = '=';
        }
        else if( szBuf[i] == 'F' )
        {
            break;
        }
    }
    memcpy( szTrack2, szBuf, i );
    szTrack2[i] = 0;
    iTrack2Len = i;

    //跳过F
    for( j=i; j<iLenTmp*2; j++ )
    {
        if( szBuf[j] != 'F' )
            break;
    }

    //取第3磁道
    for( i=j; i<iLenTmp*2; i++ )
    {
        if( szBuf[i] == 'D' )
        {
            szBuf[i] = '=';
        }
        else if( szBuf[i] == 'F' )
        {
            break;
        }
    }
    memcpy( szTrack3, szBuf+j, i-j );
    szTrack3[i-j] = 0;
    iTrack3Len = i-j;

    i = 0;
    tInterface->szData[i] = iTrack2Len;
    i ++;
    memcpy( tInterface->szData+i, szTrack2, iTrack2Len );
    i += iTrack2Len;

    tInterface->szData[i] = iTrack3Len;
    i ++;
    memcpy( tInterface->szData+i, szTrack3, iTrack3Len );
    i += iTrack3Len;

    tInterface->iDataLen = i;
    strcpy( tInterface->szReturnCode, TRANS_SUCC );
    
   
    return SUCC;
}
