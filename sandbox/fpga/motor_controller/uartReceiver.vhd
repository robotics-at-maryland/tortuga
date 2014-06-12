----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:11:46 10/21/2007 
-- Design Name: 
-- Module Name:    uartReceiver - Behavioral 
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

entity uartReceiver is
    Port ( clk : in  STD_LOGIC;
           RxD : in  STD_LOGIC;
           RxD_data_ready : out  STD_LOGIC;
			  RxD_idle : out STD_LOGIC;
			  RxD_data_error : out STD_LOGIC;
           RxD_data : out  STD_LOGIC_VECTOR (7 downto 0);
           baud8Tick : in  STD_LOGIC);
end uartReceiver;

architecture Behavioral of uartReceiver is
	signal RxD_sync_inv : STD_LOGIC_VECTOR (1 downto 0);
	signal RxD_cnt_inv : STD_LOGIC_VECTOR (1 downto 0);
	signal RxD_bit_inv : STD_LOGIC;
	signal state : STD_LOGIC_VECTOR (3 downto 0);
	signal bit_spacing : STD_LOGIC_VECTOR (3 downto 0);
	signal next_bit : STD_LOGIC;
	signal RxD_data_buf : STD_LOGIC_VECTOR (7 downto 0);
	signal state_is_one : STD_LOGIC;

begin

-- Input sampling
	P10 : process(clk,baud8Tick,RxD)
	begin
		if (rising_edge(clk)) then
			if (Baud8Tick='1') then
				if (RxD='1') then
					RxD_sync_inv <= RxD_sync_inv(0) & "1";
				else
					RxD_sync_inv <= RxD_sync_inv(0) & "0";
				end if;
			end if;
		end if;
	end process;

-- Synchronization
	P0 : process(clk,baud8Tick)
	begin
		if (rising_edge(clk)) then
			if (Baud8Tick='1') then
				if( (RxD_sync_inv(1)='1') AND NOT ( RxD_cnt_inv = "11" ) ) then
					RxD_cnt_inv <= RxD_cnt_inv + 1;
				elsif( (RxD_sync_inv(1)='0') and NOT (RxD_cnt_inv = "00") ) then
					RxD_cnt_inv <= RxD_cnt_inv - 1;
				end if;
				
				if ( RxD_cnt_inv = "00" ) then
					RxD_bit_inv <= '0';
				elsif ( RxD_cnt_inv = "11" ) then
					RxD_bit_inv <= '1';
				end if;
			end if;
		end if;
	end process;
	
	
	P1 : process(bit_spacing)
	begin
		-- "next_bit" controls when the data sampling occurs
		-- depending on how noisy the RxD is, different values might work better
		-- with a clean connection, values from 8 to 11 work
		if (bit_spacing = "10") then
			next_bit <= '1';
		else
			next_bit <= '0';
		end if;
	end process;
	
	
	-- Catch starting sync edge
	P2 : process(clk, state, Baud8Tick, bit_spacing)
	begin
		if ( rising_edge(clk) ) then
			if (state="0000") then
				bit_spacing <= "0000";
			elsif (Baud8Tick='1') then
				bit_spacing <= (("0" & bit_spacing(2 downto 0)) + 1) OR (bit_spacing(3) & "000");
			end if;
		end if;
	end process;
	
	
	-- Bit advancement state machine
	P3 : process(clk,state)
	begin
		if (rising_edge(clk)) then
			if (Baud8Tick='1') then
				case state is
					when "0000" =>
						if (RxD_bit_inv='1') then
							state <= "1000";  -- start bit found?
						end if;
					when "1000" =>
						if (next_bit='1') then
							state <= "1001";  -- bit 0
						end if;
					when "1001" =>
						if (next_bit='1') then
							state <= "1010";  -- bit 1
						end if;
					when "1010" =>
						if (next_bit='1') then
							state <= "1011";  -- bit 2
						end if;
					when "1011" =>
						if (next_bit='1') then
							state <= "1100";  -- bit 3
						end if;
					when "1100" =>
						if (next_bit='1') then
							state <= "1101";  -- bit 4
						end if;
					when "1101" =>
						if (next_bit='1') then
							state <= "1110";  -- bit 5
						end if;
					when "1110" =>
						if (next_bit='1') then
							state <= "1111";  -- bit 6
						end if;
					when "1111" =>
						if (next_bit='1') then
							state <= "0001";  -- bit 7
						end if;
					when "0001" =>
						if (next_bit='1') then
							state <= "0000";  -- stop bit
						end if;
					when others =>
						state <= "0000";
				end case;
			end if;
		end if;	
	end process;
	
	
	-- Shift data in on appropriate clock edges
	P4 : process(clk,Baud8Tick,next_bit,state,RxD_bit_inv,RxD_data_buf)
	begin
		if (rising_edge(clk)) then
			if (Baud8Tick='1' AND next_bit='1' AND state(3)='1') then
				RxD_data_buf <= (NOT RxD_bit_inv) & RxD_data_buf(7 downto 1);
			end if;
		end if;
		
		RxD_data <= RxD_data_buf;
		
	end process;
	
	-- Output flags
	P5 : process(clk,state,Baud8Tick,next_bit,state_is_one,RxD_bit_inv)
	begin
		-- idle when state is 0
		if ( state="0000" ) then
			RxD_idle <= '1';
		else
			RxD_idle <= '0';
		end if;
		
		-- pulse RX ready at end of byte
		-- we can assume that it will be read immediately
		if ( state="0001" ) then
			state_is_one <= '1';
		else
			state_is_one <= '0';
		end if;
		
		if (rising_edge(clk)) then
			RxD_data_ready <= (Baud8Tick AND next_bit AND state_is_one AND NOT RxD_bit_inv);  -- ready only if the stop bit is received
			RxD_data_error <= (Baud8Tick AND next_bit AND state_is_one AND RxD_bit_inv);  -- error if the stop bit is not received
		end if;
	end process;
	
	


end Behavioral;

