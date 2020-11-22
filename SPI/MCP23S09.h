#ifndef MCP_H
#define MCP_H

#include "SPI.h"

void MCP_Init (void);

void MCP_Set_Output(unsigned char ucOutputValue);

unsigned char MCP_Get_Input(void);

#endif
