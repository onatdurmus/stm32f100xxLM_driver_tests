#ifndef STM32F100LM_SPI_DRIVER_H
#define STM32F100LM_SPI_DRIVER_H

#include "stm32F100xxLMdensity.h"

#define SPI_SSOE_ENABLE 	ENABLE
#define SPI_SSOE_DISABLE	DISABLE


typedef struct
{
	uint8_t BusConfig;
	uint8_t DeviceMode;
	uint8_t BusSpeed;
	uint8_t DFF;
	uint8_t CPOL;
	uint8_t CPHA;
	uint8_t SlaveManagement;
	
}SPI_Config_ts;

typedef struct
{
	SPI_RegDef_ts* pSPIx;
	SPI_Config_ts SPIConfig;
	uint8_t TxLen;
	uint8_t RxLen;
	uint8_t TxState;
	uint8_t RxState;
	uint8_t* pTxBuffer;
	uint8_t* pRxBuffer;
		
}SPI_Handle_ts;

/*
	@SPI CONFIGURATION MACROS
*/

/*@SPI BUS CONFIG OPTIONS*/
#define SPI_BUSCONF_FD											1
#define SPI_BUSCONF_HD											2
#define SPI_BUSCONF_SIMPLEXRXONLY						3

/* @SPI DEVICE MODES */
#define SPI_DEVMODE_SLAVE				0
#define SPI_DEVMODE_MASTER			1

/* @SPI BUS SPEED, @SPI_CLOCK SPI CLOCK OPTIONS*/
//BR Register is 3 bits so do the shifting accordingly.

#define SPI_FPCLK_DIV2				0x00
#define SPI_FPCLK_DIV4				0x01
#define SPI_FPCLK_DIV8				0x02
#define SPI_FPCLK_DIV16				0x03
#define SPI_FPCLK_DIV32				0x04
#define SPI_FPCLK_DIV64				0x05
#define SPI_FPCLK_DIV128			0x06
#define SPI_FPCLK_DIV256			0x07

/*@SPI DFF SPI DATA FRAME FORMAT */
#define SPI_DFF_8BIT			0															//8 BIT Data transfer
#define SPI_DFF_16BIT			1															//16Bit Data transfer

/*SPI FIRSTBIT LSB OR MSB FIRST*/
#define SPI_FIRSTBIT_MSB				0												//MSB Transmitted first
#define SPI_FIRSTBIT_LSB				1

/*@SPI CPOL OPTIONS */
#define SPI_CPOL_CLKIDLE0				0												//Clock is 0 when idle
#define SPI_CPOL_CLKIDLE1				1

/*@SPI CPHA OPTIONS*/
#define SPI_CPHA_FIRSTEDGE			0												//Data is captured on the first clock edge
#define SPI_CPHA_SECONDEDGE			1												//Data is captured on the second clock edge

/*@SPI SLAVE MANAGEMENT SELECT OPTIONS*/
#define SPI_SS_HWSM							0												//Hardware slave management is selected
#define SPI_SS_SWSM							1												//Software slave management is selected
/*
*	@SPI REGISTER BIT POSITIONS
*/

/* @SPI CR1 REGISTER BIT POSITIONS*/
#define SPI_CR1_CPHA				0
#define SPI_CR1_CPOL				1
#define SPI_CR1_MSTR				2
#define SPI_CR1_BR					3
#define SPI_CR1_SPE					6
#define SPI_CR1_LSBFIRST		7
#define SPI_CR1_SSI					8
#define SPI_CR1_SSM					9
#define SPI_CR1_RXONLY			10
#define SPI_CR1_DFF					11
#define SPI_CR1_CRCNEXT			12
#define SPI_CR1_CRCEN				13
#define SPI_CR1_BIDIOE			14
#define SPI_CR1_BIDIMODE		15

/* @SPI CR2 REGISTER BIT POSITIONS*/
#define SPI_CR2_RXDMAEN			0
#define SPI_CR2_TXDMAEN			1
#define SPI_CR2_SSOE				2
#define SPI_CR2_ERRIE				5
#define SPI_CR2_RXNEIE			6
#define SPI_CR2_TXEIE				7

/* @SPI SR REGISTER BIT POSITIONS*/
#define SPI_SR_RXNE					0
#define SPI_SR_TXE					1
#define SPI_SR_CRCERR				4
#define SPI_SR_MODF					5
#define SPI_SR_OVR					6
#define SPI_SR_BSY					7

/* @SPI EnorDi Functions*/
#define SPI1_ENABLE()				SPI1->SPI_CR1 |= (1<<SPI_CR1_SPE)
#define SPI2_ENABLE()				SPI2->SPI_CR1 |= (1<<SPI_CR1_SPE)

#define SPI1_DISABLE()			SPI1->SPI_CR1 &= ~(1<<SPI_CR1_SPE)
#define SPI2_DISABLE()			SPI2->SPI_CR1 &= ~(1<<SPI_CR1_SPE)


/* @ SPI TX AND RX STATES @SPI TX STATE */
#define SPI_STATE_FREE				0
#define SPI_STATE_BSY_TX			1
#define SPI_STATE_BSY_RX			2


/* @SPI EVENTS*/

#define SPI_EVENT_TX_COMPLETE			1
#define SPI_EVENT_RX_COMPLETE			2
#define SPI_EVENT_OVERRUNERR			3

/* SPI DRIVER FUNCTIONS */

void SPI_PCLKControl(SPI_RegDef_ts* pSPIx,uint8_t EnorDi);
void SPI_PeriControl(SPI_RegDef_ts* pSPIx, uint8_t EnorDi);
void SPIInit(SPI_Handle_ts* pSPIHandle);
uint8_t SPISendData(SPI_Handle_ts* pSPIHandle, uint8_t* pTxBuffer, uint32_t Len);
uint8_t SPIReceiveData(SPI_Handle_ts* pSPIHandle, uint8_t* pRxBuffer, uint32_t Len);
uint8_t SPIGetFlag(SPI_RegDef_ts* pSPIx,uint32_t SRBit);
void SPI_Driver_ISR_Handler(SPI_Handle_ts* pSPIHandle);
void SPICloseTransmission(SPI_Handle_ts* pSPIHandle);
void SPICloseReception(SPI_Handle_ts* pSPIHandle);
void SPIApplicationCallback(SPI_Handle_ts* pSPIHandle, uint8_t SpiEvent);
void SPI_SSOEControl(SPI_RegDef_ts* pSPIx, uint8_t EnorDi);
void SPI_SSIControl(SPI_RegDef_ts* pSPIx, uint8_t EnorDi);
void SPI_ClearOVRFlag(SPI_RegDef_ts* pSPIx);

#endif