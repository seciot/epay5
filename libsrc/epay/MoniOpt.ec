/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：epay库文件,主要包括发送监控信息到WEB的UPD监听端口
** 创 建 人：高明鑫
** 创建日期：2012/11/8
** $Revision: 1.7 $
** $Log: MoniOpt.ec,v $
** Revision 1.7  2013/06/17 09:07:09  fengw
**
** 1、修改WebDispMoni函数向web输出监控数据格式。
**
** Revision 1.6  2012/12/25 07:02:57  fengw
**
** 1、修改端口号参数类型为字符串。
**
** Revision 1.5  2012/12/19 07:40:54  fengw
**
** 1、修改GetResult函数调用参数。
**
** Revision 1.4  2012/12/19 07:14:49  fengw
**
** 1、将GetResult函数从文件中分离成独立代码文件。
**
** Revision 1.3  2012/12/07 01:51:56  fengw
**
** 1、替换sgetdate为GetSysDate，sgettime为GetSysTime。
**
** Revision 1.2  2012/12/03 08:23:50  gaomx
** *** empty log message ***
**
** Revision 1.1  2012/12/03 07:44:03  gaomx
** *** empty log message ***
**
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "../../incl/user.h"
#include "../../incl/app.h"
#include "../../incl/dbtool.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/*************************************************************
*  功    能: 	
*		发送监控信息到WEB的UDP监听端口。
*  输入参数: 
*		ptApp		    APP公共数据结构
        pszTransName    交易名称
        pszIp           WEB系统的IP地址
        iPort           WEB系统的UPD监控端口
*  输出参数: 	
*       无
*  返    回：	
        无
*************************************************************/
void WebDispMoni( ptApp, pszTransName, pszIp, szPort ) 
T_App  *ptApp;
char   *pszTransName;
char   *pszIp;
char   *szPort;
{
	char	szTimeBuf[20], szDateBuf[10];
	char	szCardNo[20], szTransName[9], szResult[23]; 
	unsigned long	lMoney;
	int	iLen;
	char	szAmtBuf[20], szBuf[512];

	strcpy( szTimeBuf, ptApp->szHostTime );
	strcpy( szDateBuf, ptApp->szHostDate );

	if( strlen(szTimeBuf) == 0 )
	{
		GetSysTime(szTimeBuf);
	}
	if( strlen(szDateBuf) == 0 )
	{
		GetSysDate(szDateBuf);
	}

	memset( szCardNo, '\0', 20 );
	memset( szBuf, 0, sizeof(szBuf) );

	memset( szResult, 0, sizeof( szResult ) );
	
    memcpy(szResult, ptApp->szRetCode, 2);
    GetResult(ptApp->szRetCode, szResult+2);

	//用新的取卡号函数替换get_cardno( szCardNo, ptApp );

	DelTailSpace( szResult );
	strcat( szResult, ptApp->szHostRetCode );

    /* 新协议里用20个字节来标识交易名称 */
	if( pszTransName == NULL ) 
	{
		sprintf( szTransName, "%-20.20s", ptApp->szTransName );
	}
	else
	{
		sprintf( szTransName, "%-20.20s", pszTransName );
	}

	lMoney = atol( ptApp->szAmount );
	if( lMoney != 0 )
	{
		ChgAmtZeroToDot( ptApp->szAmount, 12, szAmtBuf);
		sprintf( szBuf, "%-15.15s%15.15s%-19.19s%-20.20s%12.12s%-16.16s%8.8s%6.6s%8.8s%8.8s%-20.20s", \
                 ptApp->szShopNo, ptApp->szPosNo, szCardNo, szTransName, szAmtBuf, szResult, szDateBuf, \
                 szTimeBuf, ptApp->szOutBankId, ptApp->szAcqBankId, ptApp->szShopName );
	}
	else if( strlen(ptApp->szAddiAmount) > 0 )
	{
		ChgAmtZeroToDot( ptApp->szAddiAmount, 12, szAmtBuf);
		sprintf( szBuf, "%-15.15s%15.15s%-19.19s%-20.20s%12.12s%-16.16s%8.8s%6.6s%8.8s%8.8s%-20.20s", \
                 ptApp->szShopNo, ptApp->szPosNo, szCardNo, szTransName, szAmtBuf, szResult, szDateBuf, 
                 szTimeBuf, ptApp->szOutBankId, ptApp->szAcqBankId, ptApp->szShopName );
	}
	else
	{
		sprintf( szBuf, "%-15.15s%15.15s%-19.19s%-20.20s%12s%-16.16s%8.8s%6.6s%8.8s%8.8s%-20.20s", \
                 ptApp->szShopNo, ptApp->szPosNo, szCardNo, szTransName, "", szResult, szDateBuf, 
                 szTimeBuf, ptApp->szOutBankId, ptApp->szAcqBankId, ptApp->szShopName );
	}

	iLen = strlen(szBuf);

	SendToUdpSrv( pszIp, szPort, szBuf, iLen );
}
