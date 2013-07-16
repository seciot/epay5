#ifndef _SCRIPTPOS_H_
#define _SCRIPTPOS_H_

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <netinet/tcp.h>
#include <errno.h>

#include "app.h"
#include "user.h"
#include "dbtool.h"
#include "transtype.h"
#include "errcode.h"
#include "DbStru.h"
#include "Tlv.h"
#include "EpayShm.h"

#define BUFFSIZE 	        2048

#define MAX_CMD_INDEX       62              /* 最大指令数 */
#define MAX_SP_CMD_INDEX    6               /* 最大自定义指令数 */
#define MAX_CMD_NAME        64              /* 指令名称最大长度 */

#define MAX_REC_CARD_NUM    14              /* 每次上传卡号的最大条数 */

#define MAX_COMMAND_NUM     100             /* 最大指令条数 */

static jmp_buf	env;

T_App* GetTdiAddress(char *szInData, int iLen);

#ifndef _EXTERN_

char    gszaCmdName[MAX_CMD_INDEX][MAX_CMD_NAME+1] =
{
    "保留",                     /* 01号指令 */
    "安全模块号",               /* 02号指令 */
    "磁道明文",                 /* 03号指令 */
    "磁道密文",                 /* 04号指令 */
    "密码密文",                 /* 05号指令 */
    "交易数量(原流水号)",       /* 06号指令 */
    "交易金额",                 /* 07号指令 */
    "金融应用号",               /* 08号指令 */
    "商务应用号",               /* 09号指令 */
    "交易日期(YYYYMMDD)",       /* 0A号指令 */
    "年月(YYYYMM)",             /* 0B号指令 */
    "自定义数据",               /* 0C号指令 */
    "计算MAC",                  /* 0D号指令 */
    "数字签名(暂不使用)",       /* 0E号指令 */
    "冲正信息",                 /* 0F号指令 */
    "终端程序版本",             /* 10号指令 */
    "终端应用功能版本",         /* 11号指令 */
    "终端序列号",               /* 12号指令 */
    "加密报文数据",             /* 13号指令 */
    "解密报文数据",             /* 14号指令 */
    "账单支付数据",             /* 15号指令 */
    "更新终端参数",             /* 16号指令 */
    "更新安全参数",             /* 17号指令 */
    "更新应用菜单",             /* 18号指令 */
    "更新功能提示",             /* 19号指令 */
    "更新操作提示",             /* 1A号指令 */
    "更新首页信息",             /* 1B号指令 */
    "更新打印记录",             /* 1C号指令 */
    "保留(暂不使用)",           /* 1D号指令 */
    "存储帐单",                 /* 1E号指令 */
    "记录日志",                 /* 1F号指令 */
    "存储短信",                 /* 20号指令 */
    "打印数据",                 /* 21号指令 */
    "显示结果信息",             /* 22号指令 */
    "连接系统",                 /* 23号指令 */
    "发送数据",                 /* 24号指令 */
    "接收数据",                 /* 25号指令 */
    "挂机",                     /* 26号指令 */
    "验证固网支付密码",         /* 27号指令 */
    "验证MAC",                  /* 28号指令 */
    "免提拨号",                 /* 29号指令 */
    "交易确认",                 /* 2A号指令 */
    "选择动态菜单",             /* 2B号指令 */
    "更新静态菜单",             /* 2C号指令 */
    "静态菜单显示与选择",       /* 2D号指令 */
    "上传卡号",                 /* 2E号指令 */
    "临时提示信息",             /* 2F号指令 */
    "流程控制",                 /* 30号指令 */
    "保留(暂不使用)",           /* 31号指令 */
    "文件数据处理",             /* 32号指令 */
    "读取卡号",                 /* 33号指令 */
    "上传交易日志",             /* 34号指令 */
    "上传错误日志",             /* 35号指令 */
    "读取利率",                 /* 36号指令 */
    "读串口数据",               /* 37号指令 */
    "写串口数据",               /* 38号指令 */
    "更新工作密钥",             /* 39号指令 */
    "预拨号",                   /* 3A号指令 */
    "清空菜单",                 /* 3B号指令 */
    "冲正重发控制",             /* 3C号指令 */
    "信息显示",                 /* 3D号指令 */
    "TMS参数下载"               /* 3E号指令 */
};

char    gszaSpecialCmdName[MAX_SP_CMD_INDEX][MAX_CMD_NAME+1] =
{
    "EMV参数版本号",            /* 01号指令 */
    "保存EMV参数",              /* 02号指令 */
    "EMV公钥版本号",            /* 03号指令 */
    "保存EMV公钥",              /* 04号指令 */
    "EMV完整流程",              /* 05号指令 */
    "EMV联机处理"               /* 06号指令 */
};

int giEachPackMaxBytes, giMacChk;
int giSock, giDispMode, giHolderNameMode, giTimeoutTdi;
char gszWebIp[25], gszAuthKey[17], gszAcqBankId[12], gszWebPort[5+1];

#else

extern int giEachPackMaxBytes, giMacChk;
extern int giSock, giDispMode, giHolderNameMode, giTimeoutTdi;
extern char gszWebIp[25], gszAuthKey[17], gszAcqBankId[12], gszWebPort[5+1];
extern char gszaCmdName[MAX_CMD_INDEX][MAX_CMD_NAME+1];
extern char gszaSpecialCmdName[MAX_SP_CMD_INDEX][MAX_CMD_NAME+1];

#endif
#endif