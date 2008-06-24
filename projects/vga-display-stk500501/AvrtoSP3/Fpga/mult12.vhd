--------------------------------------------------------------------------------
--     This file is owned and controlled by Xilinx and must be used           --
--     solely for design, simulation, implementation and creation of          --
--     design files limited to Xilinx devices or technologies. Use            --
--     with non-Xilinx devices or technologies is expressly prohibited        --
--     and immediately terminates your license.                               --
--                                                                            --
--     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"          --
--     SOLELY FOR USE IN DEVELOPING PROGRAMS AND SOLUTIONS FOR                --
--     XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION        --
--     AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE, APPLICATION            --
--     OR STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS              --
--     IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,                --
--     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE       --
--     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY               --
--     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE                --
--     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR         --
--     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF        --
--     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS        --
--     FOR A PARTICULAR PURPOSE.                                              --
--                                                                            --
--     Xilinx products are not intended for use in life support               --
--     appliances, devices, or systems. Use in such applications are          --
--     expressly prohibited.                                                  --
--                                                                            --
--     (c) Copyright 1995-2006 Xilinx, Inc.                                   --
--     All rights reserved.                                                   --
--------------------------------------------------------------------------------
-- You must compile the wrapper file mult12.vhd when simulating
-- the core, mult12. When compiling the wrapper file, be sure to
-- reference the XilinxCoreLib VHDL simulation library. For detailed
-- instructions, please refer to the "CORE Generator Help".

-- The synthesis directives "translate_off/translate_on" specified
-- below are supported by Xilinx, Mentor Graphics and Synplicity
-- synthesis tools. Ensure they are correct for your synthesis tool(s).

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
-- synthesis translate_off
Library XilinxCoreLib;
-- synthesis translate_on
ENTITY mult12 IS
	port (
	a: IN std_logic_VECTOR(5 downto 0);
	p: OUT std_logic_VECTOR(12 downto 0));
END mult12;

ARCHITECTURE mult12_a OF mult12 IS
-- synthesis translate_off
component wrapped_mult12
	port (
	a: IN std_logic_VECTOR(5 downto 0);
	p: OUT std_logic_VECTOR(12 downto 0));
end component;

-- Configuration specification 
	for all : wrapped_mult12 use entity XilinxCoreLib.mult_gen_v9_0(behavioral)
		generic map(
			c_a_width => 6,
			c_b_type => 1,
			c_ce_overrides_sclr => 0,
			c_opt_goal => 1,
			c_has_sclr => 0,
			c_round_pt => 0,
			c_out_high => 12,
			c_mult_type => 2,
			c_ccm_imp => 2,
			c_has_load_done => 0,
			c_pipe_stages => 0,
			c_has_ce => 0,
			c_has_zero_detect => 0,
			c_round_output => 0,
			c_mem_init_prefix => "mgv9",
			c_xdevicefamily => "spartan3",
			c_a_type => 1,
			c_out_low => 0,
			c_b_width => 7,
			c_b_value => "1010000");
-- synthesis translate_on
BEGIN
-- synthesis translate_off
U0 : wrapped_mult12
		port map (
			a => a,
			p => p);
-- synthesis translate_on

END mult12_a;

