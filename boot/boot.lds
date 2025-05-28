OUTPUT_ARCH("riscv")       /* 指定目标架构 */

ENTRY(_start)              /* 指定程序入口 */

/* 定义名为flash的内存域，指定权限、起始地址和长度 */
MEMORY
{
  flash (rxai!w) : ORIGIN = 0x20000000, LENGTH = 512k
}
/* 定义代码段，并将其存放到flash内存区域 */
SECTIONS                
{
  .text :               
  {
    KEEP(*(.text))      
  } >flash              
}