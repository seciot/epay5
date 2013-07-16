/* ----------------------------------------------------------------
 *  Copyright(C)2006 - 2013 联迪商用设备有限公司
 *  主要内容：TLV数据格式处理库头文件
 *  创 建 人：
 *  创建日期：
 * ----------------------------------------------------------------
 * $Revision: 1.1 $
 * $Log: Tlv.h,v $
 * Revision 1.1  2012/12/17 07:18:58  fengw
 *
 * 1、将基础库、EPAY库中头文件移至$WORKDIR/incl目录。
 *
 * Revision 1.2  2012/12/13 05:06:24  fengw
 *
 * 1、增加注释。
 * 2、命名规范化。
 *
 * Revision 1.1  2012/11/20 03:27:37  chenjr
 * init
 *
 * ----------------------------------------------------------------
 */

#ifndef _TLV_H_
#define _TLV_H_

#define MAX_TLV_NUM         20              /* TLV数组最大个数 */

#define MAX_TAG_LEN         4               /* TAG最大占用字节数 */
#define MAX_LEN_LEN         4               /* LENGTH最大占用字节数 */
#define MAX_VALUE_LEN       2048            /* VALUE最大占用字节数 */

#define TAG_NORMAL          0               /* 普通类型，一个TAG占用一个字节 */
#define TAG_STANDARD        1               /* 标准类型，若Tag标签的第一个字节(字节按从左往右排序
                                               最左边的为第一个字节，bit顺序正好相反)的bit1-bit5
                                               为"11111"，则说明Tag长度不止一个字节，具体有几字节
                                               参考下一字节的bit8,如果bit8为1，再看下一字节的bit8
                                               直至下一字节的bit8为0止算是Tag的最后一个字节。
                                            */

#define LEN_NORMAL          0               /* 普通类型，长度占用一个字节(HEX格式) */
#define LEN_STANDARD        1               /* 标准类型，若Len最左边的bit位值为0时，
                                               Len值占用一个字节，bit7-bit1代表长度，取值
                                               范围为0-127；若Len最左边的bit位值为1时，
                                               Len值占用2-3个字节，bit7-bit1代表Len值占用字节
                                               数，例如，若最左字节为10000010，表示L字段除该
                                               字节外，后面还有两个字节。其后续字节的十进
                                               制取值表示子域取值的长度。
                                             */

#define VALUE_NORMAL        0               /* 普通类型，数据原样拷贝，不做处理 */
#define VALUE_BCD_RIGHT     1               /* BCD码压缩(右对齐)，数据以BCD码压缩，长度为奇数时，左补0对齐 */
#define VALUE_BCD_LEFT      2               /* BCD码压缩(左对齐)，数据以BCD码压缩，长度为奇数时，右补0对齐 */

#define DATA_NULL           0               /* 空值 */
#define DATA_NOTNULL        1               /* 非空值 */

typedef struct
{
    int     iFlag;                          /* 是否空值标志 */
    char    szTag[MAX_TAG_LEN+1];           /* TAG */
    int     iLen;                           /* LENGTH */
    char    szValue[MAX_VALUE_LEN+1];       /* VALUE */
} T_TLVData;

typedef struct
{
    T_TLVData   tTLVData[MAX_TLV_NUM];      /* TLV数组 */
    int         iTagType;                   /* TAG类型 */
    int         iLenType;                   /* LEN类型 */
    int         iValueType;                 /* VALUE类型 */
} T_TLVStru;

#endif