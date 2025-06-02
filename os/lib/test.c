#include "task.h"
#include "os.h"
#include "string.h"
#include "stdio.h"
/* 创建一系列任务以测试轮转调度机制 */
void task1(){
    while (1){
        printf("task1 is running!\n");
        task_delay(10000);
        sys_yield();
    }
}

void task2(){
    while (1){
        printf("task2 is running!\n");
        task_delay(10000);
        sys_yield();
    }
}

void task3(){
    while (1){
        printf("task3 is running!\n");
        task_delay(10000);
        sys_yield();
    }
}

void task_init(void){
	task_create(task1);
	task_create(task2);
    task_create(task3);
}