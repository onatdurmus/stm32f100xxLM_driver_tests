//If this device is not ran until GPIOA readpin command before F407Disc starts receiving, 
//the first byte received from the F407 is garbage, and the messages shift one byte.
//Das heisst,The first byte of second message is the last byte of the first, and one byte is lost from both messages.

#include "stm32F100xxLMdensity.h"
#include "string.h"

#define USER_BUTTON_PRESSED 		GPIO_PIN_SET
#define USER_BUTTON_NOTPRESSED 	GPIO_PIN_RESET

#define FIRST_RCV_MSGLEN	20
#define SCND_RCV_MSGLEN		15

void GPIOButtonInit(void);//GPIO PA0 is connected to the onboard user button.
void GPIOInits(void);//PA9 will be USART TX, PA10 will be USART RX
void UARTInits(void);

uint8_t msg1[] = "Hello from an old man";
uint8_t msg2[] = "I am not dead yet";

uint8_t rcvbuf1[50];
uint8_t rcvbuf2[50];

USART_Handle_ts usart1;

int main(void)
{
	GPIOInits();
	UARTInits();
	
	while(!(GPIO_ReadPin(GPIOA,0)));//Wait until button is pressed, pressed = 1
	
	USART_SendBlocking(usart1.USARTx,msg1,strlen((char*)msg1));
	
	USART_ReceiveBlocking(usart1.USARTx,rcvbuf1,FIRST_RCV_MSGLEN);
	
	USART_ReceiveBlocking(usart1.USARTx,rcvbuf2,SCND_RCV_MSGLEN);
	
	USART_SendBlocking(usart1.USARTx,msg2,strlen((char*)msg2));
	
	for(;;);
}

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
}