/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台web交易请求接收模块 更新指定终端
** 创 建 人：冯炜
** 创建日期：2012-12-18
**
** $Revision: 1.4 $
** $Log: DownByDept.ec,v $
** Revision 1.4  2012/12/25 07:00:07  fengw
**
** 1、修改web交易监控通讯端口号变量类型为字符串。
**
** Revision 1.3  2012/12/21 02:05:32  fengw
**
** 1、将文件格式从DOS转为UNIX。
**
** Revision 1.2  2012/12/21 02:04:02  fengw
**
** 1、修改Revision、Log格式。
**
*******************************************************************/

#define _EXTERN_

#include "comweb.h"

EXEC SQL BEGIN DECLARE SECTION;
    EXEC SQL INCLUDE SQLCA;
EXEC SQL END DECLARE SECTION;

/****************************************************************
** 功    能：更新指定终端
** 输入参数：
**        ptApp                 app结构指针
**        lTimeOut              交易超时时间
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
int DownByDept(T_App *ptApp, long lTimeOut)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    szDeptDetail[70+1];         /* 机构层级信息 */
        char    szShopNo[15+1];             /* 商户号 */
        char    szPosNo[15+1];              /* 终端号 */
        char    szPsamNo[16+1];             /* 安全模块号 */
        char    szIp[15+1];                 /* IP地址 */
        char    szTelephone[15+1];          /* 电话号码 */
        int     iTermModule;                /* 终端参数模版 */
        int     iPsamModule;                /* 安全参数模版 */
    EXEC SQL END DECLARE SECTION;

    memset(szDeptDetail, 0, sizeof(szDeptDetail));
    strcpy(szDeptDetail, ptApp->szDeptDetail);

    EXEC SQL
        DECLARE cur_down_by_dept CURSOR FOR
        SELECT t.shop_no, t.pos_no, t.psam_no, t.ip, t.telephone, t.term_module, t.psam_module
        FROM terminal t, shop s
        WHERE t.shop_no = s.shop_no AND s.dept_detail = :szDeptDetail;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "声明游标cur_down_by_dept失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    EXEC SQL OPEN cur_down_by_dept;
    if(SQLCODE)
    {
        strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

        WriteLog(ERROR, "打开游标cur_down_by_dept失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

        return FAIL;
    }

    while(1)
    {
        memset(szShopNo, 0, sizeof(szShopNo));
        memset(szPosNo, 0, sizeof(szPosNo));
        memset(szPsamNo, 0, sizeof(szPsamNo));
        memset(szIp, 0, sizeof(szIp));
        memset(szTelephone, 0, sizeof(szTelephone));

        EXEC SQL
            FETCH cur_down_by_dept INTO :szShopNo, :szPosNo, :szPsamNo, :szIp, :szTelephone, :iTermModule, :iPsamModule;
        if(SQLCODE == SQL_NO_RECORD)
        {
            EXEC SQL CLOSE cur_down_by_dept;

            break;
        }
        else if(SQLCODE)
        {
            EXEC SQL CLOSE cur_down_by_dept;

            strcpy(ptApp->szRetCode, ERR_SYSTEM_ERROR);

            WriteLog(ERROR, "读取游标cur_down_by_dept失败!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);

            return FAIL;
        }

        DelTailSpace(szShopNo);
        DelTailSpace(szPosNo);        
        DelTailSpace(szPsamNo);
        DelTailSpace(szIp);
        DelTailSpace(szTelephone);

        strcpy(ptApp->szShopNo, szShopNo);
        strcpy(ptApp->szPosNo, szPosNo);
        strcpy(ptApp->szPsamNo, szPsamNo);
        strcpy(ptApp->szIp, szIp);
        strcpy(ptApp->szCallingTel, szTelephone);
        ptApp->iTermModule = iTermModule;
        ptApp->iPsamModule = iPsamModule;

        if(SendWebReq(ptApp) != SUCC)
        {
            /* 发送WEB监控 */
            WebDispMoni(ptApp, ptApp->szTransName, gszMoniIP, gszMoniPort);
        }
	}

	return SUCC;
}