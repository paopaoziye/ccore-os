#include "os.h"

/* 根据系统调用号调用对应的处理函数 */
uint64_t __SYSCALL(size_t syscall_id,reg_t arg1,reg_t arg2,reg_t arg3){
    switch(syscall_id){
        case __NR_write:
            __sys_write(arg1,(const char*)arg2,arg3);
            break;
        case __NR_read:
            __sys_read(arg1,(char*)arg2, arg3);
            break;
        case __NR_yield:
            __sys_yield();
            break;
        case __NR_gettimeofday:
            return __sys_gettime();
        default:
            printk("Unsupported syscall id:%d\n",syscall_id);
            break;
    }
}

/* write系统调用的最终实现 */
void __sys_write(size_t fd,const char* data,size_t len){
    if(fd == 1){
        printk(data);
    }else{
        panic("Unsupported fd in sys_write!");
    }
}

/* read系统调用的最终实现 */
void __sys_read(size_t fd, char* data, size_t len)
{
    //确保len为1且data不为NULL
    assert(len == 1);
    assert(data != NULL);
    //当读取到字符时，设置data
    //如果没有读取到字符，调用schedule()暂时放弃CPU资源
    if(fd == stdin){
        int c;
        while(1){
            c = sbi_console_getchar();
            if( c != -1){
                break;
            }else{
                schedule();
                continue;
            }  
        }
        data[0] = c;
    }else{
        panic("fd != stdin!!!\n");
    }
}

/* yield系统调用的最终实现 */
void __sys_yield(){
    schedule();
}

/* gettime系统调用的最终实现 */
uint64_t __sys_gettime(){
    return get_time_us();
}