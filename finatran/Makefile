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

OBJS = finatran.o GetEpayConf.o GetTransConf.o GetDeptConf.o GetShopConf.o GetPosConf.o \
	ChkEpayConf.o ChkCardType.o CalcFee.o \
	ChkShopValid.o ChkPosValid.o ChkTeleValid.o ChkValid.o GetSysTrace.o \
	ProcTrans.o SendReqToHost.o SendRspToPos.o \
	Login.o Purchase.o PreAuth.o PurCancel.o AutoVoid.o Inquery.o Refund.o Confirm.o \
	TFOutQuery.o TFOut.o TFOtherQuery.o TFOther.o PurNotice.o \
	CTCCQuery.o CTCCPrepay.o EmvPurTrans.o EmvPurOnline.o EmvInquery.o EmvPurCancel.o

finatran: ${OBJS}
	${CC} ${OFLAGS} -o $@ ${OBJS} ${INC_DIR} ${LIB_DIR} ${LIB_LIST}
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
