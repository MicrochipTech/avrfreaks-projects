`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    13:30:36 10/25/2006 
// Design Name: 
// Module Name:    TopLevel 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module TopLevel(clkin, wd, rd, ale, ad, a, led,ledseg,leddig,irq,tirq,db,H,V,R,G,B);
    input clkin;
    input wd;
    input rd;
    input ale;
//	 input reset;
    inout [7:0] ad;
    input [7:0] a;
    output [7:0] led;
	output [6:0] ledseg;
	output [3:0] leddig;
	output irq;
	output tirq;
	output [7:0] db;
	output H,V,R,G,B;
	
	wire [7:0]db;
	wire timerO;
	wire [1:0] e;
	wire tirq;
	wire irq;
	wire [7:0]la,ea;
	wire [7:0] din;
	wire [7:0] d;
	wire [6:0]ledseg;
	wire [3:0] leddig;
	wire clk;
	
	//----------------------------
	// 4 digit LED display
	//----------------------------
	reg [7:0] selcntr;
	
avrif AvrInterface (
    .clk(clk), 
    .reset(), 
    .ad(din), 
    .d(d),		//output data 
    .a(a), 
    .ale(ale), 
    .wd(wd), 
    .rd(rd), 
    .oen(oen), 
	 .la(la),
	 .led(led),
	 .EA(ea),
	 .irq(irq),
	 .tirq(tirq),
	 .db(db),
    .H(H), 
    .V(V), 
    .R(R), 
    .G(G), 
    .B(B)
    );

IOBUF Io0 (
      .O(din[0]),     // Buffer output
      .IO(ad[0]),   // Buffer inout port (connect directly to top-level port)
      .I(d[0]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

IOBUF Io1 (
      .O(din[1]),     // Buffer output
      .IO(ad[1]),   // Buffer inout port (connect directly to top-level port)
      .I(d[1]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

IOBUF Io2 (
      .O(din[2]),     // Buffer output
      .IO(ad[2]),   // Buffer inout port (connect directly to top-level port)
      .I(d[2]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

IOBUF Io3 (
      .O(din[3]),     // Buffer output
      .IO(ad[3]),   // Buffer inout port (connect directly to top-level port)
      .I(d[3]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

IOBUF Io4 (
      .O(din[4]),     // Buffer output
      .IO(ad[4]),   // Buffer inout port (connect directly to top-level port)
      .I(d[4]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

IOBUF Io5 (
      .O(din[5]),     // Buffer output
      .IO(ad[5]),   // Buffer inout port (connect directly to top-level port)
      .I(d[5]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

IOBUF Io6 (
      .O(din[6]),     // Buffer output
      .IO(ad[6]),   // Buffer inout port (connect directly to top-level port)
      .I(d[6]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

IOBUF Io7 (
      .O(din[7]),     // Buffer output
      .IO(ad[7]),   // Buffer inout port (connect directly to top-level port)
      .I(d[7]),     // Buffer input
      .T(oen)      // 3-state enable input 
);

	//-------------------------------------------------------------------
	// 4 digit LED display for DEBUG
	//-------------------------------------------------------------------
	always @ (posedge clk)
		selcntr <= selcntr + 1;
		
	leddisplay leddisp1(
		.ain(la[3:0]),
		.bin(la[7:4]),
		.cin(ea[3:0]),
		.din(ea[7:4]),
		.led(ledseg),
		.sel(selcntr[7:6]),
		.dig(leddig)
);

   BUFG BUFG1 (
      .O(clk),     // Clock buffer output
      .I(clkin)      // Clock buffer input
   );

endmodule
