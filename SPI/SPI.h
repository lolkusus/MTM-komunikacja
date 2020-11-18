#ifndef SPI_H
#define SPI_H

//TYPY

typedef struct
{
	unsigned char ucCpha;				// clock phase
	unsigned char ucCpol;				// clock polarity
	unsigned char ucClsbf;			// !=0 lsb first, 0 = msb first
	unsigned char ClkDivider;		// dzielnik zegara
}SPI_FrameParams;

typedef struct 
{
	unsigned char *pucBytesForTx; 		// wskaznik na tablice z bajtami do wyslania
	unsigned char ucNrOfBytesForTx; 	// ilosc bajt�w do wyslania
	unsigned char ucTxBytesOffset; 		// offset bajt�w do wyslania
	unsigned char *pucBytesForRx; 		// wskaznik na tablice na odebrane bajty
	unsigned char ucNrOfBytesForRx;	 	// ilosc bajt�w do odebrania
	unsigned char ucRxBytesOffset; 		// offset bajt�w do odebrania
}SPI_TransactionParams;

//FUNKCJE

void SPI_ConfigMaster(SPI_FrameParams sSPI_FrameParams);

void SPI_ExecuteTransaction(SPI_TransactionParams sSPI_TransactionParams);

#endif 
