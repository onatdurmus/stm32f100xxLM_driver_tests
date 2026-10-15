#ifndef STM32F100LM_I2C_DRIVER_H
#define STM32F100LM_I2C_DRIVER_H

#include "stm32F100xxLMdensity.h"

typedef struct
{
	uint16_t SCLSpeed;//I2C SCL Speed. Must be given in KHz. Fast mode is up to 400kbit/s. Standard Mode is up to 100kbit/s
	uint8_t OwnAddress;//Only 7 bits are valid. This driver only support 7 bit adressing.
	uint8_t Acking;//@I2C Ack Control
	uint8_t DutyCycle;//@I2C Duty Cycle Options
	uint8_t ClockStretching;//@I2C Clock Stretch
}I2C_Config_ts;

typedef struct
{
	I2C_RegDef_ts* pI2C;
	I2C_Config_ts I2CConfig;
	uint8_t  SlaveAddr;
	uint8_t  TxRxState;//@I2C States
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t* pTxBuffer;
	uint8_t* pRxBuffer;
	uint8_t  RepeatedStart;
	uint8_t	 DevMode;//@I2C Device Modes
	uint8_t	 ComDirection;//@I2C Communication Direction
	uint8_t	 ErrorEvent;//@I2C Error Types
	
}I2C_Handle_ts;


#define I2C_R_NW_WRITE		0
#define I2C_R_NW_READ			1

#define I2C_RS_TRUE				1//I2C Repeated Start 
#define I2C_RS_FALSE			0

/* I2C CONFIGURATION MACROS START*/

//@I2C SCLSpeed

//!!!Value must be 0-400 !!!

//@I2C Ack Control
#define I2C_ACKING_ENABLED	1//Acknowledge returned after a byte is received.
#define I2C_ACKING_DISABLED 0

//@I2C Duty Cycle Options
//Duty Cycle Options for Fast Mode
#define I2C_FM_DUTY2			0//tlow/thigh=2
#define I2C_FM_DUTY16_9		1//tlow/thigh=16/9

//@I2C Clock Stretch Options
#define I2C_CLKSTRETCH_ENABLED 	0
#define I2C_CLKSTRETCH_DISABLED 1

//@I2C States
#define I2C_STATE_READY					0
#define I2C_STATE_BSY_IN_RX			1
#define I2C_STATE_BSY_IN_TX			2



/* I2C CONFIGURATION MACROS END*/

/* I2C HANDLE STRUCTURE MACROS START*/

//@I2C Device Modes
#define I2C_DEVMODE_MSTR				1
#define I2C_DEVMODE_SLV					2

//@I2C Communication Direction
#define I2C_COMDIR_TRNSMTR			1
#define I2C_COMDIR_RCVR					2
#define I2C_COMDIR_IDLE					3

//@I2C Error Types
#define I2C_ERR_EARLYSTOP				1
#define I2C_ERR_AF							2

/* I2C HANDLE STRUCTURE MACROS END*/

/* I2C REGISTER BIT POSITIONS START */

#define	I2C_CR1_PE					0	//Peripheral Enable
#define	I2C_CR1_SMBUS				1
#define	I2C_CR1_SMBTYPE			3
#define	I2C_CR1_ENARP				4
#define	I2C_CR1_ENPEC				5
#define	I2C_CR1_ENCC				6
#define	I2C_CR1_NOSTRETCH		7	//Clock Stretching Disable(Slave Mode) 0:Clock Stretching Enabled 1: Clock stretching disabled

#define	I2C_CR1_START				8	//Star Generation. In Master Mode 0:No Start Generation 1:Repeated Start Generation. 
//In Slave Mode: 0 No start generation 1: Start generation when bus is free

#define	I2C_CR1_STOP				9	//Stop Generation. In Master Mode 0:No Stop Generation 1: Stop generation after the current byte transfer or after current Start condition is sent.
//In Slave Mode 0: No stop Generation 1:Release SDA and SCL after the current byte transfer.

#define	I2C_CR1_ACK					10//Acknowledge Enable 0:No Acking 1: Ack returned after a byte is received. 
#define	I2C_CR1_POS					11
#define	I2C_CR1_PEC					12
#define	I2C_CR1_ALERT				13
#define	I2C_CR1_SWRST				15//Software Reset : When set, i2c is under reset state. Before resetting this bit, make sure i2c lines are released and bus is free.

#define	I2C_CR2_FREQ				0	//Freq Bits are [5:0]. The FREQ bits must be configured with the APB clock frequency value (I2C peripheral connected to APB). Minimum Val is 2MHz and Max is 50MHz. 10:2MHz 11:3MHz and so on.
#define	I2C_CR2_ITERREN			8//Error Interrupt Enable. Int is generated when: BERR=1, ARLO=1, AF=1, OVR=1, PECERR=1, TIMEOUT=1, SMBALERT=1
#define	I2C_CR2_ITEVTEN			9//1:Event Interrupt Enabled. This interrupt is generated when: SB=1(Master), ADDR=1(Master/Slave), ADD10=1 (Master), STOPF=1(Slave),
//BTF =1 with no TXE or RXNE event, TXE event to 1 if ITBUFEN=1, RXNE event to 1 if ITBUFEN=1

#define	I2C_CR2_ITBUFEN			10//Buffer interrupt enable. 0: TXE or RXNE doesnt generate interrupt 1:TXE or RXNE generates interrupt.
#define	I2C_CR2_DMAEN				11//Event Interrupt enable
#define	I2C_CR2_LAST				12

#define	I2C_OAR1_ADD0				0	//7Bit Mode:Dont Care, 10Bit Mode: bit 0 of address
#define	I2C_OAR1_ADD7_1			1	//Bits 7:1 of address
#define	I2C_OAR1_ADD9_8			8	//7Bit Mode:Dont Care, 10Bit Mode: bits 9:8 of address
#define	I2C_OAR1_ADDMODE		15//Addressing Mode(Slave Mode) 0: 7Bit Mode 1: 10Bit Mode

#define I2C_SR1_SB					0//Start Bit(Master Mode) 0:No start condition. 1: Start Condition Generated. Set when a start cond. is generated. 
//Cleared by SW by reading the SR1 register followed by writing the DR register, or by hardware when PE=0

#define I2C_SR1_ADDR				1//Address sent(master)/Address matched(Slave)This bit is cleared by software reading SR1 register followed reading SR2, or by hardware when PE=0.
//Slave 1:Received address matched. Set by hw as soon as received slave address matched with OAR. In slave mode, it is recommended to perform the complete clearing sequence (READ SR1 then READ SR2) after ADDR is set.
//Master 1: End of address transmission. For 7Bit addressing, the bit is set after the ACK of the byte(Not set if NACK)

#define I2C_SR1_BTF					2//Byte transfer finished. 0:Data byte transfer not done. 1:Data byte transfer succeeded.
//Set by hw when NoStretch=0 AND -In reception when a new byte is received including ACK and DR has not been read yet (RXNE=1)
//Set by hw when NoStretch=0 AND -In transmission when a new byte should be sent and DR has not been written yet(TXE =1)
//Cleared by sw reading SR1 followed by either a read or write in the DR register or by hardware after a start or a stop condition in transmission or when PE=0.
//The BTF bit is not set after NACK reception.

#define I2C_SR1_ADD10				3
#define I2C_SR1_STOPF				4//Stop detection(Slave) 1: Stop condition detected. Set by hw when a stop cond is detected by the slave after an acknowledge if ACK=1
//Cleared by software reading the SR1 register followed by a write in the CR1 register, or by hardware when PE=0
//STOPF bit is not set after a NACK reception.

#define I2C_SR1_RXNE				6//Data register not empty(receivers).
//– Set when data register is not empty in receiver mode. RxNE is not set during address phase.
//– Cleared by software reading or writing the DR register or by hardware when PE=0.
//RxNE is not cleared by reading data when BTF is set, as the data register is still full.

#define I2C_SR1_TXE					7//Data Register Empty(Transmitters)
//– Set when DR is empty in transmission. TxE is not set during address phase.
//– Cleared by software writing to the DR register or by hardware after a start or a stop condition or when PE=0.
//TxE is not set if either a NACK is received, or if next byte to be transmitted is PEC (PEC=1)
//TxE is not cleared by writing the first data being transmitted, or by writing data when BTF is set, as in both cases the data register is still empty.

#define I2C_SR1_BERR				8//Bus Error. 1: Misplaced start or stop condition. Cleared by software writing 0, or by hardware when PE=0.
#define I2C_SR1_ARLO				9//Arbitration lost(Master)
#define I2C_SR1_AF					10//Acknowledge failure.1:ACK Failure. Set by hw when no ACK is returned. Cleared by sw by writing 0, or by hw when PE=0
#define I2C_SR1_OVR					11//Overrun or Underrun error. Set by hw when NoStertch=1 AND: In reception when a new byte is received (including ACK pulse) and the DR register has not been read yet. New received byte is lost.
//In transmission when a new byte should be sent and the DR register has not been written yet. The same byte is sent twice.
//– Cleared by software writing 0, or by hardware when PE=0.

#define I2C_SR1_PECERR			12
#define I2C_SR1_TIMEOUT			14//Only available in SMBUS mode.1: SCL remained LOW for 25 ms (Timeout) or
//Master cumulative clock low extend time more than 10 ms (Tlow:mext) or
//Slave cumulative clock low extend time more than 25 ms (Tlow:sext)
//– When set in slave mode: slave resets the communication and lines are released by hardware
//– When set in master mode: Stop condition sent by hardware

#define I2C_SR1_SMBALERT		15

#define I2C_SR2_MSL					0//1:Master,0:Slave Set by hw as soon as interface is in master mode(SB=1),cleared by hw after detecting a stop condition, or when ARLO=1 or PE=0
#define I2C_SR2_BSY					1//0:No com on the bus. 1: Communication ongoing on the bus. Set by hw on detection
#define I2C_SR2_TRA					2//Transmitter/Receiver 0:Data Bytes Received 1:Data Bytes Transmitted. This bit is set depending on the R/W bit of the address byte, at the end of total address phase.
//It is also cleared by hardware after detection of Stop condition (STOPF=1), repeated Start condition, loss of bus arbitration (ARLO=1), or when PE=0.

#define I2C_SR2_GENCALL			4
#define I2C_SR2_SMBDEFAULT	5
#define I2C_SR2_SMBHOST			6
#define I2C_SR2_DUALF				7
#define I2C_SR2_PEC					8

#define I2C_CCR_CCR					0//Long explanation. Read Technical Reference Manual
#define I2C_CCR_DUTY				14//Duty cycle select for FM. 0:Fm mode tlow/thigh =2 1: Fm mode tlow/thigh=16/9
#define I2C_CCR_F_S					15//0:Standard Mode I2C 1:Fast Mode I2C. FPCLK must be at least 2MHz for SM and 4MHz for FM

#define I2C_TRISE_TRISE			0//[5:0] Bits are valid. These bits must be programmed with the maximum SCL rise time given in the I2C bus specification, incremented by 1.
//For instance: in Sm mode, the maximum allowed SCL rise time is 1000 ns.
//If, in the I2C_CR2 register, the value of FREQ[5:0] bits is equal to 0x08 and TPCLK1 = 125 ns
//therefore the TRISE[5:0] bits must be programmed with 09h. (1000 ns / 125 ns = 8 + 1)
//TRISE[5:0] must be configured only when periph is disabled. (PE=0)

/* I2C REGISTER BIT POSITIONS END */

/* I2C INTERRUPT TYPES EVENT AND ERROR MACROS*/

#define	I2C_INTEV_START			1
#define I2C_INTEV_ADDR			2
#define I2C_INTEV_STOP			3
#define I2C_INTEV_BTF				4
#define I2C_INTEV_RXNE			5
#define I2C_INTEV_TXE				6

#define I2C_INTERR_BERR			7
#define I2C_INTERR_ARLO			8
#define I2C_INTERR_AF				9
#define I2C_INTERR_OV				10
#define I2C_INTERR_PECERR		11
#define I2C_INTERR_TIMEOUT	12
#define I2C_INTERR_SMBALERT	13

/* I2C INTERRUPT TYPES EVENT AND ERROR MACROS*/

/* I2C APPLICATION CALLBACK EVENTS START*/
#define I2C_CLLBK_EARLYSTOP	1
#define I2C_CLLBK_RXCMPLT		2
#define I2C_CLLBK_TXCMPLT		3
#define I2C_CLLBK_ACKFAIL		4
/* I2C APPLICATION CALLBACK EVENTS END*/

/*	I2C FUNCTION DECLARATIONS	*/
void I2C_PeriphControl(I2C_RegDef_ts* pI2C, uint8_t EnOrDi);
uint8_t I2C_Init(I2C_Handle_ts* pI2CHandle);
void I2C_ExecuteAddressPhase(I2C_RegDef_ts* pI2C, uint8_t SlaveAddr,uint8_t ReadWrite);
void I2C_GenerateStartCondition(I2C_RegDef_ts* pI2C);
void I2C_GenerateStopCondition(I2C_RegDef_ts* pI2C);
void I2C_ClearAddrFlag(I2C_RegDef_ts* pI2C);
void I2C_ManageAcking(I2C_RegDef_ts* pI2C,uint8_t EnorDi);
uint8_t I2C_MasterTransmitBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len, uint8_t SlaveAddr,uint8_t RepeatedStart);//Function for blocking transmitting
uint8_t I2C_MasterReceiveBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len, uint8_t SlaveAddr, uint8_t RepeatedStart);//Function for blocking receiving
uint8_t I2C_SlaveTransmitBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len);
uint8_t I2C_SlaveReceiveBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len);
void I2C_ClearStopFlag(I2C_RegDef_ts* pI2C);
uint8_t I2C_WaitRxneTimeout(I2C_Handle_ts* pI2CHandle);
uint8_t I2C_WaitStopFlagTimeout(I2C_Handle_ts* pI2CHandle);
void I2C_IRQEventHandler(I2C_Handle_ts* pI2CHandle);
void I2C_IRQErrorHandler(I2C_Handle_ts* pI2CHandle);
void I2C_IRQHandleStop(I2C_Handle_ts* pI2CHandle);
void I2C_IRQHandleStart(I2C_Handle_ts* pI2CHandle);
void I2C_IRQHandleAddr(I2C_Handle_ts* pI2CHandle);
void I2C_IRQHandleBtf(I2C_Handle_ts* pI2CHandle);
void I2C_IRQHandleTxE(I2C_Handle_ts* pI2CHandle);
void I2C_IRQHandleRxNE(I2C_Handle_ts* pI2CHandle);
void I2C_Flush_DR(I2C_RegDef_ts* pI2C);
__attribute__ ((weak))void I2C_ApplicationCallback(I2C_Handle_ts* pI2CHandle,uint8_t CallbackEvent);
uint8_t I2C_MasterTransmitIT(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len, uint8_t SlaveAddr,uint8_t RepeatedStart);//Function for transmitting with interrupts
uint8_t I2C_MasterReceiveIT(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len, uint8_t SlaveAddr,uint8_t RepeatedStart);//Function for receiving with interrupts
uint8_t I2C_SlaveTransmitIT(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len);
uint8_t I2C_SlaveReceiveIT(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len);
#endif
