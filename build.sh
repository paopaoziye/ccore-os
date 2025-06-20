# 指定编译工具链前缀为riscv64-unknown-elf
CROSS_PREFIX=riscv64-unknown-elf
# 获取当前脚本文件所在的目录，并创建output文件夹
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
if [ ! -d "$SHELL_FOLDER/output" ]; then  
mkdir $SHELL_FOLDER/output
fi  

echo "------------------------- 编译qemu---------------------------------------"
# 切换到qemu-8.0.2
cd qemu-8.0.2
# 如果没有output/qemu目录则运行config脚本进行配置，指定安装目录为$SHELL_FOLDER/output/qemu
if [ ! -d "$SHELL_FOLDER/output/qemu" ]; then  
./configure --prefix=$SHELL_FOLDER/output/qemu  --target-list=riscv64-softmmu --enable-gtk  --enable-virtfs --disable-gio
fi  
# 编译并安装qemu
make -j16$PROCESSORS
make install

echo "-------------------编译lowlevelboot即start.s---------------------------"
# 如果没有output/lowlevelboot目录则创建
if [ ! -d "$SHELL_FOLDER/output/lowlevelboot" ]; then  
mkdir $SHELL_FOLDER/output/lowlevelboot
fi  
# 切换到boot
cd  $SHELL_FOLDER/boot
# 将start.s编译为start.o
$CROSS_PREFIX-gcc -x assembler-with-cpp -c start.s -o $SHELL_FOLDER/output/lowlevelboot/start.o
# 将start.o链接为lowlevel_fw.elf，指定链接脚本为boot.lds，并生成对应的map文件
$CROSS_PREFIX-gcc -nostartfiles -T./boot.lds -Wl,-Map=$SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.map -Wl,--gc-sections $SHELL_FOLDER/output/lowlevelboot/start.o -o $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.elf
# 使用gnu工具生成原始的程序bin文件
$CROSS_PREFIX-objcopy -O binary -S $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.elf $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.bin
# 使用gnu工具生成反汇编文件，方便调试分析
$CROSS_PREFIX-objdump --source --demangle --disassemble --reloc --wide $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.elf > $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.lst

echo "-------------------------编译opensbi ---------------------------------"
# 如果没有output/opensbi目录则创建
if [ ! -d "$SHELL_FOLDER/output/opensbi" ]; then  
mkdir $SHELL_FOLDER/output/opensbi
fi  
# 切换到output/opensbi
cd $SHELL_FOLDER/opensbi-1.2
# 指定编译工具链和目标平台
make CROSS_COMPILE=$CROSS_PREFIX- PLATFORM=my_board
# 将生成的.bin文件复制到output/opensbi
cp -r $SHELL_FOLDER/opensbi-1.2/build/platform/my_board/firmware/*.bin $SHELL_FOLDER/output/opensbi/

echo "------------------------- 编译设备树 ----------------------------------"
# 切换到dts目录
cd $SHELL_FOLDER/dts
# 编译设备树文件生成.dtb文件
dtc -I dts -O dtb -o $SHELL_FOLDER/output/opensbi/my_board_sbi.dtb my_board_sbi.dts

echo "---------------------编译trusted_domaini -----------------------------"
# 如果没有output/trusted_domaini目录则创建
if [ ! -d "$SHELL_FOLDER/output/trusted_domain" ]; then  
mkdir $SHELL_FOLDER/output/trusted_domain
fi  
# 切换到output/trusted_domaini
cd $SHELL_FOLDER/trusted_domain
# 将startup.s编译为startup.o
$CROSS_PREFIX-gcc -x assembler-with-cpp -c startup.s -o $SHELL_FOLDER/output/trusted_domain/startup.o
# 将startup.o链接为trusted_fw.elf，指定链接脚本为link.lds，并生成对应的map文件
$CROSS_PREFIX-gcc -nostartfiles -T./link.lds -Wl,-Map=$SHELL_FOLDER/output/trusted_domain/trusted_fw.map -Wl,--gc-sections $SHELL_FOLDER/output/trusted_domain/startup.o -o $SHELL_FOLDER/output/trusted_domain/trusted_fw.elf
# 基于trusted_fw.elf生成trusted_fw.bin
$CROSS_PREFIX-objcopy -O binary -S $SHELL_FOLDER/output/trusted_domain/trusted_fw.elf $SHELL_FOLDER/output/trusted_domain/trusted_fw.bin
# 生成反汇编文件，方便调试分析
$CROSS_PREFIX-objdump --source --demangle --disassemble --reloc --wide $SHELL_FOLDER/output/trusted_domain/trusted_fw.elf > $SHELL_FOLDER/output/trusted_domain/trusted_fw.lst
echo "----------------------------编译os -----------------------------------"
# 如果没有output/os则创建
if [ ! -d "$SHELL_FOLDER/output/os" ]; then  
mkdir $SHELL_FOLDER/output/os
fi
# 切换到output/os
cd $SHELL_FOLDER/os
make
cp $SHELL_FOLDER/os/os.bin $SHELL_FOLDER/output/os/os.bin
make clean
echo "-----------------------合成firmware固件 -------------------------------"
# 如果没有output/fw目录则创建
if [ ! -d "$SHELL_FOLDER/output/fw" ]; then  
mkdir $SHELL_FOLDER/output/fw
fi  
# 切换到output/fw
cd $SHELL_FOLDER/output/fw
# 清除fw.bin
rm -rf fw.bin
# 创建大小为32kb的全零文件fw.bin
dd of=fw.bin bs=1k count=32k if=/dev/zero   
# 写入lowlevel_fw.bin，地址偏移量地址为 0
dd of=fw.bin bs=1k conv=notrunc seek=0 if=$SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.bin
# 写入my_board_sbi.dtb，地址偏移量为512kb，因此 fdt的地址偏移量为0x80000
dd of=fw.bin bs=1k conv=notrunc seek=512 if=$SHELL_FOLDER/output/opensbi/my_board_sbi.dtb
# 写入fw_jump.bin，地址偏移量为2MB，因此 fw_jump.bin的地址偏移量为0x2000000
dd of=fw.bin bs=1k conv=notrunc seek=2k if=$SHELL_FOLDER/output/opensbi/fw_jump.bin
# 写入trusted_fw.bin，地址偏移量为4MB，因此fw_jump.bin的地址偏移量为0x400000
dd of=fw.bin bs=1k conv=notrunc seek=4K if=$SHELL_FOLDER/output/trusted_domain/trusted_fw.bin
# 写入os.bin,地址偏移量为8MB，因此os.bin的地址偏移量为x800000
dd of=fw.bin bs=1k conv=notrunc seek=8K if=$SHELL_FOLDER/output/os/os.bin
