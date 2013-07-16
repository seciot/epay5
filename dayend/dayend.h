#ifndef _DAYEND_H_
#define _DAYEND_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"
#include "dbtool.h"
#include "libdb.h"
#include "libpub.h"

#define E_TYPE 1
#define T_TYPE 2
#define H_TYPE 3
#define M_TYPE 4

#define CONFIG_FILE "Setup.ini"
#define CONFIG_SECTION "SECTION_PUBLIC"

#define GetCharParam(szItem, szValue) \
    do{ \
        if( ReadConfig( CONFIG_FILE, CONFIG_SECTION, szItem, szValue ) != SUCC ) \
        { \
            WriteLog(ERROR, "Setup≤Œ ˝≈‰÷√¥ÌŒÛ[%s]", szItem ); \
            return FAIL; \
        } \
    }while(0)

#define GetIntParam(szItem, iValue) \
    do{ \
        char szValue[80]; \
        if( ReadConfig( CONFIG_FILE, CONFIG_SECTION, szItem, szValue ) != SUCC ) \
        { \
            WriteLog(ERROR, "Setup≤Œ ˝≈‰÷√¥ÌŒÛ[%s]", szItem ); \
            return FAIL; \
        } \
        iValue = atoi(szValue); \
    }while(0)

#define GetLongParam(szItem, lValue) \
    do{ \
        char szValue[80]; \
        if( ReadConfig( CONFIG_FILE, CONFIG_SECTION, szItem, szValue ) != SUCC ) \
        { \
            WriteLog(ERROR, "Setup≤Œ ˝≈‰÷√¥ÌŒÛ[%s]", szItem ); \
            return FAIL; \
        } \
        lValue = atol(szValue); \
    }while(0)

#endif
