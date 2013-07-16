#
# Make the Proc PosUpInfo
#

.SUFFIXES:
.SUFFIXES: .pc .c .o

include ../makeinc/make.inc

USER_LIB= -L$(WORKDIR)/lib -lepay -lpub
INC_DIR = -I$(WORKDIR)/incl 
LIB_DIR = -L../lib 
LIB_LIST = $(USER_LIB)

BINDIR  = $(WORKDIR)/bin

OBJS = genMasterKey.o 

key: $(OBJS)
	$(CC) $(OFLAGS) -o $@ ${OBJS} ${INC_DIR} ${LIB_DIR} ${LIB_LIST}
	@size   $@
	@mv $@ ${BINDIR}
	@rm ${OBJS}
	@echo "make $@ ok!"

.c.o:
	${CC} $(CFLAGS) ${INC_DIR} $<

clean :
	@-rm ${OBJS}

#
# End.
#
