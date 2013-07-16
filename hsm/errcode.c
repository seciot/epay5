/******************************************************************
** Copyright(C)2006 - 2008联迪商用设备有限公司
** 主要内容:各类型加密机错误码转换
           
** 函数列表:
** 创 建 人:Robin 
** 创建日期:2009/08/29


$Revision: 1.10 $
$Log: errcode.c,v $
Revision 1.10  2012/12/05 06:32:14  wukj
*** empty log message ***

Revision 1.9  2012/12/03 03:24:46  wukj
int类型前缀修改为i

Revision 1.8  2012/11/29 07:51:43  wukj
修改日志函数,修改ascbcd转换函数

Revision 1.7  2012/11/29 01:57:55  wukj
日志函数修改

Revision 1.6  2012/11/21 04:13:38  wukj
修改hsmincl.h 为hsm.h

Revision 1.5  2012/11/21 03:20:31  wukj
1:加密机类型名字修改 2: 全局变量移至hsmincl.h


*******************************************************************/

#include "hsm.h"

/*****************************************************************
** 功    能:转化SJL05加密机的一位错误码为平台返回码 
** 输入参数:
           uszHsmErrCode    加密机返回的一位错误码
** 输出参数:
           szRetCode       已经分类的四位错误码
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int DispSjl05ErrorMsg( unsigned char *uszHsmErrCode, char *szRetCode)
{
    char    szMsg[100], szStr[100];

    strcpy( szMsg, "加密机返回 " );

    switch ( uszHsmErrCode[0] )
    {

    case 0x00:
        strcpy(szRetCode, TRANS_SUCC);
        return SUCC;
    case 0x01:
        strcat( szMsg, "无本地主密钥" );
        break;
    case 0x02:
        strcat( szMsg, "无银行主密钥" );
        break;
    case 0x05:
        strcat( szMsg, "无终端主密钥" );
        break;
    case 0x2E:
        strcat( szMsg, "无效的MAC长度" );
        break;
    case 0x32:
        strcat( szMsg, "无效的密钥长度" );
        break;
    case 0x0A:
        strcat( szMsg, "非法命令" );
        break;
    case 0x0C:
        strcat( szMsg, "非法银行主密钥索引" );
        break;
    case 0x10:
        strcat( szMsg, "无效的算法模式" );
        break;
    case 0x19:
        strcat( szMsg, "奇偶校验错误" );
        break;
    case 0x20:
        strcat( szMsg, "非法的附加信息" );
        break;
    case 0x2C:
        strcat( szMsg, "非法银行主密钥" );
        break;
    case 0x59:
        strcat( szMsg, "输入的数据长度错误" );
        break;
    case 0x5D:
        strcat( szMsg, "输入长度错" );
        break;
    case 0x61:
        strcat( szMsg, "消息太短" );
        break;
    case 0x62:
        strcat( szMsg, "消息太长" );
        break;
    case 0xb2:
        strcat( szMsg, "消息发送错误" );
        break;
    case 0xb3:
        strcat( szMsg, "消息接收错误" );
        break;
    case 0xb4:
        strcat( szMsg, "接受消息长度错误" );
        break;
    default:
        sprintf( szStr, "hsm system error %02x", uszHsmErrCode[0]&0xFF );
        strcat( szMsg, szStr );
        break;
    }
    WriteLog( ERROR, szMsg );
    strcpy( szRetCode, ERR_SYSTEM_ERROR );

    return FAIL;
}

/*****************************************************************
** 功    能:转化银联专用加密机的一位错误码为平台返回码 
** 输入参数:
           uszHsmErrCode    加密机返回的一位错误码
** 输出参数:
           szRetCode       已经分类的四位错误码
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int DispUphsmErrorMsg( unsigned char *uszHsmErrCode, char *szRetCode )
{
    int iErr;
    char    szMsg[100], szTmpStr[100];

    uszHsmErrCode[2] = 0;

    iErr = atol( uszHsmErrCode );

    sprintf( szMsg, "加密机返回%02ld，", iErr );
    switch ( iErr )
    {

    case 0: 
        strcat( szMsg, "hsm运算成功" );
        break;
    case 1:
        strcat( szMsg, "hsm无主密钥" );
        break;
    case 2:
        strcat( szMsg, "hsm无工作密钥1" );
        break;
    case 3:
        strcat( szMsg, "hsm无工作密钥2" );
        break;
    case 4:
        strcat( szMsg, "hsm工作密钥1奇偶校验错" );
        break;
    case 5:
        strcat( szMsg, "hsm工作密钥2奇偶校验错" );
        break;
    case 6:
        strcat( szMsg, "hsm无老的主密钥" );
        break;
    case 10:
        strcat( szMsg, "hsm口令错" );
        break;
    case 11:
        strcat( szMsg, "hsm密码机不在授权状态" );
        break;
    case 12:
        strcat( szMsg, "hsm没有插IC卡(从串行口进入密钥管理时要插A卡)" );
        break;
    case 13:
        strcat( szMsg, "hsm写IC卡错" );
        break;
    case 14:
        strcat( szMsg, "hsm读IC卡错" );
        break;
    case 15:
        strcat( szMsg, "hsmIC卡不配套" );
        break;
    case 16:
        strcat( szMsg, "hsm打印机没准备好" );
        break;
    case 17:
        strcat( szMsg, "hsmIC卡未格式化" );
        break;
    case 18:
        strcat( szMsg, "hsm打印格式没定义" );
        break;
    case 20:
        strcat( szMsg, "hsmMAC校验错" );
        break;
    case 21:
        strcat( szMsg, "hsmMAC标志指示域错" );
        break;
    case 22:
        strcat( szMsg, "hsm密钥长度与使用模式不符" );
        break;
    case 23:
        strcat( szMsg, "hsmMAC模式指示域错" );
        break;
    case 24:
        strcat( szMsg, "hsm数据长度指示域错" );
        break;
    case 26:
        strcat( szMsg, "hsm加密模式指示域错" );
        break;
    case 27:
        strcat( szMsg, "hsm加解密标志错" );
        break;
    case 28:
        strcat( szMsg, "hsmPIN格式错" );
        break;
    case 29:
        strcat( szMsg, "hsmPIN检查长度大于实际PIN长度" );
        break;
    case 31:
        strcat( szMsg, "hsm工作密钥1标志错" );
        break;
    case 32:
        strcat( szMsg, "hsm工作密钥2标志错" );
        break;
    case 33:
        strcat( szMsg, "hsm工作密钥索引错" );
        break;
    case 34:
        strcat( szMsg, "hsm密钥离散次数错" );
        break;
    case 35:
        strcat( szMsg, "hsmPIN参考值校验错" );
        break;
    case 36:
        strcat( szMsg, "hsm主帐号参考值校验错" );
        break;
    case 37:
        strcat( szMsg, "hsmPIN校验错" );
        break;
    case 38:
        strcat( szMsg, "hsmPIN长度错[小于4或者大于12]" );
        break;
    case 39:
        strcat( szMsg, "hsmCVN标志错" );
        break;
    case 40:
        strcat( szMsg, "hsmDES算法模块错误" );
        break;
    case 41:
        strcat( szMsg, "hsmSSF33算法模块错误" );
        break;
    case 60:
        strcat( szMsg, "hsm无此命令" );
        break;
    case 61:
        strcat( szMsg, "hsm消息太短" );
        break;
    case 62:
        strcat( szMsg, "hsm消息太长" );
        break;
    case 63:
        strcat( szMsg, "hsm消息检查值错" );
        break;
    case 76:
        strcat( szMsg, "hsm子网掩码无效" );
        break;
    case 77:
        strcat( szMsg, "hsm非法字符" );
        break;
    case 78:
        strcat( szMsg, "hsm文件尾" );
        break;
    case 79:
        strcat( szMsg, "hsm客户IP地址语法错" );
        break;
    default:
        sprintf( szTmpStr, "hsm error %ld", iErr );
        strcat( szMsg, szTmpStr );
        break;
    }
    if( iErr != 0 )
    {
        WriteLog( ERROR, szMsg );
        strcpy( szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    strcpy( szRetCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能:转化SJL06E加密机的一位错误码为平台返回码 
** 输入参数:
           uszHsmErrCode    加密机返回的一位错误码
** 输出参数:
           szRetCode       已经分类的四位错误码
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int DispSjl06eErrorMsg( unsigned char *uszHsmErrCode, char *szRetCode )
{
    int iErr;
    char    szMsg[100], szTmpStr[100];

    uszHsmErrCode[2] = 0;

    iErr = atol( uszHsmErrCode );

    sprintf( szMsg, "加密机返回%02ld，", iErr );
    switch ( iErr )
    {

    case 0: 
        strcat( szMsg, "hsm运算成功" );
        break;
    case 1:
        strcat( szMsg, "hsm输入密钥奇偶校验错误警告" );
        break;
    case 2:
        strcat( szMsg, "hsm密钥长度错误" );
        break;
    case 4:
        strcat( szMsg, "hsm无效密钥类型错误" );
        break;
    case 5:
        strcat( szMsg, "hsm无效密钥长度标识" );
        break;
    case 10:
        strcat( szMsg, "hsm源密钥奇偶校验错" );
        break;
    case 11:
        strcat( szMsg, "hsm目的密钥奇偶校验错" );
        break;
    case 12:
        strcat( szMsg, "hsm用户存储区域的内容无效" );
        break;
    case 13:
        strcat( szMsg, "hsm主密钥奇偶校验错" );
        break;
    case 14:
        strcat( szMsg, "hsmLMK对 02-03 加密下的 PIN失效" );
        break;
    case 15:
        strcat( szMsg, "hsm无效的输入数据" );
        break;
    case 16:
        strcat( szMsg, "hsm控制台或打印机没有准备好或者没有连接好" );
        break;
    case 17:
        strcat( szMsg, "hsmHSM 不在授权状态，或者不能清除 PIN输出" );
        break;
    case 18:
        strcat( szMsg, "hsm没有装载文档格式定义" );
        break;
    case 19:
        strcat( szMsg, "hsm指定的 Diebold 表无效" );
        break;
    case 20:
        strcat( szMsg, "hsmPIN块没有包含有效值" );
        break;
    case 21:
        strcat( szMsg, "hsm无效的索引值" );
        break;
    case 22:
        strcat( szMsg, "hsm无效的账号" );
        break;
    case 23:
        strcat( szMsg, "hsm无效的 PIN块格式代码" );
        break;
    case 24:
        strcat( szMsg, "hsmPIN长度小于 4 或大于12" );
        break;
    case 25:
        strcat( szMsg, "hsm十进制表错误" );
        break;
    case 26:
        strcat( szMsg, "hsm无效的密钥方案" );
        break;
    case 27:
        strcat( szMsg, "hsm不匹配的密钥长度" );
        break;
    case 28:
        strcat( szMsg, "hsm无效的密钥类型" );
        break;
    case 29:
        strcat( szMsg, "hsm密钥函数被禁止" );
        break;
    case 30:
        strcat( szMsg, "hsm参考数无效" );
        break;
    case 31:
        strcat( szMsg, "hsm没有足够的请求入口以提供批量处理" );
        break;
    case 33:
        strcat( szMsg, "hsmLMK密钥转换存储区被破坏" );
        break;
    case 40:
        strcat( szMsg, "hsm无效的固件校验和" );
        break;
    case 41:
        strcat( szMsg, "hsm内部的硬件/软件错" );
        break;
    case 42:
        strcat( szMsg, "hsmDES错误" );
        break;
    case 47:
        strcat( szMsg, "hsmDSP 错误；报告给管理员" );
        break;
    case 49:
        strcat( szMsg, "hsm私钥错误；报告给管理员" );
        break;
    case 60:
        strcat( szMsg, "hsm无此命令" );
        break;
    case 74:
        strcat( szMsg, "hsm无效摘要信息语法" );
        break;
    case 75:
        strcat( szMsg, "hsm无效公钥/私钥对" );
        break;
    case 76:
        strcat( szMsg, "hsm公钥长度错误" );
        break;
    case 77:
        strcat( szMsg, "hsm明文数据块错误" );
        break;
    case 78:
        strcat( szMsg, "hsm私钥长度错误" );
        break;
    case 79:
        strcat( szMsg, "hsm哈希算法对象标识错误" );
        break;
    case 80:
        strcat( szMsg, "hsm数据长度错误" );
        break;
    case 81:
        strcat( szMsg, "hsm哈希算法对象标识错误" );
        break;
    case 90:
        strcat( szMsg, "hsmHSM 接收的请求信息中的数据奇偶校验错" );
        break;
    case 91:
        strcat( szMsg, "hsm纵向冗余校验错" );
        break;
    case 92:
        strcat( szMsg, "hsm计数值（命令/数据域）不在有效范围内" );
        break;
    default:
        sprintf( szTmpStr, "hsm error %ld", iErr );
        strcat( szMsg, szTmpStr );
        break;
    }
    if( iErr != 0 )
    {
        WriteLog( ERROR, szMsg );
        strcpy( szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    strcpy( szRetCode, TRANS_SUCC );

    return SUCC;
}

/*****************************************************************
** 功    能:转化SJL06E加密机的一位错误码为平台返回码 
** 输入参数:
           uszHsmErrCode    加密机返回的一位错误码
** 输出参数:
           szRetCode       已经分类的四位错误码
** 返 回 值:
           成功 - SUCC
           失败 - FAIL
** 作    者:Robin     
** 日    期:2009/08/25 
** 调用说明:
** 修改日志:mod by wukj 20121031规范命名及排版修订
**          
****************************************************************/
int DispSjl06eRacalErrorMsg( unsigned char *uszHsmErrCode, char *szRetCode )
{
    int iErr;
    char    szMsg[100], szTmpStr[100];

    uszHsmErrCode[2] = 0;

    iErr = atol( uszHsmErrCode );

    sprintf( szMsg, "加密机返回%02ld，", iErr );
    switch ( iErr )
    {

    case 0: 
        strcat( szMsg, "hsm运算成功" );
        break;
    case 1:
        strcat( szMsg, "hsm输入密钥奇偶校验错误警告" );
        break;
    case 2:
        strcat( szMsg, "hsm密钥长度错误" );
        break;
    case 4:
        strcat( szMsg, "hsm无效密钥类型错误" );
        break;
    case 5:
        strcat( szMsg, "hsm无效密钥长度标识" );
        break;
    case 10:
        strcat( szMsg, "hsm源密钥奇偶校验错" );
        break;
    case 11:
        strcat( szMsg, "hsm目的密钥奇偶校验错" );
        break;
    case 12:
        strcat( szMsg, "hsm用户存储区域的内容无效" );
        break;
    case 13:
        strcat( szMsg, "hsm主密钥奇偶校验错" );
        break;
    case 14:
        strcat( szMsg, "hsmLMK对 02-03 加密下的 PIN失效" );
        break;
    case 15:
        strcat( szMsg, "hsm无效的输入数据" );
        break;
    case 16:
        strcat( szMsg, "hsm控制台或打印机没有准备好或者没有连接好" );
        break;
    case 17:
        strcat( szMsg, "hsmHSM 不在授权状态，或者不能清除 PIN输出" );
        break;
    case 18:
        strcat( szMsg, "hsm没有装载文档格式定义" );
        break;
    case 19:
        strcat( szMsg, "hsm指定的 Diebold 表无效" );
        break;
    case 20:
        strcat( szMsg, "hsmPIN块没有包含有效值" );
        break;
    case 21:
        strcat( szMsg, "hsm无效的索引值" );
        break;
    case 22:
        strcat( szMsg, "hsm无效的账号" );
        break;
    case 23:
        strcat( szMsg, "hsm无效的 PIN块格式代码" );
        break;
    case 24:
        strcat( szMsg, "hsmPIN长度小于 4 或大于12" );
        break;
    case 25:
        strcat( szMsg, "hsm十进制表错误" );
        break;
    case 26:
        strcat( szMsg, "hsm无效的密钥方案" );
        break;
    case 27:
        strcat( szMsg, "hsm不匹配的密钥长度" );
        break;
    case 28:
        strcat( szMsg, "hsm无效的密钥类型" );
        break;
    case 29:
        strcat( szMsg, "hsm密钥函数被禁止" );
        break;
    case 30:
        strcat( szMsg, "hsm参考数无效" );
        break;
    case 31:
        strcat( szMsg, "hsm没有足够的请求入口以提供批量处理" );
        break;
    case 33:
        strcat( szMsg, "hsmLMK密钥转换存储区被破坏" );
        break;
    case 40:
        strcat( szMsg, "hsm无效的固件校验和" );
        break;
    case 41:
        strcat( szMsg, "hsm内部的硬件/软件错" );
        break;
    case 42:
        strcat( szMsg, "hsmDES错误" );
        break;
    case 47:
        strcat( szMsg, "hsmDSP 错误；报告给管理员" );
        break;
    case 49:
        strcat( szMsg, "hsm私钥错误；报告给管理员" );
        break;
    case 60:
        strcat( szMsg, "hsm无此命令" );
        break;
    case 74:
        strcat( szMsg, "hsm无效摘要信息语法" );
        break;
    case 75:
        strcat( szMsg, "hsm无效公钥/私钥对" );
        break;
    case 76:
        strcat( szMsg, "hsm公钥长度错误" );
        break;
    case 77:
        strcat( szMsg, "hsm明文数据块错误" );
        break;
    case 78:
        strcat( szMsg, "hsm私钥长度错误" );
        break;
    case 79:
        strcat( szMsg, "hsm哈希算法对象标识错误" );
        break;
    case 80:
        strcat( szMsg, "hsm数据长度错误" );
        break;
    case 81:
        strcat( szMsg, "hsm哈希算法对象标识错误" );
        break;
    case 90:
        strcat( szMsg, "hsmHSM 接收的请求信息中的数据奇偶校验错" );
        break;
    case 91:
        strcat( szMsg, "hsm纵向冗余校验错" );
        break;
    case 92:
        strcat( szMsg, "hsm计数值（命令/数据域）不在有效范围内" );
        break;
    default:
        sprintf( szTmpStr, "hsm error %ld", iErr );
        strcat( szMsg, szTmpStr );
        break;
    }
    if( iErr != 0 )
    {
        WriteLog( ERROR, szMsg );
        strcpy( szRetCode, ERR_SYSTEM_ERROR );
        return FAIL;
    }

    strcpy( szRetCode, TRANS_SUCC );

    return SUCC;
}
