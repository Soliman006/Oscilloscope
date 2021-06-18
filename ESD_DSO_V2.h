/**********************************************
ESD_DSO_V2.c

ELEN90035 DSO Firmware v2
Bryce Widdicombe
The University of Melbourne
23/10/2019

Based on code originally by: M J Adams, 2014

Main header file.
Pin definitions go here.

***********************************************/

#include "Macros.h"

//10MHz oscillator
#define FOSC 10000000
#define F_CPU 10000000
//UART speed settings
#define BAUDRATE 38400
#define BAUDRATE_FAST 153600
#define MYUBRR FOSC/16/BAUDRATE-1
#define MYUBRR_FAST FOSC/16/BAUDRATE_FAST-1

//ADC_D Pins.
//If you split ADC_D among multiple ports,
//you need to modify readSampleFromRAM() in ESD_DSO_V2.c
#define SET_ADC_D_DDR(DIR) DDRA=DIR
#define GET_ADC_D PINA

//RAM_READ pin
#define SET_RAM_READ(STATE) SET(PORTD,_BV(PD7),STATE)
#define SET_RAM_READ_DDR(DIR) SET(DDRD,_BV(PD7),DIR)

//FORCE pin
#define SET_FORCE(STATE) SET(PORTC,_BV(PC0),STATE)
#define SET_FORCE_DDR(DIR) SET(DDRC,_BV(PC0),DIR)

//ENABLE pin
#define SET_ENABLE(STATE) SET(PORTC,_BV(PC1),STATE)
#define SET_ENABLE_DDR(DIR) SET(DDRC,_BV(PC1),DIR)

//EDGE_SW pin
#define SET_EDGE_SW(STATE) SET(PORTD,_BV(PD6),STATE)
#define SET_EDGE_SW_DDR(DIR) SET(DDRD,_BV(PD6),DIR)

//DAC Chip Select Pin
#define SET_DAC_CS(STATE) SET(PORTB,_BV(PB4),STATE)
#define SET_DAC_CS_DIR(DIR) SET(DDRB,_BV(PB4),DIR)

//PLL S0 Pin
#define SET_PLL_S0(STATE) SET(PORTD,_BV(PD3),STATE)
#define SET_PLL_S0_DDR(DIR) SET(DDRD,_BV(PD3),DIR)

//PLL S1 Pin
#define SET_PLL_S1(STATE) SET(PORTD,_BV(PD4),STATE)
#define SET_PLL_S1_DDR(DIR) SET(DDRD,_BV(PD4),DIR)

//IN_CLK Pin
//IN_CLK can only be PD5, PD4, PD7 or PB3
//Default is PD5, You MUST change setupInCLK() in ESD_DSO_V2.c if you change this
#define SET_IN_CLK(STATE) SET(PORTD,_BV(PD5),STATE)
#define SET_IN_CLK_DDR(DIR) SET(DDRD,_BV(PD5),DIR)

//SLOW_CLK pin
#define SET_SLOW_CLK(STATE) SET(PORTC,_BV(PC6),STATE)
#define SET_SLOW_CLK_DDR(DIR) SET(DDRC,_BV(PC6),DIR)

//FILL_COMP Pin
#define SET_FILL_COMP(STATE) SET(PORTC,_BV(PC7),STATE)
#define SET_FILL_COMP_DDR(DIR) SET(DDRC,_BV(PC7),DIR)
#define GET_FILL_COMP GET(PINC,_BV(PC7))	

//Convenience Macros for PLL S0 and S1 lines
//These should not need to be changed
#define SET_PLL_S1_HIGH SET_PLL_S1(HIGH); SET_PLL_S1_DDR(OUT)
#define SET_PLL_S1_LOW SET_PLL_S1(LOW); SET_PLL_S1_DDR(OUT)
#define SET_PLL_S1_TRI SET_PLL_S1(LOW); SET_PLL_S1_DDR(IN)
#define SET_PLL_S0_HIGH SET_PLL_S0(HIGH); SET_PLL_S0_DDR(OUT)
#define SET_PLL_S0_LOW SET_PLL_S0(LOW); SET_PLL_S0_DDR(OUT)
#define SET_PLL_S0_TRI SET_PLL_S0(LOW); SET_PLL_S0_DDR(IN)
