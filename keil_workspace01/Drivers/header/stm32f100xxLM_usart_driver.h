#ifndef STM32F100LM_USART_DRIVER_H
#define STM32F100LM_USART_DRIVER_H

#include "stm32F100xxLMdensity.h"

typedef struct
{
	uint8_t WordLen;//@USART Word Length Options
	uint8_t StopBits;//@USART Stop Bit Options
	uint32_t BaudRate;//@USART BaudRate Options
	uint8_t UsartMode;//@USART Mode Options
	uint8_t ParityMode;//@USART Parity Mode Options
	uint8_t HWControl;//@USART Hardware Flow Control Options
}USART_Config_ts;

/*USART Configuration Structure Options START*/
//@USART Word Length Options WordLen
#define USART_WORDLEN_8							0//Data Length is 8 Bits excluding start and stop bits.
#define USART_WORDLEN_9							1//Data Length is 9 Bits excluding start and stop bits.
//@USART Stop Bit Options Stopbits
#define USART_STOP_1								0
#define USART_STOP_0_5							1
#define USART_STOP_2								2
#define USART_STOP_1_5							3
//@USART Baud Rate Options Baudrate
#define USART_STD_BAUD_1200					1200
#define USART_STD_BAUD_2400					2400
#define USART_STD_BAUD_9600					9600
#define USART_STD_BAUD_19200 				19200
#define USART_STD_BAUD_38400 				38400
#define USART_STD_BAUD_57600 				57600
#define USART_STD_BAUD_115200 			115200
#define USART_STD_BAUD_230400 			230400
#define USART_STD_BAUD_460800 			460800
#define USART_STD_BAUD_921600 			921600
#define USART_STD_BAUD_2M 					2000000
#define USART_STD_BAUD_3M 					3000000
//@USART Mode Options UsartMode
#define USART_MODE_TXONLY						1
#define USART_MODE_RXONLY						2
#define USART_MODE_TXRX							3
//@USART Parity Mode Options ParityMode
#define USART_PRTY_DI								1
#define USART_PRTY_EVEN							2
#define USART_PRTY_ODD							3
//@USART Hardware Flow Control Options HWControl
#define USART_HWCNTRL_DI						0
#define USART_HWCNTRL_CTS						1
#define USART_HWCNTRL_RTS						2
#define USART_HWCNTRL_CTSRTS				3
/*USART Configuration Structure Options END*/


typedef struct
{
	USART_RegDef_ts* USARTx;
	USART_Config_ts* USARTConfig;
	uint8_t* pTxBuffer;
	uint8_t* pRxBuffer;
	uint32_t	 TxLen;
	uint32_t	 RxLen;	
	uint8_t TxState;
	uint8_t RxState;
}USART_Handle_ts;

//USART States
#define USART_STATE_TXBSY				1
#define USART_STATE_RXBSY				2
#define USART_STATE_READY				3

/*	USART REGISTER BIT DEFINITIONS START*/

//USART_SR STATUS REGISTER Bit Definitions
#define USART_SR_PE					0//Parity Error
#define USART_SR_FE					1//Framing Error
#define USART_SR_NF					2//Noise Detected Flag
#define USART_SR_ORE				3//Overrun Error
#define USART_SR_IDLE				4//IDLE Line Detected
#define USART_SR_RXNE				5
#define USART_SR_TC					6//Transmission Complete.This bit is set by hardware if the transmission of a frame containing data is complete and if TXE is set.
#define USART_SR_TXE				7
#define USART_SR_LBD				8//LIN break detection flag
#define USART_SR_CTS				9//CTS(Clear to Send) flag

//USART DR has 9LSB's for usage.

//USART_BRR BAUD RATE REGISTER Bit Definitions
#define USART_BRR_FRACTION	0//DIV_Fraction has 4 bits [3:0]
#define USART_BRR_MANTISSA	4//DIV_Mantissa has 12 bits[15:4]

//USART_CR1 CONTROL REGISTER 1 Bit Definitions
#define USART_CR1_SBK				0//Send Break. This bit set is used to send break characters. It can be set and cleared by software. It should be set by software, and is reset by hardware during the stop bit of break.
#define USART_CR1_RWU				1//Receiver Wakeup. This bit determines if the USART is in mute mode or not. It is set and cleared by software and can be cleared by hardware when a wakeup sequence is recognized.
#define USART_CR1_RE				2//Receiver Enable. This bit enables the receiver. It is set and cleared by SW.
#define USART_CR1_TE				3//Transmitter Enable. This bit enables the transmitter. It is set and cleared by SW.
#define USART_CR1_IDLEIE		4//Idle Interrupt Enable
#define USART_CR1_RXNEIE		5//RXNE Interrupt Enable
#define USART_CR1_TCIE			6//Transmission Complete Interrupt Enable
#define USART_CR1_TXEIE			7//TXE Interrupt Enable
#define USART_CR1_PEIE			8//PE(Parity Error)Interrupt Enable
#define USART_CR1_PS				9//Parity Selection. 0:Even Parity, 1:Odd Parity
#define USART_CR1_PCE				10//Parity Control Enable. 0: PC Disabled, 1: Enabled
#define USART_CR1_WAKE			11//Determines USART Wakeup method. 0:Idle Line, 1: Address Mark
#define USART_CR1_M					12//Word Length. 0: 1 Start bit, 8 Data Bits n Stop bit. 1: 1 Start Bit, 9Data Bits n Stop bit
#define USART_CR1_UE				13//USART Enable. 0:Usart Prescaler and Outputs are disabled, 1: Usart Enabled
#define USART_CR1_OVER8			15//0: Oversampling by 16, 1 : Oversampling by 8

//USART_CR2 CONTROL REGISTER 2 Bit Definitions
#define USART_CR2_ADD				0//Bits[3:0]Address of the USART node. Used for multiprocessor com during mute mode, or for wakeup with address mark detection
#define USART_CR2_LBDL			5//lin break detection length.
#define USART_CR2_LBDIE			6//LIN break detection interrupt enable
#define USART_CR2_LBCL			8//Last bit clock value. In synchronous mode, 0: Clock pulse of last data bit is not output to CK pin. 1: It is output to CK pin.
#define USART_CR2_CPHA			9//Clock Phase. 0: First clock transition is the first data capture edge. 1: Second clock transition is the first data capture
#define USART_CR2_CPOL			10//Clock Polarity. 0: Steady low on CK outside of transmission window. 1: Steady high on CK pin outside of transmission window
#define USART_CR2_CLKEN			11//Clock Enable. 0: CK pin disabled, 1: CK pin enabled.
#define USART_CR2_STOP			12//Bits [13:12] Number of stop bits. 00:1 Stop bit, 01: 0.5 Stop bit, 10: 2 Stop Bits, 11: 1.5 Stop bit
#define USART_CR2_LINEN			14//LIN Mode Enable. 0: Disabled, 1: Enabled.

//USART_CR3 CONTROL REGISTER 3 Bit Definitions
#define USART_CR3_EIE				0//Error Interrupt Enable. Error Interrupt Enable Bit is required to enable interrupt generation in case of a framing error, overrun error or noise flag
#define USART_CR3_IREN			1//IrDA mode enable
#define USART_CR3_IRLP			2//IrDA low-power
#define USART_CR3_HDSEL			3//Half-duplex selection. 0: Not selected, 1: Half-Duplex mode selected
#define USART_CR3_NACK			4//Smartcard NACK enable. 0: NACK transmission in case of parity error is disabled. 1: NACK transmission during parity error is enabled
#define USART_CR3_SCEN			5//Smartcard mode enable
#define USART_CR3_DMAR			6//DMA enable receiver
#define USART_CR3_DMAT			7//DMA enable transmitter
#define USART_CR3_RTSE			8//Request to Send Enable
#define USART_CR3_CTSE			9//Clear to Send Enable
#define USART_CR3_CTSIE			10//Clear to send Interrupt Enable
#define USART_CR3_ONEBIT		11//One sample bit method enable. This bit allows the user to select the sample method. When the one sample bit method is selected the noise detection flag (NF) is disabled.
//0: Three sample bit method
//1: One sample bit method
//Note: The ONEBIT feature applies only to data bits. It does not apply to START bit.

//USART_GTPR GUARD TIME AND PRESCALER REGISTER Bit Definitions
#define USART_GTPR_PSC			0//[7:0]Prescaler value
#define USART_GTPR_GT				8//[15:8]Guard Time?

/*	USART REGISTER BIT DEFINITIONS END*/


/*USART APPLICATION CALLBACK EVENT DEFINITIONS*/
#define USART_EV_TXCMPLT						1
#define USART_EV_RXCMPLT						2
#define USART_EV_IDLE								3
#define USART_EV_CTS								4
#define USART_EV_PE									5
#define USART_EV_ORE								6
#define USART_EV_FE									7
#define USART_EV_NE									8
#define USART_EV_LBDE								9

/*USART FUNCTION DECLARATIONS START */

void USART_PeriCLK_Control(USART_RegDef_ts* pUSART,uint8_t EnorDi);
void USART_PeriControl(USART_RegDef_ts* pUSART, uint8_t EnorDi);
uint8_t USART_GetFlag(USART_RegDef_ts* pUSART,uint8_t UsartFlag);
void USART_Init(USART_Handle_ts* pUSARTHandle);
void USART_DeInit(USART_RegDef_ts* pUSART);
uint8_t USART_SendBlocking(USART_RegDef_ts* USARTx,uint8_t* pTxBuf,uint32_t Len);
uint8_t USART_ReceiveBlocking(USART_RegDef_ts* USARTx,uint8_t* pRxBuf,uint32_t Len);
uint8_t USART_TransmitIT(USART_Handle_ts* pUSARTHandle,uint8_t* pTxBuf,uint32_t Len);
uint8_t USART_ReceiveIT(USART_Handle_ts* pUSARTHandle,uint8_t* pRxBuf,uint32_t Len);
void USART_SetBaudRate(USART_RegDef_ts* pUSART,uint32_t BaudRate);
void USART_IRQHandler(USART_Handle_ts* pUSARTHandle);

void USART_ApplicationCallback(USART_Handle_ts* pUSARTHandle, uint8_t Event);

#define USART_GETFLAG(x,y)	(((x)->USART_SR & (1U << (y))) != 0U)//Bu fonksiyonu da deneriz sonra.
/*USART FUNCTION DECLARATIONS END */

#endif