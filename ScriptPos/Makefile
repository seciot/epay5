#
# Make the Proc ScriptPos
#

.SUFFIXES:
.SUFFIXES: .ec .c .o

include ${WORKDIR}/makeinc/make.inc

USER_LIB = -lepay -ldb -lpub

INC_DIR = -I${WORKDIR}/incl -I${DATABASE_INCPATH} -I${ORACLE_HOME}/rdbms/public/
LIB_DIR = -L${WORKDIR}/lib -L${DATABASE_LIBPATH}
LIB_LIST = ${DB_LIBS} ${UNIXLIB} ${NETLIB} ${USER_LIB}

BINDIR  = ${WORKDIR}/bin

OBJS = ScriptPos.o Return.o Print.o GetTermRec.o WriteCmdData.o CheckAuthCode.o GetTdiAddress.o \
       GetMenuData.o GetTmpOperationInfo.o UnpackScriptPos.o PackScriptPos.o PosOutput.o PosInput.o \
       InsertUpdateFirstPageCmd.o CheckFirstPage.o \
       ChkAppUpdate.o ChkTmsUpdate.o ChkTermInfo.o SendOutToPos.o GetNextTransCmd.o GetTrack.o \
       SpecialOutput.o SpecialInput.o

scriptpos: ${OBJS}
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
