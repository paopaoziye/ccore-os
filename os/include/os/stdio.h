#ifndef OS_STDIO_H__
#define OS_STDIO_H__
#include <stdarg.h>
#include <stdarg.h>
#include <stddef.h>
#include "sbi.h"
//函数接口
int _vsnprintf(char * out, size_t n, const char* s, va_list vl);

#endif