#ifndef OS_SBI_H__
#define OS_SBI_H__

//防止stdint.h和types.h发生冲突
#ifndef OS_TYPES_H__
#include <stdint.h>
#endif

// sbi扩展类和EID的映射
enum sbi_ext_id {
	SBI_EXT_0_1_SET_TIMER = 0x0,
	SBI_EXT_0_1_CONSOLE_PUTCHAR = 0x1,
	SBI_EXT_0_1_CONSOLE_GETCHAR = 0x2,
	SBI_EXT_0_1_CLEAR_IPI = 0x3,
	SBI_EXT_0_1_SEND_IPI = 0x4,
	SBI_EXT_0_1_REMOTE_FENCE_I = 0x5,
	SBI_EXT_0_1_REMOTE_SFENCE_VMA = 0x6,
	SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID = 0x7,
	SBI_EXT_0_1_SHUTDOWN = 0x8,
	SBI_EXT_BASE = 0x10,
	SBI_EXT_TIME = 0x54494D45,
	SBI_EXT_IPI = 0x735049,
	SBI_EXT_RFENCE = 0x52464E43,
	SBI_EXT_HSM = 0x48534D,
	SBI_EXT_SRST = 0x53525354,
	SBI_EXT_PMU = 0x504D55,
};

// sbi功能名和EID的映射
enum sbi_ext_time_fid {
	SBI_EXT_TIME_SET_TIMER = 0,
};

// sbi返回结构体
struct sbiret {
	long error;  //SBI错误码
	long value;  //返回值
};

//函数声明
void sbi_console_putchar(int ch);
int sbi_console_getchar(void);
void sbi_set_timer(uint64_t stime_value);
#endif  