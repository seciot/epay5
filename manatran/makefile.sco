#---------------------------------------------
# rules of making *.ec, DON'T change anytime !
.c.o:
	cc	-s -I$(INCL)	-D$(OS) -O -c	$<
.ec.o:
	esql	-s -I$(INCL)	-D$(OS) -O -c	$<
	@rm	$*.c
.SUFFIXES:	.ec	.o
#---------------------------------------------

OS=UNIX
TERMLIB=curses

OBJS = manatran.o other.o

LIB =-L$(HOME)/lib -lregister -lepos -llsopt -lipc -lpublic -lsafe -lx
INCL=$(HOME)/incl

ESQL = esql -D_M_TERMINFO -D$(OS) -c -I$(INCL) -l$(TERMLIB) 
MAKEFILE = makefile

up:manatran
	@echo "manatran MAKED!"
manatran:$(OBJS) 
	esql -D_M_TERMINFO -o manatran $(OBJS) -I$(INCL) $(LIB) -l$(TERMLIB) 
	@rm *.o
	@mv manatran ../bin
