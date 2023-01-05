#include	"TSIM.h"

//====================================================
//	Timer 0 interrutp service routine
void	TMR_ISR(void)
{
	if(SCINTSTS1 & Bit0_En )				//	TIMER0
	{	
		SCINTEN1 &= Bit0_Dis;
		TCON &= Bit5_En;					
	}
	else if(SCINTSTS1 & Bit1_En)			//	TIMER1
	{
		SCINTEN1 &= Bit1_Dis;
		TCON &= Bit7_En;
	}
	return;
}
//====================================================
//	Flash write finish interrutp service routine
void	FLASH_ISR(void)	
{
	SCINTEN2 &= Bit0_Dis;
	FLSTS |= Bit0_En;
	return;
}
