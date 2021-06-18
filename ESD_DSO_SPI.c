/**********************************************
ESD_DSO_V2.c

ELEN90035 DSO Firmware v2
Bryce Widdicombe
The University of Melbourne
23/10/2019

Simple SPI Driver

***********************************************/

#include "ESD_DSO_SPI.h"

//Initialises SPI module
void SPIinit() {
	//Set SPI Pin Directions
	SET_MOSI_DIR(OUT);
	SET_MISO_DIR(IN);
	SET_SCK_DIR(OUT);
	SET_SS_DIR(OUT); //to prevent SPI changing to slave mode
	//Enable SPI, Set Master Mode, maximum speed
	SPCR = (1<<SPE)|(1<<MSTR);
	//Set SPI double speed mode
	SPSR = (1<<SPI2X);
}

//SPI main send and recieve function.
//Sends given byte through SPI,
// and returns byte that was received while sending.
byte SPIsendReceive(byte data) {
	//Load new byte to begin sending
	SPDR = data;
	//wait for send to complete
	while(!(SPSR & (1<<SPIF))) {
		//Do nothing while waiting
	}
	//return received byte
	return SPDR;
}
