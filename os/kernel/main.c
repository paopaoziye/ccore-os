#include "os.h"
extern void task_init(void);
void os_main(){
    printk("Os is running......\n");
    timer_init();
    trap_init();
    task_init();
    run_first_task();
}