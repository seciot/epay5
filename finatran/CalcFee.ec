/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台金融交易处理模块 手续费计算
** 创 建 人：冯炜
** 创建日期：2012-11-08
**
** $Revision: 1.3 $
** $Log: CalcFee.ec,v $
** Revision 1.3  2013/02/21 06:35:00  fengw
**
** 1、修改手续费金额字符串格式。
**
** Revision 1.2  2012/12/04 01:24:28  fengw
**
** 1、替换ErrorLog为WriteLog。
**
** Revision 1.1  2012/11/23 09:09:16  fengw
**
** 金融交易处理模块初始版本
**
** Revision 1.2  2012/11/22 08:58:23  fengw
**
** 1、修正手续费率查询SQL语句
**
** Revision 1.1  2012/11/21 07:20:46  fengw
**
** 金融交易处理模块初始版本
**
*******************************************************************/

#define _EXTERN_

#include "finatran.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    char    szDeptNo[15+1];                 /* 机构号 */
    char    szDeptDetail[70+1];             /* 机构层级信息 */
    char    szShopNo[15+1];                 /* 商户号 */
    int     iTransType;                     /* 交易类型 */
    int     iHostFeeType;                   /* 费率种类 */
    int     iCardType;                      /* 卡种类 */
    double  dAmountBegin;                   /* 费率区间上限金额 */
    double  dAmount;                        /* 交易金额 */
    int     iFeeRate;                       /* 手续费率 */
    double  dFeeBase;                       /* 基础费用 */
    double  dFeeMin;                        /* 最低手续费 */
    double  dFeeMax;                        /* 最高手续费 */
EXEC SQL END DECLARE SECTION;

/* 手续费金额 */
static double gdFeeAmount;

static int _CalcByRate();
static int _CalcByInterval();

/****************************************************************
** 功    能：计算手续费
** 输入参数：
**        ptApp                     app结构指针
** 输出参数：
**        ptApp->szAddiAmount       手续费金额
** 返 回 值：
**        SUCC                      计算手续费成功
**        FAIL                      计算手续费失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/09
** 调用说明：
**
** 修改日志：
****************************************************************/
int CalcFee(T_App *ptApp, int iFeeType)
{
    /* 变量赋值 */
    memset(szDeptNo, 0, sizeof(szDeptNo));
    memset(szDeptDetail, 0, sizeof(szDeptDetail));
    memset(szShopNo, 0, sizeof(szShopNo));

    strcpy(szDeptNo, ptApp->szDeptNo);
    strcpy(szDeptDetail, ptApp->szDeptDetail);
    strcpy(szShopNo, ptApp->szShopNo);
    iTransType = ptApp->iTransType;
    iHostFeeType = iFeeType;
    iCardType = ptApp->iOutCardLevel;
    dAmount = atof(ptApp->szAmount) / 100;

    switch(giFeeCalcType)
    {
        case FEE_CALC_NOT:
            gdFeeAmount = 0.0f;
            break;
        case FEE_CALC_RATE:
            if(_CalcByRate(ptApp) != SUCC)
            {
                return FAIL;
            }
            break;
        case FEE_CALC_INTERVAL:
            if(_CalcByInterval(ptApp) != SUCC)
            {
                return FAIL;
            }
            break;
        default:
            strcpy(ptApp->szRetCode, ERR_UNDEF_FEECALCTYPE);
            WriteLog(ERROR, "手续费计算方式[%d]未定义!", giFeeCalcType);
            return FAIL;
    }

    /* 保存手续费金额到app结构 */
    sprintf(ptApp->szAddiAmount, "%d", (int)(gdFeeAmount * 100));

    return SUCC;
}

/****************************************************************
** 功    能：按比率计算手续费
** 输入参数：
**        ptApp                 app结构指针
** 输出参数：
**        gdFeeAmount           手续费金额
** 返 回 值：
**        SUCC                  计算手续费成功
**        FAIL                  计算手续费失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/09
** 调用说明：
**
** 修改日志：
****************************************************************/
static int _CalcByRate(T_App *ptApp)
{
    /* 查询商户手续费率表 */
    EXEC SQL
        SELECT fee_rate, fee_base, fee_min, fee_max
        INTO :iFeeRate, :dFeeBase, :dFeeMin, :dFeeMax
        FROM
            (SELECT fee_rate, fee_base, fee_min, fee_max
            FROM shop_fee_rate
            WHERE shop_no = :szShopNo AND (trans_type = :iTransType OR trans_type = 0) AND
                  fee_type = :iHostFeeType AND card_type = :iCardType AND
                  amount_begin < :dAmount
            ORDER BY trans_type DESC, amount_begin DESC) WHERE ROWNUM = 1;
    if(SQLCODE == SQL_NO_RECORD)
    {
        /* 查询机构手续费率表 */
        EXEC SQL
            SELECT fee_rate, fee_base, fee_min, fee_max
            INTO :iFeeRate, :dFeeBase, :dFeeMin, :dFeeMax
            FROM
                (SELECT fee_rate, fee_base, fee_min, fee_max
                FROM dept_fee_rate
                WHERE (trans_type = :iTransType OR trans_type = 0) AND fee_type = :iHostFeeType AND
                       card_type = :iCardType AND amount_begin < :dAmount AND
                       INSTR(:szDeptDetail, dept_detail) = 1
                ORDER BY LENGTH(dept_detail) DESC, trans_type DESC, amount_begin DESC) WHERE ROWNUM = 1;
        if(SQLCODE == SQL_NO_RECORD)
        {
            strcpy(ptApp->szRetCode, ERR_NO_FEE_RECORD);

            WriteLog(ERROR, "机构[%s] 商户[%s] 交易类型[%d] 费率种类[%d] 卡种[%d] 手续费率未定义!",
                     szDeptDetail, szShopNo, iTransType, iHostFeeType, iCardType);

            return FAIL;
        }
        else if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "查询 机构[%s] 交易类型[%d] 费率种类[%d] 卡种[%d] 手续费率失败!SQLCODE=%d SQLERR=%s",
                     szDeptDetail, iTransType, iHostFeeType, iCardType, SQLCODE, SQLERR);

            return FAIL;
        }
    }
    else if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询 商户[%s] 交易类型[%d] 费率种类[%d] 卡种[%d] 手续费率失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, iTransType, iHostFeeType, iCardType, SQLCODE, SQLERR);

        return FAIL;
    }

    /* 计算手续费 */
    gdFeeAmount = dFeeBase +  dAmount * iFeeRate / 10000;

    if((dFeeMin - gdFeeAmount) > 0.001)
    {
        gdFeeAmount = dFeeMin;
    }

    if(dFeeMax > 0.001 && (gdFeeAmount - dFeeMax) > 0.001)
    {
        gdFeeAmount = dFeeMax;
    }

    return SUCC;
}

/****************************************************************
** 功    能：按区间计算手续费
** 输入参数：
**        ptApp                 app结构指针
** 输出参数：
**        gdFeeAmount           手续费金额
** 返 回 值：
**        SUCC                  计算手续费成功
**        FAIL                  计算手续费失败
** 作    者：
**        fengwei
** 日    期：
**        2012/11/09
** 调用说明：
**
** 修改日志：
****************************************************************/
static int _CalcByInterval(T_App *ptApp)
{
    int    iFlag;
    double dTmpAmount, dTmpFeeAmount;

    /* 默认去商户手续费定义 */
    iFlag = SHOP_FEE_CONF;

    /* 查找手续费定义 */
    /* 查询商户手续费率表 */
    EXEC SQL
        SELECT trans_type INTO :iTransType
        FROM
            (SELECT trans_type
            FROM shop_fee_rate
            WHERE shop_no = :szShopNo AND (trans_type = :iTransType OR trans_type = 0) AND
                  fee_type = :iHostFeeType AND card_type = :iCardType AND
                  amount_begin < :dAmount
            ORDER BY trans_type DESC, amount_begin DESC) WHERE ROWNUM = 1;
    if(SQLCODE == SQL_NO_RECORD)
    {
        /* 查询机构手续费率表 */
        EXEC SQL
            SELECT dept_no, trans_type
            INTO :szDeptNo, :iTransType
            FROM
                (SELECT dept_no, trans_type
                FROM dept_fee_rate
                WHERE (trans_type = :iTransType OR trans_type = 0) AND fee_type = :iHostFeeType AND
                      card_type = :iCardType AND amount_begin < :dAmount AND
                INSTR(:szDeptDetail, dept_detail) = 1
            ORDER BY LENGTH(dept_detail) DESC, trans_type DESC, amount_begin DESC) WHERE ROWNUM = 1;
        if(SQLCODE == SQL_NO_RECORD)
        {
            strcpy(ptApp->szRetCode, ERR_NO_FEE_RECORD);

            WriteLog(ERROR, "机构[%s] 商户[%s] 交易类型[%d] 费率种类[%d] 卡种[%d] 手续费率未定义!",
                     szDeptDetail, szShopNo, iTransType, iHostFeeType, iCardType);

            return FAIL;
        }
        else if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "查询 机构[%s] 交易类型[%d] 费率种类[%d] 卡种[%d] 手续费率失败!SQLCODE=%d SQLERR=%s",
                     szDeptDetail, iTransType, iHostFeeType, iCardType, SQLCODE, SQLERR);

            return FAIL;
        }

        iFlag = DEPT_FEE_CONF;
    }
    else if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "查询 商户[%s] 交易类型[%d] 费率种类[%d] 卡种[%d] 手续费率失败!SQLCODE=%d SQLERR=%s",
                 szShopNo, iTransType, iHostFeeType, iCardType, SQLCODE, SQLERR);

        return FAIL;
    }

    /* 保存交易金额作为第一个区间手续费计算金额 */
    dTmpAmount = dAmount;

    /* 通过游标查询计算手续费 */
    if(iFlag == SHOP_FEE_CONF)
    {
        /* 声明游标 */
        EXEC SQL
            DECLARE cur_shop_fee_rate CURSOR FOR
            SELECT amount_begin, fee_rate, fee_base, fee_min, fee_max
            FROM shop_fee_rate
            WHERE shop_no = :szShopNo AND trans_type = :iTransType AND
                  fee_type = :iHostFeeType AND card_type = :iCardType AND
                  amount_begin < :dAmount
            ORDER BY amount_begin DESC;
        if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "声明游标cur_shop_fee_rate失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            return FAIL;
        }

        /* 打开游标 */
        EXEC SQL OPEN cur_shop_fee_rate;
        if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "打开游标cur_shop_fee_rate失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            return FAIL;
        }

        /* 循环读取费率，计算手续费 */
        while(1)
        {
            EXEC SQL
                FETCH cur_shop_fee_rate
                INTO :dAmountBegin, :iFeeRate, :dFeeBase, :dFeeMin, :dFeeMax;
            if(SQLCODE == SQL_NO_RECORD)
            {
                EXEC SQL CLOSE cur_shop_fee_rate;

                break;
            }
            else if(SQLCODE)
            {
                strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

                WriteLog(ERROR, "读取游标cur_shop_fee_rate失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

                EXEC SQL CLOSE cur_shop_fee_rate;

                return FAIL;
            }

            /* 计算该区间手续费 */
            dTmpFeeAmount = dFeeBase + (dTmpAmount-dAmountBegin) * iFeeRate / 10000;

            if((dFeeMin - dTmpFeeAmount) > 0.001)
            {
                dTmpFeeAmount = dFeeMin;
            }

            if(dFeeMax > 0.001 && (dTmpFeeAmount - dFeeMax) > 0.001)
            {
                dTmpFeeAmount = dFeeMax;
            }

            /* 累加至手续费金额 */
            gdFeeAmount += dTmpFeeAmount;

            /* 下一区间手续费计算金额 */
            dTmpAmount = dAmountBegin;
        }
    }
    else
    {
        /* 声明游标 */
        EXEC SQL
            DECLARE cur_dept_fee_rate CURSOR FOR
            SELECT amount_begin, fee_rate, fee_base, fee_min, fee_max
            FROM dept_fee_rate
            WHERE dept_no = :szDeptNo AND trans_type = :iTransType AND
                  fee_type = :iHostFeeType AND card_type = :iCardType AND
                  amount_begin < :dAmount
            ORDER BY amount_begin DESC;
        if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "声明游标cur_dept_fee_rate失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            return FAIL;
        }

        /* 打开游标 */
        EXEC SQL OPEN cur_dept_fee_rate;
        if(SQLCODE)
        {
            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "打开游标cur_dept_fee_rate失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            return FAIL;
        }

        /* 循环读取费率，计算手续费 */
        while(1)
        {
            EXEC SQL
                FETCH cur_dept_fee_rate
                INTO :dAmountBegin, :iFeeRate, :dFeeBase, :dFeeMin, :dFeeMax;
            if(SQLCODE == SQL_NO_RECORD)
            {
                EXEC SQL CLOSE cur_dept_fee_rate;

                break;
            }
            else if(SQLCODE)
            {
                strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

                WriteLog(ERROR, "读取游标cur_dept_fee_rate失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

                EXEC SQL CLOSE cur_dept_fee_rate;

                return FAIL;
            }

            /* 计算该区间手续费 */
            dTmpFeeAmount = dFeeBase + (dTmpAmount-dAmountBegin) * iFeeRate / 10000;

            if((dFeeMin - dTmpFeeAmount) > 0.001)
            {
                dTmpFeeAmount = dFeeMin;
            }

            if(dFeeMax > 0.001 && (dTmpFeeAmount - dFeeMax) > 0.001)
            {
                dTmpFeeAmount = dFeeMax;
            }

            /* 累加至手续费金额 */
            gdFeeAmount += dTmpFeeAmount;

            /* 下一区间手续费计算金额 */
            dTmpAmount = dAmountBegin;
        }
    }

    return SUCC;
}
