
#---------------------------------------------
# rules of making *.ec, DON'T change anytime !
.c.o:
	cc	-s	-D$(OS) -O	-c	-I$(INCL)	$<
.ec.o:
	esql	-s	-D$(OS) -O	-c	-I$(INCL)	$<
	@rm	$*.c
.SUFFIXES:	.ec	.o
#---------------------------------------------

INCL=$(HOME)/incl
LIBS=-L$(HOME)/lib -lepay -ldb -lpub -lx
OBJS= ScriptPos.o
MAKEFILE = makefile
OS=UNIX

up:ScriptPos
	@echo "ScriptPost MAKED!"
ScriptPos :$(OBJS) 
	esql -o ScriptPos $(OBJS) $(LIBS)  
	@rm *.o
	@mv ScriptPos ../bin

$(OBJS): $(HEAD) $(MAKEFILE)
