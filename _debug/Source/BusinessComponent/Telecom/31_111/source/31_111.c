#include "31_111.h"

//Variables definition

uMonoByte	gumbProactiveSession;//it is a indication for proactive session is ongoing or not gumbProactiveSession == '00' means not any proactive sess is ongoing and '01' means proact sess is ongoing
uMonoByte	gumbProactive[MAX_RESPONSE_LEN];
uDiByte		gudbProactiveLen;

/*
* It checks for the profile(CAT facilities) that terminal supports
*
* @param byteIndex: byte that contain the facility supported by the terminal.
* @param bitNo: bit which indicate that facility supported or not.
*				bit = 1: facility supported by terminal. 
*				bit = 0: facility not supported by terminal. 
*
* @return TRUE if facility supported by terminal, otherwise FALSE.
* for example : if terminal supports SMS-PP data download 
*				then set BitNo 2 of ByteIndex 1.
*/
boolean _31111_checkTerminalProfile(IN_ARG uMonoByte umbByteIndex, IN_ARG uMonoByte umpBitNo)
{
	tFileHeaderAddr   fhaFile;
	uMonoByte         umbProfileByte;

	DBG_FUNC_ENTRY(">>[31111] _31111_checkTerminalProfile() \n");

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	/*get memory reference of Terminal profile ef*/
	_7816_4_ifFileExists(FID_PROP_TERMINAL_PROFILE, fhaFile, MODE_FIND_FID, &fhaFile);

	/*get the byte which will be checked further*/
	_7816_4_readUpdateFile(fhaFile, &umbProfileByte, umbByteIndex - 1, 0x01, ACC_MODE_FILE_READ);


	/*check if bit is set or not*/
	if (SHIFT_LEFT_BY_N_BITS(0x01, (umpBitNo - 1)) & umbProfileByte)
	{
		/*facility is supported by the terminal*/
		DBG_FUNC("[31111] _31111_checkTerminalProfile: TRUE \n");
		DBG_FUNC_EXIT("<<[31111] _31111_checkTerminalProfile() \n");
		return TRUE;
	}
	/*facility does not supported by the terminal*/
	DBG_FUNC("[31111] _31111_checkTerminalProfile: FALSE \n");
	DBG_FUNC_EXIT("<<[31111] _31111_checkTerminalProfile() \n");
	return FALSE;
}