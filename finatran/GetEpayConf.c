/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 获取参数定义
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.3 $
** $Log: GetEpayConf.c,v $
** Revision 1.3  2012/11/26 01:33:05  fengw
**
** 1、修改文件名后缀为ec，重新上传该模块代码
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

/****************************************************************
** 功    能：获取终端参数定义
** 输入参数：
**        ptApp                app结构指针
** 输出参数：
**        ptEpayConf           参数定义
** 返 回 值：
**        CONF_GET_SUCC        参数查询成功
**        CONF_GET_FAIL        参数查询失败
**        CONF_NOT_FOUND       未定义参数
** 作    者：
**        fengwei
** 日    期：
**        2012/11/08
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetEpayConf(T_App *ptApp, T_EpayConf *ptEpayConf)
{
    int iRet;

    /* 读取终端参数 */
    iRet = GetPosConf(ptApp->iTransType, ptApp->szShopNo, ptApp->szPosNo, ptEpayConf);
    if(iRet != CONF_NOT_FOUND)
    {
        return iRet;
    }

    /* 读取商户参数 */
    iRet = GetShopConf(ptApp->iTransType, ptApp->szShopNo, ptEpayConf);
    if(iRet != CONF_NOT_FOUND)
    {
        return iRet;
    }

    /* 读取机构参数 */
    iRet = GetDeptConf(ptApp->iTransType, ptApp->szDeptDetail, ptEpayConf);
    if(iRet != CONF_NOT_FOUND)
    {
        return iRet;
    }

    /* 读取交易参数 */
    iRet = GetTransConf(ptApp->iTransType, ptEpayConf);
    if(iRet != CONF_NOT_FOUND)
    {
        return iRet;
    }

    return iRet;
}
