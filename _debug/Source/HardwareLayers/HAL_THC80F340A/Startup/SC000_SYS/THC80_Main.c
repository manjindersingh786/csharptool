
#include	"TSIM.h"

BYTE	ISO7816_Time;
// const WORD testbuf[0x126a];

//====================================================
//	Main process
void	Testmain(void)
{	
	HWInit();								//	H/W initial
	
	hwAbs_osMain();

}
