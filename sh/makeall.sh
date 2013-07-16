#!/bin/sh

#编译lib标志
FLAG_LIB=0

#编译bin标志
FLAG_BIN=0

#清理编译标志
FLAG_CLEAN=0

#当前编译标志 1:lib 2:bin
FLAG_CUR=0

#编译成功模块数
COUNT_SUCC=0

#编译失败模块数
COUNT_FAIL=0

MODULE_MAKE()
{
	MODULE=`pwd | awk -F "/" '{print $NF}'`

	if [ $MODULE = 'apache-tomcat-6.0.35' ]
	then
		return
	fi 

	if [ $MODULE = 'makeinc' -o $MODULE = 'sqllib' ]
	then
		return
	fi

	if [ $MODULE = 'libsrc' -a $FLAG_CUR -eq 2 ]
	then
		return
	fi

	if [ -f Makefile -o -f makefile ]
	then
		if [ $FLAG_CLEAN -eq 1 ]
		then
			make clean 1>/dev/null 2>/dev/null
		else
			make 1>/dev/null 2>/dev/null

			if [ $? -ne 0 ]
			then
				COUNT_FAIL=`expr $COUNT_FAIL + 1`

				echo "**********make $MODULE error**********"
			else
				COUNT_SUCC=`expr $COUNT_SUCC + 1`
			
				echo "make $MODULE ok!"
			fi	
		fi
	fi

	for DIR in `ls -l | grep ^d |  awk '{print $NF}'`
	do
		cd $DIR

		MODULE_MAKE

		cd ..
	done
}

if [ $# -eq 0 ]
then
	FLAG_LIB=1
	FLAG_BIN=1
fi

if [ $# -eq 1 -o $# -eq 2 ]
then
	if [ TEST$1 = 'TESTLIB' -o TEST$1 = 'TESTlib' ]
	then
		FLAG_LIB=1
	fi

	if [ TEST$1 = 'TESTBIN' -o TEST$1 = 'TESTbin' ]
	then
		FLAG_BIN=1
	fi

	if [ TEST$1 = 'TESTCLEAN' -o TEST$1 = 'TESTclean' ]
	then
		FLAG_LIB=1
		FLAG_BIN=1
		FLAG_CLEAN=1
	fi
	
	if [ TEST$2 = 'TESTCLEAN' -o TEST$2 = 'TESTclean' ]
	then
		FLAG_CLEAN=1
	fi
fi

if [ $# -ge 3 ]
then
	echo "$0 [lib|bin] [clean]"

	exit
fi

LOCAL_PWD=`pwd`

cd $WORKDIR/libsrc

if [ $FLAG_LIB -eq 1 ]
then
	echo "##################################################"
	echo "                     make lib                     "
	echo "##################################################"

	FLAG_CUR=1

	COUNT_SUCC=0

	COUNT_FAIL=0

	cd $WORKDIR/libsrc

	if [ ! -d $WORKDIR/lib ]
	then
		mkdir $WORKDIR/lib
	fi

	MODULE_MAKE

	echo "##################################################"
	echo "  make succ : $COUNT_SUCC                         "		
	echo "  make fail : $COUNT_FAIL                         "
	echo "##################################################"
fi

if [ $FLAG_BIN -eq 1 ]
then
	echo "##################################################"
	echo "                     make bin                     "
	echo "##################################################"

	FLAG_CUR=2

	COUNT_SUCC=0

	COUNT_FAIL=0

	#rm -f $WORKDIR/bin/*

	if [ ! -d $WORKDIR/bin ]
	then
		mkdir $WORKDIR/bin
	fi

	cd $WORKDIR

	MODULE_MAKE

	echo "##################################################"
	echo "  make succ : $COUNT_SUCC                         "
	echo "  make fail : $COUNT_FAIL                         "
	echo "##################################################"
fi

cd $LOCAL_PWD
