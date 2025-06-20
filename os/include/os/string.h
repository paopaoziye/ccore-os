#ifndef OS_STRING_H__
#define OS_STRING_H__

#include "types.h"

//定义字符串的终止符
#define EOS '\0'

//函数声明
size_t strlen(const char* str);
void* memcpy(void* dest,const void *src,size_t count);
void* memset(void *dest,int ch,size_t count);
#endif
