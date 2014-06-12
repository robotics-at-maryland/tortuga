----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:33:40 10/25/2007 
-- Design Name: 
-- Module Name:    uartTransceiver - Behavioral 
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

entity uartTransceiver is

	Port( clk : in STD_LOGIC;
	
			baudTick : in STD_LOGIC;
			baud8Tick : in STD_LOGIC;
			TxD : out STD_LOGIC;
			RxD : in STD_LOGIC;
			
			data_in : in STD_LOGIC_VECTOR (7 downto 0);
			data_out : out STD_LOGIC_VECTOR (7 downto 0);
			data_index : in STD_LOGIC_VECTOR (2 downto 0);
			buffer_index : in STD_LOGIC;
			latch : in STD_LOGIC;
			
			start : in STD_LOGIC;
			reset : in STD_LOGIC;
			busy : out STD_LOGIC;
			done : out STD_LOGIC;
			exit_code : out STD_LOGIC_VECTOR (3 downto 0);

			data_length : in STD_LOGIC_VECTOR (11 downto 0);
			use_checksum : in STD_LOGIC_VECTOR (3 downto 0);
			buffer_enable : in STD_LOGIC_VECTOR (3 downto 0);
			timeout_value : in STD_LOGIC_VECTOR (11 downto 0)

		);

end uartTransceiver;

architecture Behavioral of uartTransceiver is


	-- Uart Interface
	signal TxD_data : STD_LOGIC_VECTOR (7 downto 0);
	signal TxD_start : STD_LOGIC;
	signal TxD_busy : STD_LOGIC;
	signal RxD_data : STD_LOGIC_VECTOR (7 downto 0);
	signal RxD_data_ready : STD_LOGIC;
	
	
	-- Buffer Interfaces
	signal Tx0_start : STD_LOGIC;
	signal Tx0_done : STD_LOGIC;
	signal Tx0_latch : STD_LOGIC;
	signal Tx0_TxD_data : STD_LOGIC_VECTOR (7 downto 0);
	signal Tx0_TxD_start : STD_LOGIC;
	signal Tx0_busy : STD_LOGIC;
	
	signal Rx1_start : STD_LOGIC;
	signal Rx1_done : STD_LOGIC;
	signal Rx1_exit_code : STD_LOGIC_VECTOR (1 downto 0);
	signal Rx1_data : STD_LOGIC_VECTOR (7 downto 0);
	signal Rx1_busy : STD_LOGIC;
	
	signal Tx2_start : STD_LOGIC;
	signal Tx2_done : STD_LOGIC;
	signal Tx2_latch : STD_LOGIC;
	signal Tx2_TxD_data : STD_LOGIC_VECTOR (7 downto 0);
	signal Tx2_TxD_start : STD_LOGIC;
	signal Tx2_busy : STD_LOGIC;
	
	signal Rx3_start : STD_LOGIC;
	signal Rx3_done : STD_LOGIC;
	signal Rx3_exit_code : STD_LOGIC_VECTOR (1 downto 0);
	signal Rx3_data : STD_LOGIC_VECTOR (7 downto 0);
	signal Rx3_busy : STD_LOGIC;
	
	-- Interconnects
	signal bypass : STD_LOGIC_VECTOR (3 downto 0);

begin



-- Flag equations
	P2 : process( Rx1_exit_code, Rx3_exit_code, Tx0_busy, Rx1_busy, Tx2_busy, Rx3_busy )
	begin
		-- status byte is the exit codes
		exit_code <= Rx3_exit_code & Rx1_exit_code;
		
		-- any buffer is busy
		if (Tx0_busy='1' OR Rx1_busy='1' OR Tx2_busy='1' OR Rx3_busy='1') then
			busy <= '1';
		else
			busy <= '0';
		end if;
		
	end process;


-- Data buffer interface
	P1 : process( latch, Rx1_data, Rx3_data, buffer_index )
	begin
	
		-- buffer select
		if (buffer_index='0') then
			data_out <= Rx1_data;
			Tx0_latch <= latch;
			Tx2_latch <= '0';
		else
			data_out <= Rx3_data;
			Tx0_latch <= '0';
			Tx2_latch <= latch;
		end if;

	end process;


-- Start/done casacade
	P0 : process( Tx0_done, Rx1_done, Tx2_done, Rx3_done, buffer_enable, start )
	begin
	
	-- buffer 0
		if (start='1' AND buffer_enable(0)='1') then
			Tx0_start <= '1';
		else
			Tx0_start <= '0';
		end if;
		if ( (start='1' AND buffer_enable(0)='0') OR (Tx0_done='1') ) then
			bypass(0) <= '1';
		else
			bypass(0) <= '0';
		end if;
	
	-- buffer 1
		if (bypass(0)='1' AND buffer_enable(1)='1') then
			Rx1_start <= '1';
		else
			Rx1_start <= '0';
		end if;
		if ( (bypass(0)='1' AND buffer_enable(1)='0') OR (Rx1_done='1') ) then
			bypass(1) <= '1';
		else
			bypass(1) <= '0';
		end if;
	
	-- buffer 2
		if (bypass(1)='1' AND buffer_enable(2)='1') then
			Tx2_start <= '1';
		else
			Tx2_start <= '0';
		end if;
		if ( (bypass(1)='1' AND buffer_enable(2)='0') OR (Tx2_done='1') ) then
			bypass(2) <= '1';
		else
			bypass(2) <= '0';
		end if;
	
	-- buffer 3
		if (bypass(2)='1' AND buffer_enable(3)='1') then
			Rx3_start <= '1';
		else
			Rx3_start <= '0';
		end if;
		if ( (bypass(2)='1' AND buffer_enable(3)='0') OR (Rx3_done='1') ) then
			bypass(3) <= '1';
		else
			bypass(3) <= '0';
		end if;
		
		-- flag output
		done <= bypass(3);
	end process;


-- UART input mux
	P4 : process( Tx0_TxD_data, Tx0_TxD_start, Tx2_TxD_data, Tx2_TxD_start, Tx0_busy, Tx2_busy )
	begin
		-- connect buffer to uart as soon as it is activated, in priority order
		if (Tx0_busy='1') then
			TxD_data <= Tx0_TxD_data;
			TxD_start <= Tx0_TxD_start;
		elsif (Tx2_busy='1') then
			TxD_data <= Tx2_TxD_data;
			TxD_start <= Tx2_TxD_start;
		else
			TxD_data <= "--------";
			TxD_start <= '0';
		end if;
	end process;



-- UART Interface
	Tx0 : entity uartTransmitter
		port map( clk => clk, BaudTick => BaudTick, TxD => TxD,
					data => TxD_data, TxD_start => TxD_start, TxD_busy => TxD_busy );

	-- UART object
	Rx0: entity uartReceiver port map (clk => clk, RxD => RxD, baud8Tick => baud8Tick,
											RxD_data => RxD_data, RxD_data_ready => RxD_data_ready );


-- Buffer Entities
	TxBuffer0: entity uartTxBuffer
		port map( clk=>clk, reset=>reset, start=>Tx0_start, done=>Tx0_done,
			TxD_data=>Tx0_TxD_data, TxD_start=>Tx0_TxD_start, TxD_busy=>TxD_busy,
			use_checksum=>use_checksum(0), data_length=>(data_length(2 downto 0)),
			index=>data_index, latch=>Tx0_latch, data=>data_in, busy=>Tx0_busy );
			
	RxBuffer1: entity uartRxBuffer
		port map( clk=>clk, baudTick=>baudTick, reset=>reset, start=>Rx1_start,
			use_checksum=>use_checksum(1), data_length=>(data_length(5 downto 3)),
			timeout_value=>timeout_value, exit_code=>Rx1_exit_code, done=>Rx1_done,
			index=>data_index, data=>Rx1_data, busy=>Rx1_busy,
			RxD_data=>RxD_data, RxD_data_ready=>RxD_data_ready );
			
			
	TxBuffer2: entity uartTxBuffer
		port map( clk=>clk,reset=>reset,start=>Tx2_start,done=>Tx2_done,
			TxD_data=>Tx2_TxD_data,TxD_start=>Tx2_TxD_start,TxD_busy=>TxD_busy,
			use_checksum=>use_checksum(2),data_length=>(data_length(8 downto 6)),
			index=>data_index, latch=>Tx2_latch, data=> data_in, busy=>Tx2_busy );
	
	RxBuffer3: entity uartRxBuffer
		port map( clk=>clk, baudTick=>baudTick, reset=>reset, start=>Rx3_start,
			use_checksum=>use_checksum(3), data_length=>(data_length(11 downto 9)),
			timeout_value=>timeout_value, exit_code=>Rx3_exit_code, done=>Rx3_done,
			index=>data_index, data=>Rx3_data, busy=>Rx3_busy,
			RxD_data=>RxD_data, RxD_data_ready=>RxD_data_ready );
	
			
			

end Behavioral;

