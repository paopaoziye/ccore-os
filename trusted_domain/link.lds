OUTPUT_ARCH( "riscv" )     /* 指定目标架构 */

ENTRY( _start )            /* 指定程序入口 */

/* 定义名为ddr的内存域，指定权限、起始地址和长度 */
MEMORY
{
	ddr (rxai!w) : ORIGIN = 0xb0000000, LENGTH = 256M
}
/* 定义代码段，并将其存放到ddr内存区域 */
SECTIONS
{
  .text :
  {
    KEEP(*(.text))
  } >ddr
}