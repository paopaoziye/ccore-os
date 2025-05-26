# 获取脚本所在目录的绝对路径
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
# 运行qemu-system-riscv64，指定机器类型为my_board，分配1GB物理内存，模拟8个CPU核心并禁用BIOS
# 并将monitor映射到控制台
$SHELL_FOLDER/output/qemu/bin/qemu-system-riscv64 \
-M my_board \
-m 1G \
-smp 8 \
-bios none \
-monitor stdio \