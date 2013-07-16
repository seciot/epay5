/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容：模拟POS程序
** 创 建 人：
** 创建日期：
**
** $Revision
** $Log
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/timeb.h>
#include <time.h>
#include <errno.h>
#include <setjmp.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "user.h"
#include "dbtool.h"

EXEC SQL BEGIN DECLARE SECTION;
EXEC SQL INCLUDE SQLCA;
EXEC SQL include "../incl/DbStru.h";
EXEC SQL END DECLARE SECTION;

#define DBGMODE
#define SCOUNIX
#define SINGLE_DES   1     /* DES */
#define TRIPLE_DES   2     /* 3DES */

/* 最大并发数 */
#define MAX_POS_NUM    100

/* 返回包数据位置宏定义 */
#define RETURN_POS    66
#define TRACE_POS    50
#define PIN_KEY_POS    83
#define MAC_KEY_POS    105

const int REQ_PSAM_POS=57;
const int REQ_TRACE_POS=73;
const int REQ_TRACK2_POS=94;
const int REQ_TRACK3_POS=114;
const int REQ_PIN_POS=172;
const int REQ_MAC_POS=180;

const int RSP_TRACE_POS=56;
const int RSP_PIN_KEY_POS=89;
const int RSP_MAC_KEY_POS=121;

const int REQ_MAC_SKIP=14;

#define MAXBUFF        4096

#define SA struct sockaddr

typedef struct sockaddr * P_ST_SA;

/* 全局 Socket 句柄 */
int giSockId;

/* 全局变量计时器开始时间 */
unsigned long glBeginTime;

int giSleepTime;

/* 全局变量, 时间间隔 */
char    gszMasterKey[17];

/* 通信连接信息 */
struct T_CommInfo{
    char szHostIP[16];    /*主机 IP 地址*/
    int iHostPort;        /*主机服务端口*/
};

struct T_CommInfo tCommStru = {0};

/* 全局变量交易流水号 */
int    giMaxNum;

/* 配置参数信息 */
struct T_ParamInfo {
    unsigned char szSecureUnit[16+1];    /*安全模块号*/
    unsigned char pin_key[16+1];        /*PIN密钥*/
    unsigned char mac_key[16+1];        /*MAC密钥*/
    unsigned char szTrack2[19+1];        /*二磁道数据*/
    unsigned char szTrack3[52+1];        /*三磁道数据*/
    unsigned char szPIN[6+1];        /* PIN 口令明文*/
    int    iTrace;
    int    iNextTrace;        /*下一次起始流水号*/
};

struct T_ParamInfo tParamStru[MAX_POS_NUM];

typedef struct T_ParamInfo T_ParamInfo;

unsigned char guszLoginPacket[1024]=
"\x60\x00\x05\x00\x10"
"\x01"
"\x00\x00"
"\x01"
"\x01"
"\x01"
"\x01"
"\x00\x55"
"\x02"
"\x00"
"\x20\x12"
"\x07\x10\x23\x04"
"\x20\x12\x12\x15"
"\x4c\x41\x4e\x44\x49\x20\x20\x20\x20\x20"
"\x8d\x91\x46\x53"
"\x31\x36\x37\x33\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x30"
"\x30\x31\x31\x30\x34\x37\x30\x36\x30\x30\x30\x30\x30\x30\x30\x31"
"\x00\x00\x01"
"\x00\x00\x00\x05"
"\x01\x82\x00\x10\x30"
"\x31\x31\x30\x34\x37\x30\x36\x30\x30\x30\x30\x30"
"\x30\x30\x31";

unsigned char guszTermPacket[] = 
{0x60, 0x08, 0x08, 0x82, 0x78, 0x01, 0x00, 0x00, 0x38, 0x30, 0x30, 0x38, 0x36, 0x30, 0x30, 0x37,
0x37, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x02, 0x00, 0x20, 0x01, 0x20, 0x10, 0x06,
0x26, 0x30, 0x30, 0x39, 0x33, 0x4c, 0x31, 0x37, 0x4c, 0x35, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
0x32, 0x37, 0x00, 0x11, 0x17, 0x00, 0x00, 0x01, 0x01, 0x04, 0x03, 0x06, 0x07, 0x12, 0xc5, 0x07,
0x81, 0xcd, 0x00, 0xc0, 0x00, 0x5f, 0x25, 0x90, 0x03, 0x09, 0x10, 0x00, 0x36, 0x26, 0x96, 0xd0,
0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x18, 0x67, 0x90, 0x68, 0x99, 0x90, 0x03, 0x09, 0x10,
0x00, 0x36, 0x26, 0x96, 0xd1, 0x56, 0x15, 0x60, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x30,
0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x18, 0x67, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xd4, 0x62, 0xb8, 0x64, 0x57, 0xdf, 0xce, 0xbb, 0x2b, 0x1d, 0xf5,
0xb8, 0x8c, 0xb2, 0x91, 0x72};

unsigned char CheckValue[] = {0x30, 0x30};

const int LOGIN_PACK_SIZE=100;

#define PACKET_LEN    sizeof(guszTermPacket)

int ChildProc(int iPosNum, int iTotalNum);
int ParentProc(int iPosNo, int iTotalNum);
void PackingData(int iPosNo, unsigned char * szBuff);
int InitParamTable();

/*******************************************
 * 安全退出函数
 *******************************************/
void safe_exit(int exit_parm)
{
    WriteLog(TRACE, "simulate program terminates : %d", exit_parm);
    close(giSockId);
    CloseDB();
    exit(exit_parm);
}

/*****************************************************************
 ** 功    能：以微秒为单位计算时间
 ** 输入参数：
 ** 输出参数：
 ** 返 回 值：
 **         以微秒为单位的时间
 ** 作    者：
 ** 日    期：
 ** 修改日志：
 ****************************************************************/
unsigned long GetMillTime( )
{
    struct timeb tp;
    struct tm *ptm;
    unsigned long lTime;

    ftime ( &tp );
    ptm = localtime (  & ( tp.time )  );

    lTime = (ptm->tm_hour)*3600000
            + (ptm->tm_min)*60000+(ptm->tm_sec)*1000+tp.millitm;
    
    return ( lTime );
}

int main(int argc, char ** argv)
{
    int i;
    int iRet, iLen, iSndLen, iRecLen, iSleepTime;
    int iPosNum, iTotalNum, iBeginTrace, iHostPort;
    char szTmpBuf[1024], szSndBuf[1024];
    char szTime[50], szTmpStr[100];
    char szBcdPinKey[17], szBcdMacKey[17], szTmk[17];
    char cEnter;
    pid_t pid;
    struct sockaddr_in tServAddr;
    
    EXEC SQL BEGIN DECLARE SECTION;
    T_TERMINAL tTerminal;
    T_POS_KEY tPosKey;
    int iKeyIndex;
    char szTermId[9];
    char kk[100];
    EXEC SQL END DECLARE SECTION;

    for( i = 0; i < 32; i++ ){
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
            continue;
        signal( i, SIG_IGN );
    }

    signal( SIGKILL, safe_exit);
    signal( SIGUSR1, safe_exit);

    /* 如果初始化参数表失败就退出 */
    if (InitParamTable() != 0) 
    {
        WriteLog( ERROR, "init param fail" );
        exit(0);
    }

    iRet = GetMasterKey( gszMasterKey );
    if( iRet != SUCC )
    {
        WriteLog( ERROR, "read key fail" );
        exit(0);
    }
    giSockId = CreateCliSocket(tCommStru.szHostIP, tCommStru.iHostPort);
    if( giSockId < 0 )
    {
        WriteLog(ERROR, "create connect failed errno[%d]", errno);
#ifdef DBGMODE
        printf("create connect failed errno[%d] \n", errno);
#endif
        close(giSockId);
        exit(0);
    }

    if( OpenDB() )
    {
        WriteLog( ERROR, "open database fail [%ld]!!", SQLCODE );
        close( giSockId );
        exit(0);
    }

    EXEC SQL DECLARE func_cur CURSOR 
        FOR SELECT * FROM terminal WHERE status = 0 ORDER BY psam_no;
    if( SQLCODE )
    {
        WriteLog( ERROR, "declare cur fail %ld", SQLCODE );
        close( giSockId );
        CloseDB();
        exit(0);
    }

    EXEC SQL OPEN func_cur;
    if( SQLCODE )
    {
        WriteLog( ERROR, "open func_cur fail %ld", SQLCODE );
        close( giSockId );
        CloseDB();
        exit(0);
    }

    iLen = strlen(guszLoginPacket);
    iSndLen = iLen/2;
    giMaxNum = 0;
    while(1)
    {
        EXEC SQL FETCH func_cur INTO :tTerminal;
        if( SQLCODE == SQL_NO_RECORD || giMaxNum == MAX_POS_NUM )
        {
            EXEC SQL close func_cur;
            break;
        }
        else if( SQLCODE )
        {
            WriteLog( ERROR, "fetch func_cur fail %ld", SQLCODE );
            EXEC SQL CLOSE func_cur;
            CloseDB();
            return FAIL;
        }

        /* 组签到包 */
        memcpy(szTmpBuf, guszLoginPacket, LOGIN_PACK_SIZE);
        memcpy(szTmpBuf + REQ_PSAM_POS, tTerminal.szPsamNo, 16 );

        szSndBuf[0] = iSndLen/256;
        szSndBuf[1] = iSndLen%256;
        memcpy( szSndBuf+2, szTmpBuf, iSndLen );

        /* 发送报文信息到主机 */
        iRet = WriteSock(giSockId, szSndBuf, iSndLen+2, 0);
        if(iRet <= 0) 
        {
            WriteLog(ERROR, "send data failed : %d", errno);
#ifdef DBGMODE
            printf("send data failed : %d\n", errno);
#endif
            close(giSockId);
            EXEC SQL CLOSE func_cur;
            CloseDB();
            exit(0);
        }

        if( (iRet = ReadSockFixLen(giSockId, 0, 2, szTmpBuf)) < 0 ){
            WriteLog( ERROR, "recv length fail" );
            printf( "recv length fail\n" );
            close( giSockId );
            EXEC SQL CLOSE func_cur;
            CloseDB();
            return FAIL;
        }
        iRecLen = (unsigned char)szTmpBuf[0]*256+
                (unsigned char)szTmpBuf[1];
           iRet = ReadSockVarLen(giSockId, 5, szTmpBuf);
        if( iRet != iRecLen )
        {
            printf( "recv data fail\n" );
            WriteLog( ERROR, "recv data fail %ld", errno );
            close( giSockId );
            EXEC SQL CLOSE func_cur;
            CloseDB();
            return FAIL;
        }

        /* 签到成功 */
        if( memcmp(szTmpBuf+RETURN_POS, "00", 2) == 0 )
        {
            printf( "%s 签到成功!\n", tTerminal.szPsamNo );

            strcpy(tParamStru[giMaxNum].szSecureUnit, tTerminal.szPsamNo );
            memcpy( szTmpStr, tTerminal.szPsamNo+8, 8 );
            szTmpStr[8] = 0;
            iKeyIndex = atol(szTmpStr);

            /* 取流水号 */
            BcdToAsc( szTmpBuf+TRACE_POS, 6, 0, szTmpStr );
            szTmpStr[6] = 0;
            tParamStru[giMaxNum].iTrace = atol(szTmpStr);

            tParamStru[giMaxNum].iNextTrace = tParamStru[giMaxNum].iTrace;

            /* 取PinKey、MacKey密文 */
            memcpy( szBcdPinKey, szTmpBuf+PIN_KEY_POS, 16 );
            memcpy( szBcdMacKey, szTmpBuf+MAC_KEY_POS, 16 );

            EXEC SQL SELECT * INTO :tPosKey FROM pos_key WHERE key_index = :iKeyIndex;
            if( SQLCODE == SQL_NO_RECORD )
            {
                WriteLog( ERROR, "pos_key[%d] not exist", iKeyIndex );
                continue;
            }
            else if( SQLCODE )
            {
                WriteLog( ERROR, "select pos_key fail[%ld]", SQLCODE );
                continue;
            }
            
            /* 解密终端主密钥 */
            AscToBcd(tPosKey.szMasterKey, 32, 0, szTmpBuf);
            _TriDES( gszMasterKey, szTmpStr, szTmk );
            _TriDES( gszMasterKey, szTmpStr+8, szTmk+8 );

            /* 解密PinKey */
            _TriDES( szTmk, szBcdPinKey, tParamStru[giMaxNum].pin_key );
            _TriDES( szTmk, szBcdPinKey+8, tParamStru[giMaxNum].pin_key+8 );

            /* 解密MacKey */
            _TriDES( szTmk, szBcdMacKey, tParamStru[giMaxNum].mac_key );
            _TriDES( szTmk, szBcdMacKey+8, tParamStru[giMaxNum].mac_key+8 );

            giMaxNum ++;
        }
    }
    CloseDB();
    
    /* 无限循环 */
    while (1) {
        do {
            do {
                printf("1、开始测试\n");
                printf("2、退    出\n");
                printf("请选择: \n");

                memset(szTmpBuf, 0x00, sizeof(szTmpBuf));
                scanf("%s", szTmpBuf);
                cEnter = szTmpBuf[0];
            } while(cEnter < '1' || cEnter > '2');

            if (cEnter == '2') {
                // doing some thing ...
                close( giSockId );
                exit( 0 );
            }

            printf( "请输入POS数(最多%d台): \n", giMaxNum );
            do{
                memset(szTmpBuf, 0x00, sizeof(szTmpBuf));
                scanf("%s", szTmpBuf);
                if (IsNumber(szTmpBuf) == 0) 
                {
                    printf( "输入有误, 请不要输入除了数字以外的字符!\n" );
                    printf( "请重新输入POS数(最多%d台): \n", giMaxNum );
                    continue;
                }
                iPosNum = atoi(szTmpBuf);
                if( iPosNum > giMaxNum )
                {
                    printf( "输入数据超限!\n" );
                    printf( "请重新输入POS数(最多%d台): \n", giMaxNum );
                }
                if( iPosNum == 0 )
                {
                    printf( "输入数据不能为0\n" );
                    printf( "请重新输入POS数(最多%d台): \n", giMaxNum );
                }
            }while( iPosNum > giMaxNum );

            printf( "请输入每台POS交易数: \n" );
            do{
                memset(szTmpBuf, 0x00, sizeof(szTmpBuf));
                scanf("%s", szTmpBuf);
                if (IsNumber(szTmpBuf) == 0) 
                {
                    printf( "输入有误, 请不要输入除了数字以外的字符!\n" );
                    printf( "请重新输入每台POS交易数: \n" );
                    continue;
                }
                iTotalNum = atoi(szTmpBuf);
                if( iTotalNum >= 1 )
                {
                    break;
                }
                else
                {
                    printf( "输入有误\n" );
                    printf( "请重新输入每台POS交易数: \n" );
                }
            }while( 1 );

            printf( "请输入时间间隔： \n" );
            do{
                memset(szTmpBuf, 0x00, sizeof(szTmpBuf));
                scanf("%s", szTmpBuf);
                if (IsNumber(szTmpBuf) == 0) 
                {
                    printf( "输入有误, 请不要输入除了数字以外的字符!\n" );
                    printf( "请重新输入时间间隔：\n" );
                    continue;
                }
                giSleepTime = atoi(szTmpBuf);
                if( giSleepTime >= 1 )
                {
                    break;
                }
                else
                {
                    printf( "输入有误\n" );
                    printf( "请重新输入时间间隔：\n" );
                }
            }while( 1 );

            for( i=0; i<MAX_POS_NUM; i++ )
            {
                tParamStru[i].iTrace = tParamStru[i].iNextTrace;

                tParamStru[i].iNextTrace =
                tParamStru[i].iTrace + iTotalNum; 
            }

            /* 记录测试开始时间 */
            glBeginTime = GetMillTime();
WriteLog( TRACE, "begin time %ld", glBeginTime );

            /* 创建新进程 */
            pid = fork();
            switch(pid) {
            case    -1:
                /* doing some thing close */
                WriteLog(ERROR, "can not fork" );
#ifdef DBGMODE
                printf("can not fork\n");
#endif
                close(giSockId);
                exit(0);
                break;
            case    0:
                /* 子进程处理 */
                ChildProc(iPosNum, iTotalNum);
                close(giSockId);

                fflush(stdin);
                fflush(stdout);
                exit(0);
                break;
            default:
                /* 父进程处理 */
                iRet = ParentProc(iPosNum, iTotalNum);
                break;
            }
        } while(iRet == SUCC);

        giSockId = CreateCliSocket(tCommStru.szHostIP, tCommStru.iHostPort);
        if( giSockId < 0 )
        {
            WriteLog(ERROR, "create connect failed errno[%d]", errno);
            printf("create connect failed errno[%d] \n", errno);
            close(giSockId);
            exit(0);
        }
    }
}

/*****************************************************************
 ** 功    能：子进程处理函数
 ** 输入参数：
 **          iPosNum:POS数
 **          iTotalNum:单台交易数
 ** 输出参数：
 ** 返 回 值：
 ** 作    者：
 ** 日    期：
 ** 修改日志：
 ****************************************************************/
int ChildProc( int iPosNum, int iTotalNum )
{
    int pid, i, iRet;

    for( i = 0; i<iPosNum; i++ ){
        pid = fork();
        switch( pid ){
        case -1:
            WriteLog( ERROR, "fork child[%ld] fail %ld", i+1, errno );
            printf( "fork child[%ld] fail %ld\n", i+1, errno );
            close(giSockId);
            exit(0);
        case 0:
            iRet = SingleChildProc( i, iTotalNum );
            close(giSockId);
            exit(0);
        default:
            break;
        }
    }
    if( pid > 0 )
    {
        close(giSockId);
        exit(0);
    }
}

/*****************************************************************
 ** 功    能：通讯处理函数
 ** 输入参数：
 **          iPosNum:POS数
 **          iTotalNum:单台交易数
 ** 输出参数：
 ** 返 回 值：
 ** 作    者：
 ** 日    期：
 ** 修改日志：
 ****************************************************************/
int SingleChildProc(int iPosNo, int iTotalNum)
{
    int i, iRet;
    unsigned char szBuffer[1024];
    
    for( i = 0; i < 32; i++ ){
        if( i == SIGALRM || i == SIGKILL || 
            i == SIGUSR1 || i == SIGUSR2 )
            continue;
        signal( i, SIG_IGN );
    }

    for (i=0; i<iTotalNum; i++) 
    {
        /* 组包 */
        memset(szBuffer, 0x00, sizeof(szBuffer));
        PackingData(iPosNo, szBuffer);

        /* 发送报文信息到主机 */
        iRet = WriteSock(giSockId, szBuffer, PACKET_LEN+2);
        if(iRet <= 0) 
        {
            WriteLog(ERROR, "send data failed : %d", errno);
#ifdef DBGMODE
            printf("send data failed : %d\n", errno);
#endif
            close(giSockId);
            exit(0);
        }

        sleep( giSleepTime );
    }

    return(0);
}

/*****************************************************************
 ** 功    能：父进程处理函数
 ** 输入参数：
 **          iPosNum:POS数
 **          iTotalNum:单台交易数
 ** 输出参数：
 ** 返 回 值：
 ** 作    者：
 ** 日    期：
 ** 修改日志：
 ****************************************************************/
int ParentProc(int iPosNum, int iTotalNum)
{
    int flag, iTimeOut, i, iLen, iTotal, iSuccNum, iFailNum, iRet;
    char szTmpBuf[1024], szTmpStr[100];
    unsigned long lTotalTime, lEachTime, lEndTime;
    float fTotalTime, fNum;

    iTotal = iPosNum*iTotalNum;

    signal(SIGUSR1, safe_exit);
    signal(SIGUSR2, safe_exit);
    signal(SIGKILL, safe_exit);

    iSuccNum = 0;
    iFailNum = 0;
    iTimeOut = 20;
    flag = SUCC;

    for( i = 0; i < iTotal; i++ )
    {
        if( (iRet = ReadSockFixLen(giSockId, iTimeOut, 2, szTmpBuf)) < 0 ){
            WriteLog( ERROR, "recv length fail" );
            printf( "recv length fail\n" );
            flag = FAIL;
            break;
        }
        iLen = (unsigned char)szTmpBuf[0]*256+(unsigned char)szTmpBuf[1];
           iRet = ReadSockVarLen(giSockId, iTimeOut, szTmpBuf);
        if( iRet != iLen )
        {
            printf( "recv data fail\n" );
            WriteLog( ERROR, "recv data fail %ld", errno );
            close( giSockId );
            return FAIL;
        }

        if( memcmp(szTmpBuf+68, "00", 2) == 0 )
        {
            iSuccNum ++;
            printf( "SuccNum[%ld] FailNum[%ld] RetCode[00]\n", iSuccNum, iFailNum );
        }
        else
        {
            iFailNum ++;
            printf( "SuccNum[%ld] FailNum[%ld] RetCode[%2.2s]\n", iSuccNum, iFailNum, szTmpBuf+64 );
        }
    }

    lEndTime = GetMillTime();
    if( i < iTotal )
    {
        lEndTime = lEndTime-iTimeOut*1000;
    }
WriteLog( TRACE, "End time %ld", lEndTime );
    
    lTotalTime = lEndTime-glBeginTime;
    fTotalTime = (float)lTotalTime;
    fNum = ((float)iTotal)/(fTotalTime/1000.0);

    WriteLog( ERROR, "共[%ld]台POS并发交易，每台不间断发送[%ld]笔交易，共[%ld]笔",
iPosNum, iTotalNum, iTotal ); 
    WriteLog( ERROR, "成功[%ld]笔 失败[%ld]笔 超时[%ld]笔 耗时%ld毫秒 %ld毫秒/笔 %.2f笔/秒", iSuccNum, iFailNum, iTotal-(iSuccNum+iFailNum), lTotalTime, lTotalTime/iTotal, fNum );
    printf( "共[%ld]台POS并发交易，每台不间断发送[%ld]笔交易，共[%ld]笔\n",
iPosNum, iTotalNum, iTotal ); 
    printf( "成功[%ld]笔 失败[%ld]笔 超时[%ld]笔 耗时%ld毫秒 %ld毫秒/笔 %.2f笔/秒\n", iSuccNum, iFailNum, iTotal-(iSuccNum+iFailNum), lTotalTime, lTotalTime/iTotal, fNum );

    return(flag);
}

/*****************************************************************
 ** 功    能：打包函数
 ** 输入参数：
 **          iPosNo:POS序号
 ** 输出参数：
 **          szOutData:打包后的数据
 ** 返 回 值：
 ** 作    者：
 ** 日    期：
 ** 修改日志：
 ****************************************************************/
void PackingData(int iPosNo, unsigned char *szOutData)
{
    int iRet, iMacDataLen, iLen;
    unsigned char szPan[50], szMacData[1024], szMac[9];
    unsigned char szEncryptPin[24], szBuff[1024];
    unsigned char szTraceNo[7];

    /* 从二磁道中取出PAN */
    memset(szPan, 0x00, sizeof(szPan));
    getAccNoFromTrackData(tParamStru[iPosNo].szTrack2, szPan);

    /* 计算PIN密文 */
    memset(szEncryptPin, 0x00, sizeof(szEncryptPin));
    iRet = ANSIX98(szEncryptPin, tParamStru[iPosNo].pin_key, tParamStru[iPosNo].szPIN, szPan, 6, TRIPLE_DES);
    if (iRet != 0) 
    {
        WriteLog(ERROR, "encrypt pin failed!" );
#ifdef DBGMODE    
        printf("encrypt pin failed!\n" );
#endif
        return;
    }

    /* 取交易流水号(ASC码) */
    memset(szTraceNo, 0x00, sizeof(szTraceNo));
    sprintf(szTraceNo, "%06ld", tParamStru[iPosNo].iTrace);
    tParamStru[iPosNo].iTrace++;

    /* 组交易报文包 */
    memcpy(szBuff, guszTermPacket, PACKET_LEN);

    iMacDataLen = ((unsigned char)szBuff[23])*256 + 
           (unsigned char)szBuff[24];    //消息长度
    iMacDataLen = iMacDataLen-8;                //扣除MAC8个字节

    /* 安全模块号 offset=34 */
    memcpy(szBuff+REQ_PSAM_POS, tParamStru[iPosNo].szSecureUnit, 16);

    /* 流水号 50 */
    AscToBcd(szTraceNo, 6, 0, szBuff+REQ_TRACE_POS);

    /* 二磁道 */
    memcpy(szBuff+REQ_TRACK2_POS, tParamStru[iPosNo].szTrack2, 19);

    /* 三磁道 */
    memcpy(szBuff+REQ_TRACK3_POS, tParamStru[iPosNo].szTrack3, 52);

    /* 密码 */
    memcpy(szBuff+REQ_PIN_POS, szEncryptPin, 8);

    /* 计算MAC */
    memcpy( szMacData, szBuff+REQ_MAC_SKIP, iMacDataLen );
    Mac_Normal(tParamStru[iPosNo].mac_key, szMacData, iMacDataLen, szMac, 
        TRIPLE_DES);

    /* MAC */
    memcpy(szBuff+REQ_MAC_POS, szMac, 8);

    iLen = PACKET_LEN;    

    szOutData[0] = iLen/256;    
    szOutData[1] = iLen%256;    
    memcpy( szOutData+2, szBuff, iLen );
}

/*****************************************************************
 ** 功    能：初始化参数配置表
 ** 输入参数：
 ** 输出参数：
 ** 返 回 值：
 ** 作    者：
 ** 日    期：
 ** 修改日志：
 ****************************************************************/
 #define CONFIG_FILE "Simconfig"
 #define CONFIG_SECTION "simpos"
 
int InitParamTable()
{
    int i;
    int iRet;
    char szTmpBuf[1024], szFieldName[100];
    unsigned char szTmp[200];

    /* 开始初始化操作,读取配置文件中的参数 */
    printf("\nBegin param init ... \n");

    memset(&tCommStru, 0x00, sizeof(tCommStru));
    
    memset(szTmpBuf, 0x00, 100);
    memset(szFieldName, 0x00, sizeof(szFieldName));
    strcpy(szFieldName, "HostIP");
    iRet = ReadConfig( CONFIG_FILE, CONFIG_SECTION, szFieldName, szTmpBuf );
    if (iRet != 0)    {    /*不成功的话*/
        WriteLog( ERROR, "Simconfig参数配置错误[%s]", szFieldName);
        return(-1);
    }
    strcpy(tCommStru.szHostIP, szTmpBuf);

    memset(szTmpBuf, 0x00, 100);
    memset(szFieldName, 0x00, sizeof(szFieldName));
    strcpy(szFieldName, "HostPORT");
    iRet = ReadConfig( CONFIG_FILE, CONFIG_SECTION, szFieldName, szTmpBuf );
    if (iRet != 0)    {    /*不成功的话*/
        WriteLog( ERROR, "Simconfig参数配置错误[%s]", szFieldName);
        return(-1);
    }
    tCommStru.iHostPort = atoi(szTmpBuf);

    /*给结构数组清零*/
    memset(tParamStru, 0x00, sizeof(tParamStru)*MAX_POS_NUM);

    /*循环读取参数配置表中的值*/
    for (i=0; i<MAX_POS_NUM; i++) {
        memset(szTmpBuf, 0x00, 100);
        memset(szFieldName, 0x00, sizeof(szFieldName));
        sprintf(szFieldName, "Track2_%d", i+1);
        
        iRet = ReadConfig( CONFIG_FILE, CONFIG_SECTION, szFieldName, szTmpBuf );
        if (iRet != 0)    {    /*不成功的话*/
            WriteLog( ERROR, "Simconfig参数配置错误[%s]", szFieldName);
            return(-1);
        }
        AscToBcd(szTmpBuf, 19, 0, tParamStru[i].szTrack2);

        memset(szTmpBuf, 0x00, 100);
        memset(szFieldName, 0x00, sizeof(szFieldName));
        sprintf(szFieldName, "Track3_%d", i+1);
        
        iRet = ReadConfig( CONFIG_FILE, CONFIG_SECTION, szFieldName, szTmpBuf );
        if (iRet != 0)    {
            WriteLog( ERROR, "Simconfig参数配置错误[%s]", szFieldName);
            return(-1);
        }
        AscToBcd(szTmpBuf, 52, 0, tParamStru[i].szTrack3);

        strcpy(tParamStru[i].szPIN, "000000");
    }
    printf("\nread param succ... \n");

    return(0);
}

/*****************************************************************
 ** 功    能：从磁道数据获取帐户号
 ** 输入参数：
 **           szTrackData:磁道数据信息
 ** 输出参数：
 **           szAccNo:获取到的帐户号
 ** 返 回 值：
 ** 作    者：
 ** 日    期：
 ** 修改日志：
 ****************************************************************/
#define MAX_ACCOUNT_NO_LEN 19
void getAccNoFromTrackData(char * szTrackData, char * szAccNo)
{
	char szTrackDataAsc[200] = {0};
	int i;
	BcdToAsc(szTrackDataAsc,80,0,szTrackData);
	for (i=0;i<MAX_ACCOUNT_NO_LEN+1;i++)
	{
		if (szTrackDataAsc[i]=='D')
			break;
	}
	memcpy(szAccNo,szTrackDataAsc,i);
}
