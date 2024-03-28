#!/bin/bash
if [ ! -d "dataset" ]; then
  echo "Make dataset first!"
  exit 1
fi
cd tests

iverilog -o sth testbench.v ../dataset/Karatsuba_0.v
vvp sth > result.txt
