
#---------------------------------------------
# rules of making *.ec, DON'T change anytime !
.c.o:
	cc	 -s -D$(OS) -O	-c -I$(INCL)	$<
.ec.o:
	esql	 -s -D$(OS) -O	-c -I$(INCL)	$<
	@rm	$*.c
.SUFFIXES:	.ec	.o
#---------------------------------------------

OS=UNIX
TERMLIB=curses
MAKEFILE = makefile
INCL=$(HOME)/incl
LIBS= -L$(HOME)/lib -lipc -lsafe -lpublic -lx -lm -lsocket

OBJ =  sock.o errcode.o hsm.o simhsm.o unionpayhsm.o

hsm: $(OBJ)
	@echo Compiling the program $@
	cc -o $@  $(OBJ) $(LIBS) 
	@echo "---$@ compiling done."
	@rm *.o
	@mv $@ ../bin

