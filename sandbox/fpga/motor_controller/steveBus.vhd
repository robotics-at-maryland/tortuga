----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:28:43 10/27/2007 
-- Design Name: 
-- Module Name:    steveBus - Behavioral 
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

--Interface use:
--	when data_write_rq pulses high, read a byte from data_in.
--	when data_read_rq pulses high, it means the data on data_out has
--		been latched onto the bus, and you can shift the next byte onto
--		data_out.





entity steveBus is
    Port ( clk : in  STD_LOGIC;
	 
           IN_REQ : inout  STD_LOGIC := '1';
           ACK : inout  STD_LOGIC;
           RW : in  STD_LOGIC;
           DataBus : inout  STD_LOGIC_VECTOR (7 downto 0);
			  
			  reset : in STD_LOGIC;
			  interrupt : in STD_LOGIC;
           data_write_rq : out  STD_LOGIC;
           data_read_rq : out  STD_LOGIC;
			  data_out : in STD_LOGIC_VECTOR (7 downto 0);
			  data_in : out STD_LOGIC_VECTOR (7 downto 0)
			  
			  );
			  
end steveBus;

architecture Behavioral of steveBus is

	signal state : STD_LOGIC_VECTOR (1 downto 0);
	signal data_out_latch : STD_LOGIC_VECTOR (7 downto 0);
	
	signal master_rq : STD_LOGIC;


begin

	P1 : process( clk )
	begin
	
		data_in <= DataBus;
		
		master_rq <= IN_REQ;
	
	
		if (rising_edge(clk)) then
		
			if (reset='1') then
				-- synchronous reset signal
				data_write_rq <= '0';
				data_read_rq <= '0';
				state <= "00";
				DataBus <= "ZZZZZZZZ";
				ACK <= 'Z';
				IN_REQ <= 'Z';
				data_out_latch <= "--------";
			else
			
				case state is
					when "00" =>
						ACK <= 'Z';
						IN_REQ <= 'Z';
						if (master_rq='1' AND RW='1') then
							-- master write request
							data_write_rq <= '1';
							data_read_rq <= '0';
							state <= "01";
							DataBus <= "ZZZZZZZZ";
						elsif (master_rq='1' AND RW='0') then
							-- master read request
							data_write_rq <= '0';
							data_read_rq <= '1';
							state <= "10";
							data_out_latch <= data_out;
							DataBus <= data_out_latch;
						elsif (interrupt='1') then
							-- give interrupt pulse
							data_write_rq <= '0';
							data_read_rq <= '0';
							state <= "11";
							DataBus <= "ZZZZZZZZ";
						else
							-- idle
							data_write_rq <= '0';
							data_read_rq <= '0';
							state <= "00";
							DataBus <= "ZZZZZZZZ";
						end if;
						
					when "01" =>
						-- give ack pulse after reading byte
						data_write_rq <= '0';
						data_read_rq <= '0';
						DataBus <= "ZZZZZZZZ";
						ACK <= '1';
						if (master_rq='0') then
							-- received ack, end of transaction
							state <= "00";
						else
							-- waiting for ack
							state <= "01";
						end if;
						
					when "10" =>
						-- give ack pulse after putting data on bus
						data_write_rq <= '0';
						data_read_rq <= '0';
						ACK <= '1';
						DataBus <= data_out_latch;
						if (master_rq='0') then
							-- received ack, end of transaction
							state <= "00";
						else
							-- waiting for ack
							state <= "10";
						end if;
						
					when "11" =>
						-- giving interrupt pulse
						IN_REQ <= '1';
						data_write_rq <= '0';
						data_read_rq <= '0';
						state <= "00";
						DataBus <= "ZZZZZZZZ";
						ACK <= 'Z';
						
					when others =>
						null;				
				end case;
				
			end if;
		end if;
		
	end process;



end Behavioral;

