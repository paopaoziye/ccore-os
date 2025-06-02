#include "string.h"

/* 计算字符串的长度 */
size_t strlen(const char* str){
    char *ptr = (char *)str;
    while(*ptr != EOS){
        ptr++;
    }
    return ptr-str;
}
/* 将src的n个字节复制到dest */
void* memcpy(void* dest,const void *src,size_t count){
    char *ptr = dest;
    while(count--){
        *ptr++ = *((char*)(src++));
    }
    return dest;
}