`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Company: Zaca Design
// Engineer: Jim Patchell
//
// Create Date:    8:13:34 01/22/06
// Design Name:	AVR host interface to Spartan III 
// Module Name:    avrif
// Project Name:   
// Target Device:  
// Tool versions:  
// Description:
//
// Dependencies:
// 
// 
////////////////////////////////////////////////////////////////////////////////
module avrif(clk, reset, ad, d, a, ale, wd, rd,oen,la,led,EA,irq,tirq,db,H,V,R,G,B);
   input clk;				//system clock
   output reset; 			//system reset
   input [7:0] ad;		//address data input
   output [7:0] d;		//data output
   input [7:0] a;			//upper address bits
   input ale;				//address latch eneable
   input wd;				//write data (active low)
   input rd;				//read data (active low)
	output oen;				//output tristate enable
	output [7:0]la,EA;
	output [7:0] led;
	output irq;
	output tirq;
	output [7:0] db;
	output H,V,R,G,B;
	
	reg reset;
	wire toggle;
	wire tirq;
	wire irq;
	wire [7:0] led;
	reg [7:0]la,EA;
	wire audiopwm;
	wire Sce,Sdo,Sclk;
	reg [7:0] LedOut;
	reg wdLed,rdLed;
	wire Srst;
	wire H,V,R,G,B;
	
	reg [7:0] d;
	reg [7:0] alow;		//lower portion of the address
	reg oen;
	reg wdedge, wdstrobe;

	wire [15:0] adr;
	//-------------------------------------------------------------------
	// Sample rate Generator
	//-------------------------------------------------------------------
	reg [11:0]srCntr;
	reg sample;
	
	//-------------------------------------------------------------------
	// address decoder stuff
	//-------------------------------------------------------------------

	//-------------------------------------------------------------------
	// VGA interface
	//-------------------------------------------------------------------
	
	wire [7:0] vgadata;
	reg wdVGA;
	reg rdVGA;
	wire vgaIrq;
	
	//-------------------------------------------------------------------
	// Control Port
	// Bit defines:
	//	CTL0:Enable Tone Generator
	//-------------------------------------------------------------------
	
	reg [7:0] CtlPort;
	reg wdCtl,rdCtl;
	reg SingleStep;
	
	//-------------------------------------------------------------------
	// timer
	//-------------------------------------------------------------------
	
	reg rdTimer,wdTimer;
	wire [7:0] TimerD;
	wire Timerirq;
	
	//-------------------------------------------------------------------
	// led display
	//-------------------------------------------------------------------
	
	reg wdLedDispLo,wdLedDispHi,rdLedDispLo,rdLedDispHi;
	
	//-------------------------------------------------------------------
	// Address defines
	//-------------------------------------------------------------------
	parameter STATUSLED 	= 16'hf000;	//led status port
	parameter ADR_RESET	= 16'hf001;	//reset port for fpga
	parameter ADR_LEDDISPLO	=	16'hf002;
	parameter ADR_LEDDISPHI	=	16'hf003;
	parameter ADR_SERPER = 16'hf004;	//serial periphera transmitter

	parameter ADR_SER_TFIFOLO = 16'hf008;	//transmit fifo status low word
	parameter ADR_SER_TFIFOHI = 16'hf009;
	parameter ADR_SER_RFIFOLO = 16'hf00a;
	parameter ADR_SER_RFIFOHI = 16'hf00b;
	parameter ADR_SER_STATUS  = 16'hf00c;
	parameter ADR_VGA_START		=	16'h4000;
	parameter ADR_VGA_END		=	16'h701f;
	
	parameter ADR_SINGLE = 16'hf021;	//single step hardware
	parameter ADR_CTL		= 16'hf020;	//control port
	parameter ADR_TIMER	= 16'hf040;	//timer port
	parameter ADR_TIMER_BLK = 13'h1e8;	//timer port address range
	
	//-----------------------------------------------------------------
	// test points
	//-----------------------------------------------------------------
/*	
	assign tp[0] = wdstrobe;
	assign tp[1] = wdLed;
	assign tp[2] = oen;
	assign tp[3] = rdLed;
*/
	//--------------------------------------------------
	// address latch
	//--------------------------------------------------

	always @ (negedge ale) begin
		alow <= ad;			//latch address on falling edge of ale
	end

	assign adr = {a,alow};	//generate internal address bus

	//--------------------------------------------------
	// generate write data strobe
	//--------------------------------------------------

	always @ (negedge wd or posedge wdstrobe) begin
		if(wdstrobe)
			wdedge <= 0;	//reset
		else
			wdedge <= 1;	//set
	end

	always @ (posedge clk)
		wdstrobe <= wdedge;

	//-----------------------------------------------------
	// Control Register
	//-----------------------------------------------------
	
	always @ (posedge clk)
		if(wdCtl) CtlPort <= ad;

	assign Srst = CtlPort[0];
	
	//-----------------------------------------------------
	// Address decode Write Logic
	//-----------------------------------------------------

	always @ (adr or wdstrobe or ad)
		if((adr == ADR_RESET) && (wdstrobe) && (ad == 8'h5a))
			reset = 1;
		else
			reset = 0;
			
	always @ (adr or wdstrobe) begin
		if((adr == STATUSLED) && (wdstrobe))
			wdLed = 1;
		else
			wdLed = 0;
	end

	always @ (posedge clk) begin
		if( (adr == ADR_SINGLE) && wdstrobe)
			SingleStep <= 1;
		else
			SingleStep <= 0;
	end
	
	always @ (adr or wdstrobe) begin
		if((adr == ADR_CTL) && wdstrobe)
			wdCtl = 1;
		else
			wdCtl = 0;
	end
		
	always @ (adr or wdstrobe)
		if( (adr >= ADR_TIMER) && (adr <= ADR_TIMER + 7) && wdstrobe)
			wdTimer = 1;
		else
			wdTimer = 0;
	
	always @ (adr or wdstrobe)
		if( (adr >= ADR_VGA_START) && (adr <= ADR_VGA_END) && wdstrobe)
			wdVGA = 1;
		else
			wdVGA = 0;
			
	always @ (adr or rd)
		if( (adr >= ADR_VGA_START) && (adr <= ADR_VGA_END) && (rd == 0))
			rdVGA = 1;
		else
			rdVGA = 0;
	
	always @ (adr or wdstrobe)
		if( (adr == ADR_LEDDISPLO) && wdstrobe)
			wdLedDispLo = 1;
		else
			wdLedDispLo = 0;
			
	always @ (adr or rd)
		if( (adr == ADR_LEDDISPLO) && (rd == 0) )
			rdLedDispLo = 1;
		else
			rdLedDispLo = 0;
	
	always @ (adr or wdstrobe)
		if( (adr == ADR_LEDDISPHI) && wdstrobe)
			wdLedDispHi = 1;
		else
			wdLedDispHi = 0;

	always @ (adr or rd)
		if( (adr == ADR_LEDDISPHI) && (rd == 0) )
			rdLedDispHi = 1;
		else
			rdLedDispHi = 0;
			
	//-----------------------------------------------------------------------------
	// Output enable for data bus tri state
	//-----------------------------------------------------------------------------
	always @ (adr or rd) begin
	//------------------------------------------
	// rd is active low
	//------------------------------------------
		if(rd == 0) begin
			if(adr[15:8] == 8'hf0) oen = 0;
			else if (rdVGA) oen = 0;
			else oen = 1;
		end
		else
			oen = 1;
	end

	//---------------------------------------------------
	// mux for reading stuff back
	//---------------------------------------------------

	always @ (adr)
		if(adr == STATUSLED)
			rdLed = 1;
		else
			rdLed = 0;

	always @ (adr)
		if(adr == ADR_CTL)
			rdCtl = 1;
		else
			rdCtl = 0;

	always @ (adr)
		if( (adr >= ADR_TIMER) && (adr <= ADR_TIMER + 7))
			rdTimer = 1;
		else
			rdTimer = 0;
			
	always @ (rdLed or LedOut or 
				rdCtl or CtlPort or rdVGA or vgadata or
				rdTimer or TimerD or
				rdLedDispLo or rdLedDispHi or la or EA) 
	begin
		if(rdLed) d = LedOut;
		else if (rdCtl) d = CtlPort;
		else if (rdTimer) d = TimerD;
		else if (rdVGA) d = vgadata;
		else if (rdLedDispLo) d = la;
		else if (rdLedDispHi) d = EA;
		else
			d = 8'b01111011;
	end

	//---------------------------------------------------
	// Led Display Register
	//---------------------------------------------------
	
	always @ (posedge clk)
		if(wdLedDispLo)
			la <= ad;
			
	always @ (posedge clk)
		if(wdLedDispHi)
			EA <= ad;
			
	//---------------------------------------------------
	// Data register for status LEDs
	//---------------------------------------------------

	always @ (posedge clk or posedge reset) begin
		if(reset)
			LedOut <= 8'b00000000;
		else begin
			if(wdLed) LedOut <= ad;
		end
	end

	assign led = LedOut;

vga vga1 (
    .clk(clk), 
    .reset(reset), 
    .wd(wdVGA), 
    .A(adr), 
    .D(ad), 
    .O(vgadata), 
    .H(H), 
    .V(V), 
    .R(R), 
    .G(G), 
    .B(B),
	 .db(db),
	 .vIrq(vgaIRQ)
);

timer timer1 (
    .clk(clk), 
    .reset(reset), 
    .a(adr[2:0]), 
	 .O(TimerD),
    .d(ad), 
    .wdstb(wdTimer), 
    .irq(Timerirq), 
    .toggle(toggle)
    );

assign tirq = ~Timerirq;
assign irq = vgaIRQ;
/*
assign db[0] = oen;
assign db[1] = wdstrobe;
assign db[2] = wdVGA;
assign db[3] = rdVGA;
assign db[4] = wdLed;
assign db[5] = ale;
assign db[6] = 0;
assign db[7] = 0;
*/
endmodule
