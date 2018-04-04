#!/bin/sh
# author:liao fuhsin 
# date:2017/10/25
# script for compile DTree.c
#  

CC=gcc

$CC -o test2 -lm test_v2.c
if [ $? != 0 ]; then
    echo "compile error"
    exit 1
fi

./test2 $1 $2 end

