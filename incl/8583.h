/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：8583报文处理头文件
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.1 $
 * $Log: 8583.h,v $
 * Revision 1.1  2012/12/17 07:18:58  fengw
 *
 * 1、将基础库、EPAY库中头文件移至$WORKDIR/incl目录。
 *
 * Revision 1.4  2012/12/04 02:05:42  chenjr
 * 代码规范化
 *
 * Revision 1.3  2012/11/26 08:54:41  yezt
 * *** empty log message ***
 *
 * Revision 1.2  2012/11/26 03:13:40  chenjr
 * 添加规则
 *
 * Revision 1.1  2012/11/20 03:23:45  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#ifndef _8583
#define _8583

#define DBUFSIZE 512

#define FIELDLENTYPE_ASC  1
#define FIELDLENTYPE_BCD  2
#define FIELDLENTYPE_HEX  3

#define MSGIDTYPE_BCD  2
#define MSGIDTYPE_ASC  4

/* ISO 8583 Message Structure Definitions */

/* 报文具体每个域规则 */
struct  ISO_8583 
{              
    int            len;   /* data element max length */
    unsigned char  type;  /* bit0--C/D数据, bit1--n左对齐, 
                             bit2--z右对齐 bit3--BIN DATA*/
    unsigned char  flag;  /* length field length: 0--固定 
                             1--LLVAR型 2--LLLVAR型*/
};

/* 报文规则 */
typedef struct
{
    short   iMidType;        /* MessageID类型  ASC or BCD  */
    short   iFieldLenType;   /* 域长度类型 ASC、BCD or HEX */
    struct  ISO_8583 *ptISO; /* ISO结构 */
}MsgRule;


struct data_element_flag {
    short bitf;
    short len;
    int   dbuf_addr;
};

typedef struct  {
    struct  data_element_flag f[128];
    short   off;
    char    dbuf[512];
    char    message_id[10];
} ISO_data;


#endif

