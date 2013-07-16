/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：定义本系统自定义的宏
** 创 建 人：高明鑫
** 创建日期：2012/11/8
** $Revision: 1.13 $
** $Log: user.h,v $
** Revision 1.13  2013/06/14 02:34:48  fengw
**
** 1、增加终端主密钥类型TERM_KEY宏定义。
** 2、增加8583报文62域FIELD62宏定义。
**
** Revision 1.12  2013/03/11 07:07:21  fengw
**
** 1、新增自定义指令相关宏定义。
**
** Revision 1.11  2012/12/21 05:43:23  chenrb
** *** empty log message ***
**
** Revision 1.10  2012/12/18 10:16:39  wukj
** *** empty log message ***
**
** Revision 1.9  2012/12/18 07:16:26  wukj
** *** empty log message ***
**
** Revision 1.8  2012/12/11 06:38:05  chenrb
** 增加主叫方宏定义
**
** Revision 1.7  2012/11/29 07:10:06  zhangwm
**
** 增加是否打印日志判断
**
** Revision 1.6  2012/11/27 06:02:55  chenrb
** 增加AscBcd互转、sock类型宏定义
**
** Revision 1.5  2012/11/27 05:11:45  epay5
** *** empty log message ***
**
** Revision 1.4  2012/11/27 03:40:15  chenrb
** 删除一些未用的宏定义
**
** Revision 1.3  2012/11/27 03:35:19  zhangwm
**
** 将日志相关的公用宏定义放置到user.h中
**
** Revision 1.2  2012/11/27 02:34:00  epay5
** modified by gaomx 修改头文件
**
** Revision 1.1  2012/11/22 01:42:35  epay5
** add by gaomx
**
*******************************************************************/
/* 最大数据索引号 */
#define MAX_TRANS_DATA_INDEX 512

/* 跟踪日志的分级管理 */
#define DEBUG_TLOG    0x01
#define DEBUG_HLOG    0x02
#define DEBUG_MLOG    0x04
#define DEBUG_ALOG    0x08

/*数据库定义*/
#define DB_ORACLE    1

/*是否联动向主机签到，定义则联动签到，否则不向主机签到*/
#define LOGIN_TO_HOST    1

/*是否增加0A换行符，如果终端显示结果信息从首行开始显示，为了使得终端显示达到居中显示效果则定义；否则不定义*/
#define ADD_0A_LINE    1

/*主密钥长度定义为双倍长，否则单倍长。只适用于软加密*/
#define MKEY_TRIDES    1

/* 定义绑定卡交易的类型 0-付款 1-收款 2-缴费*/
#define TRAN_TYPE_OUT       '0'
#define TRAN_TYPE_IN        '1'
#define TRAN_TYPE_JIAOFEI   '2'

/*卡级别 0-普卡 1-金卡*/
#define NORMAL_CARD     0
#define GOLD_CARD       1

/*卡种 0-借记卡 1-贷记卡 2-任意卡 3-准贷记卡*/
#define DEBIT_CARD      '0'
#define CREDIT_CARD     '1'
#define ANY_CARD        '2'
#define PRECREDIT_CARD  '3'

/* 卡归属 1-他行卡 2－本行本地卡 3-本行异地卡 */
#define OTHER_BANK                  1
#define LOCAL_BANK_LOCAL_CITY       2
#define LOCAL_BANK_OTHER_CITY       3

/*商户费率类型 0-正常费率 1-VIP费率*/
#define NORMAL_RATE     0
#define VIP_RATE        1

/* 密钥类型定义 */
#define    TERM_KEY       0
#define    PIN_KEY        1
#define    MAC_KEY        2
#define    MAG_KEY        3

/* 是否的宏定义 */
#define YES   1
#define NO    0    

/*加密机运算类型宏定义*/
#define DECRYPT_PIN     0
#define CALC_MAC        1
#define GET_WORK_KEY    3
#define CHANGE_PIN      4
#define DECRYPT_TRACK   5
#define GET_MASTER_KEY  6
#define VERIFY_PIN      7
#define CALC_CHKVAL     8
#define CHANGE_KEY      9
#define CHANGE_PIN_PIK2TMK    10

/* MAC算法标识 */
#define X99_CALC_MAC    1
#define XOR_CALC_MAC    2
#define X919_CALC_MAC   3

/*链路类型*/
#define LINK_CLIENT     1
#define LINK_SERVER     2

/*主机标识宏定义*/
#define UNIONPAY_PPP    0    /*银联公共支付平台*/
#define BOC             1    /*中国银行*/
#define ABC             2    /*中国农业银行*/
#define ICBC            3    /*中国工商银行*/
#define CCB             4    /*中国建设银行*/
#define BCC             5    /*交通银行*/
#define CMB             6    /*招商银行*/
#define CB              7    /*商业银行*/
#define CIB             8    /*中信银行*/
#define MSB             9    /*中国民生银行*/
#define HXB             10   /*华夏银行*/
#define CEB             11   /*光大银行*/
#define SPDB            12   /*上海浦东发展银行*/
#define POST            13   /*邮政储蓄*/
#define SDB             14   /*深发展*/
#define NXS             15   /*农信社*/
#define FIB             16   /*兴业*/
#define JXBOC_CONET     17   /*江西中行CONET*/
#define JXBOC_CSP       18   /*江西中行CSP*/
#define ICBC_ZJYW       19   /*工行中间业务平台*/
#define GFB             20   /*国付宝*/
#define CIC             97   /*中信银行*/
#define CCUN            98   /*CCUN*/
#define YLPOSP          99   /*银联POSP*/
#define YLGGZF          100  /*银联公共支付*/


/*通讯模式*/
#define FSK_COMMU       1
#define MODEM_COMMU     2

/*函数返回*/
#define SUCC            0
#define FAIL            -1
#define DUPLICATE       -2
#define TIMEOUT         -3
#define INVALID_PACK    -4


/*多媒体交换机参数宏定义，如果采用其他方式通讯设备，MAX_FSK_NUM只能为1 */
#define MAX_FSK_NUM             1
#define MAX_DSP_MODULE_NUM      3    
#define MAX_CHANNEL_NUM         256
#define MAX_COMPOS_NUM          60
#define MAX_QUERY_LEN           420

/* 卡bin的最大数量 */
#define MAX_CARD_NUM    4000

/*ISO8583域宏定义*/
#define MSG_ID          0        
#define BIT_MAP         1
#define PAN             2
#define PROC_CODE       3
#define AMOUNT          4
#define DATE_TIME       7
#define POS_TRACE       11
#define LOCAL_TIME      12
#define LOCAL_DATE      13
#define EXPIRY          14
#define SETTLE_DATE     15
#define MERCH_TYPE      18
#define MODE            22
#define NII             24
#define SERVER_CODE     25
#define PIN_MODE        26
#define FIELD28         28
#define ACQUIRER_ID     32
#define SENDER_ID       33
#define TRACK_2         35
#define TRACK_3         36
#define RETR_NUM        37
#define AUTH_ID         38
#define RET_CODE        39
#define POS_ID          41
#define CUSTOM_ID       42
#define CUSTOM_NAME     43
#define FIELD44         44
#define FIELD48         48
#define FUND_TYPE       49
#define PIN_DATA        52
#define SEC_CTRL_CODE   53
#define ADDI_AMOUNT     54
#define FIELD59         59
#define FIELD60         60
#define HOLDER_ID       61
#define FIELD62         62
#define FIELD63         63
#define NET_MANAGE_CODE 70
#define ORIG_DATA       90
#define DESTINATION_CODE   100
#define MAC             128

/* 打印记录号宏定义，应该与数据库表print_info(打印记录信息表)中定义一致 */
#define PRINT_TRANS_TYPE        1    /* 交易类型 */
#define PRINT_CARD_NO           2    /* 卡号 */
#define PRINT_CARD_IN           3    /* 转入卡号 */
#define PRINT_CARD_OUT          4    /* 转出卡号 */
#define PRINT_PAY_CARD_NO       5    /* 付款卡号 */
#define PRINT_PAN               6    /* 付款账号 */
#define PRINT_AMOUNT            7    /* 交易金额 */
#define PRINT_SYS_TRACE         8    /* 系统参考号 */
#define PRINT_TRACE             9    /* 流水号 */
#define PRINT_DATE_TIME         10   /* 交易时间 */
#define PRINT_AWARD             11   /* 中奖信息 */
#define PRINT_PAY_CODE1         12   /* 缴费号码 */
#define PRINT_PAY_CODE2         13   /* 充值号码*/
#define PRINT_TELEPHONE         14   /* 手机号码 */
#define PRINT_USER_CODE         15   /* 订单号 */
#define PRINT_SIGN              16   /* 持卡人签名 */
#define PRINT_TITLE1            17   /* 标题 */
#define PRINT_SHOP_SLIP         18   /* 落款1 */
#define PRINT_HOLDER_SLIP       19   /* 落款2 */
#define PRINT_TERM_NO           20   /* 终端号 */
#define PRINT_SHOP_NO           21   /* 商户号 */
#define PRINT_SHOP_NAME         22   /* 商户名称 */
#define PRINT_NOTE0             23   /* 备注0 */
#define PRINT_NOTE1             24   /* 备注1 */
#define PRINT_NOTE2             25   /* 备注2 */
#define PRINT_NOTE3             26   /* 备注3 */
#define PRINT_LINE              27   /* 横线 */
#define PRINT_BALANCE           28   /* 余额 */
#define PRINT_AUTH_CODE         29   /* 授权码 */
#define PRINT_FEE               30   /* 手续费 */
#define PRINT_ISSUER_BANK       31   /* 发卡行号 */
#define PRINT_ACQ_BANK          32   /* 收单行号 */
#define PRINT_EXP_DATE          33   /* 有效期 */
#define PRINT_ENTER             34   /* 换行符 */
#define PRINT_REPRINT           35   /* 重打印 */
#define PRINT_BANK_OUT          36   /* 转出行 */
#define PRINT_BANK_IN           37   /* 转入行 */
#define PRINT_YINGJIAO_AMT      38   /* 应缴金额 */
#define PRINT_SHIJIAO_AMT       39   /* 实缴金额 */
#define PRINT_HOLDER_NAME       40   /* 用户名 */
#define PRINT_BLANK             41   /* 空格 */
#define PRINT_TOTAL_AMT         42   /* 合计金额 */

/*数据索引宏定义，与print_data(打印数据索引表)定义要一致，用于指明数据来源于
  公共数据结构的哪个字段*/
#define HOST_DATETIME_IDX       1    /* 主机日期时间 */
#define POS_DATETIME_IDX        2    /* 终端日期时间 */
#define PAN_IDX                 3    /* 主账户 */
#define AMOUNT_IDX              4    /* 交易金额或应缴金额 */
#define TRANS_NAME_IDX          5    /* 交易名称 */
#define POS_TRACE_IDX           6    /* 终端流水号 */
#define RETRI_REF_NUM_IDX       7    /* 后台检索参考号 */      
#define AUTH_CODE_IDX           8    /* 授权码 */
#define SHOP_NO_IDX             9    /* 商户号 */
#define POS_NO_IDX              10   /* 终端号 */
#define SHOP_NAME_IDX           11   /* 商户名称 */
#define PSAM_NO_IDX             12   /* 安全模块号 */
#define SYS_TRACE_IDX           13   /* 平台流水号 */ 
#define ACCOUNT2_IDX            14   /* 转入账号 */
#define OUT_CARD_NAME_IDX       15   /* 银行卡名称 */
#define OLD_POS_TRACE_IDX       16   /* 原终端流水号 */
#define OLD_RETRI_REF_NUM_IDX   17   /* 原后台检索参考号 */
#define FINANCIAL_CODE_IDX      18   /* 金融应用号 */
#define BUSINESS_CODE_IDX       19   /* 商务应用号 */
#define HOST_DATE_IDX           20   /* 平台交易日期，格式YYYYMMDD */
#define POS_DATE_IDX            21   /* 终端交易日期，格式YYYYMMDD */
#define HOST_TIME_IDX           22   /* 平台交易时间，格式HHMMSS */
#define POS_TIME_IDX            23   /* 终端交易时间，格式HHMMSS */
#define EXPIRE_DATE_IDX         24   /* 卡有效期 */
#define SHOP_TYPE_IDX           25   /* 商户类型 */
#define TRANS_NUM_IDX           26   /* 数量 */
#define RATE_IDX                27   /* 利率 */
#define TRACK2_IDX              28   /* 二磁道明文 */    
#define TRACK3_IDX              29   /* 三磁道明文 */
#define MAC_IDX                 30   /* 交易MAC */
#define RET_CODE_IDX            31   /* 平台返回码 */
#define RET_DESC_IDX            32   /* 返回信息描述 */
#define BATCH_NO_IDX            33   /* 交易批次号 */
#define RESERVED_IDX            34   /* 自定义数据 */
#define ADDI_AMOUNT_IDX         35   /* 余额或手续费 */
#define OUT_BANK_ID_IDX         36   /* 转出行ID */
#define ACQ_BANK_ID_IDX         37   /* 收单行ID */
#define TOTAL_AMT_IDX           38   /* 交易金额总额  */
#define OUT_BANK_NAME_IDX       39   /* 转出行名称 */
#define IN_BANK_ID_IDX          40   /* 转入卡发卡行ID */
#define IN_BANK_NAME_IDX        41   /* 转入卡发卡行名称 */
#define MENU_NAME_IDX           42   /* 菜单名称 */
#define HOLDER_NAME_IDX         43   /* 持卡人姓名 */
#define HAND_INPUT_DATE_IDX         45   /* 手工输入日期 */

/* 工作目录的宏定义 */
#define WORKDIR        "WORKDIR"

#define uchar   unsigned char
#define uint    unsigned int
#define ulong   unsigned long

/* 状态标识 对应数据库中status字段 0-无效 1-有效*/
#define VALID    0
#define INVALID    1

/* 终端异常处理机制 '0'-不冲正不重发 '1'-冲正 '2'-重发'*/
#define POS_NO_VOID_RESEND      '0'
#define POS_MUST_VOID           '1'
#define POS_MUST_RESEND         '2'

/* 手续费计算方式 0-不计算 1-按比率 2-按区间'*/
#define NO_CALC_FEE        0
#define CALC_FEE_BY_RATE   1
#define CALC_FEE_BY_AREA   2

/* ASC、BCD互转函数对齐方式 */
#define LEFT_ALIGN      0       //左对齐，右补0
#define RIGHT_ALIGN     1       //右对齐，左补0

/* 定义数据类型 */
#define HEX_DATA        0
#define ASC_DATA        1
#define BCD_DATA        2

/* 终端主叫 */
#define POS_CALLING     1
/* 中心主叫 */
#define EPAY_CALLING    2

/* 创建sock服务类型，调用CreateSrvSocket()函数的第2个参数 */
#define SOCK_TCP        "tcp"
#define SOCK_UDP        "udp"

/* 日志打印相关宏定义 */
#define E_ERROR     0
#define T_TRACE     1

/* 平台自定义指令相关宏定义 */
#define SPECIAL_CMD_HEAD    0xC0            /* 自定义指令头字节 */
#define SPECIAL_CMD_LEN     3               /* 自定义指令长度 */

#define ERROR       __FILE__, __LINE__, E_ERROR
#define TRACE       __FILE__, __LINE__, T_TRACE
