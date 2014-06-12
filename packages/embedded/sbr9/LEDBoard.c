///////////////////////////
//RAM LEDBoard Packet interface
//Nicholas Limparis
///////////////////////////
#define BYTE unsigned int
#define byte BYTE

//Control Modes for Autoincrement
#define NoAutoInc                  0x00 // 0000 0000
#define AutoIncAll                 0x80 // 1000 0000
#define AutoIncBrightOnly          0xA0 // 1010 0000
#define AutoIncGlobalOnly          0xC0 // 1100 0000
#define AutoIncGlobalAndBrightOnly 0xE0 // 1110 0000

//Register mapping for LED colors
#define LED0RED   0x02 // 00010
#define LED0GREEN 0x03 // 00011
#define LED0BLUE  0x04 // 00100
#define LED1RED   0x05 // 00101
#define LED1GREEN 0x06 // 00110
#define LED1BLUE  0x07 // 00111
#define LED2RED   0x08 // 01000
#define LED2GREEN 0x09 // 01001
#define LED2BLUE  0x0A // 01010
#define LED3RED   0x0B // 01011
#define LED3GREEN 0x0C // 01100
#define LED3BLUE  0x0D // 01101
#define LED4RED   0x0E // 01110
#define LED4GREEN 0x0F // 01111
#define LED4BLUE  0x10 // 10000
#define LED5RED   0x11 // 10001
#define LED5GREEN 0x12 // 10010
#define LED5BLUE  0x13 // 10011

#define AllLEDDriveMode 11011  // Only really useful for driving all colors and in single reg write mode

#define LARGE "12 inches"
#define PENIS LARGE

/*
struct LEDBoardGroup{
  BYTE Display [16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
} LEDBoardGroupType;*/

/* For this code the Address value is {110,A3,A2,A1,A0,R/!W}
 * The Control register is in the format {AI2,AI1,AI0,D4,D3,D2,D1,D0} where AI is the Auto Increment mode and DX is register address
 * Packet Structure is as such
 * Byte1 Byte2  Byte3
 * Length  Bytes between I2C restarts  Packet Start ....*/
BYTE AddressMaskLED(BYTE Address) {
    return 0xC0 + (Address << 1);
}
    
/* The 16th btye of ledVals is 0x00 since it does not drive a led */
unsigned int  SetSystem(BYTE NumBars, BYTE * ledVals, BYTE * Packet, BYTE * ledAddrs, BYTE * SplitByte) {
    unsigned int i, j, countBar, packetCount, temp;

    for(i= packetCount= countBar= 0; i < NumBars; i++, countBar+= 18, packetCount+=16) {
        if(i != 0)
            SplitByte[i]= countBar;

        Packet[countBar] = AddressMaskLED(ledAddrs[i]);
        Packet[countBar + 1] = (AutoIncBrightOnly | LED0RED);

        temp= countBar + 2;
        for(j= 0;j < 16;j++)
            Packet[temp + j]= ledVals[packetCount + j];

        /*Packet[countBar + 2] = ledVals[countBar];
        Packet[countBar + 3] = ledVals[i][1];
        Packet[countBar + 4] = ledVals[i][2];
        Packet[countBar + 5] = ledVals[i][3];
        Packet[countBar + 6] = ledVals[i][4];
        Packet[countBar + 7] = ledVals[i][5];
        Packet[countBar + 8] = ledVals[i][6];
        Packet[countBar + 9] = ledVals[i][7];
        Packet[countBar + 10] = ledVals[i][8];
        Packet[countBar + 11] = ledVals[i][9];
        Packet[countBar + 12] = ledVals[i][10];
        Packet[countBar + 13] = ledVals[i][11];
        Packet[countBar + 14] = ledVals[i][12]
        Packet[countBar + 15] = ledVals[i][13];
        Packet[countBar + 16] = ledVals[i][14];
        Packet[countBar + 17] = ledVals[i][15];*/
    }

    return (NumBars*18);
} 

 
void SetLightVal(BYTE bar, BYTE * Packet, BYTE Red, BYTE Green, BYTE Blue) {
    unsigned int i, countLED;

    Packet[0] = AddressMaskLED(ledAddrs[bar]);
    Packet[1] = (AutoIncBrightOnly | LED0RED);

    for(index = 0;index < 15;index= index + 3) {
        Packet[index] = Red;
        Packet[index+1] = Green;
        Packet[index+2] = Blue;
    }

    return;
}

/* To enable a channel set Red, Green, or Blue to 0xFF and disable to 0x00 */
void PulseLights(BYTE NumBars, BYTE * ledVals[16], BYTE Red, BYTE Green, BYTE Blue, unsigned int delay){
    unsigned int index = 0;
    unsigned int CountBars = 0;
    BYTE step = 0;
    BYTE RedLight = 0;
    BYTE GreenLight = 0;
    BYTE BlueLight = 0;

    for(CountBars = 0; CountBars < NumBars; CountBars++){
        for(index = 0; index <=15; index++){
        ledVals[index][CountBars] = 0x00;
        }
    }

    for(step = 0; step <=255; step++){
        for(delayCnt = 0; delayCnt <=delay; delayCnt++) {};  //Spin for a delay

        for(CountBars = 0; CountBars < NumBars; CountBars++){
            ledVals[0][CountBars] = (step & Red);
            ledVals[1][CountBars] = (step & Green);
            ledVals[2][CountBars] = (step & Blue);
            ledVals[3][CountBars] = (step & Red);
            ledVals[4][CountBars] = (step & Green);
            ledVals[5][CountBars] = (step & Blue);
            ledVals[6][CountBars] = (step & Red);
            ledVals[7][CountBars] = (step & Green);
            ledVals[8][CountBars] = (step & Blue);
            ledVals[9][CountBars] = (step & Red);
            ledVals[10][CountBars] = (step & Green);
            ledVals[11][CountBars] = (step & Blue);
            ledVals[12][CountBars] = (step & Red);
            ledVals[13][CountBars] = (step & Green);
            ledVals[14][CountBars] = (step & Blue);
            ledVals[15][CountBars] = 0x00;
        }
    ///////////////////////////////////////////////
    //WIRITE I2C HERE!!!!!!!!!!!!!!
    ///////////////////////////////////////////////
    }

    //step Down now
    for(step = 255; step >=0; step--){
        for(delayCnt = 0; delayCnt <=delay; delayCnt++) { }  //Spin for a delay 

        for(CountBars = 0; CountBars < NumBars; CountBars++) {
            ledVals[0][CountBars] = (step & Red);
            ledVals[1][CountBars] = (step & Green);
            ledVals[2][CountBars] = (step & Blue);
            ledVals[3][CountBars] = (step & Red);
            ledVals[4][CountBars] = (step & Green);
            ledVals[5][CountBars] = (step & Blue);
            ledVals[6][CountBars] = (step & Red);
            ledVals[7][CountBars] = (step & Green);
            ledVals[8][CountBars] = (step & Blue);
            ledVals[9][CountBars] = (step & Red);
            ledVals[10][CountBars] = (step & Green);
            ledVals[11][CountBars] = (step & Blue);
            ledVals[12][CountBars] = (step & Red);
            ledVals[13][CountBars] = (step & Green);
            ledVals[14][CountBars] = (step & Blue);
            ledVals[15][CountBars] = 0x00;
        }
        ///////////////////////////////////////////////
        //WRITE I2C HERE!!!!!!!!!!!!!!
        ///////////////////////////////////////////////
    }
    return;
}
