/*
 * 创建表空间和用户
 */
create tablespace epay5_data01
logging
datafile '/opt/oracle/dbfile/orcl/epay5/epay5_data01.dbf'
size 512m
autoextend on
next 32m maxsize 2048m
extent management local;

create temporary tablespace epay5_temp01
tempfile '/opt/oracle/dbfile/orcl/epay5/epay5_temp01.dbf'
size 512m
autoextend on
next 32m maxsize 2048m
extent management local;

create user epay5 identified by epay5
default tablespace epay5_data01
temporary tablespace epay5_temp01;

grant connect,resource to epay5;
