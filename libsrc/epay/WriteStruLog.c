/******************************************************************
 * ** Copyright(C)2012 - 2015联迪商用设备有限公司
 * ** 主要内容：epay库文件,主要打印APP等结构体日志函数
 * ** 创 建 人：高明鑫
 * ** 创建日期：2012/11/8
 * ** $Revision: 1.13 $
 * ** $Log: WriteStruLog.c,v $
 * ** Revision 1.13  2013/02/25 01:11:16  fengw
 * **
 * ** 1、修改TPDU地址日志打印格式。
 * **
 * ** Revision 1.12  2012/12/17 06:41:46  chenrb
 * ** app结构增加控制参数长度、控制参数2个字段，修改WriteAppStru适应
 * **
 * ** Revision 1.11  2012/12/11 03:28:49  chenrb
 * ** iMenuItem修改成iaMenuItem
 * **
 * ** Revision 1.10  2012/12/11 03:08:23  chenrb
 * ** WriteAppStru增加几个字段的显示
 * **
 * ** Revision 1.9  2012/12/10 02:43:32  fengw
 * **
 * ** 1、替换sgetdate函数为GetSysDate。
 * **
 * ** Revision 1.8  2012/11/30 06:07:49  zhangwm
 * **
 * ** 加密机流转日志打印函数增加日志打印开关
 * **
 * ** Revision 1.7  2012/11/29 07:09:22  zhangwm
 * **
 * ** 增加判断是否打印日志
 * **
 * ** Revision 1.6  2012/11/29 06:04:22  chenrb
 * ** 删除lLastVisitTime字段
 * **
 * ** Revision 1.5  2012/11/29 05:40:45  gaomx
 * ** 修订消息类型变量
 * **
 * ** Revision 1.4  2012/11/29 02:23:44  gaomx
 * ** *** empty log message ***
 * **
 * ** Revision 1.3  2012/11/28 08:14:06  yezt
 * **
 * ** 增加函数注释
 * **
 * ** Revision 1.2  2012/11/28 07:49:42  gaomx
 * ** 修正结构体定义
 * **
 * ** Revision 1.1  2012/11/28 01:54:30  gaomx
 * ** *** empty log message ***
 * **
 * ** Revision 1.4  2012/11/28 01:50:11  gaomx
 * ** *** empty log message ***
 * **
 * ** Revision 1.3  2012/11/28 01:48:23  gaomx
 * ** 重新编辑格式
 * **
 * *******************************************************************/

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../../incl/app.h"
#include "../../incl/user.h"

/* ----------------------------------------------------------------
 * 功    能： 打印应用程序结构包到App日志中
 * 输入参数：
 *            ptApp      应用程序结构包
 *            szTitle    字符串指针(传入报文头)
 * 输出参数： 无
 * 返 回 值： FAIL  失败/ SUCC    成功
 * 作    者：
 * 日    期： 2012/11/28
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
    int
WriteAppStru( ptApp, szTitle )
    T_App    *ptApp;
    char *szTitle;
{
    FILE    *fd;
    char    szFileName[80], szTmpBuf[2048], szDate[20];
    int i;

    if(IsPrint(DEBUG_ALOG) == NO)
    {
        return FAIL;
    }

    GetFullName( "WORKDIR", "/log/Applog", szFileName);
    GetSysDate( szDate );
    strcat( szFileName, szDate );
    if( (fd = fopen( szFileName, "a+") ) == NULL )
    {
        WriteLog( ERROR , "fopen [%s] err",szFileName);
        return( FAIL );
    }
    fprintf( fd , "%s==================\n", szTitle );
    fprintf( fd , "=========报文协议部分=========\n" );
    fprintf( fd, "iFskId(接入平台ID)[%d]  ", ptApp->iFskId );
    fprintf( fd, "iModuleId(模块号)[%d]   ", ptApp->iModuleId );
    fprintf( fd, "iChannelId(通道号)[%d]  \n", ptApp->iChannelId );
    fprintf( fd, "iCallType(呼叫类型)[%d]  ", ptApp->iCallType );
    fprintf( fd, "iSteps(报文同步序号)[%d]  ", ptApp->iSteps );
    fprintf( fd, "lTransDataIdx(交易数据索引号)[%ld]\n", ptApp->lTransDataIdx );
    fprintf( fd, "szMsgVer(终端报文协议版本)[%02x%02x]  ", ptApp->szMsgVer[0], ptApp->szMsgVer[1] );
    fprintf( fd, "szAppVer(终端应用脚本版本)[%02x%02x%02x%02x]  ", 
                 ptApp->szAppVer[0], ptApp->szAppVer[1], 
                 ptApp->szAppVer[2], ptApp->szAppVer[3] );
    fprintf( fd, "szPosCodeVer(终端程序版本)[%s]  ", ptApp->szPosCodeVer );
    fprintf( fd, "szPosType(终端型号)[%s]  \n",  ptApp->szPosType );
    

    fprintf( fd , "=========主被叫号码=========\n" );
    fprintf( fd, "szCalledTelByTerm(终端上送的被叫中心号码)[%s]  \n", ptApp->szCalledTelByTerm );
    fprintf( fd, "szCalledTelByNac(网控器上送的被叫中心号码)[%s]  \n", ptApp->szCalledTelByNac );
    fprintf( fd, "szCallingTel(终端主叫号码)[%s]  \n", ptApp->szCallingTel );

    fprintf( fd , "=========交易路由信息=========\n" );
    fprintf( fd, "szSourceTpdu(源地址)[%02x%02x]  \n", ptApp->szSourceTpdu[0] & 0xFF, ptApp->szSourceTpdu[1] & 0xFF );
    fprintf( fd, "szTargetTpdu(目的地址)[%02x%02x]  \n", ptApp->szTargetTpdu[0] & 0xFF, ptApp->szTargetTpdu[1] & 0xFF );
    fprintf( fd, "lProcToAccessMsgType(接入层接收交易类型应答消息类型)[%ld]  \n", ptApp->lProcToAccessMsgType );
    fprintf( fd, "lPresentToProcMsgType(交易处理层接收应答消息类型)[%ld]  \n", ptApp->lPresentToProcMsgType );
    fprintf( fd, "lAccessToProcMsgType(交易处理层接收交易请求消息类型)[%ld]  \n", ptApp->lAccessToProcMsgType );
    fprintf( fd, "lProcToPresentMsgType(业务提交层接收交易请求消息类型)[%ld]  \n", ptApp->lProcToPresentMsgType );
    fprintf( fd, "szIp(请求方IP)[%s]  \n", ptApp->szIp );

    fprintf( fd , "=========终端采集信息=========\n" );
    fprintf( fd, "iTransNum(数量)[%d]  \n", ptApp->iTransNum );
    fprintf( fd, "szFinancialCode(金融应用号)[%s]  \n", ptApp->szFinancialCode );
    fprintf( fd, "szBusinessCode(商务应用号)[%s]  \n", ptApp->szBusinessCode );
    fprintf( fd, "szInDate(日期)[%s]  \n", ptApp->szInDate );
    fprintf( fd, "szInMonth(年月)[%s]  \n", ptApp->szInMonth );
    fprintf( fd, "szUserData(用户输入数据)[%s]  \n", ptApp->szUserData );

    fprintf( fd, "iMenuNum(动态菜单个数)[%d]  iMenuRecNo动态菜单对应的菜单标识)/iMenuItem(选中的动态菜单项)[", ptApp->iMenuNum );
    for( i=0; i<ptApp->iMenuNum; i++ )
    {
        fprintf( fd, "%d/%d ", ptApp->iMenuRecNo[i], ptApp->iaMenuItem[i] );
    }
    fprintf( fd, "]\n" );

    fprintf( fd, "iStaticMenuId(静态菜单ID)[%d]  \n", ptApp->iStaticMenuId );
    fprintf( fd, "iStaticMenuItem(选中的静态菜单项)[%d]  \n", ptApp->iStaticMenuItem );
    fprintf( fd, "szStaticMenuOut(选中的静态菜单内容)[%s]  \n", ptApp->szStaticMenuOut );
    fprintf( fd, "lRate(0x36号指令，读取利率输出)[%ld]  \n", ptApp->lRate );
    fprintf( fd, "szTermRetCode(所有更新类指令的返回结果)[%s]  \n", ptApp->szTermRetCode );

    fprintf( fd, "=========收单方(商户终端)属性=========  \n" );
    fprintf( fd, "szPsamNo(安全模块号)[%s]  \n", ptApp->szPsamNo );
    fprintf( fd, "szTermSerialNo(终端硬件序列号)[%s]  \n", ptApp->szTermSerialNo );
    fprintf( fd, "szDeptNo(机构号)[%s]  \n", ptApp->szDeptNo );
    fprintf( fd, "szDeptDetail(机构层级信息)[%s]  \n", ptApp->szDeptDetail );
    fprintf( fd, "szShopNo(商户号)[%s]  \n", ptApp->szShopNo );
    fprintf( fd, "szPosNo(终端号)[%s]  \n", ptApp->szPosNo );
    fprintf( fd, "szAcqBankId(收单行)[%s]  \n", ptApp->szAcqBankId );    
    fprintf( fd, "lBatchNo(交易批次号)[%ld]  \n", ptApp->lBatchNo );
    fprintf( fd, "lPosTrace(终端流水号)[%ld]  \n", ptApp->lPosTrace );
    fprintf( fd, "lOldPosTrace(原POS流水号)[%ld]  \n", ptApp->lOldPosTrace );
    fprintf( fd, "szPosDate(终端交易日期)[%s]  \n", ptApp->szPosDate );
    fprintf( fd, "szPosTime(终端交易时间)[%s]  \n", ptApp->szPosTime );
    fprintf( fd, "szShopName(商户名称)[%s]  \n", ptApp->szShopName );
    fprintf( fd, "szShopType(商户类型)[%s]  \n", ptApp->szShopType );   
    fprintf( fd, "lMarketNo(市场类别)[%ld]  \n", ptApp->lMarketNo );
    fprintf( fd, "szOperNo(操作员编号)[%s]  \n", ptApp->szOperNo );
    fprintf( fd, "szEntryMode(服务点输入方式码)[%s]  \n", ptApp->szEntryMode );    

    fprintf( fd, "=========交易要素=========  \n" );
    fprintf( fd, "iTransType(交易类型)[%ld]  \n", ptApp->iTransType );
    fprintf( fd, "iOldTransType(原交易类型)[%ld]  \n", ptApp->iOldTransType );
    fprintf( fd, "iBusinessType(业务类型)[%ld]  \n", ptApp->iBusinessType );
    fprintf( fd, "szTransCode(终端交易代码)[%s]  \n", ptApp->szTransCode );
    fprintf( fd, "szNextTransCode(后续交易代码)[%s]  \n", ptApp->szNextTransCode );
    fprintf( fd, "szTransName(交易名称)[%s]  \n", ptApp->szTransName );
    fprintf( fd, "szAmount(交易金额或应缴金额)[%s]  \n", ptApp->szAmount );   
    fprintf( fd, "szAddiAmount(余额或手续费)[%s]  \n", ptApp->szAddiAmount );
    fprintf( fd, "iCommandNum(操作码个数)[%d]  \n", ptApp->iCommandNum );
    BcdToAsc( (uchar*)ptApp->szCommand, ptApp->iCommandLen*2, LEFT_ALIGN,  (uchar*)szTmpBuf);
    szTmpBuf[ptApp->iCommandLen*2] = 0;    
    fprintf( fd, "iCommandLen[%d] szCommand[%s]\n", ptApp->iCommandLen, szTmpBuf );
    BcdToAsc( (uchar*)ptApp->szMac, 16, LEFT_ALIGN,  (uchar*)szTmpBuf);    
    szTmpBuf[16] = 0;
    fprintf( fd, "iControlLen(控制参数长度)[%d]  ", ptApp->iControlLen );
    BcdToAsc( (uchar*)ptApp->szControlPara, ptApp->iControlLen*2, LEFT_ALIGN,  (uchar*)szTmpBuf);
    szTmpBuf[ptApp->iControlLen*2] = 0;    
    fprintf( fd, "szControlPara(控制参数)[%s]\n", szTmpBuf );
    fprintf( fd, "MAC[%s] \n", szTmpBuf );   

    fprintf( fd, "=========转出卡账户信息=========  \n" );
    fprintf( fd, "szPan(主账户)[%s]  \n", ptApp->szPan );
    fprintf( fd, "szTrack2(2磁道明文)[%s]  \n", ptApp->szTrack2 );
    fprintf( fd, "szTrack3(3磁道明文)[%s]  \n", ptApp->szTrack3 );
    fprintf( fd, "szExpireDate(卡有效期)[%s]  \n", ptApp->szExpireDate );
    fprintf( fd, "szOutBankId(发卡行ID)[%s]  \n", ptApp->szOutBankId );
    fprintf( fd, "szOutBankName(发卡行名称)[%s]  \n", ptApp->szOutBankName );
    fprintf( fd, "szOutCardName(银行卡名称)[%s]  \n", ptApp->szOutCardName );
    fprintf( fd, "cOutCardType(转出卡类型)[%c]  \n", ptApp->cOutCardType );   
    fprintf( fd, "iOutCardLevel(卡级别)[%d]  \n", ptApp->iOutCardLevel );
    fprintf( fd, "iOutCardBelong( 卡归属 )[%d]  \n", ptApp->iOutCardBelong );
    fprintf( fd, "szHolderName(持卡人姓名)[%s]  \n", ptApp->szHolderName );

    fprintf( fd, "=========转入卡账户信息=========  \n" );
    fprintf( fd, "szAccount2(转入账号)[%s]  \n", ptApp->szAccount2 );
    fprintf( fd, "szInBankId(转入卡发卡行ID)[%s]  \n", ptApp->szInBankId );
    fprintf( fd, "szInBankName(转入卡发卡行名称)[%s]  \n", ptApp->szInBankName );
    fprintf( fd, "cInCardType(转入卡类型)[%c]  \n", ptApp->cInCardType );
    fprintf( fd, "iInCardBelong(转入卡归属)[%d]  \n", ptApp->iInCardBelong );

    fprintf( fd, "=========平台、后台信息=========  \n" );
    fprintf( fd, "szHostDate(平台交易日期)[%s]  \n", ptApp->szHostDate );
    fprintf( fd, "szHostTime(平台交易时间)[%s]  \n", ptApp->szHostTime );
    fprintf( fd, "lSysTrace(平台流水号)[%ld]  \n", ptApp->lSysTrace );
    fprintf( fd, "lOldSysTrace(原平台流水号)[%ld]  \n", ptApp->lOldSysTrace );
    fprintf( fd, "szRetriRefNum(后台检索参考号)[%s]  \n", ptApp->szRetriRefNum );
    fprintf( fd, "szOldRetriRefNum(原后台检索参考号)[%s]  \n", ptApp->szOldRetriRefNum );
    fprintf( fd, "szAuthCode(授权码)[%s]  \n", ptApp->szAuthCode );
    fprintf( fd, "szRetCode(平台返回码)[%s]  \n", ptApp->szRetCode );
    fprintf( fd, "szHostRetCode(后台返回码)[%s]  \n", ptApp->szHostRetCode );
    fprintf( fd, "szHostRetMsg(后台返回信息)[%s]  \n", ptApp->szHostRetMsg );
    fprintf( fd, "szRetDesc(返回信息描述)[%s]  \n", ptApp->szRetDesc );

    fprintf( fd, "=========其他=========  \n" );    
    fprintf( fd, "szControlCode(流程控制码)[%s]  \n", ptApp->szControlCode );    
    fprintf( fd, "DataNum(数据源个数)[%d]  DataSource(数据源标识)[", ptApp->iDataNum );
    for( i=0; i< ptApp->iDataNum; i++ )
    {
        fprintf( fd, "%d ", ptApp->szDataSource[i] );
    }
    fprintf( fd, "]\n" );    

    fprintf( fd , " END ==================\n\n");
    fclose( fd );

    return(SUCC);
}


/* ----------------------------------------------------------------
 * 功    能： 打印加密机的消息体结构到App日志中
 * 输入参数：
 *            ptFace     加密机的消息体结构
 *            szTitle    字符串指针(传入报文头)
 * 输出参数： 无
 * 返 回 值： FAIL  失败/ SUCC    成功
 * 作    者：
 * 日    期： 2012/11/28
 * 调用说明：
 * 修改日志：修改日期    修改者      修改内容简述
 * ----------------------------------------------------------------
 */
    int
WriteHsmStru( ptFace, szTitle )
    T_Interface *ptFace;
    char *szTitle;
{
    FILE    *fd;
    char    szFileName[80], szDate[20];

    if(IsPrint(DEBUG_ALOG) == NO)
    {
        return FAIL;
    }

    GetFullName( "WORKDIR", "/log/Hsmlog", szFileName);
    GetSysDate( szDate );
    strcat( szFileName, szDate );
    if( (fd = fopen( szFileName, "a+") ) == NULL )
    {
        WriteLog( ERROR , "fopen [%s] err",szFileName);
        return( FAIL );
    }
    fprintf( fd , "%s==================\n", szTitle );

    fprintf( fd, "lSourceType(进程号)[%ld]  ", ptFace->lSourceType );
    fprintf( fd, "iTransType(加密类型)[%d]  ", ptFace->iTransType );
    fprintf( fd, "szPsamNo(加密类型)[%s]  ", ptFace->szPsamNo );
    fprintf( fd, "iAlog(加密类型)[%d]  ", ptFace->iAlog );
    fprintf( fd, "iPinBlock(加密类型)[%d]  ", ptFace->iPinBlock );
    fprintf( fd, "szPinKey(加密类型)[%s]  ", ptFace->szPinKey );
    fprintf( fd, "szMacKey(加密类型)[%s]  ", ptFace->szMacKey );
    fprintf( fd, "iDataLen(加密类型)[%d]  ", ptFace->iDataLen );
    fprintf( fd, "szData(加密类型)[%s]  ", ptFace->szData );
    fprintf( fd, "szReturnCode(加密类型)[%s]  ", ptFace->szReturnCode );

    fprintf( fd , " END ==================\n\n");
    fclose( fd );
    return(SUCC);
}

