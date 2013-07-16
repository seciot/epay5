/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：epay库文件,加密接口
** 创 建 人：wukj
** 创建日期：2012/11/29
**
**
** $Revision: 1.3 $
** $Log: HsmClient.c,v $
** Revision 1.3  2013/06/14 02:24:33  fengw
**
** 1、修改HsmChangePin函数参数说明。
**
** Revision 1.2  2013/01/05 06:38:17  fengw
**
** 1、删除HsmDecryptTrack函数中iMagAlog参数。
**
** Revision 1.1  2012/12/10 06:49:05  wukj
** *** empty log message ***
**
** Revision 1.4  2012/12/03 05:56:34  wukj
** *** empty log message ***
**
** Revision 1.3  2012/12/03 05:55:52  wukj
** 修改WriteETLog为WriteLog
**
** Revision 1.2  2012/11/29 04:36:01  wukj
** 变量改写及函数注释
**
**
*******************************************************************/
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>

# include "app.h"
# include "errcode.h"
# include "transtype.h"
# include "user.h"

long GetCurMillTime()
{
    unsigned long   lTime;
	struct	timeb	tp;
	struct	tm	*tm;

    ftime ( &tp );
    tm = localtime (  & ( tp.time )  );
    lTime = (tm->tm_min)*60000+(tm->tm_sec)*1000+
                tp.millitm;
	lTime = lTime%10000;

	return (lTime);
}

/*****************************************************************
** 功    能:通过消息队列访问加密服务模块，计算MAC
** 输入参数:
            ptApp	        -- 公共数据结构
            nMacType        -- XOR_CALC_MAC           XOR算法
                X99_CALC_MAC    X99算法
                X919_CALC_MAC   X919算法
            szEnMacKey      -- MacKey密文
            szMacData       -- 计算mac的数据串
            nLen	        -- 数据串长度
** 输出参数:
            szMac	-- 计算的MAC值
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
****************************************************************/
int HsmCalcMac( ptApp, nMacType, szEnMacKey, szMacData, nLen, szMac )
T_App *ptApp;
int nMacType;
char szEnMacKey[17];
char *szMacData;
int nLen;
char *szMac;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = CALC_MAC;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	tFace.iAlog = nMacType;
	strcpy( tFace.szPsamNo, ptApp->szPsamNo);
	memcpy( tFace.szMacKey, szEnMacKey, 16 );
	tFace.iDataLen = nLen;
	memcpy( tFace.szData, szMacData, nLen );

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "calc mac send to hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "calc mac read from hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	if( memcmp( tFace.szReturnCode, TRANS_SUCC, 2 ) != 0 )
	{
		strcpy( ptApp->szRetCode, tFace.szReturnCode );
		return FAIL;
	}

	memcpy( szMac, tFace.szData, 8 );
	return SUCC;
}

/*****************************************************************
** 功    能:通过消息队列访问加密服务模块，进行密码转加密
** 输入参数:
            ptApp	    -- 公共数据结构
            nPinType    -- 1-szPasswd   2-szNewPasswd
            iPinAlog    -- TRIPLE_DES SINGLE_DES 只有软加密时使用到
            szEnPinKey1 -- 源PinKey密文
            szEnPinKey2 -- 目的PinKey密文
            szPan1      -- 源帐号
            szPan2      -- 目的帐号
** 输出参数:
            szPin	    -- 转加密后的密文
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
****************************************************************/
int HsmChangePin( ptApp, nPinType,iPinAlog, szEnPinKey1, szEnPinKey2, szPan1, szPan2 )
T_App *ptApp;
int nPinType;
int iPinAlog;
char szEnPinKey1[17];
char szEnPinKey2[17];
char *szPan1;
char *szPan2;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = CHANGE_PIN;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	tFace.iAlog = iPinAlog;
	strcpy( tFace.szPsamNo, ptApp->szPsamNo);
	memcpy( tFace.szPinKey, szEnPinKey1, 16 );
	memcpy( tFace.szMacKey, szEnPinKey2, 16 );
	
	memcpy( tFace.szData, szPan1, 16 );	
	if( nPinType == 1 )
	{
		memcpy( tFace.szData+16, ptApp->szPasswd, 8 );
	}
	else
	{
		memcpy( tFace.szData+16, ptApp->szNewPasswd, 8 );
	}

	memcpy( tFace.szData+24, szPan2, 16 );
	tFace.iDataLen = 40;

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin send to hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin read from hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	if( memcmp( tFace.szReturnCode, TRANS_SUCC, 2 ) != 0 )
	{
		WriteLog( ERROR, "change pin fail %s", tFace.szReturnCode );
		strcpy( ptApp->szRetCode, tFace.szReturnCode );
		return FAIL;
	}

	if( nPinType == 1 )
	{
		memcpy( ptApp->szPasswd, tFace.szData, 8 );
	}
	else
	{
		memcpy( ptApp->szNewPasswd, tFace.szData, 8 );
	}
		
	return SUCC;
}

/*****************************************************************
** 功    能:通过消息队列访问加密服务模块，进行密码转加密(PIK2TMK)
** 输入参数:
*      		ptApp	    -- 公共数据结构
*	    	nPinType    -- 1-szPasswd   2-szNewPasswd
*	    	szEnPinKey1 -- 源PinKey密文
*		    szEnPinKey2 -- 目的PinKey密文
*		    szPan1      -- 源帐号
*		    szPan2      -- 目的帐号
** 输出参数:
*   		szPin	    -- 转加密后的密文
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
****************************************************************/
int HsmChangePin_PIK2TMK( ptApp, nPinType, szEnPinKey1, szPan1, szPan2 )
T_App *ptApp;
int nPinType;
char szEnPinKey1[17];
char *szPan1;
char *szPan2;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = CHANGE_PIN_PIK2TMK;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	strcpy( tFace.szPsamNo, ptApp->szPsamNo);
	memcpy( tFace.szPinKey, szEnPinKey1, 16 );
	
	memcpy( tFace.szData, szPan1, 16 );	
	if( nPinType == 1 )
	{
		memcpy( tFace.szData+16, ptApp->szPasswd, 8 );
	}
	else
	{
		memcpy( tFace.szData+16, ptApp->szNewPasswd, 8 );
	}

	memcpy( tFace.szData+24, szPan2, 16 );
	tFace.iDataLen = 40;

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin send to hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin read from hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	if( memcmp( tFace.szReturnCode, TRANS_SUCC, 2 ) != 0 )
	{
		WriteLog( ERROR, "change pin fail %s", tFace.szReturnCode );
		strcpy( ptApp->szRetCode, tFace.szReturnCode );
		return FAIL;
	}

	if( nPinType == 1 )
	{
		memcpy( ptApp->szPasswd, tFace.szData, 8 );
	}
	else
	{
		memcpy( ptApp->szNewPasswd, tFace.szData, 8 );
	}
		
	return SUCC;
}

/*****************************************************************
** 功    能:解密
** 输入参数:
            ptApp    
            nPinType    1--szPasswd 2--szNewPasswd
            szEnPinKey  加密密钥
            szPan       账号
** 输出参数:
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
****************************************************************/
int HsmDecryptPin( ptApp, nPinType, szEnPinKey, szPan )
T_App *ptApp;
int nPinType;
char szEnPinKey[17];
char *szPan;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = DECRYPT_PIN;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	strcpy( tFace.szPsamNo, ptApp->szPsamNo);
	memcpy( tFace.szPinKey, szEnPinKey, 16 );
	
	memcpy( tFace.szData, szPan, 16 );	
	if( nPinType == 1 )
	{
		memcpy( tFace.szData+16, ptApp->szPasswd, 8 );
	}
	else
	{
		memcpy( tFace.szData+16, ptApp->szNewPasswd, 8 );
	}

	tFace.iDataLen = 24;

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin send to hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin read from hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	if( memcmp( tFace.szReturnCode, TRANS_SUCC, 2 ) != 0 )
	{
		WriteLog( ERROR, "change pin fail %s", tFace.szReturnCode );
		strcpy( ptApp->szRetCode, tFace.szReturnCode );
		return FAIL;
	}

	if( nPinType == 1 )
	{
		strcpy( ptApp->szPasswd, tFace.szData );
	}
	else
	{
		strcpy( ptApp->szNewPasswd, tFace.szData );
	}
		
	return SUCC;
}

/*****************************************************************
** 功    能:通过消息队列访问加密服务模块，进行密码校验
** 输入参数:
*   		ptApp	-- 公共数据结构
*   		szPin	-- 密码明文
*   		szEnPinKey -- PinKey密文
** 输出参数:
*   		szEnPin -- 密码密文
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
****************************************************************/
int HsmVerifyPin( ptApp, szPin, szEnPinKey, szHsmRet )
T_App *ptApp;
char *szPin;
char szEnPinKey[17];
char *szHsmRet;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = VERIFY_PIN;
	time( &lMsgType );
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	strcpy( tFace.szPsamNo, ptApp->szPsamNo );
	memcpy( tFace.szPinKey, szEnPinKey, 16 );
	
	memcpy( tFace.szData, szPin, 8 );	
	memcpy( tFace.szData+8, ptApp->szPasswd, 8 );
	tFace.iDataLen = 16;

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin send to hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change pin read from hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	if( memcmp( tFace.szReturnCode, TRANS_SUCC, 2 ) != 0 )
	{
		WriteLog( ERROR, "change pin fail %s", tFace.szReturnCode );
		strcpy( ptApp->szRetCode, tFace.szReturnCode );
		return FAIL;
	}

	strcpy( szHsmRet, tFace.szData );
		
	return SUCC;

}

/*****************************************************************
** 功    能:解密磁道信息
** 输入参数:
*	    	ptApp	-- 公共数据结构
*   		iMagAlog --SINGLE_DES  TRIPLE_DES  只有软加密时使用到
*   		szEnMagKey -- MagKey密文
*   		szMagData -- 磁道密文
*   		nLen	-- 磁道密文长度
** 输出参数:
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
 ****************************************************************/
int HsmDecryptTrack( ptApp, szEnMagKey, szMagData, nLen )
T_App *ptApp;
char szEnMagKey[17];
char *szMagData;
int nLen;
{
	int iRet, iTimeOut, nTrack2Len, nTrack3Len;
	long	lMsgType;
	T_Interface tFace;
	char szPan[20+1];
	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	memset(szPan, 0, sizeof(szPan));
	tFace.iTransType = DECRYPT_TRACK;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	strcpy( tFace.szPsamNo, ptApp->szPsamNo );
	memcpy( tFace.szPinKey, szEnMagKey, 16 );
	tFace.iDataLen = nLen;
	memcpy( tFace.szData, szMagData, nLen );

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "decrypt track send to hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "decrypt track read from hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	if( memcmp( tFace.szReturnCode, TRANS_SUCC, 2 ) != 0 )
	{
		WriteLog( ERROR, "decrypt track fail %s", tFace.szReturnCode );
		strcpy( ptApp->szRetCode, tFace.szReturnCode );
		return FAIL;
	}

	nTrack2Len = (unsigned char)tFace.szData[0];
	memcpy( ptApp->szTrack2, tFace.szData+1, nTrack2Len );
	ptApp->szTrack2[nTrack2Len] = 0;

	nTrack3Len = (unsigned char)tFace.szData[1+nTrack2Len];
	memcpy( ptApp->szTrack3, tFace.szData+nTrack2Len+2, nTrack3Len );
	ptApp->szTrack3[nTrack3Len] = 0;
	return SUCC;
}

/*****************************************************************
** 功    能:获取工作密钥密文
** 输入参数:
*	    	ptApp	-- 公共数据结构
** 输出参数:
    		szKeyData -- 下传给终端的密钥密文数据		
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
 ****************************************************************/
int HsmGetWorkKey( ptApp, szKeyData )
T_App *ptApp;
char *szKeyData;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = GET_WORK_KEY;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	memcpy( tFace.szPsamNo, ptApp->szPsamNo, 16 );
	memcpy( tFace.szData, szKeyData, 32 );

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "get work key send to hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "get work key read from hsm fail" );
		strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
		return FAIL;
	}

	if( memcmp(tFace.szReturnCode, TRANS_SUCC, 2) != 0 )
	{
		WriteLog( ERROR, "get work key fail %s", tFace.szReturnCode );
		strcpy( ptApp->szRetCode, tFace.szReturnCode );
		return FAIL;
	}

	memcpy( szKeyData, tFace.szData, 240 );
		
	return SUCC;
}

/*****************************************************************
** 功    能:通过消息队列访问加密服务模块，获取终端主密钥
** 输入参数:
** 输出参数:
            szKeyData -- 主密钥密文数据+校验值
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
 ****************************************************************/
int HsmGetMasterKey( szKeyData )
char *szKeyData;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = GET_MASTER_KEY;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "get master key send to hsm fail" );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "get master key read from hsm fail" );
		return FAIL;
	}

	if( memcmp(tFace.szReturnCode, TRANS_SUCC, 2) != 0 )
	{
		WriteLog( ERROR, "get master key ret[%s]", tFace.szReturnCode );
		return FAIL;
	}

	memcpy( szKeyData, tFace.szData, 68 );
		
	return SUCC;
}

/*****************************************************************
** 功    能:通过消息队列访问加密服务模块，计算密钥校验值
** 输入参数:
    		szKeyData -- 密钥密文
 			iFlag -- 密钥类型
** 输出参数:
            szChkVal -- 校验值
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
 ****************************************************************/
int HsmCalcChkval( szKeyData, szChkVal ,iFlag)
char *szKeyData;
char *szChkVal;
int iFlag;   //该值为LMK对代码,即表示szKeyData在该LMK对下加密
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = CALC_CHKVAL;
	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;
	tFace.iAlog = iFlag;
	memcpy( tFace.szData, szKeyData, 32 );
	tFace.iDataLen = 32;

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "calc chkval send to hsm fail" );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "calc chkval read from hsm fail" );
		return FAIL;
	}

	if( memcmp(tFace.szReturnCode, TRANS_SUCC, 2) != 0 )
	{
		WriteLog( ERROR, "calc chkval ret[%s]", tFace.szReturnCode );
		return FAIL;
	}

	memcpy( szChkVal, tFace.szData, 8 );
		
	return SUCC;
}


/*****************************************************************
 * 函数功能：	* 输入参数：
 * 输出参数：
 * 返    回：
 *     		SUCC	-- 成功 
 *     		FAIL	-- 成功 
 ****************************************************************/
/*****************************************************************
** 功    能:通过消息队列访问加密服务模块，获取进行终端主密钥转加密
** 输入参数:
            szInKey -- TMK密文(32Bytes)+PIK密文(32Bytes)+MAC密文(32Bytes)
** 输出参数:
            szOutKey -- PIK密文(32Bytes)+MAC密文(32Bytes)+TMK密文(32Bytes)
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121129规范命名及排版修订
**
 ****************************************************************/
int HsmChangeWorkKey( szInKey, szOutKey )
char *szInKey;
char *szOutKey;
{
	int iRet, iTimeOut;
	long	lMsgType;
	T_Interface tFace;

	iTimeOut = 10;

	memset( (char *)&tFace, 0, sizeof(T_Interface) );
	tFace.iTransType = CHANGE_KEY;
	memcpy( tFace.szData, szInKey, 96 );
	tFace.iDataLen = 96;

	lMsgType = GetCurMillTime()+10000*tFace.iTransType;
	tFace.lSourceType = lMsgType;

	iRet = SendToHsmQue( lMsgType, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change work key send to hsm fail" );
		return FAIL;
	}

	iRet = RecvFromHsmQue( lMsgType, iTimeOut, &tFace );
	if( iRet != SUCC )
	{
		WriteLog( ERROR, "change work key read from hsm fail" );
		return FAIL;
	}

	if( memcmp(tFace.szReturnCode, TRANS_SUCC, 2) != 0 )
	{
		WriteLog( ERROR, "change work key ret[%s]", tFace.szReturnCode );
		return FAIL;
	}

	memcpy( szOutKey, tFace.szData, 96 );
	tFace.iDataLen = 96;
		
	return SUCC;
}
