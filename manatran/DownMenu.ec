
/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:终端下载类交易

** 函数列表:
** 创 建 人:Robin
** 创建日期:2009/08/29


$Revision: 1.8 $
$Log: DownMenu.ec,v $
Revision 1.8  2013/01/24 07:41:31  wukj
QLCODE打印格式修改为%d

Revision 1.7  2013/01/15 02:09:23  fengw

1、修改非末级菜单控制参数长度赋值。

Revision 1.6  2013/01/06 05:05:44  fengw

1、trans_def表新增excep_times字段(异常处理次数)，修改相关代码。

Revision 1.5  2013/01/05 08:39:22  wukj
*** empty log message ***

Revision 1.4  2013/01/05 06:41:24  fengw

1、修正SQL语句中字段名错误。

Revision 1.3  2012/12/26 01:53:07  wukj
当指令参数长度不为0时,需要下载指令参数

Revision 1.2  2012/12/24 04:45:03  wukj
GetCommans新增指令参数

Revision 1.1  2012/12/18 10:04:56  wukj
*** empty log message ***

*******************************************************************/
    
# include "manatran.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

/*****************************************************************
** 功    能: 菜单信息下载
** 输入参数:
           ptAppStru
** 输出参数:
           ptAppStru->szReserved        菜单信息
           ptAppStru->iReservedLen    菜单信息长度
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int
DownMenu( ptAppStru ) 
T_App    *ptAppStru;
{
    EXEC SQL BEGIN DECLARE SECTION;
        int     iLevel1, iLevel2, iLevel3;
        int    iAppType, iCount, iBeginMenu, iTransType;
        char    szPsamNo[17];
        
        struct T_APP_MENU {
            int     iMenuNo;
            int     iUpMenuNo;
            int     iAppType;
            int     iLevel1;
            int     iLevel2;
            int     iLevel3;
            char    szMenuName[21];
            char    szTransCode[9];
            char    szIsValid[2];
            char    szUpdateDate[9];
        }tAppMenu;

        
        struct T_APP_DEF {
            int     iAppType;
            char    szAppName[21];
            char    szAppDescribe[31];
            char    szAppVer[9];
        }tAppDef;


        
        struct T_TRANS_DEF {
            int     iTransType;
            char    szTransCode[9];
            char    szNextTransCode[9];
            char    szExcepHandle[2];
            int     iExcepTimes;
            char    szPinBlock[2];
            int     iFunctionIndex;
            char    szTransName[21];
            int     iTelephoneNo;
            char    szDispType[2];
            int     iToTransMsgType;
            int     iToHostMsgType;
            char    szIsVisible[2];
        } tTransDef;

        
        struct T_TRANS_COMMANDS {
            int     iTransType;
            int     iStep;
            char    szTransFlag[2];
            char    szCommand[3];
            int     iOperIndex;
            char    szAlog[9];
            char    szCommandName[31];
            char    szOrgCommand[3];
            int     iDataIndex;
        }tTransCmd;
    EXEC SQL END DECLARE SECTION;
    
    int     iMenuNo;    //菜单序号
    int     iCurPos, iMenuNum, iRet, iCmdNum, iCmdLen, iMenuLen, iDataNum, iRealType;
    char    szBuf[1024], szCmd[512], szTmpStr[20], szMenuData[1024];
    int     iPreCmdLen, iPreCmdNum;
    char    szPreCmd[512], szFlag[2], szDataSource[30];
    int     iCtlLen;
    char    szCtlPara[101];

    strcpy( szPsamNo, ptAppStru->szPsamNo );

    //中心发起交易，用于判断是否送comweb
    strcpy( ptAppStru->szAuthCode, "YES" );

    //终端返回最后一个包交易结果，不用回送终端
    if( memcmp( ptAppStru->szTransCode, "FF", 2 ) == 0 )
    {
        strcpy( ptAppStru->szAuthCode, "NO" );
        if( memcmp( ptAppStru->szHostRetCode, "00", 2 ) == 0 )
        {
            EXEC SQL UPDATE terminal
            set down_menu = 'N', menu_recno = 0
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update down_menu fail %d", SQLCODE );
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
    if( ptAppStru->iTransType == DOWN_ALL_MENU && 
            memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {
        iTransType = DOWN_ALL_MENU;

        //有更新结果&trans_code前两位为00，表明菜单下载刚开始，
        //置起始记录为0
        if( memcmp(ptAppStru->szHostRetCode, "NN", 2) != 0 )
        {
            EXEC SQL UPDATE terminal
            set all_transtype = :iTransType, menu_recno = 0,
                down_function = 'N', function_recno = 0
            WHERE psam_no = :szPsamNo;
        }
        //无更新结果&trans_code前两位为00，表明是菜单下载的断点
        //续传，不需要重置起始记录号
        else
        {
            EXEC SQL UPDATE terminal
            set all_transtype = :iTransType
            WHERE psam_no = :szPsamNo;
        }
        if( SQLCODE )
        {
            WriteLog( ERROR, "update all_transtype fail %d", SQLCODE );
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            RollbackTran();
            return FAIL;
        }
        CommitTran();
    }

    ptAppStru->iReservedLen = 0;

    EXEC SQL SELECT app_type INTO :iAppType
    FROM terminal
    WHERE psam_no = :szPsamNo;
    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptAppStru->szRetCode, ERR_INVALID_TERM );
        WriteLog( ERROR, "term[%s] not exist", szPsamNo );
        return FAIL;
    }
    else if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "get app_type fail %d", SQLCODE );
        return FAIL;
    }

    //首个下载包，需要根据control_code判断是断点续传，还是重新下载
    if( memcmp( ptAppStru->szTransCode, "00", 2 ) == 0 )
    {
        //断点续传
        if( ptAppStru->szControlCode[0] == '1' )
        {
WriteLog( TRACE, "%s 断点续传", ptAppStru->szTransName );
            EXEC SQL SELECT NVL(menu_recno,0) INTO :iBeginMenu
            FROM terminal
            WHERE psam_no = :szPsamNo;
            if( SQLCODE == SQL_NO_RECORD )
            {
                strcpy( ptAppStru->szRetCode, ERR_INVALID_TERM );
                WriteLog( ERROR, "term[%s] not exist", szPsamNo );
                return FAIL;
            }
            else if( SQLCODE )
            {
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                WriteLog( ERROR, "sel menu_recno fail %d", SQLCODE );
                return FAIL;
            }
        }
        /* 重新下载 */
        else
        {
            iBeginMenu = 0;
        }
    }
    else
    {
        AscToBcd( (uchar*)(ptAppStru->szTransCode), 2, 0 , (uchar*)szBuf );
        szBuf[1] = 0;
        iBeginMenu = (uchar)szBuf[0];

        //终端更新成功，更改已下载记录号
        if( memcmp(ptAppStru->szHostRetCode, "00", 2) == 0 )
        {
            EXEC SQL UPDATE terminal 
            set menu_recno = :iBeginMenu
            WHERE psam_no = :szPsamNo;
            if( SQLCODE )
            {
                WriteLog( ERROR, "update operate_recno fail %d", SQLCODE );
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

    /*统计总菜单个数，用于判断是否需要通知
      终端进行后续下载，即决定终端的下一步操作*/
    EXEC SQL SELECT count(*) INTO :iCount
    FROM app_menu
    WHERE app_type = :iAppType;
    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptAppStru->szRetCode, ERR_INVALID_APP );
        WriteLog( ERROR, "term[%s] app[%d] iot exist", szPsamNo, iAppType );
        return FAIL;
    }
    else if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "count app_menu fail %d", SQLCODE );
        return FAIL;
    }

    if( iBeginMenu >= iCount )
    {
        strcpy( ptAppStru->szRetCode, ERR_DOWN_FINISH );
        return FAIL;
    }

    EXEC SQL SELECT 
        APP_TYPE,
        NVL(APP_NAME,' '),
        NVL(DESCRIBE, ' '),
        NVL(APP_VER, ' ')
    INTO 
        :tAppDef.iAppType,
        :tAppDef.szAppName,
        :tAppDef.szAppDescribe,
        :tAppDef.szAppVer
    FROM app_def
    WHERE app_type = :iAppType;
    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptAppStru->szRetCode, ERR_INVALID_APP );
        WriteLog( ERROR, "term[%s] app[%d] not exist", szPsamNo, iAppType );
        return FAIL;
    }
    else if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "SELECT app_def fail %d", SQLCODE );
        return FAIL;
    }

    EXEC SQL DECLARE menu_cur cursor for
    SELECT  
        MENU_NO,
        UP_MENU_NO,
        APP_TYPE,
        NVL(LEVEL_1,0),
        NVL(LEVEL_2,1),
        NVL(LEVEL_3,2),
        NVL(MENU_NAME,' '),
        NVL(TRANS_CODE,' '),
        NVL(IS_VALID, ' '),
        NVL(UPDATE_DATE,' ')
    FROM app_menu
    WHERE app_type = :iAppType and level_1 <= 9 and level_2 <= 9 and 
          level_3 <= 9 and level_1 >= 0 and level_2 >= 0 and
          level_3 >= 0
    ORDER BY level_1, level_2, level_3;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "delare menu_cur fail %d", SQLCODE );
        return FAIL;
    }

    EXEC SQL OPEN menu_cur;
    if( SQLCODE )
    {
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "open menu_cur fail %d", SQLCODE );
        EXEC SQL CLOSE menu_cur;
        return FAIL;
    }

    iMenuNum = 0;
    iMenuNo = 0;
    iMenuLen = 0;
    while(1)
    {
        EXEC SQL FETCH menu_cur 
        INTO 
            :tAppMenu.iMenuNo,
            :tAppMenu.iUpMenuNo,
            :tAppMenu.iAppType,
            :tAppMenu.iLevel1,
            :tAppMenu.iLevel2,
            :tAppMenu.iLevel3,
            :tAppMenu.szMenuName,
            :tAppMenu.szTransCode,
            :tAppMenu.szIsValid,
            :tAppMenu.szUpdateDate;
        if( SQLCODE == SQL_NO_RECORD )
        {
            EXEC SQL CLOSE menu_cur;
            break;
        }
        else if( SQLCODE )
        {
            strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
            WriteLog( ERROR, "fetch fun_cur fail %d", SQLCODE );
            EXEC SQL CLOSE menu_cur;
            return FAIL;
        }

        iMenuNo ++;
        iLevel1 = tAppMenu.iLevel1;
        iLevel2 = tAppMenu.iLevel2;
        iLevel3 = tAppMenu.iLevel3;
        DelTailSpace(tAppMenu.szMenuName);

        //该菜单已经下载，跳过
        if( iMenuNo <= iBeginMenu )
        {
            continue;
        }

        iCurPos = 0;
        //菜单操作标识
        szBuf[iCurPos] = tAppMenu.szIsValid[0]; 
        iCurPos ++;

        //菜单级别
        szTmpStr[0] = tAppMenu.iLevel1+'0';    
        szTmpStr[1] = tAppMenu.iLevel2+'0';
        szTmpStr[2] = tAppMenu.iLevel3+'0';
        szTmpStr[3] = '0';
        AscToBcd(  (uchar*)szTmpStr, 4, 0 , (uchar*)(szBuf+iCurPos));
        iCurPos += 2;

        //交易代码
        AscToBcd( (uchar*)tAppMenu.szTransCode, 8, 0 ,  (uchar*)(szBuf+iCurPos));
        iCurPos += 4;

        /* 非末级菜单 */
        if( memcmp( tAppMenu.szTransCode, "00000000", 8 ) == 0 )
        {
            //冲正标识
            szBuf[iCurPos] = '0';
            iCurPos ++;

            //业务类型标识
            szBuf[iCurPos] = 0;
            iCurPos ++;

            //功能提示索引
            szBuf[iCurPos] = 0;
            iCurPos ++;

            //系统号码序号
            szBuf[iCurPos] = '0';
            iCurPos ++;

            //流程代码长度
            szBuf[iCurPos] = 0;
            iCurPos ++;
            
            //指令参数长度
            szBuf[iCurPos] = 0;
            iCurPos++;
        }
        /* 末级菜单，根据交易代码取之前流程 */
        else
        {
            EXEC SQL SELECT 
                TRANS_TYPE,
                TRANS_CODE,
                NVL(NEXT_TRANS_CODE, ' '),
                NVL(EXCEP_HANDLE,' '),
                NVL(excep_times, 0),
                NVL(PIN_BLOCK, ' '),
                NVL(FUNCTION_INDEX, 0),
                NVL(TRANS_NAME, ' '),
                NVL(TELEPHONE_NO, 0),
                NVL(DISP_TYPE, ' '),
                NVL(TOTRANS_MSG_TYPE, 0),
                NVL(TOHOST_MSG_TYPE, 0),
                NVL(IS_VISIBLE, ' ')
             INTO 
                :tTransDef.iTransType,
                :tTransDef.szTransCode,
                :tTransDef.szNextTransCode,
                :tTransDef.szExcepHandle,
                :tTransDef.iExcepTimes,
                :tTransDef.szPinBlock,
                :tTransDef.iFunctionIndex,
                :tTransDef.szTransName,
                :tTransDef.iTelephoneNo,
                :tTransDef.szDispType,
                :tTransDef.iToTransMsgType,
                :tTransDef.iToHostMsgType,
                :tTransDef.szIsVisible
            FROM TRANS_DEF
            WHERE trans_code = :tAppMenu.szTransCode;
            if( SQLCODE == SQL_NO_RECORD )
            {
                strcpy( ptAppStru->szRetCode, ERR_INVALID_MENU );
                WriteLog( ERROR, "trans[%s] not exist", tAppMenu.szTransCode );
                return FAIL;
            }
            else if( SQLCODE )
            {
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                WriteLog( ERROR, "SELECT trans_def [%s] fail[%d:%s]", tAppMenu.szTransCode, SQLCODE, SQLERR );
                return FAIL;
            }

            /* 终端上多个代缴费交易可对应后台一个交易，他们后三位一致 */
            iRealType = tTransDef.iTransType%1000L;
            iRet = GetCommands( iRealType, '0', 
                szCmd, &iCmdNum, &iCmdLen, szDataSource, &iDataNum ,&iCtlLen,szCtlPara);
            if( iRet != SUCC )
            {
                strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
                WriteLog( ERROR, "get command fail" );
                return FAIL;
            }

            //业务类型标识
            szBuf[iCurPos] = tTransDef.szPinBlock[0]-'0';
            iCurPos ++;

            //冲正标识
            szBuf[iCurPos] = ((tTransDef.szExcepHandle[0] - '0') << 4) | tTransDef.iExcepTimes;
            iCurPos ++;

            //功能提示索引
            szBuf[iCurPos] = tTransDef.iFunctionIndex;
            iCurPos ++;

            //系统号码序号
            szBuf[iCurPos] = tTransDef.iTelephoneNo+'0';
            iCurPos ++;

            //流程代码长度
            szBuf[iCurPos] = iCmdLen+1;
            iCurPos ++;

            //流程代码(包括指令个数)
            szBuf[iCurPos] = iCmdNum;
            iCurPos ++;
            memcpy( szBuf+iCurPos, szCmd, iCmdLen );
            iCurPos += iCmdLen;
 
            if(iCtlLen > 0)
            {
                //指令参数长度
                szBuf[iCurPos] = iCtlLen;
                iCurPos ++;
                memcpy( szBuf + iCurPos, szCtlPara, iCtlLen);
                iCurPos += iCtlLen;
            }
            else
            {
                //指令参数长度
                szBuf[iCurPos] = 0;
                iCurPos++;
            }
        }

        //显示内容长度(菜单长度)
        szBuf[iCurPos] = strlen(tAppMenu.szMenuName);
        iCurPos ++;

        //显示内容(菜单)
        memcpy( szBuf+iCurPos, tAppMenu.szMenuName, strlen(tAppMenu.szMenuName) );
        iCurPos += strlen(tAppMenu.szMenuName);

        if( (iMenuLen+iCurPos+6) <= 255 )
        {
            iMenuNum ++;
            memcpy( szMenuData+iMenuLen, szBuf, iCurPos );
            iMenuLen += iCurPos;
        }
        //数据包过长，后续菜单项由下个包下载
        else
        {
            iMenuNo --;
            break;
        }
    }

    //应用版本原值返回，如果是最后一个包再重新赋值为tAppDef.szAppVer
    memcpy( ptAppStru->szReserved, ptAppStru->szAppVer, 4 );    

    ptAppStru->szReserved[4] = '2';    //处理模式，存储后显示
    ptAppStru->szReserved[5] = iMenuNum;        //菜单个数
    memcpy( ptAppStru->szReserved+6, szMenuData, iMenuLen);    //菜单内容
    ptAppStru->iReservedLen = 6+iMenuLen;

    sprintf( ptAppStru->szPan, "下载%03d-%03d / %03d", iBeginMenu+1, iMenuNo, iCount );

    /* 需要进行后续下载 */
    if( iCount > iMenuNo )
    {
        //后续交易代码前2位表示已下载菜单数，后6位为当前交易代码后6位
        szBuf[0] = iMenuNo;
        szBuf[1] = 0;
        BcdToAsc( (uchar*)szBuf, 2, 0, (uchar*)(ptAppStru->szNextTransCode) );
        memcpy( ptAppStru->szNextTransCode+2, ptAppStru->szTransCode+2, 6 );
        ptAppStru->szNextTransCode[8] = 0;

        //后续终端流程代码(指令码集)
        iCmdLen = 0;
        ptAppStru->iCommandNum = 0;
        if( iBeginMenu == 0 )
        {
            //更新首个菜单前，需要清空菜单
            memcpy( ptAppStru->szCommand+iCmdLen, "\xBB", 1 );    
            iCmdLen += 1;
            ptAppStru->iCommandNum ++;
        }

        if( memcmp(ptAppStru->szTransCode, "00", 2) == 0 )
        {
            //临时提示信息
            memcpy( ptAppStru->szCommand+iCmdLen, "\xAF", 1 );    
            iCmdLen += 1;
            ptAppStru->iCommandNum ++;
        }
        memcpy( ptAppStru->szCommand+iCmdLen, "\x18\xFF", 2 );//更新应用菜单
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

        //需要后续下载，不送comweb
        strcpy( ptAppStru->szAuthCode, "NO" );
    }
    else
    {
        AscToBcd( (uchar*)(tAppDef.szAppVer), 8, 0,(uchar*)(ptAppStru->szReserved));  //应用版本
        //更新首个菜单前，需要清空菜单
        if( iBeginMenu == 0 )
        {
            ptAppStru->iCommandNum++;
            memcpy( szCmd, "\xBB", 1 );    //清空菜单    
            memcpy(szCmd+1, ptAppStru->szCommand, ptAppStru->iCommandLen);
            memcpy(ptAppStru->szCommand, szCmd, ptAppStru->iCommandLen+1);
            ptAppStru->iCommandLen ++;
        }

        if( ptAppStru->iTransType == DOWN_ALL_MENU )
        {
            //需要后续下载，不送comweb
            strcpy( ptAppStru->szAuthCode, "NO" );
        }
        else
        {
            memcpy( ptAppStru->szNextTransCode, "FF", 2 );
            memcpy( ptAppStru->szNextTransCode+2, 
                ptAppStru->szTransCode+2, 6 );
            ptAppStru->szNextTransCode[8] = 0;
        }
    }

    strcpy( ptAppStru->szRetCode, TRANS_SUCC );
    return ( SUCC );
}
