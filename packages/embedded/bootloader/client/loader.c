#include "dspicprg.h"
#include "serial.h"

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */

// static char *IC_names[]= ["Master","LCD Controller","Temperature","Depth","Distribution","Balancer","Sonar"];

static unsigned char handshake() {
	unsigned int i, j;
	//writeByte(0x55);
	//hcsleep(100);
	for (i=0; i<30; i++) {
		writeByte(0x55);
		j = readByte();
		if (j == 0x55 || j == 0xFF)
			break;
		hcsleep(5);
	}

	if (j != 0x55)
		return 0;

	writeByte(0x55);
	readByte();

	hcsleep(50);

	writeByte(0x03);

	if (readByte() == 1 && readByte() == 1 && readByte() == 0x55)
		return 1;

	return 0; //Failure
}

static unsigned char writeProgram() {
	struct MemoryRow *MR;
	
	for (MR=MRList; MR; MR=MR->Next) {
		unsigned char crc = 0;
		int i, c;
		switch( MR->Type )
		{ 
		case CODE_MEM:
			printf("Writing code memory to address %08X...\n",MR->Address);
			break;
			
		case DATA_MEM:
			continue;
			printf("Writing data memory to address %08X...\n",MR->Address);
			break;
			
		case CNFG_MEM:
			continue;
			printf("Writing cnfg memory at address %08X...\n",MR->Address);
			break;			
		};

		writeByte(0x02);
		writeByte(MR->Address >> 16);
		crc += 0xFF & (MR->Address >> 16);
		writeByte(MR->Address >> 8);
		crc += 0xFF & (MR->Address >> 8);
		writeByte(MR->Address);
		crc += 0xFF & (MR->Address);

		writeByte(0x81);
		crc += 0x81;
		
		if (MR->Address == 0) {
			MR->Buffer[0] = 0x00;
			MR->Buffer[1] = 0x7D;
			MR->Buffer[2] = 0x04;
			MR->Buffer[3] = 0x00;
			MR->Buffer[4] = 0x01;
			MR->Buffer[5] = 0x00;
		}

		for (i=0; i<128; i++) {
			writeByte(MR->Buffer[i]);
			crc += MR->Buffer[i];
			/*if ((i % 8) == 0)
				printf("\n  ");
				printf("%02X ", MR->Buffer[i]);*/
		}
		//puts("\n");

		writeByte(0xFF - crc + 1);

		c = 0;
		i = readByte();
		while (i == -1 && (c++) < 100 ) {
			hcsleep(10);
			i = readByte();
		}

		if (i != 0x55)
			return 0; //Failure
	}

	return 1;
}

int main(int argc, char* argv[]) {
	printf("Tortuga Embedded Firmware Uploader\n\n");

	if (argc != 4) {
		printf("Usage: %s ICx (/dev/ttyUSBx|COMx) firmware.hex\n", argv[0]);
		return 1;
	} else {
		WhichChip = &SupportedChips[6];

		printf("Reading firmware file...\n");

		HexFile = fopen(argv[2], "r");
		ReadHexFile();

		if (initSerial(argv[1]) != 0)
			return 1;

		printf("Connecting to IC1...\n");
		if (handshake()) {
			printf("Writing program...\n");
			if (writeProgram()) {
				printf("Running program...\n");
				writeByte(0x0F);
			} else {
				puts("Failed uploading program\n");
			}
		} else {
			printf("Unable to connect to HandyCAM.\n");
		}

		printf("Done.\n");

		return 0;
	}
}
