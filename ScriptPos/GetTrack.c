/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：易收付平台epay公共库 获取磁道数据
** 创 建 人：fengwei
** 创建日期：2013/03/06
**
** $Revision: 1.1 $
** $Log: GetTrack.c,v $
** Revision 1.1  2013/03/11 07:13:19  fengw
**
** 1、将磁道数据处理封装为函数，供报文解析时调用。
**
*******************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/****************************************************************
** 功    能：解析POS上送磁道数据
** 输入参数：
**        ptApp                     app结构指针
**        szData                    输入数据
** 输出参数：
**        无
** 返 回 值：
**        >0                        磁道数据长度
**        FAIL                      解析磁道数据失败
** 作    者：
**        fengwei
** 日    期：
**        2013/03/06
** 调用说明：
**
** 修改日志：
****************************************************************/
int GetTrack(T_App *ptApp, char *szData)
{
    int     iTrackLen;              /* 磁道数据长度 */
    int     iTmp;                   /* 临时变量 */
    int     i;                      /* 临时变量 */

    iTrackLen = 0;

    /* 二磁道 */
    iTmp = (uchar)(szData[iTrackLen]);
    if(iTmp > 37)
    {
        WriteLog(ERROR, "二磁道数据长度[%d]非法!", iTmp);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }
    iTrackLen += 1;

    if(iTmp != 0)
    {
        BcdToAsc((uchar*)(szData+iTrackLen), iTmp, LEFT_ALIGN, 
                 ptApp->szTrack2);

        iTrackLen += iTmp%2==0?iTmp/2:iTmp/2+1;

        /* 替换磁道数据分隔符 */
        for(i=0;i<iTmp;i++)
        {
            if(ptApp->szTrack2[i] == 'D')
            {
                ptApp->szTrack2[i] = '=';
            }
        }
    }
#ifdef DEBUG
    WriteLog(TRACE, "二磁道[%d]:[%s]", iTmp, ptApp->szTrack2);
#endif

    /* 三磁道 */
    iTmp = (uchar)(szData[iTrackLen]);    
    if(iTmp > 104)
    {
        WriteLog(ERROR, "三磁道数据长度[%d]非法!", iTmp);

        strcpy(ptApp->szRetCode, ERR_DATA_FORMAT);

        return FAIL;
    }
    iTrackLen += 1;

    if(iTmp != 0)
    {
        BcdToAsc((uchar*)(szData+iTrackLen), iTmp, LEFT_ALIGN, 
                 ptApp->szTrack3);

        iTrackLen +=  iTmp%2==0?iTmp/2:iTmp/2+1;

        /* 替换磁道数据分隔符 */
        for(i=0;i<iTmp;i++)
        {
            if(ptApp->szTrack3[i] == 'D')
            {
                ptApp->szTrack3[i] = '=';
            }
        }
    }
#ifdef DEBUG
    WriteLog(TRACE, "三磁道[%d]:[%s]", iTmp, ptApp->szTrack3);
#endif

    return iTrackLen;
}