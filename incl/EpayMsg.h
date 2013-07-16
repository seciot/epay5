/******************************************************************
** Copyright(C)2012 - 2015联迪商用设备有限公司
** 主要内容：定义本系统消息队列的宏以及函数定义等
** 创 建 人：高明鑫
** 创建日期：2012/11/8
** $Revision: 1.7 $
** $Log: EpayMsg.h,v $
** Revision 1.7  2012/12/07 05:47:02  fengw
**
** 1、添加libepay库下头文件。
**
** Revision 1.2  2012/12/07 01:51:06  fengw
**
** 1、修改消息队列类型定义。
** 2、修改文件格式，替换tab为空格。
**
** Revision 1.1  2012/11/27 02:42:33  epay5
** add by gaomx
**
*******************************************************************/
# ifndef _EPAYMSG
# define _EPAYMSG

#define	MSGFILE         "/etc/MSGFILE"

/* 定义消息队列类型 */
#define ACCESS_TO_PORC_QUEUE        1
#define PROC_TO_ACCESS_QUEUE        2
#define PROC_TO_PRESENT_QUEUE       3
#define PRESENT_TO_PROC_QUEUE       4
#define TO_HSM_QUEUE                5
#define FROM_HSM_QUEUE              6

#define IPCPERM                 0666
#define IPCGET                  0	
#define IPCCREAT                (IPCPERM | IPC_CREAT)

int giPresentToProc, giProcToPresent, giProcToAccess, giAccessToProc, giToHsm, giFromHsm;

#endif
