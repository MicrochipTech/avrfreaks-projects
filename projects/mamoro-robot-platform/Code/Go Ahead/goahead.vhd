----------------------------------------------------------------------------------
--
-- Author: Jidan Al-Eryani,	jidan@gmx.net  
-- 
-- Date:	17/04/2007 
-- 
-- Purpose: 
-- Simple program to test the interaction between the microcontroller and FPGA.
-- This Program makes the robot platform move forward (hence the name of program = Go Ahead)
-- and turn randomly right or left around an obstacle if one is detected.
-- The sensors are two infrared sensors and the motors encoders.
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity goahead is
    Port ( mcu_clk_i : in  STD_LOGIC;
			  irsense_i : in  STD_LOGIC;
           obstacle_i : in  STD_LOGIC;
           command1_o : out  STD_LOGIC;
           command2_o : out  STD_LOGIC);
end goahead;

--COMMAND2 | COMMAND1 | Action
--		0				0			Turn around right
--		0				1			Turn around left
--		1				0			Move forward
--		1				1			Stop


architecture Behavioral of goahead is

constant random_number : std_logic_vector(29 downto 0) := "011010010111001010101010111101";
signal fwd_counter : integer := 0;
signal random : integer := 0;

begin

process(mcu_clk_i) -- µC generates 128 pulses per second
begin
	if mcu_clk_i='1' and mcu_clk_i'event then
		if irsense_i='1' or (obstacle_i='1' and fwd_counter = 255) then --fwd_counter = 255 are 2 seconds
			command2_o <= '0';
			command1_o <= random_number(random); -- turn right or left randomly
			fwd_counter <= 0;
			if random = 29 then 
				random <= 0; 
			else
				random <= random + 1;
			end if;
		else -- move forward
			if fwd_counter = 255 then 
				fwd_counter <= 0;
			else
				fwd_counter <= fwd_counter + 1;
			end if;
			command2_o <= '1';
			command1_o <= '0';
		end if;
	end if;
end process;

end Behavioral;

