#include "stm32f100xxLM_i2c_driver.h"


void I2C_PeriphControl(I2C_RegDef_ts* pI2C, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE){
		pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);
	}
	else if(EnOrDi == DISABLE){
		pI2C->I2C_CR1 &= ~(1<<I2C_CR1_PE);
	}
	
}



uint8_t I2C_Init(I2C_Handle_ts* pI2CHandle){
	
	//1.Enable Peripheral Clock
	I2C1_PCLK_EN();
	
	//2.Configure Clock Stretching
	if(pI2CHandle->I2CConfig.ClockStretching == I2C_CLKSTRETCH_ENABLED){
		pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_NOSTRETCH);
	}
	else if(pI2CHandle->I2CConfig.ClockStretching == I2C_CLKSTRETCH_DISABLED){
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_NOSTRETCH);
	}
	//3.Configure Own Address
	//We Only Support 7Bit Addressing in this driver.
	pI2CHandle->pI2C->I2C_OAR1 &= ~(1<<I2C_OAR1_ADDMODE);//Clears this bit, 7 bit addressing mode.
	pI2CHandle->pI2C->I2C_OAR1 |= (pI2CHandle->I2CConfig.OwnAddress<<I2C_OAR1_ADD7_1);
	
	//4.Get the peripheral clock
	uint32_t i2cPclk = RCC_GetPCLK_APB1();
	i2cPclk = i2cPclk/1000000;//We will program the freq bits in MHz
	
	//5.Configure periph FREQ bits
	if((i2cPclk<2)||(i2cPclk>50)){
		return DISABLE;//Below 2MHz or above 50 MHz is not allowed.
	}
	i2cPclk &= (0x1F);//Mask the first 6 bits for protection
	pI2CHandle->pI2C->I2C_CR2 &= ~(0x3F);//Clear the Freq bits. They are the first 6 bits.
	pI2CHandle->pI2C->I2C_CR2 |= i2cPclk;//Configure the Freq bits.
	
	
	//7.Configure FastMode/StandardMode in CCR
	uint16_t ccr;
	if(pI2CHandle->I2CConfig.SCLSpeed<=100){//Standard Mode allows up to 100KHz
		//Do Standard mode configurations here
		pI2CHandle->pI2C->I2C_CCR &= ~(1<<I2C_CCR_F_S);//Clear the F/M mode bit. Clear means standard mode.
		//Duty is dont care in standard mode.
		
		//Configure CCR: Standard Mode CCR= fpclk/(fi2cclk*2) You can check the coding in the TRM.
		ccr = ((i2cPclk*1000)/(pI2CHandle->I2CConfig.SCLSpeed*2));//We multiply i2cPclk with 1000, since we would have to convert it to MHz and SCL to KHz, it is simplified to 1000.
		ccr &= (0x0FFF);//Mask the LS 12 bits just in case.
		pI2CHandle->pI2C->I2C_CCR &= ~(0x0FFF);//Clear the first 12 bits.
		pI2CHandle->pI2C->I2C_CCR |= ccr;
		//In Sm Mode, Max Trise is 1000ns
		//Trise bits must be configured with Max for the Mode, divided by the period of PCLK+1. E.g. 8MhzPclk = 125ns period. 1000ns/125ns=8+1=9 must be coded.
		//Also means (1000ns*fpclk)+1. Translate 1000ns to frequency also => (fpclk/1MHz)+1. Our pclk is already in Mhz.
		pI2CHandle->pI2C->I2C_TRISE &= ~(0x3F);//Clear the first 6 bits
		pI2CHandle->pI2C->I2C_TRISE |= (i2cPclk+1);
	}
	else if((pI2CHandle->I2CConfig.SCLSpeed<=400)&&(i2cPclk>=4)){//Fast Mode Requires minimum 4MHz as pclk.
		//Do Fast mode Configurations here 
		pI2CHandle->pI2C->I2C_CCR |= (1<<I2C_CCR_F_S);//Set the F/M mode bit. Set means fast mode.
		//Duty is dont care in standard mode.
		
		if(pI2CHandle->I2CConfig.DutyCycle == I2C_FM_DUTY2){
			//Configure Duty Bit
			pI2CHandle->pI2C->I2C_CCR &= ~(1<<I2C_CCR_DUTY);//Clear Duty means tlow/thigh=2
			
			//Configure CCR: Fast Mode Duty2 CCR= fpclk/(fi2cclk*3) You can check the coding in the TRM.
		ccr = ((i2cPclk*1000)/(pI2CHandle->I2CConfig.SCLSpeed*3));//We multiply i2cPclk with 1000, since we would have to convert it to MHz and SCL to KHz, it is simplified to 1000.
		ccr &= (0x0FFF);//Mask the LS 12 bits just in case.
		pI2CHandle->pI2C->I2C_CCR &= ~(0x0FFF);//Clear the first 12 bits.
		pI2CHandle->pI2C->I2C_CCR |= ccr;
		}
		else if(pI2CHandle->I2CConfig.DutyCycle == I2C_FM_DUTY16_9){
			//Configure Duty Bit
			pI2CHandle->pI2C->I2C_CCR |= (1<<I2C_CCR_DUTY);//Clear Duty means tlow/thigh=16/9
			
			//Configure CCR: Fast Mode Duty16_9 CCR= fpclk/(fi2cclk*25) You can check the coding in the TRM.
		ccr = ((i2cPclk*1000)/(pI2CHandle->I2CConfig.SCLSpeed*25));//We multiply i2cPclk with 1000, since we would have to convert it to MHz and SCL to KHz, it is simplified to 1000.
		ccr &= (0x0FFF);//Mask the LS 12 bits just in case.
		pI2CHandle->pI2C->I2C_CCR &= ~(0x0FFF);//Clear the first 12 bits.
		pI2CHandle->pI2C->I2C_CCR |= ccr;
		}
		//Configure the Trise bits. Max Trise in FM mode is 300ns
		uint8_t trise = (((i2cPclk*3)/10)+1);//300ns/Tclk= 300ns*Pclk. Pclk is in Mhz so 10^-1 is left. 3pclk/10 is the formula transformed. Trise must be programmed with +1.
		pI2CHandle->pI2C->I2C_TRISE &= ~(0x3F);//Clear the first 6 bits
		pI2CHandle->pI2C->I2C_TRISE |= trise;//Configure the Trise
	}
	else{
		return DISABLE;//If speed requested is larger than 400, or Fm with pclk<4MHz return error(DISABLE).
	}
	
	//11.Enable Peripheral
	I2C_PeriphControl(pI2CHandle->pI2C,ENABLE);
		//6.Configure Acking
	if(pI2CHandle->I2CConfig.Acking == I2C_ACKING_ENABLED){
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_ACK);//Set this bit so acking is enabled.
	}
	else if(pI2CHandle->I2CConfig.Acking == I2C_ACKING_DISABLED){
		pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_ACK);//Clear the bit so no ack is sent.
	}
	
	return ENABLE;//Enable returned=>No error
	
}

void I2C_GenerateStartCondition(I2C_RegDef_ts* pI2C){
	pI2C->I2C_CR1 |= (1<<I2C_CR1_START);
}
void I2C_ExecuteAddressPhase(I2C_RegDef_ts* pI2C, uint8_t SlaveAddr,uint8_t ReadWrite){//Bu fonksiyonu sadece handle'i alacak sekilde degistirebilirsin.
	uint32_t dummyread;
	dummyread = pI2C->I2C_SR1;
	(void)dummyread;//Reading SR1 then writing to DR clears the SB bit.
	SlaveAddr = (SlaveAddr<<1);//Make room for the LSB R/~W bit.
	if(ReadWrite == I2C_R_NW_READ)
	{
		SlaveAddr |= 1;//Set the least significant bit.
	}
	else if(ReadWrite == I2C_R_NW_WRITE)
	{
		SlaveAddr &= ~(1);//Clear the least significant bit.
	}
	pI2C->I2C_DR = SlaveAddr;
}

void I2C_GenerateStopCondition(I2C_RegDef_ts* pI2C)
{
	pI2C->I2C_CR1 |= (1<<I2C_CR1_STOP);
}

void I2C_ClearAddrFlag(I2C_RegDef_ts* pI2C)
{
	uint32_t dummyread;	
	dummyread = pI2C->I2C_SR1;
	dummyread = pI2C->I2C_SR2;//These two reads clear the ADDR bit.
	(void)dummyread;
}

uint8_t I2C_MasterTransmitBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len, uint8_t SlaveAddr,uint8_t RepeatedStart)
{	
	if((pTxBuffer==NULL)||(Len==0)||(pI2CHandle == NULL)||(pI2CHandle->TxRxState != I2C_STATE_READY)){
		return DISABLE;}//Return error
	if(!(pI2CHandle->pI2C->I2C_CR1 & (1<<I2C_CR1_PE))){
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);//Enable Peripheral if not enabled
	}
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_TX;
	I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);//Enable Acking if not enabled.
	I2C_GenerateStartCondition(pI2CHandle->pI2C);
	while(!(pI2CHandle->pI2C->I2C_SR1 & (1<<I2C_SR1_SB)));//Wait until start bit is set
	I2C_ExecuteAddressPhase(pI2CHandle->pI2C,SlaveAddr,I2C_R_NW_WRITE);
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_ADDR)));//Wait until ADDR bit is set: Address sent and ACK received.
	I2C_ClearAddrFlag(pI2CHandle->pI2C);
	while(Len>0){
		while(!(pI2CHandle->pI2C->I2C_SR1 & (1<<I2C_SR1_TXE)));//Wait for Txe to be set.
		pI2CHandle->pI2C->I2C_DR = *pTxBuffer;
		Len--;
		pTxBuffer++;		
	}
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_TXE)));//Wait for Txe. Last byte is removed from Tx Buffer into shift reg.(Not really needed but to be sure. Btf doesnt set if Txe is not 1)
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_BTF)));//Wait for byte transfer finished bit.Shift reg is now also empty. BTF=1 and TXE=1 happens when there is an error(underrun) or when there are no bytes left.
	
	if(RepeatedStart == I2C_RS_FALSE){
	I2C_GenerateStopCondition(pI2CHandle->pI2C);}//Generate Stop if no repeated start is wanted. If it RS=1, there will be no stop generated.
	//Stop generation clears btf and txe flags.
	if(pI2CHandle->I2CConfig.Acking == I2C_ACKING_DISABLED){
		I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Disable acking if it is initalized that way for some reason....
	}
	pI2CHandle->TxRxState = I2C_STATE_READY;
	return ENABLE;//Return no error
}

void I2C_ManageAcking(I2C_RegDef_ts* pI2C,uint8_t EnorDi)
{
	if(EnorDi == ENABLE){
		pI2C->I2C_CR1 |= (1<<I2C_CR1_ACK);
	}
	else if(EnorDi == DISABLE){
		pI2C->I2C_CR1 &= ~(1<<I2C_CR1_ACK);
	}
}

uint8_t I2C_MasterReceiveBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len, uint8_t SlaveAddr, uint8_t RepeatedStart)
{
	if((pI2CHandle == NULL)||(pRxBuffer == NULL)|| (Len==0)||(SlaveAddr == NULL)||(pI2CHandle->TxRxState != I2C_STATE_READY)){
		return DISABLE;}//Check for basic parameters, return error if they are invalid.
	if(!(pI2CHandle->pI2C->I2C_CR1 & (1<<I2C_CR1_PE))){
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);//Enable Peripheral if not enabled
	}
	pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_POS);//Disable POS.
	if(pI2CHandle->I2CConfig.Acking == I2C_ACKING_ENABLED){
		I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);
	}
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_RX;
	while(pI2CHandle->pI2C->I2C_SR2 & (1<<I2C_SR2_BSY));
	I2C_GenerateStartCondition(pI2CHandle->pI2C);
	while(!(pI2CHandle->pI2C->I2C_SR1 & (1<<I2C_SR1_SB)));//Wait until start bit is set
	I2C_ExecuteAddressPhase(pI2CHandle->pI2C,SlaveAddr,I2C_R_NW_READ);
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_ADDR))){};//Wait until ADDR bit is set: Address sent and ACK received.
	
	if(Len == 1){//If we want to receive 1 byte only, ACK and STOP must be programmed right after the address phase. This is a special condition.
	I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Acking disable,clear addr and generate stop is the correct sequence for stopping.
	I2C_ClearAddrFlag(pI2CHandle->pI2C);
	if(RepeatedStart == I2C_RS_FALSE){
	I2C_GenerateStopCondition(pI2CHandle->pI2C);}//Setting the STOP bit generates stop after the current byte transfer which is the last(only) byte.
	}
	else if(Len==2){
		
	pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_POS);//Setting the POS bit ensures that NACK will be sent after the current byte.(2 bytes->first is acked, last is nacked)
	I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);//TRM says set POS and ACK before clearing ADDR
	I2C_ClearAddrFlag(pI2CHandle->pI2C);
	I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Acking should be disabled since it is wanted to do so for stopping the communication.
	}
	else{
	I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);//Enable ACK'ing if not enabled for good measure.
	I2C_ClearAddrFlag(pI2CHandle->pI2C);
	}
	while(Len>0)
	{
		if(Len == 2){//If we are to receive 2 bytes,
			while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_BTF)));//Wait until BTF is set. This means DR=Not Empty Shift Reg=Not Empty. So we received the last two bytes.
			I2C_GenerateStopCondition(pI2CHandle->pI2C);//Generate stop, we received all bytes.
			*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;//Read the byte in data register.
			pRxBuffer++;//Update buffer and length.
			Len--;
			*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;//Data in shift register will be moved to DR as soon as we read DR, so read the last remaining byte.
			pRxBuffer++;//Update buffer and length.
			Len--;
		}
		else if(Len == 3){//Method 3 on the TRM makes the 3 last bytes special case. Since we will use BTF, we cant NACK at Len==2 is set because there will be no bytes left.
			while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_BTF)));//Wait for the second last byte to arrive using btf.
			I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Disable acking so last byte will be received with NACK.
			*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;//Read the byte in data register.
			pRxBuffer++;//Update buffer and length.
			Len--;
			while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_BTF)));//Wait for the last byte to arrive using btf. After the read, we have second last byte carried to DR, last byte is now on shift reg.
			I2C_GenerateStopCondition(pI2CHandle->pI2C);
			*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;//Read the byte in data register(second last).
			pRxBuffer++;//Update buffer and length.
			Len--;
			*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;//Read the byte in data register(last).
			pRxBuffer++;//Update buffer and length.
			Len--;
		}
		else{//Normal reading if we didnt reach the last 2 bytes. 1 Byte case is already covered outside of the while loop.
		while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_RXNE)));//Wait until RXNE is set. Then read the buffer.
		*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
		pRxBuffer++;//Update buffer and length.
		Len--;
		}
	}
	
	pI2CHandle->TxRxState = I2C_STATE_READY;
	return ENABLE;//Return positive if no error.
}

void I2C_ClearStopFlag(I2C_RegDef_ts* pI2C)
{
	uint32_t dummy;
	dummy = pI2C->I2C_SR1;//Stop is cleared by reading SR1 then writing to CR1.
	pI2C->I2C_CR1 |= 0;
	(void)dummy;
}

uint8_t I2C_SlaveTransmitBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len)
{
	if((pI2CHandle == NULL)||(pTxBuffer == NULL)|| (Len==0)||(pI2CHandle->TxRxState != I2C_STATE_READY)){
		return DISABLE;}//Check for basic parameters, return error if they are invalid.
	if(!(pI2CHandle->pI2C->I2C_CR1 & (1<<I2C_CR1_PE))){
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);//Enable Peripheral if not enabled	
		}
	pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_POS);
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_TX;
	pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_ACK);//Enable ACK for the address acknowledge.
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_ADDR)));//Wait until ADDR is set = Address matched
	I2C_ClearAddrFlag(pI2CHandle->pI2C);
	while(Len>0){
		while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_TXE)));//Wait until TXE is set.
		pI2CHandle->pI2C->I2C_DR = *pTxBuffer;//Write data into DR and update length and buffer.
		pTxBuffer++;
		Len--;
		if(((pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_BTF)))&&(Len!=0)){//If BTF is set when Len is not equal to zero, underrun has occured. Put one more byte to DR.
			pI2CHandle->pI2C->I2C_DR = *pTxBuffer;
			pTxBuffer++;
			Len--;	
		}			
	}
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_AF)));//After Len becomes 0, wait until AF is set. This means slave doesnt need to transmit anymore.
	pI2CHandle->pI2C->I2C_SR1 &= ~(1<<I2C_SR1_AF);//Clear the AF bit.
	pI2CHandle->pI2C->I2C_CR1 &=~(1<<I2C_CR1_ACK);//Disable Address Acknowledge.
	pI2CHandle->TxRxState = I2C_STATE_READY;
return ENABLE;
}

uint8_t I2C_SlaveReceiveBlocking(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len)
{
	if((pI2CHandle == NULL)||(pRxBuffer == NULL)|| (Len==0)||(pI2CHandle->TxRxState != I2C_STATE_READY)){
		return DISABLE;}//Check for basic parameters, return error if they are invalid.
	if(!(pI2CHandle->pI2C->I2C_CR1 & (1<<I2C_CR1_PE))){
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);//Enable Peripheral if not enabled	
		}
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_RX;
	pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_POS);//Clear POS
	pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_ACK);//Enable ACK for the address acknowledge and data bytes acknowledge.
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_ADDR)));//Wait until ADDR is set = Address matched
	I2C_ClearAddrFlag(pI2CHandle->pI2C);
	while(Len>0){
		if(I2C_WaitRxneTimeout(pI2CHandle)==DISABLE){
			I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Disable before exit so it doesnt ACK address without us wanting.
			return DISABLE;
		}//Wait until RXNE is set. If timeout occurs or an early stop is sent, exit.
		*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
		pRxBuffer++;
		Len--;
		if((pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_BTF))&&(Len !=0)){//Do an extra read if BTF is set AND data reception is not finished.
			*pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
			pRxBuffer++;
			Len--;
		}
	}
	if(I2C_WaitStopFlagTimeout(pI2CHandle)==DISABLE){//If timeout occurs when waiting.
		I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Disable before exit so it doesnt ACK address without us wanting.
		return DISABLE;//Exit with error.
	}//If stop comes in expected time,
	I2C_ClearStopFlag(pI2CHandle->pI2C);//Clear the stop flag
	I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Disable before exit so it doesnt ACK address without us wanting.
	pI2CHandle->TxRxState = I2C_STATE_READY;//Return state to ready before exiting.
	return ENABLE;
}

uint8_t I2C_WaitRxneTimeout(I2C_Handle_ts* pI2CHandle)//To avoid further developing a systick driver and functions, a simple delay is used.
{	
	uint16_t timeout = 50000;
	while(!(pI2CHandle->pI2C->I2C_SR1 & (1<<I2C_SR1_RXNE)))//While RXNE is not set,
	{	
		
		timeout--;
		if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_STOPF)){//If stop is detected.
			I2C_ClearStopFlag(pI2CHandle->pI2C);
			pI2CHandle->TxRxState = I2C_STATE_READY;
			return DISABLE;
			}
		if(timeout ==0){//Timeout has occured
			if(!(pI2CHandle->pI2C->I2C_SR1 & (1<<I2C_SR1_RXNE)))//If RXNE  is still not set, exit.
			{
				pI2CHandle->TxRxState = I2C_STATE_READY;
				return DISABLE;
			}
		}	
	}
	return ENABLE;
}

uint8_t I2C_WaitStopFlagTimeout(I2C_Handle_ts* pI2CHandle)
{
	uint16_t timeout = 50000;
	while(!(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_STOPF)))//While STOP Flag is not set
	{
		
		timeout--;
		if(timeout == 0){
			pI2CHandle->TxRxState = I2C_STATE_READY;//If timeout occurs, return state to ready.
			return DISABLE;
		}
		
	}
	return ENABLE;
}
/*I2C Interrupts with their flags:
-ITEVFEN Dependent Interrupts:
	*Start bit sent(Master) SB
	*Address sent(Master) or Address matched(Slave) ADDR
	*Stop received(Slave) STOPF
	*Data byte transfer finished BTF
-ITEVFEN AND ITBUFEN Dependent Interrupts:
	*Receive Buffer not Empty RXNE
	*Transmit Buffer Emty TXE
-ITERREN Dependent Interrupts:
	*Bus Error BERR
	*Arbitration Loss(Master)	ARLO
	*Acknowledge Failure AF
	*Overrun/Underrun	OVR
	*PEC Error PECERR
	*Timeout/Tlow Error TIMEOUT
	*SMBus alert SMBALERT

All error interrupts are OR'ed on the same line:it_error
All other interrupts are OR'ed on the same line:it_event
*/

void I2C_IRQEventHandler(I2C_Handle_ts* pI2CHandle)
{
	//Must determine what caused the interrupt first.
	//1.Check STOP flag
	if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_STOPF)){//StopF interrupt is received in Slave mode.
		I2C_IRQHandleStop(pI2CHandle);
	}
	//2.Check START flag
	else if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_SB)){
		I2C_IRQHandleStart(pI2CHandle);
	}
	//3.Check ADDR flag
	else if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_ADDR)){
		I2C_IRQHandleAddr(pI2CHandle);
	}
	//4.Check BTF Flag
	else if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_BTF)){
		I2C_IRQHandleBtf(pI2CHandle);
	}
	//5.Check TXE Flag
	else if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_TXE)){
		I2C_IRQHandleTxE(pI2CHandle);
	}
	//6.Check RXNE Flag
	else if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_RXNE)){
		I2C_IRQHandleRxNE(pI2CHandle);
	}
}


void I2C_IRQHandleStop(I2C_Handle_ts* pI2CHandle)//STOP interrupt only happens in Slave mode.
{
	if(pI2CHandle->TxRxState == I2C_STATE_BSY_IN_RX)
	{
		//Disable All Interrupts
		pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
		pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
		pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
		
		I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);
		I2C_ClearStopFlag(pI2CHandle->pI2C);
		
		if(pI2CHandle->RxLen!=0)//If we have an early stop, check for the last bytes via BTF and RXNE.
		{
			if(pI2CHandle->pI2C->I2C_SR1 & (1<<I2C_SR1_BTF))//If BTF is set, read one byte
			{
				*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
				pI2CHandle->RxLen--;
				pI2CHandle->pRxBuffer++;
			}
			
			if(pI2CHandle->pI2C->I2C_SR1 & (1<<I2C_SR1_BTF))//If RXNE is set, read one byte
			{
				*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
				pI2CHandle->RxLen--;
				pI2CHandle->pRxBuffer++;
			}
		}
		
		if(pI2CHandle->RxLen == 0)//We check again if RxLen reached 0, meaning data transfer is completed succesfully.
		{
			pI2CHandle->pRxBuffer = NULL;
			pI2CHandle->TxRxState = I2C_STATE_READY;
			I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_RXCMPLT);
		}
		else
		{
			pI2CHandle->ErrorEvent = I2C_ERR_EARLYSTOP;
			I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_EARLYSTOP);
		}
	}
	else if(pI2CHandle->TxRxState == I2C_STATE_BSY_IN_TX)
	{
		//Disable All Interrupts
		pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
		pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
		pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
		
		I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);
		
		pI2CHandle->ErrorEvent = I2C_ERR_EARLYSTOP;
		I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_EARLYSTOP);
	}
	else
	{
		
	}

}

void I2C_IRQHandleStart(I2C_Handle_ts* pI2CHandle)//SB is only set in Master mode
{
	if(pI2CHandle->ComDirection == I2C_COMDIR_TRNSMTR){//You can change the check to look up register bit M/S later.
		I2C_ExecuteAddressPhase(pI2CHandle->pI2C,pI2CHandle->SlaveAddr,I2C_R_NW_WRITE);
	}
	else if(pI2CHandle->ComDirection == I2C_COMDIR_RCVR){
		I2C_ExecuteAddressPhase(pI2CHandle->pI2C,pI2CHandle->SlaveAddr,I2C_R_NW_READ);
	}
}


void I2C_IRQHandleAddr(I2C_Handle_ts* pI2CHandle)
{
	if(pI2CHandle->DevMode == I2C_DEVMODE_MSTR)
	{
		if(pI2CHandle->ComDirection == I2C_COMDIR_TRNSMTR)
		{
			I2C_ClearAddrFlag(pI2CHandle->pI2C);
		}
		else if(pI2CHandle->ComDirection == I2C_COMDIR_RCVR)//Real Fun Begins
		{
			if(pI2CHandle->RxLen == 0)
			{
				I2C_ClearAddrFlag(pI2CHandle->pI2C);
				I2C_GenerateStopCondition(pI2CHandle->pI2C);
			}
			else if(pI2CHandle->RxLen == 1)
			{
				I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);
				I2C_ClearAddrFlag(pI2CHandle->pI2C);
				I2C_GenerateStopCondition(pI2CHandle->pI2C);
			}
			else if(pI2CHandle->RxLen == 2)
			{
				pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_POS);//Set the POS
				I2C_ClearAddrFlag(pI2CHandle->pI2C);
				I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);
				//For STM32F100 master receiver 2-byte reception: POS bit must be set before clearing ADDR flag. ACK must be cleared immediately after ADDR flag clearing.
			}
			else
			{
				I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);
				I2C_ClearAddrFlag(pI2CHandle->pI2C);
			}
		}
	}
	else if(pI2CHandle->DevMode == I2C_DEVMODE_SLV)
	{
		I2C_ClearAddrFlag(pI2CHandle->pI2C);
	}
	else
	{
	}
	//DONE = Add an extra condition for master receiver 1 byte and 2 byte conditions.
}

void I2C_IRQHandleTxE(I2C_Handle_ts* pI2CHandle)
{
	if(pI2CHandle->DevMode == I2C_DEVMODE_MSTR)
	{
		if(pI2CHandle->TxLen >0)//If Len==0, we will wait for BTF to be set. We will handle stopping there.
		{
			pI2CHandle->pI2C->I2C_DR = *pI2CHandle->pTxBuffer;
			pI2CHandle->TxLen--;
			pI2CHandle->pTxBuffer++;
		}
		else if(pI2CHandle->TxLen == 0)
		{
			//Disable All Interrupts
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
			
			I2C_GenerateStopCondition(pI2CHandle->pI2C);
			
			pI2CHandle->TxRxState = I2C_STATE_READY;		

			I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_TXCMPLT);
			
		}
	}
	else if(pI2CHandle->DevMode == I2C_DEVMODE_SLV)
	{
		if(pI2CHandle->TxLen >0)
		{
			pI2CHandle->pI2C->I2C_DR = *pI2CHandle->pTxBuffer;
			pI2CHandle->TxLen--;
			pI2CHandle->pTxBuffer++;
		}
	}
	else
	{
	}
}

void I2C_IRQHandleRxNE(I2C_Handle_ts* pI2CHandle)
{
	if(pI2CHandle->DevMode == I2C_DEVMODE_MSTR)
	{
		if(pI2CHandle->RxLen >3)//General Case When Len>3
		{
			*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->RxLen--;
			if(pI2CHandle->RxLen == 3)//If Len becomes 3, we want it handled by BTF interrupt algorithm.
			{
				pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);//Disable RXNE and TXE interrupts so BTF will handle last 3 bytes.
			}
		}
		else if((pI2CHandle->RxLen ==1)||(pI2CHandle->RxLen==0))
		{
			I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);//Disable Acking to prevent non wanted extra address acks.
			
			//Disable All Interrupts
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
			
			//Read the byte and close the communication.
			*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
			pI2CHandle->RxLen--;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->TxRxState = I2C_STATE_READY;
			I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_RXCMPLT);
		}
		else//If length is 2 or 3, we want it handled by BTF procedure.
		{
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);//Disable RXNE and TXE interrupts so BTF will handle last 2 bytes. 2 Byte receive special case
		}
		
	}
	else if(pI2CHandle->DevMode == I2C_DEVMODE_SLV)
	{
		if(pI2CHandle->RxLen > 0)
		{
			*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
			pI2CHandle->RxLen--;
			pI2CHandle->pRxBuffer++;
		}
	}
	else
	{
	}
}

void I2C_IRQHandleBtf(I2C_Handle_ts* pI2CHandle)//Transmissionda son byte gönderimi için kontrol edilip com kapatilabilir.
{
	//uint32_t dummyread;//Probably not needed since we already read SR1 to get here. Use it if needed later.
	if(pI2CHandle->DevMode == I2C_DEVMODE_MSTR)
	{
		if(pI2CHandle->TxRxState == I2C_STATE_BSY_IN_TX)
		{
			if(pI2CHandle->TxLen>0)
			{
				pI2CHandle->pI2C->I2C_DR = *pI2CHandle->pTxBuffer;
				pI2CHandle->pTxBuffer++;
				pI2CHandle->TxLen--;
			}
			else if(pI2CHandle->TxLen ==0)
			{
				//Disable all interrupts.
				pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
				pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
				pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
				//Generate Stop
				I2C_GenerateStopCondition(pI2CHandle->pI2C);//Maybe we should add a closecomm function here to clear significant the variables of handle.
				pI2CHandle->TxRxState = I2C_STATE_READY;
				I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_TXCMPLT);
				
			}
		}
		else if(pI2CHandle->TxRxState == I2C_STATE_BSY_IN_RX)
		{
			if(pI2CHandle->RxLen>3)
			{
				*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
				pI2CHandle->pRxBuffer++;
				pI2CHandle->RxLen--;
				if(pI2CHandle->RxLen==3)
				{
					pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);//Disable RXNE in case of 3 bytes left. We want last 3 bytes handled by BTF.
				}
			}
			else
			{
				if(pI2CHandle->RxLen == 3)
				{
					pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);//Disable RXNE if it is not already disabled. Just a guard.
					
					I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);
					*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
					pI2CHandle->pRxBuffer++;
					pI2CHandle->RxLen--;
				}
				else if(pI2CHandle->RxLen==2)
				{
					
					I2C_GenerateStopCondition(pI2CHandle->pI2C);
					
					*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
					pI2CHandle->pRxBuffer++;
					pI2CHandle->RxLen--;
					*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
					pI2CHandle->pRxBuffer++;
					pI2CHandle->RxLen--;
					
					//Disable all interrupts.
					pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
					pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
					pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
					
					pI2CHandle->TxRxState = I2C_STATE_READY;
					I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_RXCMPLT);
				}
			}
		}
		else
		{
		}
	}
	else if(pI2CHandle->DevMode == I2C_DEVMODE_SLV)
	{
		if(pI2CHandle->TxRxState == I2C_STATE_BSY_IN_TX)
		{
			if(pI2CHandle->TxLen>0)
			{
				pI2CHandle->pI2C->I2C_DR = *pI2CHandle->pTxBuffer;
				pI2CHandle->pTxBuffer++;
				pI2CHandle->TxLen--;
			}
		}
		else if(pI2CHandle->TxRxState == I2C_STATE_BSY_IN_RX)
		{
			if(pI2CHandle->RxLen>0)
			{
				*pI2CHandle->pRxBuffer = (uint8_t)pI2CHandle->pI2C->I2C_DR;
				pI2CHandle->pRxBuffer++;
				pI2CHandle->RxLen--;
			}
		}
	}
	else
	{
		
	}
}

void I2C_IRQErrorHandler(I2C_Handle_ts* pI2CHandle)
{
	if(pI2CHandle->pI2C->I2C_SR1&(1<<I2C_SR1_AF))//If ACK fail occurs
	{
		if(pI2CHandle->DevMode == I2C_DEVMODE_MSTR)
		{
			//Disable all interrupts.
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
			pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
			
			pI2CHandle->ErrorEvent = I2C_ERR_AF;
			
			I2C_GenerateStopCondition(pI2CHandle->pI2C);
			
			I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_ACKFAIL);
		}
		else if(pI2CHandle->DevMode == I2C_DEVMODE_SLV)
		{
			if(pI2CHandle->ComDirection == I2C_COMDIR_TRNSMTR)
			{
				//Disable all interrupts.
				pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITBUFEN);
				pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITERREN);
				pI2CHandle->pI2C->I2C_CR2 &= ~(1<<I2C_CR2_ITEVTEN);
				
				pI2CHandle->pI2C->I2C_SR1 &= (~(1<<I2C_SR1_AF));//Clear the AF flag
				
				I2C_ManageAcking(pI2CHandle->pI2C,DISABLE);
				
				I2C_Flush_DR(pI2CHandle->pI2C);//Clear TXE Flag
				
				if(pI2CHandle->TxLen == 0)
				{
					pI2CHandle->ComDirection = I2C_COMDIR_IDLE;
					pI2CHandle->TxRxState = I2C_STATE_READY;
					I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_TXCMPLT);					
				}
				else
				{
					pI2CHandle->ErrorEvent = I2C_ERR_AF;
					pI2CHandle->ComDirection = I2C_COMDIR_IDLE;
					I2C_ApplicationCallback(pI2CHandle,I2C_CLLBK_ACKFAIL);
					
				}
			}
			else
			{
				pI2CHandle->pI2C->I2C_SR1 &= (~(1<<I2C_SR1_AF));//Clear the AF flag
			}
		}
	}
	
}

void I2C_Flush_DR(I2C_RegDef_ts* pI2C)
{
	if(pI2C->I2C_SR1&(1<<I2C_SR1_TXE))//If TXE flag is up,
	{
		//Write dummy data to DR to clear TXE
		pI2C->I2C_DR = 0x00;
	}
}

uint8_t I2C_MasterTransmitIT(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len, uint8_t SlaveAddr,uint8_t RepeatedStart)
{
	if((pTxBuffer==NULL)||(Len==0)||(pI2CHandle == NULL)||(SlaveAddr == NULL)){
		return DISABLE;}//Return error
	if (pI2CHandle->TxRxState != I2C_STATE_READY){
		return DISABLE;
	}
	if((pI2CHandle->pI2C->I2C_CR1&(1<<I2C_CR1_PE))!=(1<<I2C_CR1_PE))//Enable the peripheral if it is not enabled.
	{
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);
	}
	
	pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_POS);//Disable POS.
	
	pI2CHandle->pTxBuffer = pTxBuffer;
	pI2CHandle->SlaveAddr = SlaveAddr;
	pI2CHandle->TxLen = Len;
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_TX;
	pI2CHandle->ComDirection = I2C_COMDIR_TRNSMTR;
	pI2CHandle->DevMode = I2C_DEVMODE_MSTR;
	
	//Enable all interrupts.
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITBUFEN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITERREN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITEVTEN);
		
	I2C_GenerateStartCondition(pI2CHandle->pI2C);
	
	return ENABLE;
}

uint8_t I2C_MasterReceiveIT(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len, uint8_t SlaveAddr,uint8_t RepeatedStart)
{
	if((pRxBuffer==NULL)||(Len==0)||(pI2CHandle == NULL)||(SlaveAddr == NULL)){
		return DISABLE;}//Return error
	if (pI2CHandle->TxRxState != I2C_STATE_READY){
		return DISABLE;
	}
	if(!(pI2CHandle->pI2C->I2C_CR1 & (1<<I2C_CR1_PE)))//Enable the peripheral if it is not enabled.
	{
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);
	}
	
	pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_POS);//Disable POS.
	
	pI2CHandle->pRxBuffer = pRxBuffer;
	pI2CHandle->SlaveAddr = SlaveAddr;
	pI2CHandle->RxLen = Len;
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_RX;
	pI2CHandle->ComDirection = I2C_COMDIR_RCVR;
	pI2CHandle->DevMode = I2C_DEVMODE_MSTR;
	
	//Enable all interrupts.
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITBUFEN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITERREN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITEVTEN);
		
	I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);
	I2C_GenerateStartCondition(pI2CHandle->pI2C);
	
	return ENABLE;	
}

uint8_t I2C_SlaveTransmitIT(I2C_Handle_ts* pI2CHandle,uint8_t* pTxBuffer,uint32_t Len)
{
	if((pTxBuffer==NULL)||(Len==0)||(pI2CHandle == NULL)){
		return DISABLE;}//Return error
	if (pI2CHandle->TxRxState != I2C_STATE_READY){
		return DISABLE;
	}
	if(!(pI2CHandle->pI2C->I2C_CR1 & (1<<I2C_CR1_PE)))//Enable the peripheral if it is not enabled.
	{
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);
	}
	
	pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_POS);//Disable POS.	
	
	pI2CHandle->pTxBuffer = pTxBuffer;
	pI2CHandle->TxLen = Len;
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_TX;
	pI2CHandle->ComDirection = I2C_COMDIR_TRNSMTR;
	pI2CHandle->DevMode = I2C_DEVMODE_SLV;
	
	I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);
	
	//Enable all interrupts.
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITBUFEN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITERREN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITEVTEN);
		
	return ENABLE;	
}

uint8_t I2C_SlaveReceiveIT(I2C_Handle_ts* pI2CHandle,uint8_t* pRxBuffer,uint32_t Len)
{
	if((pRxBuffer==NULL)||(Len==0)||(pI2CHandle == NULL)){
		return DISABLE;}//Return error
	if (pI2CHandle->TxRxState != I2C_STATE_READY){
		return DISABLE;
	}
	if(!(pI2CHandle->pI2C->I2C_CR1 & (1<<I2C_CR1_PE)))//Enable the peripheral if it is not enabled.
	{
		pI2CHandle->pI2C->I2C_CR1 |= (1<<I2C_CR1_PE);
	}
	
	pI2CHandle->pI2C->I2C_CR1 &= ~(1<<I2C_CR1_POS);//Disable POS.	
	
	pI2CHandle->pRxBuffer = pRxBuffer;
	pI2CHandle->RxLen = Len;
	pI2CHandle->TxRxState = I2C_STATE_BSY_IN_RX;
	pI2CHandle->ComDirection = I2C_COMDIR_RCVR;
	pI2CHandle->DevMode = I2C_DEVMODE_SLV;
	
	I2C_ManageAcking(pI2CHandle->pI2C,ENABLE);
	
	//Enable all interrupts.
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITBUFEN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITERREN);
	pI2CHandle->pI2C->I2C_CR2 |= (1<<I2C_CR2_ITEVTEN);
		
	return ENABLE;		
}

__attribute__ ((weak))void I2C_ApplicationCallback(I2C_Handle_ts* pI2CHandle,uint8_t CallbackEvent)
{
	//while(1);
	return;
}