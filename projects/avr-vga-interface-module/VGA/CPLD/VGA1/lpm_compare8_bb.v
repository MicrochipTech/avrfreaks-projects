// megafunction wizard: %LPM_COMPARE%VBB%
// GENERATION: STANDARD
// VERSION: WM1.0
// MODULE: lpm_compare 

// ============================================================
// File Name: lpm_compare8.v
// Megafunction Name(s):
// 			lpm_compare
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

module lpm_compare8 (
	dataa,
	aeb);

	input	[9:0]  dataa;
	output	  aeb;

endmodule

// ============================================================
// CNX file retrieval info
// ============================================================
// Retrieval info: PRIVATE: AeqB NUMERIC "1"
// Retrieval info: PRIVATE: AgeB NUMERIC "0"
// Retrieval info: PRIVATE: AgtB NUMERIC "0"
// Retrieval info: PRIVATE: AleB NUMERIC "0"
// Retrieval info: PRIVATE: AltB NUMERIC "0"
// Retrieval info: PRIVATE: AneB NUMERIC "0"
// Retrieval info: PRIVATE: LPM_PIPELINE NUMERIC "0"
// Retrieval info: PRIVATE: Latency NUMERIC "0"
// Retrieval info: PRIVATE: PortBValue NUMERIC "515"
// Retrieval info: PRIVATE: Radix NUMERIC "10"
// Retrieval info: PRIVATE: SignedCompare NUMERIC "0"
// Retrieval info: PRIVATE: aclr NUMERIC "0"
// Retrieval info: PRIVATE: clken NUMERIC "0"
// Retrieval info: PRIVATE: isPortBConstant NUMERIC "1"
// Retrieval info: PRIVATE: nBit NUMERIC "10"
// Retrieval info: CONSTANT: LPM_HINT STRING "ONE_INPUT_IS_CONSTANT=YES"
// Retrieval info: CONSTANT: LPM_REPRESENTATION STRING "UNSIGNED"
// Retrieval info: CONSTANT: LPM_TYPE STRING "LPM_COMPARE"
// Retrieval info: CONSTANT: LPM_WIDTH NUMERIC "10"
// Retrieval info: USED_PORT: AeB 0 0 0 0 OUTPUT NODEFVAL AeB
// Retrieval info: USED_PORT: dataa 0 0 10 0 INPUT NODEFVAL dataa[9..0]
// Retrieval info: CONNECT: AeB 0 0 0 0 @AeB 0 0 0 0
// Retrieval info: CONNECT: @dataa 0 0 10 0 dataa 0 0 10 0
// Retrieval info: CONNECT: @datab 0 0 10 0 515 0 0 0 0
// Retrieval info: LIBRARY: lpm lpm.lpm_components.all
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_compare8.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_compare8.inc FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_compare8.cmp FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_compare8.bsf TRUE FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_compare8_inst.v FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_compare8_bb.v TRUE
// Retrieval info: LIB_FILE: lpm
