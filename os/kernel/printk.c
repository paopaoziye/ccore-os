#include "stdio.h"
//输出缓冲区
static char out_buf[1000];

/* 逐步输出字符串 */
static void uart_puts(char *s){
	while (*s) {
		sbi_console_putchar(*s++);
	}
}
/* 将字符串格式化到out_buf中，并通过uart_puts输出到串口，最后返回格式化的s的长度 */
static int vprintk(const char* s, va_list vl){
	//计算格式化后的s的长度
	int res = _vsnprintf(NULL, -1, s, vl);
	if (res+1 >= sizeof(out_buf)) {
		uart_puts("error: output string size overflow\n");
		while(1) {}
	}
	_vsnprintf(out_buf, res + 1, s, vl);
	uart_puts(out_buf);
	return res;
}
/* 调用vprintk，并返回格式化的s的长度 */
int printk(const char* s, ...){
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = vprintk(s, vl);
	va_end(vl);
	return res;
}
/* 发出错误警告 */
void panic(char *s){
	printk("panic: ");
	printk(s);
	printk("\n");
	while(1){};
}