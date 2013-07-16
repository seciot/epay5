/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块 更新操作提示
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.5 $
** $Log: DownOperation.ec,v $
** Revision 1.5  2012/12/26 08:33:58  fengw
**
** 1、修正SQL语句中字段名错误。
**
** Revision 1.4  2012/12/25 07:02:05  fengw
**
** 1、修正按机构更新类型UPDATE语句。
**
** Revision 1.3  2012/12/21 02:05:32  fengw
**
** 1、将文件格式从DOS转为UNIX。
**
** Revision 1.2  2012/12/21 02:04:03  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#define _EXTERN_

#include "comweb.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
    char    szBitmap[64+1];             /* 更新信息 */
    char    szDeptDetail[70+1];         /* 机构层级信息 */
    char    szShopNo[15+1];             /* 商户号 */
    char    szPosNo[15+1];              /* 终端号 */
    int     iAppType;                   /* 应用类型 */
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：更新操作提示
** 输入参数：
**        ptApp                 app结构指针
** 输出参数：
**        无
** 返 回 值：
**        SUCC                  成功
**        FAIL                  失败
** 作    者：
**        fengwei
** 日    期：
**        2012/12/18
** 调用说明：
**
** 修改日志：
****************************************************************/
int DownOperation(T_App *ptApp)
{
    memset(szBitmap, 0, sizeof(szBitmap));
    memset(szShopNo, 0, sizeof(szShopNo));
    memset(szPosNo, 0, sizeof(szPosNo));
    memset(szDeptDetail, 0, sizeof(szDeptDetail));

    CompressBitmap(szBitmap, gszBitmap, 256);

    BeginTran();

	switch(giDownType)
	{
        /* 更新指定终端 */
        case DOWN_SPECIFY_POS:
            strcpy(szShopNo, ptApp->szShopNo);
            strcpy(szPosNo, ptApp->szPosNo);

            EXEC SQL
                UPDATE terminal SET down_operate = 'Y', operate_bitmap = :szBitmap
                WHERE shop_no = :szShopNo AND pos_no = :szPosNo;
            break;
        /* 更新指定商户终端 */
        case DOWN_SPECIFY_SHOP:
            strcpy(szShopNo, ptApp->szShopNo);

            EXEC SQL
                UPDATE terminal SET down_operate = 'Y', operate_bitmap = :szBitmap
                WHERE shop_no = :szShopNo;
            break;
        /* 更新所有终端 */
        case DOWN_ALL:
            EXEC SQL
                UPDATE terminal SET down_operate = 'Y', operate_bitmap = :szBitmap;
            break;
        /* 更新指定应用类型终端 */
        case DOWN_SPECIFY_TYPE:
            iAppType = atoi(ptApp->szShopNo);

            EXEC SQL
                UPDATE terminal SET down_operate = 'Y', operate_bitmap = :szBitmap
                WHERE app_type = :iAppType;
            break;
        /* 更新指定机构下终端 */
        case DOWN_SPECIFY_DEPT:
            strcpy(szDeptDetail, ptApp->szDeptDetail);

            EXEC SQL
                UPDATE terminal SET down_operate = 'Y', operate_bitmap = :szBitmap
                WHERE EXISTS
                (SELECT 1 FROM terminal t, shop s
                WHERE t.shop_no = s.shop_no AND
                INSTR(s.dept_detail, :szDeptDetail) = 1);
            break;
        default:
            strcpy(ptApp->szRetCode, ERR_UNDEF_DOWNTYPE);

            WriteLog(ERROR, "更新类型:[%d]未定义!", giDownType);

            return FAIL;
    }

    if(SQLCODE)
	{
		RollbackTran();

        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "更新终端表 商户[%s] 终端[%s] 应用类型[%d] 机构信息[%s] down_operate标志失败!SQLCODE=%d SQLERR=%s",
                        szShopNo, szPosNo, iAppType, szDeptDetail, SQLCODE, SQLERR);

		return FAIL;
	}

    CommitTran();

    return SUCC;
}
