// megafunction wizard: %LPM_FF%
// GENERATION: STANDARD
// VERSION: WM1.0
// MODULE: lpm_ff 

// ============================================================
// File Name: lpm_dff0.v
// Megafunction Name(s):
// 			lpm_ff
//
// Simulation Library Files(s):
// 			lpm
// ============================================================
// ************************************************************
// THIS IS A WIZARD-GENERATED FILE. DO NOT EDIT THIS FILE!
//
// 6.1 Build 201 11/27/2006 SJ Web Edition
// ************************************************************


//Copyright (C) 1991-2006 Altera Corporation
//Your use of Altera Corporation's design tools, logic functions 
//and other software and tools, and its AMPP partner logic 
//functions, and any output files from any of the foregoing 
//(including device programming or simulation files), and any 
//associated documentation or information are expressly subject 
//to the terms and conditions of the Altera Program License 
//Subscription Agreement, Altera MegaCore Function License 
//Agreement, or other applicable license agreement, including, 
//without limitation, that your use is for the sole purpose of 
//programming logic devices manufactured by Altera and sold by 
//Altera or its authorized distributors.  Please refer to the 
//applicable agreement for further details.


// synopsys translate_off
`timescale 1 ps / 1 ps
// synopsys translate_on
module lpm_dff0 (
	clock,
	data,
	q);

	input	  clock;
	input	[9:0]  data;
	output	[9:0]  q;

	wire [9:0] sub_wire0;
	wire [9:0] q = sub_wire0[9:0];

	lpm_ff	lpm_ff_component (
				.clock (clock),
				.data (data),
				.q (sub_wire0)
				// synopsys translate_off
				,
				.aclr (),
				.aload (),
				.aset (),
				.enable (),
				.sclr (),
				.sload (),
				.sset ()
				// synopsys translate_on
				);
	defparam
		lpm_ff_component.lpm_fftype = "DFF",
		lpm_ff_component.lpm_type = "LPM_FF",
		lpm_ff_component.lpm_width = 10;


endmodule

// ============================================================
// CNX file retrieval info
// ============================================================
// Retrieval info: PRIVATE: ACLR NUMERIC "0"
// Retrieval info: PRIVATE: ALOAD NUMERIC "0"
// Retrieval info: PRIVATE: ASET NUMERIC "0"
// Retrieval info: PRIVATE: ASETV NUMERIC "0"
// Retrieval info: PRIVATE: ASET_ALL1 NUMERIC "1"
// Retrieval info: PRIVATE: CLK_EN NUMERIC "0"
// Retrieval info: PRIVATE: DFF NUMERIC "1"
// Retrieval info: PRIVATE: SCLR NUMERIC "0"
// Retrieval info: PRIVATE: SLOAD NUMERIC "0"
// Retrieval info: PRIVATE: SSET NUMERIC "0"
// Retrieval info: PRIVATE: SSETV NUMERIC "0"
// Retrieval info: PRIVATE: SSET_ALL1 NUMERIC "1"
// Retrieval info: PRIVATE: UseTFFdataPort NUMERIC "0"
// Retrieval info: PRIVATE: nBit NUMERIC "10"
// Retrieval info: CONSTANT: LPM_FFTYPE STRING "DFF"
// Retrieval info: CONSTANT: LPM_TYPE STRING "LPM_FF"
// Retrieval info: CONSTANT: LPM_WIDTH NUMERIC "10"
// Retrieval info: USED_PORT: clock 0 0 0 0 INPUT NODEFVAL clock
// Retrieval info: USED_PORT: data 0 0 10 0 INPUT NODEFVAL data[9..0]
// Retrieval info: USED_PORT: q 0 0 10 0 OUTPUT NODEFVAL q[9..0]
// Retrieval info: CONNECT: @clock 0 0 0 0 clock 0 0 0 0
// Retrieval info: CONNECT: q 0 0 10 0 @q 0 0 10 0
// Retrieval info: CONNECT: @data 0 0 10 0 data 0 0 10 0
// Retrieval info: LIBRARY: lpm lpm.lpm_components.all
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_dff0.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_dff0.inc FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_dff0.cmp FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_dff0.bsf TRUE FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_dff0_inst.v FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_dff0_bb.v TRUE
// Retrieval info: LIB_FILE: lpm
