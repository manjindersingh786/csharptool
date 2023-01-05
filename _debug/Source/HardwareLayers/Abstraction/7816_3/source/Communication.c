#include "7816_3.h"

/*Definition of variables*/
uMonoByte	gumbCmdApdu[MAX_CMD_LEN];
uDiByte		gudbOutputLen;
/*
* Recieve command header from ID (Terminal)
* 
* @pre OS supposed to receive header only, otherwise PCSC error will come
*/
void _7816_3_receiveCommandHeader(void)
{
	INS_BYTE = receiveByte();
	P1_BYTE = receiveByte();
	P2_BYTE = receiveByte();
	P3_BYTE = receiveByte();
}


/*
* Sends the response data back to ID (interface device)
*/
void _7816_3_sendResponseData(void)
{	

#ifndef WIN32
	/*Send ACK byte first*/
	SendByteToID(INS_BYTE);
#endif

	/*Below variable use for index*/
	uDiByte udbIndex;
	uDiByte udbLength;

	if (0x00 == P3_BYTE) { // check if p3 is 00
		
		/*Max requested length will 256 bytes*/
		udbLength = ((gudbOutputLen > 0x100) ? 0x100 : gudbOutputLen);
	}
	else { // if p3 is non-zero

		/*keep same data*/
		udbLength = P3_BYTE;
	}

	/*Send all prepared response data back to ID, 1-by-1*/
	for (udbIndex = 0; udbIndex < udbLength; udbIndex++)
	{
		/*Send current element of Response buffer*/
		SendByteToID( RESPONSE_BUFF[udbIndex]);
	}

	/*Reset the response length*/
	gudbOutputLen -= udbLength;
}

/*
* Receive command data
*
* @pre command supposed to be of Input type i.e. it must have command data
*/
void _7816_3_receiveCommandData(void)
{
#ifndef WIN32
	SendByteToID(INS_BYTE);
#endif
	uMonoByte umbIndex;
	for(umbIndex = 0; umbIndex< LC_BYTE; umbIndex++)
	{ 
		gumbCmdApdu[INDEX_CMD_DATA + umbIndex] = receiveByte();
	}
}
/*
* It sets status words acc. to requested procedure bytes
* 
* @precondition: Requestor ensure that Procedure byte can never be PROCDEDURE_NULL_BYTE
*/
void _7816_3_setProcedureByte(void) {

	/*Set status words acc. to procedure byte*/
	excepRet = (tSwExceps)PROCEDURE_SEND_GET_RESPONSE;
	
	/*second byte of status words are*/
	P3_BYTE = (uMonoByte)gudbOutputLen;
}

void _7816_3_sendStatusWords(void) {

	uDiByte udbSw;


	/*Get status words*/
	udbSw = SIM ? SW[excepRet - 1].udbSimSw : \
			            SW[excepRet - 1].udbUiccSw;
	
	if ((EXCEP_WRONG_P3 == excepRet)
		||
		(EXCEP_SEND_GET_RESPONSE == excepRet)
		||
		(EXCEP_RESEND_COMMAND_WITH_CORRECT_P3 == excepRet)
		||
		(EXCEP_NORMAL_END_PENDING_PROACTIVE_CMD == excepRet)
		||
		(EXCEP_PIN_VERIFICATION_FAILED == excepRet)) { // if its 67xx or 61xx or 6Cxx or 63Cx

		/*P3 will help to provide SW2 in this case*/
		udbSw |= ((uMonoByte)P3_BYTE);
		
	}


	/*Send SW1*/
	SendByteToID((uMonoByte)SHIFT_RIGHT_BY_N_BITS(udbSw,0x08));

	/*Send SW2 and receive next command*/
	CLA_BYTE = SendLastByteToID((uMonoByte)(udbSw));
}