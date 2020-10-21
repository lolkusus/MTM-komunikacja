#include <LPC213X.H>

//PINSEL0
#define SCL_MASK (1<<4)
#define SDA_MASK (1<<6)

#define IIC_MASK 0x00F0
#define IIC_CONFIG (SCL_MASK | SDA_MASK)

//I2C0CONSET/CLR 
#define I2EN_MASK (1<<6) //IIC Enable
#define STA_MASK (1<<5) //Start flag
#define STO_MASK (1<<4) //Stop flag  - nie wolno clearowac! - tabela 104 str 173
#define SI_MASK (1<<3) //Interupt flag
#define AA_MASK (1<<2) //Assert Ack

//I2C0STAT
#define IIC_STATUS_8 	0x08	//Start condition transmitted
#define IIC_STATUS_18 0x18	//Slave address ACK
#define IIC_STATUS_28 0x28	//Slave data ACK

//VIC
#define VIC_IIC_CHANNER_NR 9
#define VIC_IRQ_SLOT_ENABLE (1<<5) //VICVectCntl

void ISR_Start_Transmitted()
{
	I2C0DAT = 0x40; //address
	I2C0CONCLR = (STA_MASK | SI_MASK); //wyczyscic flage przerwania i flage startu
}

void ISR_Slave_Addr_Ack()
{
	I2C0DAT = 0x69; //dane
	I2C0CONCLR = SI_MASK; //czysc tylko flage przerwania
}

void ISR_Slave_Data_Ack()
{
	I2C0CONSET = (I2EN_MASK | STO_MASK);
}

__irq void I2C_Interrupt()
{
	switch (I2C0STAT)
	{
		case IIC_STATUS_8:
			ISR_Start_Transmitted();
			break;
		
		case IIC_STATUS_18:
			ISR_Slave_Addr_Ack();
			break;
		
		case IIC_STATUS_28:
			ISR_Slave_Data_Ack();
			break;
		
		default:
			break;
	}
	VICVectAddr=0x00; //strona 102 sugeruje zerowanie tej wartosci pod koniec kazdego isr
}

void I2C_Init(void)
{
	PINSEL0 = ((PINSEL0 & ~IIC_MASK) | IIC_CONFIG);
	
	I2C0CONCLR = (I2EN_MASK | STA_MASK | SI_MASK | AA_MASK); 
	I2C0CONSET = I2EN_MASK; 
	I2C0SCLH = 0x80; 		//dane z instrukcji do zadania
	I2C0SCLL = 0x80; 		//dane z instrukcji do zadania

	VICIntEnable |= (1<<VIC_IIC_CHANNER_NR);
	VICVectCntl0 = (VIC_IRQ_SLOT_ENABLE | VIC_IIC_CHANNER_NR); 
	VICVectAddr0  =(unsigned long) I2C_Interrupt;
}



int main()
{
	I2C_Init();
	
	I2C0CONSET = (I2EN_MASK | STA_MASK);
	
	while(1){}
}
