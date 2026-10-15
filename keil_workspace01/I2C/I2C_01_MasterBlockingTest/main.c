/*THIS IS THE TESTING FOR I2C MASTER BLOCKING FUNCTIONS.*/


#include "stm32F100xxLMdensity.h"
#include <string.h>

#define I2C_OWNADDRESS 0x56
#define I2C_SLAVEADDR  0x39
#define I2C_SLAVEMSGLEN 20

#define GPIO_BUTTON_PRESSED 		RESET//We will pull the pin to ground when it is pressed.
#define GPIO_BUTTON_NOTPRESSED	SET//Since we are using with Pull-Up, pin will be high when it is not pressed.

uint8_t firstMessage[]="Transmitting through I2C...\n";
uint8_t MsgRcv[70];
uint8_t OneByteReceive[1];
uint8_t TwoByteReceive[2];
uint8_t secondMessage[]="Transmitting the second message\n";


void GPIOInits(void);
void I2CInits(void);
void GPIOButtonInit(void);//We will use a button to start transmission.

void delay(uint32_t cnt){
	for(;cnt>0;cnt--);
}

I2C_Handle_ts I2C1Handle;
	
int main(void){
	
	GPIOButtonInit();
	
	I2CInits();
	
	while(1){
	while((GPIO_ReadPin(GPIOA,1)));//Wait until button is pressed.
	//Because of pull-up, this will read 1 when button is not pressed and wait. When button is pressed,will be pulled to low and continue.
	I2C_MasterTransmitBlocking(&I2C1Handle,firstMessage,strlen((char*)firstMessage),I2C_SLAVEADDR,I2C_RS_FALSE);
	delay(4000000);//Wait while the slave prints the first message transmitted and gets ready to transmit its message.
	I2C_MasterReceiveBlocking(&I2C1Handle,OneByteReceive,1,I2C_SLAVEADDR,I2C_RS_FALSE);
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until the receive is finished.
	I2C_MasterReceiveBlocking(&I2C1Handle,TwoByteReceive,2,I2C_SLAVEADDR,I2C_RS_FALSE);
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until the receive is finished.
	I2C_MasterReceiveBlocking(&I2C1Handle,MsgRcv,I2C_SLAVEMSGLEN,I2C_SLAVEADDR,I2C_RS_FALSE);
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until the second receive is finished.
	I2C_MasterTransmitBlocking(&I2C1Handle,secondMessage,strlen((char*)secondMessage),I2C_SLAVEADDR,I2C_RS_FALSE);
	while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait until state is ready again
	while(I2C1Handle.pI2C->I2C_SR2 & (1<<I2C_SR2_BSY));//Wait until transmission is finished(BSY flag down)
	break;
	}
	for(;;);//Check the memory values here.
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

void GPIOButtonInit(void){
	GPIO_Handle_ts GPIOButton;
	GPIOButton.pGPIO = GPIOA;
	GPIOButton.GPIO_PinConfig.InterruptType = GPIO_INTERRUPT_NOINT;//I want interrupts both when button is pressed and released.
	GPIOButton.GPIO_PinConfig.PinMode = GPIO_MODE_IN_PU;
	GPIOButton.GPIO_PinConfig.PinSpeed = GPIO_SPEED_INPUT;
	GPIOButton.GPIO_PinConfig.PinNo = 1;//We will use PA1 to connect the button.
	GPIO_InitPin(&GPIOButton);
}