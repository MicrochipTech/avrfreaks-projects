`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    12:02:43 11/16/2007 
// Design Name: 
// Module Name:    vga 
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
module vga(clk, reset,wd, A, D, O, H, V, R, G, B,db,vIrq);
    input clk;		//50MHz
    input reset;
	 input wd;
    input [15:0] A;
    input [7:0] D;
    output [7:0] O;
    output H;
    output V;
    output R;
    output G;
    output B;
	output [7:0] db;
	output vIrq;	//interrupts from vga interface
	
	wire vIrq;
	reg Hp;
	reg R,G,B;
	reg Vp;
//	reg VP;
	reg [7:0] O;
	
	//-------------------
	// pixel clock vars
	//-------------------
	reg pixclk;
	
	//------------------------
	// horizontal sync vars
	//------------------------
	
	reg [9:0] HCounter;
	reg [1:0] HState,NS;
	reg [9:0] Hmux;
	reg Ht;
	reg HDisp;
	
	parameter HSTATE_SYNC	=	2'b00;
	parameter HSTATE_BP		=	2'b01;
	parameter HSTATE_DISP	=	2'b10;
	parameter HSTATE_FP		=	2'b11;
	
	//-------------------------
	// Vertical sync
	//-------------------------
	
	reg [9:0] LineCounter;
	reg [10:0] VSyncCounter;
	reg StartV;
//	reg Vbp;		//vertical back poarch
	reg VDisp;
	
	//---------------------------
	// font ram
	//---------------------------
	
	reg wdFontRam,rdFontRam;
	wire[7:0] FontD,PixData;
	reg [2:0] FontLine;
	reg [6:0] CharCol;
	reg [5:0] CharRow;
	reg [2:0] PixCount;
	wire [12:0] RowMult;
	//---------------------------
	// char ram
	//---------------------------
	
	reg wdCharRam,rdCharRam;
	wire[7:0] CharD,Char,Attrib;
	reg [12:0] CharAdr;
	reg [12:0] AdrMux;
	reg Pix;
	
	//----------------------------
	// Hardware Scrolling
	//----------------------------
	
	reg [2:0] vsCS,vsNS;
	reg [12:0] wAdr,rAdr;
	reg [15:0] vsHold;
	reg doBlockMove,irqBlockMove;
	reg startBlockMove,clrBlockMove;
	wire MuxSelChar,MuxSelRead,MuxSelWrite;
	wire ResetAddresses,IncAddresses,wdHold;
	wire wdRam;
	reg rdStatus;
	
	reg [8:0] Ctl;		//control line outputs from state machine
	
	//state definitions
	
	parameter VS_IDLE		=	3'b000;
	parameter VS_READ		=	3'b001;
	parameter VS_NOP		=	3'b010;
	parameter VS_WRITE	=	3'b011;
	parameter VS_DONE		=	3'b100;
	
	// control line defs
	
	parameter CTL_MUX_CHAR		=	9'b000000001;
	parameter CTL_MUX_READ		=	9'b000000010;
	parameter CTL_MUX_WRITE		=	9'b000000100;
	parameter CTL_RST_ADR		=	9'b000001000;
	parameter CTL_INC_ADR		=	9'b000010000;
	parameter CTL_WD_HOLD		=	9'b000100000;
	parameter CTL_WD_RAM			=	9'b001000000;
	parameter CTL_DONE			=	9'b010000000;
	
	assign MuxSelChar = Ctl[0];
	assign MuxSelRead = Ctl[1];
	assign MuxSelWrite = Ctl[2];
	assign ResetAddresses = Ctl[3];
	assign IncAddresses = Ctl[4];
	assign wdHold = Ctl[5];
	assign wdRam = Ctl[6];
	assign doneBlockMove = Ctl[7];
	
	//----------------------------
	// Color look up tables
	//----------------------------
	
	wire [2:0] Bgc,Fgc;
	reg wdLut,rdLut;
	wire [5:0] LutDat;
	
	//----------------------------
	// addresses
	//----------------------------
	
	parameter ADR_CHARRAM		=	16'h4000;
	parameter ADR_CHARRAMEND	=	16'h67ff;
	parameter ADR_FONTRAM		=	16'h6800;
	parameter ADR_FONTRAM_END	=	16'h6fff;
	parameter ADR_LUTRAM			=	16'h7000;
	parameter ADR_LUTRAM_END	=	16'h700f;
	parameter ADR_CTL				=	16'h7010;	//various control/status function
	
	/////////////////////////////////////////////////////////////////////
	// 25MHz pixel clock
	/////////////////////////////////////////////////////////////////////
	
	always @ (posedge clk or posedge reset)
		if(reset)
			pixclk <= 0;
		else
			pixclk <= ~pixclk;
	
	/////////////////////////////////////////////////////////////////////
	// horizontal sync gen
	/////////////////////////////////////////////////////////////////////
	
	always @ (HState) begin
		case (HState)
			HSTATE_SYNC: Hmux = 47;
			HSTATE_BP:	Hmux = 639;
			HSTATE_DISP: Hmux = 16;
			HSTATE_FP: Hmux = 95;
			default: Hmux = 47;
		endcase
	end
	
	always @ (posedge clk or posedge reset) begin
		if(reset)
			HCounter <= 95;
		else if(pixclk) begin
			if(HCounter == 0)
				HCounter <= Hmux;
			else
				HCounter <= HCounter - 1;
		end
	end
	
	always @(posedge clk or posedge reset)
		if(reset)
			HState <= HSTATE_SYNC;
		else if (pixclk)
			HState <= NS;
			
	always @ (HCounter or HState) begin
		case(HState)
			HSTATE_SYNC: begin
				if(HCounter == 0)
					NS = HSTATE_BP;
				else
					NS = HSTATE_SYNC;
				Hp = 1;
				HDisp = 0;
			end
			HSTATE_BP:begin
				if(HCounter == 0)
					NS = HSTATE_DISP;
				else
					NS = HSTATE_BP;
				Hp = 0;
				HDisp = 0;
			end
			HSTATE_DISP:begin
				if(HCounter == 0)
					NS = HSTATE_FP;
				else
					NS = HSTATE_DISP;
				Hp = 0;
				HDisp = 1;
			end
			HSTATE_FP:begin
				if(HCounter == 0)
					NS = HSTATE_SYNC;
				else
					NS = HSTATE_FP;
				Hp = 0;
				HDisp = 0;
			end
			default:begin
				NS = HSTATE_SYNC;
				Hp = 0;
				HDisp = 0;
			end
		endcase
	end

	always @ (posedge clk or posedge reset)
		if(reset)
			Ht = 0;
		else
			if((HState == HSTATE_FP) && (HCounter == 0) && pixclk)
				Ht = 1;
			else
				Ht = 0;
	////////////////////////////////////////////////////////////
	// Vertical Sync timing
	////////////////////////////////////////////////////////////
	
	always @ (posedge clk or posedge reset) begin
		if(reset)
			LineCounter <= 520;
		else
			if(Ht) begin
				if(LineCounter == 0)
					LineCounter <= 520;
				else
					LineCounter <= LineCounter - 1;
			end
	end
	
	always @ (Ht or LineCounter)
		if(Ht && (LineCounter == 0))
			StartV = 1;
		else
			StartV = 0;
			
	always @ (posedge clk or posedge reset) begin
		if(reset)
			Vp <= 0;
		else if (StartV)
			Vp <= 1;
		else if (VSyncCounter == 0)
			Vp <= 0;
	end
	
	assign V = ~Vp;
	assign H = ~Hp;
	
	always @ (posedge clk or posedge reset) begin
		if(reset)
			VSyncCounter <= 1599;
		else if (pixclk && Vp)
			VSyncCounter <= VSyncCounter - 1;
		else if (!pixclk && Vp)
			VSyncCounter <= VSyncCounter;
		else
			VSyncCounter <= 1599;
	end
	
//	always @ (LineCounter)
//		if((LineCounter <= 519) && (LineCounter > 490))
//			Vbp = 1;
//		else
//			Vbp = 0;
			
	always @ (LineCounter)
		if((LineCounter <= 490) && (LineCounter > 10))
			VDisp = 1;
		else
			VDisp = 0;

//--------------------------------------------------------
// display memory
//--------------------------------------------------------
	always @ (A or wd)
		if((A >= ADR_LUTRAM) && (A <= ADR_LUTRAM_END) && wd)
			wdLut = 1;
		else
			wdLut = 0;

	always @ (A)
		if((A >= ADR_LUTRAM) && (A <= ADR_LUTRAM_END))
			rdLut = 1;
		else
			rdLut = 0;
			
	always @ (A or wd)
		if((A >= ADR_CHARRAM) && (A <= ADR_CHARRAMEND) && wd)
			wdCharRam = 1;
		else
			wdCharRam = 0;
	
	always @ (A or wd)
		if((A >= ADR_FONTRAM) && ( A <= ADR_FONTRAM_END) && wd)
			wdFontRam = 1;
		else
			wdFontRam = 0;

	always @ (A)
		if((A >= ADR_CHARRAM) && (A <= ADR_CHARRAMEND))
			rdCharRam = 1;
		else
			rdCharRam = 0;
	
	always @ (A)
		if((A >= ADR_FONTRAM) && ( A <= ADR_FONTRAM_END))
			rdFontRam = 1;
		else
			rdFontRam = 0;

	always @ (A or wd or D)
		if((A == ADR_CTL) && wd && D[0])
			startBlockMove = 1;
		else
			startBlockMove = 0;

	always @ (A or wd or D)
		if((A == ADR_CTL) && wd && D[1])
			clrBlockMove = 1;
		else
			clrBlockMove = 0;
			
	always @ (A)
		if(A == ADR_CTL)
			rdStatus = 1;
		else
			rdStatus = 0;
			
	always @ (rdFontRam or rdCharRam or FontD or CharD or rdLut or LutDat or rdStatus or doBlockMove or irqBlockMove) begin
		if(rdFontRam) O = FontD;
		else if (rdCharRam) O = CharD;
		else if (rdLut) O = {2'b0,LutDat};
		else if (rdStatus) O = {6'b0,irqBlockMove,doBlockMove};
		else O = 8'h5a;
	end
	
/********************************************
input clka;
input [7 : 0] dina;
input [10 : 0] addra;
input [0 : 0] wea;
output [7 : 0] douta;
input clkb;
input [7 : 0] dinb;
input [10 : 0] addrb;
input [0 : 0] web;
output [7 : 0] doutb;
 Font Ram
*********************************************/

dpram2048x8 font(
	.clka(clk),
	.dina(D),
	.addra(A[10:0]),
	.wea(wdFontRam),
	.douta(FontD),
	.clkb(clk),
	.dinb(8'b0),
	.addrb({Char,FontLine}),
	.web(1'b0),
	.doutb(PixData)
);


/**********************************************

input clka;
input [7 : 0] dina;
input [13 : 0] addra;
input [0 : 0] wea;
output [7 : 0] douta;
input clkb;
input [15 : 0] dinb;
input [12 : 0] addrb;
input [0 : 0] web;
output [15 : 0] doutb;
*********************************************/

dpram10240x8 charram(
	.clka(clk),
	.dina(D),
	.addra(A[13:0]),
	.wea(wdCharRam),
	.douta(CharD),
	.clkb(clk),
	.dinb(vsHold),
	.addrb(AdrMux),
	.web(wdRam),
	.doutb({Attrib,Char})
);

//-----------------------------------------------
// Pixel Mux
//-----------------------------------------------

always @ (PixCount or PixData)
	case (PixCount)
		3'b000: Pix = PixData[7];
		3'b001: Pix = PixData[6];
		3'b010: Pix = PixData[5];
		3'b011: Pix = PixData[4];
		3'b100: Pix = PixData[3];
		3'b101: Pix = PixData[2];
		3'b110: Pix = PixData[1];
		3'b111: Pix = PixData[0];
		default: Pix = PixData[7];
	endcase
/*
input [3 : 0] a;
input [2 : 0] d;
input [3 : 0] dpra;
input clk;
input we;
output [2 : 0] spo;
output [2 : 0] dpo;
*/

ColorLUT BackGround(
	.a(A[3:0]),
	.d(D[2:0]),
	.dpra({1'b0,Attrib[2:0]}),
	.clk(clk),
	.we(wdLut),
	.spo(LutDat[2:0]),
	.dpo(Bgc)
);

ColorLUT ForeGround(
	.a(A[3:0]),
	.d(D[5:3]),
	.dpra({1'b0,Attrib[5:3]}),
	.clk(clk),
	.we(wdLut),
	.spo(LutDat[5:3]),
	.dpo(Fgc)
);

always @ (posedge clk or posedge reset)
	if(reset)
		R <= 0;
	else if(VDisp && HDisp && Pix && !Attrib[7])
		R <= Fgc[0];
	else if(VDisp && HDisp && !Pix && !Attrib[7])
		R <= Bgc[0];
	else if(VDisp && HDisp && Pix && Attrib[7])
		R <= Bgc[0];
	else if (VDisp && HDisp && !Pix && Attrib[7])
		R <= Fgc[0];
	else
		R <= 0;
		
always @ (posedge clk or posedge reset)
	if(reset)
		G <= 0;
	else if(VDisp && HDisp && Pix && !Attrib[7])
		G <= Fgc[1];
	else if(VDisp && HDisp && !Pix && !Attrib[7])
		G <= Bgc[1];
	else if(VDisp && HDisp && Pix && Attrib[7])
		G <= Bgc[1];
	else if (VDisp && HDisp && !Pix && Attrib[7])
		G <= Fgc[1];
	else
		G <= 0;

always @ (posedge clk or posedge reset)
	if(reset)
		B <= 0;
	else if(VDisp && HDisp && Pix && !Attrib[7])
		B <= Fgc[2];
	else if(VDisp && HDisp && !Pix && !Attrib[7])
		B <= Bgc[2];
	else if(VDisp && HDisp && Pix && Attrib[7])
		B <= Bgc[2];
	else if (VDisp && HDisp && !Pix && Attrib[7])
		B <= Fgc[2];
	else
		B <= 0;
		
//-----------------------------------------------
// display counters
//-----------------------------------------------

always @ (posedge clk or posedge reset)
	if(reset)
		FontLine <= 0;
	else if (VDisp && HDisp && (HState == HSTATE_DISP) && (HCounter == 0) && pixclk)
		FontLine <= FontLine + 1;
	else if (VSyncCounter == 0)
		FontLine <= 0;
		
always @ (posedge clk or posedge reset)
	if(reset)
		CharCol <= 0;
	else if (HState == HSTATE_SYNC)
		CharCol <= 0;
	else if (pixclk && (PixCount == 7) && VDisp && HDisp)
		CharCol <= CharCol + 1;

always @ (posedge clk or posedge reset)		
	if(reset)
		CharRow <= 0;
	else if (VDisp && HDisp && (HState == HSTATE_DISP) && (HCounter == 0) && (FontLine == 7) && pixclk)
		CharRow <= CharRow +	1;
	else if (VSyncCounter == 0)
		CharRow <= 0;

always @ (posedge clk or posedge reset)
	if(reset)
		PixCount <= 0;
	else if (VDisp && HDisp && pixclk)
		PixCount <= PixCount + 1;
	else if (VSyncCounter == 0)
		PixCount <= 0;
		
always @ (CharCol or RowMult)
	CharAdr = {5'b00000,CharCol} + {RowMult[12:0]};
/*
  input [5 : 0] a;
  output [12 : 0] p;
*/

mult12 RowMult1(
	.a(CharRow),
	.p(RowMult)
);
			
//*********************************************
// block move hardware
//*********************************************

always @ (posedge clk or posedge reset)
	if(reset)
		doBlockMove <= 0;
	else if (startBlockMove)	//start the block move hardware
		doBlockMove <= 1;
	else if (doneBlockMove)		//block move hardware has finished
		doBlockMove <= 0;			//clear status flag

always @ (posedge clk or posedge reset)
	if(reset)
		irqBlockMove <= 0;
	else if(doneBlockMove)
		irqBlockMove <= 1;	//generate interrupt
	else if (clrBlockMove)
		irqBlockMove <= 0;	//clear interrupt
		
assign vIrq = ~irqBlockMove;

always @ (posedge clk or posedge reset)
	if(reset)
		vsCS <= VS_IDLE;
	else
		vsCS <= vsNS;
		
always @ (vsCS or doBlockMove or Vp) begin
	case (vsCS)
		VS_IDLE: begin
			if(doBlockMove && Vp) begin
				//-----------------------------------
				// only start the block move when
				// we have a flag that says to start
				// AND we are at the verticle blank
				//-----------------------------------
				vsNS = VS_READ;
				Ctl = CTL_MUX_READ | CTL_RST_ADR;
			end
			else begin
				vsNS = VS_IDLE;
				Ctl = CTL_MUX_CHAR;
			end
		end
		VS_READ: begin
			if(rAdr == 4800) begin
				Ctl = CTL_MUX_CHAR;
				vsNS = VS_DONE;
			end
			else begin
				Ctl = CTL_MUX_READ;
				vsNS = VS_NOP;
			end
		end
		VS_NOP: begin
			Ctl = CTL_WD_HOLD | CTL_MUX_READ;
			vsNS = VS_WRITE;
		end
		VS_WRITE: begin
			Ctl = CTL_WD_RAM | CTL_MUX_WRITE | CTL_INC_ADR;
			vsNS = VS_READ;
		end
		VS_DONE: begin
			vsNS = VS_IDLE;
			Ctl = CTL_DONE | CTL_MUX_CHAR;
		end
		default: begin
			vsNS = VS_IDLE;
			Ctl = CTL_MUX_CHAR;
		end
	endcase
end

always @ (posedge clk)
	if(ResetAddresses)
		wAdr <= 0;
	else if (IncAddresses)
		wAdr <= wAdr + 1;
		
always @ (posedge clk)
	if(ResetAddresses)
		rAdr <= 80;
	else if (IncAddresses)
		rAdr <= rAdr + 1;

always @ (posedge clk)
	if(wdHold)
		vsHold <= {Attrib,Char};

always @ (MuxSelChar or MuxSelRead or MuxSelWrite)
	case ({MuxSelChar,MuxSelRead,MuxSelWrite})
		3'b100: AdrMux = CharAdr;
		3'b010: AdrMux = rAdr;
		3'b001: AdrMux = wAdr;
		default: AdrMux = CharAdr;
	endcase
	
//---------------------------------------------
// debug connections
//---------------------------------------------
			
assign db[0] = doBlockMove;
assign db[1] = VDisp;
assign db[2] = V;
assign db[3] = irqBlockMove;
assign db[4] = 0;
assign db[5] = 0;
assign db[6] = 0;
assign db[7] = 0;
		
endmodule
