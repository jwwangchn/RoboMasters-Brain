#!/bin/bash

# 打开UART的权限
sudo chmod 666 /dev/ttyUSB0

# 判断build文件夹是否存在，不存在则创建
if [ ! -d "$build" ]; then
  mkdir build
fi

# 进入build
cd build

# 判断build文件夹中是否有文件
files=`ls $build`
if [ -z "$files" ]; then
    echo "no make clean"
else
    echo "used make clean"
    make clean
fi
# 编译
cmake ..
make
./robomasters-brain

