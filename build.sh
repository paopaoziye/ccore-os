# 获取脚本所在目录的绝对路径
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
# 如果没有output目录则创建
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
# 回到脚本所在目录
cd ..