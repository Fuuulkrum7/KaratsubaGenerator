# Welcome to Karatsuba multiplier generator

Here is an instruction for compiling this project

it is neccesary to have an installed cmake and make on your PC.

Run this commands:
```
chmod a+x buildProject.sh
./buildProject.sh
```

For running project, execute
```
./build/karatsuba <NumberOfBits>
```
Where `NumberOfBits` is bits number, which should be natural. All generated files would be located in folder named `dataset`.
Main module with `NumberOfBits` size inputs would be located their and would have modulename Karatsuba_0. Other submodules would be located 
in folder named `submodules` in same directory, where top-module is located (in `dataset`).

If you want to test project code, run 
```
chmod a+x testProject.sh
./testProject.sh <NumberOfBits>
```

It would create a file `result.txt` in folder `tests` with dump from icarus verilog. 
Of course, it is neccesary to install icarus verilog first.

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
