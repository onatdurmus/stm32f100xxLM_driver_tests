#include "stm32f100xxLM_rcc_driver.h"

static uint16_t poweroftwos[] = {1, 2, 4, 8, 16, 32,64,128,256,512};// We will use this so we wont use unnecessary multiplications.

uint32_t RCC_GetPCLK_APB1(void){//Get APB1 Clock Value
	uint8_t sws;
	uint32_t sysclk;
	uint32_t pclk = DISABLE;
	sws = ((RCC->RCC_CFGR & 12)>>RCC_CFGR_SWS);//Get the 3:2 bits of CFGR. 12 is 1100 then we right shift.
	//On Board HSE is 8MHz. HSI RC is also 8MHz.
	if((sws==0)||(sws==1))//if SWS is HSI or HSE, it is 8MHz
	{
		sysclk = 8000000;
	}
	else if(sws==2){//PLL is selected
		uint8_t pllmul = (uint8_t)((RCC->RCC_CFGR & (0x0F<<RCC_CFGR_PLLMUL))>>RCC_CFGR_PLLMUL);//Mask the PLL bits(4). Then right shift them into LSB's.
		pllmul = pllmul+2;//PLL multiplier is +2 of the register bits. 0000: input x2, 0001:input x3, 0010:input x4 and so on.
		if(pllmul==17){
			pllmul = 16;//if register value is 15, multiplier is still 16
		}
		if(RCC->RCC_CFGR & (1<<RCC_CFGR_PLLSRC)){//if pllsrc bit is set, Clock from PREDIV1 is PLL input clk
			sysclk = 8000000; //HSE is 8MHz
			sysclk = sysclk/((RCC->RCC_CFGR2&0x0F)+1);//Prediv bits are the first 4 bits of CFGR2. Input of PREDIV1 is divided by +1 of prediv bits. 0000: no div, 0101: div by 6 and so on. This is now input to the PLL.
			sysclk = sysclk*pllmul;
		}
		else{//if it is not set, HSI/2 is selected as PLL input clk
			sysclk = 4000000;//Input into PLL will be 4MHz(HSI/2)
			sysclk = sysclk*pllmul;//PLL output is sysclk.
		}
	}
	else{
		return DISABLE;//if sws is different, or 3(reserved) it is an error.
	}
	//After sysclk is defined, it must go through AHB and APB1 prescalers.
	uint16_t ahbpre = (uint16_t)((RCC->RCC_CFGR &(0x0F<<RCC_CFGR_HPRE))>>RCC_CFGR_HPRE);//Get the HPRE(AHB Prescaler) bits(4 bits) of the CFGR register.
	if(ahbpre<8){
		ahbpre=1;//if 0xxx, Sysclk is not divided so 1.
	}
	else{
		ahbpre = ahbpre%8;//We take mod8 because the bits after that are important
		if(ahbpre<4)
		{
			ahbpre = poweroftwos[ahbpre+1];//8(0) corresponds to divby2 so we take the power of 2 of+1 
		}
		else{//For some reason, divby32 value is skipped which would be at 4, instead it is 64 at 4 so we take +2 from that point on
			ahbpre = poweroftwos[ahbpre+2];
		}
	}
	uint8_t apb1pre;
	apb1pre = (uint8_t)((RCC->RCC_CFGR & (7<<RCC_CFGR_PPRE1))>>RCC_CFGR_PPRE1);//We take the APB1 prescaler(PPRE1) bits (3 bits)
	if(apb1pre<4){
		apb1pre = 1; //if 0xx , it is not divided so 1
	}
	else{
		apb1pre = poweroftwos[apb1pre-3];//The same mechanism as ahb, without the exception. 4 corresponds to 2, 5 to 4, 6 to 8 etc.
	}
	pclk = sysclk/(ahbpre*apb1pre);//We finally calculate PCLK for I2C(APB1) by dividing system clock to ahb and apb1 prescalers.
	return pclk;
}

uint32_t RCC_GetPCLK_APB2(void){//Get APB2	 Clock Value
	uint8_t sws;
	uint32_t sysclk;
	uint32_t pclk = DISABLE;
	sws = ((RCC->RCC_CFGR & 12)>>RCC_CFGR_SWS);//Get the 3:2 bits of CFGR. 12 is 1100 then we right shift.
	//On Board HSE is 8MHz. HSI RC is also 8MHz.
	if((sws==0)||(sws==1))//if SWS is HSI or HSE, it is 8MHz
	{
		sysclk = 8000000;
	}
	else if(sws==2){//PLL is selected
		uint8_t pllmul = (uint8_t)((RCC->RCC_CFGR & (0x0F<<RCC_CFGR_PLLMUL))>>RCC_CFGR_PLLMUL);//Mask the PLL bits(4). Then right shift them into LSB's.
		pllmul = pllmul+2;//PLL multiplier is +2 of the register bits. 0000: input x2, 0001:input x3, 0010:input x4 and so on.
		if(pllmul==17){
			pllmul = 16;//if register value is 15, multiplier is still 16
		}
		if(RCC->RCC_CFGR & (1<<RCC_CFGR_PLLSRC)){//if pllsrc bit is set, Clock from PREDIV1 is PLL input clk
			sysclk = 8000000; //HSE is 8MHz
			sysclk = sysclk/((RCC->RCC_CFGR2&0x0F)+1);//Prediv bits are the first 4 bits of CFGR2. Input of PREDIV1 is divided by +1 of prediv bits. 0000: no div, 0101: div by 6 and so on. This is now input to the PLL.
			sysclk = sysclk*pllmul;
		}
		else{//if it is not set, HSI/2 is selected as PLL input clk
			sysclk = 4000000;//Input into PLL will be 4MHz(HSI/2)
			sysclk = sysclk*pllmul;//PLL output is sysclk.
		}
	}
	else{
		return DISABLE;//if sws is different, or 3(reserved) it is an error.
	}
	//After sysclk is defined, it must go through AHB and APB1 prescalers.
	uint16_t ahbpre = (uint16_t)((RCC->RCC_CFGR &(0x0F<<RCC_CFGR_HPRE))>>RCC_CFGR_HPRE);//Get the HPRE(AHB Prescaler) bits(4 bits) of the CFGR register.
	if(ahbpre<8){
		ahbpre=1;//if 0xxx, Sysclk is not divided so 1.
	}
	else{
		ahbpre = ahbpre%8;//We take mod8 because the bits after that are important
		if(ahbpre<4)
		{
			ahbpre = poweroftwos[ahbpre+1];//8(0) corresponds to divby2 so we take the power of 2 of+1 
		}
		else{//For some reason, divby32 value is skipped which would be at 4, instead it is 64 at 4 so we take +2 from that point on
			ahbpre = poweroftwos[ahbpre+2];
		}
	}
	uint8_t apb2pre;
	apb2pre = (uint8_t)((RCC->RCC_CFGR & (7<<RCC_CFGR_PPRE2))>>RCC_CFGR_PPRE2);//We take the APB1 prescaler(PPRE2) bits (3 bits)
	//Buranin alti apb1pre icin yaptigimiz kodlama. Sadece apb2 ile degistirecegiz.
	if(apb2pre<4){
		apb2pre = 1; //if 0xx , it is not divided so 1
	}
	else{
		apb2pre = poweroftwos[apb2pre-3];//The same mechanism as ahb, without the exception. 4 corresponds to 2, 5 to 4, 6 to 8 etc.
	}
	pclk = sysclk/(ahbpre*apb2pre);//We finally calculate PCLK for I2C(APB1) by dividing system clock to ahb and apb1 prescalers.
	return pclk;	
}