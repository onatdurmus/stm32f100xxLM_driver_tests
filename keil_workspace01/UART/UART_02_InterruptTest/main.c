//#define CHECK_CONCURRENCY//This is defined to halt the program at a point and check if rx and tx are being done concurrently

#include "stm32F100xxLMdensity.h"
#include "string.h"

#define USER_BUTTON_PRESSED 		GPIO_PIN_SET
#define USER_BUTTON_NOTPRESSED 	GPIO_PIN_RESET

#define FIRST_RCV_MSGLEN	20
#define SCND_RCV_MSGLEN		15

void GPIOButtonInit(void);//GPIO PA0 is connected to the onboard user button.
void GPIOInits(void);
void UARTInits(void);

uint8_t msg1[] = "Hello from an old man";
uint8_t msg2[] = "I am not dead yet";

uint8_t rcvbuf1[50];
uint8_t rcvbuf2[50];

USART_Handle_ts usart1;

uint8_t RxComplete = RESET;
uint8_t TxComplete = RESET;

int main(void)
{
	GPIOInits();
	UARTInits();
	
	USART_ReceiveIT(&usart1,rcvbuf1,FIRST_RCV_MSGLEN);//Can start sampling on Rx before the button is pressed.
	
	while(!(GPIO_ReadPin(GPIOA,0)));//Wait until button is pressed, pressed = 1
		
	USART_TransmitIT(&usart1,msg1,strlen((char*)msg1));
	
	while(TxComplete!=SET);
	while(RxComplete!=SET);
	
	RxComplete=RESET;
	TxComplete=RESET;
	
	while(GPIO_ReadPin(GPIOA,0));//The button might be not released until all the com is done, so lets wait for the button to be released.PA0=1
	//This line will wait when PA0==1
	
	while(!(GPIO_ReadPin(GPIOA,0)));//Lets initiate the second transfer also with the button.
	//This line will wait until PA0==1
	
	USART_ReceiveIT(&usart1,rcvbuf2,SCND_RCV_MSGLEN);
	
	USART_TransmitIT(&usart1,msg2,strlen((char*)msg2));
	
	while(RxComplete!=SET)
#ifdef CHECK_CONCURRENCY		
	{
		if(usart1.RxLen==5)
		{
			break;//We can use a breakpoint here to see if the communication was indeed carried out concurrently.
			//Check the Tx state on this breakpoint.
			//At this point, Tx is also almost done. Fast fertig wie ich.			
		}
	}
#endif	
	;
	while(TxComplete!=SET);
	
	for(;;);
};

void GPIOButtonInit(void)
{
	GPIO_PinConfig_ts userbuttoncnf;
	userbuttoncnf.InterruptType = GPIO_INTERRUPT_NOINT;
	userbuttoncnf.PinMode = GPIO_MODE_IN_FLT;//This user button already has a pull-down resistor.
	userbuttoncnf.PinSpeed = GPIO_SPEED_INPUT;
	userbuttoncnf.PinNo = 0;
	
	GPIO_Handle_ts userbuttonhandle;
	userbuttonhandle.GPIO_PinConfig = userbuttoncnf;
	userbuttonhandle.pGPIO = GPIOA;
	
	GPIO_InitPin(&userbuttonhandle);
}

void GPIOInits(void)
{
	GPIOButtonInit();
	
	GPIO_PinConfig_ts usartgpios;
	usartgpios.InterruptType = GPIO_INTERRUPT_NOINT;
	usartgpios.PinSpeed	= GPIO_SPEED_MAX2MHZ;
	usartgpios.PinMode = GPIO_MODE_AFO_PP;
	usartgpios.PinNo = 9;
	
	GPIO_Handle_ts usartgpiohandle;
	usartgpiohandle.GPIO_PinConfig = usartgpios;
	usartgpiohandle.pGPIO = GPIOA;
	
	GPIO_InitPin(&usartgpiohandle);
	
	usartgpios.PinMode = GPIO_MODE_IN_PU;
	usartgpios.PinSpeed = GPIO_SPEED_INPUT;
	usartgpios.PinNo = 10;
	
	GPIO_InitPin(&usartgpiohandle);
}

void UARTInits(void)
{
	USART_Config_ts USART1Config;
	USART1Config.HWControl = USART_HWCNTRL_DI;
	USART1Config.ParityMode = USART_PRTY_DI;
	USART1Config.StopBits = USART_STOP_1;
	USART1Config.WordLen = USART_WORDLEN_8;
	USART1Config.UsartMode = USART_MODE_TXRX;
	USART1Config.BaudRate = USART_STD_BAUD_115200;
	
	usart1.USARTConfig = &USART1Config;
	usart1.RxState = USART_STATE_READY;
	usart1.TxState = USART_STATE_READY;
	
	usart1.USARTx = USART1;
	
	USART_Init(&usart1);
	NVIC_IRQ_Control(IRQNO_USART1,ENABLE);
}

void USART_ApplicationCallback(USART_Handle_ts* pUSARTHandle, uint8_t Event)
{
	if(Event == USART_EV_TXCMPLT)
	{
		TxComplete = SET;
	}
	if(Event == USART_EV_RXCMPLT)
	{
		RxComplete = SET;
	}
	(void)pUSARTHandle;
}

void USART1_IRQHandler(void)
{
	USART_IRQHandler(&usart1);
}