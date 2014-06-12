----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    18:25:27 10/21/2007 
-- Design Name: 
-- Module Name:    uartTransmitter - Behavioral 
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

entity uartTransmitter is
    Port ( clk : in  STD_LOGIC;
			  TxD : out STD_LOGIC;
           baudTick : in  STD_LOGIC;
			  TxD_start : in STD_LOGIC;
           TxD_busy : out  STD_LOGIC;
           data : in  STD_LOGIC_VECTOR (7 downto 0) );
end uartTransmitter;


architecture Behavioral of uartTransmitter is
	signal state : STD_LOGIC_VECTOR (3 downto 0);
	signal busy : STD_LOGIC;
	signal muxBit : STD_LOGIC;
	signal outBit : STD_LOGIC;
	signal dataLatch : STD_LOGIC_VECTOR (7 downto 0);

begin

	P0 : process(clk,busy,baudTick,state,outBit,dataLatch)
	begin
	
	-- Bit advancement state machine
		if (rising_edge(clk)) then
			case state is
				when "0000" =>
					if( TxD_start = '1' ) then 
						state <= "0001";
						dataLatch <= data;
					end if;
				when "0001" =>
					if( baudTick = '1' ) then
						state <= "0100";
					end if;
				when "0100" =>
					if( baudTick = '1' ) then
						state <= "1000";
					end if;
				when "1000" =>
					if( baudTick = '1' ) then
						state <= "1001";
					end if;
				when "1001" =>
					if( baudTick = '1' ) then
						state <= "1010";
					end if;
				when "1010" =>
					if( baudTick = '1' ) then
						state <= "1011";
					end if;
				when "1011" =>
					if( baudTick = '1' ) then
						state <= "1100";
					end if;
				when "1100" =>
					if( baudTick = '1' ) then
						state <= "1101";
					end if;
				when "1101" =>
					if( baudTick = '1' ) then
						state <= "1110";
					end if;
				when "1110" =>
					if( baudTick = '1' ) then
						state <= "1111";
					end if;
				when "1111" =>
					if( baudTick = '1' ) then
						state <= "0010";
					end if;
				when "0010" =>
					if( baudTick = '1' ) then
						state <= "0011";
					end if;
				when "0011" =>
					if( baudTick = '1' ) then
						state <= "0000";
					end if;
				when others =>
					if( baudTick = '1' ) then
						state <= "0000";
					end if;
			end case;
			
		end if;
		
		
		-- Bit selector mux
		case state(2 downto 0) is
			when "000" =>
				muxBit <= dataLatch(0);
			when "001" =>
				muxBit <= dataLatch(1);
			when "010" =>
				muxBit <= dataLatch(2);
			when "011" =>
				muxBit <= dataLatch(3);
			when "100" =>
				muxBit <= dataLatch(4);
			when "101" =>
				muxBit <= dataLatch(5);
			when "110" =>
				muxBit <= dataLatch(6);
			when "111" =>
				muxBit <= dataLatch(7);
			when others =>
				muxBit <= '-';
		end case;
		
		
		-- Output bit logic
		if (rising_edge(clk)) then
			if (state(3 downto 2) = "01") then 
				-- start bit
				outBit <= '0';
			elsif (state(3) = '1') then
				-- enable output bit for data
				outBit <= muxBit;
			else
				-- idle bit state
				outBit <= '1';
			end if;
		end if;
	
		-- Tie output line to bit register
		TxD <= outBit;
	
		-- Idle flag
		if (state="0000") then
			busy <= '0';
		else
			busy <= '1';
		end if;
		
		-- Assign to output
		TxD_busy <= busy;
	
	end process;


end Behavioral;

