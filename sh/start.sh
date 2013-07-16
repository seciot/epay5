#!/bin/sh

#读取数据库用户、密码
DBUSER=epay5
DBPWD=epay5
DBSERVICES=orcl

ORCL_CONN=$DBUSER/$DBPWD@$DBSERVICES

EPAY_USER=`whoami`

#判断平台是否已经启动
epayadm

if [ $? -eq 0 ]
then
    echo "平台已经启动，请先关闭!"

    exit
fi

#创建IPC
epayadm 0
if [ $? -ne 0 ]
then
    echo "启动平台失败!"

    exit
fi

#启动各模块进程

#查询需要启动模块进程名称、参数
TMP_FILE=.epay_module.tmp
sqlplus -S /nolog <<! 1>/dev/null
	conn $ORCL_CONN;
	set echo off feedback off heading off pagesize 0 linesize 1000 numwidth 12 termout off trimout on trimspool on;
	set colsep ' ';
	spool $TMP_FILE;
	SELECT module_name,para1,para2,para3,para4,para5,para6,msg_type FROM module WHERE run = 1 ORDER BY module_id;
	spool off;
	exit;
!

while read STR_CMD
do
    MODULE_NAME=`echo $STR_CMD | awk '{print $1}'` 

    echo "启动"$MODULE_NAME"模块!"

    $STR_CMD

    if [ $? -ne 0 ]
    then
        echo "**********模块"$MODULE_NAME"启动失败!**********"
    else
        echo "模块"$MODULE_NAME"启动成功!"
    fi

	sleep 1
done < $TMP_FILE

rm -f $TMP_FILE

echo "启动平台成功!"
