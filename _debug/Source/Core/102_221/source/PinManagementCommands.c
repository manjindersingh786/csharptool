#include"102_221.h"

#define PIN_RETRY_COUNTER									(0x00)

/*
* It throws the error exception as per give Pin status code
*
* @param pscVal: contains status code sent by Pin functions
*/
void _102221_throwStatusResponseAsPer(tPinStatusCode pscVal) {

	if (pscVal > PIN_SC_BLOCKED) { // atleast few retries left
		
		THROW2(EXCEP_PIN_VERIFICATION_FAILED, pscVal - PIN_SC_NOT_VERIFIED);
	}
	else if (PIN_SC_BLOCKED == pscVal) { //If CHV is blocked

		THROW(EXCEP_PIN_BLOCKED);
	}
	else if (PIN_SC_INVALID_STATE == pscVal) { //If PIN is disabled

		THROW(EXCEP_CONTRADICTS_PIN_STATUS);
	}
	else if (PIN_SC_NOT_FOUND == pscVal) { //if no such pin found.

		THROW(EXCEP_NO_PIN_INITIALIZED);
	}
}

/*
* It implements the Change PIN command as per ETSI 102 221
*/
void _102221_changePin(void) {
	
	tPinStatusCode pscReturned;
	DBG_FUNC_ENTRY(">>[102_221] _102221_changePin() \n");

	if ((P1_BYTE) || (_7816_4_isInvalidReferenceData(P2_BYTE))) { //Check if P1 is non-zero OR p2 conatins invalid key reference
		DBG_ERR("[102_221] _102221_changePin: Invalid P1 - P2 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_changePin() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE != LENGTH_VALID_FOR_CHANGE_UNBLOCK_PIN) {// Incorrect P3

		DBG_ERR("[102_221] _102221_changePin: Invalid P3 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_changePin() \n");
		THROW2(EXCEP_WRONG_P3, LENGTH_VALID_FOR_CHANGE_UNBLOCK_PIN);
	}
	pscReturned = _7816_4_changeUnblockPin(P2_BYTE, PIN_OP_CHANGE, COMMAND_DATA, COMMAND_DATA + LENGTH_PIN_VALUE);

	DBG_FUNC_EXIT("<<[102_221] _102221_changePin() \n");

	_102221_throwStatusResponseAsPer(pscReturned);
}

/*
* It implements the Unblock PIN command as per ETSI 102 221
*/
void _102221_unblockPin(void) {

	tPinStatusCode pscReturned;
	tPinOperation	poFor = PIN_OP_UNBLOCK;
	DBG_FUNC_ENTRY(">>[102_221] _102221_unblockPin() \n");

	if ((P1_BYTE) || (_7816_4_isInvalidReferenceData(P2_BYTE))) { //Check if P1 is non-zero OR p2 conatins invalid key reference
		DBG_ERR("[102_221] _102221_unblockPin: Invalid P1 - P2 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_unblockPin() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (PIN_RETRY_COUNTER == P3_BYTE) { //If empty command data

		/*Request to retreive Unblock Pin Retry counter*/
		poFor = PIN_OP_UNBLOCK_COUNTER;
	}
	else if (P3_BYTE != LENGTH_VALID_FOR_CHANGE_UNBLOCK_PIN) {// Incorrect P3

		DBG_ERR("[102_221] _102221_unblockPin: Invalid P3 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_unblockPin() \n");
		THROW2(EXCEP_WRONG_P3, LENGTH_VALID_FOR_CHANGE_UNBLOCK_PIN);
	}

	pscReturned = _7816_4_changeUnblockPin(P2_BYTE, poFor, COMMAND_DATA, COMMAND_DATA + LENGTH_PIN_VALUE);

	DBG_FUNC_EXIT("<<[102_221] _102221_unblockPin() \n");

	_102221_throwStatusResponseAsPer(pscReturned);
}

/*
* It implements the Verify PIN command as per ETSI 102 221
*/
void _102221_verifyPin(void) {

	tPinStatusCode	pscReturned; // Stores the Pins status code
	tPinOperation	poFor = PIN_OP_VERIFY;

	DBG_FUNC_ENTRY(">>[102_221] _102221_verifyPin() \n");

	if ((P1_BYTE) || (_7816_4_isInvalidReferenceData(P2_BYTE))) { //Check if P1 is non-zero OR p2 conatins invalid key reference
		DBG_ERR("[102_221] _102221_verifyPin: Invalid P1 - P2 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_verifyPin() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (PIN_RETRY_COUNTER == P3_BYTE) { // If P3 indicates to retreive Retry counter
		
		DBG_FUNC("[102_221] _102221_verifyPin: Request for Verify PIN retry counter\n");

		/*Change the pin operation*/
		poFor = PIN_OP_VERIFY_COUNTER;

	}
	else if(P3_BYTE != LENGTH_PIN_VALUE) {// Incorrect P3

		DBG_ERR("[102_221] _102221_verifyPin: Invalid P3 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_verifyPin() \n");
		THROW2(EXCEP_WRONG_P3, LENGTH_PIN_VALUE);

	}

	/*Verify PIN*/
	pscReturned = _7816_4_verifyPin(P2_BYTE, PIN_ENABLED,  COMMAND_DATA, poFor);

	DBG_FUNC_EXIT("<<[102_221] _102221_verifyPin() \n");

	/*Handle negative values of pscReturned*/
	_102221_throwStatusResponseAsPer(pscReturned);
}

/*
* It implements the Enable PIN command as per ETSI 102 221
*/
void _102221_enablePin(void) {

	tPinStatusCode pscReturned; // Stores the Pins status code

	DBG_FUNC_ENTRY(">>[102_221] _102221_enablePin() \n");

	if ((P1_BYTE) || (_7816_4_isInvalidReferenceData(P2_BYTE))) { //Check if P1 is non-zero OR p2 conatins invalid key reference
		DBG_ERR("[102_221] _102221_enablePin: Invalid P1 - P2 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_enablePin() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE != LENGTH_PIN_VALUE) {// Incorrect P3

		DBG_ERR("[102_221] _102221_enablePin: Invalid P3 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_enablePin() \n");
		THROW2(EXCEP_WRONG_P3, LENGTH_PIN_VALUE);
	}

	/*Change PIN*/
	pscReturned = _7816_4_changePinState(P2_BYTE, PIN_OP_ENABLE, COMMAND_DATA);

	DBG_FUNC_EXIT("<<[102_221] _102221_enablePin() \n");

	/*Handle negative values of pscReturned*/
	_102221_throwStatusResponseAsPer(pscReturned);
}

/*
* It implements the Disable PIN command as per ETSI 102 221
*/
void _102221_disablePin(void) {

	tPinStatusCode pscReturned; // Stores the Pins status code

	DBG_FUNC_ENTRY(">>[102_221] _102221_disablePin() \n");

	if ((P1_BYTE) || (_7816_4_isInvalidReferenceData(P2_BYTE))) { //Check if P1 is non-zero OR p2 conatins invalid key reference
		DBG_ERR("[102_221] _102221_disablePin: Invalid P1 - P2 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_disablePin() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE != LENGTH_PIN_VALUE) {// Incorrect P3

		DBG_ERR("[102_221] _102221_disablePin: Invalid P3 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_disablePin() \n");
		THROW2(EXCEP_WRONG_P3, LENGTH_PIN_VALUE);
	}

	/*Change PIN*/
	pscReturned = _7816_4_changePinState(P2_BYTE, PIN_OP_DISABLE, COMMAND_DATA);

	DBG_FUNC_EXIT("<<[102_221] _102221_disablePin() \n");

	/*Handle negative values of pscReturned*/
	_102221_throwStatusResponseAsPer(pscReturned);
}