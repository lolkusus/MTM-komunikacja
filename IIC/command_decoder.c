#include "command_decoder.h"
#include "string.h"

#define HEX_bm 0x000F
#define MAX_KEYWORD_NR 4
#define DELIM ' '

struct RecieverBuffer{
char cData[RECIEVER_SIZE];
unsigned char ucCharCtr;
enum eRecieverStatus eStatus;
} sRecieverBuffer;

struct Keyword asKeywordList[MAX_KEYWORD_NR]= 
{
{PORT_WRITE,"portwr"},
{PORT_READ,"portrd"},
{MEM_WRITE,"memwr"},
{MEM_READ,"memrd"}
};

struct Token asToken[MAX_TOKEN_NR];
enum Stan {TOKEN,DELIMITER};
unsigned char ucTokenNr = 0;

unsigned char ucFindTokensInString(char *pcString)
{
	enum Stan eState = DELIMITER;
	unsigned char ucTokenCounter = 0;
	unsigned char ucCharCounter = 0;
	char cActualChar;
	
	for(ucCharCounter=0; pcString[ucCharCounter] != '\0'; ucCharCounter++){
		cActualChar = pcString[ucCharCounter];
		switch(eState){
			case DELIMITER: 	
				if(cActualChar == DELIM){
					eState = DELIMITER;
				}else if(ucTokenCounter < MAX_TOKEN_NR){
					eState = TOKEN;
					asToken[ucTokenCounter].uValue.pcString = pcString + ucCharCounter; 
					ucTokenCounter++;
				}else{
					return(ucTokenCounter);
				}
				break;
			case TOKEN: 
				if(cActualChar!= DELIM){
					eState = TOKEN;
				}else{
					eState = DELIMITER;
				}
				break;
		}	
	}
	return(ucTokenCounter);
}

enum Result eStringToKeyword(char pcStr[],enum KeywordCode *peKeywordCode){
	unsigned char ucKeywordCounter;
	for(ucKeywordCounter=0;ucKeywordCounter<MAX_KEYWORD_NR;ucKeywordCounter++)
	{
		if (eCompareString(pcStr,asKeywordList[ucKeywordCounter].cString) == EQUAL) 
		{
			*peKeywordCode = asKeywordList[ucKeywordCounter].eCode;
			return OK;
		}
	}
	return ERROR;
};

void DecodeTokens(void){
	
	unsigned int uiNumber;
	unsigned char ucTokenIndex;
	enum KeywordCode eKeyword;
	
	for(ucTokenIndex=0; ucTokenIndex < ucTokenNr; ucTokenIndex++){
		if((eStringToKeyword (asToken[ucTokenIndex].uValue.pcString, &eKeyword))== OK){
			asToken[ucTokenIndex].eType = KEYWORD;
			asToken[ucTokenIndex].uValue.eKeyword=eKeyword;
		}else if((eHexStringToUInt(asToken[ucTokenIndex].uValue.pcString,&uiNumber))== OK){
			asToken[ucTokenIndex].eType = NUMBER;
			asToken[ucTokenIndex].uValue.uiNumber = uiNumber;		
		}else{
			asToken[ucTokenIndex].eType = STRING;
		}
	}
}

void DecodeMsg(char *pcString){

	ucTokenNr = ucFindTokensInString(pcString);
	ReplaceCharactersInString(pcString, ' ', '\0');
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
