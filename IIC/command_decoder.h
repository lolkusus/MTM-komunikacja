#define RECIEVER_SIZE 32
#define TERMINATOR 0x0D
#define MAX_TOKEN_NR 3
#define MAX_KEYWORD_STRING_LTH 6

typedef enum TokenType 
{KEYWORD, NUMBER, STRING} TokenType;

typedef enum KeywordCode 
{PORT_WRITE, PORT_READ, MEM_WRITE, MEM_READ} KeywordCode;

typedef struct Keyword
{
enum KeywordCode eCode;
char cString[MAX_KEYWORD_STRING_LTH + 1];
} Keyword;

typedef union TokenValue
{
enum KeywordCode eKeyword;
unsigned int uiNumber;
char *pcString; 
} TokenValue;

typedef struct Token
{
enum TokenType eType; 
union TokenValue uValue; 
} Token;

extern struct Token asToken[MAX_TOKEN_NR];

extern unsigned char ucTokenCount;

void DecodeMsg(char *pcString);

enum eRecieverStatus {EMPTY, READY, OVERFLOW};
void Reciever_PutCharacterToBuffer(char cCharacter);
enum eRecieverStatus eReciever_GetStatus(void);
void Reciever_GetStringCopy(char *ucDestination);
