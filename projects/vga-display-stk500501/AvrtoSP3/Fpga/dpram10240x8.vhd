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
-- You must compile the wrapper file dpram10240x8.vhd when simulating
-- the core, dpram10240x8. When compiling the wrapper file, be sure to
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
ENTITY dpram10240x8 IS
	port (
	clka: IN std_logic;
	dina: IN std_logic_VECTOR(7 downto 0);
	addra: IN std_logic_VECTOR(13 downto 0);
	wea: IN std_logic_VECTOR(0 downto 0);
	douta: OUT std_logic_VECTOR(7 downto 0);
	clkb: IN std_logic;
	dinb: IN std_logic_VECTOR(15 downto 0);
	addrb: IN std_logic_VECTOR(12 downto 0);
	web: IN std_logic_VECTOR(0 downto 0);
	doutb: OUT std_logic_VECTOR(15 downto 0));
END dpram10240x8;

ARCHITECTURE dpram10240x8_a OF dpram10240x8 IS
-- synthesis translate_off
component wrapped_dpram10240x8
	port (
	clka: IN std_logic;
	dina: IN std_logic_VECTOR(7 downto 0);
	addra: IN std_logic_VECTOR(13 downto 0);
	wea: IN std_logic_VECTOR(0 downto 0);
	douta: OUT std_logic_VECTOR(7 downto 0);
	clkb: IN std_logic;
	dinb: IN std_logic_VECTOR(15 downto 0);
	addrb: IN std_logic_VECTOR(12 downto 0);
	web: IN std_logic_VECTOR(0 downto 0);
	doutb: OUT std_logic_VECTOR(15 downto 0));
end component;

-- Configuration specification 
	for all : wrapped_dpram10240x8 use entity XilinxCoreLib.blk_mem_gen_v2_1(behavioral)
		generic map(
			c_has_regceb => 0,
			c_has_regcea => 0,
			c_mem_type => 2,
			c_has_mux_output_regs => 0,
			c_prim_type => 1,
			c_sinita_val => "0",
			c_read_width_b => 16,
			c_family => "spartan3",
			c_read_width_a => 8,
			c_disable_warn_bhv_coll => 0,
			c_init_file_name => "no_coe_file_loaded",
			c_write_mode_b => "WRITE_FIRST",
			c_write_mode_a => "WRITE_FIRST",
			c_load_init_file => 0,
			c_write_depth_b => 5120,
			c_write_depth_a => 10240,
			c_has_ssrb => 0,
			c_has_ssra => 0,
			c_addra_width => 14,
			c_addrb_width => 13,
			c_default_data => "0",
			c_algorithm => 1,
			c_disable_warn_bhv_range => 0,
			c_has_mem_output_regs => 0,
			c_write_width_b => 16,
			c_write_width_a => 8,
			c_read_depth_b => 5120,
			c_read_depth_a => 10240,
			c_byte_size => 9,
			c_sim_collision_check => "ALL",
			c_common_clk => 0,
			c_wea_width => 1,
			c_has_enb => 0,
			c_web_width => 1,
			c_has_ena => 0,
			c_sinitb_val => "0",
			c_use_byte_web => 0,
			c_use_byte_wea => 0,
			c_use_default_data => 0);
-- synthesis translate_on
BEGIN
-- synthesis translate_off
U0 : wrapped_dpram10240x8
		port map (
			clka => clka,
			dina => dina,
			addra => addra,
			wea => wea,
			douta => douta,
			clkb => clkb,
			dinb => dinb,
			addrb => addrb,
			web => web,
			doutb => doutb);
-- synthesis translate_on

END dpram10240x8_a;

