#!/usr/bin/env bash

riscv64-linux-gnu-gcc -march=rv64g -c $1 -o rv32tmp.o
riscv64-linux-gnu-objdump -d ./rv32tmp.o \
  | rg '^...[0-9]+:' \
  | awk '{ print $2 }'
rm ./rv32tmp.o
