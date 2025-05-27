# SPDX-License-Identifier: BSD-2-Clause

# 编译选项
platform-cppflags-y =
platform-cflags-y =
platform-asflags-y =
platform-ldflags-y =

# 编译目标
platform-objs-y += platform.o

# 固件配置
FW_JUMP=y
FW_TEXT_START=0x80000000
FW_JUMP_ADDR=0x0
