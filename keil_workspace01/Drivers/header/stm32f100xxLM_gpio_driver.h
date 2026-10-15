#ifndef STM32F100LM_GPIO_DRIVER_H
#define STM32F100LM_GPIO_DRIVER_H

#include "stm32F100xxLMdensity.h"

typedef struct
{
	uint8_t PinNo;
	uint8_t	PinMode;					//@GPIO_PIN_MODE
	uint8_t PinSpeed;					//@GPIO_PIN_SPEED
	uint8_t InterruptType;		//@GPIO_INTERRUPT_TYPE
	
}GPIO_PinConfig_ts;

typedef struct
{
	GPIO_RegDef_ts* pGPIO;
	GPIO_PinConfig_ts GPIO_PinConfig;
}GPIO_Handle_ts;


/*
	@GPIO_PIN_SPEED
	GPIO Possible Pin Speed Options. No Configuration for input.
*/

#define GPIO_SPEED_INPUT				0x00		//This is reserved for input. Can not be configured when in output mode.
#define GPIO_SPEED_MAX10MHZ			0x01
#define GPIO_SPEED_MAX2MHZ			0x02
#define GPIO_SPEED_MAX50MHZ			0x03


/*
	@GPIO_PIN_MODE
	GPIO Pin Modes Including Output,Input,Analog and Alternate Function
*/
#define GPIO_MODE_GPO_PP				0x00		//General Purpose Output, Push-Pull
#define GPIO_MODE_GPO_OD				0x01		//General Purpose Output, Open Drain
#define GPIO_MODE_AFO_PP				0x02		//Alternate Function Output, Push-Pull
#define GPIO_MODE_AFO_OD				0x03		//Alternate Function Output, Open Drain
#define GPIO_MODE_IN_ANG				0x00		//Input Analog
#define GPIO_MODE_IN_FLT				0x01		//Input Floating
#define GPIO_MODE_IN_PU					0x02		//Input with Pull-Up
#define GPIO_MODE_IN_PD					0x02		//Input with Pull-Down

/*
	@GPIO_INTERRUPT_TYPE
	Defines Falling Trigger,Rising Trigger and Rising-Falling Trigger Interrupts
*/

#define GPIO_INTERRUPT_NOINT		0x00
#define GPIO_INTERRUPT_FT				0x01
#define GPIO_INTERRUPT_RT				0x02
#define GPIO_INTERRUPT_FRT			0x03



void GPIO_ClockControl(GPIO_RegDef_ts* pGPIOx, uint8_t EnorDi);
uint8_t GPIO_EXTILineToIRQNum(uint8_t ExtiNum);
void EXTI_Line_Config(uint8_t PortCode, uint8_t LineNum,uint8_t IntType ,uint8_t EnorDi);
void NVIC_IRQ_Control(uint8_t IRQNo, uint8_t EnorDi);
void GPIO_InitPin(GPIO_Handle_ts* pGPIOHandle);


void GPIO_WriteToPort(GPIO_RegDef_ts* pGPIOx,uint16_t Value);
void GPIO_WriteToPin(GPIO_RegDef_ts* pGPIOx,uint8_t PinNo,uint8_t Value);
uint8_t GPIO_ReadPin(GPIO_RegDef_ts* pGPIOx,uint8_t PinNo);
uint16_t GPIO_ReadPort(GPIO_RegDef_ts* pGPIOx);
void GPIO_TogglePin(GPIO_RegDef_ts* pGPIOx, uint8_t PinNo);


#endif