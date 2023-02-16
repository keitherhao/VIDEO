#!/bin/sh

if [ ! -d ./build ];then
    mkdir build
    echo "mkdir build"
fi

if [ "`uname -m`" = "x86_64" ];then
    echo "Is armv7l"
    ARM32=TRUE
elif [ "`uname -m`" = "aarch64" ];then
    echo "Is aarch64"
    ARM64=TRUE
fi

echo "cmake -B build -DARM32=${ARM32} . &&"
cmake -B build -DARM32=${ARM32} . &&
make -C build

exit 0