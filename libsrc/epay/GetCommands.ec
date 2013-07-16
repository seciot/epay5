/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容: 交易指令相关函数
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.6 $
 * $Log: GetCommands.ec,v $
 * Revision 1.6  2013/03/11 05:42:48  fengw
 *
 * 1、增加自定义指令处理代码。
 *
 * Revision 1.5  2013/01/06 05:12:08  fengw
 *
 * 1、修改2D指令处理方式。
 * 2、修改控制参数相关代码。
 *
 * Revision 1.4  2012/12/26 08:30:28  fengw
 *
 * 1、修正变量szControlPara类型定义错误。
 *
 * Revision 1.3  2012/12/25 07:03:59  fengw
 *
 * 1、修改SQL语句中错误字段名。
 *
 * Revision 1.2  2012/12/24 04:44:15  wukj
 * 取指令函数新增指令参数
 *
 * Revision 1.1  2012/12/20 09:06:28  fengw
 *
 * 1、将Epos.ec中函数拆分为单个文件。
 *
 * Revision 1.1  2012/12/10 06:49:05  wukj
 * *** empty log message ***
 *
 * Revision 1.1  2012/12/10 05:12:22  wukj
 * *** empty log message ***
 *
 *----------------------------------------------------------------
 */
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>

# include "app.h"
# include "errcode.h"
# include "transtype.h"
# include "user.h"
# include "dbtool.h"

#ifdef DB_ORACLE
EXEC SQL BEGIN DECLARE SECTION;
	EXEC SQL include "../../incl/DbStru.h";
	EXEC SQL INCLUDE SQLCA;
EXEC SQL EnD DECLARE SECTION;
#else
	$include "../../incl/DbStru.h";
	$include sqlca;
#endif

int CalcCmdBytes( unsigned char cCmd );
/*获取交易流程代码*/
int
GetCommands( int iTrans, char flag, char *szCmd, int *iCmdNum, int *iCmdLen, char *szDataSource, int *iDataNum ,int *iCtlLen,char* szCtlPara)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int 	iTransType;
		char	szFlag[2];
                struct T_TRANS_COMMANDS {
                    int     iTransType;
                    int     iStep;
                    char    szTransFlag[2];
                    char    szCommand[3];
                    int     iOperIndex;
                    char    szAlog[9];
                    char    szCommandName[31];
                    char    szOrgCommand[3];
                    int     iControlLen;
                    char    szControlPara[61];
                    int     iDataIndex;
                }tTransCommand;
		
	EXEC SQL END DECLARE SECTION;

	int	iTotalCommands, iCurPos, iBytes, i, iNum;
	char	szBuf[512], szTmpStr[20], szData[30];
	unsigned char	alog, cChr, cOrgCmd, cCmd;
    char	szCtlParaTmp[61];
    int     iCtlIndex;
    char    szCtlBuf[255+1];

	memset( szData, 0, 30 );
	iTransType = iTrans;
	szFlag[0] = flag;
	szFlag[1] = 0;

	EXEC SQL DECLARE com_cur cursor for
	SELECT
		TRANS_TYPE,
		STEP,
		TRANS_FLAG,
		COMMAND,
		OPER_INDEX,
		NVL(ALOG,' '),
		NVL(COMMAND_NAME, ' '),
	    NVL(ORG_COMMAND, ' '),
        NVL(CONTROL_LEN,0),
        NVL(CONTROL_PARA,' '),
		NVL(DATA_INDEX, 0)
	FROM TRANS_COMMANDS 
	WHERE TRANS_TYPE = :iTransType and TRANS_FLAG = :szFlag
	ORDER BY STEP;

	EXEC SQL OPEN com_cur;
	if ( SQLCODE ) 
	{
		WriteLog ( ERROR, "Open com_cur error!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);
		return ( FAIL );
	}

	iTotalCommands = 0;
	iCurPos = 0;
	iNum = 0;
	iCtlIndex = 0;

	while(1)
	{
		EXEC SQL FETCH com_cur 
        INTO :tTransCommand.iTransType,
             :tTransCommand.iStep,
             :tTransCommand.szTransFlag,
             :tTransCommand.szCommand,
             :tTransCommand.iOperIndex,
             :tTransCommand.szAlog,
             :tTransCommand.szCommandName,
             :tTransCommand.szOrgCommand,
             :tTransCommand.iControlLen,
             :tTransCommand.szControlPara,
             :tTransCommand.iDataIndex;

		if ( SQLCODE ==  SQL_NO_RECORD ) 
		{
			EXEC SQL CLOSE com_cur;
			break;
		}
		else if ( SQLCODE ) 
		{
			WriteLog( ERROR, "fetch com_cur error!SQLCODE=%d SQLERR=%s", SQLCODE, SQLERR);
			return FAIL;
		}

		iTotalCommands ++;
		AscToBcd( (char*)(tTransCommand.szCommand), 2, 0 ,(char*)szTmpStr);
		cCmd = szTmpStr[0];

		if(cCmd == SPECIAL_CMD_HEAD)
		{
		    szBuf[iCurPos] = SPECIAL_CMD_HEAD;
		    iCurPos += 1;

		    szBuf[iCurPos] = tTransCommand.iOperIndex;
		    iCurPos += 1;

            AscToBcd( (char*)(tTransCommand.szOrgCommand), 2, 0 ,(char*)szTmpStr);
		    szBuf[iCurPos] = szTmpStr[0];
		    iCurPos += 1;
		}
		else
		{
		    cOrgCmd = cCmd&0x3F;
		    //选择项目、临时操作提示信息、打印有数据源，设置数据源,静态菜单显示与选择
		    if( cOrgCmd == 0x33 || cOrgCmd == 0x2F || cOrgCmd == 0x21 )
		    {
		    	szData[iNum] = tTransCommand.iDataIndex;
		    	iNum ++;
		    }
		    iBytes = CalcCmdBytes( cCmd );
		    szBuf[iCurPos] = cCmd;
		    iCurPos ++;
		    if( iBytes >= 2 )
		    {
		    	szBuf[iCurPos] = tTransCommand.iOperIndex;
		    	iCurPos ++;
		    	if( iBytes == 3 )
			    {
                    cChr = 0x80;
                    alog = 0;
                    for( i=0; i<8; i++ )
                    {
                        if( tTransCommand.szAlog[i] == '1' )
                        {
                            alog = alog|cChr;
                        }
                    cChr = cChr/2;
                    }	
                    szBuf[iCurPos] = alog;	

                    iCurPos ++;
                }
		    }
		}

		if(tTransCommand.iControlLen > 0)
		{
		    memset(szCtlParaTmp, 0 ,sizeof(szCtlParaTmp));
            AscToBcd(tTransCommand.szControlPara,tTransCommand.iControlLen,0,szCtlParaTmp);

            szCtlBuf[iCtlIndex] = tTransCommand.iControlLen/2;
            iCtlIndex++;

            memcpy(szCtlBuf+iCtlIndex, szCtlParaTmp, tTransCommand.iControlLen/2);
            
            iCtlIndex += tTransCommand.iControlLen/2;
		}
	}

	memcpy( szDataSource, szData, iNum );
	*iDataNum = iNum;
	memcpy( szCmd, szBuf, iCurPos );
	*iCmdNum = iTotalCommands;
	*iCmdLen = iCurPos;
	*iCtlLen = iCtlIndex;
	memcpy(szCtlPara, szCtlBuf, iCtlIndex);

	return SUCC;
}

int CalcCmdBytes( unsigned char cCmd )
{
    int iCmdBytes;

    /* 第1位为0表示双字节操作码；第1位为1，第2位为0表示单字节操作
       码；第1位为1，第2位为1表示三字节操作码 */
    /* 双字节操作码 */
    if( (cCmd&0x80) == 0 )
    {
        iCmdBytes = 2;
    }
    /* 单字节操作码 */
    else if( (cCmd&0x40) == 0 )
    {
        iCmdBytes = 1;
    }
    /* 三字节操作码 */
    else 
    {
        iCmdBytes = 3;
    }

    return iCmdBytes;
}



