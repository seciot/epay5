PATH=$PATH:$HOME/bin:$HOME/sh:$HOME/init
LANG=C

export PATH LANG
unset USERNAME

export WORKDIR=$HOME/epay5

export LOG_SWITCH=15

export ORACLE_BASE=/home/oracle
export ORACLE_HOME=$ORACLE_BASE/product/10g
export ORACLE_SID=orcl
export NLS_LANG=AMERICAN_AMERICA.ZHS16GBK

export LD_LIBRARY_PATH=$ORACLE_HOME/lib:/lib:/usr/lib:/usr/local/lib:$LD_LIBRARY_PATH
export PATH=$ORACLE_HOME/bin:$WORKDIR/bin:$WORKDIR/sh:$PATH

$WORKDIR/bin/dayend;
