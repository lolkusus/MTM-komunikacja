#include <LPC213X.H>
#include "MCP23S09.h"
#include "command_decoder.h"
#include "uart.h"
#include "string.h"

//SPI PRESCALER REGISTER S0SPCCR
#define SPI_CLK_DIV 0x08

//TX STRING SIZE
#define MAX_TRANSMIT_SIZE 8 

int main()
{
	SPI_FrameParams sSPI_FrameParams;
	unsigned int uiGPIO_InputValue;
	char cReceivedString[RECIEVER_SIZE];
	char cTransmitString[MAX_TRANSMIT_SIZE];
	enum TransmitFlag {INIT, IDLE};
	enum TransmitFlag eTransmitFlag = IDLE;
	
	sSPI_FrameParams.ClkDivider = SPI_CLK_DIV;
	sSPI_FrameParams.ucClsbf = 0;
	sSPI_FrameParams.ucCpha = 0;
	sSPI_FrameParams.ucCpol = 0;
	SPI_ConfigMaster(sSPI_FrameParams);
	
	MCP_Init();
	MCP_Set_Output(0x00);
	
	UART_InitWithInt(9600);
	
	while(1)
		{
			if(eReciever_GetStatus() == READY)
				{
					Reciever_GetStringCopy(cReceivedString);
					DecodeMsg(cReceivedString);
					
					if((ucTokenCount > 0) & (asToken[0].eType == KEYWORD))
					{
						switch(asToken[0].uValue.eKeyword)
						{
							case SPI_GET:
								uiGPIO_InputValue = MCP_Get_Input();
								UIntToHexStr(uiGPIO_InputValue, cTransmitString);
								eTransmitFlag = INIT;
								break;
								
							case SPI_SET:
								MCP_Set_Output(asToken[1].uValue.uiNumber & 0xFF); 
								break;
								
							default:
								break;
						}
					}
				}
				
				if ((eTransmitFlag == INIT) && (Transmiter_GetStatus() == FREE))
				{
					Transmiter_SendString(cTransmitString);
					eTransmitFlag = IDLE;
				}
		}
}
