/*******************************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：数据库表对应的数据结构
** 创 建 人：Robin
** 创建日期：2012/11/30
**
** $Revision: 1.12 $
** $Log: DbStru.h,v $
** Revision 1.12  2012/12/28 05:25:33  fengw
**
** 1、删除T_CARDS结构体定义。
**
** Revision 1.11  2012/12/28 03:40:58  wukj
** *** empty log message ***
**
** Revision 1.10  2012/12/28 03:33:45  wukj
** 流水表新增DeptDetail字段
**
** Revision 1.9  2012/12/24 09:04:49  wukj
** *** empty log message ***
**
** Revision 1.8  2012/12/18 07:16:26  wukj
** *** empty log message ***
**
** Revision 1.7  2012/12/13 06:50:42  wukj
** *** empty log message ***
**
** Revision 1.6  2012/12/13 03:00:01  wukj
** *** empty log message ***
**
** Revision 1.5  2012/12/12 09:02:25  wukj
** *** empty log message ***
**
** Revision 1.4  2012/12/07 01:52:45  wukj
** 新增库表结构定义,完成
**
** Revision 1.3  2012/12/05 08:06:45  wukj
** 新增库表结构体,未完待续
**
** Revision 1.2  2012/12/05 06:33:40  wukj
** *** empty log message ***
**
** Revision 1.1  2012/11/30 07:20:43  chenrb
** 初始版本
**
**
*******************************************************************************/

#ifndef POS_STRC
#define POS_STRC

struct s_fee_rate{
    char    zone_no[6];
    int    fee_type;
    long    rate;
    double  fee1;
    double  fee2;
};

struct s_my_customer_bak{
    char    merchant_id[16];
    char    pan[20];
    char    name[41];
    char    expire_date[5];
    char    bank_name[21];
    char    register_date[9];
    char    register_time[7];
};


struct s_query_condition{
    char    psam_no[17];
    char    pan[21];
    char    begin_date[9];
    char    end_date[9];
    char    merchant_id[17];
    char    terminal_id[17];
    double    amount;
};

struct s_register_card{
    char    merchant_id[16];
    char    terminal_id[16];
    char    pan[20];
    char    name[41];
    char    expire_date[5];
    char    register_date[9];
    char    register_time[7];
    char    bank_name[21];
    char    transtype[2];
    char    oper_no[17];
    int    status;
};

struct s_pid_match{
    long    pid;
    char    local_date[9];
    char    merchant_id[16];
    char    terminal_id[16];
    long    send_time;
    long    posp_trace;
};


struct s_comweb_pid{
    char    merchant_id[16];
    char    terminal_id[16];
    long    pid;
};


struct s_trans_def{
    int    trans_type;
    char    trans_code[9];
    char    next_trans_code[9];
    char    autovoid[2];
    char    pin_block[2];
    int    function_index;
    char    trans_name[21];
    int    telephone_no;
    char    disp_type[2];
    char    nii[4];
    char    server_code[3];
};


struct s_trans_commands{
    int    trans_type;
    int    step;
    char    flag[2];
    char    command[3];
    int    func_index;
    char    alog[9];
    char    command_name[31];
    char    orgcommand[3];
    int    data_index;
};

struct s_app_def{
    int     app_type;
    char    app_name[21];
    char    app_describe[31];
    char    app_ver[9];
};

struct s_app_menu{
    int    moduleid;
    int    smoduleid;
    int    app_type;
    int    level_1;
    int    level_2;
    int    level_3;
    char    menu_name[21];
    char    trans_code[9];
    char    valid[2];
    char    update_date[9];
};

typedef struct {
    int     iAppType;
    char    szAppName[21];
    char    szAppDescribe[31];
    char    szAppVer[9];
}T_APP_DEF;

typedef struct {
    int     iMenuNo;
    int     iUpMenuNo;
    int     iAppType;
    int     iLevel1;
    int     iLevel2;
    int     iLevel3;
    char    szMenuName[21];
    char    szTransCode[9];
    char    szIsValid[2];
    char    szUpdateDate[9];
}T_APP_MENU;

typedef struct {
    char    szParaId[21];
    char    szParaName[41];
    char    szParaValue[41];
    char    szDescribe[61];
}T_ARTCONF;

typedef struct {
    char    szPan[20];
    char    szRegisterDate[9];
}T_BIND_CARD;

typedef struct {
    char    szPan[20];
    double dBalance;
}T_BIND_CARD_BALANCE;

typedef struct {
    char    szOperator[5];
    char    szOperPwd[7];
    char    szOperFlag[2];
    char    szOperLevel[2];
}T_CEN_OPER;

typedef struct {
    int     iCmdIndex;
    char    szOrgCommand[3];
    char    szDestCommand[3];
    int     iOperIndex;
    char    szAlog[9];
    char    szCommandName[31];
    int     iControlLen;
    char    szControlPara[61];
}T_COMMANDS;

typedef struct {
    char    szShopNo[16];
    char    szPosNo[16];
    int     iPid;
}T_COMWEB_PID;

typedef struct {
    char    szDeptNo[16];
    char    szDeptName[46];
    char    szUpDeptNo[16];
    char    szDeptDetail[71];
}T_DEPT;

typedef struct {
    char    szCatCode[21];
    char    szCatText[41];
}T_DICT_CAT;

typedef struct {
    char    szCatCode[21];
    char    szItemCode[21];
    char    szItemText[41];
}T_DICT_ITEM;

typedef struct
{
    int     iRecNo;
    char    szMenuTitle[31];
    char    szDescribe[41];
    int     iMenuNum;
    char    szMenuName1[21];
    char    szTransCode1[9];
    char    szMenuName2[21];
    char    szTransCode2[9];
    char    szMenuName3[21];
    char    szTransCode3[9];
    char    szMenuName4[21];
    char    szTransCode4[9];
    char    szMenuName5[21];
    char    szTransCode5[9];
    char    szMenuName6[21];
    char    szTransCode6[9];
    char    szMenuName7[21];
    char    szTransCode7[9];
    char    szMenuName8[21];
    char    szTransCode8[9];
    char    szMenuName9[21];
    char    szTransCode9[9];
}T_DYNAMIC_MENU;

typedef struct {
    char    szMoniTime[15];
    int     iHostNo;
    char    szHostName[33];
    char    szProcStatus[1025];
    char    szMsgStatus[1025];
    char    szCommStatus[1025];
}T_EPAY_MONI;

typedef struct {
    char    szReturnCode[3];
    char    szReturnName[13];
    char    szPosMsg[21];
    char    szHost1Ret[41];
    char    szHost2Ret[41];
    char    szHost3Ret[41];
    char    szHost4Ret[41];
}T_ERROR_CODE;

typedef struct {
    int     iErrorIndex;
    char    szOpFlag[2];
    int     iModuleNum;
    char    szInfo1Format[3];
    char    szInfo1[101];
    char    szInfo2Format[3];
    char    szInfo2[101];
    char    szInfo3Format[3];
    char    szInfo3[101];
    char    szUpdateDate[9];
}T_ERROR_INFO;

typedef struct {
    int     iFileId;
    char    szFileName[41];
    char    szLastDate[21];
    char    szLastMender[21];
    char    szDescribe[201];
}T_FILES_MGR;

typedef struct {
    int     iRecNo;
    char    szPageTitle[31];
    char    szDescribe[201];
    char    szValidDate[9];
}T_FIRST_PAGE;

typedef struct {
    int     iFuncIndex;
    char    szOpFlag[2];
    int     iModuleNum;
    char    szInfo1Format[3];
    char    szInfo1[101];
    char    szInfo2Format[3];
    char    szInfo2[101];
    char    szInfo3Format[3];
    char    szInfo3[101];
    char    szUpdatedate[9];
}T_FUNCTION_INFO;

typedef struct {
    char    szHostDate[9];
    char    szHostTime[7];
    char    szPan[20];
    double  dAmount;
    char    szCardType[2];
    int     iTransType;
    int     szBusinessType;
    char    szRetriRefNum[13];
    char    szAuthCode[7];
    char    szPosNo[16];
    char    szShopNo[16];
    char    szAccount2[19];
    double  dAddiAmount;
    int     iBatchNo;
    char    szPsamNo[17];
    int     iInvoice;
    char    szRetCode[3];
    char    szHostRetCode[7];
    char    szCancelFlag[2];
    char    szRecoverFlag[2];
    char    szPosSettle[2];
    char    szPosBatch[2];
    char    szHostSettle[2];
    int     iSysTrace;
    char    szOldRetriRefNum[13];
    char    szPosDate[9];
    char    szPosTime[7];
    char    szFinancialCode[41];
    char    szBusinessCode[41];
    char    szBankId[12];
    char    szSettleDate[9];
    char    szOperNo[5];
    char    szMac[17];
    int     iPosTrace;
    char    szDeptDetail[71];
}T_HISTORY_LS;

typedef struct {
    char    szReturnCode[7];
    char    szErrorMsg[8];
}T_HOST_ERROR;

typedef struct {
    int     iHostId;
    char    szHostName[41];
    char    szMacKey[33];
    char    szPinKey[33];
}T_HOST_KEY;

typedef struct {
    char    szShopNo[16];
    char    szPosNo[16];
    char    szMasterKey[33];
    char    szMacKey[33];
    char    szPinKey[33];
    char    szTermVersion[8];
    char    szCapkVersion[9];
    char    szCardTableVersion[15];
}T_HOST_TERM_KEY;

typedef struct {
    char    szHostDate[9];
    char    szHostTime[7];
    char    szPan[20];
    double  dAmount;
    char    szCardType[2];
    int     iTransType;
    int     szBusinessType;
    char    szRetriRefNum[13];
    char    szAuthCode[7];
    char    szPosNo[16];
    char    szShopNo[16];
    char    szAccount2[19];
    double  dAddiAmount;
    int     iBatchNo;
    char    szPsamNo[17];
    int     iInvoice;
    char    szRetCode[3];
    char    szHostRetCode[7];
    char    szCancelFlag[2];
    char    szRecoverFlag[2];
    char    szPosSettle[2];
    char    szPosBatch[2];
    char    szHostSettle[2];
    int     iSysTrace;
    char    szOldRetriRefNum[13];
    char    szPosDate[9];
    char    szPosTime[7];
    char    szFinancialCode[41];
    char    szBusinessCode[41];
    char    szBankId[12];
    char    szSettleDate[9];
    char    szOperNo[5];
    char    szMac[17];
    int     iPosTrace;
    char    szDeptDetail[71];
}T_VOID_LS;

typedef struct {
    int     iBankType;
    int     iFieldId;
    int     iMaxLen;
    int     FieldType;
    int     iFieldFlag;
    int     iLenFlag;
}T_ISO8583;

typedef struct {
    int     iBankType;
    int     iTransType;
    int     iFieldId;
    char    szFieldName[51];
    int     szFieldFormat[41];
    char    szDefaultData[41];
    int     iMondary;
}T_ISO8583_REQ_FIELD;

typedef struct {
    int     iBankType;
    int     iTransType;
    int     iFieldId;
    char    szFieldName[51];
    int     szFieldFormat[41];
    char    szDefaultData[41];
    int     iMondary;
}T_ISO8583_RSP_FIELD;

typedef struct {
    char    szCardId[17];
    char    szCardName[41];
    int     iCardNoLen;
    char    szCardType[2];
}T_LOCAL_CARD;

typedef struct {
    char    szDeptNo[16];
    int     iMarketNo;
    char    szMarketName[17];
}T_MARKET;

typedef struct {
    int     iModuleId;
    char    szModuleName[33];
    int     iMsgType;
    char    szPara1[16];
    char    szPara2[16];
    char    szPara3[16];
    char    szPara4[16];
    char    szPara5[16];
    char    szPara6[16];
    int     iRun;
}T_MODULE;

typedef struct {
    char    szShopNo[16];
    char    szPosNo[16];
    char    szPan[20];
    char    szAcctName[41];
    char    szExpireDate[5];
    char    szBankName[21];
    char    szRegisterDate[9];
    int     iRecNo;
}T_MYCUSTOMER;

typedef struct {  
    char    szOperNo[9];
    char    szOperName[41];
    int     iRealRoleId;
    char    szPassword[33];
    char    szAuthCode[13];
    char    szDeptNo[16];
    char    szDeptName[41];
    char    szCreaetDate[21];
    char    szIsValid[2];
    char    szLasterLogin[21];
    char    szBranchId[7];
    char    szIsAdmin[2];
    char    szEnterDate[9];
    int     szFailTimes;
    char    szEffectDate[9];
    char    szMac[15];
    char    szDeptDetail[71];
}T_OPER;

typedef struct {
    int     iOperIndex;
    char    szOpFlag[2];
    int     iModuleNum;
    char    szInfo1Format[3];
    char    szInfo1[41];
    char    szInfo2Format[3];
    char    szInfo2[41];
    char    szInfo3Format[3];
    char    szInfo3[41];
    char    szUpdateDate[9];
}T_OPERATION_INFO;

typedef struct {
    int     iOperIndex;
    char    szOpFlag[2];
    int     iModuleNum;
    char    szInfo1Format[3];
    char    szInfo1[41];
    char    szInfo2Format[3];
    char    szInfo2[41];
    char    szInfo3Format[3];
    char    szInfo3[41];
    char    szUpdateDate[9];
}T_OPERATION_TEMP;

typedef struct {
    int     iId;
    char    szModuleName[31];
    char    szMethodDesc[31];
    char    szKeyId[21];
    char    szKeyName[51];
    char    szUserName[21];
    char    szOperTime[31];
}T_OPERATION_LOG;

typedef struct {
    int     szListClass;
    char    szClassName[31];
    char    szContactor[11];
    char    szTelephone[27];
    char    szAddr[49];
}T_PAY_CLASS;

typedef struct{
    char    szPsamNo[17];
    int     iListClass;
    int     iListType;
    char    szGenDate[9];
    int     iListNo;
    char    szListData[201];
    double  dAmount;
    char    szPayDate[9];
    char    szDownFlag[2];
    char    szPayStatus[2];
}T_PAY_LIST;

typedef struct {
    int     iListClass;
    int     iListType;
    char    szTypeName[31];
    char    szTransCode[9];
}T_PAY_TYPE;

typedef struct {
    int     iTransDataIndex;
    char    szLocalDate[9];
    char    szShopNo[16];
    char    szPosNo[16];
    int     iSendTime;
    int     iSysTrace;
}T_TDI_MATCH;

typedef struct {
    char    szHostDate[9];
    char    szHostTime[7];
    char    szPan[20];
    double  dAmount;
    char    szCardType[2];
    int     iTransType;
    int     szBusinessType;
    char    szRetriRefNum[13];
    char    szAuthCode[7];
    char    szPosNo[16];
    char    szShopNo[16];
    char    szAccount2[19];
    double  dAddiAmount;
    int     iBatchNo;
    char    szPsamNo[17];
    int     iInvoice;
    char    szRetCode[3];
    char    szHostRetCode[7];
    char    szCancelFlag[2];
    char    szRecoverFlag[2];
    char    szPosSettle[2];
    char    szPosBatch[2];
    char    szHostSettle[2];
    int     iSysTrace;
    char    szOldRetriRefNum[13];
    char    szPosDate[9];
    char    szPosTime[7];
    char    szFinancialCode[41];
    char    szBusinessCode[41];
    char    szBankId[12];
    char    szSettleDate[9];
    char    szOperNo[5];
    char    szMac[17];
    int     iPosTrace;
    char    szDeptDetail[71];
} T_POSLS;

typedef struct {
    long    lKeyIndex;
    char    szMasterKey[33];
    char    szMasterKeyLMK[33];
    char    szMasterChk[5];
    char    szPinKey[33];
    char    szMacKey[33];
    char    szMagKey[33];
}T_POS_KEY;

typedef struct {
    int     iDataIndex;
    char    szDataName[41];
    char    szDataExample[41];
}T_PRINT_DATA;

typedef struct {
    int     iRecNo;
    char    szInfo[61];
    int     iDataIndex;
    char    szUpdateDate[9];
}T_PRINT_INFO;

typedef struct {
    int     iModuleId;
    char    szDescribe[41];
    int     iPrintNum;
    int     iTitle1;
    int     iTitle2;
    int     iTitle3;
    int     iSign1;
    int     iSign2;
    int     iSign3;
    int     iRecNum;
    char    szRecNo[81];
}T_PRINT_MODULE;

typedef struct {
    int     iModuleId;
    char    szDescribe[21];
    int     iPinKeyIndex;
    int     iMacKeyIndex;
    int     iFskTeleNum;
    char    szFskTeleNo1[16];
    char    szFskTeleNo2[16];
    char    szFskTeleNo3[16];
    int     iFskDownTeleNum;
    char    szFskDownTeleNo1[16];
    char    szFskDownTeleNo2[16];
    char    szFskDOwnTeleNo3[16];
    int     iHdlcTeleNum;
    char    szHdlcTeleNo1[16];
    char    szHdlcTeleNo2[16];
    char    szHdlcTeleNo3[16];
    int     iHdlcDownTeleNum;
    char    szHdlcDownTeleNo1[16];
    char    szHdlcDownTeleNo2[16];
    char    szHdlcDownTeleNo3[16];
    int     iFskBakTeleNum;
    char    szFskBakTeleNo1[16];
    char    szFskBakTeleNo2[16];
    char    szFskBakTeleNo3[16];
    int     iHdlcBakTeleNum;
    char    szHdlcBakTeleNo1[16];
    char    szHdlcBakTeleNo2[16];
    char    szHdlcBakTeleNo3[16];
}T_PSAM_PARA;

typedef struct {
    char    szPsamNo[17];
    char    szPan[21];
    char    szBeginDate[9];
    char    szEndDate[9];
    char    szShopNo[17];
    char    szPosNo[17];
    double  dAmount;
}T_QUERY_CONDITION;

typedef struct {
    char    szPsamNo[17];
    int     iPosTrace;
    char    szResult[1025];
}T_QUERY_RESULT;

typedef struct {
    int     iRealRoleId;
    char    szRoleName[21];
    char    szDeptNo[16];
}T_REAL_ROLE;

typedef struct {
    int     iRealRoleId;
    int     iSubRoleId;
    char    szDeptNo[16];
}T_REAL_SUB;

typedef struct {
    char    szShopNo[16];
    char    szPosNo[16];
    char    szPan[20];
    char    szAcctName[41];
    char    szExpireDate[5];
    char    szRegisterDate[9];
    char    szRegisterTime[7];
    char    szBankName[21];
    char    szTransType[2];
    char    szOperNo[17];
    int     iStatus;
}T_REGISTER_CARD;

typedef struct {
    int     iTransType;
    char    szFieldName[81];
    char    szFieldFormat[151];
    char    szStatus[2];
}T_RET_DESC;

typedef struct {
    char    szShopNo[16];
    int     iMarketNo;
    char    szShopName[41];
    char    szAcqBank[12];
    char    szContactor[11];
    char    szTelephone[26];
    char    szAddr[48];
    int     iFee;
    char    szFaxNum[26];
    int     iSignFlag;  /*0-签约，1-撤销*/
    char    szSignDate[9];   /*签约日期*/
    char    szUnSignDate[9]; /*撤销日期*/
    char    szDeptNo[15+1]; /*所属机构*/
    char    szCardKind[9+1]; /*卡应用类型*/
}T_SHOP;

typedef struct {
    char    szMccCode[5];
    char    szDescribe[41];
}T_SHOP_TYPE;

typedef struct {
    int     iRecNo;
    char    szMsgTitle[31];
    char    szMsgContent[141];
    char    szValidDate[9];
}T_SHORT_MESSAGE;

typedef struct {
    int     iStaticMenuId;
    char    szDownStaticMenu[2];
    int     iStaticMenuRecno;
    char    szPsamNo[17];
}T_STATIC_MENU_CFG;

typedef struct {
    char    szShopNo[16];
    char    szPosNo[16];
    char    szTransDate[9];
    int     iMarketNo;
    char    szBindCardNo[20];
    int     iPurCount;
    double  dPurAmount;
    double  dPurFee;
    int     iPayInCount;
    double  dPayInAmount;
    double  dPayInFee;
    int     iPayOutCount;
    double  dPayOutAmount;
    double  dPayOutFee;
    int     iType1Count;
    double  dType1Amount;
    double  dType1Fee;
    int     iType2Count;
    double  dType2Amount;
    double  dType2Fee;
    int     iType3Count;
    double  dType3Amount;
    double  dType3Fee;
    char    szReserve1[51];
    char    szDeptDetail[70];
    char    szDeptName[46];
    char    szShopName[41];
    int     iDelFlag;
}T_STAT_LINE;

typedef struct {
    int     iSubRoleId;
    char    szSysId[11];
    char    szSubRoleName[21];
    char    szDeptNo[16];
}T_SUB_ROLE;

typedef struct {
    int     iModuleId;
    int     iSubRoleId;
    char    szButtonMap[21];
    char    szDeptNo[16];
}T_SUB_ROLE_FUNC;
typedef struct {
    char    szSysId[11];
    char    szSysName[22];
}T_SUB_SYS;

typedef struct {
    int     iCurKeyIndex;
    int     iBatchNo;
    int     iSysTrace;
}T_SYSTEM_PARAMETER;

typedef struct {
    char    szIp[16];
    int     iIpid;
    int     iHostNo;
}T_TCPCOM_PID;

typedef struct  {
    char    szShopNo[16];
    char    szPosNo[16];
    char    szPsamNo[17];
    char    szTelephone[16];
    int     iTermModule;
    int     iPsamModule;
    int     iAppType;
    char    szDescribe[21];
    char    szPosType[41];
    char    szAddress[41];    /*安装地址*/
    char    szPutDate[9];    /*安装日期*/
    long    lCurTrace;
    char    szIp[16];
    int     iPort;
    char    szDownMenu[2];
    char    szDownTerm[2];
    char    szDownPsam[2];
    char    szDownPrint[2];
    char    szDownOperate[2];
    char    szDownFunction[2];
    char    szDownError[2];
    char    szDownAll[2];
    char    szDownPayList[2];
    int     iMenuRecNo;
    int     iPrintRecNo;
    int     iOperateRecNo;
    int     iFunctionRecNo;
    int     iErrorRecNo;
    int     iAllTransType;
    char    szTermBitMap[9];
    char    szPsamBitMap[9];
    char    szPrintBitMap[65];
    char    szOperateBitMap[65];
    char    szFunctionBitMap[65];
    char    szErrorBitMap[65];
    int     iMsgRecNum;
    char    szMsgRecNo[256+1];
    int     iFirstPage;
    int     iTStatus;       /*0-正常、1-停用*/
    long    lCurBatch;
}T_TERMINAL;

typedef struct {
    char  szShopNo[15+1] ;
    char  szPosNo[15+1];
    char  szOperNo[4+1];
    char  szOperPwd[6+1];
    char  szOperName[20];
    int   iDelFlag;
    int   iLoginStatus;
}T_TERMINAL_OPER;

typedef struct {
    int     iModuleId;
    char    szDescribe[21];
    char    szLineType[2];
    int     iInPutTimeOut;
    int     iTransTimeOut;
    char    szManagerPwd[9];
    char    szOperatorPwd[7];
    int     iTelephoneNo;
    int     iPinMaxLen;
    char    szAuthKey[9];
    char    szInnerKey[33];
    char    szExtKey[33];
    char    szPreTeleNo[13];
    int     iWaitTime;
    char    szTipSwitch[2];
    char    szAutoAnswer[2];
    int     iDelayTime;
    char    szHandDial[2];
    char    szSaveList[2];
    char    szPrintOrNot[2];
    char    szReader[2];
    char    szPinInput[2];
}T_TERMINAL_PARA;

typedef struct {
    char    szAppNo[51];
    char    szModelNo[41];
    char    szAppVer[41];
    char    szIssueDate[21];
}T_TM_APP_FILE_INFO;

typedef struct {
    char    szAppNo[51];
    char    szAppVer[41];
    char    szRemark[41];
    char    szCreateDate[21];
    char    szDeptNo[16];
}T_TM_APP_VER_INFO;

typedef struct {
    char    szAppNo[51];
    char    szAppName[81];
    char    szShareFlag[2];
    char    szIssueDate[21];
    char    szStatus[21];
    char    szDeptNo[16];
}T_TM_APP_INFO;

typedef struct {
    char    szFacNo[21];
    char    szFacName[41];
    char    szContactMan[51];
    char    szAddress[81];
    char    szTel[51];
    char    szFax[51];
    char    szPostCode[51];
}T_TM_FACTORY_INFO;

typedef struct {
    char    szModelNo[21];
    char    szModelName[31];
    char    szPlugInName[101];
    char    szFacNo[51];
    char    szFolderPath[256];
}T_TM_MODEL_INFO;

typedef struct {
    char    szId[21];
    char    szShopNo[21];
    char    szPosNo[21];
    char    szSn[21];
    char    szTransDate[21];
    char    szTransTime[21];
    char    szAppFlag[2];
    char    szStatus[51];
    char    szVposId[33];
}T_TM_TRADE_INFO;

typedef struct {
    char    szId[33];
    char    szAppNo[21];
    char    szAppVer[21];
    char    szAppChange[21];
    char    szParamNo[21];
    char    szParaChange[2];
    char    szVposId[33];
}T_TM_VPOS_APP_INFO;

typedef struct {
    char    szVposId[33];
    char    szPosNo[21];
    char    szNoticeFlag[2];
    char    szNoticeBtime[15];
    char    szNoticeEdate[9];
    char    szDownFlag[11];
    char    szShopNo[21];
    char    szDeptNo[16];   
}T_TM_VPOS_INFO;

typedef struct {
    char    szCollateDate[9];
    char    szShopNo[16];
    char    szPosNo[16];
    char    szTransDate[11];
    char    szTransTime[9];
    double  dAmount;
    double  dCost;
    double  dPureIncome;
    char    szPan[20];
    char    szAccount2[20];
    char    szRetriRefNum[13];
    char    szOldRetriRefNum[13];
    char    szBalance[21];
}T_TRANS_BALANCE;

typedef struct {
    int     iTransType;
    int     iStep;
    char    szTransFlag[2];
    char    szCommand[3];
    int     iOperIndex;
    char    szAlog[9];
    char    szCommandName[31];
    char    szOrgCommand[3];
    int     iControlLen;
    int     szControlPara[61];
    int     iDataIndex;
}T_TRANS_COMMANDS;

typedef struct {
    int     iTransType;
    char    szTransCode[9];
    char    szNextTransCode[9];
    char    szExcepHandle[2];
    char    szPinBlock[2];
    int     iFunctionIndex;
    char    szTransName[21];
    int     iTelephoneNo;
    char    szDispType[2];
    int     iToTransMsgType;
    int     iToHostMsgType;
    char    szIsVisible[2];
} T_TRANS_DEF;

typedef struct {
    int     iBankType;
    int     iTransType;
    char    szReqMsgId[5];
    char    szReqProcCode[7];
    char    szRspMsgId[5];
    char    szRspProcCode[7];
    char    szTransName[41];
}T_TRANS_TYPES_8583;

typedef struct {
    char    szTransCode[9];
    char    szUnitCode[9];
    char    szUnitName[41];
}T_UNIT_INFO;

typedef struct {
    char    szTrack2[41];
    char    szTrack3[108];
    char    szSDate[9];
    char    szFlag[3];
}T_UNKNOWN_CARD;

typedef struct {
    int     iModuleId;
    char    szSysId[11];
    char    szMenuName[21];
    char    szRote[256];
    int     iUpModuleId;
    char    szSplit[2];
}T_WEB_MENU;

typedef struct {
    int     iTransType;
    double  dAmountSingle;
    double  dAmountSum;
    int     iMaxCount;
    double  dCreditAmountSingle;
    double  dCreditAmountSum;
    int     iCreditMaxCount;
    char    szCardTypeOut[10];
    char    szCardTypeIn[10];
    int     iFeeCalcType;
}T_TRANS_CONF;

typedef struct {
    char    szDeptNo[16];
    char    szDeptDetail[71];
    int     iTransType;
    double  dAmountSingle;
    double  dAmountSum;
    int     iMaxCount;
    double  dCreditAmountSingle;
    double  dCreditAmountSum;
    int     iCreditMaxCount;
    char    szCardTypeOut[10];
    char    szCardTypeIn[10];
    int     iFeeCalcType;
}T_DEPT_CONF;

typedef struct {
    char    szShopNo[16];
    int     iTransType;
    double  dAmountSingle;
    double  dAmountSum;
    int     iMaxCount;
    double  dCreditAmountSingle;
    double  dCreditAmountSum;
    int     iCreditMaxCount;
    char    szCardTypeOut[10];
    char    szCardTypeIn[10];
    int     iFeeCalcType;
}T_SHOP_CONF;

typedef struct {
    char    szShopNo[16];
    char    szPosNo[16];
    int     iTransType;
    double  dAmountSingle;
    double  dAmountSum;
    int     iMaxCount;
    double  dCreditAmountSingle;
    double  dCreditAmountSum;
    int     iCreditMaxCount;
    char    szCardTypeOut[10];
    char    szCardTypeIn[10];
    int     iFeeCalcType;
}T_POS_CONF;

typedef struct {
    char    szDeptNo[16];
    char    szDeptDetail[70];
    int     iTransType;
    int     iFeeType;
    int     iCardType;
    double  dAmountBegin;
    int     iFeeRate;
    double  dFeeBase;
    double  dFeeMin;
    double  dFeeMax;
}T_DEPT_FEE_RATE;

typedef struct {
    char    szShopNo[16];
    int     iTransType;
    int     iFeeType;
    int     iCardType;
    double  dAmountBegin;
    int     iFeeRate;
    double  dFeeBase;
    double  dFeeMin;
    double  dFeeMax;
}T_SHOP_FEE_RATE;

typedef struct {
    char    szSysCode[8+1];       /*平台标识码*/
    char    szDownBitMap[2+1];    /*下载内容标志*/
    char    szDownFileName[72+1]; /*下载文件名*/
    char    szValidDate[8+1];         /*限制日期*/
    char    szAppName[72+1];          /*应用标识*/
    char    szDownTypeFlag[4+1];  /*下载时机标示*/
    char    szTmsTelNo1[20+1];        /*TMS电话号码1*/
    char    szTmsTelNo2[20+1];        /*TMS电话号码2*/
    char    szTmsIpPort1[30+1];       /*TMS IP和端口1*/
    char    szTmsIpPort2[30+1];       /*TMS IP和端口2*/
    char    szDownTime[14+1];         /*TMS下载时间*/
} T_TMS_PARA;

struct s_print_szInfo{
    int     rec_no;
    char    szInfo[61];
    int    data_index;
    char    update_date[9];
};

struct s_print_module{
    int    module;
    char    describe[41];
    int    print_num;
    int    title1;
    int    title2;
    int    title3;
    int    sign1;
    int    sign2;
    int    sign3;
    int    rec_num;
    char    rec_no[81];
};

struct s_term_para{
    int    module;
    char    describe[21];
    char    line_type[2];
    int    input_timeout;
    int    trans_timeout;
    char    manager_pwd[9];
    char    operator_pwd[7];
    int    telephone_no;
    int    pin_max_len;
    char    auth_key[9];
    char    inter_key[33];
    char    ext_key[33];
    char    pre_tele_no[13];
    int    wait_time;
    char    tip_switch[2];
    char    auto_answer[2];
    int    delay_time;
    char    hand_dial[2];
    char    save_list[2];
    char    print[2];
    char    reader[2];
    char    pin_input[2];
};

struct s_psam_para{
    int    module;
    char    describe[21];
    int    pin_key_index;
    int    mac_key_index;
    int    fsk_tele_num;
    char    fsk_tele_no1[16];
    char    fsk_tele_no2[16];
    char    fsk_tele_no3[16];
    int    fsk_down_tele_num;
    char    fsk_down_tele_no1[16];
    char    fsk_down_tele_no2[16];
    char    fsk_down_tele_no3[16];
    int    hdlc_tele_num;
    char    hdlc_tele_no1[16];
    char    hdlc_tele_no2[16];
    char    hdlc_tele_no3[16];
    int    hdlc_down_tele_num;
    char    hdlc_down_tele_no1[16];
    char    hdlc_down_tele_no2[16];
    char    hdlc_down_tele_no3[16];
    int    fskbak_tele_num;
    char    fskbak_tele_no1[16];
    char    fskbak_tele_no2[16];
    char    fskbak_tele_no3[16];
    int    hdlcbak_tele_num;
    char    hdlcbak_tele_no1[16];
    char    hdlcbak_tele_no2[16];
    char    hdlcbak_tele_no3[16];
};

struct s_term_key{
    char     psam_no[17];
    char     psam_status[2];
    char     master_key[33];
    char     telecom_pin_key[33];
    char     telecom_mac_key[33];
    char     telecom_mag_key[33];
    char     ccb_pin_key[33];
    char     ccb_mac_key[33];
    char     ccb_mag_key[33];
};

struct s_pos_key{
    long    key_index;
    char     master_key[33];
    char     master_key_LMK[33];
    char    master_chk[5];
    char     pin_key[33];
    char     mac_key[33];
    char     mag_key[33];
};

struct s_host_term_key{
    char    merchant_id[16];
    char    terminal_id[16];
    char     master_key[33];
    char     mac_key[33];
    char     pin_key[33];
    char    TermVersion[8];
    char    CAPKVersion[9];
    char    ParaVersion[13];
    char    CardTableVersion[15];
};

struct s_app_term_key{
    int        App_type;
    char    merchant_id[16];
    char    terminal_id[16];
    char     master_key[33];
    char     mac_key[33];
    char     pin_key[33];
    char    TermVersion[8];
    char    CAPKVersion[9];
    char    ParaVersion[13];
    char    CardTableVersion[15];
};

struct s_pay_class{
    int    list_class;    
    char    class_name[31];
    char    contactor[11];
    char    telephone[27];
    char    addr[49];
};

struct s_pay_type{
    int    list_class;
    int    list_type;
    char    type_name[31];
    char    trans_code[9];
};

struct s_pay_list{
    char    psam_no[17];
    int    list_class;
    int    list_type;
    char    gen_date[9];
    long    list_no;
    char    data[201];
    double    amount;
    char    pay_date[9];
    char    down[2];
    char    status[2];
};

struct s_cen_oper {
    char operator[5];
    char oper_pwd[7];
    char oper_flag[2];
    char oper_level[2];
};

struct s_shop {   
    char    shop_no[16];
    int    shop_type;
    char    shop_name[41];
    char    acq_bank[12];
    
    char    contactor[11];
    char    telephone[26];
    char    addr[48];
    int    fee;
    long    collec_limit;
    long    collec_total;
    long    out_limit;
    long    out_total;
    long    other_limit;
    long    other_total;

    char     fax_num[26];
    int     sign_flag;  /*0-签约，1-撤销*/
    char    sign_date[9];    /*签约日期*/
    char    unsign_date[9];    /*撤销日期*/
    char    szDeptNo[15+1];    /*所属机构*/
    char    szCardKind[9+1]; /*卡应用类型*/    
};

struct s_terminal {
    char     shop_no[16];
        char     pos_id[16];
    char    psam_no[17];
    char    telephone[16];

    int    term_module;
    int    psam_module;
    int    app_type;
    char    describe[21];

    long    collec_limit;
    long    collec_total;
    long    out_limit;
    long    out_total;
    long    other_limit;
    long    other_total;

    char    pos_type[41];
    char    address[41];    /*安装地址*/
    char    put_date[9];    /*安装日期*/
    long    cur_trace;
    char    ip[16];
    char    down_menu[2];
    char    down_term[2];
    char    down_psam[2];
    char    down_print[2];
    char    down_operate[2];
    char    down_function[2];
    char    down_error[2];
    char    down_all[2];
    char    down_paylist[2];
    int    menu_recno;
    int    print_recno;
    int    operate_recno;
    int    function_recno;
    int    error_recno;
    int    all_transtype;
    char    term_bitmap[9];
    char    psam_bitmap[9];
    char    print_bitmap[65];
    char    operate_bitmap[65];
    char    function_bitmap[65];
    char    error_bitmap[65];
    int    msg_recnum;
    char    msg_recno[61];
    int     first_page;
    int    t_status;    /*0-正常、1-停用*/
    long    cur_batch;
};

struct s_cards {
    char     bank_name[21];
    char     bank_id[12];
    char     card_name[41];
    char     card_id[20];
    int    card_no_len;
    int     card_site2;
    int     exp_site2;
    int    pan_site3;
    int     card_site3;
    int     exp_site3;
    char    card_type[2];
    int    card_level;

};

struct s_cipher  {
    char shop_no[16];
    char pos_id[16];
    char master_key[17];
    char work_key[17];
    char mac_key[17];
    char old_work_key[17];
    char old_mac_key[17];
};

struct s_module {
    int    module_id;
    char    module_name[33];
    char    nii[4];
    char    server_code[3];
    char    para1[16];
    char    para2[11];
    char    para3[11];
    char    para4[11];
    char    para5[11];
    char    para6[11];
    int    run;
};

struct s_router {
    char    nii [ 4 ];
    char    server_code [ 3 ];
    int    pid;
};

struct s_transtype {
    int    trans_type;
    char    trans_name [ 9 ];
    char    msg_id [ 5 ];
    char    proc_code [ 7 ];
    char    tran_code [ 31 ];
    char    r_bitmap [ 17 ];
    char    s_bitmap [ 17 ];
};

struct s_error_code {
    char    return_code[3];
    char     return_name[13];
    char    pos_msg[21];
    char    host1_ret[5];
    char    host2_ret[5];
    char    host3_ret[5];
    char    host4_ret[5];
};

struct s_trans_limit{

    char     card_no[20];
    long    single_amount;
    int    days;
    long    total_amount;
};

/*================ auto_void 表 ================*/
struct s_auto_void {
    char    equip_date[11];
    char    equip_time[9];

    long    msgtype;

    char    terminal_id[16];
    char    merchant_id[16];
    char    merchant_name[31];

    char    card_type[3];
    char    card_no[20];
    char    unit[8];
    char    expire[5];
    char    card_no2[20];

    int    trans_type;

    long    trace;
    long    invoice;
    long    void_invoice;
    long    hostls;
    long    void_hostls;
    long    batch_no;
    char    authno[7];

    double    amount;
    double    fee;

    char    res_flag[2];
    char    ret_code[4];
    char    oper_no[3];
    char    flag1[2];
    char    flag2[2];

    char    mode[4];
    char    offset[7];
    char    passwd[9];
    char    person_id[16];
    char    track2[38];
    char    track3[105];
    
    char    dac[17];
};

struct s_local_card {
    char    card_id[17];
    char    card_name[41];
    int    card_no_len;
    char    card_type[2];
};

/*================ t_std_fee_conf 表 ================*/
struct s_std_fee_conf {
    char    szTransCode[8+1];
    double  dblBeginAmt;
    int      iBeginOpercode;
    double  dblEndAmt;
    int      iEndOpercode;
    double  dblFee;
};

/*================ t_std_trans_conf 表 ================*/
struct s_std_trans_conf {
    char    szTransCode[8+1];
    int        iFeeType;
    double  dblBaseFee;
    double  dblMinFee;
    double  dblPercent;
    double  dblMaxFee;
    double  dblSingleAmount;
    double  dblTotalAmount;
    long    lMaxCount;
    char    szCardOutKind[9+1];
    char     szCardInKind[9+1];
};

/*================ t_term_conf 表 ================*/
struct s_term_conf {
    char    szTransCode[8+1];
    char    szMerchantId[15+1];
    char    szTerminalId[15+1];
    int        iFeeType;
    long   lFeePercent;
    double  dblSingleAmount;
    double  dblTotalAmount;
    long    lMaxCount;
    char    szCardOutKind[9+1];
    char     szCardInKind[9+1];
};

/*add by gaomx 20110517 */
struct s_terminal_oper {
            char  shop_no[15+1] ;  
            char  pos_no[15+1];
            char  oper_no[4+1];
            char  oper_pwd[6+1];
            char  oper_name[20];
            int   del_flag;
            int   login_status;
};
/*密钥信息*/
struct s_keydata{
        char    szProviderBusinessId[21];       /*业务id号*/
        char    szInitKey[17];  /*初始密钥*/
        char    szPinKey[17];   /*pinkey*/
        char    szMacKey[17];   /*Mackey*/
        char    szKey1[17];     /*身份验证密钥*/
};


/*TMS通知参数add by gaomx 20120426*/
struct s_tmspara{
        char    szSysCode[8+1];       /*平台标识码*/
        char    szDownBitMap[2+1];    /*下载内容标志*/
        char    szDownFileName[72+1]; /*下载文件名*/
        char    szValidDate[8+1];      /*限制日期*/
        char    szAppName[72+1];      /*应用标识*/
        char    szDownTypeFlag[4+1];  /*下载时机标示*/
        char    szTmsTelNo1[20+1];      /*TMS电话号码1*/
        char    szTmsTelNo2[20+1];      /*TMS电话号码2*/
        char    szTmsIpPort1[30+1];      /*TMS IP和端口1*/
        char    szTmsIpPort2[30+1];      /*TMS IP和端口2*/
        char    szDownTime[14+1];      /*TMS下载时间*/  
};

/*静态菜单数据add by gaomx 20120926*/
struct s_menu_item{
        int        iItemId;            //菜单索引
        char    szDispTitle[20+1];    //显示标题
        char    szDispData[90+1];    //显示内容
        char    szOutData[30+1];    //输出内容
};

struct s_static_menu{
        int        iMenu_Count;           /*静态菜单个数*/
        int     iMenu_ID;             /*静态菜单ID*/
        int        iMenu_item_Count;     /*静态菜单选项个数*/
        struct s_menu_item stMenuItem[20+1];    /*静态菜单明细*/ 
};

struct s_my_customer{
        int     rec_no;
        char    pan[19+1];             
        char    name[40+1];
        char    expire_date[6+1]; 
        char    bank_name[20+1];
        char    update_date[9];
};

#endif
