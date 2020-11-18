#include "SPI.h"
#include <LPC213X.H>

//PINSEL0
#define SCK_bm 		(1<<8)
#define MISO_bm 	(1<<10)
#define MOSI_bm 	(1<<12)
#define SSEL_bm 	(1<<14)	
#define SPI_PINSEL_bm		0xFF00

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

//FUNKCJE

void SPI_ConfigMaster(SPI_FrameParams sSPI_FrameParams)
{
	unsigned char ucS0SPCR_Temp = 0;
	
	PINSEL0 = ((PINSEL0 & ~SPI_PINSEL_bm) | SCK_bm | MISO_bm | MOSI_bm | SSEL_bm);
	
	if (sSPI_FrameParams.ucClsbf != 0)
	{
		ucS0SPCR_Temp |= LSBF_bm;
	}

	if (sSPI_FrameParams.ucCpha != 0)
	{
		ucS0SPCR_Temp |= CPHA_bm;
	}
	
	if (sSPI_FrameParams.ucCpol != 0)
	{
		ucS0SPCR_Temp |= CPOL_bm;
	}
	
	ucS0SPCR_Temp |= MSTR_bm; 		//wymog zadania to ustawic master
	
	S0SPCR =  ucS0SPCR_Temp; 										//mode 0,0; no interrupt, master, MSB first
	S0SPCCR = sSPI_FrameParams.ClkDivider; 			//dzielnik zegara systemowego do zegara spi
}

void SPI_ExecuteTransaction(SPI_TransactionParams sSPI_TransactionParams)
{
	unsigned char ucTransactionLenght;
	unsigned char ucCurrentTxByte = 0;
	unsigned char ucCurrentRxByte = 0;
	unsigned char ucCurrentByte;
	
	//Musze znalezc ile trwa cala transakcja - trwa maksymalnie tyle ile jest offset + ilosc bytow, ktoregokolwiek nadawania lub odbierania
	if ((sSPI_TransactionParams.ucNrOfBytesForRx + sSPI_TransactionParams.ucRxBytesOffset) >= (sSPI_TransactionParams.ucNrOfBytesForTx + sSPI_TransactionParams.ucTxBytesOffset))
	{
		ucTransactionLenght = (sSPI_TransactionParams.ucNrOfBytesForRx + sSPI_TransactionParams.ucRxBytesOffset);
	}
	else
	{
		ucTransactionLenght = (sSPI_TransactionParams.ucNrOfBytesForTx + sSPI_TransactionParams.ucTxBytesOffset);
	}
	
	for (ucCurrentByte = 0; ucCurrentByte < ucTransactionLenght; ucCurrentByte++)
	{
		if ((ucCurrentByte >= sSPI_TransactionParams.ucTxBytesOffset) && (ucCurrentByte < sSPI_TransactionParams.ucNrOfBytesForTx))
		{
			S0SPDR = sSPI_TransactionParams.pucBytesForTx[ucCurrentTxByte];
			ucCurrentTxByte++;
		}
		
		while ((S0SPSR & SPIF_bm) == 0) {}
			
		if ((ucCurrentByte >= sSPI_TransactionParams.ucRxBytesOffset) && (ucCurrentByte < sSPI_TransactionParams.ucNrOfBytesForRx))
		{
			sSPI_TransactionParams.pucBytesForRx[ucCurrentRxByte] = S0SPDR;
			ucCurrentRxByte++;
		}
	}
	
}
