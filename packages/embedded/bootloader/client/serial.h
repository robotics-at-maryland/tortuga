#ifndef __SERIAL_H_
#define __SERIAL_H_

extern int initSerial(char* loc);
extern void closeSerial(void);
extern int readByte();
extern void writeByte(char b);

extern void hcsleep(unsigned long tme);

#endif
