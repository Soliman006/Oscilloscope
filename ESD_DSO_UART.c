/**********************************************
ESD_DSO_V2.c

ELEN90035 DSO Firmware v2
Bryce Widdicombe
The University of Melbourne
23/10/2019

Simple UART Driver

***********************************************/

#include "ESD_DSO_UART.h"
#include <string.h>

//Initialises UART Transmit and Receive modes using MYUBRR
void UARTinit(unsigned int myubrr) {
	//disable UART if enabled
	UCSRB = (0<<RXEN)|(0<<TXEN);
	//Set UART baud rate
	UARTsetUBRR(myubrr);
	//Enable UART Transmit and Receive
	UCSRB = (1<<RXEN)|(1<<TXEN);
}

//Sets UART baud rate registers
void UARTsetUBRR(unsigned int myubrr) {
	UBRRH = (unsigned char)(myubrr>>8);
	UBRRL = (unsigned char)myubrr;
}

//Sends a single character (byte) through UART
void UARTsendChar(byte data) {
	//Wait until any pending transfers are completed
	while (!(UCSRA & (1<<UDRE))) {
		//wait
	}
	//load new data to be sent.
	//UART module will automatically begin sending
	UDR = data;
}

//Waits for a new character to be received and returns it
byte UARTwaitAndGetChar() {
	//wait for a new character
	while (UARTcheckForNew() == FALSE) {
		//do nothing
	}
	return UDR;
}

//Immediately return a new character if one has been recieved,
//return 0x00 if no character received
//(Does not wait)
byte UARTgetChar() {
	if (UARTcheckForNew() == FALSE) {
		//no new character
		return 0x00;
	} else {
		//we have a new character
		return UDR;
	}
}

//Returns TRUE if a character is ready to be read.
//FALSE if no new character has been received.
byte UARTcheckForNew() {
	if (!(UCSRA & (1<<RXC))) {
		//No New character
		return FALSE;
	} else {
		return TRUE;
	}
}

//Sends a string through UART
void UARTsendStr(char *str) {
	byte i;
	for(i=0;i<strlen(str);i++) {
		UARTsendChar(str[i]);
	}
}
