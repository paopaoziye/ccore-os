#ifndef OS_CONTEXT_H__
#define OS_CONTEXT_H__

#include "types.h"

/* S模式的trap上下文，即需要保护的寄存器的值 */
typedef struct TrapContext{
	/* 通用寄存器 */
	reg_t x0;
	reg_t ra;
	reg_t sp;
	reg_t gp;
	reg_t tp;
	reg_t t0;
	reg_t t1;
	reg_t t2;
	reg_t s0;
	reg_t s1;
	reg_t a0;
	reg_t a1;
	reg_t a2;
	reg_t a3;
	reg_t a4;
	reg_t a5;
	reg_t a6;
	reg_t a7;
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
	reg_t t3;
	reg_t t4;
	reg_t t5;
	reg_t t6;
	/* S模式下的寄存器 */
	reg_t sstatus;
	reg_t sepc;
}TrapContext;

/* S模式的任务上下文 */
typedef struct TaskContext{
	//ra为__switch返回地址，sp为内核栈指针
	reg_t ra;
	reg_t sp;
	//riscv规定的函数调用时需要被被调用者保存的寄存器
	reg_t s0;
	reg_t s1;
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
}TaskContext;
#endif