#include "MCP23S09.h"
#include <LPC213X.H>

#define CS_IO_bm 	(1<<10) 

#define MCP23S09_GPIO_REG_ADDR 0x09
#define MCP23S09_IODIR_REG_ADDR 0x00

#define MCP23S09_ALL_INPUTS 0xFF
#define MCP23S09_ALL_OUTPUTS 0x00

#define MCP23S09_REG_WRITE_OPCODE 0x40
#define MCP23S09_REG_READ_OPCODE 0x41

void MCP_Init (void)
{
	IO0DIR = (IO0DIR | CS_IO_bm);
	IO0SET = CS_IO_bm;
}

void MCP_Set_Output(unsigned char ucOutputValue)
{
	SPI_TransactionParams sSPI_TransactionParams;
	unsigned char ucaTxArray[3];
	unsigned char ucaRxArray[1];
	
	sSPI_TransactionParams.pucBytesForRx = ucaRxArray;
	sSPI_TransactionParams.pucBytesForTx = ucaTxArray;
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	sSPI_TransactionParams.ucNrOfBytesForTx = 3;
	sSPI_TransactionParams.ucRxBytesOffset = 0;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	
	ucaTxArray[0] = MCP23S09_REG_WRITE_OPCODE;
	ucaTxArray[1] = MCP23S09_IODIR_REG_ADDR;		//upewnianie sie ze to output
	ucaTxArray[2] = MCP23S09_ALL_OUTPUTS;

	IO0CLR = CS_IO_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);	//iodir
	IO0SET = CS_IO_bm;

	ucaTxArray[1] = MCP23S09_GPIO_REG_ADDR;		//wysylanie wartosci na output
	ucaTxArray[2] = ucOutputValue;
	
	IO0CLR = CS_IO_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);	//gpio
	IO0SET = CS_IO_bm;
}

unsigned char MCP_Get_Input()
{
	SPI_TransactionParams sSPI_TransactionParams;
	unsigned char ucaTxArray[3];
	unsigned char ucaRxArray[1];
	
	sSPI_TransactionParams.pucBytesForRx = ucaRxArray;
	sSPI_TransactionParams.pucBytesForTx = ucaTxArray;
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	sSPI_TransactionParams.ucNrOfBytesForTx = 3;
	sSPI_TransactionParams.ucRxBytesOffset = 0;
	sSPI_TransactionParams.ucTxBytesOffset = 0;
	
	ucaTxArray[0] = MCP23S09_REG_WRITE_OPCODE;
	ucaTxArray[1] = MCP23S09_IODIR_REG_ADDR;		//ustalanie kierunku na input
	ucaTxArray[2] = MCP23S09_ALL_INPUTS;

	IO0CLR = CS_IO_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);	//iodir
	IO0SET = CS_IO_bm;
	
	sSPI_TransactionParams.ucNrOfBytesForTx = 2;
	sSPI_TransactionParams.ucRxBytesOffset = 2;
	sSPI_TransactionParams.ucNrOfBytesForRx = 1;
	ucaTxArray[0] = MCP23S09_REG_READ_OPCODE;
	ucaTxArray[1] = MCP23S09_GPIO_REG_ADDR;		//odbieranie stanu wartosci na input
	
	IO0CLR = CS_IO_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);	//odczyt
	IO0SET = CS_IO_bm;
	
	sSPI_TransactionParams.ucNrOfBytesForTx = 3;
	sSPI_TransactionParams.ucNrOfBytesForRx = 0;
	sSPI_TransactionParams.ucRxBytesOffset = 0;
	ucaTxArray[0] = MCP23S09_REG_WRITE_OPCODE;
	ucaTxArray[1] = MCP23S09_IODIR_REG_ADDR;		//ustalanie z powrotem na output
	ucaTxArray[2] = MCP23S09_ALL_OUTPUTS;

	IO0CLR = CS_IO_bm;
	SPI_ExecuteTransaction(sSPI_TransactionParams);	//iodir
	IO0SET = CS_IO_bm;
	
	return ucaRxArray[0];
}
