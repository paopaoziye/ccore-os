#ifndef OS_TASK_H__
#define OS_TASK_H__

#include "context.h"
#include "riscv.h"
#include "types.h"
#include "os.h"
//人物状态
typedef enum TaskState{
    UnInit,
    Ready,
    Running,
    Exited,
}TaskState;

//任务控制块
typedef struct TaskControlBlock{
    TaskState   task_state;
    TaskContext task_context;
}TaskControlBlock;
//函数声明
void task_create(void (*task_entry)(void));
void schedule();
void task_delay(volatile int count);
void run_first_task();

#endif