/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块 请求报文拆包
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.3 $
** $Log: UnpackWebReq.c,v $
** Revision 1.3  2012/12/25 07:01:15  fengw
**
** 1、修正交易类型错误赋值BUG。
**
** Revision 1.2  2012/12/21 02:03:53  fengw
**
** 1、修改GetTransTypeByTransCode函数为GetTranInfo。
** 2、修改Revision、Log格式。
**
*******************************************************************/

#define _EXTERN_

#include "comweb.h"

/****************************************************************
** 功    能：请求报文拆包
** 输入参数：
**        szReqBuf              交易请求报文
**        iLen                  报文长度
** 输出参数：
**        ptApp                 app结构指针
** 返 回 值：
**        SUCC                  成功
**        FAIL                  失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/18
** 调用说明：
**
** 修改日志：
****************************************************************/
int UnpackWebReq(T_App *ptApp, char *szReqBuf, int iLen)
{
    int     iIndex;                 /* buf索引 */
    int     iMsgCount;              /* 短信记录数 */
    
    iIndex = 0;
    
    /* 交易代码 */
    memcpy(ptApp->szTransCode, szReqBuf+iIndex, 8);
    iIndex += 8;

    /* 根据交易代码获取交易定义 */
    if(GetTranInfo(ptApp) != SUCC)
    {
        return FAIL;
    }

    /* 更新方式 */
    giDownType = szReqBuf[iIndex] - '0';
    iIndex += 1;

	switch(giDownType)
	{
        /* 更新指定终端 */
        case DOWN_SPECIFY_POS:
            memcpy(ptApp->szShopNo, szReqBuf+iIndex, 15);
            iIndex += 15;
            DelTailSpace(ptApp->szShopNo);

            memcpy(ptApp->szPosNo, szReqBuf+iIndex, 15);
            iIndex += 15;
            DelTailSpace(ptApp->szPosNo);

            iIndex += 40;

            break;
        /* 更新指定商户终端 */
        case DOWN_SPECIFY_SHOP:
            memcpy(ptApp->szShopNo, szReqBuf+iIndex, 15);
            iIndex += 15;
            DelTailSpace(ptApp->szShopNo);

            iIndex += 55;

            break;
        /* 更新所有终端 */
        case DOWN_ALL:
            iIndex += 70;

            break;
        /* 更新指定应用类型终端 */
        case DOWN_SPECIFY_TYPE:
            memcpy(ptApp->szShopNo, szReqBuf+iIndex, 10);
            iIndex += 10;

            iIndex += 60;

            break;
        /* 更新指定机构下终端 */
        case DOWN_SPECIFY_DEPT:
            memcpy(ptApp->szDeptDetail, szReqBuf+iIndex, 70);
            iIndex += 70;
            DelTailSpace(ptApp->szDeptDetail);

            break;
        default:
            strcpy(ptApp->szRetCode, ERR_UNDEF_DOWNTYPE);

            WriteLog(ERROR, "更新类型:[%d]未定义!", giDownType);

            return FAIL;
    }

    /* 下载模式 */
    giDownMode = szReqBuf[iIndex] - '0';
    iIndex += 1;

    /* 根据交易拆解报文 */
    switch(ptApp->iTransType)
    {
        case CENDOWN_TERM_PARA:             /* 更新终端参数模板 */
        case CENDOWN_PSAM_PARA:             /* 更新安全参数模板 */
            /* 更新位图 */
            memcpy(gszBitmap, szReqBuf+iIndex, 32);
            iIndex += 32;

            break;
        case CENDOWN_MENU:                  /* 更新菜单 */
        case CENDOWN_ALL_OPERATION:         /* 更新应用 */
        case CENDOWN_PAYLIST:               /* 更新账单 */
        /*
        case CENDOWN_COMM_PARA:             更新通讯参数
            break;
        */
        case CENDOWN_OPERATION_INFO:        /* 更新操作提示 */
        case CENDOWN_FUNCTION_INFO:         /* 更新功能提示 */
        case CENDOWN_PRINT_INFO:            /* 更新打印记录 */
            /* 更新位图 */
            memcpy(gszBitmap, szReqBuf+iIndex, 256);
            iIndex += 256;

            break;
        case CENDOWN_MSG:                   /* 更新短信 */
            /* 短信记录数 */
            iMsgCount = szReqBuf[iIndex] - '0';

            memcpy(gszBitmap, szReqBuf+iIndex, 1+6*iMsgCount);
            iIndex += 1+6*iMsgCount;

            break;
        case CENDOWN_FIRST_PAGE:            /* 更新首页信息 */
            /* 更新记录号 */
            memcpy(gszBitmap, szReqBuf+iIndex, 6);
            iIndex += 6;

            break;
        default:
            strcpy(ptApp->szRetCode, ERR_INVALID_TRANS);

            WriteLog(ERROR, "未定义交易类型[%d]请求报文接口!", ptApp->iTransType);

            return FAIL;
    }

    /* POS交易日期、时间 */
	GetSysDate(ptApp->szPosDate);
	GetSysTime(ptApp->szPosTime);

    /* 响应码 */
	strcpy(ptApp->szRetCode, "NN");
	strcpy(ptApp->szHostRetCode, "NN");

    /* 呼叫类型 */
	ptApp->iCallType = CALLTYPE_CENTER;

	return SUCC;
}
