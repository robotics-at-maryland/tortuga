----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    13:53:50 10/22/2007 
-- Design Name: 
-- Module Name:    uartRxBuffer - Behavioral 
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

entity uartRxBuffer is

	Port( clk : in STD_LOGIC;
	
			BaudTick : in STD_LOGIC;
	
			RxD_data : in STD_LOGIC_VECTOR (7 downto 0);
			RxD_data_ready : in STD_LOGIC;
	
			reset : in STD_LOGIC;
			start : in STD_LOGiC;
			use_checksum : in STD_LOGIC;
			data_length : in STD_LOGIC_VECTOR (2 downto 0);
			timeout_value : in STD_LOGIC_VECTOR (11 downto 0);
			
			index : in STD_LOGIC_VECTOR (2 downto 0);
			data : out STD_LOGIC_VECTOR (7 downto 0);
			
			done : out STD_LOGIC;
			exit_code : out STD_LOGIC_VECTOR (1 downto 0);
			busy : out STD_LOGIC
			
	);
	
	type arrayBuffer is array (0 to 4) of STD_LOGIC_VECTOR (7 downto 0);

end uartRxBuffer;

architecture Behavioral of uartRxBuffer is

	signal timeout_counter : STD_LOGIC_VECTOR (11 downto 0);

	-- data buffer
	signal RxBuffer : arrayBuffer;
	signal RxB_count : STD_LOGIC_VECTOR (2 downto 0);
	
	-- flag buffers
	signal done_flag : STD_LOGIC;
	signal busy_flag : STD_LOGIC;
	signal timeout_flag : STD_LOGIC;
	signal response_good_flag : STD_LOGIC;
	signal packet_full_flag : STD_LOGIC;
	
	
	-- checksum
	signal checksum : STD_LOGIC_VECTOR (7 downto 0);
	
begin




-- State machine
	PRX3: process(clk,busy_flag,reset,start,timeout_flag,packet_full_flag,done_flag,response_good_flag)
	begin
	
		if (rising_edge(clk)) then
		
			if (reset='1') then
				busy_flag <= '0';
				done_flag <= '0';
			
			elsif (busy_flag='0') then
				-- check start input
				if (start='1') then
					-- start receive state
					busy_flag <= '1';
				end if;
				
				-- clear done pulse output
				done_flag <= '0';
				
			else
			
				if (timeout_flag='1' OR packet_full_flag='1') then
					-- done, either by data or by by timeout or by overflow
					-- status bits will display outcome
					done_flag <= '1';
					busy_flag <= '0';
				end if;
				
			end if;
		
		end if;
		
		-- flag outputs
		busy <= busy_flag;
		done <= done_flag;
		
		-- exit code output
		if (timeout_flag='1') then
			exit_code <= "01";
		elsif (packet_full_flag='1') then
			if (response_good_flag='0') then
				exit_code <= "10";
			else
				exit_code <= "11";
			end if;
		else
			exit_code <= "00";
		end if;

	
	end process;
	
	
	
-- UART interface
	PRX5: process(clk, start, checksum, busy_flag, reset, RxD_data_ready, RxD_data, RxB_count, data_length)
	begin
	
		if( rising_edge(clk) ) then
		
			-- receive byte processor and state flags
			if (start='1' AND busy_flag='0') OR (reset='1') then
				-- reset byte count on start
				RxB_count <= "000";
				checksum <= "00000000";
				response_good_flag <= '0';
			
			elsif( RxD_data_ready='1' AND busy_flag='1' ) then
				-- receive this byte
				if (RxB_count=data_length-1 AND use_checksum='1') then
					-- receive checksum
					if (RxD_data=checksum) then
						response_good_flag <= '1';
					end if;
				else
					case RxB_count is
						when "000" =>
							if (RxD_data="00000110" AND use_checksum='0') then
								response_good_flag <= '1';
							end if;
							RxBuffer(0) <= RxD_data;
						when "001" =>
							RxBuffer(1) <= RxD_data;
						when "010" =>
							RxBuffer(2) <= RxD_data;
						when "011" =>
							RxBuffer(3) <= RxD_data;
						when "100" =>
							RxBuffer(4) <= RxD_data;
						when others =>
							null;
					end case;
				
					-- add byte to checksum
					checksum <= checksum + RxD_data;
				end if;
				
				-- increment counter
				RxB_count <= RxB_count + 1;
			end if;
		end if;
		
		-- packet full flag
		if( RxB_count = data_length ) then
			-- received all the bytes we were supposed to
			packet_full_flag <= '1';
		else
			packet_full_flag <= '0';
		end if;
		
	end process;
	
	
	
-- Timeout counter
	PRX2: process(clk,start,busy_flag,timeout_counter,BaudTick,timeout_value)
	begin
		if (rising_edge(clk)) then
			-- reset on start from idle
			if ((start='1' AND busy_flag='0') OR reset='1') then
				timeout_counter <= "000000000000";
			elsif (BaudTick='1' AND busy_flag='1') then
				-- increment timeout counter every baud time
				timeout_counter <= timeout_counter + 1;
			end if;
		end if;
		
		-- if timeout is reached, set the flag until counter is reset
		-- timeout value of 0x0000 disables timeout
		if ((timeout_counter = timeout_value) AND NOT (timeout_value="000000000000")) then
			timeout_flag <= '1';
		else
			timeout_flag <= '0';
		end if;
		
	end process;

-- Output data mux
	PRX0: process(RxBuffer,index)
	begin
		-- mux 5-byte data buffer to output
		case index is
			when "000" =>
				data <= RxBuffer(0);
			when "001" =>
				data <= RxBuffer(1);
			when "010" =>
				data <= RxBuffer(2);
			when "011" =>
				data <= RxBuffer(3);
			when "100" =>
				data <= RxBuffer(4);
			when others =>
				data <= "00000000";
		end case;
	end process;


end Behavioral;

