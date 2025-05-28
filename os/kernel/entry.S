    .section .text.entry          //代码段声明
    .globl _start                 //将_start声明为全局可见
_start:                           //设置栈指针并跳转到os_main
    la sp, boot_stack_top
    call os_main
spin:
        j spin                    //防止程序跑飞

    .section .bss.stack           //bss段声明
    .globl boot_stack_lower_bound //将boot_stack_lower_bound声明为全局可见
boot_stack_lower_bound:           //标记栈底端位置
    .space 4096 * 16              //分配栈内存，大小为64KB
    .globl boot_stack_top         //将.globl boot_stack_top声明为全局可见
boot_stack_top:                   //标记栈顶端位置