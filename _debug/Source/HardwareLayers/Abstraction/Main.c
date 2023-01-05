
#include "Main.h"
void hwAbs_osMain(void)
{
	/*Send ATR*/
	_7816_3_transmitAtr();

	/*PPS handshaking*/
	_7816_3_ppsExchange();

	/*First command after ATR*/
	bFirstCommandAfterATR = TRUE;
	
	/*Continues till the end of card session*/
	while (TRUE)
	{
		/*Route the command*/
		cmdRout_commandRouter(); //source is 00 for terminal
	}
}

