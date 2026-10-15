//THIS MASTER DEVICE SHOULD RUN AFTER THE SLAVE IS ALREADY RUNNING.
//Since i didnt wait for a button press like my other projects, the master will start the transmit immideately after running.
//Therefore, slave should have already ran the line SPIReceive before the master starts transmitting.

#include "stm32F100xxLMdensity.h"
#include <string.h>
#include <stdio.h>

#define MAX_RCV_LEN									500
#define DATA_SEND_COMPLETE 					5
#define DATA_SEND_INCOMPLETE 				2
#define DATA_RECEIVE_COMPLETE 			6
#define DATA_RECEIVE_INCOMPLETE 		3
#define SLAVE_DATA_READY    				8
#define SLAVE_DATA_NOTREADY					12


SPI_Handle_ts SPI1Handle;

char ReceiveBuffer[MAX_RCV_LEN];
char DatatoSend[]="Your mastef has spoken";//Slave device expects "Your master has spoken"
volatile char DummyReadByte;
volatile char DummyWriteByte;
volatile uint8_t SlaveDataState = SLAVE_DATA_NOTREADY;



void SPIGPIOInits(void);
void SPIInits(void);
void SPISlavePinInterruptInit(void);

volatile uint8_t DataSendComplete = DATA_SEND_INCOMPLETE;
volatile uint8_t DataReceiveComplete = DATA_RECEIVE_INCOMPLETE;


int main(void){
	SPIInits();
	SPI_SSOEControl(SPI1,SPI_SSOE_ENABLE);
	//SPI_SSIControl(SPI1,ENABLE);
	SPI_PeriControl(SPI1,ENABLE);
	SPISlavePinInterruptInit();
	NVIC_IRQ_Control(IRQNO_SPI1,ENABLE);
	
	//while(!(SPI1->SPI_SR & (1 << SPI_SR_TXE)));
	//*(volatile uint8_t *)&SPI1->SPI_DR = 0x00;

	// Lets first send the data
	SPISendData(&SPI1Handle,(uint8_t*)DatatoSend,strlen(DatatoSend));
	//Wait for datasend to be over
	while(DataSendComplete == DATA_SEND_INCOMPLETE);
	//Then wait for the assessment of data from the slave. Slave will let us know via a gpio pin when it is ready to send response.
	while(SlaveDataState == SLAVE_DATA_NOTREADY);
	//Then get the response from the slave.
	SPIReceiveData(&SPI1Handle,(uint8_t*)ReceiveBuffer,21);//Slave will send 21 bytes regardless.
	SPISendData(&SPI1Handle,(uint8_t*)ReceiveBuffer,21);//To generate clock as master, a device must transmit dummy bytes.
	
	while(SPIGetFlag(SPI1,SPI_SR_BSY) == 1);//Wait until SPI is not busy.
	SPI_PeriControl(SPI1,DISABLE);//Close the spi peripheral.
	
	for(;;);
	
}


void SPI1_IRQHandler(void)
{
	SPI_Driver_ISR_Handler(&SPI1Handle);
}

void EXTI4_IRQHandler(void)
{
	SlaveDataState = SLAVE_DATA_READY;
	NVIC_IRQ_Control(IRQNO_EXTI4,DISABLE);
}
void SPIGPIOInits(void)
{	
	
	GPIO_Handle_ts SPIGPIOHandle;
	SPIGPIOHandle.pGPIO = GPIOA;
	SPIGPIOHandle.GPIO_PinConfig.InterruptType = GPIO_INTERRUPT_NOINT;
	SPIGPIOHandle.GPIO_PinConfig.PinSpeed = GPIO_SPEED_MAX10MHZ;
	SPIGPIOHandle.GPIO_PinConfig.PinMode = GPIO_MODE_AFO_PP;
	SPIGPIOHandle.GPIO_PinConfig.PinNo = 5;
	//SPI SCK should be AF PUSH PULL for master mode.
	//SPI1_SCK is PA5(NO REMAP)
	GPIO_InitPin(&SPIGPIOHandle);
	
	//SPI MOSI should be AF Push Pull for Full Duplex master mode.
	//SPI1 MOSI is PA7(NO REMAP)
	SPIGPIOHandle.GPIO_PinConfig.PinNo = 7;
	GPIO_InitPin(&SPIGPIOHandle);
	
	//SPIx_NSS should be AF Push Pull for HW slave management master,NSS Output Enabled(SSOE)
	//SPI1_NSS is PA4(NO REMAP)
	SPIGPIOHandle.GPIO_PinConfig.PinNo = 4;
	GPIO_InitPin(&SPIGPIOHandle);
	
	//SPI MISO should be input floating/input pull up for FD master.
	//SPI1 MISO is PA6 (NO REMAP)
	SPIGPIOHandle.GPIO_PinConfig.PinSpeed = GPIO_SPEED_INPUT;
	SPIGPIOHandle.GPIO_PinConfig.PinMode = GPIO_MODE_IN_PU;
	SPIGPIOHandle.GPIO_PinConfig.PinNo = 6;
	
	GPIO_InitPin(&SPIGPIOHandle);
	

}
void SPIInits(void)
{
	SPIGPIOInits();
	SPI1Handle.pSPIx = SPI1;
	SPI1Handle.RxState = SPI_STATE_FREE;
	SPI1Handle.TxState = SPI_STATE_FREE;
	SPI1Handle.TxLen = 0;
	SPI1Handle.RxLen = 0;
	SPI1Handle.SPIConfig.BusConfig = SPI_BUSCONF_FD;
	SPI1Handle.SPIConfig.BusSpeed = SPI_FPCLK_DIV64;
	SPI1Handle.SPIConfig.CPHA = 0;
	SPI1Handle.SPIConfig.CPOL = 0;
	SPI1Handle.SPIConfig.DeviceMode = SPI_DEVMODE_MASTER;
	SPI1Handle.SPIConfig.SlaveManagement = SPI_SS_HWSM;//Test için degistirdik.
	SPI1Handle.SPIConfig.DFF = SPI_DFF_8BIT;
	SPIInit(&SPI1Handle);
}

void SPIApplicationCallback(SPI_Handle_ts* pSPIHandle, uint8_t SpiEvent)
{	
	(void)pSPIHandle;
	if(SpiEvent == SPI_EVENT_TX_COMPLETE)
	{
		DataSendComplete = DATA_SEND_COMPLETE;
	}
	else if(SpiEvent == SPI_EVENT_RX_COMPLETE)
	{
		DataReceiveComplete = DATA_RECEIVE_COMPLETE;
	}
	else if(SpiEvent == SPI_EVENT_OVERRUNERR)
	{
		
	}
	
}

void SPISlavePinInterruptInit(void)
{
	GPIO_Handle_ts GPIOSlaveInt;
	GPIOSlaveInt.pGPIO = GPIOC;
	GPIOSlaveInt.GPIO_PinConfig.InterruptType = GPIO_INTERRUPT_RT;//Slave will pull the signal high when data is ready.
	GPIOSlaveInt.GPIO_PinConfig.PinMode = GPIO_MODE_IN_PD;
	GPIOSlaveInt.GPIO_PinConfig.PinSpeed = GPIO_SPEED_INPUT;
	GPIOSlaveInt.GPIO_PinConfig.PinNo = 4;//We will use PC4 for interrupt
	GPIO_InitPin(&GPIOSlaveInt);
}