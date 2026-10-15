#include "stm32F100xxLM_spi_driver.h"

static void spi_txe_isr_handler(SPI_Handle_ts* pSPIHandle);
static void spi_rxne_isr_handler(SPI_Handle_ts* pSPIHandle);
static void spi_ovrerr_isr_handler(SPI_Handle_ts* pSPIHandle);


//TODO: When/how to disable SPI without causing problems?
//TXE should be set, busy should be reset. Write decent enough functions for these.
//Should check if master mode, bidi or else.


void SPI_PCLKControl(SPI_RegDef_ts* pSPIx,uint8_t EnorDi)
{
	if(EnorDi == ENABLE){		
		if(pSPIx == SPI1){
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2){
			SPI2_PCLK_EN();
		}
	}
	else if(EnorDi == DISABLE){
		if(pSPIx == SPI1){
			SPI1_PCLK_DI();
		}
		else if(pSPIx == SPI2){
			SPI2_PCLK_DI();
		}
	}
}

void SPI_PeriControl(SPI_RegDef_ts* pSPIx, uint8_t EnorDi){
	if(EnorDi == ENABLE){
			pSPIx->SPI_CR1 |= (1<<SPI_CR1_SPE);
	}
	else if(EnorDi==DISABLE){
			pSPIx->SPI_CR1 &= ~(1<<SPI_CR1_SPE);
	}	
}

void SPIInit(SPI_Handle_ts* pSPIHandle)
{
	SPI_PCLKControl(pSPIHandle->pSPIx,ENABLE);
	
	uint32_t tempreg = 0;
	
	tempreg |= (pSPIHandle->SPIConfig.DeviceMode<<SPI_CR1_MSTR);
	tempreg |= (pSPIHandle->SPIConfig.DFF<<SPI_CR1_DFF);
	tempreg |= (pSPIHandle->SPIConfig.CPHA<<SPI_CR1_CPHA);
	tempreg |= (pSPIHandle->SPIConfig.CPOL<<SPI_CR1_CPOL);
	tempreg |= (pSPIHandle->SPIConfig.SlaveManagement<<SPI_CR1_SSM);
	tempreg |= (pSPIHandle->SPIConfig.BusSpeed<<SPI_CR1_BR);
	
	if(pSPIHandle->SPIConfig.BusConfig == SPI_BUSCONF_FD)
	{
		
	}
	else if(pSPIHandle->SPIConfig.BusConfig == SPI_BUSCONF_HD)
	{
		tempreg |= (1<<SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.BusConfig == SPI_BUSCONF_SIMPLEXRXONLY)
	{
		tempreg |= (1<<SPI_CR1_RXONLY);
	}
	
	pSPIHandle->pSPIx->SPI_CR1 = tempreg;
}

uint8_t SPISendData(SPI_Handle_ts* pSPIHandle, uint8_t* pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;
	if(state != SPI_STATE_BSY_TX){
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;
		pSPIHandle->TxState = SPI_STATE_BSY_TX;
		pSPIHandle->pSPIx->SPI_CR2 |= (1<< SPI_CR2_TXEIE);
			}
	return state;
}

uint8_t SPIReceiveData(SPI_Handle_ts* pSPIHandle, uint8_t* pRxBuffer, uint32_t Len)
{
	
	uint8_t state = pSPIHandle->RxState;
	if(state != SPI_STATE_BSY_RX){
		pSPIHandle->pSPIx->SPI_CR2 &= ~(1<<SPI_CR2_RXNEIE);
		 while (pSPIHandle->pSPIx->SPI_SR & (1 << SPI_SR_RXNE))
		 {
			 SPI_ClearOVRFlag(pSPIHandle->pSPIx);
		 }		
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;
		pSPIHandle->RxState = SPI_STATE_BSY_RX;
		pSPIHandle->pSPIx->SPI_CR2 |= (1<<SPI_CR2_RXNEIE);
	}
	return state;
}

uint8_t SPIGetFlag(SPI_RegDef_ts* pSPIx,uint32_t SRBit)
{
	if(pSPIx->SPI_SR & (1<<SRBit))
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void SPI_Driver_ISR_Handler(SPI_Handle_ts* pSPIHandle)
{
	uint8_t temp1,temp2;
	temp1 = pSPIHandle->pSPIx->SPI_SR&(1<<SPI_SR_TXE);//Check for Tx interrupt, if so call txe isr handler
	temp2 = pSPIHandle->pSPIx->SPI_CR2&(1<<SPI_CR2_TXEIE);
	if(temp1&&temp2){
		spi_txe_isr_handler(pSPIHandle);
	}
	temp1 = pSPIHandle->pSPIx->SPI_SR&(1<<SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->SPI_CR2&(1<<SPI_CR2_RXNEIE);
	if(temp1&&temp2){
		spi_rxne_isr_handler(pSPIHandle);
	}
	temp1 = pSPIHandle->pSPIx->SPI_SR&(1<<SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->SPI_CR2&(1<<SPI_CR2_ERRIE);
	if(temp1&&temp2){
		spi_ovrerr_isr_handler(pSPIHandle);
	}
		
}


static void spi_txe_isr_handler(SPI_Handle_ts* pSPIHandle)
{
	if(pSPIHandle->TxState != SPI_STATE_BSY_TX){//Guard against wrongly timed interrupts
    return;}
	if(pSPIHandle->SPIConfig.DFF == SPI_DFF_8BIT){
		//Write code for 8bit dff
		*((volatile uint8_t*)&pSPIHandle->pSPIx->SPI_DR) = *(pSPIHandle->pTxBuffer);
		pSPIHandle->pTxBuffer++;
		pSPIHandle->TxLen--;
	}
	else if(pSPIHandle->SPIConfig.DFF == SPI_DFF_16BIT){
		//Write code for 16bit Dff
		pSPIHandle->pSPIx->SPI_DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->pTxBuffer++;
		pSPIHandle->pTxBuffer++;
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
	}
	if(pSPIHandle->TxLen == 0){
			SPICloseTransmission(pSPIHandle);//Clears TXEIE, spihandle TxLen and TxBuffer. Makes TxState free.
			if((pSPIHandle->RxLen ==0) &&(pSPIHandle->SPIConfig.BusConfig == SPI_BUSCONF_FD))
			{
				SPI_ClearOVRFlag(pSPIHandle->pSPIx);//If bus is full duplex, we should also clear RXNE flag so it wont bother us in the future.
			}
			SPIApplicationCallback(pSPIHandle,SPI_EVENT_TX_COMPLETE);
	}		
}


static void spi_rxne_isr_handler(SPI_Handle_ts* pSPIHandle)
{	
	static uint16_t ReceiveBufMaxLen = 500;//Change this later so multiple SPI's can be used.
	if(pSPIHandle->pSPIx->SPI_SR & (1 << SPI_SR_OVR)) {
    SPI_ClearOVRFlag(pSPIHandle->pSPIx);//Clears OVR if there is any
	}
	if(pSPIHandle->SPIConfig.DFF == SPI_DFF_8BIT){
		//Write code for 8bit dff
		*pSPIHandle->pRxBuffer = (uint8_t)pSPIHandle->pSPIx->SPI_DR;
		pSPIHandle->pRxBuffer++;
		pSPIHandle->RxLen--;
		ReceiveBufMaxLen--;
	}
	else if(pSPIHandle->SPIConfig.DFF == SPI_DFF_16BIT){
		//Write code for 16bit Dff
		*(uint16_t*)pSPIHandle->pRxBuffer = (uint16_t)pSPIHandle->pSPIx->SPI_DR;
		pSPIHandle->pRxBuffer += 2;
		pSPIHandle->RxLen -= 2;
		ReceiveBufMaxLen -=2;
	}
	if((pSPIHandle->RxLen == 0)||(ReceiveBufMaxLen == 0)){
			SPICloseReception(pSPIHandle);
			SPIApplicationCallback(pSPIHandle,SPI_EVENT_RX_COMPLETE);
	}		
}

static void spi_ovrerr_isr_handler(SPI_Handle_ts* pSPIHandle)//This is not complete
{
		SPI_ClearOVRFlag(pSPIHandle->pSPIx);
}

void SPICloseTransmission(SPI_Handle_ts* pSPIHandle)
{
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_STATE_FREE;
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->pSPIx->SPI_CR2 &= ~(1<<SPI_CR2_TXEIE);
}

void SPICloseReception(SPI_Handle_ts* pSPIHandle)
{
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_STATE_FREE;
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->pSPIx->SPI_CR2 &= (1<<SPI_CR2_RXNEIE);
}

void SPI_ClearOVRFlag(SPI_RegDef_ts* pSPIx)
{
	uint8_t temp;
	temp = pSPIx->SPI_DR;
	temp = pSPIx->SPI_SR;
	(void)temp;
}

void SPI_SSOEControl(SPI_RegDef_ts* pSPIx, uint8_t EnorDi)
{
	if(EnorDi == SPI_SSOE_ENABLE){
		pSPIx->SPI_CR2 |= (1<<SPI_CR2_SSOE);
	}
	else if(EnorDi == SPI_SSOE_DISABLE){
		pSPIx->SPI_CR2 &= ~(1<<SPI_CR2_SSOE);
	}
}

void SPI_SSIControl(SPI_RegDef_ts* pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE){
		pSPIx->SPI_CR1 |= (1<<SPI_CR1_SSI);
	}
	else if(EnorDi == DISABLE){
		pSPIx->SPI_CR1 &= ~(1<<SPI_CR1_SSI);
	}
}
__attribute__ ((weak))void SPIApplicationCallback(SPI_Handle_ts* pSPIHandle, uint8_t SpiEvent)
{
	(void)pSPIHandle;
	(void)SpiEvent;
}