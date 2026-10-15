#include "stm32F100xxLMdensity.h"
#include <string.h>

#define I2C_OWNADDRESS 0x56
#define I2C_SLAVEADDR  0x39
#define I2C_SLAVEMSGLEN 20

#define GPIO_BUTTON_PRESSED 		RESET//We will pull the pin to ground when it is pressed.
#define GPIO_BUTTON_NOTPRESSED	SET//Since we are using with Pull-Up, pin will be high when it is not pressed.

void GPIOInits(void);
void GPIOButtonInit(void);//We will use a button to start transmission.
void I2CInits(void);

void delay(uint32_t cnt){
	for(;cnt>0;cnt--);
}

uint8_t MasterOneByteMsg = 'T';
uint8_t MasterTwoByteMsg[] = "TO";
uint8_t MasterThreeByteMsg[] = "TOD";
uint8_t MasterTransmitMsg[] = "This is your I2C Master Tayyar";
uint8_t OneByteRcvBuf[1];
uint8_t TwoByteRcvBuf[2];
uint8_t ThreeByteRcvBuf[3];

uint8_t RcvBuf[50];

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
		
		I2C_MasterReceiveIT(&I2C1Handle,OneByteRcvBuf,1,I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still receiving.
		delay(10000);//Small delay so the slave sets up buffers for the next transfer.
		
		I2C_MasterReceiveIT(&I2C1Handle,TwoByteRcvBuf,2,I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still receiving.
		delay(10000);//Small delay so the slave sets up buffers for the next transfer.
		
		I2C_MasterReceiveIT(&I2C1Handle,ThreeByteRcvBuf,3,I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still receiving.
		delay(10000);//Small delay so the slave sets up buffers for the next transfer.
		
		I2C_MasterReceiveIT(&I2C1Handle,RcvBuf,I2C_SLAVEMSGLEN,I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still receiving.
		delay(10000);//Small delay so the slave sets up buffers for the next transfer.
		
		I2C_MasterTransmitIT(&I2C1Handle,&MasterOneByteMsg,1,I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still transmitting.
		delay(400000);//Delay is to wait for the slave to finish printing.
		
		I2C_MasterTransmitIT(&I2C1Handle,MasterTwoByteMsg,2,I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still transmitting.
		delay(400000);//Delay is to wait for the slave to finish printing.
		
		I2C_MasterTransmitIT(&I2C1Handle,MasterThreeByteMsg,3,I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still transmitting.
		delay(400000);//Delay is to wait for the slave to finish printing.
		
		I2C_MasterTransmitIT(&I2C1Handle,MasterTransmitMsg,strlen((char*)MasterTransmitMsg),I2C_SLAVEADDR,DISABLE);
		while(I2C1Handle.TxRxState != I2C_STATE_READY);//Wait while I2C is still transmitting.
		delay(400000);//Delay is to wait for the slave to finish printing.
		
		
		
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