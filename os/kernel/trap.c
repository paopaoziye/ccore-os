#include "os.h"

/* 异常处理函数d，根据 scause 寄存器的值进行不同的处理 */
TrapContext* trap_handler(TrapContext* cx){
	//读取 scause 寄存器
    reg_t scause = r_scause();
	reg_t cause_code = scause & 0xfff;
	if(scause & 0x8000000000000000)
	{
		switch (cause_code)
		{
		//时钟中断
		case 5:
			set_next_trigger();
			schedule();
			break;
		default:
			printk("undfined interrrupt scause:%x\n",scause);
			break;
		}
	}
	else
	{
		switch (cause_code)
		{
		//系统调用
		case 8:
			cx->a0 = __SYSCALL(cx->a7,cx->a0,cx->a1,cx->a2);
			cx->sepc += 8;
			break;
		default:
			printk("undfined exception scause:%x\n",scause);
			break;
		}
	}

	return cx;
}

/* 将stvec即发生异常时处理函数地址设置为__alltraps */
void trap_init(){
	w_stvec((reg_t)__alltraps);
}