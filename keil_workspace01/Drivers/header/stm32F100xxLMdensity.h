#ifndef STM32F100xx_LM_DENS_H
#define STM32F100xx_LM_DENS_H

#include <stdint.h>
#include <stddef.h>


#define FLASH_BASEADDR				0x08000000U
#define SRAM_BASEADDR					0x20000000U
#define SYSMEM_BASEADDR				0x1FFFF000U


#define APB1_PERIPH_BASEADDR 	0x40000000U
#define APB2_PERIPH_BASEADDR 	0x40010000U
#define AHB_PERIPH_BASEADDR 	0x40020000U


/* APB1 BUS PERIPHERAL MACROS*/
#define TIM2_REG_BASEADDR			0x40000000U
#define TIM3_REG_BASEADDR			0x40000400U
#define TIM4_REG_BASEADDR			0x40000800U
#define TIM6_REG_BASEADDR			0x40001000U
#define TIM7_REG_BASEADDR			0x40001400U
#define RTC_REG_BASEADDR			0x40002800U
#define WWDG_REG_BASEADDR 		0x40002C00U			//Window Watchdog Reg
#define IWDG_REG_BASEADDR 		0x40003000U			//Independent Watchdog Reg
#define SPI2_REG_BASEADDR			0x40003800U
#define USART2_REG_BASEADDR		0x40004400U
#define USART3_REG_BASEADDR		0x40004800U
#define I2C1_REG_BASEADDR			0x40005400U
#define I2C2_REG_BASEADDR			0x40005800U
#define BKP_REG_BASEADDR			0x40006C00U 		//Backup Registers
#define PWR_REG_BASEADDR			0x40007000U 		//Power Control PWR
#define DAC_REG_BASEADDR			0x40007400U
#define CEC_REG_BASEADDR			0x40007800U
/* APB1 BUS PERIPHERAL MACROS*/	


/* APB2 BUS PERIPHERAL MACROS*/
#define AFIO_REG_BASEADDR			0x40010000U
#define EXTI_REG_BASEADDR			0x40010400U
#define GPIOA_REG_BASEADDR		0x40010800U
#define GPIOB_REG_BASEADDR		0x40010C00U
#define GPIOC_REG_BASEADDR		0x40011000U
#define GPIOD_REG_BASEADDR		0x40011400U
#define GPIOE_REG_BASEADDR		0x40011800U
#define ADC1_REG_BASEADDR			0x40012400U
#define TIM1_REG_BASEADDR			0x40012C00U
#define SPI1_REG_BASEADDR			0x40013000U
#define USART1_REG_BASEADDR		0x40013800U
#define TIM15_REG_BASEADDR		0x40014000U
#define TIM16_REG_BASEADDR		0x40014400U
#define TIM17_REG_BASEADDR		0x40014800U
/* APB2 BUS PERIPHERAL MACROS*/

/* AHB BUS PERIPHERAL MACROS*/
#define DMA1_REG_BASEADDR			0x40020000U
#define RCC_REG_BASEADDR			0x40021000U
#define FMI_REG_BASEADDR			0x40022000U			//Flash memory interface
#define CRC_REG_BASEADDR			0x40023000U
/* APB2 BUS PERIPHERAL MACROS*/


/* NVIC PERIPHERAL MACROS */

#define NVIC_ISER0						((volatile uint32_t*)0xE000E100)//The NVIC_ISER0-NVIC_ISER7 registers enable interrupts, and show which interrupts are enabled.
#define NVIC_ISER1						((volatile uint32_t*)0xE000E104)//We have 60 IRQ in our device, so other NVIC Registers are not needed.

#define NVIC_ICER0 						((volatile uint32_t*)0xE000E180)//The NVIC_ICER0-NVIC_ICER7 registers disable interrupts, and show which interrupts are enabled
#define NVIC_ICER1 						((volatile uint32_t*)0xE000E184)//Write => 0: No Effect 1: Disable Interrupt

#define NVIC_ISPR0 						((volatile uint32_t*)0xE000E200)//The NVIC_ISPR0-NVIC_ISPR7 registers force interrupts into the pending state, and show which interrupts are pending.
#define NVIC_ISPR1 						((volatile uint32_t*)0xE000E204)

#define NVIC_ICPR0						((volatile uint32_t*)0xE000E280)//The NVIC_ICPR0-NCVIC_ICPR7 registers remove the pending state from interrupts, and show which interrupts are pending.
#define NVIC_ICPR1						((volatile uint32_t*)0xE000E284)

#define NVIC_IABR0						((volatile uint32_t*)0xE000E300)//The NVIC_IABR0-NVIC_IABR7 registers indicate which interrupts are active.
#define NVIC_IABR1						((volatile uint32_t*)0xE000E304)

#define NVIC_IPR0							((volatile uint32_t*)0xE000E400)//The NVIC_IPR0-NVIC_IPR59 registers provide an 8-bit priority field for each interrupt and each register holds four priority fields.
#define NVIC_IPR1							((volatile uint32_t*)0xE000E404)

#define	NVIC_STIR							((volatile uint32_t*)0xE000EF00)//Write to the STIR to generate an interrupt from software.


/* NVIC PERIPHERAL MACROS */

/*			PERIPHERAL REGISTER DEFINITION MACROS			*/
typedef struct
{
	volatile uint32_t GPIO_CRL;
	volatile uint32_t GPIO_CRH;
	volatile uint32_t GPIO_IDR;
	volatile uint32_t GPIO_ODR;
	volatile uint32_t GPIO_BSRR;
	volatile uint32_t GPIO_BRR;
	volatile uint32_t GPIO_LCKR;
}GPIO_RegDef_ts;

typedef struct
{
	volatile uint32_t ADC_SR;
	volatile uint32_t ADC_CR1;
	volatile uint32_t ADC_CR2;
	volatile uint32_t ADC_SMPR1;
	volatile uint32_t ADC_SMPR2;
	volatile uint32_t ADC_JOFR1;
	volatile uint32_t ADC_JOFR2;
	volatile uint32_t ADC_JOFR3;
	volatile uint32_t ADC_JOFR4;
	volatile uint32_t ADC_HTR;
	volatile uint32_t ADC_LTR;
	volatile uint32_t ADC_SQR1;
	volatile uint32_t ADC_SQR2;
	volatile uint32_t ADC_SQR3;
	volatile uint32_t ADC_JSQR;
	volatile uint32_t ADC_JDR1;
	volatile uint32_t ADC_JDR2;
	volatile uint32_t ADC_JDR3;
	volatile uint32_t ADC_JDR4;
	volatile uint32_t ADC_DR;
	
}ADC_RegDef_ts;

typedef struct
{
	volatile uint32_t USART_SR;
	volatile uint32_t USART_DR;
	volatile uint32_t USART_BRR;
	volatile uint32_t USART_CR1;
	volatile uint32_t USART_CR2;
	volatile uint32_t USART_CR3;
	volatile uint32_t USART_GTPR;
	
}USART_RegDef_ts;

typedef struct
{
	volatile uint32_t SPI_CR1;
	volatile uint32_t SPI_CR2;
	volatile uint32_t SPI_SR;
	volatile uint32_t SPI_DR;
	volatile uint32_t SPI_CRCPR;
	volatile uint32_t SPI_RXCRCR;
	volatile uint32_t SPI_TXCRCR;	
}SPI_RegDef_ts;

typedef struct
{
	volatile uint32_t I2C_CR1;
	volatile uint32_t I2C_CR2;
	volatile uint32_t I2C_OAR1;
	volatile uint32_t I2C_OAR2;
	volatile uint32_t I2C_DR;
	volatile uint32_t I2C_SR1;
	volatile uint32_t I2C_SR2;
	volatile uint32_t I2C_CCR;
	volatile uint32_t I2C_TRISE;
	
}I2C_RegDef_ts;

typedef struct
{
	volatile uint32_t AFIO_EVCR;
	volatile uint32_t AFIO_MAPR;
	volatile uint32_t AFIO_EXTICR1;
	volatile uint32_t AFIO_EXTICR2;
	volatile uint32_t AFIO_EXTICR3;
	volatile uint32_t AFIO_EXTICR4;
	volatile uint32_t Reserved;
	volatile uint32_t AFIO_MAPR2;
	
}AFIO_RegDef_ts;

typedef struct
{
	volatile uint32_t EXTI_IMR;
	volatile uint32_t EXTI_EMR;
	volatile uint32_t EXTI_RTSR;
	volatile uint32_t EXTI_FTSR;
	volatile uint32_t EXTI_SWIER;
	volatile uint32_t EXTI_PR;
}EXTI_RegDef_ts;


/*			PERIPHERAL REGISTER DEFINITION MACROS			*/

/* 			PERIPHERAL DEFINITIONS (peripheral base addresses typecased to xxx_RegDef_ts )		*/
#define GPIOA				((GPIO_RegDef_ts*)GPIOA_REG_BASEADDR)
#define GPIOB				((GPIO_RegDef_ts*)GPIOB_REG_BASEADDR)
#define GPIOC				((GPIO_RegDef_ts*)GPIOC_REG_BASEADDR)
#define GPIOD				((GPIO_RegDef_ts*)GPIOD_REG_BASEADDR)
#define GPIOE				((GPIO_RegDef_ts*)GPIOE_REG_BASEADDR)

#define ADC1				((ADC_RegDef_ts*)ADC1_REG_BASEADDR)
#define AFIO				((AFIO_RegDef_ts*)AFIO_REG_BASEADDR)
#define EXTI				((EXTI_RegDef_ts*)EXTI_REG_BASEADDR)
#define SPI1				((SPI_RegDef_ts*)SPI1_REG_BASEADDR)
#define SPI2				((SPI_RegDef_ts*)SPI2_REG_BASEADDR)

#define I2C1				((I2C_RegDef_ts*)I2C1_REG_BASEADDR)

#define USART1			((USART_RegDef_ts*)USART1_REG_BASEADDR)
#define USART2			((USART_RegDef_ts*)USART2_REG_BASEADDR)
#define USART3			((USART_RegDef_ts*)USART3_REG_BASEADDR)

/* 			PERIPHERAL DEFINITIONS (peripheral base addresses typecased to xxx_RegDef_ts )		*/

#define EXTI_PORTA_CODE 0x00
#define EXTI_PORTB_CODE 0x01
#define EXTI_PORTC_CODE 0x02
#define EXTI_PORTD_CODE 0x03
#define EXTI_PORTE_CODE 0x04

/* RCC REGISTER MACROS */

typedef struct
{
	volatile uint32_t RCC_CR;
	volatile uint32_t RCC_CFGR;
	volatile uint32_t RCC_CIR;
	volatile uint32_t RCC_APB2RSTR;
	volatile uint32_t RCC_APB1RSTR;
	volatile uint32_t RCC_AHBENR;
	volatile uint32_t RCC_APB2ENR;
	volatile uint32_t RCC_APB1ENR;
	volatile uint32_t RCC_BDCR;
	volatile uint32_t RCC_CSR;
	volatile uint32_t Reserved;
	volatile uint32_t RCC_CFGR2;
}RCC_RegDef_ts;

#define RCC ((RCC_RegDef_ts*)RCC_REG_BASEADDR)

#define RCC_CR_HSION						0
#define RCC_CR_HSIRDY						1
#define RCC_CR_HSITRIM4_0				3//Extra programmable trimming value. Has 5 bits.
#define RCC_CR_HSICAL7_0				8//Clock calibration bits. Init'ed automatically at startup
#define RCC_CR_HSEON						16
#define RCC_CR_HSERDY						17
#define RCC_CR_HSEBYP						18//HSE is bypassed by another clock
#define RCC_CR_CSSON						19
#define RCC_CR_PLLON						24
#define RCC_CR_PLLRDY						25

#define RCC_CFGR_SW							0//Set and cleared by software to select SYSCLK source. Has 2 Bits.
#define RCC_CFGR_SWS						2//System Clock Switch Status. Set and cleared by hardware to indicate which clock source is used as system clock.
//00: HSI oscillator used as system clock
//01: HSE oscillator used as system clock
//10: PLL used as system clock
//11: not applicable

#define RCC_CFGR_HPRE						4//AHB Prescaler.Has 4 bits. 0xxx: Not Divided. 1000:SysCLK divided by 2, 1001: SysCLK divided by 4 and so on.
#define RCC_CFGR_PPRE1					8//APB Low Speed Prescaler(APB1). 0xx:Not divided. 100:HCLK divided by 2, 101:HCLK div 4 and so on.
#define RCC_CFGR_PPRE2					11//APB High Speed Prescaler(APB2). 0xx:Not divided. 100:HCLK divided by 2, 101:HCLK div 4 and so on.
#define RCC_CFGR_ADCPRE					14//ADC prescaler. 00:PCLK2 div by 2, 01:PCLK2 div by 4,10:PCLK2 div by 6, 11:PCLK2 div by 8
#define RCC_CFGR_PLLSRC					16//PLL Entry Clock Source. Can only be written when PLL is disabled. 0: HSI/2 selected as PLL input. 1:Clock From PREDIV1 selected.
#define RCC_CFGR_PLLXTPRE				17
#define RCC_CFGR_PLLMUL					18//Can only be written when PLL is disabled. PLL output freq must be in 16-24 MHz range.
//0000:PLL input clk x2
//0001:PLL input clk x3
//0010:PLL input clk x4 and so on.

#define RCC_CFGR_MCO						24//MCU clock output. Has two bits. 0xx:No clock, 100:Sysclk, 101:HSI clk, 110:HSE Clk, 111:PLLCLK/2 is selected.

#define RCC_CFGR2_PREDIV1				0//0000:Prediv Input Clock Not Divided, 0001: Div by 2, 0010: Div By 3, 0011: Div by 4 and so on.

/* RCC REGISTER MACROS */

/* PERIPHERAL CLOCK MACROS*/
#define GPIOA_PCLK_EN()	(RCC->RCC_APB2ENR |= (1<<2))
#define GPIOB_PCLK_EN()	(RCC->RCC_APB2ENR |= (1<<3))
#define GPIOC_PCLK_EN()	(RCC->RCC_APB2ENR |= (1<<4))
#define GPIOD_PCLK_EN()	(RCC->RCC_APB2ENR |= (1<<5))
#define GPIOE_PCLK_EN()	(RCC->RCC_APB2ENR |= (1<<6))

#define GPIOA_PCLK_DI()	(RCC->RCC_APB2ENR &= ~(1<<2))
#define GPIOB_PCLK_DI()	(RCC->RCC_APB2ENR &= ~(1<<3))
#define GPIOC_PCLK_DI()	(RCC->RCC_APB2ENR &= ~(1<<4))
#define GPIOD_PCLK_DI()	(RCC->RCC_APB2ENR &= ~(1<<5))
#define GPIOE_PCLK_DI()	(RCC->RCC_APB2ENR &= ~(1<<6))

#define AFIO_PCLK_EN()		(RCC->RCC_APB2ENR |= (1<<0))
#define ADC1_PCLK_EN()		(RCC->RCC_APB2ENR |= (1<<9))
#define SPI1_PCLK_EN()		(RCC->RCC_APB2ENR |= (1<<12))
#define USART1_PCLK_EN()	(RCC->RCC_APB2ENR |= (1<<14))

#define AFIO_PCLK_DI()		(RCC->RCC_APB2ENR &= ~(1<<0))
#define ADC1_PCLK_DI()		(RCC->RCC_APB2ENR &= ~(1<<9))
#define SPI1_PCLK_DI()		(RCC->RCC_APB2ENR &= ~(1<<12))
#define USART1_PCLK_DI()	(RCC->RCC_APB2ENR &= ~(1<<14))

#define SPI2_PCLK_EN()		(RCC->RCC_APB1ENR |= (1<<14))
#define SPI3_PCLK_EN()		(RCC->RCC_APB1ENR |= (1<<15))
#define USART2_PCLK_EN()	(RCC->RCC_APB1ENR |= (1<<17))
#define USART3_PCLK_EN()	(RCC->RCC_APB1ENR |= (1<<18))
#define UART4_PCLK_EN()		(RCC->RCC_APB1ENR |= (1<<19))
#define UART5_PCLK_EN()		(RCC->RCC_APB1ENR |= (1<<20))
#define I2C1_PCLK_EN()		(RCC->RCC_APB1ENR |= (1<<21))
#define I2C2_PCLK_EN()		(RCC->RCC_APB1ENR |= (1<<22))

#define SPI2_PCLK_DI()		(RCC->RCC_APB1ENR &= ~(1<<14))
#define SPI3_PCLK_DI()		(RCC->RCC_APB1ENR &= ~(1<<15))
#define USART2_PCLK_DI()	(RCC->RCC_APB1ENR &= ~(1<<17))
#define USART3_PCLK_DI()	(RCC->RCC_APB1ENR &= ~(1<<18))
#define UART4_PCLK_DI()		(RCC->RCC_APB1ENR &= ~(1<<19))
#define UART5_PCLK_DI()		(RCC->RCC_APB1ENR &= ~(1<<20))
#define I2C1_PCLK_DI()		(RCC->RCC_APB1ENR &= ~(1<<21))
#define I2C2_PCLK_DI()		(RCC->RCC_APB1ENR &= ~(1<<22))

/* PERIPHERAL CLOCK MACROS*/

/*PERIPHERAL RESET MACROS*/
#define GPIOA_RST()			do{(RCC->RCC_APB2RSTR |= (1<<2));(RCC->RCC_APB2RSTR &= ~(1<<2));}while(0)//Shouldnt be left as 1 so it wont force reset all the time
#define GPIOB_RST()			do{(RCC->RCC_APB2RSTR |= (1<<3));(RCC->RCC_APB2RSTR &= ~(1<<3));}while(0)
#define GPIOC_RST()			do{(RCC->RCC_APB2RSTR |= (1<<4));(RCC->RCC_APB2RSTR &= ~(1<<4));}while(0)
#define GPIOD_RST()			do{(RCC->RCC_APB2RSTR |= (1<<5));(RCC->RCC_APB2RSTR &= ~(1<<5));}while(0)
#define GPIOE_RST()			do{(RCC->RCC_APB2RSTR |= (1<<6));(RCC->RCC_APB2RSTR &= ~(1<<6));}while(0)

#define SPI1_RST()			do{(RCC->RCC_APB2RSTR |= (1<<12));(RCC->RCC_APB2RSTR &= ~(1<<12));}while(0)
#define USART1_RST()		do{(RCC->RCC_APB2RSTR |= (1<<14));(RCC->RCC_APB2RSTR &= ~(1<<14));}while(0)
#define ADC1_RST()			do{(RCC->RCC_APB2RSTR |= (1<<9));(RCC->RCC_APB2RSTR &= ~(1<<9));}while(0)
#define AFIO_RST()			do{(RCC->RCC_APB2RSTR |= (1<<0));(RCC->RCC_APB2RSTR &= ~(1<<0));}while(0)

#define SPI2_RST()			do{(RCC->RCC_APB1RSTR |= (1<<14));(RCC->RCC_APB1RSTR &= ~(1<<14));}while(0)
#define SPI3_RST()			do{(RCC->RCC_APB1RSTR |= (1<<15));(RCC->RCC_APB1RSTR &= ~(1<<15));}while(0)
#define USART2_RST()		do{(RCC->RCC_APB1RSTR |= (1<<17));(RCC->RCC_APB1RSTR &= ~(1<<17));}while(0)
#define USART3_RST()		do{(RCC->RCC_APB1RSTR |= (1<<18));(RCC->RCC_APB1RSTR &= ~(1<<18));}while(0)
#define UART4_RST()			do{(RCC->RCC_APB1RSTR |= (1<<19));(RCC->RCC_APB1RSTR &= ~(1<<19));}while(0)
#define UART5_RST()			do{(RCC->RCC_APB1RSTR |= (1<<20));(RCC->RCC_APB1RSTR &= ~(1<<20));}while(0)
#define I2C1_RST()			do{(RCC->RCC_APB1RSTR |= (1<<21));(RCC->RCC_APB1RSTR &= ~(1<<21));}while(0)
#define I2C2_RST()			do{(RCC->RCC_APB1RSTR |= (1<<22));(RCC->RCC_APB1RSTR &= ~(1<<22));}while(0)

/*PERIPHERAL RESET MACROS*/

#define GPIO_BASEADDR_TO_CODE(x)      ( (x == GPIOA)?0:\
																				(x == GPIOB)?1:\
																				(x == GPIOC)?2:\
																				(x == GPIOD)?3:\
																				(x == GPIOE)?4:15)// This Converts the address of the GPIO Port to its EXTI code.

#define ENABLE 1
#define DISABLE 0
#define SET ENABLE
#define RESET DISABLE
#define GPIO_PIN_RESET DISABLE
#define GPIO_PIN_SET ENABLE
#define FLAG_SET	ENABLE
#define FLAG_RESET DISABLE


/*@NVIC IRQ NUMBERS*/

/*
	@EXTI_IRQ_NUMBERS
	Corresponding IRQ numbers for EXTI Lines
*/
#define IRQNO_EXTI0					6
#define IRQNO_EXTI1					7
#define IRQNO_EXTI2					8
#define IRQNO_EXTI3					9
#define IRQNO_EXTI4					10
#define IRQNO_EXTI5TO9			23
#define IRQNO_EXTI10TO15		40
#define IRQNO_SPI1					35
#define IRQNO_SPI2					36
#define IRQNO_I2C1_EV				31
#define IRQNO_I2C1_ER				32
#define IRQNO_USART1				37
#define IRQNO_USART2				38
#define IRQNO_USART3				39

#include "stm32F100xxLM_spi_driver.h"
#include "stm32f100xxLM_gpio_driver.h"
#include "stm32f100xxLM_i2c_driver.h"
#include "stm32f100xxLM_usart_driver.h"
#include "stm32f100xxLM_rcc_driver.h"

#endif