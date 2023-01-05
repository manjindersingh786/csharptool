#include "102_221.h"


/*
* It implements the GET RESPONSE command as per 102 221
*/
void _102221_getResponse(void) {

	uDiByte udbLuicc;

	DBG_FUNC_ENTRY(">>[102_221] _102221_getResponse() \n");

	// check P1 and P2.
	if ((0x00 != P1_BYTE)
		||
		(0x00 != P2_BYTE))
	{
		DBG_WARN("[102_221] _102221_getResponse: P1 and P2 should be 00\n");

		/*Reset the remaining response*/
		gudbOutputLen = 0x00;

		THROW(EXCEP_WRONG_P1_P2);
	}

	else if (0x00 == gudbOutputLen) {

		/*Data cant be sent as no response data is available */
		DBG_WARN("[102_221] _102221_getResponse: Luicc is 00\n");
		THROW(EXCEP_TECHNICAL_PROBLEM);
	}

	else if (P3_BYTE > gudbOutputLen) { // incoming Le is greater than Luicc i.e. gudbOutputLen

		DBG_WARN("[102_221] _102221_getResponse: Requested p3 is more than Luicc \n");

		/*Store the Luicc in udbLuicc*/
		udbLuicc = gudbOutputLen;

		/*Reset the remaining response*/
		gudbOutputLen = 0x00;

		THROW2(EXCEP_WRONG_P3, ((uMonoByte)udbLuicc));
	}
	/*this condition apply only for GSM/SIM*/
	/*check if incoming cmd is GSM/SIM cmd*/
	else if (CLA_BYTE == CLA_SIM)
	{
		/*check if P3 is non zero*/
		if (P3_BYTE != 0x00)
		{
			DBG_FUNC("[102_221] _102221_getResponse: Requested length is %1\n", P3_BYTE);
			
			/*reset the output length to the incoming Le*/
			gudbOutputLen = P3_BYTE;
		}
	}

	DBG_FUNC_EXIT("<<[102_221] _102221_getResponse() \n");
}