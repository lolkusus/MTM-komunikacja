#include <LPC213X.H>
#include "IIC.h"

//PINSEL0
#define SCL_PINSEL_MASK (1<<4)
#define SDA_PINSEL_MASK (1<<6)

#define IIC_MASK 0x00F0
#define IIC_CONFIG (SCL_PINSEL_MASK | SDA_PINSEL_MASK)

//I2C0CONSET/CLR 
#define I2EN_MASK (1<<6) //IIC Enable
#define STA_MASK (1<<5) //Start flag
#define STO_MASK (1<<4) //Stop flag  - nie wolno clearowac! - tabela 104 str 173
#define SI_MASK (1<<3) //Interupt flag
#define AA_MASK (1<<2) //Assert Ack

//I2C0SCLH/I2C0SCLL
#define IIC_HIGH_CLOCK_VALUE	0x80	//dane z instrukcji zadania
#define IIC_LOW_CLOCK_VALUE		0x80 

//I2C0STAT
#define IIC_START_TRANSMITTER_STATUS 	0x08			//Start condition transmitted
#define IIC_REPEAT_START_TRANSMITTER_STATUS 	0x10			//Start condition transmitted
#define IIC_TX_SLAVE_ADDR_ACK_STATUS 0x18				//Slave address ACK n TX mode
#define IIC_TX_SLAVE_ADDR_NOT_ACK_STATUS 0x20 	//Slave did NOT ACK address in TX mode
#define IIC_TX_SLAVE_DATA_ACK_STATUS 0x28				//Slave data ACK in TX mode
#define IIC_ARBITRATION_LOST 0x38								//Arbitration lost 
#define IIC_RX_SLAVE_ADDR_ACK_STATUS 0x40				//Slave address ACK in RX mode
#define IIC_RX_SLAVE_ADDR_NOT_ACK_STATUS 0x48 	//Slave did NOT ACK address in RX mode 
#define IIC_RX_MASTER_DATA_ACK_STATUS 0x50 	//Slave did NOT ACK address in RX mode 
#define IIC_RX_MASTER_DATA_NOT_ACK_STATUS 0x58 	//Slave did NOT ACK address in RX mode 

//VIC
#define VIC_IIC_CHANNER_NR 9
#define VIC_IRQ_SLOT_ENABLE (1<<5) //VICVectCntl

unsigned char ucCurrentTxByte;
unsigned char ucCurrentRxByte;
IIC_Params *pIIC_Params;

void IIC_NextRxTx()
{
	switch(pIIC_Params->eI2CTransmisionMode)
	{
		case TX:
			if(ucCurrentTxByte < pIIC_Params->ucNrOfBytesForTx)
			{
				I2C0DAT = pIIC_Params->pucBytesForTx[ucCurrentTxByte];
				ucCurrentTxByte++;
			}
			else
			{
				I2C0CONSET = STO_MASK;
				pIIC_Params->ucDone = 1;
			}
			break;
		
		case RX:
			if(ucCurrentRxByte < pIIC_Params->ucNrOfBytesForRx)
			{
				pIIC_Params->pucBytesForRx[ucCurrentRxByte] = I2C0DAT;
				ucCurrentRxByte++;
				I2C0CONSET = AA_MASK;
			}
			else
			{
				I2C0CONSET &= ~AA_MASK;
			}
			break;
		
		case RX_AFTER_TX:
			if(ucCurrentTxByte < pIIC_Params->ucNrOfBytesForTx)
			{
				I2C0DAT = pIIC_Params->pucBytesForTx[ucCurrentTxByte];
				ucCurrentTxByte++;
			}
			else
			{
				pIIC_Params->eI2CTransmisionMode = RX;
				pIIC_Params->ucSlaveAddress = (pIIC_Params->ucSlaveAddress | 0x01);
				I2C0CONSET = STA_MASK;
			}
			break;
		
		default:
			break;
	}
}

__irq void IIC_Interrupt()
{
	switch (I2C0STAT)
	{
		case IIC_START_TRANSMITTER_STATUS:
			I2C0DAT = pIIC_Params->ucSlaveAddress; //address
			I2C0CONCLR = STA_MASK; //wyczysci flage startu
			break;
		
		case IIC_REPEAT_START_TRANSMITTER_STATUS:
			I2C0DAT = pIIC_Params->ucSlaveAddress; //address
			I2C0CONCLR = STA_MASK; //wyczysci flage startu
			break;
		
		case IIC_TX_SLAVE_ADDR_ACK_STATUS:
			IIC_NextRxTx();
			break;
		
		case IIC_TX_SLAVE_ADDR_NOT_ACK_STATUS:
			LedOn(0);	//blad
			pIIC_Params->ucDone = 1;
			I2C0CONSET = STO_MASK;
			break;
		
		case IIC_TX_SLAVE_DATA_ACK_STATUS:
			IIC_NextRxTx();
			break;
		
		case IIC_ARBITRATION_LOST:
			LedOn(1); //blad
			pIIC_Params->ucDone = 1;
			I2C0CONSET = STO_MASK;
			break;
		
		case IIC_RX_SLAVE_ADDR_ACK_STATUS:
			I2C0DAT = 0xFF;		//linia musi byc recesywna, a wysylam tylko zeby byl zegar
			break;
		
		case IIC_RX_SLAVE_ADDR_NOT_ACK_STATUS:
			LedOn(2); //blad
			pIIC_Params->ucDone = 1;
			I2C0CONSET = STO_MASK;
			break;
		
		case IIC_RX_MASTER_DATA_ACK_STATUS:
			IIC_NextRxTx();
			break;
		
		case IIC_RX_MASTER_DATA_NOT_ACK_STATUS:
			pIIC_Params->ucDone = 1;
			I2C0CONSET = STO_MASK; //ostatnich danych nie nie ackuje, tylko nackuje i stopuje
			break;
		
		default:
			break;
	}
	I2C0CONCLR = SI_MASK; //czysc flage przerwania IIC
	VICVectAddr = 0x00; //strona 102 sugeruje zerowanie tej wartosci pod koniec kazdego isr
}

void IIC_Init(void)
{
	LedInit();
	LedOn(4);
	PINSEL0 = ((PINSEL0 & ~IIC_MASK) | IIC_CONFIG);
	
	I2C0CONCLR = (I2EN_MASK | STA_MASK | SI_MASK | AA_MASK); 
	I2C0CONSET = I2EN_MASK; 
	I2C0SCLH = IIC_HIGH_CLOCK_VALUE; 		//dane z instrukcji do zadania
	I2C0SCLL = IIC_HIGH_CLOCK_VALUE; 		//dane z instrukcji do zadania

	VICIntEnable |= (1<<VIC_IIC_CHANNER_NR);
	VICVectCntl0 = (VIC_IRQ_SLOT_ENABLE | VIC_IIC_CHANNER_NR); //ustawienie przerwania iic na slocie irq0 i z vectorem na funkcje przerwania
	VICVectAddr0  =(unsigned long) IIC_Interrupt;
}

void ExecuteTransaction(IIC_Params *sIIC_Params)
{
	pIIC_Params = sIIC_Params;
	LedOn(4); //zgasic bledy
	
	if (pIIC_Params->eI2CTransmisionMode == RX)
	{
		pIIC_Params->ucSlaveAddress = (pIIC_Params->ucSlaveAddress | 0x01);
	}
	else
	{
			pIIC_Params->ucSlaveAddress = pIIC_Params->ucSlaveAddress;
	}
	
	ucCurrentTxByte = 0;
	ucCurrentRxByte = 0;
	pIIC_Params->ucDone = 0;
	I2C0CONSET = STA_MASK;
}

unsigned char isTransactionDone()
{
	return pIIC_Params->ucDone;
}
