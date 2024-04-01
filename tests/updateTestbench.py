import sys


val = sys.argv[1]

with open("testbench.v") as f:
    bench = f.read()

str_to_find  = "localparam N = "
idx = bench.find(str_to_find)

if (idx == -1):
    print("Incorrect testbench")
    quit(1)

idx += len(str_to_find)
end_idx = bench.find(";", idx)

left = bench[:idx]
right = bench[end_idx:]

with open("testbench.v", "w") as f:
    f.write(left + val + right)
