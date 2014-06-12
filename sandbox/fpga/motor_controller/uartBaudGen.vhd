----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    18:00:10 10/21/2007 
-- Design Name: 
-- Module Name:    uart_lib - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

	entity uartBaudGen is
	
		generic( clkFreq : integer := 12e6;
					baudFreq : integer := 19200;
					accWidth : integer := 16 );
					
		port( clk : in std_logic;
				baudTick : out std_logic;
				baud8Tick : out std_logic );
				
		constant baud8_inc : integer := (baudFreq * (2**accWidth)) / (clkFreq);
		
	end uartBaudGen;


architecture Behavioral of uartBaudGen is
		signal baud8_acc : std_logic_vector( accWidth downto 0 );
		signal baud_acc : std_logic_vector( 2 downto 0 );
		signal baud8_tick : std_logic;
		signal baud_tick : std_logic;
	begin
	
	
	-- Generate Baud*8 pulse
		P0 : process(clk,baud8_acc)
			begin

			if( rising_edge(clk) ) then
				baud8_acc <= ('0' & baud8_acc(accWidth-1 downto 0) ) + baud8_inc;
			end if;
			
			baud8_tick <= baud8_acc(accWidth);
			baud8Tick <= baud8_tick;
		
		end process;
		
	-- Generate Baud pulse
		P1 : process(clk,baud_acc,baud8_tick)
		begin
			if( rising_edge(clk) AND baud8_tick='1') then
				baud_acc <= baud_acc+1;
			end if;
			
			if (baud_acc = "000") then
				baud_tick <= '1';
			else
				baud_tick <= '0';
			end if;
			
			baudTick <= baud_tick AND baud8_tick;
			
		end process;



end Behavioral;

