#include "command_decoder.h"
#include "string.h"

#define HEX_bm 0x000F
#define MAX_KEYWORD_NR 2

struct RecieverBuffer{
char cData[RECIEVER_SIZE];
unsigned char ucCharCtr;
enum eRecieverStatus eStatus;
} sRecieverBuffer;

struct Keyword asKeywordList[MAX_KEYWORD_NR]= 
{
{SPI_GET,"spi_port_get"},
{SPI_SET,"spi_port_set"}
};

unsigned char ucTokenCount;

struct Token asToken[MAX_TOKEN_NR];

unsigned char ucFindTokensInString(char *pcString)
{
	unsigned char ucTokenPointer;
	unsigned char ucDelimiterCounter;
	char cCurrentChar;
	enum State {TOKEN, DELIMITER};
	enum State eState = DELIMITER;
	ucDelimiterCounter = 0;
	
	for(ucTokenPointer=0;;ucTokenPointer++)
	{
		cCurrentChar = pcString[ucTokenPointer];
		switch(eState)
		{
			case DELIMITER:
				if(cCurrentChar == '\0') 
					return ucDelimiterCounter;
				else if(cCurrentChar == ' ') {}
				else 
				{
					eState = TOKEN;
					asToken[ucDelimiterCounter].uValue.pcString = pcString+ucTokenPointer;
					ucDelimiterCounter++;
				}
				break;
			case TOKEN:
				if(cCurrentChar == '\0') 
					return ucDelimiterCounter;
				else if(ucDelimiterCounter == MAX_TOKEN_NR) 
					return ucDelimiterCounter;
				else if(cCurrentChar != ' ') {}
				else 
					eState = DELIMITER;
				break;
		}
	}
}


enum Result eStringToKeyword (char pcStr[],enum KeywordCode *peKeywordCode)
{
	unsigned char ucTokenCounter;
	for(ucTokenCounter=0;ucTokenCounter<ucTokenCount;ucTokenCounter++)
	{
		if (eCompareString(pcStr,asKeywordList[ucTokenCounter].cString) == EQUAL) 
		{
			*peKeywordCode = asKeywordList[ucTokenCounter].eCode;
			return OK;
		}
	}
	return ERROR;
}

void DecodeTokens()
{
	unsigned char ucTokenCounter;
	Token* tValue;
	for(ucTokenCounter=0;ucTokenCounter<MAX_TOKEN_NR;ucTokenCounter++)
	{
		tValue = &asToken[ucTokenCounter];
		if (eStringToKeyword(tValue->uValue.pcString,&tValue->uValue.eKeyword) == OK) tValue->eType = KEYWORD;
		else if (eHexStringToUInt(tValue->uValue.pcString,&tValue->uValue.uiNumber) == OK) tValue->eType = NUMBER;
		else tValue->eType = STRING;
	}
}

void DecodeMsg(char *pcString)
{
	ucTokenCount = ucFindTokensInString(pcString);
	ReplaceCharactersInString(pcString,' ','\0');
	DecodeTokens();
}

void Reciever_Empty()
{
	sRecieverBuffer.eStatus = EMPTY;
	sRecieverBuffer.ucCharCtr = 0;
}

void Reciever_PutCharacterToBuffer(char cCharacter)
{
	if(sRecieverBuffer.ucCharCtr == RECIEVER_SIZE)
	{
		sRecieverBuffer.eStatus = OVERFLOW;
		Reciever_Empty();
	}
	else
	{
		if(cCharacter == TERMINATOR)
		{
			sRecieverBuffer.cData[sRecieverBuffer.ucCharCtr] = '\0';
			sRecieverBuffer.eStatus = READY;
			sRecieverBuffer.ucCharCtr = 0;
		}
		else
		{
			sRecieverBuffer.cData[sRecieverBuffer.ucCharCtr] = cCharacter;
			sRecieverBuffer.ucCharCtr++;
		}
	}
}

enum eRecieverStatus eReciever_GetStatus(void)
{
	return sRecieverBuffer.eStatus;
}

void Reciever_GetStringCopy(char *ucDestination)
{
	unsigned char ucCharacterCounter;
	for(ucCharacterCounter=0;sRecieverBuffer.cData[ucCharacterCounter]!='\0';ucCharacterCounter++)
	{
		if(ucCharacterCounter==RECIEVER_SIZE)
			break;
		else
			ucDestination[ucCharacterCounter] = sRecieverBuffer.cData[ucCharacterCounter];
	}
	Reciever_Empty();
}
