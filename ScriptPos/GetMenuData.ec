/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 组织动态菜单数据
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.1 $
 * $Log: GetMenuData.ec,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
	EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#else
	$include sqlca;
#endif

/*******************************************************************************
 * 函数功能：组织动态菜单数据
 * 输入参数：
 *           ptApp      -  公共数据结构指针
 *           iDataIndex -  源数据索引号 
 *           iMenuId    -  动态菜单ID号
 * 输出参数：
 *           szMenuData -  动态菜单数据
 * 返 回 值： 
 *           FAIL       -  失败
 *           >=0        -  菜单数据长度
 * 作    者：Robin
 * 日    期：2012/11/30
 * 修订日志：
 *
 ******************************************************************************/
int GetMenuData( ptApp, iDataIndex, iMenuId, szMenuData )
T_App   *ptApp;
int     iDataIndex;
int     *iMenuId;
char    *szMenuData;
{
    EXEC SQL BEGIN DECLARE SECTION;
        int iRecNo;
        char szFlag[2], szOrgCmd[3];
        struct T_DYNAMIC_MENU
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
        } tDyMenu;
    EXEC SQL END DECLARE SECTION;

    int iMenuLen, iCurPos, iMenuNo, iRet;
    uchar szData[1024], szBuf[1024];

    //获取动态菜单号
    iRecNo = ptApp->szDataSource[iDataIndex];
    *iMenuId = iRecNo;

    //选择菜单标题
    EXEC SQL SELECT 
        NVL(rec_no, 0), NVL(menu_title, ' '), NVL(describe, ' '), NVL(menu_num, 0),
        NVL(menu_name1, ' '), NVL(szTransCode1, ' '),
        NVL(menu_name2, ' '), NVL(szTransCode2, ' '),
        NVL(menu_name3, ' '), NVL(szTransCode3, ' '),
        NVL(menu_name4, ' '), NVL(szTransCode4, ' '),
        NVL(menu_name5, ' '), NVL(szTransCode5, ' '),
        NVL(menu_name6, ' '), NVL(szTransCode6, ' '),
        NVL(menu_name7, ' '), NVL(szTransCode8, ' '),
        NVL(menu_name9, ' '), NVL(szTransCode9, ' ')
    INTO :tDyMenu.iRecNo, :tDyMenu.szMenuTitle, :tDyMenu.szDescribe, :tDyMenu.iMenuNum,
         :tDyMenu.szMenuName1, :tDyMenu.szTransCode1,
         :tDyMenu.szMenuName2, :tDyMenu.szTransCode2,
         :tDyMenu.szMenuName3, :tDyMenu.szTransCode3,
         :tDyMenu.szMenuName4, :tDyMenu.szTransCode4,
         :tDyMenu.szMenuName5, :tDyMenu.szTransCode5,
         :tDyMenu.szMenuName6, :tDyMenu.szTransCode6,
         :tDyMenu.szMenuName7, :tDyMenu.szTransCode7,
         :tDyMenu.szMenuName8, :tDyMenu.szTransCode8,
         :tDyMenu.szMenuName9, :tDyMenu.szTransCode9
    FROM dynamic_menu
    WHERE rec_no = :iRecNo;
    if( SQLCODE == SQL_NO_RECORD )
    {
        strcpy( ptApp->szRetCode, ERR_TRANS_DEFINE );
        WriteLog( ERROR, "dynamic menu[%d] not exist", iRecNo );
        return FAIL;
    }
    else if( SQLCODE )
    {
        strcpy( ptApp->szRetCode, ERR_SYSTEM_ERROR );
        WriteLog( ERROR, "select dynamic_menu fail %ld", SQLCODE );
        return FAIL;
    }

    iCurPos = 0;
    for( iMenuNo=1; iMenuNo<=tDyMenu.iMenuNum; iMenuNo++ )
    {
        switch (iMenuNo){
        case 1:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName1);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName1, strlen(tDyMenu.szMenuName1) );
            iCurPos += strlen(tDyMenu.szMenuName1);
            break;
        case 2:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName2);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName2, strlen(tDyMenu.szMenuName2) );
            iCurPos += strlen(tDyMenu.szMenuName2);
            break;
        case 3:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName3);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName3, strlen(tDyMenu.szMenuName3) );
            iCurPos += strlen(tDyMenu.szMenuName3);
            break;
        case 4:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName4);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName4, strlen(tDyMenu.szMenuName4) );
            iCurPos += strlen(tDyMenu.szMenuName4);
            break;
        case 5:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName5);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName5, strlen(tDyMenu.szMenuName5) );
            iCurPos += strlen(tDyMenu.szMenuName5);
            break;
        case 6:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName6);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName6, strlen(tDyMenu.szMenuName6) );
            iCurPos += strlen(tDyMenu.szMenuName6);
            break;
        case 7:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName7);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName7, strlen(tDyMenu.szMenuName7) );
            iCurPos += strlen(tDyMenu.szMenuName7);
            break;
        case 8:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName8);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName8, strlen(tDyMenu.szMenuName8) );
            iCurPos += strlen(tDyMenu.szMenuName8);
            break;
        case 9:
            //显示内容(菜单)
            DelTailSpace(tDyMenu.szMenuName9);
            memcpy( szBuf+iCurPos, tDyMenu.szMenuName9, strlen(tDyMenu.szMenuName9) );
            iCurPos += strlen(tDyMenu.szMenuName9);
            break;
        }
        
        //分割符
        szBuf[iCurPos] = 0xff;
        iCurPos ++;
    }

    //菜单标题
    DelTailSpace( tDyMenu.szMenuTitle );
    iMenuLen = 0;
    memcpy( szData+iMenuLen, tDyMenu.szMenuTitle, strlen(tDyMenu.szMenuTitle) );
    iMenuLen += strlen(tDyMenu.szMenuTitle);

    //分割符
    szData[iMenuLen] = 0xff;
    iMenuLen ++;

    //菜单项数    
    szData[iMenuLen] = tDyMenu.iMenuNum;
    iMenuLen ++;

    //分割符
    szData[iMenuLen] = 0xff;
    iMenuLen ++;

    //菜单项
    memcpy( szData+iMenuLen, szBuf, iCurPos );
    iMenuLen += iCurPos;

    memcpy( szMenuData, szData, iMenuLen );

    return iMenuLen;
}