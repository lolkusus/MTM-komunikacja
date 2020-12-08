//#include <LPC213X.H>
#include "IIC.h"
#include "command_decoder.h"
#include "uart.h"
#include "string.h"

//PCF8574
#define PCF_ADDRESS 0x40 //address do zapisu (lsb to flaga zapis/odczyt)
#define MCP_ADDRESS 0xA0

//TX STRING SIZE
#define MAX_TRANSMIT_SIZE 8 

unsigned char ucPCF8574_Input;
IIC_Params sIIC_Params;

void Delay(unsigned int uiDelay) //funkcja delay z ppsw
{
	unsigned int uiDelayCounter;
	uiDelay = uiDelay * 7500;
	for(uiDelayCounter=0; uiDelayCounter < uiDelay; uiDelayCounter++){
	}
}

void PCF8574_Write (unsigned char ucData)
{
	static unsigned char pucDataTX[1];
	
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
	sIIC_Params.eI2CTransmisionMode = RX;
	sIIC_Params.pucBytesForRx = &ucPCF8574_Input;
	sIIC_Params.ucNrOfBytesForRx = 1;
	sIIC_Params.ucNrOfBytesForTx = 0;
	sIIC_Params.ucSlaveAddress = PCF_ADDRESS;

	ExecuteTransaction(&sIIC_Params);
}

void MC24LC64_ByteWrite(unsigned int WordAddress, unsigned char ucData)
{
	unsigned char pucDataTX[3];
	
	pucDataTX[0] = ((WordAddress >> 8) & 0xFF);
	pucDataTX[1] = (WordAddress & 0xFF);
	pucDataTX[2] = ucData;
	
	sIIC_Params.eI2CTransmisionMode = TX;
	sIIC_Params.pucBytesForTx = pucDataTX;
	sIIC_Params.ucNrOfBytesForRx = 0;
	sIIC_Params.ucNrOfBytesForTx = 3;
	sIIC_Params.ucSlaveAddress = MCP_ADDRESS;

	ExecuteTransaction(&sIIC_Params);
}

void MC24LC64_RandomRead(unsigned int WordAddress)
{
	unsigned char pucDataTX[2];
	
	pucDataTX[0] = ((WordAddress >> 8) & 0xFF);
	pucDataTX[1] = (WordAddress & 0xFF);
	
	sIIC_Params.eI2CTransmisionMode = RX_AFTER_TX;
	sIIC_Params.pucBytesForTx = pucDataTX;
	sIIC_Params.pucBytesForRx = &ucPCF8574_Input;
	sIIC_Params.ucNrOfBytesForRx = 1;
	sIIC_Params.ucNrOfBytesForTx = 2;
	sIIC_Params.ucSlaveAddress = MCP_ADDRESS;

	ExecuteTransaction(&sIIC_Params);
}

int main()
{
	char cReceivedString[RECIEVER_SIZE] = "memwr 0xDCBA 0x77";
	char cTransmitString[MAX_TRANSMIT_SIZE];
	enum TransmitFlag {INIT, IDLE};
	enum TransmitFlag eTransmitFlag = IDLE;
	
	DecodeMsg(cReceivedString);

	IIC_Init();
	UART_InitWithInt(9600);
	PCF8574_Write(0x00);
	while(sIIC_Params.ucDone == 0){}
	
	while(1)
	{
		if (sIIC_Params.ucDone != 0)
		{
				if ((eTransmitFlag == INIT) && (Transmiter_GetStatus() == FREE))
				{
					UIntToHexStr(ucPCF8574_Input, cTransmitString);
					Transmiter_SendString(cTransmitString);
					eTransmitFlag = IDLE;
				}
				
				if(eReciever_GetStatus() == READY) 
				{
					Reciever_GetStringCopy(cReceivedString);
					DecodeMsg(cReceivedString);
					
					if(asToken[0].eType == KEYWORD)
					{
						switch(asToken[0].uValue.eKeyword)
						{
							case PORT_WRITE:
								PCF8574_Write(asToken[1].uValue.uiNumber);
								break;
							
							case PORT_READ:
								PCF8574_Read();
								eTransmitFlag = INIT;
								break;
							
							case MEM_WRITE:
								MC24LC64_ByteWrite(asToken[1].uValue.uiNumber, (asToken[2].uValue.uiNumber & 0xFF));
								break;
							
							case MEM_READ:
								MC24LC64_RandomRead(asToken[1].uValue.uiNumber);
								eTransmitFlag = INIT;
								break;
							
							default:
								break;
						}
					}
				}
			}
	}
}
