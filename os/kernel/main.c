#include "os.h"
extern void frame_allocator_test();
void os_main(){
    printk("Os is running......\n");
    frame_allocator_test();
    while(1){}
}