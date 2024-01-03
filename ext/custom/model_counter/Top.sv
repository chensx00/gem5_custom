module Top(
    input clk,
    input rst,
    input ena,
    output out
);

Counter counter(
    .clk(clk),
    .rst(rst),
    .ena(ena),
    .out(out)
);

endmodule