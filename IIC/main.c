//#include <LPC213X.H>
#include "IIC.h"

//PCF8574
#define PCF_ADDRESS 0x40 //address do zapisu (lsb to flaga zapis/odczyt)
#define MCP_ADDRESS 0xA0

unsigned char ucPCF8574_Input;

void Delay(unsigned int uiDelay) //funkcja delay z ppsw
{
	unsigned int uiDelayCounter;
	uiDelay = uiDelay * 7500;
	for(uiDelayCounter=0; uiDelayCounter < uiDelay; uiDelayCounter++){
	}
}

void PCF8574_Write (unsigned char ucData)
{
	unsigned char pucDataTX[1];
	IIC_Params sIIC_Params;
	
	pucDataTX[0] = ucData;
	sIIC_Params.eI2CTransmisionMode = TX;
	sIIC_Params.pucBytesForTx = pucDataTX;
	sIIC_Params.ucNrOfBytesForRx = 0;
	sIIC_Params.ucNrOfBytesForTx = 1;
	sIIC_Params.ucSlaveAddress = PCF_ADDRESS;

	ExecuteTransaction(&sIIC_Params);
}

void PCF8574_Read ()
{
	IIC_Params sIIC_Params;
	
	sIIC_Params.eI2CTransmisionMode = RX;
	sIIC_Params.pucBytesForRx = &ucPCF8574_Input;
	sIIC_Params.ucNrOfBytesForRx = 1;
	sIIC_Params.ucNrOfBytesForTx = 0;
	sIIC_Params.ucSlaveAddress = PCF_ADDRESS;

	ExecuteTransaction(&sIIC_Params);
}

void MC24LC64_ByteWrite( unsigned int WordAddress, unsigned char Data)
{
	
}

void MC24LC64_RandomRead (unsigned int WordAddress)
{
	
}

int main()
{
	IIC_Init();
	
	while(1)
	{
		PCF8574_Write(ucPCF8574_Input);
		ucPCF8574_Input++;
		while(isTransactionDone() == 0){}
		PCF8574_Read();
		Delay(250);
	}
}
