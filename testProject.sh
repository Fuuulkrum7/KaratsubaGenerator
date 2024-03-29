#!/bin/bash
if [ ! -d "dataset" ]; then
  echo "Make dataset first!"
  exit 1
fi
cd tests

iverilog -o sth testbench.v ../dataset/*.v ../dataset/submodules/*.v
vvp sth > result.txt
