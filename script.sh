#!/bin/bash

for i in O0 O1 O2 O3 Os Ofast Og
do
        gcc -$i 1.c -o 1
        sudo time -a -f "%E\n" -o ~/res.txt ./1
done
