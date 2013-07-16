#ifndef  __ERRORCODE__
#define __ERRORCODE__

#define	TRANS_SUCC              "00"    /* 交易成功 */
#define ERR_INVALID_MERCHANT    "03"    /* 无效商户 */
#define ERR_DECLINE             "05"    /* 不予承兑 */
#define ERR_INVALID_TRANS       "12"    /* 无效交易 */
#define ERR_INVALID_AMOUNT      "13"    /* 无效金额 */
#define	ERR_INVALID_CARD        "14"    /* 无效卡号 */
#define ERR_INVALID_BANK        "15"    /* 无此发卡行 */
#define ERR_UNACCEPTABLE_FEE    "23"    /* 手续费错误 */
#define ERR_TRANS_NOT_EXIST     "25"    /* 无原交易 */
#define ERR_DATA_FORMAT         "30"    /* 数据格式错 */
#define ERR_NOT_SUPPORTED       "40"    /* 功能不支持 */
#define ERR_NOT_PERMIT_HOLDER   "57"    /* 不允许持卡人 */
#define ERR_NOT_PERMIT_TERM     "58"    /* 无允许终端 */
#define ERR_EXCEED_TOTAL        "61"    /* 金额超限 */
#define ERR_ORIG_AMOUNT         "64"    /* 原金额错 */
#define ERR_EXCEED_TIMES        "65"    /* 超取款次数 */
#define ERR_TIMEOUT             "68"    /* 交易超时 */
#define ERR_CUT_OFF             "90"    /* 日终处理中 */
#define ERR_ROUTE               "92"    /* 网络故障 */
#define ERR_DUPLICATE           "94"    /* 重复交易 */
#define ERR_RECONCILE           "95"    /* 对账不平 */
#define ERR_SYSTEM_ERROR        "96"    /* 系统故障 */
#define ERR_INVALID_TERM        "97"    /* 无效终端 */
#define ERR_COMMU               "98"    /* 发卡方故障 */
#define ERR_PIN_BLOCK           "99"    /* PIN格式错 */
#define ERR_MAC                 "A0"    /* MAC校验错 */

#define ERR_HAS_PAY             "A2"    /* 费用已缴纳 */
#define ERR_AUTHCODE            "A9"    /* 报文格式错 */
#define ERR_EXCEED_SINGLE       "AA"    /* 单笔金额超限 */

#define ERR_PSAM_MODULE         "P1"    /* 无效安全模板 */
#define ERR_TERM_MODULE         "P2"    /* 无效终端模板 */
#define ERR_INVALID_MENU        "P3"    /* 菜单定制错 */
#define ERR_INVALID_PAYLIST     "P4"    /* 账单定制错 */
#define ERR_NOT_PAYLIST         "P5"    /* 无账单数据 */
#define ERR_DATA_TOO_LONG       "P6"    /* 数据过长 */
#define ERR_INVALID_APP         "P7"    /* 未定义应用 */
#define ERR_TERM_STATUS         "P8"    /* 终端未开通 */
#define ERR_NOT_PAY             "P9"    /* 未欠费 */

#define ERR_UNIT_CODE           "Q0"    /* 收费机构不全 */
#define ERR_NOT_RET_MSG         "Q1"    /* 返回不完整 */
#define ERR_DOWN_FINISH         "Q2"    /* 已下载完毕 */
#define ERR_TRANS_DEFINE        "Q3"    /* 交易定义错 */
#define ERR_TERM_NOT_REGISTER   "Q4"    /* 终端未登记 */
#define ERR_INVALID_FIRST_PAGE  "Q5"    /* 无效首页信息 */
#define ERR_OPER_NOT_LOGIN      "Q7"    /* 柜员未签到 */
#define ERR_TERM_NOT_LOGIN      "Q8"    /* 终端未签到 */
#define ERR_RESP_MAC            "Q9"    /* 响应MAC错 */

#define ERR_IN_CARD_NOT_REGISTER    "R0"    /* 未绑定转入卡 */  
#define ERR_VOID_VOID               "R1"    /* 原交易不能撤销 */
#define ERR_SYSTEM_PAUSE            "R2"    /* 系统暂停 */
#define ERR_INVALID_OPER            "R3"    /* 非法柜员 */
#define ERR_OPERPWD_ERROR           "R4"    /* 管理员密码错 */
#define ERR_CREDIT_CARD             "R5"    /* 不支持信用卡 */
#define ERR_MERCHANT_CODE           "R6"    /* 商户简码不全 */
#define ERR_CARD_TYPE               "R7"    /* 未能识别卡 */
#define ERR_INVALID_PHONE           "R8"    /* 终端非法接入 */
#define ERR_NOT_KEY                 "R9"    /* 无终端密钥 */

#define ERR_CARD_HAS_REGISTER       "S0"    /* 终端已绑定卡 */
#define ERR_NO_TRACE                "S1"    /* 无交易明细 */
#define ERR_OUT_CARD_NOT_REGISTER   "S2"    /* 未绑定转出卡 */
#define ERR_HAS_LOGIN               "S3"    /* 终端已签到 */
#define ERR_NOT_STOP                "S4"    /* 非止付卡 */
#define ERR_ONE_CARD                "S5"    /* 同卡不能转账 */
#define ERR_AWARDED                 "S6"    /* 已兑奖 */
#define ERR_AWARD_VOID              "S7"    /* 已取消兑奖 */
#define ERR_TRAN_IN_OTHER           "S8"    /* 转入卡异地 */
#define ERR_AWARD_EXP               "S9"    /* 过兑奖期 */
#define ERR_NAME                    "SA"    /* 户名错误 */
#define ERR_ID                      "SB"    /* 证件不符 */
#define ERR_YIDI_CARD               "SC"    /* 异地账户 */
#define ERR_SERVICE_SYSTEM          "SD"    /* 服务方系统故障 */
#define ERR_SERVICE_FAIL            "SE"    /* 服务方交易失败 */
#define ERR_USER_CODE               "SF"    /* 无此用户号 */
#define ERR_NO_CUSTOMER             "SG"    /* 无此收款人 */
#define	ERR_SELF_CARD               "SH"    /* 非绑定卡 */
#define	ERR_NO_NET_OPER             "SI"    /* 地区信息不全 */
#define ERR_EXIST_CUSTOMER          "SJ"    /* 分户账不存在 */
/* SK->SZ  保留不准使用 */

#define ERR_NO_FEE_RECORD           "T0"    /* 费率表不全 */
#define ERR_SHOP_STATUS             "T3"    /* 商户状态非法 */
/* TA->TZ  保留不准使用 */

#define ERR_JIAOFEI_CARD_NOT_REGISTER   "E0"    /* 未绑定缴费卡 */
#define ERR_AMOUNT_50                   "E1"    /* 非50整数倍 */
#define ERR_OTHER_BANK                  "E2"    /* 不支持他行卡 */
#define ERR_OUT_YIDI                    "E3"    /* 转出卡异地 */
#define ERR_NOT_HOST_KEY                "E4"    /* 需主机签到 */
#define ERR_CHK_VAL                     "E5"    /* 加密密钥错 */
#define ERR_DEBIT_CARD                  "E6"    /* 转入借记卡 */
#define ERR_NEED_INIT_KEY               "E7"    /* 初始化密钥 */
#define ERR_REG_CARD_NOT_APPROVE        "E8"    /* 绑定卡未核 */
#define ERR_ZNJ                         "E9"    /* 滞纳金不符 */
#define ERR_MAC_NOT_LOGIN               "EA"    /* MAC校验错 */
#define ERR_SEL_PURCHASE                "EB"    /* 他行卡付款 */
#define ERR_IN_OTHER_BANK               "EC"    /* 请用工行卡 */
#define ERR_DUPLICATE_PSAM_NO           "ED"    /* PSAM卡号重复 */
#define ERR_NO_OTHER_BANK               "EE"    /* 无他行信息 */
#define ERR_NEED_DOWN_APP               "EF"    /* 下载新应用 */
#define ERR_BANK_CODE                   "EG"    /* 错误行号 */
#define ERR_TRAN_OUT_OTHER_STOP         "EI"    /* 非交易时间 */
/* EH->EZ  保留不准使用 */

#define ERR_LAST_RECORD         "F0"    /* 末条记录 */
#define ERR_IN_SELF_BANK        "F1"    /* 收款卡非他行卡 */
#define ERR_USER_CODE2          "F2"    /* 无此用户号 */
#define ERR_DUPLICATE_TERM      "F3"    /* 终端号重复 */
#define ERR_CREDIT_LIMIT        "F4"    /* 贷记卡交易超限次 */
#define PSAMNO_INVALID          "F5"    /* PSAM卡号已登记 */
#define SHOP_TERM_INVALID       "F6"    /* 商户终端号已登记 */
#define ERR_NOT_PERMIT_CARD     "F7"    /* 不支持卡种 */

#define ERR_EPOS_OPERPWD_NOCHG  "82"    /* 初始密码未修改 */
#define ERR_EPOS_OPERPWD_ERROR  "83"    /* 密码长度非法 */
#define ERR_EPOS_OPT_INVALID    "84"    /* 无权操作员号 */
#define ERR_DEL_ADMIN_ERROR     "85"    /* 管理员不可删除 */
#define ERR_OPER_DUPLICATE      "86"    /* 操作员号重复 */

#define ERR_ZJYW_NO_OTHER_BANK  "79999"
#define ERR_ZJYW_BANK_CODE      "79997"

/* 错误宏定义 */
#define ERR_UNDEF_FEECALCTYPE           "U1"        /* 手续费计算方式未定义 */
#define ERR_REFUND_ERRDATE              "U2"        /* 不允许当日退货 */
#define ERR_REFUNE_ERRAMT               "U3"        /* 退货金额不允许大于原交易金额 */
/*
#define ERR_SHOP_STATUS                 "U4"        无效商户状态
*/
#define ERR_OLDTRANS_CARDERR            "U5"        /* 原卡号与刷卡信息不符 */
#define ERR_OLDTRANS_AUTHERR            "U6"        /* 原授权码与输入不符 */
#define ERR_OLDTRANS_FAIL               "U7"        /* 原交易不成功 */
#define ERR_OLDTRANS_CANCEL             "U8"        /* 原交易已撤销 */
#define ERR_OLDTRANS_RECOVER            "U9"        /* 原交易已冲正 */
#define ERR_OLDTRANS_SETTLE             "UA"        /* 原交易已结算 */

#define ERR_UNDEF_DOWNTYPE              "UB"        /* 下载方式未定义 */

#endif
