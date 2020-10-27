#include <LPC213X.H>
#include <math.h>
#include "sine.h"

/*
	INSTRUKCJA STR 127:
	The PINSEL0 register controls the functions of the pins as per the settings listed in Table 65. 
	The direction control bit in the I0DIR register is effective only when 
	the GPIO function is selected for a pin. For other functions, direction 
	is controlled automatically.
*/

//PINSEL0
#define SCK_bm 		(1<<8)
#define MISO_bm 	(1<<10)
#define MOSI_bm 	(1<<12)
#define SSEL_bm 	(1<<14)	

#define SPI_PINSEL_bm		0xFF00

//IO0DIR
#define CS_DAC_bm 	(1<<10) 

//SPI CONTROL REGISTER S0SPCR
#define CPHA_bm 		(1<<3)	//clock phase
#define CPOL_bm 		(1<<4)	//clock polarity 
#define MSTR_bm 		(1<<5) 	//1 = master, 0 = slave
#define LSBF_bm 		(1<<6) 	//1 = lsb first, 0 = msb first
#define SPIE_bm 		(1<<7) 	//spi interrupt enable

//SPI STATUS REGISTER S0SPSR
#define ABRT_bm 		(1<<3) 	//slave abort
#define MODF_bm 		(1<<4) 	//mode fault
#define ROVR_bm 		(1<<5) 	//read overrun
#define WCOL_bm 		(1<<6) 	//write collision
#define SPIF_bm 		(1<<7) 	//transfer complete

//SPI PRESCALER REGISTER S0SPCCR
#define SPI_CLK_DIV 0x08

//DAC CONTROL WORD
#define DAC_SEL_CONTROL_bm 		(1<<15) //0 = DACa, 1 = DACb
#define DAC_BUF_CONTROL_bm 		(1<<14) //0 = unbuffered, 1 = buffered
#define DAC_GAIN_CONTROL_bm 	(1<<13)	//0 = output x2, 1 = output x1
#define DAC_nSHDN_CONTROL_bm	(1<<12) //0 = shutdown, 1 = operational
#define DAC_DATA_bm 					0x0FFF
#define DAC_MAX_VOLTAGE 			3300 //mV
#define DAC_RESOLUTION 				4096 //12 bit dac = 4096 steps

//SINE
#define SIN_OFFSET 			1000 //srednia wartosc 1V = 1000mV
#define SIN_AMPLITUDE		1000 // pk-pk 2V = 1V amplitude = 1000 mV
#define SIN_SAMPLES 		360
#define TWO_PI (double) 6.28318
#define SIN_SAMPLE_TIME (double) 	0.0027778 //(1/360)



void DAC_MCP4921_Set(unsigned int uiVoltage)
{
	unsigned short usFullData = (uiVoltage & DAC_DATA_bm); //!dac control word
	unsigned char ucHighData;
	unsigned char ucLowData;	//!zmiana na low byte
	
	usFullData = (usFullData | DAC_GAIN_CONTROL_bm | DAC_nSHDN_CONTROL_bm);
	ucHighData = ((usFullData >> 8) & 0xFF);
	ucLowData = (usFullData & 0xFF);
	
	//inicjowanie
	IO0DIR = (IO0DIR | CS_DAC_bm);
	PINSEL0 = ((PINSEL0 & ~SPI_PINSEL_bm) | SCK_bm | MISO_bm | MOSI_bm | SSEL_bm);
	S0SPCR =  MSTR_bm ; 			//mode 0,0; no interrupt, master, MSB first
	S0SPCCR = SPI_CLK_DIV; 		//dzielnik zegara systemowego do zegara spi
	
	//wysylanie
	IO0CLR = CS_DAC_bm;
	S0SPDR = ucHighData;
	
	while ((S0SPSR & SPIF_bm) == 0)
	{
	}
	
	S0SPDR = ucLowData;
	
	while ((S0SPSR & SPIF_bm) == 0)
	{
	}
	
	IO0SET = CS_DAC_bm;
	
}

void DAC_MCP4921_Set_mV(unsigned int uiVoltage)
{
	unsigned int uiBitValue;
	
	uiBitValue = (uiVoltage*DAC_RESOLUTION)/DAC_MAX_VOLTAGE;
	
	DAC_MCP4921_Set(uiBitValue);
}


int main()
{
	//unsigned int uiSinValue = 0;
	//double dTime = 0;
	unsigned int uiTime = 0;
	
	while (1)
	{
		/*		klasyczne podejscie, ale z lekkim usprawnieniem - brak dzielenia
		uiSinValue = ((SIN_AMPLITUDE * sin(dTime*TWO_PI)) + SIN_OFFSET);
		DAC_MCP4921_Set_mV(uiSinValue);
		dTime = dTime + SIN_SAMPLE_TIME;
		*/
		
		//podejscie szybsze = LUT
		
		//porowananie luta z ram/rom
		
		DAC_MCP4921_Set_mV(sine[uiTime]);
		
		if (uiTime == (SIN_SAMPLES-1))
		{
			uiTime = 0;
		}
		else
		{
			uiTime = uiTime + 1;
		}
		
	}
}
