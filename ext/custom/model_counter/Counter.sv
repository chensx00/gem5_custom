module Counter
(
    input wire      clk,
    input wire      rst,
    input wire      ena,

    output reg      out,
    output reg[3:0] cnt
);

//reg[3:0]    cnt;



always_ff @( posedge clk) begin : counter
    if (rst == 1'b1) begin
        cnt <= 4'hf;
    end else if( ena == 1'b1 ) begin
        cnt <= cnt - 4'b1;
    end else begin
        cnt <= cnt;
    end
end

always_ff @( posedge clk ) begin : Outer
    if(rst == 1'b1) begin
        out <= 1'b0;
    end if( cnt == 4'h0 ) begin
        out <= 1'b1;
    end else begin
        out <= 1'b0;
    end


end



endmodule