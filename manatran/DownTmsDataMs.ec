/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:TMS相关函数

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.1 $
$Log: DownTmsDataMs.ec,v $
Revision 1.1  2012/12/18 04:57:16  wukj
*** empty log message ***


*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
        EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

/*****************************************************************
** 功    能:TMS通知参数下载, TMS参数是民生银行标准
** 输入参数:
        
** 输出参数:
           tmsdata 
           tms_len
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownTmsDataMs( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[20+1];
        char     szPosNo[20+1];

        char    szSysCode[8+1];       /*平台标识码*/
        char    szDownBitMap[2+1];    /*下载内容标志*/
        char    szDownFileName[72+1]; /*下载文件名*/
        char    szValidDate[8+1];         /*限制日期*/
        char    szAppName[72+1];          /*应用标识*/
        char    szDownTypeFlag[4+1];  /*下载时机标示*/
        char    szTmsTelNo1[20+1];        /*TMS电话号码1*/
        char    szTmsTelNo2[20+1];        /*TMS电话号码2*/
        char    szTmsIpPort1[30+1];       /*TMS IP和端口1*/
        char    szTmsIpPort2[30+1];       /*TMS IP和端口2*/
        char    szDownTime[14+1];         /*TMS下载时间*/
    EXEC SQL END DECLARE SECTION;
    int iPos = 0;
    char szTmsPara[310+1];
    memset(szTmsPara, 0, sizeof(szTmsPara));
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));
    strcpy(szShopNo, ptAppStru->szShopNo);
    strcpy(szPosNo, ptAppStru->szPosNo);
        
    EXEC SQL SELECT NOTICE_BTIME,NOTICE_EDATE
    INTO   :szDownTime,
           :szValidDate
    FROM   TM_VPOS_INFO
    WHERE  TRIM(SHOPNO) = :szShopNo
             AND    TRIM(POSNO)  = :szPosNo
             AND    NOTICE_FLAG = '1'; 
    if( SQLCODE )
    {
        WriteLog( ERROR, "TM_VPOS_INFO %d", SQLCODE );
        return( FAIL );
    }
    
    WriteLog( TRACE, "szDownTime=[%s]",szDownTime);
    memcpy(szTmsPara+iPos,szDownTime ,14);
    iPos +=14;
    WriteLog( TRACE, "szValidDate=[%s]",szValidDate);
    memcpy(szTmsPara+iPos,szValidDate ,8);
    iPos +=8;
WriteLog( TRACE, "szTmsTelNo1=[%s]",szTmsTelNo1);
    sprintf(szTmsPara+iPos,"%-40.40s",szTmsTelNo1);
    iPos +=40;
WriteLog( TRACE, "szTmsTelNo2=[%s]",szTmsTelNo2);
    sprintf(szTmsPara+iPos,"%-40.40s",szTmsTelNo2);
    iPos +=40;
        
    ptAppStru->iTmsLen = iPos;
    memcpy(ptAppStru->szTmsData, szTmsPara, iPos);
    ptAppStru->szTmsData[iPos]=0;
WriteLog( TRACE, "shopno=[%s],posno=[%s]",szShopNo,szPosNo);        
    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}
