#include <LPC213X.H>
#include "IIC.h"

//PCF8574
#define PCF_ADDRESS 0x40 //address do zapisu (lsb to flaga zapis/odczyt)

void Delay(unsigned int uiDelay) //funkcja delay z ppsw
{
	unsigned int uiDelayCounter;
	uiDelay = uiDelay * 7500;
	for(uiDelayCounter=0; uiDelayCounter < uiDelay; uiDelayCounter++){
	}
}

void PCF8574_Write (unsigned char ucData)
{
	static unsigned char pucData[1];
	IIC_Params sIIC_Params;
	
	pucData[0] = ucData;
	sIIC_Params.eI2CTransmisionMode = TX;
	sIIC_Params.pucBytesForTx = pucData;
	sIIC_Params.ucNrOfBytesForRx = 0;
	sIIC_Params.ucNrOfBytesForTx = 1;
	sIIC_Params.ucSlaveAddress = PCF_ADDRESS;
	sIIC_Params.ucDone = 0;

	ExecuteTransaction(sIIC_Params);
}

int main()
{
	unsigned char ucCounterValue = 0;
	
	IIC_Init();
	
	while(1)
	{
		PCF8574_Write(ucCounterValue);
		ucCounterValue++;
		Delay(250);
	}
}
