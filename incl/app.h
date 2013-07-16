/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：定义本系统模块间通讯的公共数据结构
** 创 建 人：高明鑫
** 创建日期：2012/11/8
** $Log: app.h,v $
** Revision 1.20  2013/06/14 02:33:54  fengw
**
** 1、增加工作密钥结构体定义。
**
** Revision 1.19  2012/12/20 09:04:37  fengw
**
** 1、增加cExcepHandle(交易异常处理机制)字段。
**
** Revision 1.18  2012/12/17 06:34:10  chenrb
** app结构增加控制参数长度iControlLen、控制参数szControlPara两个字段
**
** Revision 1.17  2012/12/13 07:33:06  chenrb
** szTermSerialNo长度由11变为10
**
** Revision 1.16  2012/12/11 03:26:02  chenrb
** iMenuItem修改成iaMenuItem
**
** Revision 1.15  2012/12/11 02:48:24  chenrb
** 增加终端型号szPosType
**
** Revision 1.14  2012/11/29 05:58:37  chenrb
** 删除lLastVisitTime
**
** Revision 1.13  2012/11/29 02:47:59  chenrb
** 修改路由字段命名
**
** Revision 1.12  2012/11/27 07:03:17  gaomx
** *** empty log message ***
**
** Revision 1.11  2012/11/27 06:25:36  epay5
** *** empty log message ***
**
** Revision 1.9  2012/11/20 06:05:04  epay5
** modified by gaomx 修复冲突
**
** Revision 1.8  2012/11/20 01:40:14  fengw
**
** 1、APPSIZE漏改，重新修改提交。
**
** Revision 1.7  2012/11/20 01:36:44  fengw
**
** 1、修改APPSIZE、HSMSIZE写法。
** 2、格式对齐。
**
** Revision 1.6  2012/11/19 08:11:01  epay5
** 增加消息队列传递最大长度值的宏MAX_MSG_SIZE
**
** Revision 1.5  2012/11/19 07:48:58  epay5
**
** modified by gaomx 2012/11/19 增加消息队列结构体定义
**
** Revision 1.4  2012/11/16 01:25:56  fengw
**
** 1、增加后台返回信息(szHostRetMsg)字段，用于保存后台返回应答中的错误提示信息
**
** Revision 1.3  2012/11/14 06:54:04  chenrb
** 1、将终端程序版本szPosCodeVer由15字节变为9字节。
** 2、删除ISO8583报文60、62自定义域
** 3、将IC卡序列号szEmvCardNo长由2字节变为3字节。
**
** Revision 1.2  2012/11/12 04:24:57  fengw
**
** 增加机构号(szDeptNo)、机构层级信息(szDeptDetail)、业务类型(iBusinessType)字段
**
** Revision 1.1.1.1  2012/11/12 02:10:42  epay5
** V5 Init
**
*******************************************************************/

#ifndef _APP_H_
#define _APP_H_

typedef struct
{
    /* 报文协议部分 */
    int  iFskId;                    /* 接入平台ID */
    int  iModuleId;                 /* 模块号 */
    int  iChannelId;                /* 通道号 */
    int  iCallType;                 /* 呼叫类型 1-终端主动 2-中心主动 */
    int  iSteps;                    /* 报文同步序号 */
    long lTransDataIdx;             /* 交易数据索引号 */
    char szMsgVer[2+1];             /* 终端报文协议版本号 */
    char szAppVer[4+1];             /* 终端应用脚本版本号 */
    char szPosCodeVer[8+1];         /* 终端程序版本号 */
    char szPosType[10+1];           /* 终端型号 */

    /* 主被叫号码 */
    char szCalledTelByTerm[15+1];   /* 终端上送的被叫中心号码 */
    char szCalledTelByNac[15+1];    /* 网控器上送的被叫中心号码 */
    char szCallingTel[15+1];        /* 终端主叫号码 */

    /* 交易路由信息 */
    char szSourceTpdu[2+1];         /* 源地址 */
    char szTargetTpdu[2+1];         /* 目的地址 */
    long lProcToAccessMsgType;      /* 接入层接收交易应答消息类型，为接入模块进程号，交易处理层返回交易应答时作为消息类型 */
    long lPresentToProcMsgType;     /* 交易处理层接收交易应答消息类型，为交易处理模块进程号，业务提交层返回交易应答时作为消息类型 */
    long lAccessToProcMsgType;      /* 交易处理层接收交易请求消息类型，接入层发送交易请求时作为消息类型，在交易定义时配置 */
    long lProcToPresentMsgType;     /* 业务提交层接收交易请求消息类型，业务处理层发送交易请求时作为消息类型，在交易定义时配置 */
    char szIp[15+1];                /* 请求方IP */

    /* 终端采集信息 */
    int  iTransNum;                 /* 数量，0x06号指令输出 */
    char szFinancialCode[40+1];	    /* 金融应用号 */
    char szBusinessCode[40+1];	    /* 商务应用号 */
    char szInDate[8+1];             /* 日期，YYYYMMDD，0x0a号指令输出 */
    char szInMonth[6+1];            /* 年月，YYYYMM，0x0b号指令输出 */
    char szUserData[80+1];          /* 用户输入数据，0x0c号指令输出 */
    int  iaMenuItem[5];             /* 选中的动态菜单项，0x2B号指令输出 */
    int  iMenuNum;                  /* 动态菜单个数, 最多支持5个动态菜单 */
    int  iMenuRecNo[5];	            /* 动态菜单对应的菜单标识 */
    int  iStaticMenuId;             /* 静态菜单ID */
    int  iStaticMenuItem;           /* 选中的静态菜单项，0x2D号静态菜单显示与选择指令输出(菜单项ID) */
    char szStaticMenuOut[30+1];	    /* 选中的静态菜单内容，0x2D号静态菜单显示与选择指令输出(菜单项内容) */
    long lRate;                     /* 0x36号指令，读取利率输出 */
    char szTermRetCode[2+1];        /* 所有更新类指令的返回结果 */

    /* 收单方(商户终端)属性 */
    char szPsamNo[16+1];            /* 安全模块号 */
    char szTermSerialNo[10+1];      /* 终端硬件序列号 */
    char szDeptNo[15+1];            /* 机构号 */
    char szDeptDetail[70+1];        /* 机构层级信息 */
    char szShopNo[15+1];            /* 商户号 */ 
    char szPosNo[15+1];             /* 终端号 */
    char szAcqBankId[11+1];         /* 收单行 */
    long lBatchNo;                  /* 交易批次号 */						
    long lPosTrace;                 /* 终端流水号 */
    long lOldPosTrace;              /* 原POS流水号 */
    char szPosDate[8+1];            /* 终端交易日期，格式YYYYMMDD */
    char szPosTime[6+1];            /* 终端交易时间，格式HHMMSS */
    char szShopName[40+1];          /* 商户名称 */
    char szShopType[4+1];           /* 商户类型 */
    long lMarketNo;                 /* 市场类别 */
    char szMacKey[16+1];            /* 终端MAC密钥 */
    char szPinKey[16+1];            /* 终端PIN密钥 */
    char szTrackKey[16+1];          /* 终端磁道数据密钥 */
    char szKeyIndex;                /* 密钥索引号 */
    int  iTermModule;	            /* 终端参数模版号 */
    int  iPsamModule;               /* Psam参数模版号 */
    char szOperNo[4+1];             /* 操作员编号 */
    char szOperPwd[6+1];            /* 操作员密码 */
    char szEntryMode[3+1];          /* 服务点输入方式码, COMWEB远程下载交易用于保存下载时机 */

    /* 交易要素 */
    int  iTransType;                /* 交易类型 */
    int  iOldTransType;             /* 原交易类型 */
    int  iBusinessType;             /* 业务类型 */
    char szTransCode[8+1];          /* 终端交易代码 */
    char szNextTransCode[8+1];      /* 后续交易代码 */
    char szTransName[20+1];         /* 交易名称 */
    char szAmount[12+1];            /* 交易金额或应缴金额 */
    char szAddiAmount[14+1];        /* 余额或手续费 */
    char szFundType[3+1];           /* 货币代码 */
    int  iCommandNum;	            /* 操作码个数=当前交易后续操作码个数+后续交易之前操作码个数*/
    int  iCommandLen;	            /* 流程代码长度 */
    char szCommand[99+1];    	    /* 流程代码 */
    int  iControlLen;               /* 控制参数长度 */
    char szControlPara[100+1];      /* 控制参数 */
    char szMac[8+1];                /* 交易MAC */

    /* 转出卡账户信息，在接入层获取赋值 */
    char szPan[19+1];               /* 主账户 */
    char szPasswd[8+1];             /* 卡密码/操作员密码 */
    char szNewPasswd[8+1];          /* 卡新密码/操作员新密码 */
    char szTrack2[37+1];            /* 2磁道明文 */ 
    char szTrack3[104+1];           /* 3磁道明文 */
    char szExpireDate[4+1];         /* 卡有效期 */
    char szOutBankId[11+1];         /* 发卡行ID */
    char szOutBankName[20+1];	    /* 发卡行名称 */
    char szOutCardName[32+1];	    /* 银行卡名称 */
    char cOutCardType;              /* 转出卡类型 '0'-借记卡 '1'-贷记卡 '3'-准贷记卡 */
    int  iOutCardLevel;             /* 卡级别  0-普卡  1-金卡 */
    int	 iOutCardBelong;            /* 卡归属  0-本行本地  1-本行异地  2-他行 */
    char szHolderName[40+1];        /* 持卡人姓名 */

    /* 转入卡账户信息，在接入层获取赋值 */
    char szAccount2[19+1];          /* 转入账号 */ 
    char szInBankId[11+1];          /* 转入卡发卡行ID */
    char szInBankName[20+1];        /* 转入卡发卡行名称 */
    char cInCardType;               /* 转入卡类型 '0'-借记卡 '1'-贷记卡 '3'-准贷记卡 */
    int	 iInCardBelong;	     	    /* 转入卡归属  0-本行本地  1-本行异地  2-他行 */

    /* 平台、后台信息 */
    char szHostDate[8+1];           /* 平台交易日期，格式YYYYMMDD */
    char szHostTime[6+1];           /* 平台交易时间，格式HHMMSS */
    long lSysTrace;                 /* 平台流水号 */
    long lOldSysTrace;              /* 原平台流水号 */
    char szRetriRefNum[12+1];       /* 后台检索参考号 */
    char szOldRetriRefNum[12+1];    /* 原后台检索参考号 */
    char szAuthCode[6+1];           /* 授权码 */		    
    char szRetCode[2+1];            /* 平台返回码 */
    char szHostRetCode[6+1];        /* 后台返回码*/
    char szHostRetMsg[40+1];        /* 后台返回信息 */
    char szRetDesc[20+1];           /* 返回信息描述 */

    /* 自定义数据 */
    int  iReservedLen;              /* 自定义数据长度 */        
    char szReserved[1024+1];        /* 自定义数据 */

    /* TMS应用数据 */
    int  iTmsLen;     	            /* TMS数据长度 */        
    char szTmsData[310+1];          /* TMS参数数据 */

    /* IC卡应用数据 */
    char szEmvRet[2+1];	            /* EMV联机数据处理结果 */
    char szEmvCardNo[3+1];          /* EMV卡序列号 */
    char szEmvParaVer[12+1];	    /* EMV参数版本号 */
    int  iEmvParaLen;               /* EMV参数长度 */
    char szEmvPara[512+1];          /* EMV参数 */
    char szEmvKeyVer[8+1];          /* EMV公钥版本号 */
    int  iEmvKeyLen;                /* MEV公钥长度 */
    char szEmvKey[512+1];           /* EMV公钥 */
    int  iEmvTcLen;                 /* EMV交易证书长度 */
    char szEmvTc[512+1];            /* EMV交易证书 */
    int  iEmvDataLen;               /* EMV数据长度 */
    char szEmvData[512+1];          /* EMV数据 */
    int  iEmvScriptLen;             /* EMV脚本长度 */
    char szEmvScript[512+1];	    /* EMV脚本 */

    /* 8583包相关 */
    char szMsgId[4+1];              /* field 0 */
    char szProcCode[6+1];           /* field 3     */
    char szSettleDate[4+1];         /* field 15 清算日期 */
    char szServerCode[2+1];         /* field 25    */
    char szCaptureCode[2+1];	    /* field 26    */

    /* 其他 */
    int  iDataNum;                  /* 数据源个数 */
    char szDataSource[20+1];	    /* 数据源标识，每个字节标识一个数据源 */
    char cExcepHandle;              /* 交易异常处理机制 0-不冲正不重发 1-冲正 2-重发 */
    char cDispType;                 /* 结果显示方式 '0'-不刷新 '1'-刷新显示首页 */
    char szControlCode[5];          /* 流程控制码，最多支持5个流程控制, 1个字节代表1个流程控制 1-是 0-否 */

    /* 各项目自定义数据 */
} T_App;
#define APPSIZE sizeof(T_App)

/*系统内部路由消息队列结构*/
#define MAX_MSG_SIZE        10

typedef struct
{
    long    lMsgType;
    char    szMsgText[MAX_MSG_SIZE];
} T_MessageStru;

/* 发往加密机的消息体结构 */
typedef struct
{
    long    lSourceType;            /*进程号*/
    int     iTransType;             /*加密类型*/
    char    szPsamNo[16+1];         /*安全模块号*/
    int     iAlog;                  /*加密算法*/
    int     iPinBlock;              /*pin block组织*/
    char    szPinKey[32+1];         /*pin_key密文*/
    char    szMacKey[32+1];         /*mac_key密文，密码转加密时存放目的PIK密文*/
    int     iDataLen;               /*数据长度*/
    char    szData[1024];           /*数据*/
    char    szReturnCode[2+1];      /*返回码*/
} T_Interface;
#define HSMSIZE sizeof(T_Interface)

typedef struct
{
    long    lMsgType;               /* 消息标识 */
    char    szMsgText[HSMSIZE];     /* 消息 */
} T_HsmStru;

/* 工作密钥结构体定义 */
typedef struct
{
    char    szPinKey[32+1];
    char    szPIKChkVal[8+1];
    char    szMacKey[32+1];
    char    szMAKChkVal[8+1];
    char    szMagKey[32+1];
    char    szMGKChkVal[8+1];
}T_WorkKey;

#endif
