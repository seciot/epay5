#
## Description:  makefile of base libs
#
#
#.SUFFIXES:
.SUFFIXES: .c .o

include ../../makeinc/make.inc

INC_DIR = -I../../incl
LIB_DIR = ../../lib/

SRC_LIST = Ansix98.c Ansix99.c AscBcd.c Base64.c ChgAmount.c DateTime.c \
    Lrc.c Md5.c Msgq.c ReadConf.c Sem.c Shm.c SockCreate.c \
    SockRdWr.c String.c Tlv.c LogPrint.c 8583.c bitmap.c

ifneq ($(COMPILER_BITMOD), 64)
    SRC_LIST += qdes32.c
else
    SRC_LIST += qdes64.c
endif
    
OBJ_LIST = ${SRC_LIST:.c=.o}

all : libpub.a
libpub.a : $(OBJ_LIST)
	@echo "Compiling the lib $@"
	ar q $@ $(OBJ_LIST)
	@echo "---$@ loaded.\n"
	@mv $@ $(LIB_DIR)
	@-rm $(OBJ_LIST)


.c.o:
	$(CC) $(CFLAGS) $(INC_DIR) $<

clean:
	@-rm $(LIB_DIR)/libpub.a
	@-rm $(OBJ_LIST)

# End .
