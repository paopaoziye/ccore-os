#ifndef OS_ASSERT_H
#define OS_ASSERT_H
#include "types.h"
#include "stdio.h"

//函数声明
void assertion_failure(char *exp, char *file, char *base, int line);

/* 断言宏，若exp不为真，则传入字符串"exp"、当前源文件名和行号等信息 */
#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

#endif