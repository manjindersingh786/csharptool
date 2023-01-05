
#include	"TSIM.h"
//#define	Emulator	1		//	Just for debug in emulator

//====================================================
//	Initialize H/W
void	HWInit(void)
{	
	SCSYS = 0x80;			
	ISOMSK = 0xFF;
// 	ISOCON1	= 0x27;				//	EGT = 4
}

//====================================================
//	Chip enters idle mode, be awaken up by UART interrupt
void	sleep_mode(void)
{
	WORD	BackUpVDCON = VDCON;
	WORD	BackUpFDCON = FDCON;

	VDCON = 0x00;
	FDCON = 0x00;
	
	SCSLEEP = 0x01;						

	__nop();
	__nop();	
	__nop();
	__nop();			
	__nop();


#if	Emulator
	while(SCSLEEP == 0x01);					//	Just for emulator debugging
#endif
   VDCON = BackUpVDCON;
   FDCON = BackUpFDCON;
}
