/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：定义本系统共享内存的宏以及函数定义等
** 创 建 人：陈瑞兵
** 创建日期：2012/11/8
** ----------------------------------------------------------------
** $Revision: 1.1 $
** $Log: EpayShm.h,v $
** Revision 1.1  2012/12/07 05:47:02  fengw
**
** 1、添加libepay库下头文件。
**
** Revision 1.4  2012/12/03 05:51:01  fengw
**
** 1、增加GetAppAddress函数声明定义。
**
** Revision 1.3  2012/11/30 06:53:35  fengw
**
** 1、修改T_HOST、T_SHM_EPAY结构定义，增加T_LOCALCARDS结构定义。
**
** Revision 1.2  2012/11/28 02:27:05  chenrb
** dos2unix 格式转换
**
** Revision 1.1  2012/11/28 02:19:52  chenrb
** 初始化版本
**
*******************************************************************/

#ifndef	_EPAY_SHM_
#define _EPAY_SHM_

#include <stdlib.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "transtype.h"
#include "libpub.h"
#include "EpaySem.h"

#define SHM_FILE                "/etc/SHMFILE"
#define EPAY_SHM_ID             1

/* 最大接入后台数 */
#define MAX_HOST_NUM            50

/* 最大接入客户端数 */
#define MAX_ACCESS_NUM          60

#define CARD_PAN                1           /* 卡号 */
#define CARD_TRACK2             2           /* 二磁道 */
#define CARD_TRACK3             3           /* 三磁道 */

typedef struct
{
    char    szBankName[21];
    char    szBankId[12];
    char    szCardName[41];
    char    szCardId[20];
    int     iCardNoLen;
    int     iCardSite2;
    int     iExpSite2;
    int     iPanSite3;
    int     iCardSite3;
    int     iExpSite3;
    char    szCardType[2];
    int     iCardLevel;
}T_CARDS;

typedef struct
{
    char    szCardId[16+1];
    char    szCardName[40+1];
    int     iCardNoLen;
    char    szCardType[1+1];
}T_LOCALCARDS;

typedef struct
{
    long    lMsgType;           /* 对应业务提交模块的第一个参数(接收消息类型) */
    int     iLinkNo;            /* 链路序号 */
	char	cClitNet;           /* client状态, 'Y'-已连接  'N'-未链接 */
	char	cServNet;           /* server状态, 'Y'-已连接  'N'-未链接 */
}T_HOST;

typedef struct
{
    long    lLastVisitTime;            /* 最近访问时间，为0或与系统当前时间相比超过15分钟则可以分配给其他访问者使用 */
}T_TDI;

/* 请求方IP与处理进程号对照表 */
typedef struct
{
	char    szIp[20];
    long    lPort;
	long	lPid;
}T_ACCESS;

/* 共享内存数据结构 */
typedef struct
{
    int             iCardNum;                       /* 卡表条数 */
    T_CARDS         tCards[MAX_CARD_NUM];           /* 卡表数组 */
    int             iLocalCardNum;                  /* 本地卡表条数 */
    T_LOCALCARDS    tLocalCards[MAX_CARD_NUM];      /* 本地卡表数组 */
    T_ACCESS        tAccess[MAX_ACCESS_NUM];        /* 接入客户端数组 */
	T_App           tApp[MAX_TRANS_DATA_INDEX];     /* 交易数据数组 */
    int             iCurTdi;                        /* 当前数据索引号 */
	T_TDI           tTdi[MAX_TRANS_DATA_INDEX];     /* 交易数据索引请求时间 */
    T_HOST          tHost[MAX_HOST_NUM];            /* 接入后台数组 */
}T_SHM_EPAY;

#define SHM_EPAY_SIZE       sizeof(T_SHM_EPAY)

T_SHM_EPAY  *gpShmEpay;
int         giShmEpayID;

extern T_App* GetAppAddress(int iTransDataIdx);

#else

extern T_EPAY   *gpShmEpay;
extern int      giShmEpayID;

#endif
