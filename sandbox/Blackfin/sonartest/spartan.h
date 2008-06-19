#define SRAM_BASE 0x20300000
#define SRAM_SIZE 0x100000


// Bus addresses
#define ADDR_ADC0           0x202F0000  //0x202F 0000
#define ADDR_ADC1           0x202F0004
#define ADDR_ADC2           0x202F0008
#define ADDR_ADC3           0x202F000C
#define ADDR_ADC4           0x202F0010
#define ADDR_ADC5           0x202F0014
#define ADDR_ADC6           0x202F0018
#define ADDR_ADC7           0x202F001C

#define ADDR_ADConfig1          0x202F0100
#define ADDR_ADConfig2          0x202F0104

#define ADDR_SampleCount1       0x202F0110
#define ADDR_SampleCount2       0x202F0114
#define ADDR_SampleCount1Swap   0x202F0118
#define ADDR_SampleCount2Swap   0x202F011C
#define ADDR_SampleCount1Latch  0x202F0120
#define ADDR_SampleCount2Latch  0x202F0124

#define ADDR_BitCounter1        0x202F0130
#define ADDR_BitCounter2        0x202F0134

#define ADDR_FIFO_OUT1A         0x202F0020
#define ADDR_FIFO_OUT1B         0x202F0024
#define ADDR_FIFO_OUT2A         0x202F0028
#define ADDR_FIFO_OUT2B         0x202F002C
#define ADDR_FIFO_OUT1S         0x202F0128
#define ADDR_FIFO_OUT2S         0x202F012C

#define ADDR_FIFO_COUNT1A       0x202F00A0
#define ADDR_FIFO_COUNT1B       0x202F00A4
#define ADDR_FIFO_COUNT2A       0x202F00A8
#define ADDR_FIFO_COUNT2B       0x202F00AC
#define ADDR_FIFO_COUNT1S       0x202F01A8
#define ADDR_FIFO_COUNT2S       0x202F01AC

#define ADDR_FIFO_EMPTY1A       0x202F00B0
#define ADDR_FIFO_EMPTY1B       0x202F00B4
#define ADDR_FIFO_EMPTY2A       0x202F00B8
#define ADDR_FIFO_EMPTY2B       0x202F00BC
#define ADDR_FIFO_EMPTY1S       0x202F01B8
#define ADDR_FIFO_EMPTY2S       0x202F01BC

#define ADDR_FIFO_HALF1A        0x202F00C0
#define ADDR_FIFO_HALF1B        0x202F00C4
#define ADDR_FIFO_HALF2A        0x202F00C8
#define ADDR_FIFO_HALF2B        0x202F00CC
#define ADDR_FIFO_HALF1S        0x202F01C8
#define ADDR_FIFO_HALF2S        0x202F01CC

#define ADDR_FIFO_STATUS1       0x202F0108
#define ADDR_FIFO_STATUS2       0x202F010C

#define ADDR_ADStep             0x202F0130





#define ADDR_LED                0x202E0000
#define ADDR_Testreg0           0x202E0010
#define ADDR_Testreg1           0x202E0014
#define ADDR_Testreg2           0x202E0018
#define ADDR_Testreg3           0x202E001C
#define ADDR_Loopback0          0x202E0020
#define ADDR_Loopback1          0x202E0024
#define ADDR_Loopback2          0x202E0028
#define ADDR_Loopback3          0x202E002C




#define REG(a) (*(volatile unsigned short *) a)


int initADC();
