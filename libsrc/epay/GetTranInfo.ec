/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 根据交易码获取交易定义
** 创 建 人：冯炜
** 创建日期：2012-12-20
**
** $Revision: 1.7 $
** $Log: GetTranInfo.ec,v $
** Revision 1.7  2013/02/22 02:49:55  fengw
**
** 1、trans_def表增加business_type字段。
**
** Revision 1.6  2013/01/14 06:22:56  fengw
**
** 1、修改指令数据来源拷贝代码。
**
** Revision 1.5  2012/12/26 08:31:11  fengw
**
** 1、修正GetCommands函数调用时传入参数类型错误。
**
** Revision 1.4  2012/12/24 04:44:15  wukj
** 取指令函数新增指令参数
**
** Revision 1.3  2012/12/20 09:20:59  wukj
** *** empty log message ***
**
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "errcode.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：设置交易数据索引匹配
** 输入参数：
**        ptApp->szTransCode        交易码
** 输出参数：
**        ptApp                     app结构指针
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/20
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetTranInfo(T_App *ptApp) 
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szTransCode[8+1];                   /* 交易码 */
        int     iTransType;                         /* 交易类型 */
        char    szNextTransCode[8+1];               /* 后续交易码 */
        int     iBusinessType;                      /* 交易业务类型 */
        char    szExcepHandle[1+1];                 /* 异常处理机制 */
        char    szPinBlock[1+1];                    /* PinBlock加密算法 */
        int     iFunctionIndex;                     /* 提示信息索引 */
        char    szTransName[20+1];                  /* 交易名称 */
        int     iTelephoneNo;                       /* 电话号码 */
        char    szDispType[1+1];                    /* 刷新方式 */
        int     iToTransMsgType;                    /* 交易处理模块接收消息类型 */
        int     iToHostMsgType;                     /* 后台接口模块接收消息类型 */
    EXEC SQL END DECLARE SECTION;
    
    char    szCmd[512+1];                           /* 后续指令 */
    int     iCmdCount;                              /* 指令个数 */
    int     iLenCmd;                                /* 指令长度 */
    char    szDataSource[32+1];                     /* 指令数据源 */
    int     iDSCount;                               /* 指令数据源个数 */
    int     iCtlLen = 0;                            /*指令参数长度*/
    int     szCtlPara[101];                         /*指令参数*/

    memset(szTransCode, 0, sizeof(szTransCode));
    memcpy(szTransCode, "__", 2);
    memcpy(szTransCode+2, ptApp->szTransCode+2, 6);
    
    memset(szNextTransCode, 0, sizeof(szNextTransCode));
    memset(szExcepHandle, 0, sizeof(szExcepHandle));
    memset(szPinBlock, 0, sizeof(szPinBlock));
    memset(szTransName, 0, sizeof(szTransName));
    memset(szDispType, 0, sizeof(szDispType));
    memset(szCtlPara, 0, sizeof(szCtlPara));

    EXEC SQL
    SELECT trans_type, next_trans_code, business_type, excep_handle, pin_block, NVL(function_index, 0), trans_name,
               NVL(telephone_no, 0), disp_type, NVL(totrans_msg_type, 0), NVL(tohost_msg_type, 0)
    INTO :iTransType, :szNextTransCode, :iBusinessType, :szExcepHandle, :szPinBlock, :iFunctionIndex, :szTransName,
             :iTelephoneNo, :szDispType, :iToTransMsgType, :iToHostMsgType
    FROM trans_def
    WHERE trans_code LIKE :szTransCode;
    if(SQLCODE == SQL_NO_RECORD)
    {
        WriteLog(ERROR, "交易码[%s]未定义!SQLCODE=%d SQLERR=%s", szTransCode, SQLCODE, SQLERR);
        strcpy(ptApp->szRetCode, ERR_INVALID_TRANS);
        strcpy(ptApp->szTransName, ptApp->szTransCode);
        return FAIL;
    }
    else if(SQLCODE)
    {
        WriteLog(ERROR, "查询 交易码[%s] 交易定义失败!SQLCODE=%d SQLERR=%s", szTransCode, SQLCODE, SQLERR);
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);
        strcpy(ptApp->szTransName, ptApp->szTransCode);
        ptApp->cDispType = '1';
        return FAIL;
    }

    DelTailSpace(szNextTransCode);
    DelTailSpace(szTransName);

    /* 分地区缴费，终端上用不同的交易代码区分，以便据此从数据库表中查找对应的
           收费机构代码。因此需要在交易定义表trans_def中定义多个交易与之对应，但不
           需要定义流程代码。我们只需要定义一个基本缴费交易，
           这些分地区缴费的交易代码后三位与基本缴费保持相同。后台的交易处理流程都
       采用基本缴费的交易流程 */
    if(iTransType > 100000)
    {
        ptApp->iTransType = iTransType % 1000;
    }
    else
    {
        ptApp->iTransType = iTransType;
    }

    /* 记住原始的交易，以便后续交易需要判断原始交易时使用 */
    if(ptApp->iSteps == 1)
    {
        ptApp->iOldTransType = iTransType;
    }

    strcpy(ptApp->szNextTransCode, szNextTransCode);
    strcpy(ptApp->szTransName, szTransName);
    ptApp->cExcepHandle = szExcepHandle[0];
    ptApp->iBusinessType = iBusinessType;
    ptApp->cDispType = szDispType[0];
    ptApp->lAccessToProcMsgType = iToTransMsgType;
    ptApp->lProcToPresentMsgType = iToHostMsgType;

    /* 取本交易的后续指令 */
    memset(szCmd, 0, sizeof(szCmd));
    memset(szDataSource, 0, sizeof(szDataSource));
        
    if(GetCommands(ptApp->iTransType, '1', szCmd, &iCmdCount,
                   &iLenCmd, szDataSource, &iDSCount,&iCtlLen,szCtlPara) != SUCC)
    {
        WriteLog(ERROR, "获取交易类型[%d]后续指令失败", ptApp->iTransType);

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        return FAIL;
    }

    /* 后续指令 */
    memcpy(ptApp->szCommand+ptApp->iCommandLen, szCmd, iLenCmd);
    ptApp->iCommandLen += iLenCmd;
    ptApp->iCommandNum += iCmdCount;
    //新增指令参数
    memcpy(ptApp->szControlPara + ptApp->iControlLen, szCtlPara, iCtlLen);
    ptApp->iControlLen += iCtlLen;

    /* 数据来源 */
    memcpy(ptApp->szDataSource+ptApp->iDataNum, szDataSource, iDSCount);
    ptApp->iDataNum += iDSCount;

    /* 取后续交易的指令，附加在本交易的后续指令之后 */
    if(strlen(ptApp->szNextTransCode) == 8)
    {
        iTransType = atol(ptApp->szNextTransCode);

        memset(szCmd, 0, sizeof(szCmd));
        memset(szDataSource, 0, sizeof(szDataSource));
            
        if(GetCommands(iTransType, '0', szCmd, &iCmdCount,
                &iLenCmd, szDataSource, &iDSCount,&iCtlLen,szCtlPara) != SUCC)
        {
            WriteLog(ERROR, "获取交易类型[%d]后续指令失败", iTransType);

            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            return FAIL;
        }

        /* 后续指令 */
        memcpy(ptApp->szCommand+ptApp->iCommandLen, szCmd, iLenCmd);
        ptApp->iCommandLen += iLenCmd;
        ptApp->iCommandNum += iCmdCount;
        //新增指令参数
        memcpy(ptApp->szControlPara+ptApp->iControlLen, szCtlPara, iCtlLen);
        ptApp->iControlLen += iCtlLen;

        /* 数据来源 */
        memcpy(ptApp->szDataSource, szDataSource, iDSCount);
        ptApp->iDataNum = iDSCount;
    }

    return SUCC;
}
