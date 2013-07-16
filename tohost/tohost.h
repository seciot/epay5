/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：后台通讯程序的头文件
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision $
 * $Log: tohost.h,v $
 * Revision 1.3  2013/06/14 02:03:31  fengw
 *
 * 1、增加签到、余额查询、消费交易报文组包、拆包处理。
 *
 * Revision 1.2  2012/12/13 01:51:17  linxiang
 * *** empty log message ***
 *
 * ----------------------------------------------------------------
 */
#ifndef _TOHOST_H_
#define _TOHOST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "user.h"
#include "libdb.h"
#include "libpub.h"
#include "app.h"
#include "EpayShm.h"
#include "errcode.h"
#include "8583.h"

#define CONFIG_FILE "Setup.ini"
#define CONFIG_SECTION "SECTION_COMMUNICATION"

#define GetCharParam(szItem, szValue) \
    do{ \
        if( ReadConfig( CONFIG_FILE, CONFIG_SECTION, szItem, szValue ) != SUCC ) \
        { \
            WriteLog( ERROR, "Setup参数配置错误[%s]", szItem ); \
            return; \
        } \
    }while(0)

#define GetIntParam(szItem, iValue) \
    do{ \
        char szValue[80]; \
        if( ReadConfig( CONFIG_FILE, CONFIG_SECTION, szItem, szValue ) != SUCC ) \
        { \
            WriteLog( ERROR, "Setup参数配置错误[%s]", szItem ); \
            return; \
        } \
        iValue = atoi(szValue); \
    }while(0)

#define GetLongParam(szItem, lValue) \
    do{ \
        char szValue[80]; \
        if( ReadConfig( CONFIG_FILE, CONFIG_SECTION, szItem, szValue ) != SUCC ) \
        { \
            WriteLog( ERROR, "Setup参数配置错误[%s]", szItem ); \
            return; \
        } \
        lValue = atol(szValue); \
    }while(0)
    
#define PACK_TYPE           0           /* 报文组包 */
#define UNPACK_TYPE         1           /* 报文拆包 */

typedef int (*PF)(MsgRule*, T_App*, ISO_data*);     // 定义函数指针类型

/* 报文处理函数指针结构定义 */
typedef struct
{
    int     iTransType;                     /* 交易类型 */
    char    szTransName[64+1];              /* 交易名称 */
    PF      pFuncPack;                      /* 组包处理函数指针 */
    PF      pFuncUnpack;                    /* 拆包处理函数指针 */
} T_PkgProc;
    
#ifndef _EXTERN_

    /* 报文处理函数定义 */
    /* 签到交易 */
    extern int LoginPack(MsgRule*, T_App*, ISO_data*);
    extern int LoginUnpack(MsgRule*, T_App*, ISO_data*);
    
    /* 余额查询交易 */
    extern int InqueryPack(MsgRule*, T_App*, ISO_data*);
    extern int InqueryUnpack(MsgRule*, T_App*, ISO_data*);

    /* 消费交易 */
    extern int PurchasePack(MsgRule*, T_App*, ISO_data*);
    extern int PurchaseUnpack(MsgRule*, T_App*, ISO_data*);

    /* 冲正交易 */
    extern int AutovoidPack(MsgRule*, T_App*, ISO_data*);
    extern int AutovoidUnpack(MsgRule*, T_App*, ISO_data*);

    /* 消费撤销交易 */
    extern int PurcancelPack(MsgRule*, T_App*, ISO_data*);
    extern int PurcancelUnpack(MsgRule*, T_App*, ISO_data*);

    /* 退货交易 */
    extern int RefundPack(MsgRule*, T_App*, ISO_data*);
    extern int RefundUnpack(MsgRule*, T_App*, ISO_data*);

    /* 预授权交易 */
    extern int PreauthPack(MsgRule*, T_App*, ISO_data*);
    extern int PreauthUnpack(MsgRule*, T_App*, ISO_data*);

    /* 预授权撤销交易 */
    extern int PrecancelPack(MsgRule*, T_App*, ISO_data*);
    extern int PrecancelUnpack(MsgRule*, T_App*, ISO_data*);

    /* 预授权完成交易 */
    extern int ConfirmPack(MsgRule*, T_App*, ISO_data*);
    extern int ConfirmUnpack(MsgRule*, T_App*, ISO_data*);

    /* 报文处理函数数组 */
    T_PkgProc gtaPkgProc[]=
    {
        {LOGIN,         "签到交易",         LoginPack,      LoginUnpack},
    	{INQUERY,       "余额查询交易",     InqueryPack,    InqueryUnpack},
    	{PURCHASE,      "消费交易",         PurchasePack,   PurchaseUnpack},
    	{AUTO_VOID,     "冲正交易",         AutovoidPack,   AutovoidUnpack},
    	{PUR_CANCEL,    "消费撤销交易",     PurcancelPack,  PurcancelUnpack},
    	{REFUND,        "退货交易",         RefundPack,     RefundUnpack},
    	{PRE_AUTH,      "预授权交易",       PreauthPack,    PreauthUnpack},
    	{PRE_CANCEL,    "预授权撤销交易",   PrecancelPack,  PrecancelUnpack},
    	{CONFIRM,       "预授权完成交易",   ConfirmPack,    ConfirmUnpack},
        {0,             "无效交易",         NULL,           NULL}
    };
#else
    extern T_PkgProc  gtaPkgProc[];
#endif

#endif
