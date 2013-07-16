/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 交易数据索引匹配函数
** 创 建 人：冯炜
** 创建日期：2012-11-27
**
** $Revision: 1.5 $
** $Log: TdiMatch.ec,v $
** Revision 1.5  2012/12/20 09:25:54  wukj
** Revision后的美元符
**
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"
#include "dbtool.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    int     iTransDataIndex;
    char    szHostShopNo[15+1];
    char    szHostPosNo[15+1];
    long    lSendTime;
    long    lHostSysTrace;
    int     iTransTypeNum;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：设置交易数据索引匹配
** 输入参数：
**        iTdi                      交易数据索引
**        szShopNo                  商户号
**        szPosNo                   终端号
**        lSysTrace                 平台流水号
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/03
** 调用说明：
**
** 修改日志：
****************************************************************/
int SetTdiMatch(int iTdi, char *szShopNo, char* szPosNo, long lSysTrace,int iTransType)
{
    memset(szHostShopNo, 0, sizeof(szHostShopNo));
    memset(szHostPosNo, 0, sizeof(szHostPosNo));

    iTransDataIndex = iTdi;
    strcpy(szHostShopNo, szShopNo);
    strcpy(szHostPosNo, szPosNo);
    time(&lSendTime);
    lHostSysTrace = lSysTrace;
    iTransTypeNum = iTransType;

    BeginTran();

    EXEC SQL 
        SELECT  shop_no  into :szHostShopNo
        FROM  tdi_match
        WHERE shop_no = :szHostShopNo AND pos_no = :szHostPosNo AND sys_trace = :lHostSysTrace  and trans_type= :iTransTypeNum;
    if(SQLCODE == 0 )
    {
        EXEC SQL 
            DELETE FROM  tdi_match 
            WHERE shop_no = :szHostShopNo AND pos_no = :szHostPosNo AND sys_trace = :lHostSysTrace  and trans_type= :iTransTypeNum;
            
    }

    EXEC SQL
        INSERT INTO tdi_match (trans_data_index, local_date, shop_no, pos_no, send_time, sys_trace,trans_type)
        VALUES (:iTransDataIndex, TO_CHAR(SYSDATE, 'YYYYMMDD'), :szHostShopNo, :szHostPosNo,:lSendTime, :lHostSysTrace,:iTransTypeNum);
    if(SQLCODE)
    {
        RollbackTran();

        WriteLog(ERROR, "插入交易数据索引 tdi:[%d] shop_no:[%s] pos_no:[%s] sys_trace:[%ld] trans_type: [%d] 匹配记录失败!SQLCODE=%d SQLERR=%s",
                 iTransDataIndex, szHostShopNo, szHostPosNo, lHostSysTrace,iTransTypeNum, SQLCODE, SQLERR);

        return FAIL;
    }

    CommitTran();

    return SUCC;
}

/****************************************************************
** 功    能：获取交易数据索引匹配
** 输入参数：
**        szShopNo                  商户号
**        szPosNo                   终端号
**        lSysTrace                 平台流水号
**        iTimeOut                  匹配超时时间
** 输出参数：
**        无
** 返 回 值：
**        >=0                       返回匹配的交易数据索引
**        FAIL                      匹配失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/03
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetTdiMatch(char *szShopNo, char* szPosNo, long lSysTrace, int iTimeOut,int iTransType)
{
    long    lRecvTime;

    memset(szHostShopNo, 0, sizeof(szHostShopNo));
    memset(szHostPosNo, 0, sizeof(szHostPosNo));

    strcpy(szHostShopNo, szShopNo);
    strcpy(szHostPosNo, szPosNo);
    lHostSysTrace = lSysTrace;
    iTransTypeNum = iTransType;
    

    time(&lRecvTime);

    EXEC SQL
        SELECT trans_data_index, send_time
        INTO :iTransDataIndex, :lSendTime
        FROM tdi_match
        WHERE shop_no = :szHostShopNo AND pos_no = :szHostPosNo AND sys_trace = :lHostSysTrace AND trans_type = :iTransTypeNum;
    if(SQLCODE)
    {
        WriteLog(ERROR, "查询交易数据索引 shop_no:[%s] pos_no:[%s] sys_trace:[%ld] trans_type:[%d] 匹配记录失败!SQLCODE=%d SQLERR=%s",
                 szHostShopNo, szHostPosNo, lHostSysTrace,iTransTypeNum, SQLCODE, SQLERR);

        return FAIL;
    }
    
    /* 检查发送时间与接收时间是否超过超时时间 */
    if(lRecvTime - lSendTime > iTimeOut)
    {
        WriteLog(ERROR, "交易数据索引 shop_no:[%s] pos_no:[%s] sys_trace:[%ld] 匹配记录超时!SendTime:[%ld] RecvTime:[%ld] TimeOut:[%d]",
                 szHostShopNo, szHostPosNo, lHostSysTrace, lSendTime, lRecvTime, iTimeOut);

        return FAIL;
    }

    return iTransDataIndex;
}
