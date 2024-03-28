`timescale 1 ns / 100 ps

module testbench_d;
	localparam  N = 2;
    reg  [ N - 1 : 0 ] a, b;
    wire [ ((N << 1) - 1) : 0] res_kar;
	reg  [ ((N << 1) - 1) : 0] res_fact;

    Karatsuba_0 karatsuba(a, b, res_kar);

	integer i, j;
    
    initial $dumpvars;

    initial
		begin
			for (i = 0; i < 1 << N; i = i + 1)
			begin
				for (j = 0; j < 1 << N; j = j + 1)
				begin
					a = i[N - 1:0];
					b = j[N - 1:0];

					res_fact <= a * b;
					# 100;   
					if(res_fact !== res_kar) begin
						$error("BAD kar2");
						$stop();
					end
				end
			end
		end
	initial $monitor ("%0d a: %b b: %b kar: %b fact: %b", $time, a, b, res_kar, res_fact);

endmodule
