/*******************************************************************************
 * Copyright(C)2012－2015 福建联迪商用设备有限公司
 * 主要内容：ScriptPos模块 校验网关认证码
 * 创 建 人：Robin
 * 创建日期：2012/11/30
 * $Revision: 1.1 $
 * $Log: CheckAuthCode.c,v $
 * Revision 1.1  2013/01/18 08:32:37  fengw
 *
 * 1、拆分函数。
 *
 ******************************************************************************/

#define _EXTERN_

#include "ScriptPos.h"

/*******************************************************************************
 * 函数功能：校验网关认证码
 * 输入参数：
 *           uszAuthCode - 网关认证码
 *           szPsamNo    - 安全模块号
 * 输出参数：
 *           无
 * 返 回 值： 
 *           SUCC    -  通过校验 
 *           FAIL    -  未通过校验
 * 作    者：Robin
 * 日    期：2012/11/30
 * 修订日志：
 *
 ******************************************************************************/
int CheckAuthCode( uchar* uszAuthCode, char* szPsamNo )
{
    uchar uszPlain[8], uszCrypt[8];
    int i;

    XOR( szPsamNo, 16, uszPlain );

    TriDES( gszAuthKey, uszPlain, uszCrypt );

    if( memcmp(uszCrypt, uszAuthCode, 4) == 0 )
    {
        return SUCC;
    }
    else
    {
        WriteLog( ERROR, "auth code not right" );

        return FAIL;
    }
}