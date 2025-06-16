#include "alloc.h"

#define MEMORY_END 0x80800000    //空闲空间起始地址
#define MEMORY_START 0x80400000  //空闲空间末尾地址

//内存分配器
static StackFrameAllocator FrameAllocatorImpl;

/* 依次分配、回收、再分配五个物理页 */
void frame_allocator_test(){
     //初始化内存分配器
    StackFrameAllocator_init(&FrameAllocatorImpl, \
            floor_phys(phys_addr_from_size_t(MEMORY_START)), \
            ceil_phys(phys_addr_from_size_t(MEMORY_END)));
     //打印起始地址
    printk("Memoery start:%d\n",floor_phys(phys_addr_from_size_t(MEMORY_START)));
    printk("Memoery end:%d\n",ceil_phys(phys_addr_from_size_t(MEMORY_END)));
    PhysPageNum frame[10];
    //分配五个物理页，并将其物理页号存入数组中
    for (size_t i = 0; i < 5; i++){
         frame[i] = StackFrameAllocator_alloc(&FrameAllocatorImpl);
         printk("frame id:%d\n",frame[i].value);
    }
    //回收这五个物理页，并打印回收栈中的物理页号
    for (size_t i = 0; i < 5; i++){
         StackFrameAllocator_dealloc(&FrameAllocatorImpl,frame[i]);
         printk("allocator->recycled.data.value:%d\n",FrameAllocatorImpl.recycled.data[i]);
         printk("frame id:%d\n",frame[i].value);
     }
     //再次分配五个物理页
     for (size_t i = 0; i < 5; i++){
          frame[i] = StackFrameAllocator_alloc(&FrameAllocatorImpl);
         printk("frame id:%d\n",frame[i].value);
     }
}