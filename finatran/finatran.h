/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块头文件
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.15 $
** $Log: finatran.h,v $
** Revision 1.15  2013/06/14 02:33:14  fengw
**
** 1、增加签到交易。
**
** Revision 1.14  2013/06/07 02:14:40  fengw
**
** 1、增加是否检查号码绑定判断相关代码。
**
** Revision 1.13  2013/03/29 02:52:34  fengw
**
** 1、增加EMV余额查询交易处理函数定义、EMV消费撤销交交易处理函数定义等。
**
** Revision 1.12  2013/03/11 07:09:43  fengw
**
** 1、新增EMV消费交易。
** 2、新增交易是否发送后台标志位。
**
** Revision 1.11  2013/02/21 06:37:32  fengw
**
** 1、增加跨行汇款查询、跨行汇款交易。
**
** Revision 1.10  2013/01/18 08:24:23  fengw
**
** 1、增加预授权交易处理函数定义、电信缴费交易处理函数定义等。
**
** Revision 1.9  2012/12/25 06:54:43  fengw
**
** 1、修改web交易监控通讯端口号变量类型为字符串。
**
** Revision 1.8  2012/12/17 02:38:33  fengw
**
** 1、将错误代码宏定义移至../incl/errcode.h。
**
** Revision 1.7  2012/12/14 06:31:50  fengw
**
** 1、增加SECTION_PUBLIC宏定义。
**
** Revision 1.6  2012/12/07 05:57:44  fengw
**
** 1、删除public.h的引用，增加stdio.h、string.h的引用。
**
** Revision 1.5  2012/12/07 02:01:44  fengw
**
** 1、增加web监控ip地址、端口号全局变量及参数配置宏定义。
**
** Revision 1.4  2012/12/04 01:24:28  fengw
**
** 1、替换ErrorLog为WriteLog。
**
** Revision 1.3  2012/11/26 01:33:05  fengw
**
** 1、修改文件名后缀为ec，重新上传该模块代码
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#ifndef _FINATRAN_H_
#define _FINATRAN_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "transtype.h"
#include "errcode.h"


/* 宏定义 */
#define CONF_GET_SUCC           0           /* 查询参数定义成功 */
#define CONF_GET_FAIL           -1          /* 查询参数定义失败 */
#define CONF_NOT_FOUND          1           /* 未找到参数定义 */

#define SHOP_FEE_CONF           0           /* 商户手续费定义 */
#define DEPT_FEE_CONF           1           /* 机构手续费定义 */

#define FEE_CALC_NOT            0           /* 不计算手续费 */
#define FEE_CALC_RATE           1           /* 按比率计算手续费 */
#define FEE_CALC_INTERVAL       2           /* 按区间计算手续费 */

#define CONFIG_FILENAME         "Setup.ini"
#define SECTION_COMMUNICATION   "SECTION_COMMUNICATION"
#define SECTION_PUBLIC          "SECTION_PUBLIC"

#define NOT_SEND                0           /* 不发送 */
#define SEND                    1           /* 发送 */

/* 交易参数结构定义 */
typedef struct
{
    double  dAmountSingle;                  /* 单笔限额 */
    double  dAmountSum;                     /* 当日累计限额 */
    int     iMaxCount;                      /* 当日最大交易笔数 */
    double  dCreditAmountSingle;            /* 信用卡单笔限额 */
    double  dCreditAmountSum;               /* 信用卡当日累计限额 */
    int     iCreditMaxCount;                /* 信用卡当日最大交易笔数 */
    char    szCardTypeOut[9+1];             /* 转出卡许可卡类型 */
    char    szCardTypeIn[9+1];              /* 转入卡许可卡类型 */
    int     iFeeCalcType;                   /* 手续费计算方式 */
} T_EpayConf;

/* 交易处理函数指针结构定义 */
typedef struct
{
    int     iTransType;                     /* 交易类型 */
    char    szTransName[64+1];              /* 交易名称 */
    int     (*pFuncPre)(T_App*);            /* 请求处理函数指针 */
    int     (*pFuncPost)(T_App*);           /* 应答处理函数指针 */
    int     iSendToHost;                    /* 发送后台标志 */
} T_TransProc;

#ifndef _EXTERN_

    /* 交易处理函数定义 */
    /* 签到交易 */
    extern int LoginPreTreat(T_App*);
    extern int LoginPostTreat(T_App*);

    /* 消费交易 */
    extern int PurchasePreTreat(T_App*);
    extern int PurchasePostTreat(T_App*);
    
    /* EMV消费交易完整流程 */
    extern int EmvPurTransPreTreat(T_App*);
    extern int EmvPurTransPostTreat(T_App*);

    /* EMV消费联机数据处理 */
    extern int EmvPurOnlinePreTreat(T_App*);
    extern int EmvPurOnlinePostTreat(T_App*);

    /* EMV余额查询 */
    extern int EmvInqueryPreTreat(T_App*);
    extern int EmvInqueryPostTreat(T_App*);
    
    /* EMV消费撤销 */
    extern int EmvPurCancelPreTreat(T_App*);
    extern int EmvPurCancelPostTreat(T_App*);
    
    /* 预授权交易 */
    extern int PreAuthPreTreat(T_App*);
    extern int PreAuthPostTreat(T_App*);

    /* 查询交易 */
    extern int InqueryPreTreat(T_App*);
    extern int InqueryPostTreat(T_App*);

    /* 撤销交易 */
    extern int PurCancelPreTreat(T_App*);
    extern int PurCancelPostTreat(T_App*);

    /* 冲正交易 */
    extern int AutoVoidPreTreat(T_App*);
    extern int AutoVoidPostTreat(T_App*);

    /* 退货交易 */
    extern int RefundPreTreat(T_App*);
    extern int RefundPostTreat(T_App*);

    /* 预授权完成交易 */
    extern int ConfirmPreTreat(T_App*);
    extern int ConfirmPostTreat(T_App*);

    /* 转出转账预查询交易 */
    extern int TranOutQueryPreTreat(T_App*);
    extern int TranOutQueryPostTreat(T_App*);

    /* 转出转账交易 */
    extern int TranOutPreTreat(T_App*);
    extern int TranOutPostTreat(T_App*);


    /* 跨行转账预查询交易 */
    extern int TranOtherQueryPreTreat(T_App*);
    extern int TranOtherQueryPostTreat(T_App*);

    /* 跨行转账交易 */
    extern int TranOtherPreTreat(T_App*);
    extern int TranOtherPostTreat(T_App*);

    /* 消费通知交易 */
    extern int PurNoticePreTreat(T_App*);
    extern int PurNoticePostTreat(T_App*);

    /* 电信缴费预查询交易 */
    extern int CTCCQueryPreTreat(T_App*);
    extern int CTCCQueryPostTreat(T_App*);

    /* 电信缴费交易 */
    extern int CTCCPrepayPreTreat(T_App*);
    extern int CTCCPrepayPostTreat(T_App*);

    /* 交易处理函数数组 */
    T_TransProc gtaTransProc[]=
    {
        {LOGIN,                 "签到交易",             LoginPreTreat,          LoginPostTreat,             SEND},
    	{PURCHASE,              "消费交易",             PurchasePreTreat,       PurchasePostTreat,          SEND},
    	{EMV_PUR_TRANS,         "EMV消费完整流程",      EmvPurTransPreTreat,    EmvPurTransPostTreat,       SEND},
    	{EMV_PUR_ONLINE,        "EMV消费联机处理",      EmvPurOnlinePreTreat,   EmvPurOnlinePostTreat,      NOT_SEND},
    	{EMV_INQUERY,           "EMV余额查询",          EmvInqueryPreTreat,     EmvInqueryPostTreat,        SEND},
    	{EMV_PUR_CANCEL,        "EMV消费撤销",          EmvPurCancelPreTreat,   EmvPurCancelPostTreat,      SEND},
    	{PRE_AUTH,              "预授权交易",           PreAuthPreTreat,        PreAuthPostTreat,           SEND},
    	{INQUERY,               "查询交易",             NULL,                   InqueryPostTreat,           SEND},
    	{PUR_CANCEL,            "撤销交易",             PurCancelPreTreat,      PurCancelPostTreat,         SEND},
    	{AUTO_VOID,             "冲正交易",             AutoVoidPreTreat,       AutoVoidPostTreat,          SEND},
    	{REFUND,                "退货交易",             RefundPreTreat,         RefundPostTreat,            SEND},
    	{CONFIRM,               "预授权完成",           ConfirmPreTreat,        ConfirmPostTreat,           SEND},
    	{TRAN_OUT_QUERY,        "转出转账预查询交易",   TranOutQueryPreTreat,   TranOutQueryPostTreat,      SEND},
    	{TRAN_OUT,              "转出转账交易",         TranOutPreTreat,        TranOutPostTreat,           SEND},
        {TRAN_OTHER_QUERY,      "跨行转账预查询交易",   TranOtherQueryPreTreat, TranOtherQueryPostTreat,    SEND},
    	{TRAN_OTHER,            "跨行转账交易",         TranOtherPreTreat,      TranOtherPostTreat,         SEND},
    	{PUR_NOTICE,            "消费通知交易",         PurNoticePreTreat,      PurNoticePostTreat,         SEND},
    	{CHINATELECOM_QUERY,    "电信缴费预查询交易",   CTCCQueryPreTreat,      CTCCQueryPostTreat,         SEND},
    	{CHINATELECOM_PREPAY,   "电信缴费交易",         CTCCPrepayPreTreat,     CTCCPrepayPostTreat,        SEND},
        {0,                     "无效交易",             NULL,                   NULL,                       NOT_SEND}
    };

    /* finatran模块全局变量 */
    int     giFeeCalcType;                      /* 手续费计算方式 */
    int     giTeleChkType;                      /* 是否检查号码绑定 */
    char    gszTelephone[15+1];                 /* 绑定电话号码 */
    int     giTeleChkLen;                       /* 电话号码检查位数 */
    long    glTimeOut;                          /* 交易超时时间 */
    char    gszMoniIP[15+1];                    /* web监控IP地址 */
    char    gszMoniPort[5+1];                   /* web监控端口号 */

#else
    extern int          giFeeCalcType;
    extern int          giTeleChkType;
    extern char         gszTelephone[15+1];
    extern int          giTeleChkLen;
    extern long         glTimeOut;
    extern char         gszMoniIP[15+1];
    extern char         gszMoniPort[5+1];

    extern T_TransProc  gtaTransProc[];
#endif

#endif
