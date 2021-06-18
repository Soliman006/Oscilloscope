/**********************************************
ESD_DSO_V2.c

ELEN90035 DSO Firmware v2
Bryce Widdicombe
The University of Melbourne
23/10/2019

Simple SPI Driver Header File

***********************************************/

#include "Macros.h"

#define SET_MOSI_DIR(DIR) SET(DDRB,_BV(PB5),DIR)
#define SET_MISO_DIR(DIR) SET(DDRB,_BV(PB6),DIR)
#define SET_SCK_DIR(DIR) SET(DDRB,_BV(PB7),DIR)
#define SET_SS_DIR(DIR) SET(DDRB,_BV(PB4),DIR)

//Function Prototypes
void SPIinit();
byte SPIsendReceive(byte data);
