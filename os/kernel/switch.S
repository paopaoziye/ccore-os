//将sn寄存器的值保存到a0 + (n+2) * 8处
.altmacro
.macro SAVE_SN n
    sd s\n, (\n+2)*8(a0)
.endm
//将a0 + (n+2) * 8处的值记载到sn寄存器处
.macro LOAD_SN n
    ld s\n, (\n+2)*8(a1)
.endm
    .section .text
    .globl __switch

//接收参数a0为当前任务上下文指针，a1为当前任务上下文指针下一个
__switch:
    //保存当前任务上下文
    //保存sp寄存器和ra寄存器
    sd sp, 8(a0)
    sd ra, 0(a0)
    //保存s0-s11寄存器
    .set n, 0
    .rept 12
        SAVE_SN %n
        .set n, n + 1
    .endr
    //加载下一个任务上下文
    //加载下一个任务的ra寄存器和s0-s11寄存器
    ld ra, 0(a1)
    .set n, 0
    .rept 12
        LOAD_SN %n
        .set n, n + 1
    .endr
    # 切换到下一个任务的内核栈
    ld sp, 8(a1)
    # 返回，即切换到下一个任务的内核执行流
    ret