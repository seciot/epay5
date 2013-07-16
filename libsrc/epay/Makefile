# 
# Make the Embedded SQL/C programs
#

.SUFFIXES:
.SUFFIXES: .ec .c .o
.SUFFIXES: .c .o

include $(WORKDIR)/makeinc/make.inc

INC_DIR = -I. -I../../incl -I$(DATABASE_INCPATH)
LIB_DIR = ../../lib

OBJS = EpayShm.o EpaySem.o EpayMsg.o WriteStruLog.o TdiMatch.o HsmClient.o EpayLog.o \
	   MoniOpt.o PreInsertPosls.o InsertVoidls.o GetFullName.o SoftMasterKey.o \
	   GetResult.o FmtRetDesc.o GetTranInfo.o GetTranType.o GetCommands.o HostTermKey.o \
	   ChangeWorkKey.o ConvertPin.o

libepay.a: $(OBJS)
	ar q $@ $(OBJS)
	@echo "make $@ ok!"
	@rm *.o
	@> tp12345
	@rm tp* 
	@mv $@ $(LIB_DIR)

.c.o:
	${CC} $(CFLAGS) ${INC_DIR} $<

.ec.o:
	${PRECOMPILER} $(PRECOMPFLAGS) parse=none include=$(DATABASE_INCPATH) $<
	$(CC) $(CFLAGS) $(INC_DIR) $*.c
	@rm $*.c $*.lis

clean :
	@-rm ${OBJS}
	@-rm $(LIB_DIR)/libepay.a

# End.
