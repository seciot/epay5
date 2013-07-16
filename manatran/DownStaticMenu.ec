/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.3 $
$Log: DownStaticMenu.ec,v $
Revision 1.3  2013/02/21 06:22:52  fengw

1、修改静态菜单显示内容。

Revision 1.2  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.1  2012/12/18 10:04:56  wukj
*** empty log message ***

*******************************************************************/

# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif


int gnStaticMenuNum = 10;

/*****************************************************************
** 功    能:静态菜单下载
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        静态菜单信息
           ptAppStru->iReservedLen    静态菜单信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownStaticMenuBak( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szShopNo[20+1];
        char     szPosNo[20+1];
    EXEC SQL END DECLARE SECTION;
    int iPos = 0;
    char szTmpBuf[90+1];
    int iLen=0;
    char szStaticMenuPara[1204+1];
    memset(szStaticMenuPara, 0, sizeof(szStaticMenuPara));
    memset(szTmpBuf,0,sizeof(szTmpBuf));
    
    szStaticMenuPara[iPos] = 0x02;
    iPos++;
    
    szStaticMenuPara[iPos] = 0x01;
    iPos++;
    
    szStaticMenuPara[iPos] = 0x06;
    iPos++;
    
    //菜单ID为1的个菜单
        //子菜单索引
        szStaticMenuPara[iPos] = 0x01;
        iPos++;
        //显示标题
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        strcpy(szTmpBuf,"高明鑫");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //显示内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s\n%s\n%s\n","账号:6225885912347654","户名:高明鑫","发卡行:工商银行");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //输出内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s","6225885912347654");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        
        szStaticMenuPara[iPos] = 0x02;
        iPos++;
        //显示标题
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        strcpy(szTmpBuf,"陈瑞兵");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //显示内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s\n%s\n%s\n","账号:622577","户名:陈瑞兵","交通");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //输出内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s","622577");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
    
        szStaticMenuPara[iPos] = 0x03;
        iPos++;
        //显示标题
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        strcpy(szTmpBuf,"林熙");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //显示内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s\n%s\n%s\n","账号:622566","户名:林熙","建设");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //输出内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s","622566");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;

        szStaticMenuPara[iPos] = 0x04;
        iPos++;
        //显示标题
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        strcpy(szTmpBuf,"尹丽");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //显示内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s\n%s\n%s\n","账号:6225555","户名:尹丽","招商");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //输出内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s","6225555");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        
        szStaticMenuPara[iPos] = 0x05;
        iPos++;
        //显示标题
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        strcpy(szTmpBuf,"吴开金");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //显示内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s\n%s\n%s\n","账号:622544","户名:吴开金","民生");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //输出内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s","622544");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        
        szStaticMenuPara[iPos] = 0x06;
        iPos++;
        //显示标题
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        strcpy(szTmpBuf,"余伟明");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //显示内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s\n%s\n%s\n","账号:622533","户名:余伟明","东亚");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //输出内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s","622533");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
    
    szStaticMenuPara[iPos] = 0x02;
    iPos++;    
    szStaticMenuPara[iPos] = 0x01;
    iPos++;    
        //子菜单
        szStaticMenuPara[iPos] = 0x01;
        iPos++;
        //显示标题
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        strcpy(szTmpBuf,"许杰云");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //显示内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s\n%s\n%s\n","账号:622522","户名:许杰云","平安");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        //输出内容
        memset(szTmpBuf,0,sizeof(szTmpBuf));
        sprintf(szTmpBuf,"%s","622522");
        iLen = strlen(szTmpBuf);
        szStaticMenuPara[iPos] = iLen;
        iPos++;
        memcpy(szStaticMenuPara+iPos,szTmpBuf,iLen);
        iPos += iLen;
        
//    ptAppStru->iReservedLen = iPos+2;    
//    ptAppStru->szReserved[0] = iPos/256;
//    ptAppStru->szReserved[1] = iPos%256;
    ptAppStru->iReservedLen = iPos;
    memcpy( ptAppStru->szReserved, szStaticMenuPara, iPos );

    ptAppStru->szReserved[iPos]=0;
    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    
    return ( SUCC );
}

/*****************************************************************
** 功    能:静态菜单下载
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        静态菜单信息
           ptAppStru->iReservedLen    静态菜单信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int DownStaticMenu( ptAppStru , iDownloadNew) 
T_App    *ptAppStru;
int    iDownloadNew;
{
    EXEC SQL BEGIN DECLARE SECTION;
        struct T_MYCUSTOMER {
            char    szShopNo[16];
            char    szPosNo[16];
            char    szPan[20];
            char    szAcctName[41];
            char    szExpireDate[5];
            char    szBankName[21];
            char    szRegisterDate[9];
            int     iRecNo;
        }tMyCustomer;
        
        char    szShopNo[15+1];
        char    szPosNo[8+1];
        char    szPsamNo[17], szUpdateDate[9];
        int    iBeginRecNo, iTransType;
        int    iMaxRecNo, iNeedDownMax;
    EXEC SQL END DECLARE SECTION;
    struct T_MENU_ITEM {
        int     iItemId;                        //菜单索引
        char    szDispTitle[20+1];      //显示标题
        char    szDispData[90+1];       //显示内容
        char    szOutData[30+1];        //输出内容
    }tMenuItem;

    char szBuf[1024], szData[1024];
    int i, iCurPos, iTotalRecNum, iLastIndex, iCmdLen, iDataLen;
    int iPreCmdLen, iPreCmdNum, iRet;
    char szPreCmd[512], szFlag[2];
    
    memset(szBuf, 0, sizeof(szBuf));
    strcpy( szPsamNo, ptAppStru->szPsamNo );
    strcpy( szShopNo,ptAppStru->szShopNo);
    strcpy( szPosNo,ptAppStru->szPosNo);
    //中心发起交易，用于判断是否送comweb
    strcpy( ptAppStru->szAuthCode, "YES" );

    //终端返回最后一个包交易结果，不用回送终端&comweb
    if( memcmp( ptAppStru->szTransCode, "FF", 2 ) == 0 )
    {
        strcpy( ptAppStru->szAuthCode, "NO" );
        if( memcmp( ptAppStru->szHostRetCode, "00", 2 ) == 0 )
        {
            EXEC SQL UPDATE static_menu_cfg
            set DOWN_STATIC_MENU = 'N', STATIC_MENU_recno = 0
            where psam_no = :szPsamNo and static_menu_id = 1;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update static_menu_cfg fail %d", SQLCODE );
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                RollbackTran();
                return FAIL;
            }
            CommitTran();
        }
        strcpy( ptAppStru->szRetCode, TRANS_SUCC );
        return SUCC;
    }

    //应用下载，更新当前下载步骤
    if( ptAppStru->iTransType == DOWN_STATIC_MENU &&
            memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {
        WriteLog( TRACE, "begin down %s", ptAppStru->szTransName );

        //有更新结果&trans_code前两位为00，表明记录下载刚开始，
        //置起始记录为0
        if( memcmp(ptAppStru->szHostRetCode, "NN", 2) != 0 )
        {
            EXEC SQL UPDATE static_menu_cfg
            set static_menu_recno = 0,
                down_static_menu = 'N'
            where psam_no = :szPsamNo and static_menu_id = 1;
                
            if( SQLCODE )
            {
                WriteLog( ERROR, "update static_menu_cfg fail %d", SQLCODE );
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                RollbackTran();
                return FAIL;
            }
            CommitTran();
        
        }
        else
        {
            EXEC SQL SELECT 
                       NVL(STATIC_MENU_ID,0),
                       NVL(DOWN_STATIC_MENU ,' '),
                       NVL(STATIC_MENU_RECNO ,0),
                       NVL(PSAM_NO,' ')
                     FROM  static_menu_cfg
                     WHERE PSAM_NO = :szPsamNo and static_menu_id = 1;
            if( SQLCODE == SQL_NO_RECORD)
            {
                EXEC SQL INSERT INTO STATIC_MENU_CFG (STATIC_MENU_ID, DOWN_STATIC_MENU,STATIC_MENU_RECNO,PSAM_NO)
                         VALUES (1,'N',0,:szPsamNo);
                if( SQLCODE )
                {
                    WriteLog( ERROR, "insert static_menu_cfg fail %d", SQLCODE );
                    strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                    RollbackTran();
                    return FAIL;
                }
                CommitTran();                    
            }
            else if( SQLCODE )
            {
                WriteLog( ERROR, "select static_menu_cfg fail %d", SQLCODE );
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                return FAIL;
            }
                
        }        
    }

    ptAppStru->iReservedLen = 0;


    //首个下载包
    if( memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {        
        iBeginRecNo = 0;        
    }
    else
    {
        AscToBcd( (uchar*)(ptAppStru->szTransCode), 2, 0 , (uchar*)szBuf);
        szBuf[1] = 0;
        iBeginRecNo = (uchar)szBuf[0];

        //终端更新成功，更改已下载记录号
        if( memcmp(ptAppStru->szHostRetCode, "00", 2) == 0 )
        {
            EXEC SQL UPDATE static_menu_cfg 
            set static_menu_recno = :iBeginRecNo
            where psam_no = :szPsamNo and static_menu_id = 1;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update static_menu_cfg fail %d", SQLCODE );
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                RollbackTran();
                return FAIL;
                
            }
            CommitTran();
        }
        else
        {
            WriteLog( ERROR, "终端[%s]更新失败[%s]", szPsamNo, ptAppStru->szHostRetCode );
            sprintf( ptAppStru->szPan, "终端更新结果" );
            return FAIL;
        }
    }

    // 终端选择了下载方式，以终端选择为准，否则以平台设置为准 
    if( ptAppStru->szControlCode[0] == '1' )
    {
        iDownloadNew = 1;
    }
    else if( ptAppStru->szControlCode[0] == '0' )
    {
        iDownloadNew = 0;
    }


    //增量下载
    if( iDownloadNew == 1 )
    {
        // 只需要下载应用版本号之后的数据 
        BcdToAsc( (uchar *)(ptAppStru->szAppVer), 8, 0 ,(uchar *)szUpdateDate);
        szUpdateDate[8] = 0;
    }
    //完全下载
    else
    {
        strcpy( szUpdateDate, "20000101" );
    }

    //取大记录号，用于判断是否需要通知
    //  终端进行后续下载，即决定终端的下一步操作
    EXEC SQL SELECT NVL(max(rec_no),0) into :iMaxRecNo
    FROM my_customer
    WHERE rec_no > :iBeginRecNo and
          shop_no = :szShopNo and pos_no = :szPosNo and
         register_date >= :szUpdateDate ;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "count my_customer_cur fail %d", SQLCODE );
        return FAIL;
    }
    if (iMaxRecNo == 0)
    {
        strcpy( ptAppStru->szRetCode, ERR_LAST_RECORD);
        WriteLog( ERROR, " 没有符合条件的静态菜单信息" );
        return FAIL;
    }
     /*if( iMaxRecNo < iNeedDownMax )
    {
        iNeedDownMax = iMaxRecNo;
    }
        */
    EXEC SQL DECLARE my_customer_cur cursor for
    SELECT
        shop_no,
        NVL(pan, '6225'),
        NVL(acct_name, 1),
        NVL(expire_date,'0000'),
        NVL(BANK_NAME,'银行'),
        NVL(REGISTER_DATE,'20080101'),        
        NVL(rec_no,0)
    FROM my_customer
    WHERE rec_no > :iBeginRecNo and
         shop_no = :szShopNo and pos_no = :szPosNo and
         register_date >= :szUpdateDate
    ORDER BY rec_no;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "delare my_customer_cur fail %d", SQLCODE );
        return FAIL;
    }

    EXEC SQL OPEN my_customer_cur;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "open my_customer_cur fail %d", SQLCODE );
        EXEC SQL CLOSE my_customer_cur;
        return FAIL;
    }

    iTotalRecNum = 0;
    iDataLen = 0;
    iCurPos = 0;
    //静态菜单个数
    szBuf[iCurPos]=1;
    iCurPos++;
    //静态菜单ID
    szBuf[iCurPos]=1;
    iCurPos++;
    //菜单选项个数
    iCurPos++;
    while(1)
    {
        memset(&tMyCustomer, 0, sizeof(struct T_MYCUSTOMER));
        memset(&tMenuItem, 0, sizeof(struct T_MENU_ITEM));
    
        EXEC SQL FETCH my_customer_cur 
        INTO 
            :tMyCustomer.szShopNo,
            :tMyCustomer.szPan,
            :tMyCustomer.szAcctName,
            :tMyCustomer.szExpireDate,
            :tMyCustomer.szBankName,
            :tMyCustomer.szRegisterDate,
            :tMyCustomer.iRecNo;
        if( SQLCODE == SQL_NO_RECORD )
        {
            EXEC SQL CLOSE my_customer_cur;
            break;
        }
        else if( SQLCODE )
        {
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            WriteLog( ERROR, "fetch my_customer_cur fail %d", SQLCODE );
            EXEC SQL CLOSE my_customer_cur;
            return FAIL;
        }

        if (iCurPos > 350)
        {
            EXEC SQL CLOSE my_customer_cur;
            break;
        }

        // 达到每包下载条数上限 
        if( iTotalRecNum >= gnStaticMenuNum )
        {
            EXEC SQL CLOSE my_customer_cur;
            break;
        }

        //后续记录不用下载
        if( tMyCustomer.iRecNo> iMaxRecNo )
        {
            EXEC SQL CLOSE my_customer_cur;
            break;
        }

        //该条记录不需要下载
        if( ptAppStru->szReserved[tMyCustomer.iRecNo-1] == '0' && tMyCustomer.iRecNo!= iNeedDownMax )
        {
            continue;
        }

        DelTailSpace( tMyCustomer.szPan );
        DelTailSpace( tMyCustomer.szAcctName );
        DelTailSpace( tMyCustomer.szExpireDate );
        DelTailSpace( tMyCustomer.szBankName );
        
        //子菜单索引
        tMenuItem.iItemId = tMyCustomer.iRecNo;
                
        //显示标题        
        memcpy(tMenuItem.szDispTitle,tMyCustomer.szAcctName,strlen(tMyCustomer.szAcctName));
        
        //显示内容    
        sprintf(tMenuItem.szDispData,"账号:%s\n户名:%s\n发卡行:%s\n",tMyCustomer.szPan,tMyCustomer.szAcctName,tMyCustomer.szBankName);
        //输出内容        
        sprintf(tMenuItem.szOutData,"%s",tMyCustomer.szPan);
                
        szBuf[iCurPos] = tMenuItem.iItemId;
        iCurPos ++;
        
        szBuf[iCurPos] = strlen(tMenuItem.szDispTitle);
        iCurPos ++;
        memcpy(szBuf+iCurPos, tMenuItem.szDispTitle, strlen(tMenuItem.szDispTitle));
        iCurPos += strlen(tMenuItem.szDispTitle);
        
        szBuf[iCurPos] = strlen(tMenuItem.szDispData);
        iCurPos ++;
        memcpy(szBuf+iCurPos, tMenuItem.szDispData, strlen(tMenuItem.szDispData));
        iCurPos += strlen(tMenuItem.szDispData);

        szBuf[iCurPos] = strlen(tMenuItem.szOutData);
        iCurPos ++;
        memcpy(szBuf+iCurPos, tMenuItem.szOutData, strlen(tMenuItem.szOutData));
        iCurPos += strlen(tMenuItem.szOutData);
        
        iLastIndex = tMyCustomer.iRecNo;
        iTotalRecNum ++;    
        
    }
    szBuf[2]= iTotalRecNum;
    ptAppStru->iReservedLen = iCurPos;    
    memcpy( ptAppStru->szReserved, szBuf, iCurPos );
    sprintf( ptAppStru->szPan, "下载%03d-%03d / %03d", iBeginRecNo+1, iLastIndex, iMaxRecNo );

    // 需要进行后续下载 
    if( iLastIndex < iMaxRecNo )
    {
        //后续交易代码前2位表示已下载记录最大记录号，后6位为当前交易
        //代码后6位
        memset(szBuf, 0, sizeof(szBuf));
        szBuf[0] = iLastIndex;
        szBuf[1] = 0;
        BcdToAsc( (uchar*)szBuf, 2, 0 ,(uchar*)(ptAppStru->szNextTransCode));
        memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
        ptAppStru->szNextTransCode[8] = 0;
        
        iCmdLen = 0;
        ptAppStru->iCommandNum = 0;
        if( memcmp(ptAppStru->szTransCode, "00", 2) == 0 )
        {
            //临时提示信息
            memcpy( ptAppStru->szCommand + iCmdLen, "\xAF", 1 );    
            iCmdLen += 1;
            ptAppStru->iCommandNum ++;
        }
        memcpy( ptAppStru->szCommand+iCmdLen, "\x2C\xFF", 2 );//更新静态菜单
        iCmdLen += 2;
        ptAppStru->iCommandNum ++;
        memcpy( ptAppStru->szCommand+iCmdLen, "\x8D", 1 );    //计算MAC
        iCmdLen += 1;
        ptAppStru->iCommandNum ++;
        memcpy( ptAppStru->szCommand+iCmdLen, "\x24\x03", 2 );//发送数据
        iCmdLen += 2;
        ptAppStru->iCommandNum ++;
        memcpy( ptAppStru->szCommand+iCmdLen, "\x25\x04", 2 );//接收数据
        iCmdLen += 2;
        ptAppStru->iCommandNum ++;

        ptAppStru->iCommandLen = iCmdLen;

        //需要进行后续下载，不送comweb
        strcpy( ptAppStru->szAuthCode, "NO" );
    }
    else
    {        
        memcpy( ptAppStru->szNextTransCode, "FF", 2 );
        memcpy( ptAppStru->szNextTransCode+2, 
            ptAppStru->szTransCode+2, 6 );
        ptAppStru->szNextTransCode[8] = 0;        
    }

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}
