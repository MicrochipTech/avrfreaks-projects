/*******************************************************************************
*     This file is owned and controlled by Xilinx and must be used             *
*     solely for design, simulation, implementation and creation of            *
*     design files limited to Xilinx devices or technologies. Use              *
*     with non-Xilinx devices or technologies is expressly prohibited          *
*     and immediately terminates your license.                                 *
*                                                                              *
*     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"            *
*     SOLELY FOR USE IN DEVELOPING PROGRAMS AND SOLUTIONS FOR                  *
*     XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION          *
*     AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE, APPLICATION              *
*     OR STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS                *
*     IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,                  *
*     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE         *
*     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY                 *
*     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE                  *
*     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR           *
*     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF          *
*     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          *
*     FOR A PARTICULAR PURPOSE.                                                *
*                                                                              *
*     Xilinx products are not intended for use in life support                 *
*     appliances, devices, or systems. Use in such applications are            *
*     expressly prohibited.                                                    *
*                                                                              *
*     (c) Copyright 1995-2006 Xilinx, Inc.                                     *
*     All rights reserved.                                                     *
*******************************************************************************/
// The synthesis directives "translate_off/translate_on" specified below are
// supported by Xilinx, Mentor Graphics and Synplicity synthesis
// tools. Ensure they are correct for your synthesis tool(s).

// You must compile the wrapper file ColorLUT.v when simulating
// the core, ColorLUT. When compiling the wrapper file, be sure to
// reference the XilinxCoreLib Verilog simulation library. For detailed
// instructions, please refer to the "CORE Generator Help".

`timescale 1ns/1ps

module ColorLUT(
	a,
	d,
	dpra,
	clk,
	we,
	spo,
	dpo);


input [3 : 0] a;
input [2 : 0] d;
input [3 : 0] dpra;
input clk;
input we;
output [2 : 0] spo;
output [2 : 0] dpo;

// synthesis translate_off

      DIST_MEM_GEN_V3_2 #(
		.C_ADDR_WIDTH(4),
		.C_DEFAULT_DATA("0"),
		.C_DEPTH(16),
		.C_HAS_CLK(1),
		.C_HAS_D(1),
		.C_HAS_DPO(1),
		.C_HAS_DPRA(1),
		.C_HAS_I_CE(0),
		.C_HAS_QDPO(0),
		.C_HAS_QDPO_CE(0),
		.C_HAS_QDPO_CLK(0),
		.C_HAS_QDPO_RST(0),
		.C_HAS_QDPO_SRST(0),
		.C_HAS_QSPO(0),
		.C_HAS_QSPO_CE(0),
		.C_HAS_QSPO_RST(0),
		.C_HAS_QSPO_SRST(0),
		.C_HAS_SPO(1),
		.C_HAS_SPRA(0),
		.C_HAS_WE(1),
		.C_MEM_INIT_FILE("no_coe_file_loaded"),
		.C_MEM_TYPE(2),
		.C_PIPELINE_STAGES(0),
		.C_QCE_JOINED(0),
		.C_QUALIFY_WE(0),
		.C_READ_MIF(0),
		.C_REG_A_D_INPUTS(0),
		.C_REG_DPRA_INPUT(0),
		.C_SYNC_ENABLE(1),
		.C_WIDTH(3))
	inst (
		.A(a),
		.D(d),
		.DPRA(dpra),
		.CLK(clk),
		.WE(we),
		.SPO(spo),
		.DPO(dpo),
		.SPRA(),
		.I_CE(),
		.QSPO_CE(),
		.QDPO_CE(),
		.QDPO_CLK(),
		.QSPO_RST(),
		.QDPO_RST(),
		.QSPO_SRST(),
		.QDPO_SRST(),
		.QSPO(),
		.QDPO());


// synthesis translate_on

// XST black box declaration
// box_type "black_box"
// synthesis attribute box_type of ColorLUT is "black_box"

endmodule

