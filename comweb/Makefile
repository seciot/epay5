#
# Make the Proc comweb
#

.SUFFIXES:
.SUFFIXES: .ec .c .o

include ${WORKDIR}/makeinc/make.inc

USER_LIB = -lepay -ldb -lpub

INC_DIR = -I${WORKDIR}/incl -I${DATABASE_INCPATH} -I${ORACLE_HOME}/rdbms/public/
LIB_DIR = -L${WORKDIR}/lib -L${DATABASE_LIBPATH}
LIB_LIST = ${DB_LIBS} ${UNIXLIB} ${NETLIB} ${USER_LIB}

BINDIR  = ${WORKDIR}/bin

OBJS = comweb.o DownByPos.o DownByShop.o DownByAll.o DownByAppType.o DownByDept.o \
       SendWebReq.o SendWebRsp.o ProcWebTrans.o UnpackWebReq.o PackWebRsp.o \
       DownApp.o DownFirstPage.o DownFunction.o DownMenu.o DownOperation.o \
       DownPayList.o DownPrint.o DownPsamPara.o DownTermPara.o DownMsg.o

comweb: ${OBJS}
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
