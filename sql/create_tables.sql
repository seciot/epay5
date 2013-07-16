--****************************************************************************
-- Copyright(C)2012 - 2015联迪商用设备有限公司                               
-- 主要内容：数据库表创建脚本                                                
-- 创 建 人：高明鑫                                                          
-- 创建日期：2012/11/8                                                       
-----------------------------------------------------------------------------
-- $Revision: 1.14 $                                                         
-- $Log: create_tables.sql,v $
-- Revision 1.14  2013/06/14 02:20:50  fengw
--
-- 1、修改HOST_TERM_KEY表，增加host_no、mag_key字段。
--
-- Revision 1.13  2013/03/11 05:42:05  fengw
--
-- 1、新增emv_key、emv_para表。
--
-- Revision 1.12  2013/02/22 02:49:44  fengw
--
-- 1、trans_def表增加business_type字段。
--
-- Revision 1.11  2013/02/21 06:17:31  fengw
--
-- 1、修改my_customer表语句。
--
-- Revision 1.10  2013/01/05 08:09:32  fengw
--
-- 1、trans_def表新增excep_times字段(异常处理次数)，类型为integer。
--
-- Revision 1.9  2012/12/28 03:28:50  fengw
--
-- 1、posls表、history_ls表、void_ls表新增dept_detail字段。
--
-- Revision 1.8  2012/12/18 07:16:40  chenrb
-- commands表增加控制参数control_para、控制参数长度control_len两个字段
--
-- Revision 1.7  2012/12/18 05:30:16  fengw
--
-- 1、将terminal表的msg_recno字段长度修改为256。
--
-- Revision 1.6  2012/12/17 06:29:13  chenrb
-- trans_commands增加控制参数长度、控制参数2个字段
--
-- Revision 1.5  2012/12/04 03:11:19  fengw
--
-- 1、修改cards表索引。
--
-- Revision 1.4  2012/11/29 07:35:57  chenrb
-- dos2unix转换
--
-- Revision 1.3  2012/11/29 07:31:14  chenrb
-- terminal表增加port字段
--                                                                     
--***************************************************************************

drop table APP_DEF;
create table APP_DEF
(
  APP_TYPE INTEGER,
  APP_NAME VARCHAR(20),
  DESCRIBE VARCHAR(30),
  APP_VER  CHAR(8)
)
;
comment on table APP_DEF
  is '应用定义表';
comment on column APP_DEF.APP_TYPE
  is '应用类型号';
comment on column APP_DEF.APP_NAME
  is '应用名称';
comment on column APP_DEF.DESCRIBE
  is '应用描述';
comment on column APP_DEF.APP_VER
  is '应用版本号';
create unique index APP_TYPE_IDX1 on APP_DEF (APP_TYPE);

drop table APP_MENU;
create table APP_MENU
(
  MENU_NO     INTEGER,
  UP_MENU_NO  INTEGER,
  APP_TYPE    INTEGER,
  LEVEL_1     INTEGER,
  LEVEL_2     INTEGER,
  LEVEL_3     INTEGER,
  MENU_NAME   VARCHAR(20),
  TRANS_CODE  CHAR(8),
  IS_VALID    CHAR(1),
  UPDATE_DATE CHAR(8)
)
;
comment on table APP_MENU
  is '应用菜单';
comment on column APP_MENU.MENU_NO
  is '菜单ID';
comment on column APP_MENU.UP_MENU_NO
  is '上级菜单ID';
comment on column APP_MENU.APP_TYPE
  is '应用类型号';
comment on column APP_MENU.LEVEL_1
  is '一级菜单中的位置';
comment on column APP_MENU.LEVEL_2
  is '二级菜单中的位置';
comment on column APP_MENU.LEVEL_3
  is '三级菜单中的位置';
comment on column APP_MENU.MENU_NAME
  is '菜单名称';
comment on column APP_MENU.TRANS_CODE
  is '交易代码';
comment on column APP_MENU.IS_VALID
  is '有效标识 0-隐藏 1-显示';
comment on column APP_MENU.UPDATE_DATE
  is '最后更新日期';
create unique index APP_MENU_IDX1 on APP_MENU (APP_TYPE, LEVEL_1, LEVEL_2, LEVEL_3);
create unique index APP_MENU_IDX2 on APP_MENU (MENU_NO);

drop table ARTCONF;
create table ARTCONF
(
  PARA_ID    VARCHAR(20),
  PARA_NAME  VARCHAR(40),
  PARA_VALUE VARCHAR(40),
  DESCRIBE  VARCHAR(60)
)
;
comment on table ARTCONF
  is '系统基础参数表';
comment on column ARTCONF.PARA_ID
  is 'ID号码';
comment on column ARTCONF.PARA_NAME
  is '参数名';
comment on column ARTCONF.PARA_VALUE
  is '参数值';
comment on column ARTCONF.DESCRIBE
  is '参数描述';

drop table BIND_CARD;
create table BIND_CARD
(
  PAN           VARCHAR(19),
  REGISTER_DATE CHAR(8)
)
;
comment on table BIND_CARD
  is '卡绑定增量表';
comment on column BIND_CARD.PAN
  is '卡号';
comment on column BIND_CARD.REGISTER_DATE
  is '绑定日期';
create unique index BIND_CARD_IDX on BIND_CARD (PAN);

drop table BIND_CARD_BALANCE;
create table BIND_CARD_BALANCE
(
  PAN     CHAR(19),
  BALANCE NUMERIC(12,2)
)
;
comment on table BIND_CARD_BALANCE
  is '绑定卡日均余额表';
comment on column BIND_CARD_BALANCE.PAN
  is '卡号';
comment on column BIND_CARD_BALANCE.BALANCE
  is '日均余额';
create unique index BIND_CARD_BALANCE_IDX on BIND_CARD_BALANCE (PAN);

drop table CARDS;
create table CARDS
(
  BANK_NAME   VARCHAR(20),
  BANK_ID     CHAR(11),
  CARD_NAME   VARCHAR(40),
  CARD_ID     VARCHAR(19) not null,
  CARD_NO_LEN INTEGER not null,
  CARD_SITE2  INTEGER not null,
  EXP_SITE2   INTEGER default 19,
  PAN_SITE3   INTEGER default 0,
  CARD_SITE3  INTEGER default 0,
  EXP_SITE3   INTEGER default 0,
  CARD_TYPE   CHAR(1),
  CARD_LEVEL  INTEGER default 0
)
;
comment on table CARDS
  is '卡表';
comment on column CARDS.BANK_NAME
  is '银行名称';
comment on column CARDS.BANK_ID
  is '银行标识';
comment on column CARDS.CARD_NAME
  is '卡名称';
comment on column CARDS.CARD_ID
  is '卡标识串';
comment on column CARDS.CARD_NO_LEN
  is '卡号长度';
comment on column CARDS.CARD_SITE2
  is '二磁道卡号位置';
comment on column CARDS.EXP_SITE2
  is '二磁道卡有效期位置';
comment on column CARDS.PAN_SITE3
  is '三磁道账号位置';
comment on column CARDS.CARD_SITE3
  is '三磁道卡号位置';
comment on column CARDS.EXP_SITE3
  is '三磁道有效期位置';
comment on column CARDS.CARD_TYPE
  is '卡类型 0-借记卡 1-贷记卡 2-任意卡 3-准贷记卡';
comment on column CARDS.CARD_LEVEL
  is '卡种类 0-普卡 1-金卡';
CREATE UNIQUE INDEX cards_idx ON cards (card_id, card_no_len);

drop table CEN_OPER;
create table CEN_OPER
(
  OPERATOR   CHAR(4),
  OPER_PWD   CHAR(6),
  OPER_FLAG  CHAR(1),
  OPER_LEVEL CHAR(1)
)
;
comment on table CEN_OPER
  is '中心操作员表';
comment on column CEN_OPER.OPERATOR
  is '操作员号';
comment on column CEN_OPER.OPER_PWD
  is '操作员密码';
comment on column CEN_OPER.OPER_FLAG
  is '操作员状态';
comment on column CEN_OPER.OPER_LEVEL
  is '操作员级别';
create unique index CEN_OPER_IDX on CEN_OPER (OPERATOR);

drop table COMMANDS;
create table COMMANDS
(
  CMD_INDEX    INTEGER,
  ORG_COMMAND   CHAR(2),
  DEST_COMMAND  CHAR(2),
  OPER_INDEX   INTEGER,
  ALOG         CHAR(8),
  COMMAND_NAME VARCHAR(30),
  CONTROL_LEN   INTEGER default 0,
  CONTROL_PARA  CHAR(60)
)
;
comment on table COMMANDS
  is '终端指令集表';
comment on column COMMANDS.CMD_INDEX
  is '顺序号，用于交易定义时排序';
comment on column COMMANDS.ORG_COMMAND
  is '原始指令码';
comment on column COMMANDS.DEST_COMMAND
  is '默认指令码';
comment on column COMMANDS.OPER_INDEX
  is '默认操作提示信息索引号';
comment on column COMMANDS.ALOG
  is '加密及校验算法';
comment on column COMMANDS.COMMAND_NAME
  is '指令名称';
comment on column COMMANDS.CONTROL_LEN
  is '默认控制参数长度';
comment on column COMMANDS.CONTROL_PARA
  is '默认控制参数';
create unique index COMMANDS_IDX on COMMANDS (CMD_INDEX);

drop table COMWEB_PID;
create table COMWEB_PID
(
  SHOP_NO CHAR(15),
  POS_NO  CHAR(15),
  PID     INTEGER
)
;
comment on table COMWEB_PID
  is 'COMWEB进程登记表';
comment on column COMWEB_PID.SHOP_NO
  is '商户号';
comment on column COMWEB_PID.POS_NO
  is '终端号';
comment on column COMWEB_PID.PID
  is 'COMWEB进程ID';
create unique index COMWEB_PID_IDX on COMWEB_PID (SHOP_NO, POS_NO);

drop table DEPT;
create table DEPT
(
  DEPT_NO     VARCHAR(15),
  DEPT_NAME   VARCHAR(45),
  UP_DEPT_NO  VARCHAR(15),
  DEPT_DETAIL VARCHAR(70)
)
;
comment on table DEPT
  is '机构表';
comment on column DEPT.DEPT_NO
  is '机构号';
comment on column DEPT.DEPT_NAME
  is '机构名称';
comment on column DEPT.UP_DEPT_NO
  is '上级机构号';
comment on column DEPT.DEPT_DETAIL
  is '机构层级信息';
create unique index DEPT_IDX on DEPT (DEPT_NO);

drop table DICT_CAT;
create table DICT_CAT
(
  CAT_CODE VARCHAR(20) not null,
  CAT_TEXT VARCHAR(40)
)
;
comment on table DICT_CAT
  is '数据字典表';
comment on column DICT_CAT.CAT_CODE
  is '字典代码';
comment on column DICT_CAT.CAT_TEXT
  is '字典描述';
alter table DICT_CAT
  add constraint PK_DICT_CAT primary key (CAT_CODE);

drop table DICT_ITEM;
create table DICT_ITEM
(
  CAT_CODE  VARCHAR(20) not null,
  ITEM_CODE VARCHAR(20) not null,
  ITEM_TEXT VARCHAR(40)
)
;
comment on table DICT_ITEM
  is '数据字典条目';
comment on column DICT_ITEM.CAT_CODE
  is '字典代码';
comment on column DICT_ITEM.ITEM_CODE
  is '条目代码';
comment on column DICT_ITEM.ITEM_TEXT
  is '条目描述';
alter table DICT_ITEM
   add constraint PK_DICT_ITEM primary key (CAT_CODE, ITEM_CODE);
create index DICT_ITEM_IDX on DICT_ITEM (CAT_CODE);

drop table DYNAMIC_MENU;
create table DYNAMIC_MENU
(
  REC_NO      INTEGER not null,
  MENU_TITLE  VARCHAR(30),
  DESCRIBE   VARCHAR(40),
  MENU_NUM    INTEGER,
  MENU_NAME1  VARCHAR(20),
  TRANS_CODE1 CHAR(8),
  MENU_NAME2  VARCHAR(20),
  TRANS_CODE2 CHAR(8),
  MENU_NAME3  VARCHAR(20),
  TRANS_CODE3 CHAR(8),
  MENU_NAME4  VARCHAR(20),
  TRANS_CODE4 CHAR(8),
  MENU_NAME5  VARCHAR(20),
  TRANS_CODE5 CHAR(8),
  MENU_NAME6  VARCHAR(20),
  TRANS_CODE6 CHAR(8),
  MENU_NAME7  VARCHAR(20),
  TRANS_CODE7 CHAR(8),
  MENU_NAME8  VARCHAR(20),
  TRANS_CODE8 CHAR(8),
  MENU_NAME9  VARCHAR(20),
  TRANS_CODE9 CHAR(8)
)
;
comment on table DYNAMIC_MENU
  is '动态菜单表';
comment on column DYNAMIC_MENU.REC_NO
  is '记录号';
comment on column DYNAMIC_MENU.MENU_TITLE
  is '菜单标题';
comment on column DYNAMIC_MENU.DESCRIBE
  is '描述';
comment on column DYNAMIC_MENU.MENU_NUM
  is '菜单个数';
comment on column DYNAMIC_MENU.MENU_NAME1
  is '菜单一名称';
comment on column DYNAMIC_MENU.TRANS_CODE1
  is '交易代码一';
comment on column DYNAMIC_MENU.MENU_NAME2
  is '菜单二名称';
comment on column DYNAMIC_MENU.TRANS_CODE2
  is '交易代码二';
comment on column DYNAMIC_MENU.MENU_NAME3
  is '菜单三名称';
comment on column DYNAMIC_MENU.TRANS_CODE3
  is '交易代码三';
comment on column DYNAMIC_MENU.MENU_NAME4
  is '菜单四名称';
comment on column DYNAMIC_MENU.TRANS_CODE4
  is '交易代码四';
comment on column DYNAMIC_MENU.MENU_NAME5
  is '菜单五名称';
comment on column DYNAMIC_MENU.TRANS_CODE5
  is '交易代码五';
comment on column DYNAMIC_MENU.MENU_NAME6
  is '菜单六名称';
comment on column DYNAMIC_MENU.TRANS_CODE6
  is '交易代码六';
comment on column DYNAMIC_MENU.MENU_NAME7
  is '菜单七名称';
comment on column DYNAMIC_MENU.TRANS_CODE7
  is '交易代码七';
comment on column DYNAMIC_MENU.MENU_NAME8
  is '菜单八名称';
comment on column DYNAMIC_MENU.TRANS_CODE8
  is '交易代码八';
comment on column DYNAMIC_MENU.MENU_NAME9
  is '菜单九名称';
comment on column DYNAMIC_MENU.TRANS_CODE9
  is '交易代码九';
create unique index DYNAMIC_MENU_IDX on DYNAMIC_MENU (REC_NO);

drop table EPAY_MONI;
create table EPAY_MONI
(
  MONI_TIME   CHAR(14) not null,
  HOST_NO     INTEGER not null,
  HOST_NAME   CHAR(32),
  PROC_STATUS VARCHAR(1024),
  MSG_STATUS  VARCHAR(1024),
  COMM_STATUS VARCHAR(1024),
  SYS_INFO    CLOB
)
;
comment on table EPAY_MONI
  is '系统状态监控表';
comment on column EPAY_MONI.MONI_TIME
  is '监控时间 YYYYMMDDHHMMSS';
comment on column EPAY_MONI.HOST_NO
  is '主机标识';
comment on column EPAY_MONI.HOST_NAME
  is '主机名称';
comment on column EPAY_MONI.PROC_STATUS
  is '进程状态';
comment on column EPAY_MONI.MSG_STATUS
  is '消息队列状态';
comment on column EPAY_MONI.COMM_STATUS
  is '通讯端口状态';
comment on column EPAY_MONI.SYS_INFO
  is '系统详细信息';
create unique index EPAY_MONI_IDX on EPAY_MONI (MONI_TIME, HOST_NO);

drop table ERROR_CODE;
create table ERROR_CODE
(
  RETURN_CODE CHAR(2) not null,
  RETURN_NAME VARCHAR(12),
  POS_MSG     VARCHAR(20),
  HOST1_RET   VARCHAR(40),
  HOST2_RET   VARCHAR(40),
  HOST3_RET   VARCHAR(40),
  HOST4_RET   VARCHAR(40)
)
;
comment on table ERROR_CODE
  is '错误码对照表';
comment on column ERROR_CODE.RETURN_CODE
  is '终端返回码';
comment on column ERROR_CODE.RETURN_NAME
  is '错误描述，显示于监控屏';
comment on column ERROR_CODE.POS_MSG
  is '终端显示错误信息';
comment on column ERROR_CODE.HOST1_RET
  is '主机返回码1';
comment on column ERROR_CODE.HOST2_RET
  is '主机返回码2';
comment on column ERROR_CODE.HOST3_RET
  is '主机返回码3';
comment on column ERROR_CODE.HOST4_RET
  is '主机返回码4';
create unique index ERROR_CODE_IDX on ERROR_CODE (RETURN_CODE);

drop table ERROR_INFO;
create table ERROR_INFO
(
  ERROR_INDEX  INTEGER not null,
  OP_FLAG      CHAR(1),
  MODULE_NUM   INTEGER not null,
  INFO1_FORMAT CHAR(2),
  INFO1        VARCHAR(100),
  INFO2_FORMAT CHAR(2),
  INFO2        VARCHAR(100),
  INFO3_FORMAT CHAR(2),
  INFO3        VARCHAR(100),
  UPDATE_DATE  CHAR(8)
)
;
comment on table ERROR_INFO
  is '错误提示信息表';
comment on column ERROR_INFO.ERROR_INDEX
  is '错误提示信息索引';
comment on column ERROR_INFO.OP_FLAG
  is '信息操作标志';
comment on column ERROR_INFO.MODULE_NUM
  is '模板数';
comment on column ERROR_INFO.INFO1_FORMAT
  is '模板1数据格式';
comment on column ERROR_INFO.INFO1
  is '模板1数据';
comment on column ERROR_INFO.INFO2_FORMAT
  is '模板2数据格式';
comment on column ERROR_INFO.INFO2
  is '模板2数据';
comment on column ERROR_INFO.INFO3_FORMAT
  is '模板3数据格式';
comment on column ERROR_INFO.INFO3
  is '模板3数据';
comment on column ERROR_INFO.UPDATE_DATE
  is '最后更新日期';
create unique index ERROR_INFO_IDX on ERROR_INFO (ERROR_INDEX);

drop table FILES_MGR;
create table FILES_MGR
(
  FILE_ID    INTEGER not null,
  FILE_NAME   VARCHAR(40),
  LAST_DATE   VARCHAR(20),
  LAST_MENDER VARCHAR(20),
  DESCRIBE  VARCHAR(200)
)
;
comment on table FILES_MGR
  is '文件管理表';
comment on column FILES_MGR.FILE_ID
  is '文件id';
comment on column FILES_MGR.FILE_NAME
  is '文件名称';
comment on column FILES_MGR.LAST_DATE
  is '最后修改时间';
comment on column FILES_MGR.LAST_MENDER
  is '最后修改人';
comment on column FILES_MGR.DESCRIBE
  is '文件描述';

drop table FIRST_PAGE;
create table FIRST_PAGE
(
  REC_NO     INTEGER,
  PAGE_TITLE VARCHAR(30),
  DESCRIBE  VARCHAR(200),
  VALID_DATE CHAR(8)
)
;
comment on table FIRST_PAGE
  is '首页信息表';
comment on column FIRST_PAGE.REC_NO
  is '记录号';
comment on column FIRST_PAGE.PAGE_TITLE
  is '信息标题';
comment on column FIRST_PAGE.DESCRIBE
  is '消息内容';
comment on column FIRST_PAGE.VALID_DATE
  is '有效截止日期';
create unique index FIRST_PAGE_IDX on FIRST_PAGE (REC_NO);

drop table FUNCTION_INFO;
create table FUNCTION_INFO
(
  FUNC_INDEX   INTEGER,
  OP_FLAG      CHAR(1),
  MODULE_NUM   INTEGER,
  INFO1_FORMAT CHAR(2),
  INFO1        VARCHAR(100),
  INFO2_FORMAT CHAR(2),
  INFO2        VARCHAR(100),
  INFO3_FORMAT CHAR(2),
  INFO3        VARCHAR(100),
  UPDATE_DATE  CHAR(8)
)
;
comment on table FUNCTION_INFO
  is '功能提示信息表';
comment on column FUNCTION_INFO.FUNC_INDEX
  is '功能提示信息索引';
comment on column FUNCTION_INFO.OP_FLAG
  is '信息操作标志';
comment on column FUNCTION_INFO.MODULE_NUM
  is '模板数';
comment on column FUNCTION_INFO.INFO1_FORMAT
  is '模板1数据格式';
comment on column FUNCTION_INFO.INFO1
  is '模板1数据';
comment on column FUNCTION_INFO.INFO2_FORMAT
  is '模板2数据格式';
comment on column FUNCTION_INFO.INFO2
  is '模板2数据';
comment on column FUNCTION_INFO.INFO3_FORMAT
  is '模板3数据格式';
comment on column FUNCTION_INFO.INFO3
  is '模板3数据';
comment on column FUNCTION_INFO.UPDATE_DATE
  is '最后更新日期';
create unique index FUNCTION_INFO_IDX on FUNCTION_INFO (FUNC_INDEX);

drop table HISTORY_LS;
create table HISTORY_LS
(
  HOST_DATE      CHAR(8),
  HOST_TIME      CHAR(6),
  PAN            VARCHAR(19),
  AMOUNT         NUMERIC(12,2),
  CARD_TYPE      CHAR(1),
  TRANS_TYPE     INTEGER,
  BUSINESS_TYPE  INTEGER,
  RETRI_REF_NUM  CHAR(12),
  AUTH_CODE      CHAR(6),
  POS_NO         CHAR(15),
  SHOP_NO        CHAR(15),
  ACCOUNT2       VARCHAR(19),
  ADDI_AMOUNT    NUMERIC(12,2),
  BATCH_NO       INTEGER,
  PSAM_NO        CHAR(16),
  INVOICE        INTEGER,
  RETURN_CODE    CHAR(2),
  HOST_RET_CODE  VARCHAR(6),
  HOST_RET_MSG	 VARCHAR(40),
  CANCEL_FLAG    CHAR(1),
  RECOVER_FLAG   CHAR(1),
  POS_SETTLE     CHAR(1),
  POS_BATCH      CHAR(1),
  HOST_SETTLE    CHAR(1),
  SYS_TRACE     INTEGER,
  OLD_RETRI_REF_NUM 	CHAR(12),
  POS_DATE       CHAR(8),
  POS_TIME       CHAR(6),
  FINANCIAL_CODE VARCHAR(40),
  BUSINESS_CODE  VARCHAR(40),
  BANK_ID        VARCHAR(11),
  SETTLE_DATE    CHAR(8),
  OPER_NO        CHAR(4),
  MAC            CHAR(16),
  POS_TRACE      INTEGER,
  DEPT_DETAIL    VARCHAR(70)
)
;
comment on table HISTORY_LS
  is '历史流水表';
comment on column HISTORY_LS.HOST_DATE
  is '主机交易日期';
comment on column HISTORY_LS.HOST_TIME
  is '主机交易时间';
comment on column HISTORY_LS.PAN
  is '卡号，转出卡号';
comment on column HISTORY_LS.AMOUNT
  is '交易金额';
comment on column HISTORY_LS.CARD_TYPE
  is '卡类型 0-借记卡 1-贷记卡 2-任意卡 3-准贷记卡';
comment on column HISTORY_LS.TRANS_TYPE
  is '交易类型';
comment on column HISTORY_LS.BUSINESS_TYPE
  is '业务类型，用于分类统计、交易额度控制';
comment on column HISTORY_LS.RETRI_REF_NUM
  is '交易参考号';
comment on column HISTORY_LS.AUTH_CODE
  is '授权码';
comment on column HISTORY_LS.POS_NO
  is '终端号';
comment on column HISTORY_LS.SHOP_NO
  is '商户号';
comment on column HISTORY_LS.ACCOUNT2
  is '卡号2，转入卡号';
comment on column HISTORY_LS.ADDI_AMOUNT
  is '手续费，附加金额';
comment on column HISTORY_LS.BATCH_NO
  is '批次号';
comment on column HISTORY_LS.PSAM_NO
  is '安全模块号';
comment on column HISTORY_LS.INVOICE
  is '发票号';
comment on column HISTORY_LS.RETURN_CODE
  is 'POS返回码';
comment on column HISTORY_LS.HOST_RET_CODE
  is '主机返回码';
comment on column HISTORY_LS.HOST_RET_MSG
  is '主机返回信息'; 
comment on column HISTORY_LS.CANCEL_FLAG
  is '撤销标识位 Y-已撤销 N-未撤销';
comment on column HISTORY_LS.RECOVER_FLAG
  is '冲正标识位 Y-已冲正 N-未冲正 U-不参与结算';
comment on column HISTORY_LS.POS_SETTLE
  is 'POS结算标志位 Y-已结算 N-未结算';
comment on column HISTORY_LS.POS_BATCH
  is 'POS批上送标识位';
comment on column HISTORY_LS.HOST_SETTLE
  is '主机结算标识位';
comment on column HISTORY_LS.SYS_TRACE
  is '系统流水号';
comment on column HISTORY_LS.OLD_RETRI_REF_NUM
  is '原交易参考号';
comment on column HISTORY_LS.POS_DATE
  is 'POS交易日期';
comment on column HISTORY_LS.POS_TIME
  is 'POS交易时间';
comment on column HISTORY_LS.FINANCIAL_CODE
  is '金融应用号';
comment on column HISTORY_LS.BUSINESS_CODE
  is '商务应用号';
comment on column HISTORY_LS.BANK_ID
  is '银行标识号';
comment on column HISTORY_LS.SETTLE_DATE
  is '结算日期';
comment on column HISTORY_LS.OPER_NO
  is '操作员号';
comment on column HISTORY_LS.MAC
  is 'MAC值';
comment on column HISTORY_LS.POS_TRACE
  is '终端流水号';
comment on column HISTORY_LS.DEPT_DETAIL
  is '机构层级信息';
create index HISTORY_LS_IDX1 on HISTORY_LS (HOST_DATE);
create index HISTORY_LS_IDX2 on HISTORY_LS (POS_NO, SHOP_NO,POS_DATE, POS_TRACE);
create index HISTORY_LS_IDX3 on HISTORY_LS (SYS_TRACE);

drop table HOST_ERROR;
create table HOST_ERROR
(
  RETURN_CODE VARCHAR(6) not null,
  ERROR_MSG   VARCHAR(80)
)
;
comment on table HOST_ERROR
  is '主机错误码表';
comment on column HOST_ERROR.RETURN_CODE
  is '返回码';
comment on column HOST_ERROR.ERROR_MSG
  is '错误信息';
create unique index HOST_ERROR_IDX on HOST_ERROR (RETURN_CODE);

drop table HOST_KEY;
create table HOST_KEY
(
  HOST_ID   INTEGER not null,
  HOST_NAME VARCHAR(40),
  MAC_KEY   CHAR(32),
  PIN_KEY   CHAR(32)
)
;
comment on table HOST_KEY
  is '主机密钥表，机构方式对接采用该密钥表存储工作密钥';
comment on column HOST_KEY.HOST_ID
  is '主机ID号';
comment on column HOST_KEY.HOST_NAME
  is '主机名称';
comment on column HOST_KEY.MAC_KEY
  is 'MAC密钥';
comment on column HOST_KEY.PIN_KEY
  is 'PIN密钥';
create unique index HOST_KEY_IDX on HOST_KEY (HOST_ID);

drop table HOST_TERM_KEY;
create table HOST_TERM_KEY
(
  HOST_NO           INTEGER NOT NULL,
  SHOP_NO          	CHAR(15) not null,
  POS_NO           	CHAR(15) not null,
  MASTER_KEY       	CHAR(32),
  PIN_KEY           CHAR(32),
  MAC_KEY          	CHAR(32),
  MAG_KEY          	CHAR(32),
  TERM_VERSION      	CHAR(7),
  CAPK_VERSION      	CHAR(8),
  PARA_VERSION      	CHAR(12) default '2',
  CARD_TABLE_VERSION 	CHAR(14)
)
;
comment on table HOST_TERM_KEY
  is '主机终端密钥表，终端方式对接采用该密钥表存储密钥';
comment on column HOST_TERM_KEY.HOST_NO
  is '主机号';
comment on column HOST_TERM_KEY.SHOP_NO
  is '商户号';
comment on column HOST_TERM_KEY.POS_NO
  is '终端号';
comment on column HOST_TERM_KEY.MASTER_KEY
  is '终端主密钥';
comment on column HOST_TERM_KEY.PIN_KEY
  is 'PIN密钥';
comment on column HOST_TERM_KEY.MAC_KEY
  is 'MAC密钥';
comment on column HOST_TERM_KEY.MAG_KEY
  is 'MAG密钥';
comment on column HOST_TERM_KEY.TERM_VERSION
  is '终端版本';
comment on column HOST_TERM_KEY.CAPK_VERSION
  is 'CAPK版本';
comment on column HOST_TERM_KEY.PARA_VERSION
  is '参数版本';
comment on column HOST_TERM_KEY.CARD_TABLE_VERSION
  is '卡表类别版本';
create unique index HOST_TERM_KEY_IDX on HOST_TERM_KEY (HOST_NO, SHOP_NO, POS_NO);

drop table VOID_LS;
create table VOID_LS
(
  HOST_DATE      CHAR(8),
  HOST_TIME      CHAR(6),
  PAN            VARCHAR(19),
  AMOUNT         NUMERIC(12,2),
  CARD_TYPE      CHAR(1),
  TRANS_TYPE     INTEGER,
  BUSINESS_TYPE  INTEGER,
  RETRI_REF_NUM  CHAR(12),
  AUTH_CODE      CHAR(6),
  POS_NO         CHAR(15),
  SHOP_NO        CHAR(15),
  ACCOUNT2       VARCHAR(19),
  ADDI_AMOUNT    NUMERIC(12,2),
  BATCH_NO       INTEGER,
  PSAM_NO        CHAR(16),
  INVOICE        INTEGER,
  RETURN_CODE    CHAR(2),
  HOST_RET_CODE  VARCHAR(6),
  HOST_RET_MSG	 VARCHAR(40),
  CANCEL_FLAG    CHAR(1),
  RECOVER_FLAG   CHAR(1),
  POS_SETTLE     CHAR(1),
  POS_BATCH      CHAR(1),
  HOST_SETTLE    CHAR(1),
  SYS_TRACE     INTEGER,
  OLD_RETRI_REF_NUM 	CHAR(12),
  POS_DATE       CHAR(8),
  POS_TIME       CHAR(6),
  FINANCIAL_CODE VARCHAR(40),
  BUSINESS_CODE  VARCHAR(40),
  BANK_ID        VARCHAR(11),
  SETTLE_DATE    CHAR(8),
  OPER_NO        CHAR(4),
  MAC            CHAR(16),
  POS_TRACE      INTEGER,
  DEPT_DETAIL    VARCHAR(70)
)
;
comment on table VOID_LS
  is '冲正流水表';
comment on column VOID_LS.HOST_DATE
  is '主机交易日期';
comment on column VOID_LS.HOST_TIME
  is '主机交易时间';
comment on column VOID_LS.PAN
  is '卡号，转出卡号';
comment on column VOID_LS.AMOUNT
  is '交易金额';
comment on column VOID_LS.CARD_TYPE
  is '卡类型 0-借记卡 1-贷记卡 2-任意卡 3-准贷记卡';
comment on column VOID_LS.TRANS_TYPE
  is '交易类型';
comment on column VOID_LS.BUSINESS_TYPE
  is '业务类型，用于分类统计、交易额度控制';
comment on column VOID_LS.RETRI_REF_NUM
  is '交易参考号';
comment on column VOID_LS.AUTH_CODE
  is '授权码';
comment on column VOID_LS.POS_NO
  is '终端号';
comment on column VOID_LS.SHOP_NO
  is '商户号';
comment on column VOID_LS.ACCOUNT2
  is '卡号2，转入卡号';
comment on column VOID_LS.ADDI_AMOUNT
  is '手续费，附加金额';
comment on column VOID_LS.BATCH_NO
  is '批次号';
comment on column VOID_LS.PSAM_NO
  is '安全模块号';
comment on column VOID_LS.INVOICE
  is '发票号';
comment on column VOID_LS.RETURN_CODE
  is 'POS返回码';
comment on column VOID_LS.HOST_RET_CODE
  is '主机返回码';
comment on column VOID_LS.HOST_RET_MSG
  is '主机返回信息'; 
comment on column VOID_LS.CANCEL_FLAG
  is '撤销标识位 Y-已撤销 N-未撤销';
comment on column VOID_LS.RECOVER_FLAG
  is '冲正标识位 Y-已冲正 N-未冲正 U-不参与结算';
comment on column VOID_LS.POS_SETTLE
  is 'POS结算标志位 Y-已结算 N-未结算';
comment on column VOID_LS.POS_BATCH
  is 'POS批上送标识位';
comment on column VOID_LS.HOST_SETTLE
  is '主机结算标识位';
comment on column VOID_LS.SYS_TRACE
  is '系统流水号';
comment on column VOID_LS.OLD_RETRI_REF_NUM
  is '原交易参考号';
comment on column VOID_LS.POS_DATE
  is 'POS交易日期';
comment on column VOID_LS.POS_TIME
  is 'POS交易时间';
comment on column VOID_LS.FINANCIAL_CODE
  is '金融应用号';
comment on column VOID_LS.BUSINESS_CODE
  is '商务应用号';
comment on column VOID_LS.BANK_ID
  is '银行标识号';
comment on column VOID_LS.SETTLE_DATE
  is '结算日期';
comment on column VOID_LS.OPER_NO
  is '操作员号';
comment on column VOID_LS.MAC
  is 'MAC值';
comment on column VOID_LS.POS_TRACE
  is '终端流水号';
comment on column VOID_LS.DEPT_DETAIL
  is '机构层级信息';
create index VOID_LS_IDX1 on VOID_LS (HOST_DATE);
create index VOID_LS_IDX2 on VOID_LS (POS_NO, SHOP_NO, POS_DATE,POS_TRACE);
create index VOID_LS_IDX3 on VOID_LS (SYS_TRACE);

drop table ISO8583;
create table ISO8583
(
  BANK_TYPE  INTEGER not null,
  FIELD_ID   INTEGER not null,
  MAX_LEN    INTEGER,
  FIELD_TYPE INTEGER,
  FIELD_FLAG INTEGER,
  LEN_FLAG   INTEGER default 2
)
;
comment on table ISO8583
  is 'ISO8583域属性配置表';
comment on column ISO8583.BANK_TYPE
  is '银行类型';
comment on column ISO8583.FIELD_ID
  is '域号';
comment on column ISO8583.MAX_LEN
  is '域最大长度';
comment on column ISO8583.FIELD_TYPE
  is '域类型 0-ASC 1-首位借贷记标识 2-右对齐压缩BCD 3-左对齐压缩BCD 8-二进制数据';
comment on column ISO8583.FIELD_FLAG
  is '长度标识 0-定长 1-LLVAR型(2字节表示长度) 2--LLLVAR型(3字节表示长度)';
comment on column ISO8583.LEN_FLAG
  is '长度表示方式 0-十进制长度，"\x31\x32"表示长度为12; 1-十六进制长度，"\x01\x00"表示长度为256; 2-压缩BCD码长度，"\x01\x23"表示长度为123';
alter table ISO8583
  add constraint PK_ISO8583 primary key (BANK_TYPE, FIELD_ID);
create unique index ISO8583_IDX on ISO8583(FIELD_ID, BANK_TYPE);

drop table ISO8583_REQ_FIELD;
create table ISO8583_REQ_FIELD
(
  BANK_TYPE    INTEGER not null,
  TRANS_TYPE   INTEGER not null,
  FIELD_ID     INTEGER not null,
  FIELD_NAME   VARCHAR(50),
  FIELD_FORMAT VARCHAR(40),
  DEFAULT_DATA VARCHAR(40),
  MONDARY      INTEGER default 0
)
;
comment on table ISO8583_REQ_FIELD
  is 'ISO8583请求报文配置表';
comment on column ISO8583_REQ_FIELD.BANK_TYPE
  is '银行类型';
comment on column ISO8583_REQ_FIELD.TRANS_TYPE
  is '交易类型';
comment on column ISO8583_REQ_FIELD.FIELD_ID
  is '域号';
comment on column ISO8583_REQ_FIELD.FIELD_NAME
  is '域名称';
comment on column ISO8583_REQ_FIELD.FIELD_FORMAT
  is '域格式';
comment on column ISO8583_REQ_FIELD.DEFAULT_DATA
  is '域默认数据';
comment on column ISO8583_REQ_FIELD.MONDARY
  is '是否必选,0-不,1-是';
alter table ISO8583_REQ_FIELD
  add constraint PK_ISO8583_REQ_FIELD primary key (BANK_TYPE, TRANS_TYPE, FIELD_ID);

drop table ISO8583_RSP_FIELD;
create table ISO8583_RSP_FIELD
(
  BANK_TYPE    INTEGER not null,
  TRANS_TYPE   INTEGER not null,
  FIELD_ID     INTEGER not null,
  FIELD_NAME   VARCHAR(50),
  FIELD_FORMAT VARCHAR(40),
  DEFAULT_DATA VARCHAR(40),
  MONDARY      INTEGER	default 0
)
;
comment on table ISO8583_RSP_FIELD
  is 'ISO8583响应报文域配置表';
comment on column ISO8583_RSP_FIELD.BANK_TYPE
  is '银行类型';
comment on column ISO8583_RSP_FIELD.TRANS_TYPE
  is '交易类型';
comment on column ISO8583_RSP_FIELD.FIELD_ID
  is '域号';
comment on column ISO8583_RSP_FIELD.FIELD_NAME
  is '域名称';
comment on column ISO8583_RSP_FIELD.FIELD_FORMAT
  is '域格式';
comment on column ISO8583_RSP_FIELD.DEFAULT_DATA
  is '域默认数据';
comment on column ISO8583_RSP_FIELD.MONDARY
  is '是否必选,0-不,1-是';
alter table ISO8583_RSP_FIELD
  add constraint PK_ISO8583_RSP_FIELD primary key (BANK_TYPE, TRANS_TYPE, FIELD_ID);

drop table LOCAL_CARD;
create table LOCAL_CARD
(
  CARD_ID     VARCHAR(16) not null,
  CARD_NAME   VARCHAR(40) not null,
  CARD_NO_LEN INTEGER not null,
  CARD_TYPE   CHAR(1) default '0' not null
)
;
comment on table LOCAL_CARD
  is '本地卡表';
comment on column LOCAL_CARD.CARD_ID
  is '卡号';
comment on column LOCAL_CARD.CARD_NAME
  is '卡名称';
comment on column LOCAL_CARD.CARD_NO_LEN
  is '卡号长度';
comment on column LOCAL_CARD.CARD_TYPE
  is '卡类型';
create unique index LOCAL_CARD_IDX on LOCAL_CARD (CARD_ID);

drop table MARKET;
create table MARKET
(
  DEPT_NO     VARCHAR(15),
  MARKET_NO   INTEGER,
  MARKET_NAME VARCHAR(40)
)
;
comment on table MARKET
  is '市场信息表';
comment on column MARKET.DEPT_NO
  is '机构号';
comment on column MARKET.MARKET_NO
  is '市场号';
comment on column MARKET.MARKET_NAME
  is '市场名称';
create unique index MARKET_IDX on MARKET (MARKET_NO);

drop table MODULE;
create table MODULE
(
  MODULE_ID   INTEGER,
  MODULE_NAME VARCHAR(32),
  MSG_TYPE    INTEGER,
  PARA1       VARCHAR(15),
  PARA2       VARCHAR(10),
  PARA3       VARCHAR(10),
  PARA4       VARCHAR(10),
  PARA5       VARCHAR(10),
  PARA6       VARCHAR(10),
  RUN         INTEGER
)
;
comment on table MODULE
  is '模块表';
comment on column MODULE.MODULE_ID
  is '模块ID';
comment on column MODULE.MODULE_NAME
  is '模块名称(进程名称)';
comment on column MODULE.MSG_TYPE
  is '接收消息类型';
comment on column MODULE.PARA1
  is '启动参数1';
comment on column MODULE.PARA2
  is '启动参数2';
comment on column MODULE.PARA3
  is '启动参数3';
comment on column MODULE.PARA4
  is '启动参数4';
comment on column MODULE.PARA5
  is '启动参数5';
comment on column MODULE.PARA6
  is '启动参数6';
comment on column MODULE.RUN
  is '是否运行 1-运行 0-不运行';
create unique index MODULE_IDX on MODULE (MODULE_ID);

drop table MY_CUSTOMER;
create table MY_CUSTOMER
(
  SHOP_NO       CHAR(15) not null,
  POS_NO        CHAR(15) not null,
  PAN           VARCHAR(19) not null,
  ACCT_NAME     VARCHAR(40),
  EXPIRE_DATE   CHAR(4),
  BANK_ID       CHAR(12),
  BANK_NAME     VARCHAR(20),
  REGISTER_DATE CHAR(8),
  REC_NO        INTEGER default 0 not null
)
;
comment on table MY_CUSTOMER
  is '收款人信息表';
comment on column MY_CUSTOMER.SHOP_NO
  is '商户号';
comment on column MY_CUSTOMER.POS_NO
  is '终端号';
comment on column MY_CUSTOMER.PAN
  is '卡号';
comment on column MY_CUSTOMER.ACCT_NAME
  is '户名';
comment on column MY_CUSTOMER.EXPIRE_DATE
  is '有效期';
comment on column MY_CUSTOMER.BANK_ID
  is '联行行号';
comment on column MY_CUSTOMER.BANK_NAME
  is '开户行';
comment on column MY_CUSTOMER.REGISTER_DATE
  is '登记日期';
comment on column MY_CUSTOMER.REC_NO
  is '序号';
create unique index MY_CUSTOMER_IDX on MY_CUSTOMER (shop_no, pos_no, pan);

drop table OPER;
create table OPER
(
  OPER_NO      VARCHAR(8) not null,
  OPER_NAME    VARCHAR(40),
  REAL_ROLE_ID INTEGER,
  PASSWORD     CHAR(32),
  AUTH_CODE    CHAR(12),
  DEPT_NO      VARCHAR(15),
  DEPT_NAME    VARCHAR(40),
  CREATE_DATE  VARCHAR(20),
  IS_VALID     CHAR(1),
  LAST_LOGIN   VARCHAR(20),
  BRANCH_ID    CHAR(6),
  IS_ADMIN     CHAR(1),
  ENTER_DATE   CHAR(8),
  FAIL_TIMES   INTEGER,
  EFFECT_DATE  CHAR(8),
  MAC          CHAR(14),
  DEPT_DETAIL  VARCHAR(70)
)
;
comment on table OPER
  is '管理监控系统操作员表';
comment on column OPER.OPER_NO
  is '操作员ID';
comment on column OPER.OPER_NAME
  is '操作员名称';
comment on column OPER.REAL_ROLE_ID
  is '所属角色ID';
comment on column OPER.PASSWORD
  is '操作员密码';
comment on column OPER.AUTH_CODE
  is '授权码';
comment on column OPER.DEPT_NO
  is '所属机构';
comment on column OPER.DEPT_NAME
  is '机构名称';
comment on column OPER.CREATE_DATE
  is '创建日期';
comment on column OPER.IS_VALID
  is '是否有效';
comment on column OPER.LAST_LOGIN
  is '最近登录时间';
comment on column OPER.IS_ADMIN
  is '是否admin';
comment on column OPER.ENTER_DATE
  is '当前登录日期';
comment on column OPER.FAIL_TIMES
  is '密码错误次数';
comment on column OPER.EFFECT_DATE
  is '有效截至日期';
comment on column OPER.MAC
  is '单点登录控制参数';
comment on column OPER.DEPT_DETAIL
  is '机构层级信息';
alter table OPER
  add constraint PK_T_OPER_INFO primary key (OPER_NO);

drop table OPERATION_INFO;
create table OPERATION_INFO
(
  OPER_INDEX   INTEGER,
  OP_FLAG      CHAR(1),
  MODULE_NUM   INTEGER,
  INFO1_FORMAT CHAR(2),
  INFO1        VARCHAR(40),
  INFO2_FORMAT CHAR(2),
  INFO2        VARCHAR(40),
  INFO3_FORMAT CHAR(2),
  INFO3        VARCHAR(40),
  UPDATE_DATE  CHAR(8)
)
;
comment on table OPERATION_INFO
  is '操作提示信息表';
comment on column OPERATION_INFO.OPER_INDEX
  is '操作提示信息索引';
comment on column OPERATION_INFO.OP_FLAG
  is '操作标识';
comment on column OPERATION_INFO.MODULE_NUM
  is '模块数';
comment on column OPERATION_INFO.INFO1_FORMAT
  is '提示信息1数据格式';
comment on column OPERATION_INFO.INFO1
  is '提示信息1数据';
comment on column OPERATION_INFO.INFO2_FORMAT
  is '提示信息2数据格式';
comment on column OPERATION_INFO.INFO2
  is '提示信息2数据';
comment on column OPERATION_INFO.INFO3_FORMAT
  is '提示信息3数据格式';
comment on column OPERATION_INFO.INFO3
  is '提示信息3数据';
comment on column OPERATION_INFO.UPDATE_DATE
  is '最后更新日期';
create unique index OPERATION_INFO_IDX on OPERATION_INFO (OPER_INDEX);

drop table OPERATION_TEMP;
create table OPERATION_TEMP
(
  OPER_INDEX   INTEGER,
  OP_FLAG      CHAR(1),
  MODULE_NUM   INTEGER,
  INFO1_FORMAT CHAR(2),
  INFO1        VARCHAR(40),
  INFO2_FORMAT CHAR(2),
  INFO2        VARCHAR(40),
  INFO3_FORMAT CHAR(2),
  INFO3        VARCHAR(40),
  UPDATE_DATE  CHAR(8)
)
;
comment on table OPERATION_TEMP
  is '临时提示信息表';
comment on column OPERATION_TEMP.OPER_INDEX
  is '操作提示信息索引';
comment on column OPERATION_TEMP.OP_FLAG
  is '信息操作标志';
comment on column OPERATION_TEMP.MODULE_NUM
  is '模块数';
comment on column OPERATION_TEMP.INFO1_FORMAT
  is '操作提示1数据格式';
comment on column OPERATION_TEMP.INFO1
  is '操作提示1数据';
comment on column OPERATION_TEMP.INFO2_FORMAT
  is '操作提示2数据格式';
comment on column OPERATION_TEMP.INFO2
  is '操作提示2数据';
comment on column OPERATION_TEMP.INFO3_FORMAT
  is '操作提示3数据格式';
comment on column OPERATION_TEMP.INFO3
  is '操作提示3数据';
comment on column OPERATION_TEMP.UPDATE_DATE
  is '最后更新日期';
create unique index OPERATION_TEMP_IDX on OPERATION_TEMP (OPER_INDEX);

drop table OPERATION_LOG;
create table OPERATION_LOG
(
  ID	      INTEGER not null,
  MODULE_NAME VARCHAR(30),
  METHOD_DESC VARCHAR(20),
  KEY_ID      VARCHAR(20),
  KEY_NAME    VARCHAR(50),
  USER_NAME   VARCHAR(20),
  OPER_TIME   VARCHAR(30)
)
;
comment on table OPERATION_LOG
  is '管理监控系统操作日志表';
comment on column OPERATION_LOG.ID
  is '系统自动维护id';
comment on column OPERATION_LOG.MODULE_NAME
  is '模块名称';
comment on column OPERATION_LOG.METHOD_DESC
  is '操作方法，增加、删除、修改等';
comment on column OPERATION_LOG.KEY_ID
  is '被操作表记录关键值id，例如操作员号';
comment on column OPERATION_LOG.KEY_NAME
  is '被操作表记录关键值名称，例如操作员名称';
comment on column OPERATION_LOG.USER_NAME
  is '操作员名称';
comment on column OPERATION_LOG.OPER_TIME
  is '操作时间';

drop table PAY_CLASS;
create table PAY_CLASS
(
  LIST_CLASS INTEGER,
  CLASS_NAME VARCHAR(30),
  CONTACTOR  VARCHAR(10),
  TELEPHONE  VARCHAR(26),
  ADDR       VARCHAR(48)
)
;
comment on table PAY_CLASS
  is '账单大类表';
comment on column PAY_CLASS.LIST_CLASS
  is '账单类别';
comment on column PAY_CLASS.CLASS_NAME
  is '类别名称';
comment on column PAY_CLASS.CONTACTOR
  is '联系人';
comment on column PAY_CLASS.TELEPHONE
  is '联系电话';
comment on column PAY_CLASS.ADDR
  is '地址';
create unique index PAY_CLASS_IDX on PAY_CLASS (LIST_CLASS);

drop table PAY_LIST;
create table PAY_LIST
(
  PSAM_NO    CHAR(16),
  LIST_CLASS INTEGER,
  LIST_TYPE  INTEGER,
  GEN_DATE   CHAR(8),
  LIST_NO    INTEGER,
  LIST_DATA  CHAR(200),
  AMOUNT     NUMERIC(12,2),
  PAY_DATE   CHAR(8),
  DOWN_FLAG  CHAR(1) default 'N',
  PAY_STATUS     CHAR(1) default 'N'
)
;
comment on table PAY_LIST
  is '支付账单表';
comment on column PAY_LIST.PSAM_NO
  is '安全模块号';
comment on column PAY_LIST.LIST_CLASS
  is '账单大类';
comment on column PAY_LIST.LIST_TYPE
  is '账单小类';
comment on column PAY_LIST.GEN_DATE
  is '账单生成日期';
comment on column PAY_LIST.LIST_NO
  is '账单号';
comment on column PAY_LIST.LIST_DATA
  is '账单数据';
comment on column PAY_LIST.AMOUNT
  is '金额';
comment on column PAY_LIST.PAY_DATE
  is '支付日期';
comment on column PAY_LIST.DOWN_FLAG
  is '下载标识 Y-已下载 N-未下载';
comment on column PAY_LIST.PAY_STATUS
  is '支付标识 Y-已支付 N-未支付';

drop table PAY_TYPE;
create table PAY_TYPE
(
  LIST_CLASS INTEGER,
  LIST_TYPE  INTEGER,
  TYPE_NAME  VARCHAR(30),
  TRANS_CODE CHAR(8)
)
;
comment on table PAY_TYPE
  is '账单小类表';
comment on column PAY_TYPE.LIST_CLASS
  is '账单大类';
comment on column PAY_TYPE.LIST_TYPE
  is '账单小类';
comment on column PAY_TYPE.TYPE_NAME
  is '类型名称';
comment on column PAY_TYPE.TRANS_CODE
  is '交易代码';
create unique index PAY_TYPE_IDX on PAY_TYPE (LIST_CLASS, LIST_TYPE);

drop table TDI_MATCH;
create table TDI_MATCH
(
  TRANS_DATA_INDEX        INTEGER,
  LOCAL_DATE CHAR(8),
  SHOP_NO    CHAR(15),
  POS_NO     CHAR(15),
  SEND_TIME  INTEGER,
  SYS_TRACE INTEGER
)
;
comment on table TDI_MATCH
  is '交易数据索引匹配表';
comment on column TDI_MATCH.TRANS_DATA_INDEX
  is '交易数据索引号';
comment on column TDI_MATCH.LOCAL_DATE
  is '本地日期';
comment on column TDI_MATCH.SHOP_NO
  is '商户号';
comment on column TDI_MATCH.POS_NO
  is '终端号';
comment on column TDI_MATCH.SEND_TIME
  is '发送时间，用于判断交易是否超时返回';
comment on column TDI_MATCH.SYS_TRACE
  is '系统流水号';
create unique index TDI_MATCH_IDX on TDI_MATCH(SHOP_NO, POS_NO, SYS_TRACE);

drop table POSLS;
create table POSLS
(
  HOST_DATE      CHAR(8),
  HOST_TIME      CHAR(6),
  PAN            VARCHAR(19),
  AMOUNT         NUMERIC(12,2),
  CARD_TYPE      CHAR(1),
  TRANS_TYPE     INTEGER,
  BUSINESS_TYPE  INTEGER,
  RETRI_REF_NUM  CHAR(12),
  AUTH_CODE      CHAR(6),
  POS_NO         CHAR(15),
  SHOP_NO        CHAR(15),
  ACCOUNT2       VARCHAR(19),
  ADDI_AMOUNT    NUMERIC(12,2),
  BATCH_NO       INTEGER,
  PSAM_NO        CHAR(16),
  INVOICE        INTEGER,
  RETURN_CODE    CHAR(2),
  HOST_RET_CODE  VARCHAR(6),
  HOST_RET_MSG	 VARCHAR(40),
  CANCEL_FLAG    CHAR(1),
  RECOVER_FLAG   CHAR(1),
  POS_SETTLE     CHAR(1),
  POS_BATCH      CHAR(1),
  HOST_SETTLE    CHAR(1),
  SYS_TRACE     INTEGER,
  OLD_RETRI_REF_NUM 	CHAR(12),
  POS_DATE       CHAR(8),
  POS_TIME       CHAR(6),
  FINANCIAL_CODE VARCHAR(40),
  BUSINESS_CODE  VARCHAR(40),
  BANK_ID        VARCHAR(11),
  SETTLE_DATE    CHAR(8),
  OPER_NO        CHAR(4),
  MAC            CHAR(16),
  POS_TRACE      INTEGER,
  DEPT_DETAIL    VARCHAR(70)
)
;
comment on table POSLS
  is 'POS流水表';
comment on column POSLS.HOST_DATE
  is '主机交易日期';
comment on column POSLS.HOST_TIME
  is '主机交易时间';
comment on column POSLS.PAN
  is '卡号，转出卡号';
comment on column POSLS.AMOUNT
  is '交易金额';
comment on column POSLS.CARD_TYPE
  is '卡类型 0-借记卡 1-贷记卡 2-任意卡 3-准贷记卡';
comment on column POSLS.TRANS_TYPE
  is '交易类型';
comment on column POSLS.BUSINESS_TYPE
  is '业务类型，用于分类统计、交易额度控制';
comment on column POSLS.RETRI_REF_NUM
  is '交易参考号';
comment on column POSLS.AUTH_CODE
  is '授权码';
comment on column POSLS.POS_NO
  is '终端号';
comment on column POSLS.SHOP_NO
  is '商户号';
comment on column POSLS.ACCOUNT2
  is '卡号2，转入卡号';
comment on column POSLS.ADDI_AMOUNT
  is '手续费，附加金额';
comment on column POSLS.BATCH_NO
  is '批次号';
comment on column POSLS.PSAM_NO
  is '安全模块号';
comment on column POSLS.INVOICE
  is '发票号';
comment on column POSLS.RETURN_CODE
  is 'POS返回码';
comment on column POSLS.HOST_RET_CODE
  is '主机返回码';
comment on column POSLS.HOST_RET_MSG
  is '主机返回信息';  
comment on column POSLS.CANCEL_FLAG
  is '撤销标识位 Y-已撤销 N-未撤销';
comment on column POSLS.RECOVER_FLAG
  is '冲正标识位 Y-已冲正 N-未冲正 U-不参与结算';
comment on column POSLS.POS_SETTLE
  is 'POS结算标志位 Y-已结算 N-未结算';
comment on column POSLS.POS_BATCH
  is 'POS批上送标识位';
comment on column POSLS.HOST_SETTLE
  is '主机结算标识位';
comment on column POSLS.SYS_TRACE
  is '系统流水号';
comment on column POSLS.OLD_RETRI_REF_NUM
  is '原交易参考号';
comment on column POSLS.POS_DATE
  is 'POS交易日期';
comment on column POSLS.POS_TIME
  is 'POS交易时间';
comment on column POSLS.FINANCIAL_CODE
  is '金融应用号';
comment on column POSLS.BUSINESS_CODE
  is '商务应用号';
comment on column POSLS.BANK_ID
  is '银行标识号';
comment on column POSLS.SETTLE_DATE
  is '结算日期';
comment on column POSLS.OPER_NO
  is '操作员号';
comment on column POSLS.MAC
  is 'MAC值';
comment on column POSLS.POS_TRACE
  is '终端流水号';
comment on column POSLS.DEPT_DETAIL
  is '机构层级信息';
create unique index POSLS_IDX on POSLS (SHOP_NO, POS_NO,POS_DATE, POS_TRACE);

drop table POS_KEY;
create table POS_KEY
(
  KEY_INDEX      INTEGER,
  MASTER_KEY     CHAR(32),
  MASTER_KEY_LMK CHAR(32),
  MASTER_CHK     CHAR(4),
  PIN_KEY        CHAR(32),
  MAC_KEY        CHAR(32),
  MAG_KEY        CHAR(32)
)
;
comment on table POS_KEY
  is '终端密钥表';
comment on column POS_KEY.KEY_INDEX
  is '密钥索引';
comment on column POS_KEY.MASTER_KEY
  is '分发给终端的终端主密钥';
comment on column POS_KEY.MASTER_KEY_LMK
  is 'LKM加密后的终端主密钥';
comment on column POS_KEY.MASTER_CHK
  is '终端主密钥校验值';
comment on column POS_KEY.PIN_KEY
  is 'pin密钥';
comment on column POS_KEY.MAC_KEY
  is 'mac密钥';
comment on column POS_KEY.MAG_KEY
  is 'mag密钥';
create unique index POS_KEY_IDX on POS_KEY (KEY_INDEX);

drop table PRINT_DATA;
create table PRINT_DATA
(
  DATA_INDEX   INTEGER,
  DATA_NAME    VARCHAR(40),
  DATA_EXAMPLE VARCHAR(40)
)
;
comment on table PRINT_DATA
  is '打印数据项表';
comment on column PRINT_DATA.DATA_INDEX
  is '数据索引';
comment on column PRINT_DATA.DATA_NAME
  is '数据名称';
comment on column PRINT_DATA.DATA_EXAMPLE
  is '数据格式样例';
create unique index PRINT_DATA_IDX on PRINT_DATA (DATA_INDEX);

drop table PRINT_INFO;
create table PRINT_INFO
(
  REC_NO      INTEGER,
  INFO        VARCHAR(60),
  DATA_INDEX  INTEGER,
  UPDATE_DATE CHAR(8)
)
;
comment on table PRINT_INFO
  is '打印记录表';
comment on column PRINT_INFO.REC_NO
  is '记录号';
comment on column PRINT_INFO.INFO
  is '数据信息';
comment on column PRINT_INFO.DATA_INDEX
  is '数据索引';
comment on column PRINT_INFO.UPDATE_DATE
  is '更新日期';
create unique index PRINT_INFO_IDX  on PRINT_INFO (REC_NO);

drop table PRINT_MODULE;
create table PRINT_MODULE
(
  MODULE_ID  INTEGER,
  DESCRIBE VARCHAR(40),
  PRINT_NUM  INTEGER,
  TITLE1     INTEGER default 0,
  TITLE2     INTEGER default 0,
  TITLE3     INTEGER default 0,
  SIGN1      INTEGER default 0,
  SIGN2      INTEGER default 0,
  SIGN3      INTEGER default 0,
  REC_NUM    INTEGER,
  REC_NO     VARCHAR(80)
)
;
comment on table PRINT_MODULE
  is '打印凭条模板表';
comment on column PRINT_MODULE.MODULE_ID
  is '模版编号';
comment on column PRINT_MODULE.DESCRIBE
  is '描述';
comment on column PRINT_MODULE.PRINT_NUM
  is '打印份数';
comment on column PRINT_MODULE.TITLE1
  is '标题1';
comment on column PRINT_MODULE.TITLE2
  is '标题2';
comment on column PRINT_MODULE.TITLE3
  is '标题3';
comment on column PRINT_MODULE.SIGN1
  is '落款1';
comment on column PRINT_MODULE.SIGN2
  is '落款2';
comment on column PRINT_MODULE.SIGN3
  is '落款3';
comment on column PRINT_MODULE.REC_NUM
  is '记录数';
comment on column PRINT_MODULE.REC_NO
  is '记录号';
create unique index PRINT_MODULE_IDX on PRINT_MODULE (MODULE_ID);

drop table PSAM_PARA;
create table PSAM_PARA
(
  MODULE_ID          INTEGER,
  DESCRIBE          VARCHAR(20),
  PIN_KEY_INDEX      INTEGER,
  MAC_KEY_INDEX      INTEGER,
  FSK_TELE_NUM       INTEGER,
  FSK_TELE_NO1       VARCHAR(15),
  FSK_TELE_NO2       VARCHAR(15),
  FSK_TELE_NO3       VARCHAR(15),
  FSK_DOWN_TELE_NUM  INTEGER,
  FSK_DOWN_TELE_NO1  VARCHAR(15),
  FSK_DOWN_TELE_NO2  VARCHAR(15),
  FSK_DOWN_TELE_NO3  VARCHAR(15),
  HDLC_TELE_NUM      INTEGER,
  HDLC_TELE_NO1      VARCHAR(15),
  HDLC_TELE_NO2      VARCHAR(15),
  HDLC_TELE_NO3      VARCHAR(15),
  HDLC_DOWN_TELE_NUM INTEGER,
  HDLC_DOWN_TELE_NO1 VARCHAR(15),
  HDLC_DOWN_TELE_NO2 VARCHAR(15),
  HDLC_DOWN_TELE_NO3 VARCHAR(15),
  FSKBAK_TELE_NUM    INTEGER,
  FSKBAK_TELE_NO1    VARCHAR(15),
  FSKBAK_TELE_NO2    VARCHAR(15),
  FSKBAK_TELE_NO3    VARCHAR(15),
  HDLCBAK_TELE_NUM   INTEGER,
  HDLCBAK_TELE_NO1   VARCHAR(15),
  HDLCBAK_TELE_NO2   VARCHAR(15),
  HDLCBAK_TELE_NO3   VARCHAR(15)
)
;
comment on table PSAM_PARA
  is '参数模板表';
comment on column PSAM_PARA.MODULE_ID
  is '参数模板编号';
comment on column PSAM_PARA.DESCRIBE
  is '参数模板说明';
comment on column PSAM_PARA.PIN_KEY_INDEX
  is 'Pin密钥索引';
comment on column PSAM_PARA.MAC_KEY_INDEX
  is 'Mac密钥索引';
comment on column PSAM_PARA.FSK_TELE_NUM
  is 'fsk电话号码数';
comment on column PSAM_PARA.FSK_TELE_NO1
  is 'fsk电话号码1';
comment on column PSAM_PARA.FSK_TELE_NO2
  is 'fsk电话号码2';
comment on column PSAM_PARA.FSK_TELE_NO3
  is 'fsk电话号码3';
comment on column PSAM_PARA.FSK_DOWN_TELE_NUM
  is 'fsk下载电话数';
comment on column PSAM_PARA.FSK_DOWN_TELE_NO1
  is 'fsk下载电话1';
comment on column PSAM_PARA.FSK_DOWN_TELE_NO2
  is 'fsk下载电话2';
comment on column PSAM_PARA.FSK_DOWN_TELE_NO3
  is 'fsk下载电话3';
comment on column PSAM_PARA.HDLC_TELE_NUM
  is 'HDLC电话数';
comment on column PSAM_PARA.HDLC_TELE_NO1
  is 'HDLC电话号码1';
comment on column PSAM_PARA.HDLC_TELE_NO2
  is 'HDLC电话号码2';
comment on column PSAM_PARA.HDLC_TELE_NO3
  is 'HDLC电话号码3';
comment on column PSAM_PARA.HDLC_DOWN_TELE_NUM
  is 'HDLC下载电话数';
comment on column PSAM_PARA.HDLC_DOWN_TELE_NO1
  is 'HDLC下载电话号码1';
comment on column PSAM_PARA.HDLC_DOWN_TELE_NO2
  is 'HDLC下载电话号码2';
comment on column PSAM_PARA.HDLC_DOWN_TELE_NO3
  is 'HDLC下载电话号码3';
comment on column PSAM_PARA.FSKBAK_TELE_NUM
  is 'fsk备用电话数';
comment on column PSAM_PARA.FSKBAK_TELE_NO1
  is 'fsk备用电话号码1';
comment on column PSAM_PARA.FSKBAK_TELE_NO2
  is 'fsk备用电话号码2';
comment on column PSAM_PARA.FSKBAK_TELE_NO3
  is 'fsk备用电话号码3';
comment on column PSAM_PARA.HDLCBAK_TELE_NUM
  is 'HDLC备用电话数';
comment on column PSAM_PARA.HDLCBAK_TELE_NO1
  is 'HDLC备用电话号码1';
comment on column PSAM_PARA.HDLCBAK_TELE_NO2
  is 'HDLC备用电话号码2';
comment on column PSAM_PARA.HDLCBAK_TELE_NO3
  is 'HDLC备用电话号码3';
create unique index PSAM_PARA_IDX on PSAM_PARA (MODULE_ID);

drop table QUERY_CONDITION;
create table QUERY_CONDITION
(
  PSAM_NO    CHAR(16),
  PAN        VARCHAR(19),
  BEGIN_DATE CHAR(8),
  END_DATE   CHAR(8),
  SHOP_NO    CHAR(16),
  POS_NO     CHAR(16),
  AMOUNT     NUMERIC(12,2)
)
;
comment on table QUERY_CONDITION
  is '查询条件缓存表';
comment on column QUERY_CONDITION.PSAM_NO
  is '安全模块号';
comment on column QUERY_CONDITION.PAN
  is '卡号';
comment on column QUERY_CONDITION.BEGIN_DATE
  is '起始日期';
comment on column QUERY_CONDITION.END_DATE
  is '截至日期';
comment on column QUERY_CONDITION.SHOP_NO
  is '商户号';
comment on column QUERY_CONDITION.POS_NO
  is '终端号';
comment on column QUERY_CONDITION.AMOUNT
  is '金额';
create unique index QUERY_CONDITION_IDX on QUERY_CONDITION (PSAM_NO);

drop table QUERY_RESULT;
create table QUERY_RESULT
(
  PSAM_NO      CHAR(16),
  POS_TRACE   INTEGER,
  RESULT VARCHAR(1024)
)
;
comment on table QUERY_RESULT
  is '查询结果缓存表';
comment on column QUERY_RESULT.PSAM_NO
  is '安全模块号';
comment on column QUERY_RESULT.POS_TRACE
  is 'POS流水号';
comment on column QUERY_RESULT.RESULT
  is '查询结果';
create unique index QUERY_RESULT_IDX on QUERY_RESULT (PSAM_NO, POS_TRACE);

drop table REAL_ROLE;
create table REAL_ROLE
(
  REAL_ROLE_ID INTEGER not null,
  ROLE_NAME  VARCHAR(20),
  DEPT_NO    VARCHAR(15)
)
;
comment on table REAL_ROLE
  is '实际角色表';
comment on column REAL_ROLE.REAL_ROLE_ID
  is '角色ID号';
comment on column REAL_ROLE.ROLE_NAME
  is '角色名称';
comment on column REAL_ROLE.DEPT_NO
  is '角色所属机构';
alter table REAL_ROLE
  add constraint PK_REAL_ROLE primary key (REAL_ROLE_ID);

drop table REAL_SUB;
create table REAL_SUB
(
  REAL_ROLE_ID INTEGER not null,
  SUB_ROLE_ID  INTEGER not null,
  DEPT_NO    VARCHAR(15)
)
;
comment on table REAL_SUB
  is '实际角色子角色关联表';
comment on column REAL_SUB.REAL_ROLE_ID
  is '实际角色ID号';
comment on column REAL_SUB.SUB_ROLE_ID
  is '子角色ID号';
comment on column REAL_SUB.DEPT_NO
  is '角色所属机构';
alter table REAL_SUB
  add constraint PK_REAL_SUB primary key (REAL_ROLE_ID, SUB_ROLE_ID);
create index REAL_SUB_IDX1 on REAL_SUB (SUB_ROLE_ID);
create index REAL_SUB_IDX2 on REAL_SUB (REAL_ROLE_ID);

drop table REGISTER_CARD;
create table REGISTER_CARD
(
  SHOP_NO       CHAR(15),
  POS_NO        CHAR(15),
  PAN           VARCHAR(19),
  ACCT_NAME     VARCHAR(40),
  EXPIRE_DATE   CHAR(4),
  REGISTER_DATE CHAR(8),
  REGISTER_TIME CHAR(6),
  BANK_NAME     VARCHAR(20),
  TRANSTYPE     CHAR(1),
  OPER_NO       VARCHAR(16),
  STATUS        INTEGER default 0
)
;
comment on table REGISTER_CARD
  is '卡绑定表';
comment on column REGISTER_CARD.SHOP_NO
  is '商户号';
comment on column REGISTER_CARD.POS_NO
  is '终端号';
comment on column REGISTER_CARD.PAN
  is '卡号';
comment on column REGISTER_CARD.ACCT_NAME
  is '用户名';
comment on column REGISTER_CARD.EXPIRE_DATE
  is '卡有效期';
comment on column REGISTER_CARD.REGISTER_DATE
  is '绑定日期';
comment on column REGISTER_CARD.REGISTER_TIME
  is '绑定时间';
comment on column REGISTER_CARD.BANK_NAME
  is '银行名称';
comment on column REGISTER_CARD.TRANSTYPE
  is '交易类型';
comment on column REGISTER_CARD.OPER_NO
  is '操作员号';
comment on column REGISTER_CARD.STATUS
  is '是否启用 0-未启用 1-启用';
create unique index REGISTER_CARD_IDX on REGISTER_CARD (SHOP_NO, POS_NO, TRANSTYPE);

drop table RET_DESC;
create table RET_DESC
(
  TRANS_TYPE   INTEGER not null,
  FIELD_NAME   VARCHAR(80),
  FIELD_FORMAT VARCHAR(150),
  STATUS       CHAR(1) default '1' not null
)
;
comment on table RET_DESC
  is '返回信息配置表';
comment on column RET_DESC.TRANS_TYPE
  is '交易类型';
comment on column RET_DESC.FIELD_NAME
  is '要显示的app字段,以封号隔开';
comment on column RET_DESC.FIELD_FORMAT
  is '组装FIELD_NAME的格式,对应app字段以%s表示';
comment on column RET_DESC.STATUS
  is '状态位,1-有效，0-无效';
create unique index RET_DESC_IDX on RET_DESC (TRANS_TYPE);

drop table SHOP;
create table SHOP
(
  SHOP_NO            CHAR(15) not null,
  MARKET_NO          INTEGER default 0,
  SHOP_NAME          VARCHAR(40),
  ACQ_BANK           CHAR(11),
  CONTACTOR          VARCHAR(10),
  TELEPHONE          VARCHAR(25),
  ADDR               VARCHAR(47),
  FEE                INTEGER,
  FAX_NUM            VARCHAR(25),
  SIGN_FLAG          INTEGER,
  SIGN_DATE          CHAR(8),
  UNSIGN_DATE        CHAR(8),
  DEPT_NO            CHAR(15),
  ACQ_BANK_CODE      VARCHAR(11),
  PROCEDURE_FEE      VARCHAR(15),
  BALANCE_DAY        VARCHAR(10),
  PROCEDURE_DAY      VARCHAR(10),
  LAWPER_NAME        VARCHAR(30),
  LAWPER_CERTIFICATE VARCHAR(30),
  REGISTER_CODE      VARCHAR(30),
  BUS_AREA           VARCHAR(40),
  FORM_CODE          VARCHAR(30),
  TAX_CODE           VARCHAR(30),
  ACQ_BANK_NAME      VARCHAR(60),
  IS_BLACK           CHAR(1),
  ACQ_NAME           VARCHAR(30),
  PAN_TYPE           CHAR(1),
  ACQ_PLACE          VARCHAR(30),
  ACQ_CITY           VARCHAR(30),
  ACQ_DEAFBANK_NAME  VARCHAR(50),
  ACQ_MANMOBILE      CHAR(11),
  ADD_AMOUNT         NUMERIC(12,2) default 0.00,
  DEPT_DETAIL        VARCHAR(70),
  MCC_CODE           CHAR(4) default '0000' 
)
;
comment on table SHOP
  is '商户资料表';
comment on column SHOP.SHOP_NO
  is '商户号';
comment on column SHOP.MARKET_NO
  is '市场类型';
comment on column SHOP.SHOP_NAME
  is '商户名称';
comment on column SHOP.ACQ_BANK
  is '收单行';
comment on column SHOP.CONTACTOR
  is '联系人';
comment on column SHOP.TELEPHONE
  is '电话';
comment on column SHOP.ADDR
  is '地址';
comment on column SHOP.FEE
  is '手续费折扣率';
comment on column SHOP.FAX_NUM
  is '传真号码';
comment on column SHOP.SIGN_FLAG
  is '签约标志';
comment on column SHOP.SIGN_DATE
  is '签约日期';
comment on column SHOP.UNSIGN_DATE
  is '撤销日期';
comment on column SHOP.DEPT_NO
  is '撤销日期';
comment on column SHOP.ACQ_BANK_CODE
  is '开户行账号';
comment on column SHOP.PROCEDURE_FEE
  is '手续费费率';
comment on column SHOP.BALANCE_DAY
  is '资金结算周期';
comment on column SHOP.PROCEDURE_DAY
  is ' 手续费结算周期';
comment on column SHOP.LAWPER_NAME
  is '法人姓名';
comment on column SHOP.LAWPER_CERTIFICATE
  is '法人证件号码';
comment on column SHOP.REGISTER_CODE
  is '	营业执照注册编号';
comment on column SHOP.BUS_AREA
  is ' 	经营范围';
comment on column SHOP.FORM_CODE
  is ' 	组织机构代码证';
comment on column SHOP.TAX_CODE
  is ' 	税务登记证号码';
comment on column SHOP.ACQ_BANK_NAME
  is '收单行名称';
comment on column SHOP.IS_BLACK
  is '是否黑名单商户';
comment on column SHOP.ACQ_NAME
  is '收款人姓名';
comment on column SHOP.PAN_TYPE
  is '账户类型(0-个人，1-企业)';
comment on column SHOP.ACQ_PLACE
  is '开户地区(列表选择、省份或直辖市)';
comment on column SHOP.ACQ_CITY
  is '开户城市(30Bytes、手工输入)';
comment on column SHOP.ACQ_DEAFBANK_NAME
  is '支行名称（50Bytes）、';
comment on column SHOP.ACQ_MANMOBILE
  is '收款人手机号(11Bytes)';
comment on column SHOP.ADD_AMOUNT
  is '挂帐金额【单位元、增加商户资料时挂帐金额为0(不允许修改)】';
comment on column SHOP.DEPT_DETAIL
  is '机构层级信息';
comment on column SHOP.MCC_CODE
  is '商户类型，对应MCC码';
create unique index SHOP_IDX on SHOP (SHOP_NO);

drop table SHOP_TYPE;
create table SHOP_TYPE
(
  MCC_CODE CHAR(4),
  DESCRIBE VARCHAR(40)
)
;
comment on table SHOP_TYPE
  is '商户类型表';
comment on column SHOP_TYPE.MCC_CODE
  is 'MCC码';
comment on column SHOP_TYPE.DESCRIBE
  is 'MCC说明';

drop table SHORT_MESSAGE;
create table SHORT_MESSAGE
(
  REC_NO       INTEGER,
  MSG_TITLE   VARCHAR(30),
  MSG_CONTENT VARCHAR(140),
  VALID_DATE  CHAR(8)
)
;
comment on table SHORT_MESSAGE
  is '短信管理表';
comment on column SHORT_MESSAGE.REC_NO
  is '短信记录号';
comment on column SHORT_MESSAGE.MSG_TITLE
  is '短信标题';
comment on column SHORT_MESSAGE.MSG_CONTENT
  is '短信内容';
comment on column SHORT_MESSAGE.VALID_DATE
  is '有效截止日期';
create unique index SHORT_MESSAGE_IDX on SHORT_MESSAGE (REC_NO);

drop table STATIC_MENU_CFG;
create table STATIC_MENU_CFG
(
  STATIC_MENU_ID    INTEGER not null,
  DOWN_STATIC_MENU  CHAR(1) default 'N',
  STATIC_MENU_RECNO INTEGER default 0,
  PSAM_NO           CHAR(16)
)
;
comment on table STATIC_MENU_CFG
  is '静态菜单配置表';
comment on column STATIC_MENU_CFG.STATIC_MENU_ID
  is '静态菜单ID';
comment on column STATIC_MENU_CFG.STATIC_MENU_RECNO
  is '目前下载的菜单序号';
comment on column STATIC_MENU_CFG.PSAM_NO
  is '安全模块号';

drop table STAT_LINE;
create table STAT_LINE
(
  SHOP_NO        CHAR(15) not null,
  POS_NO         CHAR(15) not null,
  TRANS_DATE     VARCHAR(8) not null,
  MARKET_NO      INTEGER default 0,
  BIND_CARD_NO   VARCHAR(19),
  PUR_COUNT      INTEGER,
  PUR_AMOUNT     NUMERIC(12,2),
  PUR_FEE        NUMERIC(12,2),
  PAY_IN_COUNT   INTEGER,
  PAY_IN_AMOUNT  NUMERIC(12,2),
  PAY_IN_FEE     NUMERIC(12,2),
  PAY_OUT_COUNT  INTEGER,
  PAY_OUT_AMOUNT NUMERIC(12,2),
  PAY_OUT_FEE    NUMERIC(12,2),
  TYPE_1_COUNT   INTEGER,
  TYPE_1_AMOUNT  NUMERIC(12,2),
  TYPE_1_FEE     NUMERIC(12,2),
  TYPE_2_COUNT   INTEGER,
  TYPE_2_AMOUNT  NUMERIC(12,2),
  TYPE_2_FEE     NUMERIC(12,2),
  TYPE_3_COUNT   INTEGER,
  TYPE_3_AMOUNT  NUMERIC(12,2),
  TYPE_3_FEE     NUMERIC(12,2),
  RESERVE_1      VARCHAR(50),
  DEPT_DETAIL    VARCHAR(70),
  DEPT_NAME      VARCHAR(45),
  SHOP_NAME      VARCHAR(40),
  MARKET_NAME    VARCHAR(40),
  DEL_FLAG       INTEGER default 0
)
;
comment on table STAT_LINE
  is '交易汇总表';
comment on column STAT_LINE.SHOP_NO
  is '商户号';
comment on column STAT_LINE.POS_NO
  is '终端号';
comment on column STAT_LINE.TRANS_DATE
  is '交易日期';
comment on column STAT_LINE.MARKET_NO
  is '所属市场';
comment on column STAT_LINE.BIND_CARD_NO
  is '绑定卡号 ';
comment on column STAT_LINE.PUR_COUNT
  is '消费笔数';
comment on column STAT_LINE.PUR_AMOUNT
  is '消费金额';
comment on column STAT_LINE.PUR_FEE
  is '消费手续费';
comment on column STAT_LINE.PAY_IN_COUNT
  is '收款笔数';
comment on column STAT_LINE.PAY_IN_AMOUNT
  is '收款金额';
comment on column STAT_LINE.PAY_IN_FEE
  is '收款手续费';
comment on column STAT_LINE.PAY_OUT_COUNT
  is '付款笔数';
comment on column STAT_LINE.PAY_OUT_AMOUNT
  is '付款金额';
comment on column STAT_LINE.PAY_OUT_FEE
  is '付款手续费';
comment on column STAT_LINE.TYPE_1_COUNT
  is '预留交易类型交易笔数';
comment on column STAT_LINE.TYPE_1_AMOUNT
  is '预留交易类型交易金额';
comment on column STAT_LINE.TYPE_1_FEE
  is '预留交易类型交易手续费';
comment on column STAT_LINE.TYPE_2_COUNT
  is '预留交易类型交易笔数';
comment on column STAT_LINE.TYPE_2_AMOUNT
  is '预留交易类型交易金额';
comment on column STAT_LINE.TYPE_2_FEE
  is '预留交易类型交易手续费';
comment on column STAT_LINE.TYPE_3_COUNT
  is '预留交易类型交易笔数';
comment on column STAT_LINE.TYPE_3_AMOUNT
  is '预留交易类型交易金额';
comment on column STAT_LINE.TYPE_3_FEE
  is '预留交易类型交易手续费';
comment on column STAT_LINE.RESERVE_1
  is '预留字段';
comment on column STAT_LINE.DEPT_DETAIL
  is '机构层级信息';
comment on column STAT_LINE.DEPT_NAME
  is '机构名称';
comment on column STAT_LINE.SHOP_NAME
  is '商户名称';
comment on column STAT_LINE.MARKET_NAME
  is '市场名称';
comment on column STAT_LINE.DEL_FLAG
  is '终端删除标识 0-终端未删除 1-终端已删除';
alter table STAT_LINE
  add constraint PK_T_STAT_LINE primary key (SHOP_NO, POS_NO, TRANS_DATE);
create index STAT_LINE_IDX1 on STAT_LINE (DEPT_DETAIL);

drop table SUB_ROLE;
create table SUB_ROLE
(
  SUB_ROLE_ID   INTEGER not null,
  SYS_ID       VARCHAR(10),
  SUB_ROLE_NAME VARCHAR(20),
  DEPT_NO     VARCHAR(15)
)
;
comment on table SUB_ROLE
  is '子系统角色表';
comment on column SUB_ROLE.SUB_ROLE_ID
  is '子角色ID';
comment on column SUB_ROLE.SYS_ID
  is '子系统ID';
comment on column SUB_ROLE.SUB_ROLE_NAME
  is '角色名称';
comment on column SUB_ROLE.DEPT_NO
  is '机构号';
alter table SUB_ROLE
  add constraint PK_SUB_ROLE primary key (SUB_ROLE_ID);
create index SUB_ROLE_IDX on SUB_ROLE (SYS_ID);

drop table SUB_ROLE_FUNC;
create table SUB_ROLE_FUNC
(
  MODULE_ID  INTEGER not null,
  SUB_ROLE_ID INTEGER not null,
  BUTTON_MAP VARCHAR(20),
  DEPT_NO   VARCHAR(15)
)
;
comment on table SUB_ROLE_FUNC
  is '子角色功能表';
comment on column SUB_ROLE_FUNC.MODULE_ID
  is '模块ID';
comment on column SUB_ROLE_FUNC.SUB_ROLE_ID
  is '子角色ID';
comment on column SUB_ROLE_FUNC.BUTTON_MAP
  is '功能位图，用于该角色是否具备增、删、改、查权限';
comment on column SUB_ROLE_FUNC.DEPT_NO
  is '所属机构';
alter table SUB_ROLE_FUNC
  add constraint PK_SUB_ROLE_FUNC primary key (MODULE_ID, SUB_ROLE_ID);
create index SUB_ROLE_FUNC_IDX1	on SUB_ROLE_FUNC (MODULE_ID);
create index SUB_ROLE_FUNC_IDX2	on SUB_ROLE_FUNC (SUB_ROLE_ID);

drop table SUB_SYS;
create table SUB_SYS
(
  SYS_ID   VARCHAR(10) not null,
  SYS_NAME VARCHAR(20)
)
;
comment on table SUB_SYS
  is '子系统管理表';
comment on column SUB_SYS.SYS_ID
  is '系统ID';
comment on column SUB_SYS.SYS_NAME
  is '系统名称';
alter table SUB_SYS
  add constraint PK_SUBSYS primary key (SYS_ID);

drop table SYSTEM_PARAMETER;
create table SYSTEM_PARAMETER
(
  CUR_KEY_INDEX  INTEGER,
  BATCH_NO       INTEGER,
  SYS_TRACE     INTEGER not null
)
;
comment on table SYSTEM_PARAMETER
  is '系统参数表';
comment on column SYSTEM_PARAMETER.CUR_KEY_INDEX
  is '当前密钥索引号';
comment on column SYSTEM_PARAMETER.BATCH_NO
  is '当前批次号';
comment on column SYSTEM_PARAMETER.SYS_TRACE
  is '当前系统流水号';

drop table TCPCOM_PID;
create table TCPCOM_PID
(
  IP      CHAR(15),
  PID     INTEGER,
  HOST_NO INTEGER
)
;
comment on table TCPCOM_PID
  is '进程登记表';
comment on column TCPCOM_PID.IP
  is 'IP地址';
comment on column TCPCOM_PID.PID
  is '进程号';
comment on column TCPCOM_PID.HOST_NO
  is '服务器编号';

drop table TERMINAL;
create table TERMINAL
(
  SHOP_NO         CHAR(15) not null,
  POS_NO          CHAR(15) not null,
  PSAM_NO         CHAR(16) not null,
  TELEPHONE       VARCHAR(15),
  TERM_MODULE     INTEGER,
  PSAM_MODULE     INTEGER,
  APP_TYPE        INTEGER,
  DESCRIBE        VARCHAR(20),
  POS_TYPE        VARCHAR(40),
  ADDRESS         VARCHAR(40),
  PUT_DATE        CHAR(8),
  CUR_TRACE       INTEGER default 1,
  IP              CHAR(15),
  PORT            INTEGER,
  DOWN_MENU       CHAR(1),
  DOWN_TERM       CHAR(1),
  DOWN_PSAM       CHAR(1),
  DOWN_PRINT      CHAR(1),
  DOWN_OPERATE    CHAR(1),
  DOWN_FUNCTION   CHAR(1),
  DOWN_ERROR      CHAR(1),
  DOWN_ALL        CHAR(1),
  DOWN_PAYLIST    CHAR(1),
  MENU_RECNO      INTEGER default 0,
  PRINT_RECNO     INTEGER default 0,
  OPERATE_RECNO   INTEGER default 0,
  FUNCTION_RECNO  INTEGER default 0,
  ERROR_RECNO     INTEGER default 0,
  ALL_TRANSTYPE   INTEGER default 3,
  TERM_BITMAP     CHAR(8),
  PSAM_BITMAP     CHAR(8),
  PRINT_BITMAP    CHAR(64),
  OPERATE_BITMAP  CHAR(64),
  FUNCTION_BITMAP CHAR(64),
  ERROR_BITMAP    CHAR(64),
  MSG_RECNUM      INTEGER default 0,
  MSG_RECNO       VARCHAR(256),
  FIRST_PAGE      INTEGER default 0,
  STATUS          INTEGER default 0,
  CUR_BATCH       INTEGER default 1
)
;
comment on table TERMINAL
  is '终端表';
comment on column TERMINAL.SHOP_NO
  is '商户号';
comment on column TERMINAL.POS_NO
  is '终端号';
comment on column TERMINAL.PSAM_NO
  is '安全模块号';
comment on column TERMINAL.TELEPHONE
  is '绑定电话，00000000表示不进行电话号码绑定检查';
comment on column TERMINAL.TERM_MODULE
  is '终端模板';
comment on column TERMINAL.PSAM_MODULE
  is '安全参数模板';
comment on column TERMINAL.APP_TYPE
  is '应用类型';
comment on column TERMINAL.DESCRIBE
  is '说明、描述';
comment on column TERMINAL.POS_TYPE
  is '终端型号';
comment on column TERMINAL.ADDRESS
  is '安装地址';
comment on column TERMINAL.PUT_DATE
  is '安装日期';
comment on column TERMINAL.CUR_TRACE
  is '当前流水号';
comment on column TERMINAL.IP
  is '请求方IP';
comment on column TERMINAL.PORT
  is '服务端口';
comment on column TERMINAL.DOWN_MENU
  is '是否需要下载菜单';
comment on column TERMINAL.DOWN_TERM
  is '是否需要下载终端参数';
comment on column TERMINAL.DOWN_PSAM
  is '是否需要下载安全参数';
comment on column TERMINAL.DOWN_PRINT
  is '是否需要下载打印模板';
comment on column TERMINAL.DOWN_OPERATE
  is '是否需要下载操作提示';
comment on column TERMINAL.DOWN_FUNCTION
  is '是否需要下载功能提示';
comment on column TERMINAL.DOWN_ERROR
  is '是否需要下载错误提示';
comment on column TERMINAL.DOWN_ALL
  is '是否需要下载全部内容';
comment on column TERMINAL.DOWN_PAYLIST
  is '是否需要下载账单';
comment on column TERMINAL.MENU_RECNO
  is '已下载菜单记录号';
comment on column TERMINAL.PRINT_RECNO
  is '已下载打印模板记录号';
comment on column TERMINAL.OPERATE_RECNO
  is '已下载操作提示记录号';
comment on column TERMINAL.FUNCTION_RECNO
  is '已下载功能提示记录号';
comment on column TERMINAL.ERROR_RECNO
  is '已下载错误提示记录号';
comment on column TERMINAL.ALL_TRANSTYPE
  is '当前下载功能';
comment on column TERMINAL.TERM_BITMAP
  is '终端参数下载位图';
comment on column TERMINAL.PSAM_BITMAP
  is '安全参数下载位图';
comment on column TERMINAL.PRINT_BITMAP
  is '打印位图';
comment on column TERMINAL.OPERATE_BITMAP
  is '操作提示位图';
comment on column TERMINAL.FUNCTION_BITMAP
  is '功能提示位图';
comment on column TERMINAL.ERROR_BITMAP
  is '错误提示位图';
comment on column TERMINAL.MSG_RECNUM
  is '需要下载短信记录数';
comment on column TERMINAL.MSG_RECNO
  is '需要下载短信记录号序号';
comment on column TERMINAL.FIRST_PAGE
  is '需要下载首页信息记录号';
comment on column TERMINAL.STATUS
  is '状态,0停用,1-启用';
comment on column TERMINAL.CUR_BATCH
  is '当前批次号';
create index TERMINAL_IDX on TERMINAL (PSAM_NO);

drop table TERMINAL_OPER;
create table TERMINAL_OPER
(
  SHOP_NO      CHAR(15),
  POS_NO       CHAR(15),
  OPER_NO      CHAR(4),
  OPER_PWD     CHAR(6),
  OPER_NAME    CHAR(20),
  DEL_FLAG     INTEGER,
  LOGIN_STATUS INTEGER
)
;
comment on table TERMINAL_OPER
  is '终端操作员表';
comment on column TERMINAL_OPER.SHOP_NO
  is '商户号';
comment on column TERMINAL_OPER.POS_NO
  is '终端号';
comment on column TERMINAL_OPER.OPER_NO
  is '操作员号';
comment on column TERMINAL_OPER.OPER_PWD
  is '操作员密码';
comment on column TERMINAL_OPER.OPER_NAME
  is '操作员姓名';
comment on column TERMINAL_OPER.DEL_FLAG
  is '删除标识  1-已删除 0-未删除';
comment on column TERMINAL_OPER.LOGIN_STATUS
  is '签到状态  1-签到  0-签退';

drop table TERMINAL_PARA;
create table TERMINAL_PARA
(
  MODULE_ID     INTEGER,
  DESCRIBE      VARCHAR(20),
  LINE_TYPE     CHAR(1),
  INPUT_TIMEOUT INTEGER,
  TRANS_TIMEOUT INTEGER,
  MANAGER_PWD   CHAR(8),
  OPERATOR_PWD  CHAR(6),
  TELEPHONE_NO  INTEGER,
  PIN_MAX_LEN   INTEGER,
  AUTH_KEY      CHAR(8),
  INTER_KEY     CHAR(32),
  EXT_KEY       CHAR(32),
  PRE_TELE_NO   VARCHAR(12),
  WAIT_TIME     INTEGER,
  TIP_SWITCH    CHAR(1),
  AUTO_ANSWER   CHAR(1),
  DELAY_TIME    INTEGER,
  HAND_DIAL     CHAR(1),
  SAVE_LIST     CHAR(1),
  PRINT_OR_NOT  CHAR(1),
  READER        CHAR(1),
  PIN_INPUT     CHAR(1)
)
;
comment on table TERMINAL_PARA
  is '终端参数模板表';
comment on column TERMINAL_PARA.MODULE_ID
  is '参数模板编号';
comment on column TERMINAL_PARA.DESCRIBE
  is '参数模板说明';
comment on column TERMINAL_PARA.LINE_TYPE
  is '接入模式 0-FSK接入 1-HDLC接入 2-DTMF接入 3-拨号接入';
comment on column TERMINAL_PARA.INPUT_TIMEOUT
  is '控制超时时限，单位秒';
comment on column TERMINAL_PARA.TRANS_TIMEOUT
  is '交易超时时限，单位秒';
comment on column TERMINAL_PARA.MANAGER_PWD
  is '终端管理密码';
comment on column TERMINAL_PARA.OPERATOR_PWD
  is '终端操作密码';
comment on column TERMINAL_PARA.TELEPHONE_NO
  is '缺省系统号码序号';
comment on column TERMINAL_PARA.PIN_MAX_LEN
  is '密码最大长度';
comment on column TERMINAL_PARA.AUTH_KEY
  is '接入认证密钥，ASC码';
comment on column TERMINAL_PARA.INTER_KEY
  is '内部认证密钥，ASC码';
comment on column TERMINAL_PARA.EXT_KEY
  is '外部认证密钥，ASC码';
comment on column TERMINAL_PARA.PRE_TELE_NO
  is '预拨外线号码';
comment on column TERMINAL_PARA.WAIT_TIME
  is '拨号等待时间';
comment on column TERMINAL_PARA.TIP_SWITCH
  is '交易提示音开关 0－关闭  1-打开';
comment on column TERMINAL_PARA.AUTO_ANSWER
  is '来电自动应答 0－关闭  1-打开';
comment on column TERMINAL_PARA.DELAY_TIME
  is '交换机时延等级 1-200毫秒 2-400毫秒 3-600毫妙 4-800毫秒 5-1000毫秒';
comment on column TERMINAL_PARA.HAND_DIAL
  is '终端通话屏蔽 0－不屏蔽  1-屏蔽';
comment on column TERMINAL_PARA.SAVE_LIST
  is '保存支付账单 0-否  1-是';
comment on column TERMINAL_PARA.PRINT_OR_NOT
  is '是否打印交易凭条 0-否  1-是';
comment on column TERMINAL_PARA.READER
  is '支持条码阅读 0-否  1-是';
comment on column TERMINAL_PARA.PIN_INPUT
  is '密码输入方式 0-内置密码键盘  1-外接密码键盘';
create unique index TERMINAL_PARA_IDX on TERMINAL_PARA (MODULE_ID);

drop table TM_APP_FILE_INFO;
create table TM_APP_FILE_INFO
(
  APP_NO     VARCHAR(50) not null,
  MODEL_NO   VARCHAR(40) not null,
  APP_VER    VARCHAR(40) not null,
  ISSUE_DATE VARCHAR(20)
)
;
comment on table TM_APP_FILE_INFO
  is '应用文件信息表';
comment on column TM_APP_FILE_INFO.APP_NO
  is '应用编号';
comment on column TM_APP_FILE_INFO.MODEL_NO
  is '终端型号';
comment on column TM_APP_FILE_INFO.APP_VER
  is '应用版本';
comment on column TM_APP_FILE_INFO.ISSUE_DATE
  is '程序上传日期';
alter table TM_APP_FILE_INFO
  add constraint PK_TM_APP_FILE primary key (APP_NO, MODEL_NO, APP_VER);

drop table TM_APP_VER_INFO;
create table TM_APP_VER_INFO
(
  APP_NO      VARCHAR(50) not null,
  APP_VER     VARCHAR(40) not null,
  REMARK     VARCHAR(40),
  CREATE_DATE VARCHAR(20),
  DEPT_NO     VARCHAR(15)
)
;
comment on table TM_APP_VER_INFO
  is '应用版本信息';
comment on column TM_APP_VER_INFO.APP_NO
  is '应用编号';
comment on column TM_APP_VER_INFO.APP_VER
  is '应用版本';
comment on column TM_APP_VER_INFO.REMARK
  is '应用描述';
comment on column TM_APP_VER_INFO.CREATE_DATE
  is '创建时间';
comment on column TM_APP_VER_INFO.DEPT_NO
  is '机构';
alter table TM_APP_VER_INFO
  add constraint PK_TM_APP_VER primary key (APP_NO, APP_VER);

drop table TM_APP_INFO;
create table TM_APP_INFO
(
  APP_NO     VARCHAR(50) not null,
  APP_NAME   VARCHAR(80) not null,
  SHARE_FLAG CHAR(1),
  ISSUE_DATE VARCHAR(20),
  STATUS    VARCHAR(20),
  DEPT_NO   VARCHAR(15)
)
;
comment on table TM_APP_INFO
  is '应用信息';
comment on column TM_APP_INFO.APP_NO
  is '应用编号';
comment on column TM_APP_INFO.APP_NAME
  is '应用名称';
comment on column TM_APP_INFO.SHARE_FLAG
  is '共享标志';
comment on column TM_APP_INFO.ISSUE_DATE
  is '发布日期';
comment on column TM_APP_INFO.STATUS
  is '状态';
comment on column TM_APP_INFO.DEPT_NO
  is '机构';
alter table TM_APP_INFO
  add constraint PK_TM_APP_INFO primary key (APP_NO);

drop table TM_FACTORY_INFO;
create table TM_FACTORY_INFO
(
  FAC_NO      VARCHAR(20) not null,
  FAC_NAME    VARCHAR(40),
  CONTACT_MAN VARCHAR(50),
  ADDRESS    VARCHAR(80),
  TELE       VARCHAR(50),
  FAX        VARCHAR(50),
  POST_CODE   VARCHAR(50)
)
;
comment on table TM_FACTORY_INFO
  is '厂商信息';
comment on column TM_FACTORY_INFO.FAC_NO
  is '厂商编号';
comment on column TM_FACTORY_INFO.FAC_NAME
  is '厂商名称';
comment on column TM_FACTORY_INFO.CONTACT_MAN
  is '联系人';
comment on column TM_FACTORY_INFO.ADDRESS
  is '法定地址';
comment on column TM_FACTORY_INFO.TELE
  is '联系电话';
comment on column TM_FACTORY_INFO.FAX
  is '传真';
comment on column TM_FACTORY_INFO.POST_CODE
  is '邮政编码';
alter table TM_FACTORY_INFO
  add constraint PK_TM_FACTORY primary key (FAC_NO);

drop table TM_MODEL_INFO;
create table TM_MODEL_INFO
(
  MODEL_NO    VARCHAR(20) not null,
  MODEL_NAME  VARCHAR(30),
  PLUG_IN_NAME VARCHAR(100),
  FAC_NO      VARCHAR(50),
  FOLDER_PATH VARCHAR(255)
)
;
comment on table TM_MODEL_INFO
  is '终端型号信息';
comment on column TM_MODEL_INFO.MODEL_NO
  is '终端型号编号';
comment on column TM_MODEL_INFO.MODEL_NAME
  is '终端型号名称';
comment on column TM_MODEL_INFO.PLUG_IN_NAME
  is '下载插件路径';
comment on column TM_MODEL_INFO.FAC_NO
  is '厂商代码';
comment on column TM_MODEL_INFO.FOLDER_PATH
  is '终端程序目录';
alter table TM_MODEL_INFO
  add constraint PK_TM_MODEL primary key (MODEL_NO);

drop table TM_TRADE_INFO;
create table TM_TRADE_INFO
(
  ID        VARCHAR(20) not null,
  SHOP_NO   VARCHAR(20),
  POS_NO    VARCHAR(20),
  SN        VARCHAR(20),
  TRANS_DATE VARCHAR(20),
  TRANS_TIME VARCHAR(20),
  APP_FLAG   CHAR(1),
  STATUS    VARCHAR(50),
  VPOS_ID    VARCHAR(32)
)
;
comment on table TM_TRADE_INFO
  is '下载流水表';
comment on column TM_TRADE_INFO.ID
  is 'ID';
comment on column TM_TRADE_INFO.SHOP_NO
  is '商户号';
comment on column TM_TRADE_INFO.POS_NO
  is '终端号';
comment on column TM_TRADE_INFO.SN
  is '硬件序列号';
comment on column TM_TRADE_INFO.TRANS_DATE
  is '交易日期';
comment on column TM_TRADE_INFO.TRANS_TIME
  is '交易时间';
comment on column TM_TRADE_INFO.APP_FLAG
  is '应用标志';
comment on column TM_TRADE_INFO.STATUS
  is '状态';
comment on column TM_TRADE_INFO.VPOS_ID
  is '多应用编号';
alter table TM_TRADE_INFO
  add constraint PK_TM_TRADE primary key (ID);

drop table TM_VPOS_APP_INFO;
create table TM_VPOS_APP_INFO
(
  ID         VARCHAR(32) not null,
  APP_NO      VARCHAR(20),
  APP_VER     VARCHAR(20),
  APP_CHANGE  VARCHAR(20),
  PARAM_NO    VARCHAR(20),
  PARA_CHANGE VARCHAR(1),
  VPOS_ID     VARCHAR(32)
)
;
comment on table TM_VPOS_APP_INFO
  is '逻辑终端应用信息';
comment on column TM_VPOS_APP_INFO.ID
  is 'ID';
comment on column TM_VPOS_APP_INFO.APP_NO
  is '应用编号';
comment on column TM_VPOS_APP_INFO.APP_VER
  is '应用版本';
comment on column TM_VPOS_APP_INFO.APP_CHANGE
  is '应用修改标志';
comment on column TM_VPOS_APP_INFO.PARAM_NO
  is '参数模板编号';
comment on column TM_VPOS_APP_INFO.PARA_CHANGE
  is '参数修改标志';
comment on column TM_VPOS_APP_INFO.VPOS_ID
  is '多应用编号';
alter table TM_VPOS_APP_INFO
  add constraint PK_TM_VPOS_APP_INFO primary key (ID);
create unique index TM_VPOS_APP_INFO_IDX on TM_VPOS_APP_INFO (APP_NO, APP_VER, VPOS_ID);

drop table TM_VPOS_INFO;
create table TM_VPOS_INFO
(
  VPOS_ID       VARCHAR(32) not null,
  POS_NO       VARCHAR(20),
  NOTICE_FLAG  CHAR(1),
  NOTICE_BTIME VARCHAR(14),
  NOTICE_EDATE VARCHAR(8),
  DOWN_FLAG     VARCHAR(10),
  SHOP_NO      VARCHAR(20),
  DEPT_NO      VARCHAR(15)
)
;
comment on table TM_VPOS_APP_INFO
  is '逻辑终端信息';
comment on column TM_VPOS_INFO.VPOS_ID
  is 'ID';
comment on column TM_VPOS_INFO.POS_NO
  is '终端号';
comment on column TM_VPOS_INFO.NOTICE_FLAG
  is '通知标志';
comment on column TM_VPOS_INFO.NOTICE_BTIME
  is '通知开始时间';
comment on column TM_VPOS_INFO.NOTICE_EDATE
  is '通知截至日期';
comment on column TM_VPOS_INFO.DOWN_FLAG
  is '下载标志';
comment on column TM_VPOS_INFO.SHOP_NO
  is '商户号';
comment on column TM_VPOS_INFO.DEPT_NO
  is '机构号';
alter table TM_VPOS_INFO
  add constraint PK_TM_VPOS primary key (VPOS_ID);

drop table TRANS_BALANCE;
create table TRANS_BALANCE
(
  COLLATE_DATE   CHAR(8),
  SHOP_NO        CHAR(15),
  POS_NO         CHAR(15),
  TRANS_DATE     CHAR(10),
  TRANS_TIME     CHAR(8),
  AMOUNT         NUMERIC(12,2),
  COST           NUMERIC(12,2),
  PURE_INCOME    NUMERIC(12,2),
  PAN            VARCHAR(19),
  ACCOUNT2       VARCHAR(19) not null,
  RETRI_REF_NUM    CHAR(12),
  OLD_RETRI_REF_NUM CHAR(12),
  BALANCE        VARCHAR(20)
)
;
comment on table TRANS_BALANCE
  is '交易日均余额表';
comment on column TRANS_BALANCE.COLLATE_DATE
  is '收款日期';
comment on column TRANS_BALANCE.SHOP_NO
  is '商户号';
comment on column TRANS_BALANCE.POS_NO
  is '终端号';
comment on column TRANS_BALANCE.TRANS_DATE
  is '交易日期';
comment on column TRANS_BALANCE.TRANS_TIME
  is '交易时间';
comment on column TRANS_BALANCE.AMOUNT
  is '交易金额';
comment on column TRANS_BALANCE.COST
  is '成本';
comment on column TRANS_BALANCE.PURE_INCOME
  is '净收入';
comment on column TRANS_BALANCE.PAN
  is '卡号';
comment on column TRANS_BALANCE.ACCOUNT2
  is '转入卡号';
comment on column TRANS_BALANCE.RETRI_REF_NUM
  is '参考号';
comment on column TRANS_BALANCE.OLD_RETRI_REF_NUM
  is '原参考号';
comment on column TRANS_BALANCE.BALANCE
  is '日均余额';
create unique index BALANCE_IDX on TRANS_BALANCE (SHOP_NO, POS_NO, TRANS_DATE, TRANS_TIME, RETRI_REF_NUM);

drop table TRANS_COMMANDS;
create table TRANS_COMMANDS
(
  TRANS_TYPE    INTEGER,
  STEP          INTEGER,
  TRANS_FLAG    CHAR(1),
  COMMAND       CHAR(2),
  OPER_INDEX    INTEGER,
  ALOG          CHAR(8),
  COMMAND_NAME  VARCHAR(30),
  ORG_COMMAND   CHAR(2),
  CONTROL_LEN   INTEGER default 0,
  CONTROL_PARA  CHAR(60),
  DATA_INDEX    INTEGER
)
;
comment on table TRANS_COMMANDS
  is '交易指令集表';
comment on column TRANS_COMMANDS.TRANS_TYPE
  is '交易代码';
comment on column TRANS_COMMANDS.STEP
  is '处理步骤';
comment on column TRANS_COMMANDS.TRANS_FLAG
  is '标识位 0-之前流程 1-后续流程';
comment on column TRANS_COMMANDS.COMMAND
  is '实际指令';
comment on column TRANS_COMMANDS.OPER_INDEX
  is '操作提示索引';
comment on column TRANS_COMMANDS.ALOG
  is '加密、校验算法';
comment on column TRANS_COMMANDS.COMMAND_NAME
  is '指令名称';
comment on column TRANS_COMMANDS.ORG_COMMAND
  is '原始指令';
comment on column TRANS_COMMANDS.CONTROL_LEN
  is '控制参数长度';
comment on column TRANS_COMMANDS.CONTROL_PARA
  is '控制参数';
comment on column TRANS_COMMANDS.DATA_INDEX
  is '数据索引，指明数据来源';
create unique index TRANS_COMMANDS_IDX on TRANS_COMMANDS (TRANS_TYPE, STEP, TRANS_FLAG);

drop table TRANS_DEF;
create table TRANS_DEF
(
  TRANS_TYPE       INTEGER,
  TRANS_CODE       CHAR(8),
  NEXT_TRANS_CODE  CHAR(8),
  BUSINESS_TYPE    INTEGER default 0,
  EXCEP_HANDLE	   CHAR(1),
  EXCEP_TIMES      INTEGER default 0,
  PIN_BLOCK        CHAR(1) default '1',
  FUNCTION_INDEX   INTEGER,
  TRANS_NAME       VARCHAR(20),
  TELEPHONE_NO     INTEGER default 1,
  DISP_TYPE        CHAR(1),
  TOTRANS_MSG_TYPE INTEGER,
  TOHOST_MSG_TYPE  INTEGER,
  IS_VISIBLE       CHAR(1) default '1'
)
;
comment on table TRANS_DEF
  is '交易定义表';
comment on column TRANS_DEF.TRANS_TYPE
  is '交易类型';
comment on column TRANS_DEF.TRANS_CODE
  is '交易代码';
comment on column TRANS_DEF.NEXT_TRANS_CODE
  is '后续交易代码，无后续交易为空';
comment on column TRANS_DEF.BUSINESS_TYPE
  is '交易业务类型';
comment on column TRANS_DEF.EXCEP_HANDLE
  is '异常处理机制(收应答超时或验证MAC错) 0-不冲正不重发 1-冲正 2-重发';
comment on column TRANS_DEF.EXCEP_TIMES
  is '异常处理次数';
comment on column TRANS_DEF.PIN_BLOCK
  is 'pin_block加密算法  0-普通 1-消费类 2-缴费类';
comment on column TRANS_DEF.FUNCTION_INDEX
  is '功能提示信息索引，0-无提示';
comment on column TRANS_DEF.TRANS_NAME
  is '交易名称';
comment on column TRANS_DEF.TELEPHONE_NO
  is '使用的电话号码序号';
comment on column TRANS_DEF.DISP_TYPE
  is '结果显示刷新方式 0-不刷新 1-刷新显示首页';
comment on column TRANS_DEF.TOTRANS_MSG_TYPE
  is '交易处理模块接收消息类型，用于接入层选择交易路由';
comment on column TRANS_DEF.TOHOST_MSG_TYPE
  is '后台接口模块接收消息类型，用于交易处理层选择交易路由';
comment on column TRANS_DEF.IS_VISIBLE
  is '是否在交易类型列表中呈现  0-不呈现  1-呈现';
create unique index TRANS_DEF_IDX1 on TRANS_DEF (TRANS_CODE);
create unique index TRANS_DEF_IDX2 on TRANS_DEF (TRANS_TYPE);

drop table TRANS_TYPES_8583;
create table TRANS_TYPES_8583
(
  BANK_TYPE     INTEGER not null,
  TRANS_TYPE    INTEGER not null,
  REQ_MSG_ID    CHAR(4) not null,
  REQ_PROC_CODE CHAR(6),
  RSP_MSG_ID    CHAR(4) not null,
  RSP_PROC_CODE CHAR(6),
  TRANS_NAME    VARCHAR(40)
)
;
comment on table TRANS_TYPES_8583
  is '8583包交易类型定义表';
comment on column TRANS_TYPES_8583.BANK_TYPE
  is '银行类别';
comment on column TRANS_TYPES_8583.TRANS_TYPE
  is '交易类型';
comment on column TRANS_TYPES_8583.REQ_MSG_ID
  is '请求消息码';
comment on column TRANS_TYPES_8583.REQ_PROC_CODE
  is '请求处理码';
comment on column TRANS_TYPES_8583.RSP_MSG_ID
  is '响应消息码';
comment on column TRANS_TYPES_8583.RSP_PROC_CODE
  is '相应处理码';
comment on column TRANS_TYPES_8583.TRANS_NAME
  is '交易名称';
alter table TRANS_TYPES_8583
  add constraint PK_TRANS_TYPES_8583 primary key (BANK_TYPE, TRANS_TYPE);

drop table UNIT_INFO;
create table UNIT_INFO
(
  TRANS_CODE CHAR(8),
  UNIT_CODE  CHAR(8),
  UNIT_NAME  VARCHAR(40)
)
;
comment on table UNIT_INFO
  is '收费机构信息表';
comment on column UNIT_INFO.TRANS_CODE
  is '交易代码';
comment on column UNIT_INFO.UNIT_CODE
  is '收费机构代码';
comment on column UNIT_INFO.UNIT_NAME
  is '收费机构名称';
create unique index UNIT_INFO_IDX on UNIT_INFO (TRANS_CODE);

drop table UNKNOWN_CARD;
create table UNKNOWN_CARD
(
  TRACK2 VARCHAR(40) not null,
  TRACK3 VARCHAR(107),
  S_DATE CHAR(8),
  FLAG   CHAR(2)
)
;
comment on table UNKNOWN_CARD
  is '未知卡表';
comment on column UNKNOWN_CARD.TRACK2
  is '磁道2信息';
comment on column UNKNOWN_CARD.TRACK3
  is '磁道3信息';
comment on column UNKNOWN_CARD.S_DATE
  is '记录日期';
comment on column UNKNOWN_CARD.FLAG
  is '处理标识 00-未处理  01-已处理';

drop table WEB_MENU;
create table WEB_MENU
(
  MODULE_ID     INTEGER not null,
  SYS_ID        VARCHAR(10) not null,
  MENU_NAME  VARCHAR(20) not null,
  ROUTE VARCHAR(255),
  UP_MODULE_ID    INTEGER not null,
  SPLIT        CHAR(1)
)
;
comment on table WEB_MENU
  is '管理监控系统菜单表';
comment on column WEB_MENU.MODULE_ID
  is '菜单ID';
comment on column WEB_MENU.SYS_ID
  is '系统ID';
comment on column WEB_MENU.MENU_NAME
  is '菜单名称';
comment on column WEB_MENU.ROUTE
  is '路径';
comment on column WEB_MENU.UP_MODULE_ID
  is '上级菜单ID';
alter table WEB_MENU
  add constraint PK_MODULE primary key (MODULE_ID);
create index WEB_MENU_IDX on WEB_MENU (SYS_ID);

drop table trans_conf;
create table trans_conf
(
	trans_type		INTEGER NOT NULL,
	amount_single		NUMERIC(12,2) DEFAULT 0,
	amount_sum		NUMERIC(12,2) DEFAULT 0,
	max_count		INTEGER DEFAULT 0,
	credit_amount_single	NUMERIC(12,2) DEFAULT 0,
	credit_amount_sum	NUMERIC(12,2) DEFAULT 0,
	credit_max_count	INTEGER DEFAULT 0,
	card_type_out		CHAR(9) DEFAULT '111111111',
	card_type_in		CHAR(9) DEFAULT '111111111',
	fee_calc_type		INTEGER DEFAULT 0
);
comment on table trans_conf
	is '交易风控参数表';
comment on column trans_conf.trans_type
	is '交易类型';
comment on column trans_conf.amount_single
	is '单笔交易限额';
comment on column trans_conf.amount_sum
	is '当日累计限额';
comment on column trans_conf.max_count
	is '当日最大交易笔数';
comment on column trans_conf.credit_amount_single
	is '信用卡单笔交易限额';
comment on column trans_conf.credit_amount_sum
	is '信用卡当日累计限额';
comment on column trans_conf.credit_max_count
	is '信用卡最大交易笔数';
comment on column trans_conf.card_type_out
	is '转出卡许可卡种,每位代表某种卡是否允许交易,1为允许,0为不允许。第一位：他行借记卡 第二位：他行贷记卡 第三位：他行准贷记卡 第四位：本行借记卡
	    第五位：本行贷记卡 第六位：本行准贷记卡 第七位：本地借记卡 第八位：本地贷记卡 第九位：本地准贷记卡';
comment on column trans_conf.card_type_in
	is '转入卡许可卡种';
comment on column trans_conf.fee_calc_type
	is '手续费计算方式 0-不计算 1-按比率 2-按区间';
create unique index trans_conf_idx ON trans_conf(trans_type);

drop table dept_conf;
create table dept_conf
(
	dept_no			CHAR(15) NOT NULL,
	dept_detail		VARCHAR(70) NOT NULL,
	trans_type		INTEGER NOT NULL,
	amount_single		NUMERIC(12,2) DEFAULT 0,
	amount_sum		NUMERIC(12,2) DEFAULT 0,
	max_count		INTEGER DEFAULT 0,
	credit_amount_single	NUMERIC(12,2) DEFAULT 0,
	credit_amount_sum	NUMERIC(12,2) DEFAULT 0,
	credit_max_count	INTEGER DEFAULT 0,
	card_type_out		CHAR(9) DEFAULT '111111111',
	card_type_in		CHAR(9) DEFAULT '111111111',
	fee_calc_type		INTEGER DEFAULT 0
)
;
comment on table dept_conf
	is '机构风控参数表';
comment on column dept_conf.dept_no
	is '机构号';
comment on column dept_conf.dept_detail
	is '机构层级信息';
comment on column dept_conf.trans_type
	is '交易类型';
comment on column dept_conf.amount_single
	is '单笔交易限额';
comment on column dept_conf.amount_sum
	is '当日累计限额';
comment on column dept_conf.max_count
	is '当日最大交易笔数';
comment on column dept_conf.credit_amount_single
	is '信用卡单笔交易限额';
comment on column dept_conf.credit_amount_sum
	is '信用卡当日累计限额';
comment on column dept_conf.credit_max_count
	is '信用卡最大交易笔数';
comment on column dept_conf.card_type_out
	is '转出卡许可卡种';
comment on column dept_conf.card_type_in
	is '转入卡许可卡种';
comment on column dept_conf.fee_calc_type
	is '手续费计算方式 0-不计算 1-按比率 2-按区间';
create unique index dept_conf_idx ON dept_conf(dept_no, trans_type);

drop table shop_conf;
create table shop_conf
(
	shop_no			CHAR(15) NOT NULL,
	trans_type		INTEGER NOT NULL,
	amount_single		NUMERIC(12,2) DEFAULT 0,
	amount_sum		NUMERIC(12,2) DEFAULT 0,
	max_count		INTEGER DEFAULT 0,
	credit_amount_single	NUMERIC(12,2) DEFAULT 0,
	credit_amount_sum	NUMERIC(12,2) DEFAULT 0,
	credit_max_count	INTEGER DEFAULT 0,
	card_type_out		CHAR(9) DEFAULT '111111111',
	card_type_in		CHAR(9) DEFAULT '111111111',
	fee_calc_type		INTEGER DEFAULT 0
)
;
comment on table shop_conf
	is '商户风控参数表';
comment on column shop_conf.shop_no
	is '商户号';
comment on column shop_conf.trans_type
	is '交易类型';
comment on column shop_conf.amount_single
	is '单笔交易限额';
comment on column shop_conf.amount_sum
	is '当日累计限额';
comment on column shop_conf.max_count
	is '当日最大交易笔数';
comment on column shop_conf.credit_amount_single
	is '信用卡单笔交易限额';
comment on column shop_conf.credit_amount_sum
	is '信用卡当日累计限额';
comment on column shop_conf.credit_max_count
	is '信用卡最大交易笔数';
comment on column shop_conf.card_type_out
	is '转出卡许可卡种';
comment on column shop_conf.card_type_in
	is '转入卡许可卡种';
comment on column shop_conf.fee_calc_type
	is '手续费计算方式 0-不计算 1-按比率 2-按区间';
create unique index shop_conf_idx ON shop_conf(shop_no, trans_type);

drop table pos_conf;
create table pos_conf
(
	shop_no			CHAR(15) NOT NULL,
	pos_no			CHAR(15) NOT NULL,
	trans_type		INTEGER NOT NULL,
	amount_single		NUMERIC(12,2) DEFAULT 0,
	amount_sum		NUMERIC(12,2) DEFAULT 0,
	max_count		INTEGER DEFAULT 0,
	credit_amount_single	NUMERIC(12,2) DEFAULT 0,
	credit_amount_sum	NUMERIC(12,2) DEFAULT 0,
	credit_max_count	INTEGER DEFAULT 0,
	card_type_out		CHAR(9) DEFAULT '111111111',
	card_type_in		CHAR(9) DEFAULT '111111111',
	fee_calc_type		INTEGER DEFAULT 0
)
;
comment on table pos_conf
	is '终端风控参数表';
comment on column pos_conf.shop_no
	is '商户号';
comment on column pos_conf.pos_no
	is '终端号';
comment on column pos_conf.trans_type
	is '交易类型';
comment on column pos_conf.amount_single
	is '单笔交易限额';
comment on column pos_conf.amount_sum
	is '当日累计限额';
comment on column pos_conf.max_count
	is '当日最大交易笔数';
comment on column pos_conf.credit_amount_single
	is '信用卡单笔交易限额';
comment on column pos_conf.credit_amount_sum
	is '信用卡当日累计限额';
comment on column pos_conf.credit_max_count
	is '信用卡当日最大交易笔数';
comment on column pos_conf.card_type_out
	is '转出卡许可卡种';
comment on column pos_conf.card_type_in
	is '转入卡许可卡种';
comment on column pos_conf.fee_calc_type
	is '手续费计算方式 0-不计算 1-按比率 2-按区间';
create unique index pos_conf_idx ON pos_conf(shop_no, pos_no, trans_type);

drop table dept_fee_rate;
create table dept_fee_rate
(
	dept_no			CHAR(15) NOT NULL,
	dept_detail		VARCHAR(70) NOT NULL,
	trans_type		INTEGER NOT NULL,
	fee_type		INTEGER NOT NULL,
	card_type		INTEGER DEFAULT 0,
	amount_begin		NUMERIC(12,2) NOT NULL,
	fee_rate		INTEGER NOT NULL,
	fee_base		NUMERIC(12,2) NOT NULL,
	fee_min			NUMERIC(12,2) NOT NULL,
	fee_max			NUMERIC(12,2) NOT NULL
)
;
comment on table dept_fee_rate
	is '机构手续费率表';
comment on column dept_fee_rate.dept_no
	is '机构号';
comment on column dept_fee_rate.dept_detail
	is '机构层级信息';
comment on column dept_fee_rate.trans_type
	is '交易类型';
comment on column dept_fee_rate.fee_type
	is '费率种类 1：本行同城转帐类 2：本行异地转帐类 3：跨行转帐类 4：本行卡扣款类 5：他行卡扣款类';
comment on column dept_fee_rate.card_type
	is '卡种';
comment on column dept_fee_rate.amount_begin
	is '区间起始金额';
comment on column dept_fee_rate.fee_rate
	is '手续费率';
comment on column dept_fee_rate.fee_base
	is '基础费用';
comment on column dept_fee_rate.fee_min
	is '最低手续费';
comment on column dept_fee_rate.fee_max
	is '最高手续费';
create unique index dept_fee_rate_idx ON dept_fee_rate (dept_no, trans_type, fee_type, card_type, amount_begin);

drop table shop_fee_rate;
create table shop_fee_rate
(
	shop_no			CHAR(15) NOT NULL,
	trans_type		INTEGER NOT NULL,
	fee_type		INTEGER NOT NULL,
	card_type		INTEGER DEFAULT 0,
	amount_begin		NUMERIC(12,2) NOT NULL,
	fee_rate		INTEGER NOT NULL,
	fee_base		NUMERIC(12,2) NOT NULL,
	fee_min			NUMERIC(12,2) NOT NULL,
	fee_max			NUMERIC(12,2) NOT NULL
)
;
comment on table shop_fee_rate
	is '商户手续费率表';
comment on column shop_fee_rate.shop_no
	is '商户号';
comment on column shop_fee_rate.trans_type
	is '交易类型';
comment on column shop_fee_rate.fee_type
	is '费率种类 1：本行同城转帐类 2：本行异地转帐类 3：跨行转帐类 4：本行卡扣款类 5：他行卡扣款类';
comment on column shop_fee_rate.card_type
	is '卡种';
comment on column shop_fee_rate.amount_begin
	is '区间起始金额';
comment on column shop_fee_rate.fee_rate
	is '手续费率';
comment on column shop_fee_rate.fee_base
	is '基础费用';
comment on column shop_fee_rate.fee_min
	is '最低手续费';
comment on column shop_fee_rate.fee_max
	is '最高手续费';
create unique index shop_fee_rate_idx ON shop_fee_rate (shop_no, trans_type, fee_type, card_type, amount_begin);


DROP TABLE emv_key;
CREATE TABLE emv_key
(
    key_ver             CHAR(8) NOT NULL,
    key_data            VARCHAR(1204)
)
;
COMMENT ON TABLE emv_key
    is 'EMV公钥表';
COMMENT ON COLUMN emv_key.key_ver
    is 'EMV公钥版本号';
COMMENT ON COLUMN emv_key.key_data
    is 'EMV公钥';
CREATE UNIQUE INDEX emv_key_idx ON emv_key (key_ver);

DROP TABLE emv_para;
CREATE TABLE emv_para
(
    para_ver             CHAR(12) NOT NULL,
    para_data            VARCHAR(1204)
)
;
COMMENT ON TABLE emv_para
    is 'EMV参数表';
COMMENT ON COLUMN emv_para.para_ver
    is 'EMV参数版本号';
COMMENT ON COLUMN emv_para.para_data
    is 'EMV参数';
CREATE UNIQUE INDEX emv_para_idx ON emv_para (para_ver);
