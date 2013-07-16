hsm.c      根据msgtype按照优先级从消息队列ToHsm读取交易请求，读到请求后调用 
           process进行交易处理.

sock.c     与加密机通讯函数

sjl06.c	   SJL06(56所)加密机接口函数

sjl05.c	   SJL05(30所)加密机接口函数

sjl06s.c   邳州使用的小加密机

sjl06Jk.c  SJL06金卡指令

simhsm.c   模拟加密机加密运算函数

errcode.c  将加密机返回码转化为系统统一的返回码

secure.c   基本加密运算工具
