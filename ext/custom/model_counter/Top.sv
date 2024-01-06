module Top(
    input clk,
    input rst,
    input ena,
    output out,
    output[3:0] cnt
);

Counter counter(
    .clk(clk),
    .rst(rst),
    .ena(ena),
    .out(out),
    .cnt(cnt)
);

endmodule
