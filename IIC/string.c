#include "string.h"

#define HEX_bm 0x000F

enum CompResult eCompareString(char pcStr1[], char pcStr2[])
{
	unsigned char ucCharCounter;
	for(ucCharCounter=0;pcStr1[ucCharCounter] != '\0';ucCharCounter++)
	{
		if (pcStr1[ucCharCounter] != pcStr2[ucCharCounter]) return DIFFERENT;
	}
	return EQUAL;
}

void ReplaceCharactersInString(char pcString[],char cOldChar,char cNewChar)
{
	unsigned char ucCharCounter;
	for(ucCharCounter=0;pcString[ucCharCounter]!='\0';ucCharCounter++)
	{
		if(pcString[ucCharCounter] == cOldChar) pcString[ucCharCounter] = cNewChar;
	}
}

enum Result eHexStringToUInt(char pcStr[],unsigned int *puiValue)
{
	unsigned char ucCharCounter;
	if((pcStr[0] != '0') | (pcStr[1] != 'x') | (pcStr[2] == '\0'))
		return ERROR;
	*puiValue = 0;
	for(ucCharCounter=2;ucCharCounter<7;ucCharCounter++)
	{
		if(pcStr[ucCharCounter] == '\0') 
			return OK;
	  *puiValue = *puiValue << 4;
		if(pcStr[ucCharCounter] >= 'A') 
			*puiValue = *puiValue | (pcStr[ucCharCounter] - 'A' + 10); 
		else 
			*puiValue = *puiValue | (pcStr[ucCharCounter] - '0');
	}
	return OK;
}

void UIntToHexStr(unsigned int uiValue, char pcStr[])
{
	unsigned char ucNibbleCounter;
	unsigned char ucCurrentNibble;
	pcStr[0] = '0';
	pcStr[1] = 'x';
	for(ucNibbleCounter=0;ucNibbleCounter<4;ucNibbleCounter++)
	{
		ucCurrentNibble = ((uiValue >> ucNibbleCounter*4) & HEX_bm);
		if(ucCurrentNibble>9) 
			pcStr[5-ucNibbleCounter] = ucCurrentNibble - 10 + 'A';
		else 
			pcStr[5-ucNibbleCounter] = ucCurrentNibble + '0';
	}
	pcStr[6] = '\0';
}

void AppendUIntToString (unsigned int uiValue, char pcDestinationStr[])
{
	unsigned char ucEndPointer;
	for(ucEndPointer=0;pcDestinationStr[ucEndPointer]!='\0';ucEndPointer++) {}
	UIntToHexStr(uiValue,pcDestinationStr+ucEndPointer);
}

void AppendString (char pcSourceStr[],char pcDestinationStr[])
{
	unsigned char ucEndPointer;
	for(ucEndPointer=0;pcDestinationStr[ucEndPointer]!='\0';ucEndPointer++) {}
	CopyString(pcSourceStr,pcDestinationStr+ucEndPointer);
}

void CopyString(char pcSource[], char pcDestination[])
{
	unsigned char ucCharCounter;
	for(ucCharCounter=0;pcSource[ucCharCounter]!='\0';ucCharCounter++)
	{
		pcDestination[ucCharCounter] = pcSource[ucCharCounter];
	}
	pcDestination[ucCharCounter] = '\0';
}
