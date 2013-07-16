#
# This file is the Makefile include file
# Usage : Set the flags for OS Linux
#

OS_TYPE=LINUX
CC=cc
CFLAGS= -c  -D$(OS_TYPE) -m64 -DDEBUG
OFLAGS= -O2 -m64 
#UNIXLIB= -lclntsh -ldl -lm -lnsl -lirc -lncurses
UNIXLIB= 
NETLIB=
PRECOMPILER=$(ORACLE_HOME)/bin/proc
PRECOMPFLAGS= lines=yes unsafe_null=yes  DBMS=V8
#DB_LIBS= -lclntsh -ldl -lm -lnsl -lirc
DB_LIBS= -lclntsh
DATABASE_INCPATH=$(ORACLE_HOME)/precomp/public
DATABASE_LIBPATH=$(ORACLE_HOME)/lib

#
# End.
#
