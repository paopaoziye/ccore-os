#include "os.h"
#include "address.h"
#include "alloc.h"
void os_main(){
    printk("Os is running......\n");
    frame_alloctor_init();
    kvminit();
    kvminithart();
    trap_init();
    while(1){}
}