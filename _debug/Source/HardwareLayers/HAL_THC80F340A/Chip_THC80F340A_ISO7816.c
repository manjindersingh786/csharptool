

#include	"ChipLowLevel.h"
#define INDEX_DEFAULT_FIDI									(0x01)
const	BYTE	umbSupportedFiDi[] = {11, 0x11, 0x12, 0x13, 0x18, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97};

/*
* Send PPS response and Configure Fi-Di
*
* @param umbFiDi: Frequency & Baudrate
*/
void Chip_THC80F340A_sendPPSresponse(puMonoByte pumbFiDi, uMonoByte umbLen){
		/*stores index */
	uMonoByte	umbCounter = 0;
	
	while(umbCounter < umbSupportedFiDi[0]) // check if current index is less than length of supported list
	{
		/*Check if presented FiDi matches with any of supported list*/
		if(pumbFiDi[0x02] == umbSupportedFiDi[umbCounter + 1]) 
		{
			/*Send PPS response*/
			ISO7816_TxNByte(umbLen,pumbFiDi);
			
			/*Set given FiDi*/
			ISOBRC = pumbFiDi[0x02];
			return;
		}
		else // different FiDi
		{
			/*next FiDi*/
			umbCounter ++;
		}
	}	
}
/*
* Configure Fi-Di
*
* @param umbFiDi: Frequency & Baudrate
*/
void Chip_THC80F340A_setFiDi(uMonoByte umbFiDi){
	
	/*stores index */
	uMonoByte	umbCounter = 0;
	
	while(umbCounter < umbSupportedFiDi[0]) // check if current index is less than length of supported list
	{
		/*Check if presented FiDi matches with any of supported list*/
		if(umbFiDi == umbSupportedFiDi[umbCounter + 1]) 
		{
			/*Set given FiDi*/
			ISOBRC = umbFiDi;
			return;
		}
		else // different FiDi
		{
			/*next FiDi*/
			umbCounter ++;
		}
	}	
}
void Chip_THC80F340A_sendByte(uMonoByte umbValue){
	
	ISO7816_TxByte(umbValue);
	
}
uMonoByte Chip_THC80F340A_sendLastByte(uMonoByte umbValue){
	
	/*Send Last byte of transmission*/
	ISO7816_TxByte(umbValue);
	
	/*Put the SIM in sleep mode, will awake by H/W*/
	sleep_mode();
	
	/*receive first byte i.e. class byte of next command*/
	return ISO7816_RxByte();
}

uMonoByte Chip_THC80F340A_receiveByte(void){
	
	return ISO7816_RxByte();
}

/*
*	Enable Sending Null procedure byte
*/
void Chip_THC80F340A_enableNullProcedureByte(void){
	
	TX_NULL_BYTE_ON						//	Enable every 8000 etu to send one NULL byte 60H
}

/*
*	Disable Sending Null Procedure byte
*/
void Chip_THC80F340A_disableNullProcedureByte(void){
	
	TX_NULL_BYTE_OFF					//	Disable send NULL byte 60H
}