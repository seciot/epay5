# 
# Make the Embedded SQL/C programs
#

.SUFFIXES:
.SUFFIXES: .ec .c .o
.SUFFIXES: .c .o

include ../../makeinc/make.inc

INC_DIR = -I. -I../../incl -I$(DATABASE_INCPATH)
LIB_DIR = ../../lib

OBJS = dbop.o

libdb.a: $(OBJS)
	ar q $@ $(OBJS)
	@echo "make $@ ok!"
	@rm *.o
	@> tp12345
	@rm tp* 
	@mv $@ $(LIB_DIR)
	@cp libdb.h ../../incl

.c.o:
	${CC} $(CFLAGS) ${INC_DIR} $<

.ec.o:
	${PRECOMPILER} $(PRECOMPFLAGS) parse=none include=$(DATABASE_INCPATH) $<
	$(CC) $(CFLAGS) $(INC_DIR) $*.c
	@rm $*.c $*.lis

clean :
	@-rm ${OBJS}
	@-rm $(LIB_DIR)/libdb.a

# End.
