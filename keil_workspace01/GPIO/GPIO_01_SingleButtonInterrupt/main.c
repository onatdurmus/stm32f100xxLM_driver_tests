#include "stm32F100xxLMdensity.h"

int main(void){
	
	GPIO_Handle_ts GpioOnBoardLed;
	
	GpioOnBoardLed.pGPIO = GPIOC;// On Board Leds are connected to PC8 and PC9
	GpioOnBoardLed.GPIO_PinConfig.InterruptType = GPIO_INTERRUPT_NOINT;
	GpioOnBoardLed.GPIO_PinConfig.PinMode = GPIO_MODE_GPO_PP;//Configure as Output Push Pull
	GpioOnBoardLed.GPIO_PinConfig.PinNo = 8;
	GpioOnBoardLed.GPIO_PinConfig.PinSpeed = GPIO_SPEED_MAX2MHZ;
	
	GPIO_InitPin(&GpioOnBoardLed);
	
	GpioOnBoardLed.GPIO_PinConfig.PinNo = 9;//Configure PC9 also.
	GPIO_InitPin(&GpioOnBoardLed);
	
	GPIO_Handle_ts GPIOButton;
	GPIOButton.pGPIO = GPIOA;
	GPIOButton.GPIO_PinConfig.InterruptType = GPIO_INTERRUPT_FRT;//I want interrupts both when button is pressed and released.
	GPIOButton.GPIO_PinConfig.PinMode = GPIO_MODE_IN_PU;
	GPIOButton.GPIO_PinConfig.PinSpeed = GPIO_SPEED_INPUT;
	GPIOButton.GPIO_PinConfig.PinNo = 1;//We will use PA1 to connect the button.
	GPIO_InitPin(&GPIOButton);					
	
	for(;;);
	
}

void EXTI1_IRQHandler(void){
	if(EXTI->EXTI_PR&(1<<1)){//The interrupt has happened. Now we have to clear the pending bit. IF it is not 0:
		EXTI->EXTI_PR |= (1<<1);//This bit is cleared by writing "1" into the bit.
	}
	GPIO_TogglePin(GPIOC,8);
	GPIO_TogglePin(GPIOC,9);	
}