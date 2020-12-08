#ifndef IIC_H
#define IIC_H

#include "led.h"

typedef enum 
{
	TX,
	RX,
	RX_AFTER_TX,
	TX_AFTER_RX
}I2CTransmisionMode;

typedef struct
{
	I2CTransmisionMode eI2CTransmisionMode;
	unsigned char ucSlaveAddress;
	unsigned char *pucBytesForTx;
	unsigned char ucNrOfBytesForTx;
	unsigned char *pucBytesForRx;
	unsigned char ucNrOfBytesForRx;
	unsigned char ucDone;
}IIC_Params;

void IIC_Init(void);

void ExecuteTransaction(IIC_Params *sIIC_Params);

unsigned char isTransactionDone(void);

#endif
