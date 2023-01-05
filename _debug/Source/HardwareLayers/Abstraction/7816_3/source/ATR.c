
#include "7816_3.h"

/*Fi: Clock rate conversion integer list*/
#define FI_372						(0x10)
#define FI_512						(0x90)

/*Di: Baud rate adjustment integer*/
#define DI_1							(0x01)
#define DI_2							(0x02)
#define DI_32							(0x06)


/*ATR bytes*/
#define SIZE_ATR					(0x16)
const 	uMonoByte	umbATR[SIZE_ATR] ={ 0x3B, 0x9F, 0x96, 0x80, 0x1F, 0x87, 0x80, 0x31, 0xA0, 0x73, 0xBE, 0x21, 0x11, 0x63, 0x00, 0x00, 0x01, 0x83, 0x05, 0x90, 0x00, 0x89 };

/*PPS*/
#define PPS_MAX_LENGTH				(0x04)
#define PPS_PPS0_MASK_PPS1		(0x10)
#define PPS_PPS0_MASK_PPS2		(0x20)
#define PPS_PPS0_MASK_PPS3		(0x40)
#define PPS_PPSS_INDEX				(0x00)
#define PPS_PPS0_INDEX				(0x01)
#define PPS_PPS1_INDEX				(0x02)
#define PPS_PCK_INDEX					(0x03)

/*
* Transmits the ATR bytes
*/
void _7816_3_transmitAtr(void)
{
	DBG_FUNC_ENTRY(">>[7816_3] _7816_3_transmitAtr() \n");
	DBG_FUNC("Sending ATR bytes \n");
	uMonoByte umbIndex;
	
	setFiDi(FI_372|DI_1);
	
	for(umbIndex = 0;umbIndex<SIZE_ATR ;umbIndex ++)
	{	
		SendByteToID(umbATR[umbIndex]);
	}
	
	DBG_FUNC_EXIT("<<[7816_3] _7816_3_transmitAtr() \n");
}

/*
* Exchanges PPS bytes and validate the values
*
*	@pre: This function must be called immediate after _7816_3_transmitAtr
*/
#undef THC80_CODE
void _7816_3_ppsExchange(void)
{
	uMonoByte	umbValue;
	uMonoByte	umbPPS[PPS_MAX_LENGTH] = {0xFF, 0x10, 0x00, 0xEF}; // PCK contains XOR of PPS and PPS0
#ifndef WIN32
	uMonoByte	umbPck;
#endif
	
	DBG_FUNC_ENTRY(">>[7816_3] _7816_3_ppsExchange() \n");
	umbValue = receiveByte();
#ifndef WIN32	
	if (umbValue == 0xFF)
	{
		/*Receive PPS0*/
		umbValue = receiveByte();

		/*initial value to calculate XOR*/
		umbPck = umbValue;

		if (umbValue & PPS_PPS0_MASK_PPS1) { // If Bit 5 of PPS0 is set

			/*Receive and Store in given array*/
			umbPPS[PPS_PPS1_INDEX] = receiveByte();

			/*XOR with PPS1*/
			umbPck ^= umbPPS[PPS_PPS1_INDEX];
		}
		if (umbValue & PPS_PPS0_MASK_PPS2) {  // If Bit 6 of PPS0 is set

			/*XOR with PPS2*/
			umbPck ^= receiveByte();
		}
		if (umbValue & PPS_PPS0_MASK_PPS3) { // If Bit 7 of PPS0 is set

			/*XOR with PPS3*/
			umbPck ^= receiveByte();
		}
		/*XOR with PCK */
		umbPck ^= receiveByte();
		
		if ((umbPck == 0xFF) && (umbValue & PPS_PPS0_MASK_PPS1) ){ //if XORing is valid and PPS1 is present
			
			/*XOR with incoming PPS1*/
			umbPPS[PPS_PCK_INDEX] ^= umbPPS[PPS_PPS1_INDEX];
			
			/*Check if PPS1 is valid*/
			sendPPSresponse(umbPPS, 0x04);
			umbValue = receiveByte();
		}
		
	}
#endif			
	CLA_BYTE = umbValue;
	
	DBG_FUNC_EXIT("<<[7816_3] _7816_3_ppsExchange() \n");
}