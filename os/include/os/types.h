#ifndef OS_TYPES_H__
#define OS_TYPES_H__

// 定义无符号整型
typedef unsigned int          uint32_t;
typedef unsigned char         uint8_t;
typedef unsigned short        uint16_t;
typedef unsigned long         size_t;
typedef unsigned long long    uint64_t;
typedef unsigned long int     uintptr_t;

//riscv64寄存器变量
typedef uint64_t              reg_t;

//bool类型
#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

#endif
