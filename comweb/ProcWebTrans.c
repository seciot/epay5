/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块 web交易处理
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.3 $S
** $Log: ProcWebTrans.c,v $
** Revision 1.3  2012/12/21 02:05:32  fengw
**
** 1、将文件格式从DOS转为UNIX。
**
** Revision 1.2  2012/12/21 02:04:03  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#define _EXTERN_

#include "comweb.h"

/****************************************************************
** 功    能：web交易处理
** 输入参数：
**        ptApp                 app结构指针
** 输出参数：
**        无
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
int ProcWebTrans(T_App *ptApp)
{
    int     iRet;                           /* 函数调用结果 */

    /* 根据交易调用处理函数 */
    switch(ptApp->iTransType)
    {
        case CENDOWN_TERM_PARA:             /* 更新终端参数模板 */
            iRet = DownTermPara(ptApp);
            break;
        case CENDOWN_PSAM_PARA:             /* 更新安全参数模板 */
            iRet = DownPsamPara(ptApp);
            break;
        case CENDOWN_MENU:                  /* 更新菜单 */
            iRet = DownMenu(ptApp);
            break;
        case CENDOWN_ALL_OPERATION:         /* 更新应用 */
            iRet = DownApp(ptApp);
            break;
        case CENDOWN_PAYLIST:               /* 更新账单 */
            iRet = DownPayList(ptApp);
            break;
        /*
        case CENDOWN_COMM_PARA:             更新通讯参数
            iRet = DownCommPara(ptApp);
            break;
        */
        case CENDOWN_OPERATION_INFO:        /* 更新操作提示 */
            iRet = DownOperation(ptApp);
            break;
        case CENDOWN_FUNCTION_INFO:         /* 更新功能提示 */
            iRet = DownFunction(ptApp);
            break;
        case CENDOWN_PRINT_INFO:            /* 更新打印记录 */
            iRet = DownPrint(ptApp);
            break;
        case CENDOWN_MSG:                   /* 更新短信 */
            iRet = DownMsg(ptApp);
            break;
        case CENDOWN_FIRST_PAGE:            /* 更新首页信息 */
            iRet = DownFirstPage(ptApp);
            break;
        default:
            strcpy(ptApp->szRetCode, ERR_INVALID_TRANS);

            WriteLog(ERROR, "未定义交易类型[%d]!", ptApp->iTransType);

            return FAIL;
    }

    if(iRet != SUCC)
    {
        return FAIL;
    }
    else if(giDownMode == DOWN_MODE_IMMEDIATE)
    {
        switch(giDownType)
        {
            /* 更新指定终端 */
            case DOWN_SPECIFY_POS:
                iRet = DownByPos(ptApp, glTimeOut);
                break;
            /* 更新指定商户终端 */
            case DOWN_SPECIFY_SHOP:
                iRet = DownByShop(ptApp, glTimeOut);
                break;
            /* 更新所有终端 */
            case DOWN_ALL:
                iRet = DownByAll(ptApp, glTimeOut);
                break;
            /* 更新指定应用类型终端 */
            case DOWN_SPECIFY_TYPE:
                iRet = DownByAppType(ptApp, glTimeOut);
                break;
            /* 更新指定机构下终端 */
            case DOWN_SPECIFY_DEPT:
                iRet = DownByDept(ptApp, glTimeOut);
                break;
            default:
                strcpy(ptApp->szRetCode, ERR_UNDEF_DOWNTYPE);

                WriteLog(ERROR, "下载类型:[%d]未定义!", giDownType);

                return FAIL;
        }
    }

    if(iRet == SUCC)
    {
        strcpy(ptApp->szRetCode, TRANS_SUCC);
    }

    return iRet;
}