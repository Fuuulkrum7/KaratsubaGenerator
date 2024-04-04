# Welcome to Karatsuba multiplier generator

Here is instruction, how to compile this project

Run this commands:
```
chmod a+x buildProject.sh
./buildProject.sh
```

For running project, execute
```
./build/karatsuba <NumberOfBits>
```
Where `NumberOfBits` is bits number, which should be natural.

If you want to test project code, run 
```
chmod a+x testProject.sh
./testProject.sh <NumberOfBits>
```

It would create file `result.txt` in folder tests with dump from icarus verilog. 
Of course, it is neccesary tu install icarus verilog first.

Ubuntu:
```
sudo apt-get install iverilog
```
Fedora:
```
sudo dnf install iverilog
```

For executing only c++ tests, just run
```
./build/tests/KaratsubaGenTests
```
