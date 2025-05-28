OUTPUT_ARCH(riscv)   /* 指定目标架构 */

ENTRY(_start)        /* 设置入口函数为_start */

/* 定义名为ram的内存域，指定权限、起始地址和长度 */
MEMORY
{ 
	ram (rxai!w) : ORIGIN = 0x80200000, LENGTH = 128M
}
/* 定义代码段、只读数据段、数据段和bss段，并将其依次存放到ram内存区域 */
SECTIONS
{
	.text : {
		*(.text .text.*)
	} >ram

	.rodata : {
		*(.rodata .rodata.*)
	} >ram

	.data : {
		. = ALIGN(4096);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ram

	.bss :{
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
	} >ram
}