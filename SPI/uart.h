#define TRANSMITER_SIZE 64

void UART_InitWithInt(unsigned int uiBaudRate);

typedef enum eTransmiterStatus {FREE, BUSY} eTransmiterStatus;

char Transmiter_GetCharacterFromBuffer(void);
void Transmiter_SendString(char cString[]);
enum eTransmiterStatus Transmiter_GetStatus(void);
