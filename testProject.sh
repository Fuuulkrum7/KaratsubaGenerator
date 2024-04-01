#!/bin/bash
if [ $# -eq 0 ]; then
  read -p "Enter bits number: " val
else 
  val=$1
fi

chmod a+x buildProject.sh
sh ./buildProject.sh
./build/karatsuba $val

cd tests

python3 updateTestbench.py $val

iverilog -o sth testbench.v ../dataset/*.v ../dataset/submodules/*.v
vvp sth > result.txt
