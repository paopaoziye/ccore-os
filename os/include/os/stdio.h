#ifndef OS_STDIO_H__
#define OS_STDIO_H__

#include "os.h"
#include <stdarg.h>
#include <stddef.h>
#include "sbi.h"

//函数接口
int _vsnprintf(char * out, size_t n, const char* s, va_list vl);
int printf(const char* s, ...);
char getchar();

//文件描述符
typedef enum std_fd_t
{
    stdin,  
    stdout,
    stderr,
}std_fd_t;
#endif