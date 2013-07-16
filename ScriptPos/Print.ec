/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：打印相关函数
 * 创 建 人：Robin
 * 创建日期：2012/12/11
 *
 * $Revision: 1.6 $
 * $Log: Print.ec,v $
 * Revision 1.6  2013/02/21 06:50:25  fengw
 *
 * 1、修改打印交易金额、手续费金额数据格式。
 *
 * Revision 1.5  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 * Revision 1.4  2012/12/25 08:24:32  wukj
 * 根据终端保本版本判断打印长度限制
 *
 * Revision 1.3  2012/12/21 06:53:07  wukj
 * 新增注释
 *
 * Revision 1.2  2012/12/21 03:57:11  chenrb
 * 删除ChangeAmount，调用库函数中的ChgAmtZeorToDot。且不兼容低版本
 *
 * Revision 1.1  2012/12/21 02:58:00  wukj
 * 修改文件名
 *
 * Revision 1.6  2012/12/18 09:14:02  wukj
 * *** empty log message ***
 *
 * Revision 1.5  2012/12/12 07:43:54  wukj
 * *** empty log message ***
 *
 * Revision 1.4  2012/12/12 07:17:56  wukj
 * *** empty log message ***
 *
 * Revision 1.3  2012/12/12 07:10:39  wukj
 * *** empty log message ***
 *
 * Revision 1.2  2012/12/12 07:10:04  wukj
 * 规范化改写
 *
 * Revision 1.1  2012/12/12 02:22:02  chenrb
 * 初始版本
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#endif

/*****************************************************************
** 功    能:格式化卡号
** 输入参数:
           iDispMode 是否屏蔽卡号，YES-屏蔽  NO-不屏蔽 
           szPan 
** 输出参数:
           szOutData 格式化之后的卡号
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
void ChangePan( int iDispMode, char *szPan, char *szOutData )
{
    char    szTmpStr[100], szNewPan[20];
    int    iLen, i;

    memset( szTmpStr, 0, 100 );
    if( iDispMode == YES )
    {
        strcpy( szNewPan, szPan );
        iLen = strlen(szPan)-10;
        for( i=0; i<iLen; i++ )
        {
            szNewPan[6+i] = '*';    
        }
        sprintf( szTmpStr, "%4.4s %4.4s %4.4s %s", szNewPan, szNewPan+4, 
            szNewPan+8, szNewPan+12 );
    }
    else
    {
        sprintf( szTmpStr, "%4.4s %4.4s %4.4s %s", szPan, szPan+4, 
            szPan+8, szPan+12 );
    }
    
    strcpy( szOutData, szTmpStr );

    return;
}

/*****************************************************************
** 功    能: 用户自定义打印数据 
** 输入参数:
            ptAppStru 
** 输出参数:
            szPrintData  打印数据
            iPrintNum    打印数据个数
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int UserGetPrintData( ptAppStru, szPrintData, iPrintNum )
T_App    *ptAppStru;
unsigned char    *szPrintData;
int *iPrintNum;
{
    int iPrintLen, iNum, iCurPos, i, iLastLen;
    char szTmpStr[100], szData[1024];

    iPrintLen = 0;
    iCurPos = 0;
    iNum = 2;    //打印份数
    /*打印控制符号(3 bytes):%Bn表示第n份标题，第1行居中
                  %FF为正文
                  %En表示第n份落款，最后1行居中
      模版记录号(1 bytes)
      打印信息:打印信息若为"FFFF" ，表示使用菜单显示内容替换
    */
    /*标题 & 落款*/
    for( i=1; i<=iNum; i++ )
    {
        /* 标题 */
        memcpy( szData+iCurPos, "%B", 2 );    //控制符
        iCurPos += 2;
        szData[iCurPos] = i+'0';
        iCurPos ++;
        szData[iCurPos] = PRINT_TITLE1;        //标题模版记录号
        iCurPos ++;
        szData[iCurPos] = 0x00;            //记录结束
        iCurPos ++;

        /* 落款 */
        memcpy( szData+iCurPos, "%E", 2 );    //控制符
        iCurPos += 2;
        szData[iCurPos] = i+'0';
        iCurPos ++;
        if( i == 1 )
        {
            szData[iCurPos] = PRINT_SHOP_SLIP;    //落款模版记录号
        }
        else
        {
            szData[iCurPos] = PRINT_HOLDER_SLIP;    //落款模版记录号
        }
        iCurPos ++;
        szData[iCurPos] = 0x00;            //记录结束
        iCurPos ++;
    }

    //商户号
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_SHOP_NO;    //打印信息记录号
    iCurPos ++;
    memcpy( szData+iCurPos, ptAppStru->szShopNo, strlen(ptAppStru->szShopNo) );    //打印信息
    iCurPos += strlen(ptAppStru->szShopNo);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //商户名称
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_SHOP_NAME;    //打印信息记录号
    iCurPos ++;
    memcpy( szData+iCurPos, ptAppStru->szShopName, 
            strlen( ptAppStru->szShopName) );//打印信息
    iCurPos += strlen(ptAppStru->szShopName);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //终端号
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_TERM_NO;    //打印信息记录号
    iCurPos ++;
    memcpy( szData+iCurPos, ptAppStru->szPosNo, strlen(ptAppStru->szPosNo) );    //打印信息
    iCurPos += strlen(ptAppStru->szPosNo);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;
    
    //交易类型
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_TRANS_TYPE;    //打印信息记录号
    iCurPos ++;
    memcpy( szData+iCurPos, ptAppStru->szTransName, 
        strlen(ptAppStru->szTransName) );    //打印信息，取菜单标题
    iCurPos += strlen(ptAppStru->szTransName);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //卡号
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_CARD_NO;    //打印信息记录号
    iCurPos ++;
    ChangePan( giDispMode, ptAppStru->szPan, szTmpStr );
    sprintf( szData+iCurPos, "%s", szTmpStr );    //打印信息
    iCurPos += strlen(szTmpStr);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //交易金额
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_AMOUNT;    //打印信息记录号
    iCurPos ++;
    ChgAmtZeroToDot( ptAppStru->szAmount, 1, szTmpStr );
    sprintf( szData+iCurPos, "%s", szTmpStr );//打印信息
    iCurPos += strlen(szTmpStr);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //交易时间
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_DATE_TIME;    //打印信息记录号
    iCurPos ++;
    sprintf( szData+iCurPos, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s", 
        ptAppStru->szHostDate, ptAppStru->szHostDate+4, ptAppStru->szHostDate+6,
        ptAppStru->szHostTime, ptAppStru->szHostTime+2, ptAppStru->szHostTime+4);    //打印信息
    iCurPos += 19;
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //交易流水号
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_TRACE;    //打印信息记录号
    iCurPos ++;
    sprintf( szTmpStr, "%06ld", ptAppStru->lPosTrace);    //打印信息
    memcpy( szData+iCurPos, szTmpStr, strlen(szTmpStr) );
    iCurPos += strlen(szTmpStr);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    iLastLen = iCurPos;
        
    //备注0
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_NOTE0;    //打印信息记录号
    iCurPos ++;
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //备注1
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_NOTE1;    //打印信息记录号
    iCurPos ++;
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //备注2
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_NOTE2;    //打印信息记录号
    iCurPos ++;
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //备注3
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_NOTE3;    //打印信息记录号
    iCurPos ++;
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;
        
    iPrintLen = iCurPos;

    *iPrintNum = iNum;
    memcpy( szPrintData, szData, iPrintLen );

    return iPrintLen;
}

/*****************************************************************
** 功    能: 用户自定义打印数据 
** 输入参数:
            ptAppStru 
** 输出参数:
            szPrintData  打印数据
            iPrintNum    打印数据个数
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
GetStaticPrintData( ptAppStru, szPrintData, iPrintNum )
T_App    *ptAppStru;
unsigned char    *szPrintData;
int *iPrintNum;
{
    int iPrintLen, iNum, iCurPos, i;
    char szTmpStr[100], szData[512], szBeginDate[9], szEndDate[9];

    iPrintLen = 0;
    iCurPos = 0;
    iNum = 1;    //打印份数
    /*打印控制符号(3 bytes):%Bn表示第n份标题，第1行居中
                  %FF为正文
                  %En表示第n份落款，最后1行居中
      模版记录号(1 bytes)
      打印信息:打印信息若为"FFFF" ，表示使用菜单显示内容替换
    */
    /*标题 & 落款*/
    for( i=1; i<=iNum; i++ )
    {
        /* 标题 */
        memcpy( szData+iCurPos, "%B", 2 );    //控制符
        iCurPos += 2;
        szData[iCurPos] = i+'0';
        iCurPos ++;
        szData[iCurPos] = PRINT_TITLE1;        //标题模版记录号
        iCurPos ++;
        szData[iCurPos] = 0x00;            //记录结束
        iCurPos ++;

        /* 落款 */
        memcpy( szData+iCurPos, "%E", 2 );    //控制符
        iCurPos += 2;
        szData[iCurPos] = i+'0';
        iCurPos ++;
        if( i == 1 )
        {
            szData[iCurPos] = PRINT_SHOP_SLIP;    //落款模版记录号
        }
        else
        {
            szData[iCurPos] = PRINT_HOLDER_SLIP;    //落款模版记录号
        }
        iCurPos ++;
        szData[iCurPos] = 0x00;            //记录结束
        iCurPos ++;
    }

    //商户名称
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_SHOP_NAME;    //打印信息记录号
    iCurPos ++;
    memcpy( szData+iCurPos, ptAppStru->szShopName, 
            strlen( ptAppStru->szShopName) );//打印信息
    iCurPos += strlen(ptAppStru->szShopName);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //终端号
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_TERM_NO;    //打印信息记录号
    iCurPos ++;
    memcpy( szData+iCurPos, ptAppStru->szPosNo, strlen(ptAppStru->szPosNo) );    //打印信息
    iCurPos += strlen(ptAppStru->szPosNo);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //交易汇总
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_BLANK;        //打印信息记录号
    iCurPos ++;
    if( strcmp( ptAppStru->szInDate, "FFFFFFFF" ) == 0 )
    {
        strcpy( szBeginDate, "20090101" );
    }
    else
    {
        strcpy( szBeginDate, ptAppStru->szInDate );
    }
    if( strcmp( ptAppStru->szHostDate, "FFFFFFFF" ) == 0 )
    {
        GetSysDate( szEndDate );
    }
    else
    {
        strcpy( szEndDate, ptAppStru->szHostDate );
    }
    sprintf( szTmpStr, "日期:%8.8s-%8.8s", szBeginDate, szEndDate );
    memcpy( szData+iCurPos, szTmpStr, strlen(szTmpStr) );    //打印信息
    iCurPos += strlen(szTmpStr);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;
    
    //标题
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_BLANK;        //打印信息记录号
    iCurPos ++;
    strcpy( szTmpStr, "交易 笔数(笔)  金额(元)" );
    memcpy( szData+iCurPos, szTmpStr, strlen(szTmpStr) );    //打印信息
    iCurPos += strlen(szTmpStr);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    //消费
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_BLANK;    //打印信息记录号
    iCurPos ++;
    sprintf( szTmpStr, "消费:%16.16s", ptAppStru->szReserved+25 );
    memcpy( szData+iCurPos, szTmpStr, strlen(szTmpStr) );    //打印信息
    iCurPos += strlen(szTmpStr);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;
    
    //退货
    memcpy( szData+iCurPos, "%FF", 3 );    //控制符
    iCurPos += 3;
    szData[iCurPos] = PRINT_BLANK;    //打印信息记录号
    iCurPos ++;
    sprintf( szTmpStr, "退货:%16.16s", ptAppStru->szReserved+46 );
    memcpy( szData+iCurPos, szTmpStr, strlen(szTmpStr) );    //打印信息
    iCurPos += strlen(szTmpStr);
    szData[iCurPos] = 0x00;            //记录结束
    iCurPos ++;

    iPrintLen = iCurPos;

    *iPrintNum = iNum;
    memcpy( szPrintData, szData, iPrintLen );

    return iPrintLen;
}

/*****************************************************************
** 功    能: 取打印数据 
** 输入参数:
            ptAppStru 
** 输出参数:
            szPrintData  打印数据
            iPrintNum    打印数据个数
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int GetPrintData( ptAppStru, iIndex, szPrintData, iPrintNum )
T_App    *ptAppStru;
int    iIndex;
unsigned char    *szPrintData;
int *iPrintNum;
{
    EXEC SQL BEGIN DECLARE SECTION;
        int    iPrintModule, iPrintRecNo, iDataIndex;
        struct T_PRINT_MODULE {
            int     iModuleId;
            char    szDescribe[41];
            int     iPrintNum;
            int     iTitle1;
            int     iTitle2;
            int     iTitle3;
            int     iSign1;
            int     iSign2;
            int     iSign3;
            int     iRecNum;
            char    szRecNo[81];
        }tPrintModule;
        
    EXEC SQL END DECLARE SECTION;

    int iPrintLen, iNum, iCurPos, i, iRet, iLastLen;
    long lAmt, lFee;
    char szTmpStr[100], szAmtStr[13], szData[1024], szBuf[200];
    char szRecNo[200];
    int  iTooLong;

    if( ptAppStru->iTransType == REPRINT || ptAppStru->iTransType == QUERY_LAST_DETAIL )
    {
        iPrintModule = ptAppStru->iTransNum;
    }
    iPrintModule = ptAppStru->szDataSource[iIndex];

    //未定义打印模板号，采用硬编码方式实现打印数据组织
    if( iPrintModule == 0 )
    {
        if( ptAppStru->iTransType == QUERY_TOTAL )
        {
            iPrintLen = GetStaticPrintData( ptAppStru, szPrintData, iPrintNum );
        }
        else
        {
            iPrintLen = UserGetPrintData( ptAppStru, szPrintData, iPrintNum );
        }
        return iPrintLen;
    }

    //获取打印模板
    EXEC SQL SELECT 
        MODULE_ID,
        NVL(DESCRIBE,' '),
        NVL(PRINT_NUM, 1),
        NVL(TITLE1, 0),
        NVL(TITLE2, 0),
        NVL(TITLE3, 0),
        NVL(SIGN1, 0),
        NVL(SIGN2, 0),
        NVL(SIGN3,0),
        NVL(REC_NUM, 1),
        NVL(REC_NO, ' ')
    INTO 
        :tPrintModule.iModuleId,
        :tPrintModule.szDescribe,
        :tPrintModule.iPrintNum,
        :tPrintModule.iTitle1,
        :tPrintModule.iTitle2,
        :tPrintModule.iTitle3,
        :tPrintModule.iSign1,
        :tPrintModule.iSign2,
        :tPrintModule.iSign3,
        :tPrintModule.iRecNum,
        :tPrintModule.szRecNo
    FROM print_module
    WHERE module_id = :iPrintModule;

    if( SQLCODE == SQL_NO_RECORD )
    {
        WriteLog( ERROR, "PrintModule[%d] not exist", iPrintModule );
        iPrintLen = UserGetPrintData( ptAppStru, szPrintData, iPrintNum );
        return iPrintLen;
    }
    else if( SQLCODE )
    {
        WriteLog( ERROR, "get print module fail %ld", SQLCODE );
        iPrintLen = UserGetPrintData( ptAppStru, szPrintData, iPrintNum );
        return iPrintLen;
    }

    iPrintLen = 0;
    iLastLen = 0;
    iCurPos = 0;
    iNum = tPrintModule.iPrintNum;
    /*打印控制符号(3 bytes):%Bn表示第n份标题，第1行居中
                  %FF为正文
                  %En表示第n份落款，最后1行居中
      模版记录号(1 bytes)
      打印信息:打印信息若为"FFFF" ，表示使用菜单显示内容替换
    */
    /*标题 & 落款*/
    for( i=1; i<=iNum; i++ )
    {
        /* 标题 */
        memcpy( szData+iCurPos, "%B", 2 );    //控制符
        iCurPos += 2;
        szData[iCurPos] = i+'0';
        iCurPos ++;
        switch ( i ){
        case 1:
            szData[iCurPos] = tPrintModule.iTitle1;//标题1模版记录号
            break;
        case 2:
            szData[iCurPos] = tPrintModule.iTitle2;//标题2模版记录号
            break;
        case 3:
            szData[iCurPos] = tPrintModule.iTitle3;//标题3模版记录号
            break;
        }
        iCurPos ++;
        szData[iCurPos] = 0x00;            //记录结束
        iCurPos ++;

        /* 落款 */
        memcpy( szData+iCurPos, "%E", 2 );    //控制符
        iCurPos += 2;
        szData[iCurPos] = i+'0';
        iCurPos ++;
        switch ( i ){
        case 1:
            szData[iCurPos] = tPrintModule.iSign1;//落款1模版记录号
            break;
        case 2:
            szData[iCurPos] = tPrintModule.iSign2;//落款2模版记录号
            break;
        case 3:
            szData[iCurPos] = tPrintModule.iSign3;//落款3模版记录号
            break;
        }
        iCurPos ++;
        szData[iCurPos] = 0x00;            //记录结束
        iCurPos ++;
    }

    AscToBcd( (unsigned char*)(tPrintModule.szRecNo),tPrintModule.iRecNum*2, 0 ,(unsigned char*)szRecNo);
    
    iLastLen = iCurPos;
    //正文
    for( i=0; i<tPrintModule.iRecNum; i++ )
    {
        memcpy( szData+iCurPos, "%FF", 3 );    //控制符
        iCurPos += 3;
        szData[iCurPos] = (unsigned char)szRecNo[i];    //打印数据记录号
        iCurPos ++;
        iPrintRecNo = (unsigned char)szRecNo[i];

        EXEC SQL SELECT nvl(data_index, 0) 
                INTO :iDataIndex
        FROM print_info
        WHERE rec_no = :iPrintRecNo;
        if( SQLCODE == SQL_NO_RECORD )
        {
            WriteLog( ERROR, "print rec_no[%d] not exist", iPrintRecNo );    
            iDataIndex = 0;
        }
        else if( SQLCODE )
        {
            WriteLog( ERROR, "get data_index[%ld] fail %ld", iPrintRecNo, SQLCODE );
            iDataIndex = 0;
        }

        //根据数据索引取打印信息
        if( iDataIndex > 0 )
        {
            memset( szTmpStr, 0, 100 );
            switch ( iDataIndex ){
            case HOST_DATETIME_IDX:
                sprintf( szTmpStr, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s", 
                         ptAppStru->szHostDate, ptAppStru->szHostDate+4, 
                         ptAppStru->szHostDate+6, ptAppStru->szHostTime, 
                         ptAppStru->szHostTime+2, ptAppStru->szHostTime+4 );
                break;
            case POS_DATETIME_IDX:
                sprintf( szTmpStr, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s", 
                         ptAppStru->szPosDate, ptAppStru->szPosDate+4, 
                         ptAppStru->szPosDate+6, ptAppStru->szPosTime, 
                         ptAppStru->szPosTime+2, ptAppStru->szPosTime+4 );
                break;
            case PAN_IDX:
                ChangePan( giDispMode, ptAppStru->szPan, szTmpStr );
                break;
            case AMOUNT_IDX:
                ChgAmtZeroToDot( ptAppStru->szAmount, 1, szBuf );
                sprintf(szTmpStr, "%s元", szBuf);
                break;
            case TRANS_NAME_IDX:
                if( ptAppStru->iTransType == REPRINT || ptAppStru->iTransType == QUERY_LAST_DETAIL )
                {
                    strcpy( szTmpStr, ptAppStru->szReserved );
                }
                else
                {
                    strcpy( szTmpStr, ptAppStru->szTransName );
                }
                break;
            case POS_TRACE_IDX:
                if( ptAppStru->iTransType == REPRINT || ptAppStru->iTransType == QUERY_LAST_DETAIL )
                {
                    sprintf(szTmpStr, "%06ld", ptAppStru->lOldPosTrace);
                }
                else
                {
                    sprintf(szTmpStr, "%06ld", ptAppStru->lPosTrace);
                }
                break;
            case RETRI_REF_NUM_IDX:
                strcpy( szTmpStr, ptAppStru->szRetriRefNum );
                break;
            case AUTH_CODE_IDX:
                strcpy( szTmpStr, ptAppStru->szAuthCode);
                break;
            case SHOP_NO_IDX:
                strcpy( szTmpStr, ptAppStru->szShopNo );
                break;
            case POS_NO_IDX:
                strcpy( szTmpStr, ptAppStru->szPosNo );
                break;
            case SHOP_NAME_IDX:
                strcpy( szTmpStr, ptAppStru->szShopName );
                break;
            case PSAM_NO_IDX:
                strcpy( szTmpStr, ptAppStru->szPsamNo );
                break;
            case SYS_TRACE_IDX:
                sprintf(szTmpStr, "%06ld", ptAppStru->lSysTrace);
                break;
            case ACCOUNT2_IDX:
                ChangePan( giDispMode, ptAppStru->szAccount2, 
                    szTmpStr );
                break;
            case OLD_POS_TRACE_IDX:
                if( ptAppStru->iTransType == REPRINT || ptAppStru->iTransType == QUERY_LAST_DETAIL )
                {
                    sprintf(szTmpStr, "%06ld", ptAppStru->lRate);
                }
                else
                {
                    sprintf(szTmpStr, "%06ld", ptAppStru->lOldPosTrace);
                }
                break;
            case OLD_RETRI_REF_NUM_IDX:
                strcpy( szTmpStr, ptAppStru->szOldRetriRefNum );
                break;
            case FINANCIAL_CODE_IDX:
                strcpy( szTmpStr, ptAppStru->szFinancialCode);
                break;
            case BUSINESS_CODE_IDX:
                strcpy( szTmpStr, ptAppStru->szBusinessCode);
                break;
            case HOST_DATE_IDX:
                sprintf( szTmpStr, "%4.4s/%2.2s/%2.2s", ptAppStru->szHostDate, 
                         ptAppStru->szHostDate+4, ptAppStru->szHostDate+6 );
                break;
            case POS_DATE_IDX:
                sprintf( szTmpStr, "%4.4s/%2.2s/%2.2s", ptAppStru->szPosDate, 
                         ptAppStru->szPosDate+4, ptAppStru->szPosDate+6 );
                break;
            case HOST_TIME_IDX:
                sprintf( szTmpStr, "%2.2s:%2.2s:%2.2s", ptAppStru->szHostTime, 
                         ptAppStru->szHostTime+2, ptAppStru->szHostTime+4 );
                break;
            case POS_TIME_IDX:
                sprintf( szTmpStr, "%2.2s:%2.2s:%2.2s", ptAppStru->szPosTime, 
                         ptAppStru->szPosTime+2, ptAppStru->szPosTime+4 );
                break;
            case EXPIRE_DATE_IDX:
                sprintf( szTmpStr, "%2.2s/%2.2s", ptAppStru->szExpireDate+2, 
                         ptAppStru->szExpireDate );
                break;
            case SHOP_TYPE_IDX:
                strcpy( szTmpStr, ptAppStru->szShopType );
                break;
            case TRANS_NUM_IDX:
                sprintf(szTmpStr, "%06ld", ptAppStru->iTransNum);
                break;
            case RATE_IDX:
                sprintf(szTmpStr, "%ld", ptAppStru->lRate);
                break;
            case TRACK2_IDX:
                strcpy( szTmpStr, ptAppStru->szTrack2);
                break;
            case TRACK3_IDX:
                strcpy( szTmpStr, ptAppStru->szTrack3);
                break;
            case MAC_IDX:
                BcdToAsc( (unsigned char*)(ptAppStru->szMac), 8, LEFT_ALIGN ,(unsigned char*)szTmpStr);
                szTmpStr[8] = ' ';
                BcdToAsc((unsigned char*)(ptAppStru->szMac+4), 8, LEFT_ALIGN, (unsigned char*)szBuf);
                memcpy( szTmpStr+9, szBuf, 8 );
                szTmpStr[17] = 0;
                break;
            case RET_CODE_IDX:
                strcpy( szTmpStr, ptAppStru->szRetCode);
                break;
            case RET_DESC_IDX:
                strcpy( szTmpStr, ptAppStru->szRetDesc );
                break;
            case BATCH_NO_IDX:
                sprintf( szTmpStr, "%06ld", ptAppStru->lBatchNo );
                break;
            case RESERVED_IDX:
                strcpy( szTmpStr, ptAppStru->szReserved );
                break;
            case ADDI_AMOUNT_IDX:
                ChgAmtZeroToDot( ptAppStru->szAddiAmount, 1, szBuf );
                sprintf(szTmpStr, "%s元", szBuf);
                break;
            case TOTAL_AMT_IDX:
                memcpy( szTmpStr, ptAppStru->szAddiAmount, 12 );
                szTmpStr[12] = 0;
                lFee = atoll( szTmpStr );

                memcpy( szTmpStr, ptAppStru->szAmount, 12 );
                szTmpStr[12] = 0;
                lAmt = atoll( szTmpStr );
                sprintf(szAmtStr, "%012u", lAmt+lFee);
                ChgAmtZeroToDot( szAmtStr, 1, szTmpStr );
                strcat( szTmpStr, "元" );
                break;
            case IN_BANK_ID_IDX:
                strcpy( szTmpStr, ptAppStru->szInBankId);
                break;
            case OUT_BANK_ID_IDX:
                strcpy( szTmpStr, ptAppStru->szOutBankId);
                break;
            case ACQ_BANK_ID_IDX:
                strcpy( szTmpStr, ptAppStru->szAcqBankId);
                break;
            case IN_BANK_NAME_IDX:
                strcpy( szTmpStr, ptAppStru->szInBankName);
                break;
            case OUT_BANK_NAME_IDX:
                strcpy( szTmpStr, ptAppStru->szOutBankName);
                break;
            case MENU_NAME_IDX:
                if( ptAppStru->iTransType == REPRINT || ptAppStru->iTransType == QUERY_LAST_DETAIL )
                {
                    strcpy( szTmpStr, ptAppStru->szReserved );
                }
                else
                {
                    strcpy( szTmpStr, "FFFF" );
                }
                break;
            case HOLDER_NAME_IDX:
                sprintf( szTmpStr, "*%s", ptAppStru->szHolderName+2 );
                break;
            case HAND_INPUT_DATE_IDX:
                sprintf( szTmpStr, "%4.4s/%2.2s/%2.2s", ptAppStru->szInDate, 
                         ptAppStru->szInDate+4, ptAppStru->szInDate+6 );
                break;
            default:
                strcpy( szTmpStr, "XXXX" );
                break;
            }

            sprintf( szData+iCurPos, "%s", szTmpStr );
            iCurPos += strlen(szTmpStr);
        }

        szData[iCurPos] = 0x00;            //记录结束
        iCurPos ++;

        iLastLen = iCurPos;
        //2010版本打印数据可以超过254
        if( memcmp(ptAppStru->szMsgVer, "\x20\x10", 2) < 0 )
        {
            if( iCurPos <= 254 )
            {
                iLastLen = iCurPos;
            }
            else
            {
                iTooLong = 1;
                break;
            }
        }
        else
        {
            iLastLen = iCurPos;
        }
    }

    /* 重打印需要打上重打印的标识 */
    if( ptAppStru->iTransType == REPRINT || ptAppStru->iTransType == QUERY_LAST_DETAIL )
    {
        memcpy( szData+iCurPos, "%FF", 3 );    //控制符
        iCurPos += 3;
        szData[iCurPos] = PRINT_REPRINT;    //打印数据记录号
        iCurPos ++;
        szData[iCurPos] = 0x00;        //记录结束*/
        iCurPos ++;

        iLastLen = iCurPos;

        //2010版本打印数据可以超过254
        if( memcmp(ptAppStru->szMsgVer, "\x20\x10", 2) < 0 )
        {
            if( iCurPos <= 254 )
            {
                iLastLen = iCurPos;
            }
            else
            {
                iTooLong = 1;
            }
        }
        else
        {
            iLastLen = iCurPos;
        }
    }


    iPrintLen = iCurPos;

    if( iTooLong == 1 )
    {
        WriteLog( ERROR, "打印数据太长，请调整 %d %d", iCurPos, iLastLen );
        iPrintLen = iLastLen;
    }

    *iPrintNum = iNum;
    memcpy( szPrintData, szData, iPrintLen );

    return iPrintLen;
}
