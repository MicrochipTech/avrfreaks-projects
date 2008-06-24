module leddisplay(ain,bin,cin,din,led,sel,dig);
    input [3:0] ain;
    input [3:0] bin;
    input [3:0] cin;
    input [3:0] din;
    output [6:0] led;
    input [1:0] sel;
	output [3:0] dig;

	reg [3:0] dig;
	reg [6:0] led;
	reg [3:0] t;

	always @(ain or bin or cin or din or sel)
	begin
		case(sel)
			2'b00:t <= ain;
			2'b01:t <= bin;
			2'b10:t <= cin;
			2'b11:t <= din;
			default:t <= 2'bx;
		endcase
	end

	always @ (sel)
	begin
		case (sel)
			2'b00:dig <= 4'b1110;
			2'b01:dig <= 4'b1101;
			2'b10:dig <= 4'b1011;
			2'b11:dig <= 4'b0111;
			default:dig <= 4'b1111;
		endcase
	end

	// seven segment decoder
	//
	// led[0] = a
	// led[1] = b;
	// led[2] = c;
	// led[3] = d;
	// led[4] = e;
	// led[5] = f;
	// led[6] = g;
	always @(t)
	begin
		case(t)
			4'b0000: led <= 7'b1000000;		//0
			4'b0001: led <= 7'b1111001;		//1
			4'b0010: led <= 7'b0100100; 		//2
			4'b0011: led <= 7'b0110000;		//3
			4'b0100: led <= 7'b0011001;		//4
			4'b0101: led <= 7'b0010010;		//5
			4'b0110: led <= 7'b0000010;		//6
			4'b0111: led <= 7'b1111000;		//7
			4'b1000: led <= 7'b0000000;		//8
			4'b1001: led <= 7'b0010000;		//9
			4'b1010: led <= 7'b0001000;		//a
			4'b1011: led <= 7'b0000011;		//b
			4'b1100: led <= 7'b1000110;		//c
			4'b1101: led <= 7'b0100001; 		//d
			4'b1110: led <= 7'b0000110;		//e
			4'b1111: led <= 7'b0001110;		//f
		endcase
	end
endmodule
