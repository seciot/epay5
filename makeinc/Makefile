# 
#  Use For Create the make.inc for this OS 
# 

OS_TYPE=`uname`

SRC_LIST = make.SCO_SV make.AIX

all : $(SRC_LIST)
	@-rm make.inc
	@cp make.$(OS_TYPE) make.inc

#
# Clean up after ourselves...
#
clean :
	@echo clean.

#
# End.
