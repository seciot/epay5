/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：系统软加密的主密钥合成头文件
 *  创 建 人：chenjr
 *  创建日期：2012/12/7
 * ----------------------------------------------------------------
 * $Revision: 1.1 $
 * $Log: genMasterKey.h,v $
 * Revision 1.1  2012/12/07 06:11:41  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#ifndef _GEN_MASTER_KEY_H
#define _GEN_MASTER_KEY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include "libpub.h"
#include "user.h"

#define MODELTITLE "\n\t\t系统软加密主密钥合成\n"

#define KEYLT_TITLE "\n\t一、请选择密钥长度\n"
#define KEYLT_DES1 "\t单倍长(16)------请按1\n"
#define KEYLT_DES2 "\t双倍长(32)------请按2\n"
#define KEYLT_DES3 "\t三倍长(48)------请按3\n"
#define KEYLT_INT  "\t中断操作并退出------请按q\n"
#define KEYLT_LEN  "\n\t一、密钥长度[%d]字节\n"
#define KEYLT_MENU KEYLT_TITLE KEYLT_DES1 KEYLT_DES2 KEYLT_DES3 KEYLT_INT


#define KEYCOM_TITLE "\n\t二、请选择密钥分量数(1-3)\n"
#define KEYCOM_INT  "\t中断操作并退出------请按q\n"
#define KEYCOM_NUM  "\n\t二、密钥分量数[%d]\n"
#define KEYCOM_MENU KEYCOM_TITLE KEYCOM_INT 


#define COMCON_HEAD MODELTITLE KEYLT_LEN KEYCOM_NUM
#define COMCON_INPUT "\n\t请输入分量%d内容\n"
#define COMCON_REIN  "\n\t请再次输入分量%d内容\n"
#define COMCON_ICON  "\n\t两次输入不一致,请重输\n"
#define COMCON_INLEN "\t已输入长度%d"

#define SAVECOM_SUCC "\n\t三、主密钥保存成功\n\n"
#define SAVECOM_FAIL "\n\t三、主密钥保存失败\n\n"

#endif /*_GEN_MASTER_KEY_H */
