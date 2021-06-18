/**********************************************
ESD_DSO_V2.c

ELEN90035 DSO Firmware v2 main
Bryce Widdicombe
The University of Melbourne
23/10/2019

Based on code originally by: M J Adams, 2014

The following code provides full functionality
for the basic ESD DSO.
You may modify the code to add advanced
features.

***********************************************/

#include <avr/io.h>
#include <string.h>
#include <stdint.h>
#include "Macros.h"
#include "ESD_DSO_V2.h"
#include "ESD_DSO_SPI.h"
#include "ESD_DSO_UART.h"
//import delay.h after ESD_DSO_V2.h so that F_CPU is defined
#include <util/delay.h>

//Constants
#define DSO_VERSION_STR "DSO Version 2.0\n"
#define DAC_DEFAULT 127
#define PLL_DEFAULT 2
#define MAX_SAMPLES 0xFFFF	//65535
#define CMD_STOP 'n'
#define CMD_CAP 'C'
#define CMD_FORCE 'f'
#define CMD_SEND 's'
#define CMD_PLL 'r'
#define CMD_VER 'v'
#define CMD_DAC 't'
#define CMD_EDGE 'i'
#define CMD_EDGE_RISING 'r'
#define CMD_EDGE_FALLING 'f'
//additional commands
#define CMD_BAUD 'B'	//toggles between 38400 and 153600 baud uart
byte fastUART = FALSE;

//Function Prototypes
void setDacValue(byte value);
void setPLLMultiplier(byte multi);
void captureSamples(byte force);
void sendSamples();
byte readSampleFromRAM();
void setupInCLK();
void toggleUART();

//Main Function
int main() {
	//Initial Setup
	//Set input/output pins and initial values
	SET_ADC_D_DDR(IN);
	SET_RAM_READ_DDR(OUT);
	SET_RAM_READ(OFF);
	SET_ENABLE_DDR(OUT);
	SET_ENABLE(OFF);
	SET_FORCE_DDR(OUT);
	SET_FORCE(OFF);
	SET_EDGE_SW_DDR(OUT);
	SET_EDGE_SW(OFF);
	SET_DAC_CS_DIR(OUT);
	SET_DAC_CS(ON);
	SET_PLL_S0_DDR(OUT);
	SET_PLL_S1_DDR(OUT);
	SET_IN_CLK_DDR(OUT);
	SET_SLOW_CLK_DDR(OUT);
	SET_SLOW_CLK(OFF);
	SET_FILL_COMP_DDR(IN);
	//Setup IN_CLK and PLL
	setupInCLK();
	setPLLMultiplier(PLL_DEFAULT);
	//Setup UART
	UARTinit(MYUBRR);
	//Setup SPI and DAC
	SPIinit();
	setDacValue(DAC_DEFAULT);

	//Loop
	byte command = 0x00;
	while(TRUE) {
		//wait for and get new command from computer
		command = UARTwaitAndGetChar();

		//process command
		if (command == CMD_VER) {
			//Send DSO version to PC
			UARTsendStr(DSO_VERSION_STR);

		} else if (command == CMD_PLL) {
			//Need to change PLL multiplyer
			//Read in new multiplier and update
			byte newMulti = UARTwaitAndGetChar();
			setPLLMultiplier(newMulti);

		} else if (command == CMD_DAC) {
			//read in new DAC value and update DAC
			byte newVal = UARTwaitAndGetChar();
			setDacValue(newVal);

		} else if (command == CMD_CAP) {
			//capture samples normally using trigger
			captureSamples(FALSE);

		} else if (command == CMD_FORCE) {
			//capture samples and use force to override trigger
			captureSamples(TRUE);

		} else if (command == CMD_SEND) {
			//send samples to PC
			sendSamples();

		} else if (command == CMD_EDGE) {
			//Changes between rising and falling edge trigger
			//read in new edge mode and update
			byte newEdge = UARTwaitAndGetChar();
			//set EDGE_SW accordingly
			if (newEdge == CMD_EDGE_RISING) {
				SET_EDGE_SW(OFF);
			} else if (newEdge == CMD_EDGE_FALLING) {
				SET_EDGE_SW(ON);
			}
			//any other setting is invalid for basic DSO
		} else if (command == CMD_BAUD) {
			//toggle UART baud
			toggleUART();
		}
	}
}


/*
DSO Sample Capture Function.
Sets Control signals, captures 65535 samples 
and sends CMD_CAP to PC to signify capture completed.
Can be cancelled by sending CMD_STOP.
Force can be TRUE/FALSE to use force control line.
*/
void captureSamples(byte force) {
	SET_RAM_READ(OFF);
	//Generate rising edge on Enable to start capture
	SET_ENABLE(ON);
	SET_ENABLE(OFF);
	if(force==TRUE) {
		//Generate rising edge on Force to override trigger
		SET_FORCE(ON);
		SET_FORCE(OFF);
	}
	//wait until capture finished or cancelled
	//add slight (10us) delay to ensure CPLD has started sampling
	_delay_us(10);
	while(UARTgetChar()!=CMD_STOP && !GET_FILL_COMP) {
		//CPLD is now controlling capture.
		//MCU does nothing until capture is completed
		//or computer sends STOP command
	}
	//capture finished, send CMD_CAP so computer knows
	UARTsendChar(CMD_CAP);
}

/*
DSO Sample Send Function
Sends samples to computer until all have been sent,
or CMD_STOP is received
*/
void sendSamples() {
	uint16_t sampleCount = 0;
	byte tempSample = 0x00;
	SET_RAM_READ(ON);
	while(UARTgetChar()!=CMD_STOP && sampleCount<MAX_SAMPLES) {
		//generate SLOW_CLK rinsing edge to set up sample in RAM
		SET_SLOW_CLK(ON);
		SET_SLOW_CLK(OFF);
		//Read sample from RAM
		tempSample = readSampleFromRAM();
		//send sample to PC
		UARTsendChar(tempSample);
		//increment counter
		sampleCount++;
	}
	SET_RAM_READ(OFF);
}

/*
Function to read in a sample from RAM.
If your ADC_D is not connected to PA0-PA7 you can add custom logic here.
*/
byte readSampleFromRAM() {
	//For ADC_D connected to PA0-7
	//othrwise comment this and use the below example as a template
	return GET_ADC_D;
	/* Example for arbirtary data bus.
	adc_d is connected like PA0, PB2, PC1, PC0, PA3, PB4, PB1, PD1
	byte temp1 = (PINA&_BV(PA7))>>0;
	byte temp2 = (PINA&_BV(PA6))>>1;
	byte temp3 = (PINA&_BV(PA5))<<1;
	byte temp4 = (PINA&_BV(PA4))<<3;
	byte temp5 = (PINA&_BV(PA3))<<1;
	byte temp6 = (PINA&_BV(PA2))<<1;
	byte temp7 = (PINA&_BV(PA1))<<5;
	byte temp8 = (PINA&_BV(PA0))<<6;
	byte sample = temp1|temp2|temp3|temp4|temp5|temp6|temp7|temp8;
	return sample;
	*/
}

//Function to set PLL Multiplier
void setPLLMultiplier(byte multi) {
	//refer to PLL datsheet for S0, S1 settings
	if (multi == 2) {
		SET_PLL_S0_TRI;
		SET_PLL_S1_TRI;
	} else if (multi == 4) {
		SET_PLL_S0_LOW;
		SET_PLL_S1_LOW;
	} else if (multi == 8) {
		SET_PLL_S0_HIGH;
		SET_PLL_S1_HIGH;
	}
	//any other value is invalid for basic DSO setup
}

//function to setup internal timer to generate 5MHz IN_CLK.
//Need to modify if IN_CLK comes from something other than PD5
//Refer to ATMEGA16 datasheet for timer registers
void setupInCLK() {
	OCR1A = 0x00;
	SET(TCCR1A,_BV(COM1A0),ON); 
	SET(TCCR1B,(_BV(WGM12)|_BV(CS10)),ON);
}

//Sends new value to DAC using SPI
//Refer to DAC data sheet for command structure
void setDacValue(byte value) {
	//Datasheet: CMD, Unbuffered, Vref*1, Active
	uint16_t dacCmd = 0x3000;
	//scale value to 16 bits
	uint16_t tempVal = value;
	//align value with lowest 12 bits
	tempVal = tempVal<<4;
	//merge value and base command
	dacCmd = dacCmd|tempVal;
	//split 16bit command into 2 8bit bytes
	byte dacByte1 = (byte)(dacCmd>>8);
	byte dacByte2 = (byte)(dacCmd&0x00FF);
	//send two command bytes to DAC
	SET_DAC_CS(LOW);
	SPIsendReceive(dacByte1);
	SPIsendReceive(dacByte2);
	SET_DAC_CS(HIGH);
}

//additonal command
//toggles between 38400 and 153600 baud UART.
//153600 may not be stable
void toggleUART() {
	if (fastUART) {
		//go to slow UART
		fastUART = FALSE;
		UARTinit(MYUBRR);
	} else {
		//go to fast uart
		fastUART = TRUE;
		UARTinit(MYUBRR_FAST);
	}
}
