#include "stm32F100xxLMdensity.h"
#include "stm32f100xxLM_gpio_driver.h"


void GPIO_ClockControl(GPIO_RegDef_ts* pGPIOx, uint8_t EnorDi){
	
	if(pGPIOx == GPIOA){
		if(EnorDi == ENABLE){
			GPIOA_PCLK_EN();}
		else if(EnorDi == DISABLE){
			GPIOA_PCLK_DI();}
		}
	else if(pGPIOx == GPIOB){
		if(EnorDi == ENABLE){
			GPIOB_PCLK_EN();}
		else if(EnorDi == DISABLE){
			GPIOB_PCLK_DI();}
		}
	else if(pGPIOx == GPIOC){
		if(EnorDi == ENABLE){
			GPIOC_PCLK_EN();}
		else if(EnorDi == DISABLE){
			GPIOC_PCLK_DI();}
		}
	else if(pGPIOx == GPIOD){
		if(EnorDi == ENABLE){
			GPIOD_PCLK_EN();}
		else if(EnorDi == DISABLE){
			GPIOD_PCLK_DI();}
		}
	else if(pGPIOx == GPIOE){
		if(EnorDi == ENABLE){
			GPIOE_PCLK_EN();}
		else if(EnorDi == DISABLE){
			GPIOE_PCLK_DI();}
		}
	
}

uint8_t GPIO_EXTILineToIRQNum(uint8_t ExtiNum){
	uint8_t IRQNo;
	switch(ExtiNum){
		case 0:
			IRQNo = IRQNO_EXTI0;
			break;
		case 1:
			IRQNo = IRQNO_EXTI1;
			break;
		case 2:
			IRQNo = IRQNO_EXTI2;
			break;
		case 3:
			IRQNo = IRQNO_EXTI3;
			break;
		case 4:
			IRQNo = IRQNO_EXTI4;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			IRQNo = IRQNO_EXTI5TO9;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			IRQNo = IRQNO_EXTI10TO15;
			break;
		default:
			IRQNo = 0xFF;
			break;
	}
	return IRQNo;
			
}

void NVIC_IRQ_Control(uint8_t IRQNo, uint8_t EnorDi){
	if(IRQNo < 61){// We have 60 IRQ numbers on our board, shouldnt do anything otherwise. Can return error add later
		if(IRQNo >= 32){//IRQ Number is greater than 32, we must change NVIC_ISER1 register.
			IRQNo %= 32;
			if(EnorDi == ENABLE){
				*NVIC_ISER1 |= (1<<IRQNo);
			}
			else if(EnorDi == DISABLE){
				*NVIC_ICER1 |= (1<<IRQNo);
			}
		}
		else{
			if(EnorDi == ENABLE){
				*NVIC_ISER0 |= (1<<IRQNo);
			}
			else if(EnorDi == DISABLE){
				*NVIC_ICER0 |= (1<<IRQNo);
			}		
		}
	}	
}

void EXTI_Line_Config(uint8_t PortCode, uint8_t LineNum,uint8_t IntType ,uint8_t EnorDi){
	if(EnorDi == DISABLE){
		EXTI->EXTI_IMR &= ~(1<<LineNum);
	}
	else if(EnorDi == ENABLE){
		AFIO_PCLK_EN();
			uint8_t Exti_Reg = LineNum/4;
			uint8_t ShiftCount = ((LineNum%4)*4); // Avoid second multiplication
			
			switch(Exti_Reg){
				case 0:
					AFIO->AFIO_EXTICR1 &= ~(0x0F<<ShiftCount); // Clear EXTI bits
					AFIO->AFIO_EXTICR1 |= (PortCode<<ShiftCount);//Write the Port Code
					break;
				case 1:
					AFIO->AFIO_EXTICR2 &= ~(0x0F<<ShiftCount); // Clear EXTI bits
					AFIO->AFIO_EXTICR2 |= (PortCode<<ShiftCount);
					break;
				case 2:
					AFIO->AFIO_EXTICR3 &= ~(0x0F<<ShiftCount); // Clear EXTI bits
					AFIO->AFIO_EXTICR3 |= (PortCode<<ShiftCount);
					break;
				case 3:
					AFIO->AFIO_EXTICR4 &= ~(0x0F<<ShiftCount); // Clear EXTI bits
					AFIO->AFIO_EXTICR4 |= (PortCode<<ShiftCount);
					break;
				default:
					break;
						
		}
			
			EXTI->EXTI_IMR |= (1<<LineNum);			//Disables interrupt masking on EXTI line x
			if(IntType == GPIO_INTERRUPT_FT)
			{
				EXTI->EXTI_FTSR |= (1<<LineNum);//Set falling trigger
				EXTI->EXTI_RTSR &= ~(1<<LineNum);//Disable rising trigger
			}
			else if(IntType == GPIO_INTERRUPT_RT)
			{
				EXTI->EXTI_RTSR |= (1<<LineNum);//Set rising trigger
				EXTI->EXTI_FTSR &= ~(1<<LineNum);//Disable falling trigger
			}
			else if(IntType == GPIO_INTERRUPT_FRT)
			{
				EXTI->EXTI_RTSR |= (1<<LineNum);//Set falling and rising triggers.
				EXTI->EXTI_FTSR |= (1<<LineNum);
			}
		}
}

void GPIO_InitPin(GPIO_Handle_ts* pGPIOHandle){
	
	GPIO_ClockControl(pGPIOHandle->pGPIO,ENABLE);
	uint8_t PinNumber = pGPIOHandle->GPIO_PinConfig.PinNo;
	uint8_t PinNoDiv8 = PinNumber/8; // To avoid performing two divisions and not configuring if PinNo is greater than 15.
	uint8_t PinModevar = pGPIOHandle->GPIO_PinConfig.PinMode; // For cleaner code
	if(PinNoDiv8 == 1){		//PinNo is greater than 8, CRH registers should be configured.
		PinNumber = PinNumber%8;
		pGPIOHandle->pGPIO->GPIO_CRH &= ~(0x0F<<(PinNumber*4)); //Clear the MODE[1:0] and CNF[1:0] bits in the register. @GPIO_PORTCONFREG
		pGPIOHandle->pGPIO->GPIO_CRH |= (pGPIOHandle->GPIO_PinConfig.PinSpeed<<(PinNumber*4)); //Set the MODE[1:0] bits.
		pGPIOHandle->pGPIO->GPIO_CRH |= (PinModevar<<((PinNumber*4)+2)); // Set the CONF[1:0] bits.
	}
	else if(PinNoDiv8 == 0){
		pGPIOHandle->pGPIO->GPIO_CRL &= ~(0x0F<<(PinNumber*4)); //Clear the MODE[1:0] and CNF[1:0] bits in the register. @GPIO_PORTCONFREG
		pGPIOHandle->pGPIO->GPIO_CRL |= (pGPIOHandle->GPIO_PinConfig.PinSpeed<<(PinNumber*4)); // Set the CONF[1:0] bits.
		pGPIOHandle->pGPIO->GPIO_CRL |= (PinModevar<<((PinNumber*4)+2)); //	Set the MODE[1:0] bits.	
	}

	if(PinModevar == GPIO_MODE_IN_PU){// Additional Configurations need to be done to activate Pull Up and Pull Down Resistors.
		pGPIOHandle->pGPIO->GPIO_ODR |= (1<<pGPIOHandle->GPIO_PinConfig.PinNo); // For Pull Up, Must be set to 1
	}
	else if(PinModevar == GPIO_MODE_IN_PD){						
		pGPIOHandle->pGPIO->GPIO_ODR &= ~(1<<pGPIOHandle->GPIO_PinConfig.PinNo);// For Pull Down, Must be 0
	}
	
	//Write Interrupt Code Here
	
	
	if((pGPIOHandle->GPIO_PinConfig.InterruptType != GPIO_INTERRUPT_NOINT)&&
		((PinModevar == GPIO_MODE_IN_FLT)||
		 (PinModevar == GPIO_MODE_IN_PU)||
		 (PinModevar == GPIO_MODE_IN_PD)))// Set Up Interrupt on Pin only if interrupt type is selected and pin is in Input Mode(Necessity)
	{
		uint8_t PortExtiNum = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIO);//Each port has a code that needs to be written to exti register to be selected.
		EXTI_Line_Config(PortExtiNum,pGPIOHandle->GPIO_PinConfig.PinNo,pGPIOHandle->GPIO_PinConfig.InterruptType,ENABLE);
		uint8_t IRQNumber = GPIO_EXTILineToIRQNum(pGPIOHandle->GPIO_PinConfig.PinNo);
		NVIC_IRQ_Control(IRQNumber,ENABLE);
	}
}

void GPIO_WriteToPort(GPIO_RegDef_ts* pGPIOx,uint16_t Value){
	pGPIOx->GPIO_ODR = Value;
}

void GPIO_WriteToPin(GPIO_RegDef_ts* pGPIOx,uint8_t PinNo,uint8_t Value){
	if(Value == GPIO_PIN_SET){
		pGPIOx->GPIO_ODR |= (1<<PinNo);
	}
	else if(Value == GPIO_PIN_RESET){
		pGPIOx->GPIO_ODR &= ~(1<<PinNo);
	}
}

uint8_t GPIO_ReadPin(GPIO_RegDef_ts* pGPIOx,uint8_t PinNo){
	return (uint8_t)((pGPIOx->GPIO_IDR & (1<<PinNo))>>PinNo);//First mask the corresponding pin, then right shift to 0th bit so it doesnt get truncated.
}
uint16_t GPIO_ReadPort(GPIO_RegDef_ts* pGPIOx){
	return pGPIOx->GPIO_IDR;
}
void GPIO_TogglePin(GPIO_RegDef_ts* pGPIOx, uint8_t PinNo){
	pGPIOx->GPIO_ODR ^= (1<<PinNo);
}