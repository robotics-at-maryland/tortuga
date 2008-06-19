#define SRAM_BASE 0x20300000
#define SRAM_SIZE 0x100000

#define ADDR_LED         0x202E0000
#define ADDR_LOOPBACK0   0x202E0010
#define ADDR_LOOPBACK1   0x202E0012
#define ADDR_LOOPBACK2   0x202E0014
#define ADDR_LOOPBACK3   0x202E0016


/* 1A0 1A1 1B0 1B1  2A0 2A1 2B0 2B1 */
#define ADDR_ADC0        0x202F0000
#define ADDR_ADC1        0x202F0002
#define ADDR_ADC2        0x202F0004
#define ADDR_ADC3        0x202F0006
#define ADDR_ADC4        0x202F0008
#define ADDR_ADC5        0x202F000A
#define ADDR_ADC6        0x202F000C
#define ADDR_ADC7        0x202F000E

#define ADDR_ADCONFIG      0x202F0100
#define ADDR_ADPRESCALER   0x202F0102

#define ADDR_SAMPLECOUNT1  0x202F0104
#define ADDR_SAMPLECOUNT2  0x202F0106

#define ADDR_BITCOUNTER1   0x202F0108
#define ADDR_BITCOUNTER2   0x202F010A

#define REG(a) (*(volatile unsigned short *) a)


#define REG_LED         (*(volatile unsigned short *) ADDR_LED)

#define REG_LOOPBACK0   (*(volatile unsigned short *) ADDR_LOOPBACK0)
#define REG_LOOPBACK1   (*(volatile unsigned short *) ADDR_LOOPBACK1)
#define REG_LOOPBACK2   (*(volatile unsigned short *) ADDR_LOOPBACK2)
#define REG_LOOPBACK3   (*(volatile unsigned short *) ADDR_LOOPBACK3)

#define REG_ADC0        (*(volatile signed short *) ADDR_ADC0)
#define REG_ADC1        (*(volatile signed short *) ADDR_ADC1)
#define REG_ADC2        (*(volatile signed short *) ADDR_ADC2)
#define REG_ADC3        (*(volatile signed short *) ADDR_ADC3)
#define REG_ADC4        (*(volatile signed short *) ADDR_ADC4)
#define REG_ADC5        (*(volatile signed short *) ADDR_ADC5)
#define REG_ADC6        (*(volatile signed short *) ADDR_ADC6)
#define REG_ADC7        (*(volatile signed short *) ADDR_ADC7)

#define REG_ADCONFIG    (*(volatile unsigned short *) ADDR_ADCONFIG)
#define REG_ADPRESCALER (*(volatile unsigned short *) ADDR_ADPRESCALER)


#define REG_SAMPLECOUNT1 (*(volatile unsigned short *) ADDR_SAMPLECOUNT1)
#define REG_SAMPLECOUNT2 (*(volatile unsigned short *) ADDR_SAMPLECOUNT2)

#define REG_BITCOUNTER1  (*(volatile unsigned short *) ADDR_BITCOUNTER1)
#define REG_BITCOUNTER2  (*(volatile unsigned short *) ADDR_BITCOUNTER2)


int initADC();
