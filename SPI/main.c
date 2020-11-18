#include <LPC213X.H>
#include <math.h>
#include "SPI.h"
#include "sine.h"

//IO0DIR
#define CS_DAC_bm 	(1<<10) 

//DAC CONTROL WORD
#define DAC_SEL_CONTROL_bm 		(1<<15) //0 = DACa, 1 = DACb
#define DAC_BUF_CONTROL_bm 		(1<<14) //0 = unbuffered, 1 = buffered
#define DAC_GAIN_CONTROL_bm 	(1<<13)	//0 = output x2, 1 = output x1
#define DAC_nSHDN_CONTROL_bm	(1<<12) //0 = shutdown, 1 = operational
#define DAC_VOLTAGE_bm 					0x0FFF
#define DAC_MAX_VOLTAGE 			3300 //mV
#define DAC_RESOLUTION 				4096 //12 bit dac = 4096 steps

//SINE
#define SIN_OFFSET 			1000 //srednia wartosc 1V = 1000mV
#define SIN_AMPLITUDE		1000 // pk-pk 2V = 1V amplitude = 1000 mV
#define SIN_SAMPLES 		360
#define TWO_PI (double) 6.28318
#define SIN_SAMPLE_TIME (double) 	0.0027778 //(1/360)

//SPI PRESCALER REGISTER S0SPCCR
#define SPI_CLK_DIV 0x08

void DAC_MCP4921_Set(unsigned int uiVoltage)
{
	unsigned short usDacControlWord = ((uiVoltage & DAC_VOLTAGE_bm) | (DAC_GAIN_CONTROL_bm | DAC_nSHDN_CONTROL_bm)); //!dac control word
	unsigned char ucaTxArray[2];
	unsigned char ucaRxArray[1];
	SPI_TransactionParams sSPI_TransactionParams;
	
	sSPI_TransactionParams.pucBytesForRx = ucaRxArray;
	sSPI_TransactionParams.pucBytesForTx = ucaTxArray;
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	sSPI_TransactionParams.ucNrOfBytesForTx = 2;
	sSPI_TransactionParams.ucRxBytesOffset = 0;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	
	//inicjowanie
	IO0DIR = (IO0DIR | CS_DAC_bm);

	//wysylanie
	IO0CLR = CS_DAC_bm;
	ucaTxArray[0] = ((usDacControlWord >> 8) & 0xFF);
	ucaTxArray[1] = (usDacControlWord & 0xFF);

	SPI_ExecuteTransaction(sSPI_TransactionParams);

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
	SPI_FrameParams sSPI_FrameParams;
	unsigned int uiTime = 0;
	
	sSPI_FrameParams.ClkDivider = SPI_CLK_DIV;
	sSPI_FrameParams.ucClsbf = 0;
	sSPI_FrameParams.ucCpha = 0;
	sSPI_FrameParams.ucCpol = 0;
	SPI_ConfigMaster(sSPI_FrameParams);
	
	while (1)
	{
		//porowananie luta z ram/rom
		
		DAC_MCP4921_Set_mV(sin_flash[uiTime]);
		
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
