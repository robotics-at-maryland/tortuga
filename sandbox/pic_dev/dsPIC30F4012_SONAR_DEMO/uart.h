/*header file for uart.c  
 *Scott Watson April 20, 2008
 *include this file whenever using uart.c functions in another file
 */

void init_Uart(byte enabled);
void sendByte(byte i);
void sendString( char * s);
void sendNum(signed int i);
