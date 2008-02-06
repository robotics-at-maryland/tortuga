----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    13:54:02 10/22/2007 
-- Design Name: 
-- Module Name:    uartTxBuffer - Behavioral 
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

entity uartTxBuffer is

	Port( clk : in STD_LOGIC;
	
			TxD_data : out STD_LOGIC_VECTOR (7 downto 0);
			TxD_start : out STD_LOGIC;
			TxD_busy : in STD_LOGIC;
			
			data : in STD_LOGIC_VECTOR (7 downto 0);
			index : in STD_LOGIC_VECTOR (2 downto 0);
			latch : in STD_LOGIC;
			
			reset : in STD_LOGIC;
			start : in STD_LOGIC;
			use_checksum : in STD_LOGIC;
			data_length : in STD_LOGIC_VECTOR (2 downto 0);
			
			done : out STD_LOGIC;
			busy : out STD_LOGIC
			
	);
	
	type arrayBuffer is array (0 to 6) of STD_LOGIC_VECTOR (7 downto 0);
		
end uartTxBuffer;

architecture Behavioral of uartTxBuffer is

	-- data buffer
	signal TxBuffer : arrayBuffer;
	
	-- flags
	signal done_flag : STD_LOGIC;
	signal busy_flag : STD_LOGIC;
	
	signal state : STD_LOGIC_VECTOR (3 downto 0);
	
	-- checksum signals
	signal checksum : STD_LOGIC_VECTOR (7 downto 0);
	signal lookahead_mux : STD_LOGIC_VECTOR (7 downto 0);
	
begin




-- State engine

	P4: process(clk,start,reset,TxD_busy,state,use_checksum,data_length,lookahead_mux)
	begin

		
		if (rising_edge(clk)) then
			
			if (reset='1') then
				-- reset pulse
				state <= "0000";
			
			else
			
				case state is
					when "0000" =>
						if (start='1') then
							checksum <= TxBuffer(0);
							state <= "1000";
						end if;
						
					when "1000" =>
						if (TxD_busy='0') then
							-- Tx latched byte 0
							if (data_length="001") then
								-- finished sending all data bytes
								if (use_checksum='1') then
									-- go send checksum
									state <= "1111";
								else
									-- finished sending bytes
									state <= "0001";
								end if;
							else
								-- send next data byte
								checksum <= checksum + lookahead_mux;
								state <= "1001";
							end if;
						end if;
						
					when "1001" =>
						if (TxD_busy='0') then
							-- Tx latched byte 1
							if (data_length="010") then
								-- finished sending all data bytes
								if (use_checksum='1') then
									-- go send checksum
									state <= "1111";
								else
									-- finished sending bytes
									state <= "0001";
								end if;
							else
								-- send next data byte
								checksum <= checksum + lookahead_mux;
								state <= "1010";
							end if;
						end if;
						
					when "1010" =>
						if (TxD_busy='0') then
							-- Tx latched byte 2
							if (data_length="011") then
								-- finished sending all data bytes
								if (use_checksum='1') then
									-- go send checksum
									state <= "1111";
								else
									-- finished sending bytes
									state <= "0001";
								end if;
							else
								-- send next data byte
								checksum <= checksum + lookahead_mux;
								state <= "1011";
							end if;
						end if;
						
					when "1011" =>
						if (TxD_busy='0') then
							-- Tx latched byte 3
							if (data_length="100") then
								-- finished sending all data bytes
								if (use_checksum='1') then
									-- go send checksum
									state <= "1111";
								else
									-- finished sending bytes
									state <= "0001";
								end if;
							else
								-- send next data byte
								checksum <= checksum + lookahead_mux;
								state <= "1100";
							end if;
						end if;
						
					when "1100" =>
						if (TxD_busy='0') then
							-- Tx latched byte 4
							if (data_length="101") then
								-- finished sending all data bytes
								if (use_checksum='1') then
									-- go send checksum
									state <= "1111";
								else
									-- finished sending bytes
									state <= "0001";
								end if;
							else
								-- send next data byte
								checksum <= checksum + lookahead_mux;
								state <= "1101";
							end if;
						end if;
						
					when "1101" =>
						if (TxD_busy='0') then
							-- Tx latched byte 5
							if (data_length="110") then
								-- finished sending all data bytes
								if (use_checksum='1') then
									-- go send checksum
									state <= "1111";
								else
									-- finished sending bytes
									state <= "0001";
								end if;
							else
								-- send next data byte
								checksum <= checksum + lookahead_mux;
								state <= "1110";
							end if;
						end if;
						
					when "1110" =>
						if (TxD_busy='0') then
							-- Tx latched byte 6 (last data byte possible)
							if (use_checksum='1') then
								-- go send checksum
								state <= "1111";
							else
								-- finished sending bytes
								state <= "0001";
							end if;
						end if;
						
					when "1111" =>
						if (TxD_busy='0') then
							-- Tx latched checksum
							state <= "0001";
						end if;
						
					when "0001" =>
						-- this state generates the done pulse
						
					when others =>
						state <= "0000";
				end case;
			end if;
		end if;
		
		-- transmit flag, stops transmitting when packet_done is set.
		TxD_start <= state(3);
		
		-- flag logic
		if (state="0000") then
			busy_flag <= '0';
		else
			busy_flag <= '1';
		end if;
		
		if (state="0001") then
			done_flag <= '1';
		else
			done_flag <= '0';
		end if;
	
		-- flag output
		busy <= busy_flag;
		done <= done_flag;
	
	end process;


-- Checksum addition lookahead mux
	P3 : process(TxBuffer, state)
	begin
	
		case state (2 downto 0) is
			when "000" =>
				lookahead_mux <= TxBuffer(1);
			when "001" =>
				lookahead_mux <= TxBuffer(2);
			when "010" =>
				lookahead_mux <= TxBuffer(3);
			when "011" =>
				lookahead_mux <= TxBuffer(4);
			when "100" =>
				lookahead_mux <= TxBuffer(5);
			when "101" =>
				lookahead_mux <= TxBuffer(6);
			when others =>
				lookahead_mux <= "--------";
		end case;

	
	end process;


-- Data mux and buffer logic
	P2 : process(state,checksum,TxBuffer)
	begin
	
		-- select the data byte
		case state(2 downto 0) is
			when "000" =>
				TxD_data <= TxBuffer(0);
			when "001" =>
				TxD_data <= TxBuffer(1);
			when "010" =>
				TxD_data <= TxBuffer(2);
			when "011" =>
				TxD_data <= TxBuffer(3);
			when "100" =>
				TxD_data <= TxBuffer(4);
			when "101" =>
				TxD_data <= TxBuffer(5);
			when "110" =>
				TxD_data <= TxBuffer(6);
			when "111" =>
				TxD_data <= checksum;
			when others =>
				TxD_data <= "--------";
		end case;
		
	end process;


-- Data buffer logic
	P1 : process(clk,latch,index,data)
	begin
	
		if (rising_edge(clk)) then
			-- check if we need to latch data
			if (latch='1') then
				case index is
					when "000" =>
						TxBuffer(0) <= data;
					when "001" =>
						TxBuffer(1) <= data;
					when "010" =>
						TxBuffer(2) <= data;
					when "011" =>
						TxBuffer(3) <= data;
					when "100" =>
						TxBuffer(4) <= data;
					when "101" =>
						TxBuffer(5) <= data;
					when "110" =>
						TxBuffer(6) <= data;
					when others =>
						null;
				end case;
			end if;
		end if;
	
	end process;


end Behavioral;

