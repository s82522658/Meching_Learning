#!/bin/sh
# author:liao fuhsin 
# date:2017/10/25
# script for compile DTree.c
#  

CC=gcc

$CC -o DT -lm 0416314.c
if [ $? != 0 ]; then
    echo "compile error"
    exit 1
fi


./DT end

