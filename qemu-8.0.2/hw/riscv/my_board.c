#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qemu/error-report.h"
#include "qemu/guest-random.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/sysbus.h"
#include "hw/qdev-properties.h"
#include "hw/char/serial.h"
#include "target/riscv/cpu.h"

#include "hw/riscv/riscv_hart.h"
#include "hw/riscv/my_board.h"
#include "hw/riscv/boot.h"
#include "hw/riscv/numa.h"
#include "hw/intc/riscv_aclint.h"
#include "hw/intc/riscv_aplic.h"
#include "hw/intc/sifive_plic.h"

#include "chardev/char.h"
#include "sysemu/device_tree.h"
#include "sysemu/sysemu.h"
#include "sysemu/kvm.h"
#include "sysemu/tpm.h"

//板卡的硬件地图，MemMapEntry的成员分别为基址和占用地址长度
static const MemMapEntry my_board_memmap[] = {
    [MY_BOARD_MROM]      = {        0x0,        0x8000 },   
    [MY_BOARD_SRAM]      = {     0x8000,        0x8000 },
    [MY_BOARD_CLINT]     = { 0x02000000,       0x10000 }, 
    [MY_BOARD_PLIC]      = { 0x0c000000,     MY_BOARD_PLIC_SIZE(MY_BOARD_CPUS_MAX * 2) },
    [MY_BOARD_UART0]     = { 0x10000000,         0x100 },
    [MY_BOARD_UART1]     = { 0x10001000,         0x100 },
    [MY_BOARD_UART2]     = { 0x10002000,         0x100 }, 
    [MY_BOARD_RTC]       = { 0x10003000,        0x1000 },
    [MY_BOARD_FLASH]     = { 0x20000000,     0x2000000 }, 
    [MY_BOARD_DRAM]      = { 0x80000000,    0x40000000 },  
};
/* 初始化CPU，参考virt_machine_init */
static void my_board_cpu_create(MachineState *machine){
    //获取板卡的MyBoardState结构体
    MyBoardState *s = RISCV_MY_BOARD_MACHINE(machine);
    char *soc_name;
    int i, base_hartid, hart_count;
    //获取命令行参数中配置的socket节点数量
    int socket_count = riscv_socket_count(machine);
    //检查socket节点的数目是否合法
    if (MY_BOARD_SOCKETS_MAX < socket_count) {
        error_report("number of sockets/nodes should be less than %d",
            MY_BOARD_SOCKETS_MAX);
        exit(1);
    }
    //检查各个socket节点并配置其hart属性
    for (i = 0; i < socket_count; i++) {
        //确保当前socket节点的hart id是连续的
        if (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            exit(1);
        }
        //获取当前socket节点的起始hart id
        base_hartid = riscv_socket_first_hartid(machine, i);
        if (base_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            exit(1);
        }
        //获取当前socket节点的hart数量
        hart_count = riscv_socket_hart_count(machine, i);
        if (hart_count < 0) {
            error_report("can't find hart count for socket%d", i);
            exit(1);
        }
        //初始化socket节点并命名
        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_child(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        //配置每个socket节点的hart属性，即CPU类型、起始hart id和hart数量
        object_property_set_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_set_int(OBJECT(&s->soc[i]), "hartid-base",
                                base_hartid, &error_abort);
        object_property_set_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_count, &error_abort);
        //激活socket节点
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_abort);
    }    
}
/* 创建dram、sram和mrom，参考virt_machine_init */
static void my_board_memory_create(MachineState *machine){
    //获取MyBoardState
    MyBoardState *s = RISCV_MY_BOARD_MACHINE(machine);
    //获取全局对象system_memory，代表整个系统的物理地址空间
    MemoryRegion *system_memory = get_system_memory();
    //分配三片存储空间，分别对应dram、sram和mrom
    MemoryRegion *dram_mem = g_new(MemoryRegion, 1);  
    MemoryRegion *sram_mem = g_new(MemoryRegion, 1);
    MemoryRegion *mask_rom = g_new(MemoryRegion, 1); 
    //初始化内存区域并将其挂载到system_memory对应基址处
    memory_region_init_ram(dram_mem, NULL, "riscv_my_board_board.dram",
                           my_board_memmap[MY_BOARD_DRAM].size, &error_fatal);
    memory_region_add_subregion(system_memory, 
                                my_board_memmap[MY_BOARD_DRAM].base, dram_mem);

    memory_region_init_ram(sram_mem, NULL, "riscv_my_board_board.sram",
                           my_board_memmap[MY_BOARD_SRAM].size, &error_fatal);
    memory_region_add_subregion(system_memory, 
                                my_board_memmap[MY_BOARD_SRAM].base, sram_mem);

    memory_region_init_rom(mask_rom, NULL, "riscv_my_board_board.mrom",
                           my_board_memmap[MY_BOARD_MROM].size, &error_fatal);
    memory_region_add_subregion(system_memory, 
                                my_board_memmap[MY_BOARD_MROM].base, mask_rom);
    //设置CPU的复位向量，其中第三个参数为复位向量地址
    riscv_setup_rom_reset_vec(machine, &s->soc[0], 
                              my_board_memmap[MY_BOARD_FLASH].base,
                              my_board_memmap[MY_BOARD_MROM].base,
                              my_board_memmap[MY_BOARD_MROM].size,
                              0x0, 0x0);
}
/* 创建flash，参考virt_machine_init、GITvirt_flash_create1和virt_flash_map1 */
static void my_board_flash_create(MachineState *machine){
    //获取MyBoardState以及system_memory
    MyBoardState *s = RISCV_MY_BOARD_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    //创建pflash设备并设置其属性，即扇区大小、总线宽度、设备宽度、字节序、id和名字
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);
    qdev_prop_set_uint64(dev, "sector-length", MY_BOARD_FLASH_SECTOR_SIZE);
    qdev_prop_set_uint8(dev, "width", 4);
    qdev_prop_set_uint8(dev, "device-width", 2);
    qdev_prop_set_bit(dev, "big-endian", false);
    qdev_prop_set_uint16(dev, "id0", 0x89);
    qdev_prop_set_uint16(dev, "id1", 0x18);
    qdev_prop_set_uint16(dev, "id2", 0x00);
    qdev_prop_set_uint16(dev, "id3", 0x00);
    qdev_prop_set_string(dev, "name", "my_board.flash0");
    //将pflash挂载到开发板，并绑定后端存储驱动
    object_property_add_child(OBJECT(s), "my_board.flash0", OBJECT(dev));
    object_property_add_alias(OBJECT(s), "pflash0",
                              OBJECT(dev), "drive");
    s->flash = PFLASH_CFI01(dev);
    pflash_cfi01_legacy_drive(s->flash,drive_get(IF_PFLASH, 0, 0));
    //检查flash基地址是否对齐并保证扇区数量不超过UINT32_MAX
    hwaddr flashsize = my_board_memmap[MY_BOARD_FLASH].size;
    hwaddr flashbase = my_board_memmap[MY_BOARD_FLASH].base;
    assert(QEMU_IS_ALIGNED(flashsize, MY_BOARD_FLASH_SECTOR_SIZE));
    assert(flashsize / MY_BOARD_FLASH_SECTOR_SIZE <= UINT32_MAX);
    //设置flash的块总数并初始化flash
    qdev_prop_set_uint32(dev, "num-blocks", flashsize / MY_BOARD_FLASH_SECTOR_SIZE);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    //将flash加入到系统内存空间
    memory_region_add_subregion(system_memory, flashbase,
                                sysbus_mmio_get_region(SYS_BUS_DEVICE(dev),
                                                       0));    
}
/* 创建plic，参考virt_create_plic */
static void my_board_plic_create(MachineState *machine){
    //获取MyBoardState以及socket节点数量
    MyBoardState *s = RISCV_MY_BOARD_MACHINE(machine);
    int socket_count = riscv_socket_count(machine);
    //为每个socket节点创建一个plic
    int i,hart_count,base_hartid;
    for ( i = 0; i < socket_count; i++) {
        //获取当前socket节点的hart数量和起始id
        hart_count = riscv_socket_hart_count(machine, i);
        base_hartid = riscv_socket_first_hartid(machine, i);
        char *plic_hart_config;
        //生成PLIC Hart配置字符串
        plic_hart_config = riscv_plic_hart_config_string(machine->smp.cpus);
        //设置对应PLIC的属性，分别为基地址、hart配置、hart数量、hart起始id等
        s->plic[i] = sifive_plic_create(
            my_board_memmap[MY_BOARD_PLIC].base + i *my_board_memmap[MY_BOARD_PLIC].size ,
            plic_hart_config, hart_count , base_hartid,
            MY_BOARD_PLIC_NUM_SOURCES,
            MY_BOARD_PLIC_NUM_PRIORITIES,
            MY_BOARD_PLIC_PRIORITY_BASE,
            MY_BOARD_PLIC_PENDING_BASE,
            MY_BOARD_PLIC_ENABLE_BASE,
            MY_BOARD_PLIC_ENABLE_STRIDE,
            MY_BOARD_PLIC_CONTEXT_BASE,
            MY_BOARD_PLIC_CONTEXT_STRIDE,
            my_board_memmap[MY_BOARD_PLIC].size);
        g_free(plic_hart_config);
    }
}
/* 创建clint，参考virt_machine_init */
static void my_board_aclint_create(MachineState *machine){
    int i , hart_count,base_hartid;
    int socket_count = riscv_socket_count(machine);
    //为每个socket节点创建clint
    for ( i = 0; i < socket_count; i++) {
        //获取当前socket节点的hart数量和起始id
        base_hartid = riscv_socket_first_hartid(machine, i);
        hart_count = riscv_socket_hart_count(machine, i);
        //创建clint的软件中断模块
        riscv_aclint_swi_create(
        my_board_memmap[MY_BOARD_CLINT].base + i *my_board_memmap[MY_BOARD_CLINT].size,
        base_hartid, hart_count, false);
        //创建clint的时钟中断模块
        riscv_aclint_mtimer_create(my_board_memmap[MY_BOARD_CLINT].base +
             + i *my_board_memmap[MY_BOARD_CLINT].size+ RISCV_ACLINT_SWI_SIZE,
            RISCV_ACLINT_DEFAULT_MTIMER_SIZE, base_hartid, hart_count,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
    }
}
/* 创建rtc，参考virt_machine_init */
static void my_board_rtc_create(MachineState *machine){   
    //获取MyBoardState
    MyBoardState *s = RISCV_MY_BOARD_MACHINE(machine);
    //创建goldfish_rtc进行内存映射，并设置其中断号和引脚
    sysbus_create_simple("goldfish_rtc", my_board_memmap[MY_BOARD_RTC].base,
        qdev_get_gpio_in(DEVICE(s->plic[0]), MY_BOARD_RTC_IRQ));
}
/* 创建三路uart，参考virt_machine_init */
static void my_board_serial_create(MachineState *machine){
    //获取MyBoardState以及system_memory
    MemoryRegion *system_memory = get_system_memory();
    MyBoardState *s = RISCV_MY_BOARD_MACHINE(machine);
    //创建goldfish_rtc进行内存映射，并设置了设置其中断号、引脚、波特率以及字节序
    //此外还设置了串口的后端输入/输出，在QEMU启动时可以指定
    serial_mm_init(system_memory, my_board_memmap[MY_BOARD_UART0].base,
        0, qdev_get_gpio_in(DEVICE(s->plic[0]), MY_BOARD_UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);
    serial_mm_init(system_memory, my_board_memmap[MY_BOARD_UART1].base,
        0, qdev_get_gpio_in(DEVICE(s->plic[0]), MY_BOARD_UART1_IRQ), 399193,
        serial_hd(1), DEVICE_LITTLE_ENDIAN);
    serial_mm_init(system_memory, my_board_memmap[MY_BOARD_UART2].base,
        0, qdev_get_gpio_in(DEVICE(s->plic[0]), MY_BOARD_UART2_IRQ), 399193,
        serial_hd(2), DEVICE_LITTLE_ENDIAN);
}
/* 板卡初始化函数 */
static void my_board_machine_init(MachineState *machine){
    //创建CPU
    my_board_cpu_create(machine);
    //创建主存
    my_board_memory_create(machine);
    //创建flash
    my_board_flash_create(machine);
    //创建plic
    my_board_plic_create(machine);
    //创建clint
    my_board_aclint_create(machine);
    //创建rtc
    my_board_rtc_create(machine);
    //创建serial
    my_board_serial_create(machine);
}
/* 实例初始化函数 */
static void my_board_machine_instance_init(Object *obj){

}

/* 板卡的注册函数，包含各种元数据和行为 */
static void my_board_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V my_board board";
    mc->init = my_board_machine_init;
    mc->max_cpus = MY_BOARD_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_BASE;
    mc->pci_allow_0_address = true;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_index_to_instance_props = riscv_numa_cpu_index_to_props;
    mc->get_default_cpu_node_id = riscv_numa_get_default_cpu_node_id;
    mc->numa_mem_supported = true;
}
/* 注册板卡 */
static const TypeInfo my_board_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("my_board"),
    .parent     = TYPE_MACHINE,
    .class_init = my_board_machine_class_init,
    .instance_init = my_board_machine_instance_init,
    .instance_size = sizeof(MyBoardState),
    .interfaces = (InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

static void my_board_machine_init_register_types(void)
{
    type_register_static(&my_board_machine_typeinfo);
}

type_init(my_board_machine_init_register_types)