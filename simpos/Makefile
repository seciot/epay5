#
# Make the Proc PosUpInfo
#

.SUFFIXES:
.SUFFIXES: .ec .c .o

include $(WORKDIR)/makeinc/make.inc

USER_LIB = -lpub -ldb -lepay

INC_DIR = -I$(WORKDIR)/incl -I$(DATABASE_INCPATH)
LIB_DIR = -L$(WORKDIR)/lib -L$(DATABASE_LIBPATH)
LIB_LIST = $(DB_LIBS) $(UNIXLIB) $(NETLIB) $(USER_LIB)

BINDIR  = $(WORKDIR)/bin

OBJS = simpos.o
CFLAGS += -DDB_ORACLE

simpos: $(OBJS)
	$(CC) $(OFLAGS) -o $@ ${OBJS} ${INC_DIR} ${LIB_DIR} ${LIB_LIST}
	@size   $@
	@mv $@ ${BINDIR}
	@rm ${OBJS}
	@> tp12345
	@rm tp*
	@echo "make $@ ok!"

.c.o:
	${CC} $(CFLAGS) ${INC_DIR} $<

.ec.o:
	${PRECOMPILER} $(PRECOMPFLAGS) parse=none include=$(DATABASE_INCPATH) $<
	${CC} $(CFLAGS) ${INC_DIR} $*.c
	@rm $*.c $*.lis

clean :
	@-rm ${OBJS}

#
# End.
#
