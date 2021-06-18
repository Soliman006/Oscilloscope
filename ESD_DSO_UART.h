/**********************************************
ESD_DSO_V2.c

ELEN90035 DSO Firmware v2
Bryce Widdicombe
The University of Melbourne
23/10/2019

Simple UART Driver Header File

***********************************************/

#include "Macros.h"

//Functions
void UARTinit(unsigned int myubrr);
void UARTsetUBRR(unsigned int myubrr);
void UARTsendChar(byte data);
byte UARTwaitAndGetChar();
byte UARTgetChar();
byte UARTcheckForNew();
void UARTsendStr(char *str);
