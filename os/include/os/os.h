#ifndef _OS_H__
#define _OS_H__

#include "context.h"
#include "task.h"
#include "stdio.h"
#include "assert.h"
#include "timer.h"

/* 系统调用相关 */
#define __NR_read     63
#define __NR_write    64
#define __NR_yield    124
#define __NR_gettimeofday 169
uint64_t __SYSCALL(size_t syscall_id,reg_t arg1,reg_t arg2,reg_t arg3);
void __sys_read(size_t fd, char* data, size_t len);
void __sys_write(size_t fd,const char* data,size_t len);
void __sys_yield(void);
uint64_t __sys_gettime();

/* app.c */
int sys_read(size_t fd, char* buf, size_t len);
size_t sys_write(size_t fd, const char* buf, size_t len);
size_t sys_yield();
uint64_t sys_gettime();

/* printk.c */
int printk(const char* s, ...);
void panic(char *s);

/* kerneltrap.S*/
extern void __alltraps(void);
extern void __restore(TrapContext *next);

/* trap.c */
TrapContext* trap_handler(TrapContext* cx);
void trap_init();

/* switch.S */
extern void __switch(TaskContext *current_task_cx_ptr,TaskContext* next_task_cx_ptr );

#endif  