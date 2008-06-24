////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995-2007 Xilinx, Inc.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor: Xilinx
// \   \   \/     Version: J.33
//  \   \         Application: netgen
//  /   /         Filename: mult12.v
// /___/   /\     Timestamp: Thu Dec 06 21:10:17 2007
// \   \  /  \ 
//  \___\/\___\
//             
// Command	: -intstyle ise -w -sim -ofmt verilog E:\xproj\vga\tmp\_cg\mult12.ngc E:\xproj\vga\tmp\_cg\mult12.v 
// Device	: 3s200ft256-5
// Input file	: E:/xproj/vga/tmp/_cg/mult12.ngc
// Output file	: E:/xproj/vga/tmp/_cg/mult12.v
// # of Modules	: 1
// Design Name	: mult12
// Xilinx        : C:\Xilinx91i
//             
// Purpose:    
//     This verilog netlist is a verification model and uses simulation 
//     primitives which may not represent the true implementation of the 
//     device, however the netlist is functionally correct and should not 
//     be modified. This file cannot be synthesized and should only be used 
//     with supported simulation tools.
//             
// Reference:  
//     Development System Reference Guide, Chapter 23
//     Synthesis and Simulation Design Guide, Chapter 6
//             
////////////////////////////////////////////////////////////////////////////////

`timescale 1 ns/1 ps

module mult12 (
a, p
);
  input [5 : 0] a;
  output [12 : 0] p;
  
  // synopsys translate_off
  
  wire \BU2/N1 ;
  wire \BU2/clk ;
  wire \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Pdelay/N1 ;
  wire \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Pdelay/N0 ;
  wire NLW_VCC_P_UNCONNECTED;
  wire NLW_GND_G_UNCONNECTED;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<35>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<34>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<33>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<32>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<31>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<30>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<29>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<28>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<27>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<26>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<25>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<24>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<23>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<22>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<21>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<20>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<19>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<18>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<17>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<16>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<15>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<14>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<13>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<12>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<11>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<10>_UNCONNECTED ;
  wire \NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<9>_UNCONNECTED ;
  wire [5 : 0] a_2;
  wire [0 : 0] NlwRenamedSig_OI_p;
  wire [8 : 0] \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> ;
  assign
    a_2[5] = a[5],
    a_2[4] = a[4],
    a_2[3] = a[3],
    a_2[2] = a[2],
    a_2[1] = a[1],
    a_2[0] = a[0],
    p[12] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [8],
    p[11] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [7],
    p[10] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [6],
    p[9] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [5],
    p[8] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [4],
    p[7] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [3],
    p[6] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [2],
    p[5] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [1],
    p[4] = \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [0],
    p[3] = NlwRenamedSig_OI_p[0],
    p[2] = NlwRenamedSig_OI_p[0],
    p[1] = NlwRenamedSig_OI_p[0],
    p[0] = NlwRenamedSig_OI_p[0];
  VCC VCC_0 (
    .P(NLW_VCC_P_UNCONNECTED)
  );
  GND GND_1 (
    .G(NLW_GND_G_UNCONNECTED)
  );
  MULT18X18 \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>  (
    .A({NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], 
NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], a_2[5], 
a_2[4], a_2[3], a_2[2], a_2[1], a_2[0]}),
    .B({NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], 
NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], 
NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], NlwRenamedSig_OI_p[0], \BU2/N1 , NlwRenamedSig_OI_p[0], \BU2/N1 }),
    .P({\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<35>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<34>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<33>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<32>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<31>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<30>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<29>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<28>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<27>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<26>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<25>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<24>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<23>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<22>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<21>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<20>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<19>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<18>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<17>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<16>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<15>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<14>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<13>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<12>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<11>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<10>_UNCONNECTED , 
\NLW_BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Mmult_pi_mult<0><0>_P<9>_UNCONNECTED , 
\BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [8], \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [7], 
\BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [6], \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [5], 
\BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [4], \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [3], 
\BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [2], \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [1], 
\BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/pi_mult<0><0> [0]})
  );
  VCC \BU2/XST_VCC  (
    .P(\BU2/N1 )
  );
  GND \BU2/XST_GND  (
    .G(NlwRenamedSig_OI_p[0])
  );
  VCC \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Pdelay/XST_VCC  (
    .P(\BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Pdelay/N1 )
  );
  GND \BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Pdelay/XST_GND  (
    .G(\BU2/U0/gCCM.iCCM/emb_mult_ccm.mult18_ccm.mult18_array/Pdelay/N0 )
  );

// synopsys translate_on

endmodule

// synopsys translate_off

`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;

    wire GSR;
    wire GTS;
    wire PRLD;

    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (weak1, weak0) GSR = GSR_int;
    assign (weak1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

endmodule

// synopsys translate_on
