/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：返回信息组织
 *         
 * 创 建 人：Robin
 * 创建日期：2012/11/19
 *
 * $Revision: 1.4 $
 * $Log: Return.ec,v $
 * Revision 1.4  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 * Revision 1.3  2013/01/06 05:33:06  fengw
 *
 * 1、删除GetReturnDescribe函数，替换为epay库中GetResult函数。
 *
 * Revision 1.2  2012/12/21 06:53:07  wukj
 * 新增注释
 *
 * Revision 1.1  2012/12/21 02:58:00  wukj
 * 修改文件名
 *
 * Revision 1.6  2012/12/20 07:00:56  wukj
 * *** empty log message ***
 *
 * Revision 1.5  2012/12/20 01:15:10  wukj
 * 删除ChangeAmountReal函数
 *
 * Revision 1.4  2012/12/19 09:07:20  chenrb
 * *** empty log message ***
 *
 * Revision 1.3  2012/12/19 02:13:21  wukj
 * 规范化改写
 *
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
** 功    能:根据交易类型组织交易结果信息 
** 输入参数:
           ptAppStru
** 输出参数:
           szRetData   显示数据
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin
** 日    期:2009/08/25
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**
****************************************************************/
int GetReturnData( ptAppStru, szRetData )
T_App    *ptAppStru;
unsigned char    *szRetData;
{
    int iRetLen, iTotal, iBegin, iEnd;
    int iFormatLen;
    unsigned long    lAmt, lAmount;
    char    szTmpStr[512], szBuf[512];
    int iRet;
    if( memcmp(ptAppStru->szRetCode, TRANS_SUCC, 2) == 0 )
    {
        memset(szBuf,0x00,sizeof(szBuf));
        //根据数据库配置,格式化返回信息
        iRet = FormatRetDesc(ptAppStru,szBuf,&iFormatLen);
        if(iRet == SUCC)
        {
            iRetLen = 0;
            //显示方式
            szRetData[iRetLen] = ptAppStru->cDispType;
            //szRetData[iRetLen] = 0;
            iRetLen ++;
        
            //应答码         
            memcpy( szRetData+iRetLen, ptAppStru->szRetCode, 2 );
            iRetLen += 2;
            //返回信息
            memcpy(szRetData+3,szBuf,iFormatLen);
            iRetLen += iFormatLen;
            return iRetLen;
        }
        //如果失败或未配置则取默认值

        switch( ptAppStru->iTransType){
        case AUTO_VOID:
            strcpy(ptAppStru->szRetDesc, "自动冲正成功");
            break;
        case CENDOWN_FUNCTION_INFO:
        case AUTODOWN_FUNCTION_INFO:
        case DOWN_FUNCTION_INFO:
            strcpy(ptAppStru->szRetDesc, "更新功能提示成功");
            break;
        case CENDOWN_OPERATION_INFO:
        case AUTODOWN_OPERATION_INFO:
        case DOWN_OPERATION_INFO:
            strcpy(ptAppStru->szRetDesc, "更新操作提示成功");
            break;
        case CENDOWN_PRINT_INFO:
        case AUTODOWN_PRINT_INFO:
        case DOWN_PRINT_INFO:
            strcpy(ptAppStru->szRetDesc, "更新打印记录成功");
            break;
        case CENDOWN_TERM_PARA:
        case AUTODOWN_TERM_PARA:
        case DOWN_TERM_PARA:
            strcpy(ptAppStru->szRetDesc, "更新终端参数成功");
            break;
        case CENDOWN_PSAM_PARA:
        case AUTODOWN_PSAM_PARA:
        case DOWN_PSAM_PARA:
            strcpy(ptAppStru->szRetDesc, "更新安全参数成功");
            break;
        case CENDOWN_ERROR:
        case AUTODOWN_ERROR:
        case DOWN_ERROR:
            strcpy(ptAppStru->szRetDesc, "更新错误信息成功");
            break;
        case CENDOWN_MENU:
        case AUTODOWN_MENU:
        case DOWN_MENU:
            strcpy(ptAppStru->szRetDesc, "更新菜单成功");
            break;
        case DOWN_ALL_PSAM:
            strcpy(ptAppStru->szRetDesc, "更新应用成功");
            break;
        case QUERY_DETAIL_SELF:
        case QUERY_DETAIL_OTHER:
            strcpy(ptAppStru->szRetDesc, "查询交易明细成功");
            break;
        case TERM_REGISTER:
            strcpy(ptAppStru->szRetDesc, "终端预登记成功,请到WEB管理平台审核");
            break;
        case DOWN_TMS:
            strcpy(ptAppStru->szRetDesc, "下载TMS参数成功");
        case DOWN_TMS_END:
            strcpy(ptAppStru->szRetDesc, "更新TMS参数成功");
            break;
        default:
            strcpy(ptAppStru->szRetDesc, "交易成功");
            break;
        }
    }
    else
    {
        GetResult(ptAppStru->szRetCode, ptAppStru->szRetDesc);
    }

    iRetLen = 0;
    //显示方式
    szRetData[iRetLen] = ptAppStru->cDispType;
    iRetLen ++;

    //应答码    
    memcpy( szRetData+iRetLen, ptAppStru->szRetCode, 2 );
    iRetLen += 2;

    if( memcmp(ptAppStru->szRetCode, TRANS_SUCC, 2) != 0 )
    {
        //应答信息
#ifdef    ADD_0A_LINE
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;
#endif
        memcpy( szRetData+iRetLen, ptAppStru->szRetDesc, 
            strlen(ptAppStru->szRetDesc) );
        iRetLen += strlen(ptAppStru->szRetDesc);
        return iRetLen;
    }

    switch ( ptAppStru->iTransType)
    {
    //电信查应缴费
    case CHINATELECOM_INQ:
    //电信CDMA查应缴费
    case CHINATELECOM_CDMA_INQ:
    //电费查应缴费
    case ELECTRICITY_INQ:
    //煤气查应缴费
    case GAS_INQ:
    //水费查应缴费
    case WATER_INQ:
    //网通查应缴费
    case NETCOM_INQ:
    //联通查应缴费
    case CHINAUNICOM_INQ:
    //移动查应缴费
    case CHINAMOBILE_INQ:
    //查应缴费-测试
    case TEST_INQ:
    //查电信当前余额
    case CHINATELECOM_QUERY:
    //查电信CDMA当前余额
    case CHINATELECOM_CDMA_QUERY:
    //查联通当前余额
    case CHINAUNICOM_QUERY:
    //查移动当前余额
    case CHINAMOBILE_QUERY:
    //网通查当前余额
    case NETCOM_QUERY:
        sprintf( szTmpStr, "用户名:%s", ptAppStru->szHolderName);
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;

        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        if( ptAppStru->iTransType == CHINATELECOM_QUERY ||
            ptAppStru->iTransType == CHINAUNICOM_QUERY ||
            ptAppStru->iTransType == NETCOM_QUERY ||
            ptAppStru->iTransType == CHINATELECOM_CDMA_QUERY ||
            ptAppStru->iTransType == CHINAMOBILE_QUERY )
        {

            sprintf( szTmpStr, "充值号码:%s", ptAppStru->szBusinessCode);
            sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
            iRetLen += 20;

            memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
            iRetLen ++;

            sprintf( szTmpStr, "当前余额:%u.%02ld元", atoll(ptAppStru->szAddiAmount)/100,atoll(ptAppStru->szAddiAmount)%100);
        }
        else
        {
            sprintf( szTmpStr, "缴费号码:%s", ptAppStru->szBusinessCode);
            sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
            iRetLen += 20;

            memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
            iRetLen ++;

            sprintf( szTmpStr, "应缴金额:%u.%02ld元", atoll(ptAppStru->szAmount)/100,atoll(ptAppStru->szAmount)%100);
        }
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        // 操作提示
        strcpy( szTmpStr, "确认键继续 取消键退出" );
        sprintf( szRetData+iRetLen, "%-21.21s", szTmpStr );
        iRetLen += 21;
        break;
    //计算手续费
    case PAY_CREDIT_QUERY:
    case TRANS_QUERY:
    case TRAN_IN_QUERY:
    case TRAN_OUT_QUERY:
        // 收款人
        if( strlen(ptAppStru->szHolderName) > 0 )
        {
            sprintf( szTmpStr, "收款人:%s", ptAppStru->szHolderName);
        }
        else
        {
            strcpy( szTmpStr, "收款卡号" );
        }
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        // 收款帐号
        sprintf( szTmpStr, "%s", ptAppStru->szAccount2);
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        /* 手续费 */
        lAmt = atoll( ptAppStru->szAddiAmount);
        sprintf( szTmpStr, "手续费:%u.%02ld元", lAmt/100L, lAmt%100L );
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        // 操作提示
        strcpy( szTmpStr, "确认键继续 取消键退出" );
        sprintf( szRetData+iRetLen, "%-21.21s", szTmpStr );
        iRetLen += 21;

        break;
    case TRAN_OTHER_QUERY:
        //第一个交易结果显示
        if( ptAppStru->iSteps == 1 )
        {
            // 收款人
            if( strlen(ptAppStru->szHolderName) > 0 )
            {
                sprintf( szTmpStr, "收款人:%s", ptAppStru->szHolderName );
                sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
                iRetLen += 20;
                memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
                iRetLen ++;
            }
            else
            {
                // 收款方信息
                strcpy( szTmpStr, "收款方:" );
                sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
                iRetLen += 20;
                memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
                iRetLen ++;
            }

            //查询到转入行名称，则显示
            if( strlen(ptAppStru->szReserved) > 0 )
            {
                //转入行
                strcpy( szTmpStr, ptAppStru->szReserved);
                sprintf( szRetData+iRetLen, "%-40.40s", szTmpStr );
                iRetLen += 40;
                memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
                iRetLen ++;
            }

            // 操作提示
            strcpy( szTmpStr, "确认键继续 取消键退出" );
            sprintf( szRetData+iRetLen, "%-21.21s", szTmpStr );
            iRetLen += 21;
        }
        else
        {
            // 收款帐号
            sprintf( szTmpStr, "%s", ptAppStru->szAccount2);
            sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
            iRetLen += 20;
            memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
            iRetLen ++;

            // 金额 
            sprintf( szTmpStr, "金  额:%lf元", atoll(ptAppStru->szAmount)/100.00);
            sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
            iRetLen += 20;
            memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
            iRetLen ++;

            //手续费
            lAmt = atoll( ptAppStru->szAddiAmount);
            sprintf( szTmpStr, "手续费:%u.%02ld元", lAmt/100L, lAmt%100L );
            sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
            iRetLen += 20;
            memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
            iRetLen ++;

            // 操作提示
            strcpy( szTmpStr, "确认键继续 取消键退出" );
            sprintf( szRetData+iRetLen, "%-21.21s", szTmpStr );
            iRetLen += 21;
        }

        break;
    //查余额
    case INQUERY:
        sprintf( szRetData+iRetLen, "卡号:%15.15s", " " );
        iRetLen += 20;

        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        sprintf( szRetData+iRetLen, "%-20.20s", ptAppStru->szPan);
        iRetLen += 20;

        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;


        //sprintf(szTmpStr, "余额:%s元", atoll(ptAppStru->szAddiAmount)/100.00);
        ChgAmtZeroToDot(ptAppStru->szAddiAmount,0,szBuf); 
        sprintf(szTmpStr, "余额:%s元", szBuf);

        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        break;
    //查询流水
    case QUERY_DETAIL_SELF:
    case QUERY_DETAIL_OTHER:
    case QUERY_TODAY_DETAIL:
        memcpy( szTmpStr, ptAppStru->szReserved+6, 6 );
        szTmpStr[6] = 0;
        iTotal = atol(szTmpStr);

        memcpy( szTmpStr, ptAppStru->szReserved+12, 6 );
        szTmpStr[6] = 0;
        iBegin = atol(szTmpStr);

        memcpy( szTmpStr, ptAppStru->szReserved+18, 6 );
        szTmpStr[6] = 0;
        iEnd = atol(szTmpStr);

        if( iTotal == 0 )
        {
            strcpy( szTmpStr, "未找到符合条件的流水" );
            sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
            iRetLen += 20;
        }
        else
        {
#ifdef    ADD_0A_LINE
            memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
            iRetLen ++;
#endif
            sprintf( szTmpStr, "共%ld笔 下载%ld-%ld笔", iTotal, iBegin, iEnd );
            sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
            iRetLen += 20;
        }
        break;
    case QUERY_LAST_DETAIL:
        /* 交易名称 */
        sprintf( szTmpStr, "%-8.8s 流水%-6ld", ptAppStru->szReserved, ptAppStru->lOldPosTrace);
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;

        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        /* 第一卡号 */
        if( ptAppStru->iOldTransType == TRANS || ptAppStru->iOldTransType == TRAN_IN || 
            ptAppStru->iOldTransType == TRAN_OTHER ||
            ptAppStru->iOldTransType == TRAN_OUT || ptAppStru->iOldTransType == PAY_CREDIT )
        {
            sprintf( szTmpStr, "付%s", ptAppStru->szPan );
        }    
        else
        {
            sprintf( szTmpStr, "卡号%s", ptAppStru->szPan);
        }
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;

        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        /* 第二卡号或用户号 */
        if( ptAppStru->iOldTransType == TRANS || ptAppStru->iOldTransType == TRAN_IN || 
            ptAppStru->iOldTransType == TRAN_OTHER ||
            ptAppStru->iOldTransType == TRAN_OUT || ptAppStru->iOldTransType == PAY_CREDIT )
        {
            sprintf( szTmpStr, "收%s", ptAppStru->szAccount2);
        }    
        else if( ptAppStru->iOldTransType == PURCHASE )
        {
            sprintf( szTmpStr, "授权码:%s", ptAppStru->szAuthCode);
        }
        else
        {
            sprintf( szTmpStr, "用户号:%s", ptAppStru->szBusinessCode);
        }
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;

        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        /* 交易金额 */
        lAmt = atoll(ptAppStru->szAmount);
        sprintf( szTmpStr, "金额:%u.%02ld元", lAmt/100L, lAmt%100L );
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;

        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        /* 交易时间 */
        sprintf( szTmpStr, "%4.4s-%2.2s-%2.2s %2.2s:%2.2s:%2.2s|", ptAppStru->szPosDate, ptAppStru->szPosDate+4, 
            ptAppStru->szPosDate+6, ptAppStru->szPosTime, ptAppStru->szPosTime+2, ptAppStru->szPosTime+4 );
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
    case QUERY_TOTAL:
        memcpy( szRetData+iRetLen, ptAppStru->szReserved, 
            ptAppStru->iReservedLen );    
        iRetLen += ptAppStru->iReservedLen;
        break;
    case TRAFFIC_AMERCE_INQ:
    case TRAFFIC_AMERCE_NO_INQ:
        if( ptAppStru->iTransType == TRAFFIC_AMERCE_NO_INQ )
        {
            //车牌号
            sprintf( szTmpStr, "车牌号:%s", ptAppStru->szReserved );
        }
        else
        {
            // 当事人
            sprintf( szTmpStr, "当事人:%s", ptAppStru->szHolderName );
        }
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

         /* 罚款额 */
        sprintf( szTmpStr, "罚款额:%lf元", atoll(ptAppStru->szAmount)/100.00);
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        /* 滞纳金 */
        lAmt = atoll( ptAppStru->szAddiAmount);
        sprintf( szTmpStr, "滞纳金:%u.%02ld元", lAmt/100L, lAmt%100L );
        sprintf( szRetData+iRetLen, "%-20.20s", szTmpStr );
        iRetLen += 20;
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;

        // 操作提示
        strcpy( szTmpStr, "确认键继续 取消键退出" );
        sprintf( szRetData+iRetLen, "%-21.21s", szTmpStr );
        iRetLen += 21;

        break;
    default:
        //应答信息
#ifdef    ADD_0A_LINE
        memcpy( szRetData+iRetLen, "\x0A", 1 );    //换行
        iRetLen ++;
#endif

        memcpy( szRetData+iRetLen, ptAppStru->szRetDesc, 
            strlen(ptAppStru->szRetDesc) );
        iRetLen += strlen(ptAppStru->szRetDesc);
        break;
    }

    return iRetLen;
}


/*****************************************************************
** 功    能:将查询结果从数据库中取出，同时删除该记录 
** 输入参数:
           ptAppStru
** 输出参数:
           szRetData   结果信息
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
GetQueryResult( ptAppStru, szRetData )
T_App    *ptAppStru;
uchar    *szRetData;
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szPsamNo[17], szResult[2049];
        int    lTrace;
    EXEC SQL END DECLARE SECTION;
    char    szTmpStr[100];
    int    iLen;
    int    i, j, k, iLine, iBegin, iEnd, iTotalLine;
    
    strcpy( szPsamNo, ptAppStru->szPsamNo);
    lTrace = ptAppStru->lPosTrace;

    memcpy( szTmpStr, ptAppStru->szReserved, 6 );
    szTmpStr[6] = 0;
    iLen = atol(szTmpStr);

    EXEC SQL SELECT result 
    INTO :szResult
    FROM query_result
    WHERE psam_no = :szPsamNo AND POS_TRACE = :lTrace;
    if( SQLCODE )
    {
        WriteLog( ERROR, "select result fail %ld", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;    
    }
    szResult[iLen] = 0;
    memcpy( szRetData, szResult, iLen );

    EXEC SQL DELETE FROM query_result
    WHERE psam_no = :szPsamNo AND POS_TRACE = :lTrace;
    if( SQLCODE )
    {
        WriteLog( ERROR, "delete result fail %ld", SQLCODE );
        strcpy( ptAppStru->szRetCode, ERR_SYSTEM_ERROR );
        RollbackTran();
        return FAIL;    
    }
    CommitTran();

    return iLen;
}
