#include <LPC21xx.H>
#include "uart.h"
#include "command_decoder.h"

#define UART_RXD_PIN0_1		(1<<2)
#define UART_TXD_PIN0_1		(1<<0)

#define mDIVISOR_LATCH_ACCES_BIT                   (1<<7)
#define m8BIT_UART_WORD_LENGTH                     0x00000003

#define mRX_DATA_AVALIABLE_INTERRUPT_ENABLE        (1<<0)
#define mTHRE_INTERRUPT_ENABLE                     (1<<1)

#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0x0000000F
#define mTHRE_INTERRUPT_PENDING                    (1<<1)
#define mRX_DATA_AVALIABLE_INTERRUPT_PENDING       (1<<2)

#define VIC_UART0_CHANNEL_NR  6

#define mIRQ_SLOT_ENABLE                           (1<<5)

typedef struct TransmiterBuffer{
char cData[TRANSMITER_SIZE];
enum eTransmiterStatus eStatus;
unsigned char fLastCharacter;
unsigned char cCharCtr;
}TransmiterBuffer;

struct TransmiterBuffer sTransmiterBuffer;

__irq void UART0_Interrupt (void) {
   unsigned int uiCopyOfU0IIR=U0IIR;

   if      ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING)
   {
      Reciever_PutCharacterToBuffer(U0RBR);
   } 
   
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING) 
   {
			if ((Transmiter_GetStatus() == BUSY) & (sTransmiterBuffer.fLastCharacter == 0))
			{
				U0THR = Transmiter_GetCharacterFromBuffer();
			}
			else if (sTransmiterBuffer.fLastCharacter == 1)
				sTransmiterBuffer.fLastCharacter = 0;
   }

   VICVectAddr = 0;
}

void UART_InitWithInt(unsigned int uiBaudRate){

   PINSEL0 = PINSEL0 | UART_RXD_PIN0_1 | UART_TXD_PIN0_1;                                     // ustawic pina na odbiornik uart0
   U0LCR  |= (m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT); // dlugosc slowa, DLAB = 1
   U0DLL   = (((15000000)/16)/uiBaudRate);                      // predkosc transmisji
   U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
   U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE;               
	 U0IER  |= mTHRE_INTERRUPT_ENABLE;
	
   VICVectAddr1  = (unsigned long) UART0_Interrupt;             // set interrupt service routine address
   VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
   VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel
}	 

char Transmiter_GetCharacterFromBuffer() {
	if(sTransmiterBuffer.fLastCharacter == 1)
	{
		return '\0';
	}
	else if(sTransmiterBuffer.cData[sTransmiterBuffer.cCharCtr] == '\0')
	{
		sTransmiterBuffer.cCharCtr = 0;
		sTransmiterBuffer.fLastCharacter = 1;
		sTransmiterBuffer.eStatus = FREE;
		return '\r';
	}
	else
	{
	  sTransmiterBuffer.cCharCtr++;
		return sTransmiterBuffer.cData[sTransmiterBuffer.cCharCtr-1];
	}
}

void Transmiter_SendString(char cString[]){
	unsigned char ucCopyCounter;
	
	for (ucCopyCounter = 0; cString[ucCopyCounter] != '\0'; ucCopyCounter++)
	{
		sTransmiterBuffer.cData[ucCopyCounter] = cString[ucCopyCounter];
	}
	
	sTransmiterBuffer.cData[ucCopyCounter] = cString[ucCopyCounter];
	sTransmiterBuffer.eStatus = BUSY;
	sTransmiterBuffer.fLastCharacter = 0;
	sTransmiterBuffer.cCharCtr = 1;
	U0THR = sTransmiterBuffer.cData[0];
}

enum eTransmiterStatus Transmiter_GetStatus(void)
{
	if (sTransmiterBuffer.fLastCharacter == 1)
		return BUSY;
	return sTransmiterBuffer.eStatus;
}
