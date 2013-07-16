#
# Make the Proc PosUpInfo
#

.SUFFIXES:
.SUFFIXES: .pc .c .o

include ../makeinc/make.inc

USER_LIB= -L$(WORKDIR)/lib -lepay -ldb -lpub
INC_DIR = -I$(WORKDIR)/incl -I$(DATABASE_INCPATH)
LIB_DIR = -L../lib -L$(DATABASE_LIBPATH)
LIB_LIST = $(DB_LIBS) $(UNIXLIB) $(NETLIB) $(USER_LIB)

BINDIR  = $(WORKDIR)/bin

OBJS =  sock.o errcode.o hsm.o simhsm.o unionpayhsm.o Sjl06eRacal.o sjl05.o Sjl06Jk.o Sjl06s.o 

hsm: $(OBJS)
	$(CC) $(OFLAGS) -o $@ ${OBJS} ${INC_DIR} ${LIB_DIR} ${LIB_LIST}
	@size   $@
	@mv $@ ${BINDIR}
	@rm ${OBJS}
	@echo "make $@ ok!"

.c.o:
	${CC} $(CFLAGS) ${INC_DIR} $<

.pc.o:
	${PRECOMPILER} $(PRECOMPFLAGS) parse=none include=$(DATABASE_INCPATH) $<
	${CC} $(CFLAGS) ${INC_DIR} $*.c
	@rm $*.c $*.lis

clean :
	@-rm ${OBJS}

#
# End.
#
