///////////////////////////
//RAM LEDBoard Packet interface
//Nicholas Limparis
///////////////////////////
#define BYTE unsigned int
#define byte BYTE



//Control Modes for Autoincrement
#define NoAutoInc                  0x00 //00000000
#define AutoIncAll                 0x80 //10000000
#define AutoIncBrightOnly          0xA0 //10100000
#define AutoIncGlobalOnly          0xC0 //11000000
#define AutoIncGlobalAndBrightOnly 0xE0 //11100000



//Register mapping for LED colors
#define LED0 0x02//00010
#define LED0 0x03//00011
#define LED0 0x04//00100
#define LED1 0x05//00101
#define LED1 0x06//00110
#define LED1 0x07//00111
#define LED2 0x08//01000
#define LED2 0x09//01001
#define LED2 0x0A//01010
#define LED3 0x0B//01011
#define LED3 0x0C//01100
#define LED3 0x0D//01101
#define LED4 0x0E//01110
#define LED4 0x0F//01111
#define LED4 0x10//10000
#define LED5 0x11//10001
#define LED5 0x12//10010
#define LED5 0x13//10011

#define AllLEDDriveMode 11011  //Only really useful for driving all colors and in single reg write mode
/*
struct LEDBoardGroup{
  BYTE Display [16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
} LEDBoardGroupType;*/

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
unsigned int  SetSystem(BYTE NumBars, BYTE * ledVals [16], BYTE * Packet, BYTE * ledAddrs, BYTE * SplitByte, BYTE numSplits, LEDBoardGroupType * LEDgroups[16])
  unsigned int count, temp;
  count = 0;
  countBar = 0;

  for( countBar = 0; countBar < NumBars; countBar++){
    Packet[countBar] = AddressMaskLED(ledAddrs[countBar]);
    Packet[countBar + 1] = (AutoIncBrightOnly | 
    Packet[countBar + 2] = AddressMaskLED(Address); //(1101 1110)&(




//The Packet to write to all LED drivers is 
