/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 组织临时操作提示信息
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.1 $
 * $Log: GetTmpOperationInfo.ec,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
	EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#else
	$include sqlca;
#endif

/*******************************************************************************
 * 函数功能：组织临时操作提示信息
 * 输入参数：
 *           ptApp      -  公共数据结构指针
 *           iDataIndex -  源数据索引号 
 *           iMenuId    -  动态菜单ID号
 * 输出参数：
 *           szMenuData -  动态菜单数据
 * 返 回 值： 
 *           FAIL       -  失败
 *           >=0        -  菜单数据长度
 * 作    者：Robin
 * 日    期：2012/11/30
 * 修订日志：
 *
 ******************************************************************************/
int GetTmpOperationInfo( ptApp, iDataIndex, szOutData )
T_App   *ptApp;
int     iDataIndex;
char   *szOutData;
{
    EXEC SQL BEGIN DECLARE SECTION;
        int iOperIndex;
        struct T_OPERATION_INFO{
            int     iOperIndex;
            char    szOpFlag[2];
            int     iModuleNum;
            char    szInfo1Format[3];
            char    szInfo1[41];
            char    szInfo2Format[3];
            char    szInfo2[41];
            char    szInfo3Format[3];
            char    szInfo3[41];
            char    szUpdateDate[9];
        } tOpInfo;
    EXEC SQL END DECLARE SECTION;

    int iCurPos, iInfo1Len, iInfo2Len, iInfo3Len, iModuNum, iRet, iMsgLen;
    char szData[512], szMsg[100];

    if( ptApp->iTransType == DOWN_ALL_OPERATION || 
        ptApp->iTransType == CENDOWN_ALL_OPERATION ||
        ptApp->iTransType == AUTODOWN_ALL_OPERATION ||
        ptApp->iTransType == CENDOWN_OPERATION_INFO ||
        ptApp->iTransType == AUTODOWN_OPERATION_INFO ||
        ptApp->iTransType == DOWN_OPERATION_INFO )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 1;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0xA0;    
        iCurPos ++;

        strcpy( szMsg, "更新操作提示信息..." );
        iMsgLen = strlen(szMsg);
        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
    else if( ptApp->iTransType == DOWN_ALL_FUNCTION || 
            ptApp->iTransType == CENDOWN_FUNCTION_INFO ||
            ptApp->iTransType == AUTODOWN_FUNCTION_INFO ||
            ptApp->iTransType == DOWN_FUNCTION_INFO )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 1;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0xA0;    
        iCurPos ++;

        strcpy( szMsg, "更新功能提示信息..." );
        iMsgLen = strlen(szMsg);
        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
    else if( ptApp->iTransType == DOWN_ALL_ERROR || 
            ptApp->iTransType == CENDOWN_ERROR ||
            ptApp->iTransType == AUTODOWN_ERROR ||
            ptApp->iTransType == DOWN_ERROR )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 1;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0xA0;    
        iCurPos ++;

        strcpy( szMsg, "更新错误提示信息..." );
        iMsgLen = strlen(szMsg);
        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
    else if( ptApp->iTransType == DOWN_ALL_PRINT || 
            ptApp->iTransType == CENDOWN_PRINT_INFO ||
            ptApp->iTransType == AUTODOWN_PRINT_INFO ||
            ptApp->iTransType == DOWN_PRINT_INFO )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 1;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0xA0;    
        iCurPos ++;

        strcpy( szMsg, "更新打印记录信息..." );
        iMsgLen = strlen(szMsg);
        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
    else if( ptApp->iTransType == DOWN_ALL_MENU ||
            ptApp->iTransType == CENDOWN_MENU ||
            ptApp->iTransType == AUTODOWN_MENU ||
            ptApp->iTransType == DOWN_MENU )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 1;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0xA0;    
        iCurPos ++;

        strcpy( szMsg, "更新应用菜单..." );
        iMsgLen = strlen(szMsg);
        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
    else if( ptApp->iTransType == DOWN_ALL_TERM ||
            ptApp->iTransType == CENDOWN_TERM_PARA ||
            ptApp->iTransType == AUTODOWN_TERM_PARA ||
            ptApp->iTransType == DOWN_TERM_PARA )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 1;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0xA0;    
        iCurPos ++;

        strcpy( szMsg, "更新终端参数..." );
        iMsgLen = strlen(szMsg);
        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
    else if( ptApp->iTransType == DOWN_ALL_PSAM || 
            ptApp->iTransType == CENDOWN_PSAM_PARA ||
            ptApp->iTransType == AUTODOWN_PSAM_PARA ||
            ptApp->iTransType == DOWN_PSAM_PARA )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 1;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0xA0;    
        iCurPos ++;

        strcpy( szMsg, "更新安全模块参数..." );
        iMsgLen = strlen(szMsg);
        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
/*
    // 汇款查询从后台查询到转入方相关信息，第二个临时操作提示信息
    else if( ptApp->iTransType == TRAN_OUT_OTHER_CALC_FEE && iDataIndex == 0 )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 2;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0x01;    
        iCurPos ++;

        sprintf( szMsg, "收款人:%s", ptApp->szHolderName );
        iMsgLen = strlen(szMsg);

        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        //显示格式
        szData[iCurPos] = 0x20;    
        iCurPos ++;

        strcpy( szMsg, "是否重输? 1.是 0.否" );
        iMsgLen = strlen(szMsg);

        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
    // 汇款交易，临时提示信息，在打印时显示。根据后台返回码，决定打印时的提示信息
    else if( ptApp->iTransType == TRAN_OUT_OTHER )
    {
        iCurPos = 0;
        //模板数
        szData[iCurPos] = 2;
        iCurPos ++;
        
        //显示格式
        szData[iCurPos] = 0x01;    
        iCurPos ++;

        if( memcmp( ptApp->szHostRetCode, "80000", 5 ) == 0 )
        {
            strcpy( szMsg, "应答码:00-80000" );
            iMsgLen = strlen(szMsg);

            //内容长度
            szData[iCurPos] = iMsgLen;
            iCurPos ++;

            memcpy( szData+iCurPos, szMsg, iMsgLen );
            iCurPos += iMsgLen;

            //显示格式
            szData[iCurPos] = 0x20;    
            iCurPos ++;

            sprintf( szMsg, "%-20.20s %-20.20s", "主机成功，人行未知", "正在打印..." );
            iMsgLen = strlen(szMsg);
        }
        else
        {
            strcpy( szMsg, "应答码:00" );
            iMsgLen = strlen(szMsg);

            //内容长度
            szData[iCurPos] = iMsgLen;
            iCurPos ++;

            memcpy( szData+iCurPos, szMsg, iMsgLen );
            iCurPos += iMsgLen;

            //显示格式
            szData[iCurPos] = 0x20;    
            iCurPos ++;

            sprintf( szMsg, "%-20.20s %-20.20s", "交易成功", "正在打印..." );
            iMsgLen = strlen(szMsg);
        }

        //内容长度
        szData[iCurPos] = iMsgLen;
        iCurPos ++;

        memcpy( szData+iCurPos, szMsg, iMsgLen );
        iCurPos += iMsgLen;

        memcpy( szOutData, szData, iCurPos );
        
        return iCurPos;    
    }
*/

    iOperIndex = ptApp->szDataSource[iDataIndex];
    //选择操作提示信息
    EXEC SQL SELECT 
        NVL(OPER_INDEX,0),
        NVL(OP_FLAG ,' '),
        NVL(MODULE_NUM,0),
        NVL(INFO1_FORMAT,' '),
        NVL(INFO1,' '),
        NVL(INFO2_FORMAT,' '),
        NVL(INFO2,' '),
        NVL(INFO3_FORMAT,' '),
        NVL(INFO3,' '),
        NVL(UPDATE_DATE, ' ')
    INTO :tOpInfo.iOperIndex,
         :tOpInfo.szOpFlag,
         :tOpInfo.iModuleNum,
         :tOpInfo.szInfo1Format,
         :tOpInfo.szInfo1,
         :tOpInfo.szInfo2Format,
         :tOpInfo.szInfo2,
         :tOpInfo.szInfo3Format,
         :tOpInfo.szInfo3,
         :tOpInfo.szUpdateDate
    FROM operation_temp 
    WHERE OPER_INDEX = :iOperIndex;
    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptApp->szRetCode, ERR_INVALID_APP );
        WriteLog( ERROR, "data_index[%d] not exist", iDataIndex );
        return FAIL;
    }
    else if( SQLCODE )
    {
        strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "sel operation_temp fail %ld[%d]", SQLCODE,iOperIndex );
        return FAIL;
    }
    DelTailSpace( tOpInfo.szInfo1 );
    DelTailSpace( tOpInfo.szInfo2 );
    DelTailSpace( tOpInfo.szInfo3 );

    iInfo1Len = strlen( tOpInfo.szInfo1 );
    iInfo2Len = strlen( tOpInfo.szInfo2 );
    iInfo3Len = strlen( tOpInfo.szInfo3 );

    iModuNum = 0;
    if( memcmp( tOpInfo.szInfo1Format, "FF", 2 ) != 0 )
    {
        iModuNum ++;
    }
    if( memcmp( tOpInfo.szInfo2Format, "FF", 2 ) != 0 )
    {
        iModuNum ++;
    }
    if( memcmp( tOpInfo.szInfo3Format, "FF", 2 ) != 0 )
    {
        iModuNum ++;
    }

    iCurPos = 0;
    //模板数
    szData[iCurPos] = iModuNum;
    iCurPos ++;
        
    //模板1数据
    if( memcmp( tOpInfo.szInfo1Format, "FF", 2 ) != 0 )
    {
        //显示格式
        AscToBcd((uchar*)(szData+iCurPos), 2, 0, (uchar*)(tOpInfo.szInfo1Format));
        iCurPos ++;
        //内容长度
        szData[iCurPos] = iInfo1Len;
        iCurPos ++;

        if( iInfo1Len > 0 )
        {
            memcpy(szData+iCurPos, tOpInfo.szInfo1, iInfo1Len);
            iCurPos += iInfo1Len;
        }
    }

    //模板2数据
    if( memcmp( tOpInfo.szInfo2Format, "FF", 2 ) != 0 )
    {
        //显示格式
        AscToBcd((uchar*)(szData+iCurPos), 2, 0, (uchar*)(tOpInfo.szInfo2Format));
        iCurPos ++;
        //内容长度
        szData[iCurPos] = iInfo2Len;
        iCurPos ++;

        if( iInfo2Len > 0 )
        {
            memcpy(szData+iCurPos, tOpInfo.szInfo2, iInfo2Len);
            iCurPos += iInfo2Len;
        }
    }

    //模板3数据
    if( memcmp( tOpInfo.szInfo3Format, "FF", 2 ) != 0 )
    {
        //显示格式
        AscToBcd((uchar*)(szData+iCurPos), 2, 0, (uchar*)(tOpInfo.szInfo3Format));
        iCurPos ++;
        //内容长度
        szData[iCurPos] = iInfo3Len;
        iCurPos ++;

        if( iInfo3Len > 0 )
        {
            memcpy(szData+iCurPos, tOpInfo.szInfo3, iInfo3Len);
            iCurPos += iInfo3Len;
        }
    }

    memcpy( szOutData, szData, iCurPos );

    return iCurPos;
}