----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:37:09 10/26/2007 
-- Design Name: 
-- Module Name:    uartTopLevel - Behavioral 
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

entity uartTopLevel is
    Port( clk : in  STD_LOGIC;
			 reset : in STD_LOGIC;
			
			 -- UART IO
			 TxD : out STD_LOGIC_VECTOR (5 downto 0);
			 RxD : in STD_LOGIC_VECTOR (5 downto 0);
			 
			 -- Bus IO
			 IN_REQ : inout  STD_LOGIC;
          ACK : inout  STD_LOGIC;
          RW : in  STD_LOGIC;
          DataBus : inout  STD_LOGIC_VECTOR (7 downto 0)
			 
	 );
	 
	 type byteArray is array (0 to 5) of STD_LOGIC_VECTOR (7 downto 0);
	 type nibbleArray is array (0 to 5) of STD_LOGIC_VECTOR (3 downto 0);
	
end uartTopLevel;

architecture Behavioral of uartTopLevel is

	-- protocol state machine
	signal state : STD_LOGIC_VECTOR (3 downto 0);
	
	signal task_done : STD_LOGIC;
	
	-- bus interface
	signal bus_write_rq : STD_LOGIC;
	signal bus_read_rq : STD_LOGIC;
	signal bus_data_in : STD_LOGIC_VECTOR (7 downto 0);
	signal bus_data_out : STD_LOGIC_VECTOR (7 downto 0);
	signal bus_interrupt : STD_LOGIC;
	
	-- baud generator interface
	signal baudTick : STD_LOGIC;
	signal baud8Tick : STD_LOGIC;
	
	
	-- buffer setting latches
	signal port_enable : STD_LOGIC_VECTOR (5 downto 0);
	signal buffer_enable : STD_LOGIC_VECTOR (3 downto 0);
	signal use_checksum : STD_LOGIC_VECTOR (3 downto 0);
	signal timeout_value : STD_LOGIC_VECTOR (11 downto 0);
	signal data_length : STD_LOGIC_VECTOR (11 downto 0);
	
	
	signal exit_code_mux : STD_LOGIC_VECTOR (7 downto 0);
	signal data_out_mux : STD_LOGIC_VECTOR (7 downto 0);
	signal data_in_mux : STD_LOGIC_VECTOR (7 downto 0);
	signal data_latch_mux : STD_LOGIC;
	
	signal port_index : STD_LOGIC_VECTOR (2 downto 0);
	signal buffer_index : STD_LOGIC;	
	signal data_index : STD_LOGIC_VECTOR (2 downto 0);
	
	signal buf_data_in : byteArray;
	signal buf_data_out : byteArray;
	signal buf_exit_code : nibbleArray;
	signal buf_latch : STD_LOGIC_VECTOR (5 downto 0);
	signal buf_start : STD_LOGIC_VECTOR (5 downto 0);
	signal buf_done : STD_LOGIC_VECTOR (5 downto 0);
	signal buf_busy : STD_LOGIC_VECTOR (5 downto 0);
	
	

begin



	P0 : process( clk, state, bus_write_rq, bus_read_rq, bus_data_in, port_index )
	begin
	
						
	
		if (rising_edge(clk)) then
			case state is
			
				when "0000" =>
					-- Idle state
					-- reset counters
					port_index <= "000";
					
					-- wait in incoming command byte
					if (bus_write_rq='1') then
						-- read the byte off the buffer and go to the appropriate state
						
						if (bus_data_in(7)='0') then
							-- Port Start bit
							-- store port masks
							port_enable <= bus_data_in (5 downto 0);
							-- start the selected buffers
							buf_start <= bus_data_in (5 downto 0);
							state <= "0001";
						
						else
							case (bus_data_in(6 downto 4)) is
								when "000" =>
									-- buffer enables
									buffer_enable <= bus_data_in (3 downto 0);
								
								when "001" =>
									-- checksum enables
									use_checksum <= bus_data_in (3 downto 0);
								
								when "010" =>
									-- buffer length MSB
									data_length(11 downto 8) <= bus_data_in (3 downto 0);
									state <= "0010";
								
								when "011" =>
									-- timeout value MSB
									timeout_value(11 downto 8) <= bus_data_in (3 downto 0);
									state <= "0011";
								
								when "100" =>
									-- Begin received data for TX buffers
									buffer_index <= bus_data_in (3);
									data_index <= bus_data_in (2 downto 0);
									state <= "0100";
								
								when "101" =>
									-- Begin transmitting data from RX buffers
									buffer_index <= bus_data_in (3);
									data_index <= bus_data_in (2 downto 0);
									state <= "0101";
								
								when "110" =>
									-- Begin transmitting status from RX buffers
									state <= "0110";
								
								when others =>
									null;
					
							end case;
							
						end if;
					
					end if;
					
					
				when "0001" =>
					-- end of start pulse
					buf_start <= "000000";
					state <= "0000";
				
				when "0010" =>
					-- Waiting for LSB of data_length
					if (bus_write_rq='1') then
						-- read timeout LSB
						data_length(7 downto 0) <= bus_data_in;
						state <= "0000";
					end if;
				
				when "0011" =>
					-- Waiting for LSB of timeout
					if (bus_write_rq='1') then
						-- read timeout LSB
						timeout_value(7 downto 0) <= bus_data_in;
						state <= "0000";
					end if;
					
				when "0100" =>
					-- Waiting to receive data buffer byte
					if (bus_write_rq='1') then
						-- read data byte
						data_latch_mux <= '1';
						state <= "0111";
					end if;
				
				when "0111" =>
					-- end of latch pulse for data byte
					data_latch_mux <= '0';
					if (port_index="101") then
						state <= "0000";
					else
						state <= "0100";
						port_index <= port_index + 1;
					end if;
				
				when "0101" =>
					-- Going to transmit
					-- data is on bus when in this state
					if (bus_read_rq='1') then
						-- this byte has been read
						-- go to the next one
						if (port_index="101") then
							-- last port sent
							state <= "0000";
						else
							port_index <= port_index+1;
						end if;
					end if;
					
				when "0110" =>
					-- Sending all six status bytes
					-- data is on bus when in this state
					if (bus_read_rq='1') then
						-- this byte has been read
						-- go to the next one
						if (port_index="101") then
							-- last port sent
							state <= "0000";
						else
							port_index <= port_index+1;
						end if;
					end if;
				
				when others =>
					state <= "0000";
							
			end case;
		end if;
		
	
	
	end process;
	
	
	DoneMux : process( buf_done, buf_busy, port_enable, task_done )
	begin
	
		-- Give global done pulse if:
		--   Any buffer done pulse is present  AND 
		--   All enabled buffers are not busy
	
		if (NOT (buf_done="000000")) AND ((buf_busy AND port_enable)="000000") then
			task_done <= '1';
		else
			task_done <= '0';
		end if;
		
		bus_interrupt <= task_done;
	
	end process;



	PortMux: process( port_index, state, data_out_mux, exit_code_mux, buf_exit_code, buf_data_out, data_latch_mux )
	begin
	
		-- Latch data to bus on certain states
		case state is
			when "0101" =>
				bus_data_out <= data_out_mux;
			when "0110" =>
				bus_data_out <= exit_code_mux;
			when others =>
				bus_data_out <= "--------";
		end case;
		
	
		-- bus data always connected to data input lines, only latch is switched
			
		case port_index is
			when "000" =>
				exit_code_mux <= "0000" & buf_exit_code(0);
				data_out_mux <= buf_data_out(0);
				buf_latch(0) <= data_latch_mux;
				buf_latch(1) <= '0';
				buf_latch(2) <= '0';
				buf_latch(3) <= '0';
				buf_latch(4) <= '0';
				buf_latch(5) <= '0';
				
			when "001" =>
				exit_code_mux <= "0000" & buf_exit_code(1);
				data_out_mux <= buf_data_out(1);
				buf_latch(0) <= '0';
				buf_latch(1) <= data_latch_mux;
				buf_latch(2) <= '0';
				buf_latch(3) <= '0';
				buf_latch(4) <= '0';
				buf_latch(5) <= '0';
			
			when "010" =>
				exit_code_mux <= "0000" & buf_exit_code(2);
				data_out_mux <= buf_data_out(2);
				buf_latch(0) <= '0';
				buf_latch(1) <= '0';
				buf_latch(2) <= data_latch_mux;
				buf_latch(3) <= '0';
				buf_latch(4) <= '0';
				buf_latch(5) <= '0';
			
			when "011" =>
				exit_code_mux <= "0000" & buf_exit_code(3);
				data_out_mux <= buf_data_out(3);
				buf_latch(0) <= '0';
				buf_latch(1) <= '0';
				buf_latch(2) <= '0';
				buf_latch(3) <= data_latch_mux;
				buf_latch(4) <= '0';
				buf_latch(5) <= '0';
			when "100" =>
				exit_code_mux <= "0000" & buf_exit_code(4);
				data_out_mux <= buf_data_out(4);
				buf_latch(0) <= '0';
				buf_latch(1) <= '0';
				buf_latch(2) <= '0';
				buf_latch(3) <= '0';
				buf_latch(4) <= data_latch_mux;
				buf_latch(5) <= '0';
			when "101" =>
				exit_code_mux <= "0000" & buf_exit_code(5);
				data_out_mux <= buf_data_out(5);
				buf_latch(0) <= '0';
				buf_latch(1) <= '0';
				buf_latch(2) <= '0';
				buf_latch(3) <= '0';
				buf_latch(4) <= '0';
				buf_latch(5) <= data_latch_mux;
			when others =>
				exit_code_mux <= "--------";
				data_out_mux <= "--------";
				buf_latch(0) <= '0';
				buf_latch(1) <= '0';
				buf_latch(2) <= '0';
				buf_latch(3) <= '0';
				buf_latch(4) <= '0';
				buf_latch(5) <= '0';
		end case;
		
	end process;



	
-- Baud Generator
	Baud0: entity uartBaudGen port map( clk=>clk, baudTick=>baudTick, baud8Tick=>baud8Tick );
	
-- Bus Interface
	Bus0: entity steveBus port map( clk=>clk, reset=>reset, interrupt=>bus_interrupt,
		data_write_rq=>bus_write_rq, data_read_rq=>bus_read_rq,
		data_out=>bus_data_out, data_in=>bus_data_in,
		IN_REQ=>IN_REQ, ACK=>ACK, RW=>RW, DataBus=>DataBus );
	
-- Transceiver Units
			  
	Buffer0: entity uartTransceiver
		Port map ( clk=>clk,
	
			baudTick=>baudTick,
			baud8Tick=>baud8Tick,
			TxD=>TxD(0),
			RxD=>RxD(0),
			
			data_in=>bus_data_in,
			data_out=>buf_data_out(0),
			data_index=>data_index,
			buffer_index=>buffer_index,
			latch=>buf_latch(0),
			
			start=>buf_start(0),
			reset=>reset,
			done=>buf_done(0),
			exit_code=>buf_exit_code(0),
			busy=>buf_busy(0),
			
			data_length=>data_length,
			use_checksum=>use_checksum,
			buffer_enable=>buffer_enable,
			timeout_value=>timeout_value
			
			);
	
	Buffer1: entity uartTransceiver
		Port map ( clk=>clk,
	
			baudTick=>baudTick,
			baud8Tick=>baud8Tick,
			TxD=>TxD(1),
			RxD=>RxD(1),
			
			data_in=>bus_data_in,
			data_out=>buf_data_out(1),
			data_index=>data_index,
			buffer_index=>buffer_index,
			latch=>buf_latch(1),
			
			start=>buf_start(1),
			reset=>reset,
			done=>buf_done(1),
			exit_code=>buf_exit_code(1),
			busy=>buf_busy(1),

			data_length=>data_length,
			use_checksum=>use_checksum,
			buffer_enable=>buffer_enable,
			timeout_value=>timeout_value
			
			);
	
	Buffer2: entity uartTransceiver
		Port map ( clk=>clk,
	
			baudTick=>baudTick,
			baud8Tick=>baud8Tick,
			TxD=>TxD(2),
			RxD=>RxD(2),
			
			data_in=>bus_data_in,
			data_out=>buf_data_out(2),
			data_index=>data_index,
			buffer_index=>buffer_index,
			latch=>buf_latch(2),
			
			start=>buf_start(2),
			reset=>reset,
			done=>buf_done(2),
			exit_code=>buf_exit_code(2),
			busy=>buf_busy(2),

			data_length=>data_length,
			use_checksum=>use_checksum,
			buffer_enable=>buffer_enable,
			timeout_value=>timeout_value
			
			);
	
	Buffer3: entity uartTransceiver
		Port map ( clk=>clk,
	
			baudTick=>baudTick,
			baud8Tick=>baud8Tick,
			TxD=>TxD(3),
			RxD=>RxD(3),
			
			data_in=>bus_data_in,
			data_out=>buf_data_out(3),
			data_index=>data_index,
			buffer_index=>buffer_index,
			latch=>buf_latch(3),
			
			start=>buf_start(3),
			reset=>reset,
			done=>buf_done(3),
			exit_code=>buf_exit_code(3),
			busy=>buf_busy(3),

			data_length=>data_length,
			use_checksum=>use_checksum,
			buffer_enable=>buffer_enable,
			timeout_value=>timeout_value
			
			);
	
	Buffer4: entity uartTransceiver
		Port map ( clk=>clk,
	
			baudTick=>baudTick,
			baud8Tick=>baud8Tick,
			TxD=>TxD(4),
			RxD=>RxD(4),
			
			data_in=>bus_data_in,
			data_out=>buf_data_out(4),
			data_index=>data_index,
			buffer_index=>buffer_index,
			latch=>buf_latch(4),
			
			start=>buf_start(4),
			reset=>reset,
			done=>buf_done(4),
			exit_code=>buf_exit_code(4),
			busy=>buf_busy(4),

			data_length=>data_length,
			use_checksum=>use_checksum,
			buffer_enable=>buffer_enable,
			timeout_value=>timeout_value
			
			);
	
	Buffer5: entity uartTransceiver
		Port map ( clk=>clk,
	
			baudTick=>baudTick,
			baud8Tick=>baud8Tick,
			TxD=>TxD(5),
			RxD=>RxD(5),
			
			data_in=>bus_data_in,
			data_out=>buf_data_out(5),
			data_index=>data_index,
			buffer_index=>buffer_index,
			latch=>buf_latch(5),
			
			start=>buf_start(5),
			reset=>reset,
			done=>buf_done(5),
			exit_code=>buf_exit_code(5),
			busy=>buf_busy(5),

			data_length=>data_length,
			use_checksum=>use_checksum,
			buffer_enable=>buffer_enable,
			timeout_value=>timeout_value
			
			);
	
	



end Behavioral;

