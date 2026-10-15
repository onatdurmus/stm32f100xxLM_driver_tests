#include "stm32f100xxLM_usart_driver.h"

void USART_PeriCLK_Control(USART_RegDef_ts* pUSART,uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pUSART == USART1)
		{
			USART1_PCLK_EN();
		}
		else if(pUSART == USART2)
		{
			USART2_PCLK_EN();
		}
		else if(pUSART == USART3)
		{
			USART3_PCLK_EN();
		}
	}
	else if(EnorDi == DISABLE)
	{
		if(pUSART == USART1)
		{
			USART1_PCLK_DI();
		}
		else if(pUSART == USART2)
		{
			USART2_PCLK_DI();
		}
		else if(pUSART == USART3)
		{
			USART3_PCLK_DI();
		}
	}
}

void USART_PeriControl(USART_RegDef_ts* pUSART, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pUSART->USART_CR1 |= (1<<USART_CR1_UE);
	}
	else if(EnorDi == DISABLE)
	{
		pUSART->USART_CR1 &= ~(1<<USART_CR1_UE);
	}
}

uint8_t USART_GetFlag(USART_RegDef_ts* pUSART,uint8_t UsartFlag)
{
	if(pUSART->USART_SR &(1<<UsartFlag))
	{
		return SET;
	}
	return RESET;
}

void USART_Init(USART_Handle_ts* pUSARTHandle)
{	
	//Enable the peripheral clock if not enabled.
	USART_PeriCLK_Control(pUSARTHandle->USARTx,ENABLE);
	//Enable the USART Peripheral
	pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_UE);
	
	//Set the Word Length
	if(pUSARTHandle->USARTConfig->WordLen == USART_WORDLEN_9)
	{
		pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_M);
	}
	else if(pUSARTHandle->USARTConfig->WordLen == USART_WORDLEN_8)
	{
		pUSARTHandle->USARTx->USART_CR1 &= ~(1<<USART_CR1_M);
	}
	
	//Set the number of stop bits.
	pUSARTHandle->USARTx->USART_CR2 &= (0x03<<USART_CR2_STOP);//First reset the STOP bits.
	pUSARTHandle->USARTx->USART_CR2 |=	(pUSARTHandle->USARTConfig->StopBits<<USART_CR2_STOP);//We arranged the stop bit macros so it is compatible with TRM.
	
	//Set the Parity
	if(pUSARTHandle->USARTConfig->ParityMode == USART_PRTY_DI)
	{
		pUSARTHandle->USARTx->USART_CR1 &= ~(1<<USART_CR1_PCE);//Reset the parity control enable bit.
	}
	else
	{
		pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_PCE);//Set the parity control enable bit.
		if(pUSARTHandle->USARTConfig->ParityMode == USART_PRTY_EVEN)
		{
			pUSARTHandle->USARTx->USART_CR1 &= ~(1<<USART_CR1_PS);//Reset the parity selection bit. 0: Even parity
		}
		else if(pUSARTHandle->USARTConfig->ParityMode == USART_PRTY_EVEN)
		{
			pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_PS);//Set the parity selection bit. 1: Odd parity
		}
	}
	
	//Set the Mode for USART
	if(pUSARTHandle->USARTConfig->UsartMode == USART_MODE_TXRX)
	{
		pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_TE);
		pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_RE);
	}
	else if(pUSARTHandle->USARTConfig->UsartMode == USART_MODE_TXONLY)
	{
		pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_TE);
		pUSARTHandle->USARTx->USART_CR1 &= ~(1<<USART_CR1_RE);
	}
	else if(pUSARTHandle->USARTConfig->UsartMode == USART_MODE_RXONLY)
	{
		pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_RE);
		pUSARTHandle->USARTx->USART_CR1 &= ~(1<<USART_CR1_TE);
	}
	else
	{
		//Error or maybe idle state will be added.
	}
	
	//Set Hardware Flow Control
	pUSARTHandle->USARTx->USART_CR3 &= ~(3<<USART_CR3_RTSE);//Start by clearing RTSEnable and CTS Enable bits.
	
	if(pUSARTHandle->USARTConfig->HWControl == USART_HWCNTRL_CTS)
	{
		pUSARTHandle->USARTx->USART_CR3 |= (1<<USART_CR3_CTSE);
	}
	else if(pUSARTHandle->USARTConfig->HWControl == USART_HWCNTRL_RTS)
	{
		pUSARTHandle->USARTx->USART_CR3 |= (1<<USART_CR3_RTSE);
	}
	else if(pUSARTHandle->USARTConfig->HWControl == USART_HWCNTRL_CTSRTS)
	{
		pUSARTHandle->USARTx->USART_CR3 |= (3<<USART_CR3_RTSE);
	}
	else
	{//No need to do anything for disable since we did at the start.
	}
	
	//Set The Baud Rate
	USART_SetBaudRate(pUSARTHandle->USARTx,pUSARTHandle->USARTConfig->BaudRate);
	
}

void USART_DeInit(USART_RegDef_ts* pUSART)
{
	if(pUSART == USART1)
		{
			USART1_RST();
		}
		else if(pUSART == USART2)
		{
			USART2_RST();
		}
		else if(pUSART == USART3)
		{
			USART3_RST();
		}
}

uint8_t USART_SendBlocking(USART_RegDef_ts* USARTx,uint8_t* pTxBuf,uint32_t Len)
{	
	if(!(USARTx->USART_CR1&(1<<USART_CR1_TE)))
	{
		return DISABLE;//If transmission is not enabled, return error.
	}
	while(Len>0)
	{
		while(!(USARTx->USART_SR&(1<<USART_SR_TXE)));//Wait while TXE is 0.
		if(USARTx->USART_CR1&(1<<USART_CR1_M))//If M=1, word length is 9 bits.
		{
			if(USARTx->USART_CR1&(1<<USART_CR1_PCE))//Parity is selected.8 Data bits and 1 Parity bit.
			{
				USARTx->USART_DR = *pTxBuf;
				pTxBuf++;
			}
			else//No parity, all 9 bits are data.
			{
				uint16_t* pData = (uint16_t*)pTxBuf;
				USARTx->USART_DR = (0x01FF&(*pData));//Mask the 9LSBits of pData and write it to DR
				pTxBuf += 2; // Increment the txbuf by 2 because we are using 9 bits, we will need next addresses.
			}
		}
		else
		{
			if(USARTx->USART_CR1&(1<<USART_CR1_PCE))//Parity is selected.7 Data bits and 1 Parity bit.
			{
				USARTx->USART_DR = ((0x7F)&(*pTxBuf));//Mask the 7LSBits of Data and write it into Data Register.
				pTxBuf++;
			}
			else//No parity, all 8 bits are data.
			{
				USARTx->USART_DR = *pTxBuf;
				pTxBuf++;
			}
		}
		Len--;
	}
	while(!(USART_GETFLAG(USARTx,USART_SR_TC)));//Wait while TC is not set. Bu yazdigimiz makro icin de bir test olur.
	return ENABLE;
}
uint8_t USART_ReceiveBlocking(USART_RegDef_ts* USARTx,uint8_t* pRxBuf,uint32_t Len)
{
	if(!(USARTx->USART_CR1&(1<<USART_CR1_RE)))
	{
		return DISABLE;//If receiving is not enabled, return error.
	}
	while(Len>0)
	{
		while(!(USARTx->USART_SR&(1<<USART_SR_RXNE)));//Wait while RXNE is 0.
		if(USARTx->USART_CR1&(1<<USART_CR1_M))//If M=1, word length is 9 bits.
		{
			if(USARTx->USART_CR1&(1<<USART_CR1_PCE))//Parity is selected.8 Data bits and 1 Parity bit.
			{
				*pRxBuf = (uint8_t)(USARTx->USART_DR&0xFF);//Mask the last 8 bits and read into buffer.
				pRxBuf++;
			}
			else//No parity, all 9 bits are data.
			{
				*((uint16_t*)pRxBuf) = (uint16_t)(USARTx->USART_DR & 0x01FF);//Mask the first 9 bits then read into Rx Buf
				pRxBuf += 2; // Increment the Rxbuf by 2 because we are using 9 bits, we will need next addresses.
			}
		}
		else//Else we are using 8 bit frame format (word length)
		{
			if(USARTx->USART_CR1&(1<<USART_CR1_PCE))//Parity is selected.7 Data bits and 1 Parity bit.
			{
				 *pRxBuf = (uint8_t)((0x7F)&(USARTx->USART_DR));//Mask the 7LSBits of Data and read it into Buffer.
			}
			else//No parity, all 8 bits are data.
			{
				*pRxBuf = (uint8_t)(USARTx->USART_DR&0xFF);
			}
			pRxBuf++;
		}
		Len--;
	}
	return ENABLE;
}
uint8_t USART_TransmitIT(USART_Handle_ts* pUSARTHandle,uint8_t* pTxBuf,uint32_t Len)
{
	if(pUSARTHandle->TxState == USART_STATE_TXBSY)
	{
		return DISABLE;//Return error if tx is already busy.
	}
	//NVIC_IRQ_Control(); I will add enable interrupt on nvic side if its not enabled later.
	
	pUSARTHandle->TxLen = Len;
	pUSARTHandle->pTxBuffer = pTxBuf;
	pUSARTHandle->TxState = USART_STATE_TXBSY;
				
	//Enable USART Interrupts
	pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_TXEIE);
	//pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_TCIE);We dont want TC working all the time, we will enable it in TX interrupt if len is zero
	//Add CTS,RTS and Parity codes later.
	return ENABLE;
}
uint8_t USART_ReceiveIT(USART_Handle_ts* pUSARTHandle,uint8_t* pRxBuf,uint32_t Len)
{
	if(pUSARTHandle->RxState == USART_STATE_RXBSY)
	{
		return DISABLE;//Return error if tx is already busy.
	}
	
	pUSARTHandle->RxLen = Len;
	pUSARTHandle->pRxBuffer = pRxBuf;
	pUSARTHandle->RxState = USART_STATE_RXBSY;
	
	//Enable the USART interrupts
	pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_RXNEIE);
	
	return ENABLE;
}

void USART_SetBaudRate(USART_RegDef_ts* pUSART,uint32_t BaudRate)
{
	//First get the actual peripheral clock of the usart.
	uint32_t PCLK;
	uint32_t usartdiv;
	uint32_t mantissa,fraction;
	uint32_t tempreg=0;
	
	if(pUSART== USART1)
	{
		PCLK = RCC_GetPCLK_APB2();
	}
	else if((pUSART == USART2)||(pUSART == USART3))
	{
		PCLK = RCC_GetPCLK_APB1();
	}
	
	//Standard Formula for USARTDIV:
	//BAUD RATE = Fck/(8*(2-OVER8)*USARTDIV)
	//That makes USARTDIV = Fck/(8*(2-OVER8)*BAUDRATE)
	//To avoid floating numbers, we will first multiply it by a hundred.
	//USARTDIV= (25*Fck)/(2*(2-OVER8)*BAUDRATE)
	if(pUSART->USART_CR1&(1<<USART_CR1_OVER8))
	{
		usartdiv = (25*PCLK)/(2*BaudRate);
	}
	else
	{
		usartdiv = (25*PCLK)/(4*BaudRate);
	}
	mantissa = usartdiv/100;//We get the actual whole number part.
	fraction = usartdiv-(mantissa*100);//We get the fraction part.
	
	if(pUSART->USART_CR1&(1<<USART_CR1_OVER8))
	{
		fraction =((fraction*8)+50)/100;//In oversampling by 8, we multiply the fraction by 8 times. To round off the number correctly, we add 50 before the division.
		//When OVER8=1, the fractional part is coded on 3 bits and programmed by the
		//DIV_fraction[2:0] bits in the USART_BRR register, and bit DIV_fraction[3] must be kept cleared.
		fraction &= 0x07;//We mask the first three bits.Maybe need to typecast?
	}
	else
	{
		fraction =((fraction*16)+50)/100;//The same concept when oversampling by 16.
		fraction &= 0x0F;//We mask the first four bits.Maybe need to typecast?
	}
	
	tempreg |= (mantissa<<USART_BRR_MANTISSA);//Set the mantissa part
	tempreg |= fraction;//Set the fraction part
	
	pUSART->USART_BRR = tempreg;
	
}
void USART_IRQHandler(USART_Handle_ts* pUSARTHandle)
{
	//First lets find which interrupt arrived. Interrupts are listed below.
	
	//TC Flag Transmission Complete : TCIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_TC)&&(pUSARTHandle->USARTx->USART_CR1 & (1<<USART_CR1_TCIE)))
	{
		//Handle TC TC will only run when Len==0 because we disable it and only activate the interrupt on Len==0 in TX interrupt.

		//Clear the TC Flag: Read SR(Already done), then write to DR.(causes error)
		//pUSARTHandle->USARTx->USART_DR |= 0x0000;//Clearing the LSB 16.
		//This last write to the DR is perceived as a new transmission if another device is already listening the Tx(Trying to receive)
		//So, TC will be cleared when a new transmission is started and new data is written there. TC = 1 doesnt equal to an error.
			
		pUSARTHandle->USARTx->USART_CR1 &= ~(1<<USART_CR1_TCIE);//Clear the TC interrupt enable.
			
		pUSARTHandle->TxState = USART_STATE_READY;
			
		//Call the Application Callback Function, TX is complete.
		USART_ApplicationCallback(pUSARTHandle,USART_EV_TXCMPLT);
		
	}
	
	//TXE Flag: TXEIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_TXE)&&(pUSARTHandle->USARTx->USART_CR1 & (1<<USART_CR1_TXEIE)))
	{
		//Handle TXE
		if(pUSARTHandle->TxState == USART_STATE_TXBSY)
		{
			if((pUSARTHandle->USARTConfig->WordLen == USART_WORDLEN_9)&&(pUSARTHandle->USARTConfig->ParityMode==USART_PRTY_DI))
			{//If wordlen is 9bits and the Parity is disabled, we must write 9 bits of user data and increment pointer twice.
				uint16_t* pData = (uint16_t*)pUSARTHandle->pTxBuffer;
				pUSARTHandle->USARTx->USART_DR = (uint16_t)(*pData&(uint16_t)0x01FF);//Mask the LSB 9 bits of data, then write to the DR.
				pUSARTHandle->pTxBuffer += 2;//Increment the buffer by two.
			}
			else//Else, we just write 8bit data.
			{
				pUSARTHandle->USARTx->USART_DR = (uint8_t)(*pUSARTHandle->pTxBuffer&((uint8_t)0x00FF));
				pUSARTHandle->pTxBuffer++;
			}
			pUSARTHandle->TxLen--;
			if(pUSARTHandle->TxLen == 0)
			{
				pUSARTHandle->USARTx->USART_CR1 &= ~(1<<USART_CR1_TXEIE);//Disable the TXE interrupt.
				pUSARTHandle->USARTx->USART_CR1 |= (1<<USART_CR1_TCIE);//Enable the transmit complete interrupt.
				
			}
		}	
	}
	
	//RXNE Flag : RXNEIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_RXNE)&&(pUSARTHandle->USARTx->USART_CR1 & (1<<USART_CR1_RXNEIE)))
	{
		//Handle RXNE
		if(pUSARTHandle->RxState == USART_STATE_RXBSY)
		{
			if((pUSARTHandle->USARTConfig->WordLen == USART_WORDLEN_9) &&(pUSARTHandle->USARTConfig->ParityMode == USART_PRTY_DI))
			{//We need to get 9 bits of data in this case.
				uint16_t* pData = (uint16_t*)pUSARTHandle->pRxBuffer;
				*pData= (uint16_t)(pUSARTHandle->USARTx->USART_DR&(uint16_t)0x01FF);//We mask the 9 bits
				pUSARTHandle->pRxBuffer += 2;
			}
			else
			{
				if((pUSARTHandle->USARTConfig->WordLen == USART_WORDLEN_8) &&(pUSARTHandle->USARTConfig->ParityMode!=USART_PRTY_DI))
				{//If the mode is 8bit wordlength with parity, we must take 7 bits.
					*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->USARTx->USART_DR&(uint8_t)0x007F);
				}
				else
				{//Else we read 8bits
					*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->USARTx->USART_DR&(uint8_t)0xFF);
				}
				pUSARTHandle->pRxBuffer++;
			}
			pUSARTHandle->RxLen--;
			if(pUSARTHandle->RxLen == 0)
			{//Receiving is complete
				pUSARTHandle->USARTx->USART_CR1 &= ~(1<<(USART_CR1_RXNEIE));//Disable the RXNE interrupt.
				pUSARTHandle->RxState = USART_STATE_READY;
				USART_ApplicationCallback(pUSARTHandle,USART_EV_RXCMPLT);
			}
			
		}
	}
	
	//CTS Flag Clear To Send : CSTIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_CTS)&&(pUSARTHandle->USARTx->USART_CR3 & (1<<USART_CR3_CTSIE)))
	{
		//Handle CTS
		pUSARTHandle->USARTx->USART_SR &= ~(1<<USART_SR_CTS);//Clear the CTS Flag
		USART_ApplicationCallback(pUSARTHandle,USART_EV_CTS);//Call application callback
	}
	
	//IDLE Flag Idle Line Detected : IDLEIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_IDLE)&&(pUSARTHandle->USARTx->USART_CR1 & (1<<USART_CR1_IDLEIE)))
	{
		//Handle IDLE interrupt
		uint16_t dummyread;
		dummyread = pUSARTHandle->USARTx->USART_DR;//Clearing the IDLE flag sequence:Read SR(Done). Read DR
		(void)dummyread;
		USART_ApplicationCallback(pUSARTHandle,USART_EV_IDLE);
	}
	
	//ORE Flag Overrun Error : RXNEIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_ORE)&&(pUSARTHandle->USARTx->USART_CR1 & (1<<USART_CR1_RXNEIE)))
	{
		//Handle Overrun Error
		
		USART_ApplicationCallback(pUSARTHandle,USART_EV_ORE);//Clearing of ORE is a read to the SR(Done). And a read from DR. 
		//This should be done in Application Callback since we dont know what user wants to do with Overrun Data.
		
	}
		
	//PE Flag Parity Error : PEIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_PE)&&(pUSARTHandle->USARTx->USART_CR1 & (1<<USART_CR1_PEIE)))
	{
		//Handle Parity
		USART_ApplicationCallback(pUSARTHandle,USART_EV_PE);//This Parity Error will need a lot of work later.
	}
	
	//LBD Flag Break Flag LBDIE
	if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_LBD)&&(pUSARTHandle->USARTx->USART_CR2 & (1<<USART_CR2_LBDIE)))
	{
		//Handle LBD
		USART_ApplicationCallback(pUSARTHandle,USART_EV_LBDE);
	}
	
	//NF Noise Flag || ORE Flag Overrun Error || FE Flag Framing Error in multibuffer com : EIE
	if(pUSARTHandle->USARTx->USART_CR3 & (1<<USART_CR3_EIE))//All these Errors Depend on EIE(Error Interrupt Enable
	{
		if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_NF))
		{
			USART_ApplicationCallback(pUSARTHandle,USART_EV_NE);//Handle Noise Error
		}
		if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_ORE))
		{
			USART_ApplicationCallback(pUSARTHandle,USART_EV_ORE);//Handle Overrun Error
		}
		if(USART_GETFLAG(pUSARTHandle->USARTx,USART_SR_FE))
		{
			USART_ApplicationCallback(pUSARTHandle,USART_EV_FE);//Handle Framing Error
		}
	}
	
	
	
}

__attribute__((weak)) void USART_ApplicationCallback(USART_Handle_ts* pUSARTHandle, uint8_t Event)
{
	(void)pUSARTHandle;
	(void)Event;
}
