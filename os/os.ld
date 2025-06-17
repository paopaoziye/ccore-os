OUTPUT_ARCH(riscv)
ENTRY(_start)
BASE_ADDRESS = 0x80200000;
/* 定义一个内存区域，并指明其起始地址、长度和属性 */
MEMORY
{ 
	ram (rxai!w) : ORIGIN = 0x80200000, LENGTH = 128M
}
/* 段布局 */
SECTIONS
{
    skernel = .;             /* 定义内核起始内存地址 */
	.text : {
		*(.text .text.*)
	. = ALIGN(0x1000);       /* 代码段结束地址按页对齐 */
    PROVIDE(etext = .);      /* 代码段结束地址 */
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
	
	PROVIDE(kernelend = .);  /* 内核结束地址 */
}