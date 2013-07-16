/****************************
 *
 * 64位DES算法头文件
 *
 ****************************/

void DES(unsigned char *key,unsigned char *text,unsigned char *mtext);
void _DES(unsigned char *key,unsigned char *text,unsigned char *mtext);
void  TriDES(unsigned char * key ,unsigned char* source, unsigned char* target);
void  _TriDES(unsigned char * key ,unsigned char* source, unsigned char* target);
