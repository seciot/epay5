# include <fcntl.h>
# include <string.h>
# include <errno.h>
# include <signal.h>
# include <sys/types.h>
# include <stdlib.h>
# include <stdio.h>
# include <setjmp.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <ctype.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <time.h>

# include "app.h"
# include "errcode.h"
# include "transtype.h"
//# include "tools.h"
# include "libpub.h"
# include "user.h"


#define SINGLE_DES  8
#define TRIPLE_DES  9

#define SJL06E_RACAL_HEAD_LEN           8
#define SJL06E_RACAL_HEAD_DATA         "12345678"

//加密机类型定义
#define SIM_HSM               0
#define SJL06E_RACAL_HSM      1
#define SJL05_RACAL_HSM       2
#define SJL06_UNIONPAY_HSM    3
#define SJL06_JK_HSM          4
#define SJL06S_JK_HSM         5

#define INVALID_SOCKET		-1

#ifdef _MAIN_
int    giSockFd;
int    giHsmType;
int    giSekHostWorkIndexSjl06ERacal,  giSekHostWorkIndexSjl06Up,   giSekHostWorkIndexSjl05Racal,   giSekHostWorkIndexSjl06SJk,   giSekHostWorkIndexSjl06Jk;
int    giSekHostTmkIndexSjl06ERacal,   giSekHostTmkIndexSjl06Up,    giSekHostTmkIndexSjl05Racal,    giSekHostTmkIndexSjl06SJk,    giSekHostTmkIndexSjl06Jk;
int    giSekTmkIndexSjl06ERacal,       giSekTmkIndexSjl06Up,        giSekTmkIndexSjl05Racal,        giSekTmkIndexSjl06SJk,        giSekTmkIndexSjl06Jk;
int    giSekWorkIndexSjl06ERacal ,     giSekWorkIndexSjl06Up ,      giSekWorkIndexSjl05Racal,       giSekWorkIndexSjl06SJk,       giSekWorkIndexSjl06Jk;
int    giTekTmkIndexSjl06ERacal,       giTekTmkIndexSjl06Up,        giTekTmkIndexSjl05Racal,        giTekTmkIndexSjl06SJk,        giTekTmkIndexSjl06Jk;
int    giTmkIndexSjl06ERacal,          giTmkIndexSjl06Up,           giTmkIndexSjl05Racal,           giTmkIndexSjl06SJk,           giTmkIndexSjl06Jk;
int    giTempBMKIndexSjl06Jk,giTempBMKIndexSjl06SJk;
char   gszMasterKeySim[17];
char   gszPospZMKSjl06ERacal[33], gszPoscZMKSjl06ERacal[33];
#endif

/*
 * Name:	safe.h
 */
 
#ifndef _SAFE
#define _SAFE


/*
 * Name:	ansix98.c
 */

/*
int A_ ( unsigned char *, unsigned char *, unsigned char *, int ) ;
int _A_ ( unsigned char *, unsigned char *, unsigned char * ) ;
*/


/*
 * Name:	ansix99.c
 */


/*
void ANSIX99 ( unsigned char *, unsigned char *, int, unsigned char * ) ;
void Mac_Normal ( unsigned char *, unsigned char *, int, unsigned char * ) ;
*/


/*
 * Name:	qdes.c
 */

 /* it is a special application, so don't define here 
 */

 
/*
 * Name:	rand.c
 */

void  GenerateSrand ( unsigned char *, long ) ;
void StringToSeed ( unsigned char *, long * ) ;


#endif

#define DED_OP 	1
#define EDE_OP 	2
#define XOR_OP 	3
#define NOT_OP 	4

