typedef enum CompResult 
{ DIFFERENT , EQUAL } CompResult;

typedef enum Result 
{ OK, ERROR } Result;

enum CompResult eCompareString(char pcStr1[], char pcStr2[]);
void ReplaceCharactersInString(char pcString[],char cOldChar,char cNewChar);
enum Result eHexStringToUInt(char pcStr[],unsigned int *puiValue);
void UIntToHexStr(unsigned int uiValue, char pcStr[]);
void AppendUIntToString (unsigned int uiValue, char pcDestinationStr[]);
void AppendString (char pcSourceStr[],char pcDestinationStr[]);
void CopyString(char pcSource[], char pcDestination[]);
