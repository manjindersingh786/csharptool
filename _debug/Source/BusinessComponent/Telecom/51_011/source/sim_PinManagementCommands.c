#include"51_011.h"

/*
* It validates P1-P2 of Verify and Change CHV command
* 
* @param umbChv1: Valid value of CHV1
* @param umbLc: Valid length of command data to be presented by terminal
* @return Valid Key reference mapped to incoming CHV, otherwise throws exception
*/
static uMonoByte _51011_validatePinCommand(uMonoByte umbChv1, uMonoByte umbLc) {
	
	uMonoByte umbKeyRef;

	DBG_FUNC_ENTRY(">>[51_011] _51011_validatePinCommand() \n");

	if (P1_BYTE) {  // If p1 is non-zero
		DBG_ERR("[51_011] _51011_validatePinCommand: Invalid P1 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_validatePinCommand() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (PIN_CHV_2 == P2_BYTE) { // if its CHV2

		/*Update mapped key reference of CHV2*/
		umbKeyRef = MAPPED_KEY_REF_TO_CHV_2;
	}
	else if (umbChv1 == P2_BYTE) { // if its CHV1
		
		/*CHV1 key reference*/
		umbKeyRef = MAPPED_KEY_REF_TO_CHV_1;
	}
	else if ((PIN_SIM_ADM_1 <= P2_BYTE) && (PIN_SIM_ADM_11 >= P2_BYTE)) { // if its ADM 04-0E

		/*Get UICC ADM key reference*/
		if(P2_BYTE < PIN_SIM_ADM_6) // P2 is less than ADM6 i.e. '9'
		{
			// if ADM is in range 04-08
			umbKeyRef = P2_BYTE - PIN_SIM_ADM_1 + PIN_ADM_1;
		}
		else {
			// if ADM is in range 09-0E
			umbKeyRef = P2_BYTE - PIN_SIM_ADM_6 + PIN_ADM_6;
		}
	}
	else { //invalid P2
		DBG_ERR("[51_011] _51011_validatePinCommand: Invalid P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_validatePinCommand() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (umbLc != P3_BYTE) { // Check if incorrect P3 is provided

		DBG_ERR("[51_011] _51011_validatePinCommand: Wrong P3, Correct value %1 \n", umbLc);
		DBG_FUNC_EXIT("<<[51_011] _51011_validatePinCommand() \n");
		THROW2(EXCEP_WRONG_P3, umbLc);
	}


	DBG_FUNC_EXIT("<<[51_011] _51011_validatePinCommand() returns %1\n", umbKeyRef);
	/* Mapped Key reference to presented CHV*/
	return umbKeyRef;
}
/*
* It implements the Verify CHV command as per 51.011
*/
void _51011_verifyChv(void) {

	uMonoByte		umbKeyRef;
	tPinStatusCode	pscReturned;

	DBG_FUNC_ENTRY(">>[51_011] _51011_verifyChv() \n");

	/*Do P1-P2 validation*/
	umbKeyRef = _51011_validatePinCommand(PIN_CHV_1, LENGTH_PIN_VALUE);

	DBG_FUNC("[51_011] _51011_verifyChv: P1-P2 validation succeeded, Mapped key reference is %1 \n", umbKeyRef);

	/*Verify Pin function, Pin must be enabled to verify*/
	pscReturned = _7816_4_verifyPin(umbKeyRef, PIN_ENABLED,  COMMAND_DATA, PIN_OP_VERIFY);

	if (pscReturned > PIN_SC_BLOCKED) { //check if verification failed

		/*In case of 51.011 remainig attempts are not need to be sent, So updating the variable*/
		pscReturned = PIN_SC_NOT_VERIFIED;
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_verifyChv() \n");

	/*it throws status error in case of error status code*/
	_102221_throwStatusResponseAsPer(pscReturned);
}

/*
* It implements the Change CHV command as per 51.011
*/
void _51011_changeChv(void) {

	tPinStatusCode pscReturned;
	uMonoByte umbKeyRef;

	DBG_FUNC_ENTRY(">>[51_011] _51011_changeChv() \n");

	/*Do P1-P2 and validation: First argument is valid CHV1 and second argument two pins are mandatory in Change command*/
	umbKeyRef = _51011_validatePinCommand(PIN_CHV_1, LENGTH_VALID_FOR_CHANGE_UNBLOCK_PIN);

	DBG_FUNC("[51_011] _51011_changeChv: P1-P2 validation succeeded, Mapped key reference is %1 \n", umbKeyRef);

	/*Call the function to change PIN*/
	pscReturned = _7816_4_changeUnblockPin(umbKeyRef, PIN_OP_CHANGE, COMMAND_DATA, COMMAND_DATA + LENGTH_PIN_VALUE);

	if (pscReturned > PIN_SC_BLOCKED) { //check if verification failed

		if (PIN_SC_NOT_VERIFIED == pscReturned) { // No attempts left
			//so pin is blocked now
			pscReturned = PIN_SC_BLOCKED;
		}
		else { // for non-zero attempts
			/*In case of 51.011 remainig attempts are not need to be sent, It always throws PIN_SC_NOT_VERIFIED*/
			pscReturned = PIN_SC_NOT_VERIFIED;
		}
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_changeChv() \n");

	/*it throws status error in case of error status code*/
	_102221_throwStatusResponseAsPer(pscReturned);
}

/*
* It implements the Unblock CHV command as per 51.011
*/
void _51011_unblockChv(void) {

	tPinStatusCode pscReturned;
	uMonoByte umbKeyRef;

	DBG_FUNC_ENTRY(">>[51_011] _51011_unblockChv() \n");

	/*Do P1-P2 and validation: First argument is valid CHV1 and second argument two pins are mandatory in Change command*/
	umbKeyRef = _51011_validatePinCommand(PIN_UNBLOCK_CHV_1, LENGTH_VALID_FOR_CHANGE_UNBLOCK_PIN);

	DBG_FUNC("[51_011] _51011_unblockChv: P1-P2 validation succeeded, Mapped key reference is %1 \n", umbKeyRef);

	/*Call the function to Unblock PIN*/
	pscReturned = _7816_4_changeUnblockPin(umbKeyRef, PIN_OP_UNBLOCK, COMMAND_DATA, COMMAND_DATA + LENGTH_PIN_VALUE);

	if (pscReturned > PIN_SC_BLOCKED) { //check if verification failed

		/*In case of 51.011 remainig attempts are not need to be sent, It always throws PIN_SC_NOT_VERIFIED*/
		pscReturned = PIN_SC_NOT_VERIFIED;
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_unblockChv() \n");

	/*it throws status error in case of error status code*/
	_102221_throwStatusResponseAsPer(pscReturned);
}

/*
* It implements the Enable CHV command as per 51.011
*/
void _51011_enableChv(void) {

	uMonoByte		umbKeyRef;
	tPinStatusCode	pscReturned;

	DBG_FUNC_ENTRY(">>[51_011] _51011_enableChv() \n");

	if (PIN_CHV_1 != P2_BYTE) { //Check if p2 conatins invalid key reference

		DBG_ERR("[51_011] _51011_enableChv: Invalid P1 - P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_enableChv() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}
	/*P1 and P3 validation, and get Mapped key reference*/
	umbKeyRef = _51011_validatePinCommand(PIN_CHV_1, LENGTH_PIN_VALUE);

	DBG_FUNC("[51_011] _51011_enableChv: P1-P2 validation succeeded, Mapped key reference is %1 \n", umbKeyRef);

	/*Call the function to change PIN state*/
	pscReturned = _7816_4_changePinState(umbKeyRef, PIN_OP_ENABLE, COMMAND_DATA);

	if (pscReturned > PIN_SC_BLOCKED) { //check if Pin verification failed

		if (PIN_SC_NOT_VERIFIED == pscReturned) {// no attempts left
			//so pin is blocked now
			pscReturned = PIN_SC_BLOCKED;
		}
		else {// for non-zero attempts

			  /*In case of 51.011 remainig attempts are not need to be sent, It always throws PIN_SC_NOT_VERIFIED*/
			pscReturned = PIN_SC_NOT_VERIFIED;
		}
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_enableChv() \n");

	/*it throws status error in case of error status code*/
	_102221_throwStatusResponseAsPer(pscReturned);

}

/*
* It implements the Disable CHV command as per 51.011
*/
void _51011_disableChv(void) {

	uMonoByte		umbKeyRef;
	tPinStatusCode	pscReturned;
	
	DBG_FUNC_ENTRY(">>[51_011] _51011_disableChv() \n");

	if (PIN_CHV_1 != P2_BYTE) { //Check if p2 conatins invalid key reference

		DBG_ERR("[51_011] _51011_disableChv: Invalid P1 - P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_disableChv() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}
	/*P1 and P3 validation, and get Mapped key reference*/
	umbKeyRef = _51011_validatePinCommand(PIN_CHV_1, LENGTH_PIN_VALUE);

	DBG_FUNC("[51_011] _51011_disableChv: P1-P2 validation succeeded, Mapped key reference is %1 \n", umbKeyRef);

	/*Call the function to change PIN state*/
	pscReturned = _7816_4_changePinState(umbKeyRef, PIN_OP_DISABLE, COMMAND_DATA);

	if (pscReturned > PIN_SC_BLOCKED) { //check if Pin verification failed

		if (PIN_SC_NOT_VERIFIED == pscReturned) {// no attempts left
			//so pin is blocked now
			pscReturned = PIN_SC_BLOCKED;
		}
		else {// for non-zero attempts

			  /*In case of 51.011 remainig attempts are not need to be sent, It always throws PIN_SC_NOT_VERIFIED*/
			pscReturned = PIN_SC_NOT_VERIFIED;
		}
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_disableChv() \n");

	/*it throws status error in case of error status code*/
	_102221_throwStatusResponseAsPer(pscReturned);

}