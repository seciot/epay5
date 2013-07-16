/*******************************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：交易类型宏定义
** 创 建 人：Robin
** 创建日期：2012/12/17
**
** $Revision: 1.8 $
** $Log: transtype.h,v $
** Revision 1.8  2013/03/29 02:51:20  fengw
**
** 1、新增EMV余额查询、EMV消费撤销交易类型宏定义。
**
** Revision 1.7  2013/03/11 07:07:41  fengw
**
** 1、新增EMV相关交易类型宏定义。
**
** Revision 1.6  2012/12/19 08:41:59  chenrb
** 增加缴费类交易定义
**
** Revision 1.5  2012/12/17 05:30:28  chenrb
** 增加重发交易宏定义
**
** Revision 1.4  2012/12/17 05:04:49  chenrb
** *** empty log message ***
**
*******************************************************************************/
#ifndef _TRANSTYPE_H_
#define _TRANSTYPE_H_

//管理类交易，从1至99
#define ECHO_TEST           1
#define DOWN_PAYLIST        2
#define DOWN_ALL_OPERATION  3
#define AUTO_VOID           4
#define LOGIN               5
#define SEND_INFO           6
#define REGISTER            7
#define TERM_REGISTER       8
#define SEND_TRACE          10
#define SEND_ERR_LOG        11

#define DOWN_TMS            12
#define DOWN_TMS_END        13

#define QUERY_CUSTOMER      15
#define ADD_CUSTOMER        16
#define DEL_CUSTOMER        17
#define REPRINT             18
#define PPP_LOGIN           19
#define OPER_LOGIN          20
#define SETTLE              21    
#define BATCHUP             22
#define SETTLE2             23
#define LOGOUT              24
#define OPER_PWD            25
#define ADD_OPER            26
#define DEL_OPER            27
#define PWD_INIT            28
#define HOST_SETTLE         29
#define DOWN_PARAMETER      30

//新应用下载(终端发起&中心发起共用)
#define DOWN_ALL_FUNCTION    31
#define DOWN_ALL_MENU        32
#define DOWN_ALL_PRINT       33
#define DOWN_ALL_TERM        34
#define DOWN_ALL_PSAM        35
#define DOWN_ALL_ERROR       36
#define UNREGISTER_CARD      38
#define REGISTER_CARD        39

//中心发起的单项下载
#define CENDOWN_FIRST_PAGE      40
#define CENDOWN_FUNCTION_INFO   41
#define CENDOWN_OPERATION_INFO  42
#define CENDOWN_PRINT_INFO      43
#define CENDOWN_TERM_PARA       44
#define CENDOWN_PSAM_PARA       45
#define CENDOWN_ERROR           46
#define CENDOWN_MSG             47
#define CENDOWN_PAYLIST         48
#define CENDOWN_MENU            49

//终端发起的下载
#define DOWN_FIRST_PAGE         50
#define DOWN_FUNCTION_INFO      51
#define DOWN_OPERATION_INFO     52
#define DOWN_PRINT_INFO         53
#define DOWN_TERM_PARA          54
#define DOWN_PSAM_PARA          55
#define DOWN_ERROR              56
#define DOWN_MSG                57
#define DOWN_STATIC_MENU        58    
#define DOWN_MENU               59

//动态菜单流程控制
#define DYNAMIC_CONTR           60
//入网测试
#define TEST_DIG_CHK_INPUT      61
#define TEST_DIG_CHK_TWO_INPUT  62
#define TEST_DISP_OPER_INFO     63
#define TEST_PAYLIST            64
#define TEST_OTHER              65

#define GET_DYMENU_66        66
#define GET_DYMENU_67        67
#define GET_DYMENU_68        68
#define GET_DYMENU_69        69

//交易检测到，终端自动发起的下载
#define AUTODOWN_ALL_OPERATION      70
#define AUTODOWN_FUNCTION_INFO      71
#define AUTODOWN_OPERATION_INFO     72
#define AUTODOWN_PRINT_INFO         73
#define AUTODOWN_TERM_PARA          74
#define AUTODOWN_PSAM_PARA          75
#define AUTODOWN_ERROR              76
#define AUTODOWN_MENU               77
#define AUTODOWN_PAYLIST            78
#define AUTODOWN_MSG                80

//中心发起的新应用下载-下载操作提示
#define CENDOWN_ALL_OPERATION       79

//入网测试
#define TEST_NORMAL_INPUT       81
#define TEST_TWO_INPUT          82
#define TEST_NORMAL_DISP_INPUT  83
#define TEST_TWO_DISP_INPUT     84
#define TEST_PRINT              85
#define TEST_CLEAR_MENU         86
#define TEST_CLEAR_AUTOVOID     87
#define TEST_SEND_CARDNO        88
#define TEST_INQ                89
#define TEST_PAY                300    

/* EMV */
#define DOWN_EMV_PARA           90              /* EMV参数更新 */
#define DOWN_EMV_KEY            91              /* EMV公钥更新 */


//免提拨号
/*
#define DIAL_TELE0          90
#define DIAL_TELE1          91
#define DIAL_TELE2          92
#define DIAL_TELE3          93
#define DIAL_TELE4          94
#define DIAL_TELE5          95
*/
//中止缴费
#define STOP_PAY            99


//金融类交易请从100以后开始定义，且不允许重复
#define PURCHASE                101                     /* 消费 */
#define PUR_CANCEL              102                     /* 消费撤销 */
#define REFUND                  103                     /* 退货 */
#define PUR_NOTICE              104                     /* 消费通知 */
#define PRE_AUTH                105                     /* 预授权 */
#define PRE_CANCEL              106                     /* 预授权撤销 */
#define CONFIRM                 107                     /* 预授权完成 */
#define CON_CANCEL              108                     /* 预授权完成撤销 */
#define TRANS                   109                     /* 卡卡转账 */
#define TRAN_CANCEL             110                     /* 转账撤销 */
#define TRAN_OUT                111                     /* 转出转账 */
#define TRAN_OUT_CANCEL         112                     /* 转出撤销 */
#define TRAN_IN                 113                     /* 转入转账 */
#define TRAN_IN_CANCEL          114                     /* 转入撤销 */
#define INQUERY                 115                     /* 银行卡余额查询 */
#define TRANS_QUERY             119                     /* 转账预查询 */
#define TRAN_IN_QUERY           120                     /* 转入转账预查询 */
#define TRAN_OUT_QUERY          121                     /* 转出转账预查询 */
#define PAY_CREDIT_QUERY        122                     /* 信用卡还款预查询 */
#define PAY_CREDIT              123                     /* 信用卡还款 */
#define TRAN_OTHER              124                     /* 跨行汇款 */
#define TRAN_OTHER_QUERY        125                     /* 跨行汇款预查询 */

#define EMV_PUR_TRANS           151                     /* EMV消费完整流程 */
#define EMV_PUR_ONLINE          152                     /* EMV消费联机数据处理 */
#define EMV_INQUERY             153                     /* EMV余额查询 */
#define EMV_PUR_CANCEL          154                     /* EMV消费撤销 */

//预付费缴费交易
#define CHINATELECOM_PREPAY	    170                     /* 电信充值 */
#define CHINATELECOM_QUERY	    171                     /* 电信查余额 */
#define CHINAUNICOM_PREPAY	    172                     /* 联通充值 */
#define CHINAUNICOM_QUERY	    173                     /* 联通查余额 */
#define CHINAMOBILE_PREPAY	    174                     /* 移动充值 */
#define CHINAMOBILE_QUERY	    175                     /* 移动查余额 */
#define CHINATELECOM_CDMA_PREPAY    176                 /* 电信充值-CDMA */
#define CHINATELECOM_CDMA_QUERY     177                 /* 电信查余额-CDMA */
#define NETCOM_PREPAY           178                     /* 网通充值 */
#define NETCOM_QUERY            179                     /* 网通查余额 */

//后付费缴费交易
#define	CHINAUNICOM_INQ		    201                     /* 联通查应缴费 */
#define	CHINAUNICOM_PAY		    202                     /* 联通缴费 */
#define	CHINAMOBILE_INQ		    203                     /* 移动查应缴费 */
#define	CHINAMOBILE_PAY		    204                     /* 移动缴费 */
#define	CHINAMOBILE_PIN		    205                     /* 移动验密 */
#define	CHINATELECOM_INQ	    206                     /* 电信查应缴费 */
#define	CHINATELECOM_PAY	    207                     /* 电信缴费 */
#define	ELECTRICITY_INQ		    208                     /* 查应缴电费 */
#define	ELECTRICITY_PAY		    209                     /* 缴电费 */
#define	GAS_INQ			        210                     /* 查应缴煤气费 */
#define	GAS_PAY			        211                     /* 煤气缴费 */
#define TRAFFIC_AMERCE_INQ	    212                     /* 现场罚没查询 */
#define TRAFFIC_AMERCE_PAY	    213                     /* 现场罚没 */
#define TRAFFIC_AMERCE_NO_INQ	214                     /* 非现场罚没查询 */
#define TRAFFIC_AMERCE_NO_PAY	215                     /* 非现场罚没 */
#define WATER_INQ               216                     /* 水费应缴费查询 */
#define WATER_PAY               217                     /* 水费缴费 */
#define NETCOM_INQ              218                     /* 网通应缴费查询 */
#define NETCOM_PAY              219                     /* 网通缴费 */
#define CHINATELECOM_CDMA_INQ   220                     /* 电信CDMA应缴费查询 */
#define CHINATELECOM_CDMA_PAY   221                     /* 电信CDMA缴费 */

//订单类支付
#define PLANE_TICKET_PAYLIST    250                     /* 机票支付 */

//流水明细查询
#define QUERY_DETAIL_SELF       310                     /* 查询本终端流水 */
#define QUERY_DETAIL_OTHER      311                     /* 查询其他终端流水 */
#define QUERY_TOTAL             312                     /* 汇总查询 */
#define QUERY_LAST_DETAIL       313                     /* 查询末笔交易 */
#define QUERY_TODAY_DETAIL      314                     /* 查询当日流水 */

#define RESEND                  500                     /* 重发 */

//交易检测到，终端自动发起的下载交易交易代码
#define LOGIN_CODE                      "00000005"
#define DOWN_TMS_CODE                   "00000012"
#define DOWN_TMS_END_CODE               "00000013"
#define AUTODOWN_ALL_OPERATION_CODE     "00000070"
#define AUTODOWN_FUNCTION_INFO_CODE     "00000071"
#define AUTODOWN_OPERATION_INFO_CODE    "00000072"
#define AUTODOWN_PRINT_INFO_CODE        "00000073"
#define AUTODOWN_TERM_PARA_CODE         "00000074"
#define AUTODOWN_PSAM_PARA_CODE         "00000075"
#define AUTODOWN_ERROR_CODE             "00000076"
#define AUTODOWN_MENU_CODE              "00000077"
#define AUTODOWN_PAYLIST_CODE           "00000078"
#define AUTODOWN_MSG_CODE               "00000080"

#define STOP_PAY_CODE                   "00000099"

#endif
