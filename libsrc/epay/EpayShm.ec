/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 共享内存操作
** 创 建 人：冯炜
** 创建日期：2012-11-27
**
** $Revision: 1.15 $
** $Log: EpayShm.ec,v $
** Revision 1.15  2013/06/28 08:35:16  fengw
**
** 1、添加创建、删除时记录TRACE日志，出现故障时便于跟踪确定问题。
**
** Revision 1.14  2013/06/25 01:57:46  fengw
**
** 1、增加创建成功后记录TRACE日志。
**
** Revision 1.13  2013/01/14 06:24:11  fengw
**
** 1、修改DelAccessPid函数参数。
**
** Revision 1.12  2012/12/24 08:55:50  wukj
** 删除共享内存时,将共享内存指针全局变量置为NULL
**
** Revision 1.11  2012/12/20 09:22:33  wukj
** *** empty log message ***
**
*******************************************************************/

#include "EpayShm.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

static int _LoadCard();
static int _LoadLocalCard();
static int _GetCard(char *szCardNo, int iType, char *szBankID, T_CARDS *ptCard, int *piCardBelong);
static int _InsertUnknowCard(char *szTrack2, char *szTrack3);

/****************************************************************
** 功    能：创建EPAY共享内存
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/27
** 调用说明：
**
** 修改日志：
****************************************************************/
int CreateEpayShm()
{
    char    szFileName[128+1];

    memset(szFileName, 0, sizeof(szFileName));

    WriteLog(TRACE, "创建EPAY共享内存开始");

    GetFullName("WORKDIR", SHM_FILE, szFileName);

    /* 创建EPAY共享内存 */
    if((giShmEpayID = CreateShm(szFileName, EPAY_SHM_ID, SHM_EPAY_SIZE)) == FAIL)
    {
        WriteLog(ERROR, "创建EPAY共享内存失败");

        return FAIL;
    }

    /* 初始化共享内存 */
    if(InitEpayShm() != SUCC)
    {
        /* 删除EPAY共享内存 */
        RmShm(giShmEpayID);

        return FAIL;
    }

    WriteLog(TRACE, "创建EPAY共享内存成功");

    return SUCC;
}

/****************************************************************
** 功    能：获取EPAY共享内存ID，并映射
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/29
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetEpayShm()
{
    char    szFileName[128+1];

    memset(szFileName, 0, sizeof(szFileName));

    GetFullName("WORKDIR", SHM_FILE, szFileName);

    /* 获取共享内存 */
    if(gpShmEpay == NULL)
    {
        if(giShmEpayID <= 0 && (giShmEpayID = GetShm(szFileName, EPAY_SHM_ID, SHM_EPAY_SIZE)) == FAIL)
        {
            WriteLog(ERROR, "获取EPAY共享内存ID失败!");

            return FAIL;
        }

        /* 映射共享内存 */
        if((gpShmEpay = (T_SHM_EPAY *)AtShm(giShmEpayID)) == NULL)
        {
            WriteLog(ERROR, "映射EPAY共享内存失败");

            return FAIL;
        }
    }

    return SUCC;
}

/****************************************************************
** 功    能：初始化EPAY共享内存
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int InitEpayShm()
{
    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    /* 共享内存初始化 */
    memset((char *)gpShmEpay, 0, SHM_EPAY_SIZE);

    /* 载入卡表数据到共享内存 */
    if(LoadCardToShm() != SUCC)
    {
        return FAIL;
    }

    return SUCC;
}

/****************************************************************
** 功    能：载入卡表数据到共享内存
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/19
** 调用说明：
**
** 修改日志：
****************************************************************/
int LoadCardToShm()
{
    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }
    
    if(_LoadCard() != SUCC)
    {
        WriteLog(ERROR, "载入卡表数据失败!");
        
        return FAIL;
    }
    
    if(_LoadLocalCard() != SUCC)
    {
        WriteLog(ERROR, "载入本地卡表数据失败!");
        
        return FAIL;
    }
    
    return SUCC;
}

/****************************************************************
** 功    能：载入卡表数据到共享内存
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/19
** 调用说明：
**
** 修改日志：
****************************************************************/
static int _LoadCard()
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szBankName[20+1];               /* 银行名称 */
        char    szBankID[11+1];                 /* 银行标识 */
        char    szCardName[40+1];               /* 卡名称 */
        char    szCardID[19+1];                 /* 卡标识串 */
        int     iCardNoLen;                     /* 卡号长度 */
        int     iCardSite2;                     /* 二磁道卡号位置 */
        int     iExpSite2;                      /* 二磁道卡有效期位置 */
        int     iPanSite3;                      /* 三磁道账号位置 */
        int     iCardSite3;                     /* 三磁道卡号位置 */
        int     iExpSite3;                      /* 三磁道有效期位置 */
        char    szCardType[1+1];                /* 卡类型 0-借记卡 1-贷记卡 2-任意卡 3-准贷记卡 */
        int     iCardLevel;                     /* 卡种类 0-普卡 1-金卡 */
    EXEC SQL END DECLARE SECTION;

    T_CARDS     *ptCard;

    /* 声明游标 */
    EXEC SQL
        DECLARE cur_Card CURSOR FOR
        SELECT bank_name, bank_id, card_name, card_id, card_no_len, card_site2,
               exp_site2, pan_site3, card_site3, exp_site3, card_type, card_level
        FROM cards;
    if(SQLCODE)
    {
        WriteLog(ERROR, "声明游标cur_Card失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    /* 打开游标 */
    EXEC SQL OPEN cur_Card;
    if(SQLCODE)
    {
        WriteLog(ERROR, "打开游标cur_Card失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    gpShmEpay->iCardNum = 0;

    while(1)
    {
        memset(szBankName, 0, sizeof(szBankName));
        memset(szBankID, 0, sizeof(szBankID));
        memset(szCardName, 0, sizeof(szCardName));
        memset(szCardID, 0, sizeof(szCardID));
        memset(szCardType, 0, sizeof(szCardType));

        EXEC SQL
            FETCH cur_Card
            INTO :szBankName, :szBankID, :szCardName, :szCardID, :iCardNoLen, :iCardSite2,
                 :iExpSite2, :iPanSite3, :iCardSite3, :iExpSite3, :szCardType, :iCardLevel;
        if(SQLCODE == SQL_NO_RECORD)
        {
            EXEC SQL CLOSE cur_Card;

            break;
        }
        else if(SQLCODE)
        {
            WriteLog(ERROR, "读取游标cur_Card失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            EXEC SQL CLOSE cur_Card;

            return FAIL;
        }

        /* 赋值 */
        ptCard = gpShmEpay->tCards + gpShmEpay->iCardNum;

        DelTailSpace(szBankName);
        DelTailSpace(szBankID);
        DelTailSpace(szCardName);
        DelTailSpace(szCardID);

        strcpy(ptCard->szBankName, szBankName);
        strcpy(ptCard->szBankId, szBankID);
        strcpy(ptCard->szCardName, szCardName);
        strcpy(ptCard->szCardId, szCardID);
        ptCard->iCardNoLen = iCardNoLen;
        ptCard->iCardSite2 = iCardSite2;
        ptCard->iExpSite2 = iExpSite2;
        ptCard->iPanSite3 = iPanSite3;
        ptCard->iCardSite3 = iCardSite3;
        ptCard->iExpSite3 = iExpSite3;
        ptCard->szCardType[0] = szCardType[0];
        ptCard->iCardLevel = iCardLevel;

        (gpShmEpay->iCardNum)++;

        /* 判断是否超过最大卡表记录 */
        if(gpShmEpay->iCardNum >= MAX_CARD_NUM)
        {
            WriteLog(ERROR, "Cards too much MAX_CARD_NUM:[%d]", MAX_CARD_NUM);

            EXEC SQL CLOSE cur_Card;

            return FAIL;
        }
    }

    return SUCC;
}

/****************************************************************
** 功    能：载入本地卡表数据到共享内存
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/19
** 调用说明：
**
** 修改日志：
****************************************************************/
static int _LoadLocalCard()
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szLocalCardID[16+1];            /* 卡标识串 */
        char    szLocalCardName[40+1];          /* 卡名称 */
        int     iLocalCardNoLen;                /* 卡号长度 */
        char    szLocalCardType[1+1];           /* 卡类型 0-借记卡 1-贷记卡 2-任意卡 3-准贷记卡 */
    EXEC SQL END DECLARE SECTION;

    T_LOCALCARDS     *ptLocalCard;

    /* 声明游标 */
    EXEC SQL
        DECLARE cur_LocalCard CURSOR FOR
        SELECT card_id, card_name, card_no_len, card_type
        FROM local_card;
    if(SQLCODE)
    {
        WriteLog(ERROR, "声明游标cur_LocalCard失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    /* 打开游标 */
    EXEC SQL OPEN cur_LocalCard;
    if(SQLCODE)
    {
        WriteLog(ERROR, "打开游标cur_LocalCard失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    gpShmEpay->iLocalCardNum = 0;

    while(1)
    {
        memset(szLocalCardID, 0, sizeof(szLocalCardID));
        memset(szLocalCardName, 0, sizeof(szLocalCardName));
        memset(szLocalCardType, 0, sizeof(szLocalCardType));

        EXEC SQL
            FETCH cur_LocalCard
            INTO :szLocalCardID, :szLocalCardName, :iLocalCardNoLen, :szLocalCardType;
        if(SQLCODE == SQL_NO_RECORD)
        {
            EXEC SQL CLOSE cur_LocalCard;

            break;
        }
        else if(SQLCODE)
        {
            WriteLog(ERROR, "读取游标cur_LocalCard失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            EXEC SQL CLOSE cur_LocalCard;

            return FAIL;
        }

        /* 赋值 */
        ptLocalCard = gpShmEpay->tLocalCards + gpShmEpay->iLocalCardNum;

        DelTailSpace(szLocalCardID);
        DelTailSpace(szLocalCardName);

        strcpy(ptLocalCard->szCardId, szLocalCardID);
        strcpy(ptLocalCard->szCardName, szLocalCardName);
        ptLocalCard->iCardNoLen = iLocalCardNoLen;
        ptLocalCard->szCardType[0] = szLocalCardType[0];

        (gpShmEpay->iLocalCardNum)++;

        /* 判断是否超过最大卡表记录 */
        if(gpShmEpay->iLocalCardNum >= MAX_CARD_NUM)
        {
            WriteLog(ERROR, "Local Cards too much MAX_CARD_NUM:[%d]", MAX_CARD_NUM);

            EXEC SQL CLOSE cur_LocalCard;

            return FAIL;
        }
    }

    return SUCC;
}

/****************************************************************
** 功    能：删除EPAY共享内存
** 输入参数：
**        无
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int RmEpayShm()
{
    WriteLog(TRACE, "删除EPAY共享内存开始");

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    /* 删除共享内存 */
    if(RmShm(giShmEpayID) != SUCC)
    {
        WriteLog(ERROR, "删除EPAY共享内存失败");
    }

    WriteLog(TRACE, "删除EPAY共享内存成功");

    gpShmEpay = NULL;

    return SUCC;
}

/****************************************************************
** 功    能：根据索引获取交易数据共享内存地址
** 输入参数：
**        iTransDataIdx             交易数据索引
** 输出参数：
**        无     
** 返 回 值：
**        非NULL                    结构指针
**        NULL                      获取失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/03
** 调用说明：
**
** 修改日志：
****************************************************************/
T_App* GetAppAddress(int iTransDataIdx)
{
    if(iTransDataIdx < 0 || iTransDataIdx >= MAX_TRANS_DATA_INDEX)
    {
        WriteLog(ERROR, "交易数据索引值错误!iTransDataIdx:[%d] MAX_TRANS_DATA_INDEX:[%d]",
                 iTransDataIdx, MAX_TRANS_DATA_INDEX);

        return NULL;
    }

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return NULL;
    }

    return gpShmEpay->tApp + iTransDataIdx;
}

/****************************************************************
** 功    能：根据索引获取交易数据指针
** 输入参数：
**        iTransDataIdx             交易数据索引
** 输出参数：
**        ptApp                     app结构指针     
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetApp(int iTransDataIdx, T_App *ptApp)
{
    if(iTransDataIdx < 0 || iTransDataIdx >= MAX_TRANS_DATA_INDEX)
    {
        WriteLog(ERROR, "交易数据索引值错误!iTransDataIdx:[%d] MAX_TRANS_DATA_INDEX:[%d]",
                 iTransDataIdx, MAX_TRANS_DATA_INDEX);

        return FAIL;
    }

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    memcpy(ptApp, gpShmEpay->tApp + iTransDataIdx, sizeof(T_App));

    return SUCC;
}

/****************************************************************
** 功    能：根据索引设置交易数据到共享内存
** 输入参数：
**        iTransDataIdx             交易数据索引
** 输出参数：
**        ptApp                     app结构指针     
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/30
** 调用说明：
**
** 修改日志：
****************************************************************/
int SetApp(int iTransDataIdx, T_App *ptApp)
{
    if(iTransDataIdx < 0 || iTransDataIdx >= MAX_TRANS_DATA_INDEX)
    {
        WriteLog(ERROR, "交易数据索引值错误!iTransDataIdx:[%d] MAX_TRANS_DATA_INDEX:[%d]",
                 iTransDataIdx, MAX_TRANS_DATA_INDEX);

        return FAIL;
    }

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    memcpy(gpShmEpay->tApp + iTransDataIdx, ptApp, sizeof(T_App));

    return SUCC;
}

/****************************************************************
** 功    能：根据磁道获取转出卡的卡号、卡类型、发卡行等信息
** 输入参数：
**        ptApp->szTrack2           二磁道
**        ptApp->szTrack3           三磁道
**        ptApp->szAcqBankId        收单行ID
** 输出参数：
**        ptApp->szPan              转出卡卡号
**        ptApp->szExpireDate       转出卡有效期
**        ptApp->szOutBankId        转出卡发卡行ID
**        ptApp->szOutBankName      转出卡发卡行名称
**        ptApp->szOutCardName      转出卡名称
**        ptApp->cOutCardType       转出卡类型
**        ptApp->iOutCardLevel      转出卡级别
**        ptApp->iOutCardBelong     转出卡归属
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/29
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetCardType(T_App *ptApp)
{
    T_CARDS     tCard;
    int         iCardBelong;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    if(strlen(ptApp->szTrack2) > 0 &&
       _GetCard(ptApp->szTrack2, CARD_TRACK2, ptApp->szAcqBankId, &tCard, &iCardBelong) == SUCC)
    {
        /* 卡号 */
        memcpy(ptApp->szPan, ptApp->szTrack2+tCard.iCardSite2, tCard.iCardNoLen);

        /* 默认磁道信息中等号后四位为有效期 */
        memcpy(ptApp->szExpireDate, ptApp->szTrack2+tCard.iCardSite2+tCard.iCardNoLen+1, 4);

        /* 发卡行ID */
        strcpy(ptApp->szOutBankId, tCard.szBankId);

        /* 发卡行名称 */
        strcpy(ptApp->szOutBankName, tCard.szBankName);

        /* 卡名称 */
        strcpy(ptApp->szOutCardName, tCard.szCardName);

        /* 卡类型 */
        ptApp->cOutCardType = tCard.szCardType[0];

        /* 卡级别 */
        ptApp->iOutCardLevel = tCard.iCardLevel;

        /* 卡归属 */
        ptApp->iOutCardBelong = iCardBelong;

        return SUCC;
    }
    else if(strlen(ptApp->szTrack3) > 0 &&
       _GetCard(ptApp->szTrack3, CARD_TRACK3, ptApp->szAcqBankId, &tCard, &iCardBelong) == SUCC)
    {
        /* 卡号 */
        memcpy(ptApp->szPan, ptApp->szTrack3+tCard.iCardSite3, tCard.iCardNoLen);

        /* 默认磁道信息中等号后四位为有效期 */
        memcpy(ptApp->szExpireDate, ptApp->szTrack3+tCard.iCardSite3+tCard.iCardNoLen+1, 4);

        /* 发卡行ID */
        strcpy(ptApp->szOutBankId, tCard.szBankId);

        /* 发卡行名称 */
        strcpy(ptApp->szOutBankName, tCard.szBankName);

        /* 卡名称 */
        strcpy(ptApp->szOutCardName, tCard.szCardName);

        /* 卡类型 */
        ptApp->cOutCardType = tCard.szCardType[0];

        /* 卡级别 */
        ptApp->iOutCardLevel = tCard.iCardLevel;

        /* 卡归属 */
        ptApp->iOutCardBelong = iCardBelong;

        return SUCC;
    }

    /* 登记未识别卡号 */
    _InsertUnknowCard(ptApp->szTrack2, ptApp->szTrack3);

    return FAIL;
}

/****************************************************************
** 功    能：登记未识别卡
** 输入参数：
**        szTrack2                  二磁道数据
**        szTrack3                  三磁道数据
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/30
** 调用说明：
**
** 修改日志：
****************************************************************/
static int _InsertUnknowCard(char *szTrack2, char *szTrack3)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szHostTrack2[40+1];
        char    szHostTrack3[107+1];
        char    szDate[8+1];
    EXEC SQL END DECLARE SECTION;

    memset(szHostTrack2, 0, sizeof(szHostTrack2));
    memset(szHostTrack3, 0, sizeof(szHostTrack3));
    memset(szDate, 0, sizeof(szDate));

    strcpy(szHostTrack2, szTrack2);
    strcpy(szHostTrack3, szTrack3);
    GetSysDate(szDate);

    BeginTran();

    EXEC SQL
        INSERT INTO unknown_card (track2, track3, s_date, flag)
        VALUES (:szHostTrack2, :szHostTrack3, :szDate, '00');
    if(SQLCODE)
    {
        WriteLog(ERROR, "插入未识别卡表失败(主键冲突错误可忽略)!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        RollbackTran();

        return FAIL;
    }

    CommitTran();

    return SUCC;
}

/****************************************************************
** 功    能：根据卡号获取转入卡的卡号、卡类型、发卡行等信息
** 输入参数：
**        ptApp->szAccount2         转入卡号
** 输出参数：
**        ptApp->szInBankId         转入卡发卡行ID
**        ptApp->szInBankName       转入卡发卡行名称
**        ptApp->cInCardType        转入卡卡类型
**        ptApp->iInCardBelong      转入卡归属
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/29
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetAcctType(T_App *ptApp)
{
    T_CARDS     tCard;
    int         iCardBelong;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    if(_GetCard(ptApp->szAccount2, CARD_PAN, ptApp->szAcqBankId, &tCard, &iCardBelong) != SUCC)
    {
        WriteLog(ERROR, "获取转入卡信息失败!");

        return FAIL;
    }

    strcpy(ptApp->szInBankId, tCard.szBankId);
    strcpy(ptApp->szInBankName, tCard.szBankName);
    ptApp->cInCardType = tCard.szCardType[0];
    ptApp->iInCardBelong = iCardBelong;

    return SUCC;
}

/****************************************************************
** 功    能：根据卡号从共享内存中查询卡信息
** 输入参数：
**        szCardNo                  卡号
**        iType                     输入卡号类型 1:卡号 2:二磁道 3:三磁道
**        szBankID                  本行行号
** 输出参数：
**        ptCard                    卡信息
**        piCardBelong              卡归属
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/29
** 调用说明：
**
** 修改日志：
****************************************************************/
static int _GetCard(char *szCardNo, int iType, char *szBankID, T_CARDS *ptCard, int *piCardBelong)
{
    int             i, j;
    int             iCardSite;
    int             iCardNoLen;
    T_CARDS         *ptCardShm;
    T_LOCALCARDS    *ptLocalCard;

    for(i=0;i<gpShmEpay->iCardNum;i++)
    {
        ptCardShm = gpShmEpay->tCards + i;
        
        switch(iType)
        {
            case CARD_PAN:
                iCardSite = 0;
                break;
            case CARD_TRACK2:
                iCardSite = ptCardShm->iCardSite2;
                break;
            case CARD_TRACK3:
                iCardSite = ptCardShm->iCardSite3;
                break;
            default:
                WriteLog(ERROR, "输入类型未定义iType:[%d]", iType);
                return FAIL;
        }

        if(memcmp(szCardNo+iCardSite, ptCardShm->szCardId, strlen(ptCardShm->szCardId)) == 0)
        {
            /* 判断卡号长度 */
            for(j=iCardSite;j<strlen(szCardNo);j++)
            {
                if(szCardNo[j] == '=' || szCardNo[j] == 'D')
                {
                    break;
                }
            }
            iCardNoLen = j - iCardSite;

            if(ptCardShm->iCardNoLen != iCardNoLen)
            {
                continue;
            }

            memcpy(ptCard, ptCardShm, sizeof(T_CARDS));

            /* 判断卡归属 */
            if(strcmp(szBankID, ptCard->szBankId) != 0)
            {
                *piCardBelong = OTHER_BANK;
            }
            else
            {
                /* 检查本地卡表 */
                for(j=0;j<gpShmEpay->iLocalCardNum;j++)
                {
                    ptLocalCard = gpShmEpay->tLocalCards + j;

                    if(ptLocalCard->iCardNoLen == strlen(szCardNo) &&
                       memcmp(szCardNo, ptLocalCard->szCardId, strlen(ptLocalCard->szCardId)) == 0)
                    {
                        *piCardBelong = LOCAL_BANK_LOCAL_CITY;

                        return SUCC;
                    }
                }

                *piCardBelong = LOCAL_BANK_OTHER_CITY;
            }

            return SUCC;
        }
    }

    return FAIL;
}

/****************************************************************
** 功    能：设置接入通讯状态到共享内存
** 输入参数：
**        szIP                      接入IP
**        iPort                     接入端口号
**        lPid                      接入进程号
** 输出参数：
**        无   
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int SetAccessPid(char *szIP, int iPort, long lPid)
{
    int         i;
    int         iIPLen;
    T_ACCESS    *ptAccess;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    /* 信号量操作 */
    PSem(SEM_ACCESS_ID, 1);

    for(i=0;i<MAX_ACCESS_NUM;i++)
	{
	    ptAccess = gpShmEpay->tAccess + i;

		if(strlen(ptAccess->szIp) == 0)
		{
		    iIPLen = strlen(szIP)>sizeof(ptAccess->szIp)?sizeof(ptAccess->szIp):strlen(szIP);
		    memcpy(ptAccess->szIp, szIP, iIPLen);
			ptAccess->lPort = iPort;
			ptAccess->lPid = lPid;

			break;
		}
	}

    /* 信号量操作 */
    VSem(SEM_ACCESS_ID, 1);

	if(i == MAX_ACCESS_NUM)
	{
        WriteLog(ERROR, "IP:[%s] PORT:[%d]接入进程个数超限!MAX_ACCESS_NUM:[%d]",
                 szIP, iPort, MAX_ACCESS_NUM);

		return FAIL;
	}

    return SUCC;
}

/****************************************************************
** 功    能：根据IP、端口统计返回进程PID，并清空该记录
** 输入参数：
**        szIP                      接入IP
**        iPort                     接入端口号
** 输出参数：
**        无   
** 返 回 值：
**        >0                        进程PID
**        0                         未找到批评进程
** 作    者：
**        fengwei
** 日    期：
**        2012/11/29
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetAccessPid(char *szIP, int iPort)
{
    int         i;
    long        lPid;
    T_ACCESS    *ptAccess;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    for(i=0;i<MAX_ACCESS_NUM;i++)
	{
	    ptAccess = gpShmEpay->tAccess + i;

        if(strcmp(ptAccess->szIp, szIP) == 0 && ptAccess->lPort == iPort)
		{
		    lPid = ptAccess->lPid;
		    
		    memset(ptAccess->szIp, 0, sizeof(ptAccess->szIp));
		    ptAccess->lPort = 0;
		    ptAccess->lPid = 0;
		    
		    return lPid;
		}
	}

	return 0;
}

/****************************************************************
** 功    能：根据IP、端口统计接入通讯进程个数
** 输入参数：
**        szIP                      接入IP
**        iPort                     接入端口号
** 输出参数：
**        无   
** 返 回 值：
**        >0                        进程个数
**        0                         未找到匹配进程
** 作    者：
**        fengwei
** 日    期：
**        2012/11/29
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetAccessLinkNum(char *szIP, int iPort)
{
    int         i;
    int         iCount;
    T_ACCESS    *ptAccess;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    iCount = 0;

    for(i=0;i<MAX_ACCESS_NUM;i++)
	{
	    ptAccess = gpShmEpay->tAccess + i;

        if(strcmp(ptAccess->szIp, szIP) == 0 && ptAccess->lPort == iPort)
		{
		    iCount++;
		}
	}

	return iCount;
}

/****************************************************************
** 功    能：删除共享内存中接入通讯进程状态
** 输入参数：
**        lPid                      接入进程号
** 输出参数：
**        无   
** 返 回 值：
**        SUCC                      删除成功
**        FAIL                      删除失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int DelAccessPid(long lPid)
{
    int         i;
    int         iIPLen;
    T_ACCESS    *ptAccess;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    /* 信号量操作 */
    PSem(SEM_ACCESS_ID, 1);

    for(i=0;i<MAX_ACCESS_NUM;i++)
	{
	    ptAccess = gpShmEpay->tAccess + i;

		if(ptAccess->lPid == lPid)
		{
		    memset(ptAccess->szIp, 0, sizeof(ptAccess->szIp));
		    ptAccess->lPort = 0;
			ptAccess->lPid = 0;
		}
	}

    /* 信号量操作 */
    VSem(SEM_ACCESS_ID, 1);

    return;
}
	
/****************************************************************
** 功    能：设置后台通讯状态到共享内存
** 输入参数：
**        lMsgType                  后台通讯接收消息类型
**        iLinkNo                   多进多出链路编号
**        cClitNet                  客户端状态
**        cServNet                  服务端状态
** 输出参数：
**        无   
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int SetHost(long lMsgType, int iLinkNo, char cClitNet, char cServNet)
{
    int     i;
    T_HOST  *ptHost;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    /* 信号量操作 */
    PSem(SEM_HOST_ID, 1);

    for(i=0;i<MAX_HOST_NUM;i++)
	{
	    ptHost = gpShmEpay->tHost + i;

		if(ptHost->lMsgType == 0)
		{
		    ptHost->lMsgType = lMsgType;
		    ptHost->iLinkNo = iLinkNo;
		    ptHost->cClitNet = cClitNet;
		    ptHost->cServNet = cServNet;

			break;
		}
	}

    /* 信号量操作 */
    VSem(SEM_HOST_ID, 1);

	if(i == MAX_HOST_NUM)
	{
        WriteLog(ERROR, "lMsgType:[%d] 后台通讯个数超限!MAX_HOST_NUM:[%d]",
                 lMsgType, MAX_HOST_NUM);

		return FAIL;
	}

    return SUCC;
}

/****************************************************************
** 功    能：检查后台通讯链路状态
** 输入参数：
**        lMsgType                  后台通讯对应消息队列消息类型
** 输出参数：
**        无
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int ChkHostStatus(long lMsgType)
{
    int     i;
    T_HOST  *ptHost;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    for(i=0;i<MAX_HOST_NUM;i++)
	{
	    ptHost = gpShmEpay->tHost + i;

		if(ptHost->lMsgType == lMsgType)
		{
            if(ptHost->cClitNet == 'Y' && ptHost->cServNet == 'Y')
            {
                return SUCC;
            }
		}
	}

    return FAIL;
}

/****************************************************************
** 功    能：删除后台通讯状态
** 输入参数：
**        lMsgType                  后台通讯接收消息类型
**        iLinkNo                   多进多出链路编号
** 输出参数：
**        无   
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int DelHost(long lMsgType, int iLinkNo)
{
    int     i;
    T_HOST  *ptHost;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    /* 信号量操作 */
    PSem(SEM_HOST_ID, 1);

    for(i=0;i<MAX_HOST_NUM;i++)
	{
	    ptHost = gpShmEpay->tHost + i;

		if(ptHost->lMsgType == lMsgType && ptHost->iLinkNo == iLinkNo)
		{
		    ptHost->lMsgType = 0;
		    ptHost->iLinkNo = 0;
		    ptHost->cClitNet = 'N';
		    ptHost->cServNet = 'N';
		}
	}

    /* 信号量操作 */
    VSem(SEM_HOST_ID, 1);

    return;
}

/****************************************************************
** 功    能：获取交易数据索引号
** 输入参数：
**        iMaxAliveTime             索引最大占用时间
**                                  (当前时间与索引分配时间的差大于该参数，则该索引可以被重新分配)
** 输出参数：
**        无   
** 返 回 值：
**        >=0                       索引值
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/28
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetTransDataIndex(int iMaxAliveTime)
{
    int     i;
    T_TDI   *ptTDI;
    int     iIndex;
    long    lTime;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    iIndex = gpShmEpay->iCurTdi;

    ptTDI = gpShmEpay->tTdi + iIndex;

    time(&lTime);

    /* 信号量操作 */
    PSem(SEM_TDI_ID, 1);

    for(i=0;i<MAX_TRANS_DATA_INDEX;i++)
    {  
        if(ptTDI->lLastVisitTime == 0 || (lTime - ptTDI->lLastVisitTime) > iMaxAliveTime)
        {
            gpShmEpay->iCurTdi = iIndex+1==MAX_TRANS_DATA_INDEX?0:iIndex+1;

            ptTDI->lLastVisitTime = lTime;
            
            /* 信号量操作 */
            VSem(SEM_TDI_ID, 1);

            return iIndex;
        }

        iIndex++;

        /* 索引到达最大值时，从0重新开始 */
        if(iIndex == MAX_TRANS_DATA_INDEX)
        {
            iIndex = 0;
        }

        ptTDI = gpShmEpay->tTdi + iIndex;
    }

    /* 信号量操作 */
    VSem(SEM_TDI_ID, 1);

    WriteLog(ERROR, "所有索引已使用，无空闲索引可供分配!");

    return FAIL;
}

/****************************************************************
** 功    能：根据交易数据索引号设置访问时间
** 输入参数：
**        iTransDataIndex           交易数据索引
** 输出参数：
**        无   
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/11
** 调用说明：
**
** 修改日志：
****************************************************************/
int SetTdiTime(int iTransDataIdx)
{
    T_TDI   *ptTDI;
    long    lTime;
    
    if(iTransDataIdx < 0 || iTransDataIdx >= MAX_TRANS_DATA_INDEX)
    {
        WriteLog(ERROR, "交易数据索引值错误!iTransDataIdx:[%d] MAX_TRANS_DATA_INDEX:[%d]",
                 iTransDataIdx, MAX_TRANS_DATA_INDEX);

        return FAIL;
    }

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    ptTDI = gpShmEpay->tTdi + iTransDataIdx;

    time(&lTime);

    /* 信号量操作 */
    PSem(SEM_TDI_ID, 1);

    ptTDI->lLastVisitTime = lTime;

    /* 信号量操作 */
    VSem(SEM_TDI_ID, 1);

    return SUCC;
}

/****************************************************************
** 功    能：释放交易数据索引号
** 输入参数：
**        iTransDataIndex           交易数据索引
** 输出参数：
**        无   
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/11
** 调用说明：
**
** 修改日志：
****************************************************************/
int FreeTdi(int iTransDataIdx)
{
    T_TDI   *ptTDI;

    if(iTransDataIdx < 0 || iTransDataIdx >= MAX_TRANS_DATA_INDEX)
    {
        WriteLog(ERROR, "交易数据索引值错误!iTransDataIdx:[%d] MAX_TRANS_DATA_INDEX:[%d]",
                 iTransDataIdx, MAX_TRANS_DATA_INDEX);

        return FAIL;
    }

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    ptTDI = gpShmEpay->tTdi + iTransDataIdx;

    /* 信号量操作 */
    PSem(SEM_TDI_ID, 1);

    ptTDI->lLastVisitTime = 0;

    /* 信号量操作 */
    VSem(SEM_TDI_ID, 1);

    return SUCC;
}

/****************************************************************
** 功    能：打印共享内存数据(调试使用)
** 输入参数：
**        iDebug                    调试类型
**                                  0:所有数据
**                                  1:卡表Cards
**                                  2:本地卡表LocalCards
**                                  3:接入Access
**                                  4:后台Host
**                                  5:交易数据索引TDI
**
** 输出参数：
**        无   
** 返 回 值：
**        SUCC                      成功
**        FAIL                      失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/29
** 调用说明：
**
** 修改日志：
****************************************************************/
int DebugEpayShm(int iDebug)
{
    int             i;
    T_CARDS         *ptCards;
    T_LOCALCARDS    *ptLocalCards;
    T_ACCESS        *ptAccess;
    T_HOST          *ptHost;
    T_TDI           *ptTdi;

    /* 获取共享内存 */
    if(GetEpayShm() != SUCC)
    {
        WriteLog(ERROR, "获取EPAY共享内存失败!");

        return FAIL;
    }

    /* 卡表 */
    if(iDebug == 0 || iDebug == 1)
    {
        printf("Cards:\n");
        printf("iCardNum:[%d]\n", gpShmEpay->iCardNum);
        for(i=0;i<MAX_CARD_NUM;i++)
        {
            ptCards = gpShmEpay->tCards + i;
    
            if(strlen(ptCards->szCardId) > 0)
            {
                printf("Card[%d].szBankName:[%s]\n", i, ptCards->szBankName);
                printf("Card[%d].szBankId:[%s]\n", i, ptCards->szBankId);
                printf("Card[%d].szCardName:[%s]\n", i, ptCards->szCardName);
                printf("Card[%d].szCardId:[%s]\n", i, ptCards->szCardId);
                printf("Card[%d].iCardNoLen:[%d]\n", i, ptCards->iCardNoLen);
                printf("Card[%d].iCardSite2:[%d]\n", i, ptCards->iCardSite2);
                printf("Card[%d].iExpSite2:[%d]\n", i, ptCards->iExpSite2);
                printf("Card[%d].iPanSite3:[%d]\n", i, ptCards->iPanSite3);
                printf("Card[%d].iCardSite3:[%d]\n", i, ptCards->iCardSite3);
                printf("Card[%d].iExpSite3:[%d]\n", i, ptCards->iExpSite3);
                printf("Card[%d].szCardType:[%s]\n", i, ptCards->szCardType);
                printf("Card[%d].iCardLevel:[%d]\n", i, ptCards->iCardLevel);
            }
        }
        printf("\n");
    }

    /* 本地卡表 */
    if(iDebug == 0 || iDebug == 2)
    {
        printf("LocalCards:\n");
        printf("iLocalCardNum:[%d]\n", gpShmEpay->iLocalCardNum);
        for(i=0;i<MAX_CARD_NUM;i++)
        {
            ptLocalCards = gpShmEpay->tLocalCards + i;
    
            if(strlen(ptLocalCards->szCardId) > 0)
            {
                printf("LocalCard[%d].szCardId:[%s]\n", i, ptLocalCards->szCardId);
                printf("LocalCard[%d].szCardName:[%s]\n", i, ptLocalCards->szCardName);
                printf("LocalCard[%d].iCardNoLen:[%d]\n", i, ptLocalCards->iCardNoLen);
                printf("LocalCard[%d].szCardType:[%s]\n", i, ptLocalCards->szCardType);
            }
        }
        printf("\n");
    }

    if(iDebug == 0 || iDebug == 3)
    {
        printf("Access:\n");
        for(i=0;i<MAX_ACCESS_NUM;i++)
        {  
            ptAccess = gpShmEpay->tAccess + i;
    
            if(strlen(ptAccess->szIp) > 0)
            {
                printf("Access[%d].szIp:[%s]\n", i, ptAccess->szIp);
                printf("Access[%d].lPort:[%d]\n", i, ptAccess->lPort);
                printf("Access[%d].lPid:[%d]\n", i, ptAccess->lPid);
            }
        }
        printf("\n");
    }

    if(iDebug == 0 || iDebug == 4)
    {
        printf("Host:\n");
        for(i=0;i<MAX_HOST_NUM;i++)
        {  
            ptHost = gpShmEpay->tHost + i;
    
            if(ptHost->lMsgType > 0)
            {
                printf("Host[%d].lMsgType:[%d]\n", i, ptHost->lMsgType);
                printf("Host[%d].iLinkNo:[%d]\n", i, ptHost->iLinkNo);
                printf("Host[%d].cClitNet:[%c]\n", i, ptHost->cClitNet);
                printf("Host[%d].cServNet:[%c]\n", i, ptHost->cServNet);
            }
        }
        printf("\n");
    }

    if(iDebug == 0 || iDebug == 5)
    {
        printf("TransDataIndex:\n");
        printf("iCurTdi:[%d]\n", gpShmEpay->iCurTdi);
        for(i=0;i<MAX_TRANS_DATA_INDEX;i++)
        {  
            ptTdi = gpShmEpay->tTdi + i;
    
            if(ptTdi->lLastVisitTime > 0)
            {
                printf("ptTdi[%d].lLastVisitTime:[%ld]\n", i, ptTdi->lLastVisitTime);
            }
        }
        printf("\n");
    }

    return;
}
