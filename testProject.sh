#!/bin/bash
args=("$@")
if [ $# -eq 0 ]; then
  read -p "Enter bits number: " arg
fi

chmod a+x buildProject.sh
sh ./buildProject.sh
./build/karatsuba $1

cd tests

python3 updateTestbench.py $1

iverilog -o sth testbench.v ../dataset/*.v ../dataset/submodules/*.v
vvp sth > result.txt
