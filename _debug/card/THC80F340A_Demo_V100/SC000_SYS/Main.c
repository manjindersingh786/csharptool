
#include	"TSIM.h"

BYTE	ISO7816_Time;
// const WORD testbuf[0x126a];

//====================================================
//	Main process
void	Testmain(void)
{	
	HWInit();								//	H/W initial
		
 	ISO7816_Time = 0;						//	0: 1st time,ATR
		
	while(1)
	{  
		if(ISO7816_Time > 0)
		{			
			sleep_mode();					//	Power down mode after thread has been executed			
		}
		ISO7816_main();
	}	
}
