/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:56所加密机接口(racal版本)
** 创 建 人:
** 创建日期:


$Revision: 1.11 $
$Log: Sjl06eRacal.c,v $
Revision 1.11  2012/12/26 01:44:17  wukj
%s/commu_with_hsm/CommuWithHsm/g

Revision 1.10  2012/12/26 01:41:49  wukj
CommuWithHsm返回值长度小于加密机报文头导致进程退出的处理

Revision 1.9  2012/12/05 06:32:13  wukj
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


extern char gszPospZMKSjl06ERacal[33], gszPoscZMKSjl06ERacal[33];
/*****************************************************************
** 功    能:加密机随机生成终端主密钥TMK，并将其密文和检查值返回给主机
** 输入参数:
          无
** 输出参数:
           tInterface->szData   TMK(存放中心32Bytes)+CheckValue(4Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/

int Sjl06eRacalGetTmk_back( T_Interface *tInterface )
{
    char    szInData[1024], szOutData[1024], szRetCode[3];
    char    szSndData[1024], szRcvData[1024];
       int     iLen, iRet, iSndLen, i;

    iLen = 0;
    /*命令*/
    memcpy( szInData, "A0", 2 );    
    iLen += 2;
    /*模式 不需要ZMK加密*/
    memcpy( szInData+iLen, "0",  1 );    
    iLen += 1;
    /*密钥类型 TMK*/
    memcpy( szInData+iLen, "002", 3 );    
    iLen += 3;
    /*密钥方案(LMK下加密密钥方案)，双倍长密钥*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }
    if( memcmp(szOutData, "A1", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData, szOutData+5, 32 );
    memcpy( tInterface->szData+32, szOutData+37, 4 );
    tInterface->iDataLen = 36;
       strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能:加密机随机生成终端主密钥TMK，分别用LMK及ZMK加密，并将其密文和检查值返回给主机
** 输入参数:
          无
** 输出参数:
           tInterface->szData 
           TMK(LMK加密存放在中心32Bytes)+
           TMK(ZMK加密分发给终端32Bytes)+ 
           CheckValue(4Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalGetTmk( T_Interface *tInterface )
{
    char    szInData[1024], szOutData[1024], szRetCode[3];
    char    szSndData[1024], szRcvData[1024];
       int     iLen, iRet, iSndLen, i;

    iLen = 0;
    /*命令*/
    memcpy( szInData, "A0", 2 );    
    iLen += 2;
    /*模式 需要ZMK加密*/
    memcpy( szInData+iLen, "1",  1 );    
    iLen += 1;
    /*密钥类型 TMK*/
    memcpy( szInData+iLen, "002", 3 );    
    iLen += 3;
    /*密钥方案(LMK下加密密钥方案)，双倍长密钥*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*ZMK密文*/
    memcpy( szInData+iLen, gszPoscZMKSjl06ERacal, 32 ); 
    iLen += 32;
    /*密钥方案(ZMK下加密密钥方案)，双倍长密钥*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 

    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "A1", 2) != 0  ||
        (memcmp(szOutData+2, "00", 2) != 0 && 
             memcmp(szOutData+2, "10", 2) != 0) )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    /* LMK对(14-15)加密下的TMK */
    memcpy( tInterface->szData, szOutData+5, 32 );
    /* ZMK加密下的TMK */
    memcpy( tInterface->szData+32, szOutData+38, 32 );
    /* 密钥校验值 */
    memcpy( tInterface->szData+64, szOutData+70, 4 );
    tInterface->iDataLen = 68;
    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能:生成数据密钥PIK/MAK/MAG，并将其密文和检查值返回给主机
** 输入参数:
           tInterface->szData 终端主密钥密文(32Bytes，保存在中心的)
** 输出参数:
           tInterface->szData 
           PIK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)+
           MAK(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(168Bytes)
           MAG(存放中心32Bytes)+PIK(下传POS32Bytes)+CheckValue(16Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalGetWorkKey(T_Interface *tInterface )
{
    char    szInData[1024], szOutData[1024], szPsamNo[17], szRand[33];
    char    szSndData[1024], szRcvData[1024];
    char    szEnTmk[33];
    int     iLen, iRet, iSndLen, i;
    char    cChr;

    memcpy( szEnTmk, tInterface->szData, 32 );
    
    /*获取密码加密密钥PIK(TPK)*/
    iLen = 0;
    /*命令*/
    memcpy( szInData, "HC", 2 );    
    iLen += 2;
    /*终端主密钥长度标识，16位长*/
    szInData[iLen] = 'X';    
    iLen ++;
    /*终端主密钥*/
    memcpy( szInData+iLen, szEnTmk, 32 );    
    iLen += 32;
    /*分割符*/
    szInData[iLen] = ';';            
    iLen ++;
    /*TMK下加密密钥方案*/
    szInData[iLen] = 'X';    
    iLen ++;
    /*LMK下加密密钥方案*/
    szInData[iLen] = 'X';    
    iLen ++;
    /*密钥校验值类型*/
    szInData[iLen] = '0';    
    iLen ++;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 

    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }
    memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );

    if( memcmp(szOutData, "HD", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    /*LMK对(14-15)加密的PIK，本地保存*/
    memcpy( tInterface->szData, szOutData+38, 32 );
    /*TMK加密的PIK，传给终端*/
    memcpy( tInterface->szData+32, szOutData+5, 32 );
    /*CheckVal*/
    memcpy( tInterface->szData+64, szOutData+70, 16 );

    for( i=1; i<=2; i++ )
    {
        /*获取终端认证密钥MAK(TAK)*/
        iLen = 0;
        /*命令*/
        memcpy( szInData, "HA", 2 );    
        iLen += 2;
        /*终端主密钥长度标识，16位长*/
        szInData[iLen] = 'X';    
        iLen ++;
        /*终端主密钥*/
        memcpy( szInData+iLen, szEnTmk, 32 );    
        iLen += 32;
        /*分割符*/
        szInData[iLen] = ';';            
        iLen ++;
        /*TMK下加密密钥方案*/
        szInData[iLen] = 'X';    
        iLen ++;
        /*LMK下加密密钥方案*/
        szInData[iLen] = 'X';    
        iLen ++;
        /*密钥校验值类型:0-KCV向后兼容 1-KCV6H*/
        szInData[iLen] = '0';    
        iLen ++;
        szInData[iLen] = 0;

        memset( szRcvData, 0, 1024 );
        memset( szOutData, 0, 1024 );
        memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
        memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
        iLen += SJL06E_RACAL_HEAD_LEN;
        iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 

        if(iRet == FAIL)
        {
            WriteLog( ERROR, "commu with hsm fail" );
            return FAIL;
        }
        else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
        {
            memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
        }
        else
        {
            WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
            return FAIL;
        }

        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );

        if( memcmp(szOutData, "HB", 2) != 0  ||
            memcmp(szOutData+2, "00", 2) != 0 )
        {
            DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
            WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
            return SUCC;
        }

        /*LMK对(16-17)加密的MAK，保存在中心*/
        memcpy( tInterface->szData+80*i, szOutData+38, 32 );

        /*TMK加密的MAK，传给终端*/
        memcpy( tInterface->szData+80*i+32, szOutData+5, 32 );

        /*密钥校验值*/
        memcpy( tInterface->szData+80*i+64, szOutData+70, 16 );
    }

    tInterface->iDataLen = 240;

       strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:MAC计算 
** 输入参数:
           tInterface->szData 参与MAC运算的数据，长度由tInterface->iDataLen指定
** 输出参数:
           tInterface->szData MAC(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalCalcMac(T_Interface *tInterface)
{
    char    szInData[1024], szOutData[1024];
    char    szSndData[1024], szRcvData[1024], szTmpStr[20];
       int     iLen, iRet, iSndLen;

    iLen = 0;
    memcpy( szInData, "MS", 2 );    /* 命令 */
    iLen += 2;

    /*消息块号:0-仅1块 1-第1块 2-中间块 3-尾块 */
    memcpy( szInData+iLen, "0", 1 );
    iLen += 1;

    /*密钥类型:0-TAK终端认证密钥 1-ZAK区域认证密钥 */
    memcpy( szInData+iLen, "0", 1 );
    iLen += 1;

    /*密钥长度:0-单倍长度 1-双倍长度 */
    memcpy( szInData+iLen, "1", 1 );
    iLen += 1;

    /*消息类型:0-二进制 1-扩展十六进制 */
    memcpy( szInData+iLen, "0", 1 );
    iLen += 1;

    /*MAC密钥方案 */
    memcpy( szInData+iLen, "X", 1 );
    iLen += 1;

    /*MAC密钥密文*/
    BcdToAsc((uchar *)(tInterface->szMacKey), 32, 0 , (uchar *)(szInData+iLen));    
    iLen += 32;

    /* MAC算法 1-XOR 2-X9.9 3-X9.19 */
    if( tInterface->iAlog == XOR_CALC_MAC )
    {
        /*消息长度*/
        memcpy( szInData+iLen, "0008", 8 );
        iLen += 4;

        /*消息块*/
        XOR( tInterface->szData, tInterface->iDataLen, szOutData );
           memcpy( szInData+iLen, szOutData, 8 );
        iLen += 8;
    }
    else
    {
        /*消息长度*/
        szTmpStr[0] = tInterface->iDataLen/256;
        szTmpStr[1] = tInterface->iDataLen%256;
        BcdToAsc( (uchar *)szTmpStr, 4, 0 , (uchar *)(szInData+iLen));
        iLen += 4;

        /*消息块*/
           memcpy( szInData+iLen, tInterface->szData, tInterface->iDataLen );
        iLen = iLen+tInterface->iDataLen;
    }
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }
    if( memcmp(szOutData, "MT", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm calc mac fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    AscToBcd( (uchar *)(szOutData+4), 16, 0 ,(uchar *)(tInterface->szData));

    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:将源PIN密文用源PIK解密，进行PIN格式转换，然后用目的PIK加密输出.
** 输入参数:
           tInterface->szData 帐号(16字节)+密码密文(8字节)
** 输出参数:
           tInterface->szData 转加密后的密码密文(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalChangePin(T_Interface *tInterface)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17];
    char    szSndData[1024], szRcvData[1024];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "%12.12s", tInterface->szData+3 );
    szPanBlock[12] = 0;

    iLen = 0;
    memcpy( szInData, "CA", 2 );    /* 命令 */
    iLen += 2;

    /* 源PIK密钥长度标识，16位长 */
    szInData[iLen] = 'X';    
    iLen ++;

    /* 源PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szPinKey), 32, 0 , (uchar *)(szInData+iLen));    
    iLen += 32;

    /* 目的PIK密钥长度标识，16位长 */
    szInData[iLen] = 'X';    
    iLen ++;

    /* 目的PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szMacKey), 32, 0 , (uchar *)(szInData+iLen));    
    iLen += 32;
    /* 最大PIN长度 */
    memcpy( szInData+iLen, "12", 2 );    
    iLen += 2;

    /* 源PinBlock密文 */
    BcdToAsc( (uchar *)(tInterface->szData+16), 16, 0 , (uchar *)(szInData+iLen));    
    iLen += 16;

    /* 源PinBlock格式 */
    memcpy( szInData+iLen, "01", 2 );    
    iLen += 2;

    /* 目的PinBlock格式 */
    memcpy( szInData+iLen, "01", 2 );    
    iLen += 2;

    /* 源帐号 */
    memcpy( szInData+iLen, szPanBlock, 12 );    
    iLen += 12;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "CB", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm pin change fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    AscToBcd( (uchar *)(szOutData+6), 16, 0 ,(uchar *)(tInterface->szData));
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能:将PIN密文用源PIK解密，进行PIN格式转换，然后用LMK对(02-03)加密输出.
** 输入参数:
           tInterface->szData 帐号(16字节)+密码密文(8字节)
** 输出参数:
           tInterface->szData 转加密后的密码密文(8字节)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalChangePin_TPK2LMK(T_Interface *tInterface)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17];
    char    szSndData[1024], szRcvData[1024];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "%12.12s", tInterface->szData+3 );
    szPanBlock[12] = 0;

    iLen = 0;
    memcpy( szInData, "JC", 2 );    /* 命令 */
    iLen += 2;

    /* 源PIK密钥长度标识，16位长 */
    szInData[iLen] = 'X';    
    iLen ++;

    /* 源PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szPinKey), 32, 0 , (uchar *)(szInData+iLen));
    iLen += 32;

    /* PinBlock密文 */
    BcdToAsc( (uchar *)(tInterface->szData+16), 16, 0 , (uchar *)(szInData+iLen));
    iLen += 16;

    /* PinBlock格式 */
    memcpy( szInData+iLen, "01", 2 );    
    iLen += 2;

    /* 帐号 */
    memcpy( szInData+iLen, szPanBlock, 12 );    
    iLen += 12;
    szInData[iLen] = 0;

//WriteLog( TRACE, "ChgPinSnd[%s]", szInData );

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "JD", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm pin change fail[%2.2s]", szOutData+2 );
        return SUCC;
    }
/*
WriteLog( TRACE, "ChgPinRcv[%s]", szOutData );
*/
    tInterface->iDataLen = strlen(szOutData)-4;
    memcpy( tInterface->szData, szOutData+4, tInterface->iDataLen );
    tInterface->szData[tInterface->iDataLen] = 0;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 解密PIN
** 输入参数:
           tInterface->szData 帐号(16字节)+终端PIN密文(8字节，LMK对(02-03)加密)
** 输出参数:
           tInterface->szData 密码明文
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalDecryptPin(T_Interface *tInterface)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17];
    char    szSndData[1024], szRcvData[1024], szLmkEnPin[9];
    int     iLen, iRet, iSndLen;

    sprintf( szPanBlock, "%12.12s", tInterface->szData+3 );
    szPanBlock[12] = 0;

    iRet = Sjl06eRacalChangePin_TPK2LMK( tInterface );
    if( iRet != SUCC || strcmp(tInterface->szReturnCode, TRANS_SUCC) != 0 )
    {
        WriteLog( ERROR, "change pin fail" );
        return SUCC;
    }
    strcpy( szLmkEnPin, tInterface->szData );    

    iLen = 0;
    memcpy( szInData, "NG", 2 );    /* 命令 */
    iLen += 2;

    /* 帐号 */
    memcpy( szInData+iLen, szPanBlock, 12 );    
    iLen += 12;

    /* PinBlock密文 LMK对(02-03)加密 */
    memcpy( szInData+iLen, tInterface->szData, tInterface->iDataLen );
    iLen += tInterface->iDataLen;

    szInData[iLen] = 0;

//WriteLog( TRACE, "DecPinSnd[%s]", szInData );

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

//WriteLog( TRACE, "DecPinRcv[%s]", szOutData );

    if( memcmp(szOutData, "NH", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm pin change fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData, szOutData+4, 8 );
    tInterface->szData[8] = 0;
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;
}

/*****************************************************************
** 功    能: 验证终端上送的PIN是否与数据库中的PIN一致
             分3步:
             1、用LMK对PIN明文加密，得szLmkEncPin1
             2、将终端PIN密文从TPK翻译到LMK，得szLmkEncPin2
             3、比较szLmkEncPin1 szLmkEncPin2
** 输入参数:
           tInterface->szData 数据库中密码明文(8字节)+终端PIN密文(8字节)
** 输出参数:
           tInterface->szData SUCC-一致  FAIL-不一致
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalVerifyPin(T_Interface *tInterface)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17], szLmkEncPin1[17], szLmkEncPin2[17];
    char    szSndData[1024], szRcvData[1024];
    int     iLen, iRet, iSndLen;

    memset( szPanBlock, '0', 16 );
    szPanBlock[16] = 0;

    /*========用LMK对PIN明文加密   开始==============*/
    iLen = 0;
    memcpy( szInData, "BA", 2 );    /* 命令 */
    iLen += 2;

    /* PIN明文 */
    memcpy( szInData+iLen, tInterface->szData, 8 );
    iLen += 8;
    memcpy( szInData+iLen, "FFFF", 4 );
    iLen += 4;

    /* 帐号 */
    memcpy( szInData+iLen, szPanBlock, 12 );    
    iLen += 12;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "BB", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm encrypt pin fail[%2.2s]", szOutData+2 );
        return SUCC;
    }
    memcpy( szLmkEncPin1, szOutData+4, 16 );
    /*========用LMK对PIN明文加密   结束=============*/

    /*========将终端PIN密文从TPK翻译到LMK  开始=============*/
    iLen = 0;
    memcpy( szInData, "JC", 2 );    /* 命令 */
    iLen += 2;

    /* PIK密钥长度标识，16位长 */
    szInData[iLen] = 'X';    
    iLen ++;

    /* PIK密钥密文 */
    BcdToAsc( (uchar *)(tInterface->szPinKey), 32, 0, (uchar *)(szInData+iLen) );
    iLen += 32;

    /* 终端PIN密文 */
    BcdToAsc(  (uchar *)(tInterface->szData+8), 16, 0 , (uchar *)(szInData+iLen));
    iLen += 16;

    /* PIN块格式代码 */
    memcpy( szInData+iLen, "01", 2 );
    iLen += 2;

    /* 帐号 */
    memcpy( szInData+iLen, szPanBlock, 12 );    
    iLen += 12;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "JD", 2) != 0 ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm encrypt pin fail[%2.2s]", szOutData+2 );
        return SUCC;
    }
    memcpy( szLmkEncPin2, szOutData+4, 16 );
    /*========将PIN从TPK翻译到LMK  结束=============*/

    if( memcmp( szLmkEncPin1, szLmkEncPin2, 16 ) == 0 )
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
** 输出参数:
           tInterface->szData 校验值(16)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalCalcChkval(T_Interface *tInterface)
{
    char    szInData[1024], szOutData[1024], szPanBlock[17];
    char    szSndData[1024], szRcvData[1024];
    int     iLen, iRet, iSndLen;
    memset(szInData,0x00,sizeof(szInData));
    iLen = 0;
    memcpy( szInData, "BU", 2 );    /* 命令 */
    iLen += 2;

    /* 加密密钥类型代码 */
    sprintf(szInData+iLen,"%02d",tInterface->iAlog);   //alog由调用函数传入,该值代表LMK对代码
    iLen += 2;

    /* 密钥长度标识，16位长 */
    szInData[iLen] = '1';    
    iLen ++;

    /* 密钥长度标识，16位长 */
    szInData[iLen] = 'X';    
    iLen ++;

    /* 密钥密文 */
    memcpy( szInData+iLen, tInterface->szData, 32 );
    iLen += 32;
    
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if(memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm encrypt pin fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData, szOutData+4, 8 );
    tInterface->iDataLen = 8;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );
   
    return SUCC;

}

/*****************************************************************
** 功    能:加密机转加密工作密钥，即将由TMK加密的PIK还原成明文，然后再用LMK对(06-07)加密
            输出；同时将由TMK加密的MAK还原成明文，然后再用LMK对(16-17)加密输出.为此需要
                  按以下步骤完成:
                  1、将TMK用ZMK(通讯双方约定)还原成明文，然后用LMK对(04-05)加密输出，将TMK
                     转换成ZMK；
                  2、将PIK用ZMK(TMK转换而来)还原成明文，然后再用LMK对(06-07)加密输出.
                  3、将MAK用ZMK(TMK转换而来)还原成明文，然后再用LMK对(16-17)加密输出.
                  4、将TMK用ZMK(通讯双方约定)还原成明文，然后用LMK对(16-17)加密输出.
** 输入参数:
           tInterface->szData TMK密文(32Bytes)+PIK密文(32Bytes)+MAK密文(32Bytes), 工作密钥由TMK加密
** 输出参数:
           tInterface->szData PIK密文(32Bytes)+MAK密文(32Bytes)+TMK密文(32Bytes),工作密钥分别由LMK对(06-07)、LMK对(16-17)、LMK对(16-17)加密
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:
** 日    期:
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int Sjl06eRacalChangeWorkkey( T_Interface *tInterface )
{
    char    szInData[1024], szOutData[1024], szRetCode[3];
    char    szSndData[1024], szRcvData[1024];
    char    szTmkZmk[33], szEnTmk[33];
    int     iLen, iRet, iSndLen;

    /*============步骤1:TMK转换成ZMK================*/
    iLen = 0;
    /*命令*/
    memcpy( szInData, "A6", 2 );    
    iLen += 2;
    /*输出密钥类型:ZMK */
    memcpy( szInData+iLen, "000", 3 );    
    iLen += 3;
    /*ZMK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*ZMK密文*/
    memcpy( szInData+iLen, gszPospZMKSjl06ERacal, 32 );    
    iLen += 32;
    /*TMK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*TMK密文*/
    memcpy( szInData+iLen, tInterface->szData, 32 );    
    iLen += 32;
    memcpy( szEnTmk, tInterface->szData, 32 );
    /*密钥方案(LMK下加密密钥方案)，双倍长密钥*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "A7", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( szTmkZmk, szOutData+5, 32 );

    /*============步骤2:PIK由TMK加密转换成由LMK对(06-07)加密=========*/
    iLen = 0;
    /*命令*/
    memcpy( szInData, "A6", 2 );    
    iLen += 2;
    /*输出密钥类型:ZPK */
    memcpy( szInData+iLen, "001", 3 );    
    iLen += 3;
    /*ZMK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*ZMK密文(TMK转换而来)*/
    memcpy( szInData+iLen, szTmkZmk, 32 );    
    iLen += 32;
    /*PIK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*PIK密文*/
    memcpy( szInData+iLen, tInterface->szData+32, 32 );    
    iLen += 32;
    /*密钥方案(LMK下加密密钥方案)，双倍长密钥*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "A7", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }
    memcpy( tInterface->szData, szOutData+5, 32 );

    /*============步骤3:MAK由TMK加密转换成由LMK对(16-17)加密=========*/
    iLen = 0;
    /*命令*/
    memcpy( szInData, "A6", 2 );    
    iLen += 2;
    /*输出密钥类型:TAK */
    memcpy( szInData+iLen, "003", 3 );    
    iLen += 3;
    /*ZMK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*ZMK密文(TMK转换而来)*/
    memcpy( szInData+iLen, szTmkZmk, 32 );    
    iLen += 32;
    /*MAK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*MAK密文*/
    memcpy( szInData+iLen, tInterface->szData+64, 32 );    
    iLen += 32;
    /*密钥方案(LMK下加密密钥方案)，双倍长密钥*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "A7", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData+32, szOutData+5, 32 );

    /*============步骤4:TMK转换成由LMK对(16-17)加密================*/
    iLen = 0;
    /*命令*/
    memcpy( szInData, "A6", 2 );    
    iLen += 2;
    /*输出密钥类型:TAK */
    memcpy( szInData+iLen, "003", 3 );    
    iLen += 3;
    /*ZMK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*ZMK密文*/
    memcpy( szInData+iLen, gszPospZMKSjl06ERacal, 32 );    
    iLen += 32;
    /*TMK密钥方案*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    /*TMK密文*/
    memcpy( szInData+iLen, szEnTmk, 32 );    
    iLen += 32;
    /*密钥方案(LMK下加密密钥方案)，双倍长密钥*/
    memcpy( szInData+iLen, "X", 1 ); 
    iLen += 1;
    szInData[iLen] = 0;

    memset( szRcvData, 0, 1024 );
    memset( szOutData, 0, 1024 );
    memcpy( szSndData, SJL06E_RACAL_HEAD_DATA, SJL06E_RACAL_HEAD_LEN );    
    memcpy( szSndData+SJL06E_RACAL_HEAD_LEN, szInData, iLen );    
    iLen += SJL06E_RACAL_HEAD_LEN;
    iRet = CommuWithHsm( szSndData, iLen, szRcvData ); 
    if(iRet == FAIL)
    {
        WriteLog( ERROR, "commu with hsm fail" );
        return FAIL;
    }
    else if( iRet - SJL06E_RACAL_HEAD_LEN >= 0)
    {
        memcpy( szOutData, szRcvData+SJL06E_RACAL_HEAD_LEN, iRet-SJL06E_RACAL_HEAD_LEN );
    }
    else
    {
        WriteLog(ERROR,"请检查加密机消息头长度，是否>=[%d]" , SJL06E_RACAL_HEAD_LEN );
        return FAIL;
    }

    if( memcmp(szOutData, "A7", 2) != 0  ||
        memcmp(szOutData+2, "00", 2) != 0 )
    {
        DispSjl06eRacalErrorMsg( szOutData+2, tInterface->szReturnCode );
        WriteLog( ERROR, "hsm fail[%2.2s]", szOutData+2 );
        return SUCC;
    }

    memcpy( tInterface->szData+64, szOutData+5, 32 );
    tInterface->szData[96] = 0;
    tInterface->iDataLen = 96;

    strcpy( tInterface->szReturnCode, TRANS_SUCC );

    return SUCC;
}
