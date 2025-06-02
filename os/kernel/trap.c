#include "os.h"

/* 异常处理函数d，根据 scause 寄存器的值进行不同的处理 */
TrapContext* trap_handler(TrapContext* cx){
	//读取 scause 寄存器
    reg_t scause = r_scause();
	switch (scause)
	{
	case 8:
		__SYSCALL(cx->a7,cx->a0,cx->a1,cx->a2);
		break;
	default:
		printk("undfined scause:%d\n",scause);
		break;
	}
	
	cx->sepc += 8;

	return cx;
}

/* 将stvec即发生异常时处理函数地址设置为__alltraps */
void trap_init(){
	w_stvec((reg_t)__alltraps);
}