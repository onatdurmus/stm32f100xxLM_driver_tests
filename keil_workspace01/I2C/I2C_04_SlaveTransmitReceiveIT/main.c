#include "stm32F100xxLMdensity.h"
#include <string.h>

#define I2C_OWNADDRESS 0x56
#define I2C_SLAVEADDR  0x39
#define I2C_SLAVEMSGLEN 20

#define I2C_MASTERMGSLEN 10

#define GPIO_BUTTON_PRESSED 		RESET//We will pull the pin to ground when it is pressed.
#define GPIO_BUTTON_NOTPRESSED	SET//Since we are using with Pull-Up, pin will be high when it is not pressed.

void GPIOInits(void);
void GPIOButtonInit(void);//We will use a button to start transmission.
void I2CInits(void);

void delay(uint32_t cnt){
	for(;cnt>0;cnt--);
}

uint8_t SlaveOneByteMsg = 'G';
uint8_t SlaveTwoByteMsg[] = "gA";
uint8_t SlaveThreeByteMsg[] = "GaD";
uint8_t SlaveTransmitMsg[] = "How the tables have turned...";

uint8_t SlvOneByteRcvBuf[1];
uint8_t SlvTwoByteRcvBuf[2];
uint8_t SlvThreeByteRcvBuf[3];
uint8_t SlvRcvBuf[50];


I2C_Handle_ts I2C1Handle;

int main(void)
{
	GPIOButtonInit();
	I2CInits();
	//Enable the I2C1 interrupts from the NVIC side.
	NVIC_IRQ_Control(IRQNO_I2C1_EV,ENABLE);
	NVIC_IRQ_Control(IRQNO_I2C1_ER,ENABLE);
		
	while(1)
	{
		while((GPIO_ReadPin(GPIOA,1)));//Wait until button is pressed.
		//Because of pull-up, this will read 1 when button is not pressed and wait. When button is pressed,will be pulled to low and continue.
		
		
		I2C_SlaveReceiveIT(&I2C1Handle,SlvOneByteRcvBuf,1);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		
		I2C_SlaveReceiveIT(&I2C1Handle,SlvTwoByteRcvBuf,2);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		
		I2C_SlaveReceiveIT(&I2C1Handle,SlvThreeByteRcvBuf,3);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		
		I2C_SlaveReceiveIT(&I2C1Handle,SlvRcvBuf,I2C_MASTERMGSLEN);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		
		I2C_SlaveTransmitIT(&I2C1Handle,&SlaveOneByteMsg,1);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		
		I2C_SlaveTransmitIT(&I2C1Handle,SlaveTwoByteMsg,2);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		
		I2C_SlaveTransmitIT(&I2C1Handle,SlaveThreeByteMsg,3);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		
		I2C_SlaveTransmitIT(&I2C1Handle,SlaveTransmitMsg,strlen((char*)SlaveTransmitMsg));
		while(I2C1Handle.TxRxState != I2C_STATE_READY);
		

		for(;;);
	}

	
	return 0;
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

void GPIOButtonInit(void)
{
	GPIO_Handle_ts GPIOButton;
	GPIOButton.pGPIO = GPIOA;
	GPIOButton.GPIO_PinConfig.InterruptType = GPIO_INTERRUPT_NOINT;//We will use polling for the button.
	GPIOButton.GPIO_PinConfig.PinMode = GPIO_MODE_IN_PU;
	GPIOButton.GPIO_PinConfig.PinSpeed = GPIO_SPEED_INPUT;
	GPIOButton.GPIO_PinConfig.PinNo = 1;//We will use PA1 to connect the button.
	GPIO_InitPin(&GPIOButton);
}

void I2C1_EV_IRQHandler(void)
{
	I2C_IRQEventHandler(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_IRQErrorHandler(&I2C1Handle);
}