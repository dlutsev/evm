#!/bin/bash

for i in {1..10}; do
   sudo time -a -f "%E\n" -o ~/res.txt ./1
done
