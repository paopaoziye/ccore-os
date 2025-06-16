#ifndef OS_TIMER_H__
#define OS_TIMER_H__

#include "types.h"
#include "riscv.h"
#include "sbi.h"

//定义时钟频率和时钟中断频率
#define CLOCK_FREQ 10000000
#define TICKS_PER_SEC 1000

//函数声明
void set_next_trigger();
void timer_init();
uint64_t get_time_us();
#endif