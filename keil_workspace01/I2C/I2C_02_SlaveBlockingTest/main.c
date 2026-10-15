#include "stm32F100xxLMdensity.h"
#include <string.h>

#define I2C_OWNADDRESS 30
#define I2C_SLAVEADDR  0x39
#define I2C_SLAVEMSGLEN 20
#define I2C_MSTR_MSGLEN 25
#define I2C_MSTR_SCNDMSGLEN		40

#define GPIO_BUTTON_PRESSED 		RESET//We will pull the pin to ground when it is pressed.
#define GPIO_BUTTON_NOTPRESSED	SET//Since we are using with Pull-Up, pin will be high when it is not pressed.

uint8_t SlaveTransmitMsg[]="I2C Slave Talking...\n";
uint8_t SlaveTransmitScndMsg[]="Thanks Tayyar. I2C Slave Talking Again...\n";
uint8_t SlaveRcvBuf[70];
uint8_t SecondRcvBuf[50];


void GPIOInits(void);
void I2CInits(void);

I2C_Handle_ts I2C1Handle;

int main(void){
	GPIOInits();
	I2CInits();
	I2C_SlaveTransmitBlocking(&I2C1Handle,SlaveTransmitMsg,strlen((char*)SlaveTransmitMsg));
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until I2C state is ready.
	I2C_SlaveReceiveBlocking(&I2C1Handle,SlaveRcvBuf,I2C_MSTR_MSGLEN);
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until I2C state is ready.
	I2C_SlaveReceiveBlocking(&I2C1Handle,SecondRcvBuf,I2C_MSTR_SCNDMSGLEN);//Receive the second message.
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until I2C state is ready.
	I2C_SlaveTransmitBlocking(&I2C1Handle,SlaveTransmitScndMsg,strlen((char*)SlaveTransmitScndMsg));
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until I2C state is ready.
	for(;;);
	
}

void GPIOInits(void)
{
	GPIO_Handle_ts gpioi2c;
	
	//PB6 will be I2C1_SCL
	//PB7 will be I2C1_SDA
	//They both need to be configured as Alternate Function Open Drain.
	
	gpioi2c.pGPIO = GPIOB;
	gpioi2c.GPIO_PinConfig.InterruptType = GPIO_INTERRUPT_NOINT;
	gpioi2c.GPIO_PinConfig.PinMode = GPIO_MODE_AFO_OD;
	gpioi2c.GPIO_PinConfig.PinSpeed = GPIO_SPEED_MAX2MHZ;
	gpioi2c.GPIO_PinConfig.PinNo = 6;
	
	GPIO_InitPin(&gpioi2c);
	
	gpioi2c.GPIO_PinConfig.PinNo = 7;
	
	GPIO_InitPin(&gpioi2c);
	
}

void I2CInits(void)
{
	GPIOInits();
	I2C1Handle.I2CConfig.Acking = I2C_ACKING_ENABLED;
	I2C1Handle.I2CConfig.ClockStretching = I2C_CLKSTRETCH_ENABLED;
	I2C1Handle.I2CConfig.DutyCycle = I2C_FM_DUTY2;//This is dont care, we will use i2c in standard mode.
	I2C1Handle.I2CConfig.OwnAddress = I2C_OWNADDRESS;
	I2C1Handle.I2CConfig.SCLSpeed = 100;//100 Kbit speed
	I2C1Handle.pI2C = I2C1;
	
	I2C1Handle.SlaveAddr = I2C_SLAVEADDR;
	I2C1Handle.TxRxState = I2C_STATE_READY;
	
	I2C_Init(&I2C1Handle);
}
