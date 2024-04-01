`timescale 1 ns / 1 ps

module testbench_d;
    localparam N = 10;
    reg  [ N - 1 : 0 ] a, b;
    wire [ ((N << 1) - 1) : 0] res_kar;
	reg  [ ((N << 1) - 1) : 0] res_fact;

    Karatsuba_0 karatsuba(.input_0(a), .input_1(b), .output_2(res_kar));

	integer i, j;
	reg [N - 1:0] val1, val2;
    
    initial $dumpvars;

    initial
		begin
			// for small we can check all values
			if (N <= 6) begin
			for (i = 0; i < 1 << N; i = i + 1)
			begin
				for (j = 0; j < 1 << N; j = j + 1)
				begin
					// protection from stupid mistakes
					a = i[N - 1:0];
					b = j[N - 1:0];

					res_fact <= a * b;
					# 20;   
					if(res_fact !== res_kar) begin
						$error("BAD kar2");
						$stop();
					end
				end
			end
			end
			else begin
				for (j = 0; j < 200; j = j + 1) begin

					for (i = 0; i <= N >> 5; i = i + 1) begin
						val1 = val1 << 32;
						val1[31: 0] = $urandom;
						
						val2 = val2 << 32;
						val2[31: 0] = $urandom;
					end

					a = val1;
					b = val2;

					res_fact <= a * b;
					# 20;   
					if(res_fact !== res_kar) begin
						$error("BAD kar2");
						$stop();
					end

				end
			end
		end
	initial $monitor ("%0d a: %b b: %b kar: %b fact: %b", $time, a, b, res_kar, res_fact);

endmodule
