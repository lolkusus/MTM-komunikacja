#include <LPC213X.H>
#include <math.h>

/*
	INSTRUKCJA STR 127:
	The PINSEL0 register controls the functions of the pins as per the settings listed in Table 65. 
	The direction control bit in the I0DIR register is effective only when 
	the GPIO function is selected for a pin. For other functions, direction 
	is controlled automatically.
*/

//PINSEL0
#define SCK_MASK (1<<8)
#define MISO_MASK (1<<10)
#define MOSI_MASK (1<<12)
#define SSEL_MASK (1<<14)	

#define SPI_MASK 0xFF00
#define SPI_CONFIG (SCK_MASK | MISO_MASK | MOSI_MASK | SSEL_MASK)

//IO0DIR
#define CS_DAC_MASK (1<<10) 

//SPI CONTROL REGISTER S0SPCR
#define CPHA_MASK (1<<3)
#define CPOL_MASK (1<<4)
#define MSTR_MASK (1<<5) //1 = master, 0 = slave
#define LSBF_MASK (1<<6) //1 = lsb first, 0 = msb first
#define SPIE_MASK (1<<7) //spi interrupt enable

//SPI STATUS REGISTER S0SPSR
#define ABRT_MASK (1<<3) //slave abort
#define MODF_MASK (1<<4) //mode fault
#define ROVR_MASK (1<<5) //read overrun
#define WCOL_MASK (1<<6) //write collision
#define SPIF_MASK (1<<7) //transfer complete

//DAC
#define DAC_CONFIG 0x3000
#define DAC_DATA_MASK 0x0FFF
#define DAC_MAX_VOLTAGE 3300 //mV
#define DAC_RESOLUTION (1<<12) //12 bit dac = 4096 steps

//SINE
#define SIN_OFFSET 1000 //srednia wartosc 1V = 1000mV
#define SIN_AMPLITUDE 1000 // pk-pk 2V = 1V amplitude = 1000 mV
//#define SIN_SAMPLES 360
#define SIN_SAMPLE_TIME (double) 0.0027778 //(1/360)
#define TWO_PI (double) 6.28318


void DAC_MCP4921_Set(unsigned int uiVoltage)
{
	unsigned short usFullData = (uiVoltage & DAC_DATA_MASK);
	unsigned char ucHighData;
	unsigned char ucLowData;
	
	usFullData = (usFullData | DAC_CONFIG);
	ucHighData = ((usFullData >> 8) & 0xFF);
	ucLowData = (usFullData & 0xFF);
	
	//inicjowanie
	IO0DIR = (IO0DIR | CS_DAC_MASK);
	PINSEL0 = ((PINSEL0 & ~SPI_MASK) | SPI_CONFIG);
	S0SPCR =  MSTR_MASK ; //mode 0,0; no interrupt, master, MSB first
	S0SPCCR = 0x08; //dzielnik zegara systemowego do zegara spi
	
	//wysylanie
	IO0CLR = CS_DAC_MASK;
	S0SPDR = ucHighData;
	
	while ((S0SPSR & SPIF_MASK) == 0)
	{
	}
	
	S0SPDR = ucLowData;
	
	while ((S0SPSR & SPIF_MASK) == 0)
	{
	}
	
	IO0SET = CS_DAC_MASK;
	
}

void DAC_MCP4921_Set_mV(unsigned int uiVoltage)
{
	unsigned int uiBitValue;
	
	uiBitValue = (uiVoltage*DAC_RESOLUTION)/DAC_MAX_VOLTAGE;
	
	DAC_MCP4921_Set(uiBitValue);
}


int main()
{
	unsigned int uiSinValue = 0;
	double dTime = 0;
	
	while (1)
	{
		uiSinValue = ((SIN_AMPLITUDE * sin(dTime*TWO_PI)) + SIN_OFFSET);
		DAC_MCP4921_Set_mV(uiSinValue);
		dTime = dTime + SIN_SAMPLE_TIME;
	}
	
	return 0;
}
