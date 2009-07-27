///////////////////////////
//RAM LEDBoard Packet interface
//Nicholas Limparis
///////////////////////////
#define BYTE unsigned int
#define byte BYTE



//Control Modes for Autoincrement
#define NoAutoInc                  000
#define AutoIncAll                 100
#define AutoIncBrightOnly          101
#define AutoIncGlobalOnly          110
#define AutoIncGlobalAndBrightOnly 111



//Register mapping for LED colors
#define LED0 00010
#define LED0 00011
#define LED0 00100
#define LED1 00101
#define LED1 00110
#define LED1 00111
#define LED2 01000
#define LED2 01001
#define LED2 01010
#define LED3 01011
#define LED3 01100
#define LED3 01101
#define LED4 01110
#define LED4 01111
#define LED4 10000
#define LED5 10001
#define LED5 10010
#define LED5 10011

#define AllLEDDriveMode 11011  //Only really useful for driving all colors and in single reg write mode

//For this code the Address value is {110,A3,A2,A1,A0,R/!W}
//The Control register is in the format {AI2,AI1,AI0,D4,D3,D2,D1,D0} where AI is the Auto Increment mode and DX is register address
//Packet Structure is as such 
//Byte1   Byte2                       Byte3
//Lenght  Bytes between I2C restarts  Packet Start ....
BYTE AddressMaskLED ( BYTE Address)
{
  return 0xC0 + (Address << 1);
}
    
//The 16th btye of ledVals is 0x00 since it does not drive an led
unsigned int  SetSystem(BYTE NumBars, BYTE * ledVals [16], BYTE * Packet, BYTE * ledAddrs, BYTE * SplitByte, BYTE numSplits)
  Packet[0] = (0x60 | Address) << 1;

  Packet[3] = AddressMaskLED(Address); //(1101 1110)&(
{NoAutoInc,11111}&{111




//The Packet to write to all LED drivers is 
