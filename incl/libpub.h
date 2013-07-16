/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.1 $
 * $Log: libpub.h,v $
 * Revision 1.1  2012/12/17 07:18:58  fengw
 *
 * 1、将基础库、EPAY库中头文件移至$WORKDIR/incl目录。
 *
 * Revision 1.13  2012/12/13 05:07:02  fengw
 *
 * 1、Tlv命名规范化。
 *
 * Revision 1.12  2012/12/04 07:56:25  chenjr
 * 代码规范化
 *
 * Revision 1.11  2012/11/29 07:02:31  zhangwm
 *
 * 增加是否打印日志控制
 *
 * Revision 1.10  2012/11/29 01:56:41  zhangwm
 *
 * 增加设置交易信息进环境变量函数，供错误日志打印使用
 *
 * Revision 1.9  2012/11/29 01:11:27  zhangwm
 *
 * 修改WriteETLog为WriteLog
 *
 * Revision 1.8  2012/11/28 01:33:20  chenjr
 * 添加创建接口
 *
 * Revision 1.7  2012/11/27 03:36:31  zhangwm
 *
 * 修改头文件为内部使用
 *
 * Revision 1.6  2012/11/27 03:22:56  chenjr
 * 添加<sys/msg.h>头文件
 *
 * Revision 1.5  2012/11/26 06:45:35  zhangwm
 *
 *     将错误日志打印移植到公共库中
 *
 * Revision 1.4  2012/11/26 03:19:28  chenjr
 * 添加8583新接口
 *
 * Revision 1.3  2012/11/26 02:43:35  chenjr
 * 添加获取消息队列信息接口
 *
 * Revision 1.2  2012/11/20 07:50:39  chenjr
 * 去掉ReadConfig接口的默认值输入参数
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#ifndef _LIBPUB_H_
#define _LIBPUB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#include "Tlv.h"
#include "8583.h"

/* --------------------
 * [ASC-Bcd互转接口]
 * --------------------*/
    /*--ASCII转BCD */
    extern int AscToBcd(unsigned char  *uszAscBuf, int iAscLen,
                        unsigned char   ucType, unsigned char *uszBcdBuf);
    /*--BCD转ASCII */
    extern int BcdToAsc(unsigned char  *uszBcdBuf, int iAscLen,
             unsigned char   ucType,    unsigned char *uszAscBuf);


/* --------------------
 * [日志记录类接口]
 * --------------------*/
    /* 写错误日志和跟踪日志 */
    #ifdef LINUX
        void WriteLog( char* szFileInfo, int iLine, int iType, char* szFmt, ...);

    #else
        WriteLog(char* szFileInfo, int iLine, int iType, char* szFmt, va_dcl va_alist );
    #endif

    /* 设置错误日志中打印的交易信息 */
    int SetEnvTransId(char* pszTransId);

    /* 判断日志是否需要打印 */
    int IsPrint(int iType);

/* --------------------
 * [时间日期类接口] 
 * --------------------*/
    /*--取系统当前日期时间 格式自定义*/
    extern int GetSysDTFmt(const char *szFmt, char *szDTStr);

    /*--取系统当前日期 格式YYYYMMDD */
    extern int GetSysDate(char *szDateStr);
 
    /*--取系统当前时间 格式HHMMSS */
    extern int GetSysTime(char *szTimeStr);

    /*--取系统当前日期往前(后)几天的日期  格式YYYYMMDD */
    extern int GetDateSinceCur(int iDays, char *szDateStr);

    /*--检验日期格式是否合法(合法格式YYYYMMDD, 注意平闰年等) */
    extern int ChkDateFmt(char *szDateStr);


/* --------------------
 * [字符串处理类接口] 
 * --------------------*/
    extern char *DelHeadSpace(char *szStr);
    extern char *DelTailSpace(char *szStr);
    extern char *DelAllSpace(char *szStr);
    extern char *ToUpper(char *szStr);
    extern char *ToLower(char *szStr);
    extern int  IsNumber(char *szStr);
    extern int  GetField(char *szSrc, int iFieldNo, char cDivider, 
                         char *szDest);

/* --------------------
 * [网络类接口] 
 * --------------------*/
    /* 创建TCP客户端 */
    extern int CreateCliSocket(char *szSrv, char *szSrvPort);
    /* 创建TCP/UDP服务端 */
    extern int CreateSrvSocket(char *szSrvPort, char *szSrvType, 
                               int iQueLen);

    extern int SrvAccept(int iLisSock, char *szCliIp);

    /* 读定长数据 */
    extern int ReadSockFixLen(int iSockFd, int iTimeOut, int iLen, 
                              char *szBuf);

    extern int ReadSockVarLen(int iSockFd, int iTimeOut, char *szBuf);

    extern int WriteSock(int iSockFd, unsigned char *uszBuf, int iLen, 
                         int iTimeOut);

    extern int SendToUdpSrv(char *szIp, char *szPort, char *szBuf, 
                            int iLen);


/* --------------------
 * [安全类接口]
 * --------------------*/

    /* MD5 */
    extern void GenMD5(char *szSource, char *szDest);

    /* LRC */
    extern unsigned char Lrc(unsigned char *uszStr, int iLen);

    /* BASE64编码及解码 */
    extern int base64_encode(char *szSrc, unsigned int nSize,char *szDest);
    extern int base64_decode(char *szSrc, unsigned int nSize,char *szDest);

    /* ANSIX98加解密(PIN) */
    extern int ANSIX98(unsigned char *uszKey, char *szPan,  char *szPwd,
                       int iPwdLen, int iFlag, unsigned char *uszResult);

    extern int _ANSIX98(unsigned char *uszKey, char *szPan, 
                        unsigned char *uszPwd, int iFlag, 
                        unsigned char *uszResult);

    /* MAC计算,包括ANSIX99、919、简单异或 */
    extern void ANSIX99(unsigned char *uszMacKey, unsigned char *uszBuf, 
                        int iLen, int iAlg, unsigned char *uszMac);

    extern void ANSIX919(unsigned char *uszMacKey, unsigned char *uszBuf, 
                         int iLen, unsigned char *uszMac);

    extern void Mac_Normal(unsigned char *uszMacKey, unsigned char *uszBuf,
                           int iLen, int iAlg, unsigned char *uszMac);

    extern void XOR(unsigned char *uszInData, int iLen,
                    unsigned char *uszOutData );

    /* SINGLE_DES TRIPLE_DES 加解密 */
    extern void DES(unsigned char *uszKey,unsigned char *uszSrc,
                    unsigned char *uszDest);
    extern void _DES(unsigned char *uszKey,unsigned char *uszSrc,
                     unsigned char *uszDest);
    extern void TriDES(unsigned char *uszKey ,unsigned char *uszSrc, 
                       unsigned char *uszDest);
    extern void _TriDES(unsigned char *uszKey,unsigned char *uszSrc, 
                        unsigned char *uszDest);


/* --------------------
 * [交易金额0与点格式转换]
 * --------------------*/
    extern int ChgAmtDotToZero(char *szSrc, int iOutLen, int iPreFlag, 
                               char *szDest);
    extern int ChgAmtZeroToDot(char *szSrc, int iOutLen, char *szDest);


/* --------------------
 * [IPC接口]
 * --------------------*/
    /* 消息队列 */
    extern int CreateMsgQue(char *szFile,  int iId);
    extern int GetMsgQue(char *szFile,  int iId);
    extern int RmMsgQue(int iMsgid);
    extern int GetMsgQueStat(int iMsgid, struct msqid_ds *ptDs);
    extern int SndMsgToMQ(int iMsgid, long lMsgType, char *szSndBuf, 
                          int iSndLen);
    extern int RcvMsgFromMQ(int iMsgid, long lMsgType, int iTimeOut, 
                            char *szRcvBuf);

    /* 共享内存 */
    extern int CreateShm(char *szFile,  int iId, int iShmSize);
    extern int GetShm(char *szFile,  int iId, int iShmSize);
    extern char *AtShm(int iShmid);
    extern int RmShm(int iShmid);

    /* 信号量*/
    extern int CreateSem(char *szFile,  int iId, int iResource);
    extern int GetSem(char *szFile,  int iId);
    extern int SemOpera(int iSemid, int iResource);
    extern int P(int iSemid, int iResource);
    extern int V(int iSemid, int iResource);
    extern int RmSem(int iSemid);


/* --------------------
 * [配置文件读取接口]
 * --------------------*/
    extern int ReadConfig(char *szFilenm, char *szSection, char *szItem,
                          char *szVal);

/* --------------------
 * [8583接口]
 * --------------------*/
    extern void ClearBit(ISO_data *ptData);
    extern int GetBit(MsgRule *ptMR, ISO_data *ptData, int iNo, 
                      char *szDest);
    extern int SetBit(MsgRule *ptMR, char *szSrc, int iNo, int iLen, 
                      ISO_data *ptData);
    extern int IsoToStr(MsgRule *ptMR, ISO_data *ptData, 
                        unsigned char *szDest);
    extern int StrToIso(MsgRule *ptMR, unsigned char *szSrc, 
                        ISO_data *ptData);
    extern int DebugIso8583(MsgRule *ptMR, ISO_data *ptData, char *szDest);



/* --------------------
 * [TLV接口]
 * --------------------*/
    /* TLV配置初始化，设置Tag、Len、Value格式类型 */
    extern void InitTLV(T_TLVStru *pTLV, int iTagType,
                        int iLenType, int iValueType);

    /* 添加TLV格式数据 */
    extern int SetTLV(T_TLVStru *pTLV, char *szTag,
                      int iLen, char* szValue);

    /* 根据Tag值读取Value值 */
    extern int GetValueByTag(T_TLVStru *pTLV, char* szTag,
                             char* szValueBuf, int iBufSize);
  
    /* 将TLV格式数据打包成字符串 */
    extern int PackTLV(T_TLVStru *pTLV, char* szBuf);

    /* 将字符串解包为TLV格式数据 */
    extern int UnpackTLV(T_TLVStru *pTLV, char* szBuf, int iBufLen);

    /* 打印TLV包体内容(测试用) */
    extern char *DebugTLV(T_TLVStru *pTLV, char *szDest);

#endif  /*_LIBPUB_H_ */ 
