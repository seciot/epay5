#
# Make the Proc finatran
#

.SUFFIXES:
.SUFFIXES: .ec .c .o

include ${WORKDIR}/makeinc/make.inc

USER_LIB = -lepay -ldb -lpub

INC_DIR = -I${WORKDIR}/incl -I${DATABASE_INCPATH} -I${ORACLE_HOME}/rdbms/public/
LIB_DIR = -L${WORKDIR}/lib -L${DATABASE_LIBPATH}
LIB_LIST = ${DB_LIBS} ${UNIXLIB} ${NETLIB} ${USER_LIB}

BINDIR  = ${WORKDIR}/bin

OBJS = package.o CalcMac.o PkgLogin.o PkgInquery.o PkgPurchase.o PkgAuto_void.o PkgPur_cancel.o PkgRefund.o PkgPre_auth.o PkgPre_cancel.o PkgConfirm.o
OBJS1 = sig_lt.o ${OBJS}
OBJS2 = dup_lt.o ${OBJS}
OBJS3 = dup_st.o ${OBJS}
CFLAGS += -DDB_ORACLE

all: tohost

sig_lt: ${OBJS1}
	${CC} ${OFLAGS} -o $@ ${OBJS1} ${INC_DIR} ${LIB_DIR} ${LIB_LIST}
	@size $@
	@mv $@ ${BINDIR}
	@rm *.o
	@> tp12345
	@rm tp*
	@echo "make $@ ok!"

dup_lt: ${OBJS2}
	${CC} ${OFLAGS} -o $@ ${OBJS2} ${INC_DIR} ${LIB_DIR} ${LIB_LIST}
	@size $@
	@mv $@ ${BINDIR}
	@rm *.o
	@> tp12345
	@rm tp*
	@echo "make $@ ok!"

tohost: ${OBJS3}
	${CC} ${OFLAGS} -o $@ ${OBJS3} ${INC_DIR} ${LIB_DIR} ${LIB_LIST}
	@size $@
	@mv $@ ${BINDIR}
	@rm *.o
	@> tp12345
	@rm tp*
	@echo "make $@ ok!"

.c.o:
	${CC} ${CFLAGS} ${INC_DIR} $<

.ec.o:
	${PRECOMPILER} ${PRECOMPFLAGS} parse=none include=${DATABASE_INCPATH} $<
	${CC} ${CFLAGS} ${INC_DIR} $*.c
	@rm $*.c $*.lis

clean :
	@-rm -f ${OBJS}

#
# End.
#
