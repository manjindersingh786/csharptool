#include"7816_4.h"

#define RECORD_FOUND												(0x01)
#define POS_BIT_PIN_SECOND_APP_1									(0x10)
#define DEFAULT_ALWAYS												(0x80000000)

/*It stores verifications status of different key references*/
tPinData	pdPinStatus = DEFAULT_ALWAYS;
/*
* Search Key config file for given key reference and get file addresses
*
* @param umbKeyReference: Key reference to search
* @param pfhaKeyConfigFile: will contain address of Key Config file
* @param pfhaKeyValuesFile: will contain address of Key Values file
*
* @return Record no. if Key reference found, otherwise RECORD_UNDEFINED
*
* @pre Prop. PIN files must be created
*/
static uMonoByte _7816_4_searchForKeyRef(IN_ARG uMonoByte umbKeyReference,
	OUT_ARG ptFileHeaderAddr pfhaKeyConfigFile,
	OUT_ARG ptFileHeaderAddr pfhaKeyValuesFile) {

	uMonoByte umbKeyConfigRecord = RECORD_UNDEFINED;

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_searchForKeyRef() \n");

	/*Get file address of Prop. DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, pfhaKeyValuesFile);

	/*Get file address of Prop. Key Config file*/
	_7816_4_ifFileExists(FID_PROP_KEYS_CONFIG, *pfhaKeyValuesFile, MODE_FIND_FID, pfhaKeyConfigFile);

	/*Get file address of Prop. Key Value file*/
	_7816_4_ifFileExists(FID_PROP_KEYS_VALUE, *pfhaKeyValuesFile, MODE_FIND_FID, pfhaKeyValuesFile);


	assert(NO_FILE_PRESENT != *pfhaKeyConfigFile); // ensure key config file is created
	assert(NO_FILE_PRESENT != *pfhaKeyValuesFile); // ensure keys value file is created
	assert(LENGTH_RECORD_OF_KEY_CONFIG == _7816_4_getRecordLength(*pfhaKeyConfigFile)); // ensure the record size of key config file
	assert(LENGTH_RECORD_OF_KEY_VALUE == _7816_4_getRecordLength(*pfhaKeyValuesFile)); // ensure the record size of key value file

	DBG_FUNC("[7816_4] _7816_4_searchForKeyRef: Search for key reference %1 in Key Config file\n", umbKeyReference);

	/*Search for given key reference*/
	_7816_4_SearchRecordFile(*pfhaKeyConfigFile, &umbKeyReference, 0x01, SEARCH_RECORD_FORWARD, SEARCH_FIRST_OCCURRENCE, RECORD_FIRST, OFFSET_KEY_CONFIG_KEY_REFERENCE, &umbKeyConfigRecord);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_searchForKeyRef() returns %1\n", umbKeyConfigRecord);

	/*either it will contain Record no. or RECORD_UNDEFINED*/
	return umbKeyConfigRecord;
}

/*
* It returns the remaining attempts of given key reference
*
* @param umbKeyRef: Key reference
* @param poFor: P
*
* @return remaining attempts
* @precondition umbKeyReference must be valid key reference
*/
uMonoByte _7816_4_remainingAttemptsOfPin(IN_ARG uMonoByte umbKeyReference, IN_ARG tPinOperation poFor) {

	uMonoByte		umbKeyConfigRecord;
	tFileHeaderAddr fhaKeyConfigFile;
	tFileHeaderAddr fhaKeyValuesFile;
	uMonoByte		umbConfigData[LENGTH_RECORD_OF_KEY_CONFIG];
	
	/*Search for given key reference and Key files*/
	umbKeyConfigRecord = _7816_4_searchForKeyRef(umbKeyReference, &fhaKeyConfigFile, &fhaKeyValuesFile);

	/*read the whole record*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_READ);
	
	if (PIN_OP_VERIFY == poFor) {// remaining attempts of Verify PIN
		return umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER];
	}
	else { // remaining attempts of Unblock PIN
		return umbConfigData[OFFSET_KEY_CONFIG_UNBLOCK_COUNTER];
	}
}

/*
* It returns the activation status of the corresponding pin
*
* @param umbKeyRef: Key reference
*
* @return Enabled pin: PIN_ENABLED otherwise PIN_DISABLED
*/
tPinState _7816_4_isPinActive(uMonoByte umbKeyReference) {

	uMonoByte		umbKeyConfigRecord;
	tFileHeaderAddr fhaKeyConfigFile;
	tFileHeaderAddr fhaKeyValuesFile;
	uMonoByte		umbConfigData[LENGTH_RECORD_OF_KEY_CONFIG];

	/*Search for given key reference and Key files*/
	umbKeyConfigRecord = _7816_4_searchForKeyRef(umbKeyReference, &fhaKeyConfigFile, &fhaKeyValuesFile);

	/*read the whole record*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_READ);

	return umbConfigData[OFFSET_KEY_CONFIG_PIN_STATE];
}

/*
* Check the reference data or P2 parameter if its invalid acc. to 102221
*
* @param umbReferenceData: Key reference data
*
* @retruns TRUE if key reference is invalid, FALSE if its not
*/
boolean _7816_4_isInvalidReferenceData(uMonoByte umbReferenceData) {

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_isInvalidReferenceData()\n");

	DBG_FUNC("[7816_4] _7816_4_isInvalidReferenceData: Checking Key reference value %1\n", umbReferenceData);

	/*Check validation of Key Reference Data*/
	if ((0x00 == (umbReferenceData & MASK_HIGHER_NIBBLE))
		||
		(0x80 == (umbReferenceData & MASK_HIGHER_NIBBLE))) {

		umbReferenceData &= MASK_LOWER_NIBBLE;

		/*Check below if its not an invalid Key reference*/
		if ((0x00 != umbReferenceData)
			&&
			(0x09 != umbReferenceData)
			&&
			(0x0F != umbReferenceData)) {

			/*Presented key reference is valid*/
			DBG_FUNC("[7816_4] _7816_4_isInvalidReferenceData: Presented key reference is valid\n");
			DBG_FUNC_EXIT("<<[7816_4] _7816_4_isInvalidReferenceData() returns FALSE %1\n", FALSE);
			return FALSE;
		}
	}

	DBG_FUNC("[7816_4] _7816_4_isInvalidReferenceData: Presented key reference is invalid\n");

	/*In current implementation Universal PIN 'PIN_UNIVERSAL' is also treated as invalid key reference*/
	/*If control is here it means present reference data is invalid */
	DBG_FUNC_EXIT("<<[7816_4] _7816_4_isInvalidReferenceData() returns TRUE %1\n", TRUE);
	return TRUE;

}

/*
* Set the corresponding bit in give Pin data variable as per key reference
*
* @param uqbPinData: PIN data
* @param umbKeyRef: Key reference
*
* @precondition Must be a valid key reference i.e. 01-08, 81-88, 0A-0E and 8A-8E
*/
void _7816_4_setKeyRef(ptPinData ppdPinData, uMonoByte umbKeyRef) {

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_setKeyRef() \n");
	/*
	* Ensure keys must be within 01-08, 81-88, 0A-0E and 8A-8E inclusive
	*/
	assert(((PIN_APP_1 <= umbKeyRef) && (umbKeyRef <= PIN_APP_8))\
			||\
			((PIN_SECOND_APP_1 <= umbKeyRef) && (umbKeyRef <= PIN_SECOND_APP_8))\
			||\
			((PIN_ADM_1 <= umbKeyRef) && (umbKeyRef <= PIN_ADM_5))\
			||\
			((PIN_ADM_6 <= umbKeyRef) && (umbKeyRef <= PIN_ADM_10)));

	if (umbKeyRef < PIN_SECOND_APP_1) { //01-08 and 0A-0E

		umbKeyRef -= PIN_APP_1;
	}
	else { //81-88 and 8A-8E

		umbKeyRef = POS_BIT_PIN_SECOND_APP_1 + umbKeyRef - PIN_SECOND_APP_1;
	}

	/*Set corresponding bit*/
	(*ppdPinData) |= (0x01 << umbKeyRef);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_setKeyRef() \n");
}

/*
* Returns the the verification status of given key reference
*
* @param umbKeyRef: Key reference
*
* @return ACC_VERIFIED if pin verified, otherwise ACC_NOT_VERIFIED 
* @precondition Must be a valid key reference i.e. 01-08, 81-88, 0A-0E, 8A-8E and 90
*/
tAccessResult _7816_4_isKeyRefVerified(uMonoByte umbKeyRef) {

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_isKeyRefVerified() \n");
	/*
	* Ensure keys must be within 01-08, 81-88, 0A-0E, 8A-8E inclusive and 90
	*/
	assert(((PIN_APP_1 <= umbKeyRef) && (umbKeyRef <= PIN_APP_8))\
		|| \
		((PIN_SECOND_APP_1 <= umbKeyRef) && (umbKeyRef <= PIN_SECOND_APP_8))\
		|| \
		((PIN_ADM_1 <= umbKeyRef) && (umbKeyRef <= PIN_ADM_5))\
		|| \
		((PIN_ADM_6 <= umbKeyRef) && (umbKeyRef <= PIN_ADM_10))
		|| \
		(PIN_ALWAYS == umbKeyRef) );

	DBG_FUNC("[7816_4] _7816_4_isKeyRefVerified: Checking verification status of PIN/Key %1\n", umbKeyRef);

	if (umbKeyRef < PIN_SECOND_APP_1) { //01-08 and 0A-0E

		umbKeyRef -= PIN_APP_1;
	}
	else { //81-88, 8A-8E and 90

		umbKeyRef = POS_BIT_PIN_SECOND_APP_1 + umbKeyRef - PIN_SECOND_APP_1;
	}

	/*Set corresponding bit*/
	if (pdPinStatus & (0x01 << umbKeyRef)) {
		DBG_FUNC("[7816_4] _7816_4_isKeyRefVerified: Pin has been verified\n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_isKeyRefVerified() returns ACC_VERIFIED %1\n", ACC_VERIFIED);
		return ACC_VERIFIED;
	}

	DBG_FUNC("[7816_4] _7816_4_isKeyRefVerified: Pin verified\n");
	DBG_FUNC_EXIT("<<[7816_4] _7816_4_isKeyRefVerified() \n");
	return ACC_NOT_VERIFIED;
}

/*
* Update the status of given PIN as verified
* 
* @param umbKeyRef: Key reference
* 
* @precondition Must be a valid key reference i.e. 01-08, 81-88, 0A-0E and 8A-8E
*/
void _7816_4_markKeyAsVerified(uMonoByte umbKeyRef) {


	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_markKeyAsVerified() \n");

	/*
	* Ensure keys must be within 01-08, 81-88, 0A-0E and 8A-8E inclusive
	*/
	assert(((PIN_APP_1 <= umbKeyRef) && (umbKeyRef <= PIN_APP_8))\
		|| \
		((PIN_SECOND_APP_1 <= umbKeyRef) && (umbKeyRef <= PIN_SECOND_APP_8))\
		|| \
		((PIN_ADM_1 <= umbKeyRef) && (umbKeyRef <= PIN_ADM_5))\
		|| \
		((PIN_ADM_6 <= umbKeyRef) && (umbKeyRef <= PIN_ADM_10)));

	DBG_FUNC("[7816_4] _7816_4_markKeyAsVerified: Before setting the Key reference %1 bit, pin value is %4\n", umbKeyRef, pdPinStatus);

	/*Set Bit of given Key ref*/
	_7816_4_setKeyRef(&pdPinStatus, umbKeyRef);

	DBG_FUNC("[7816_4] _7816_4_markKeyAsVerified: After Setting Pin status value is %4\n", pdPinStatus);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_markKeyAsVerified() \n");
}

/*
* It implements the Verify Pin function
*
* @param umbKeyReference: identifies the key reference, upon which operation to perform
* @param psExpected: What state does Requester expect from the PIN to perform an operation
* @param pumbPin: Stores presented pin value
* @param poFor: Pin operation  PIN_OP_VERIFY or PIN_OP_VERIFY_COUNTER
*
* @return tPinStatusCode tells if operation succedded or not
* @precondition: assuming length of pumbPin is enough to read data
*/
tPinStatusCode _7816_4_verifyPin(IN_ARG uMonoByte	umbKeyReference,
								IN_ARG tPinState	psExpected,
								IN_ARG puMonoByte	pumbPin,
								IN_ARG tPinOperation poFor) {

	uMonoByte		umbKeyConfigRecord;
	tFileHeaderAddr fhaKeyValuesFile;
	tFileHeaderAddr fhaKeyConfigFile;
	uMonoByte		umbConfigData[LENGTH_RECORD_OF_KEY_CONFIG];

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_verifyPin() \n");

	DBG_FUNC("[7816_4] _7816_4_verifyPin: Key Reference %1\n", umbKeyReference);

	/*Search for given key reference and Key files*/
	if (RECORD_UNDEFINED == (umbKeyConfigRecord = _7816_4_searchForKeyRef(umbKeyReference, &fhaKeyConfigFile, &fhaKeyValuesFile))) {

		/*Given Key reference is not available in Key config file*/
		DBG_ERR("[7816_4] _7816_4_verifyPin: Key reference not found\n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_verifyPin() returns %1\n", PIN_SC_NOT_FOUND);
		return PIN_SC_NOT_FOUND;
	}

	/*read the whole record*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_READ);

	if (PIN_OP_VERIFY_COUNTER == poFor) {
		
		/*Request to return PIN retry counter only*/
		DBG_WARN("[7816_4] _7816_4_verifyPin: For Pin verification %1 attempts left\n", umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]);
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_verifyPin() returns %1\n", PIN_SC_NOT_VERIFIED + umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]);
		return (PIN_SC_NOT_VERIFIED + umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]);
	}

	/*Check if Current Pin state is not equal to expected one*/
	if (psExpected != umbConfigData[OFFSET_KEY_CONFIG_PIN_STATE]) {

		/*Current state of Pin is different than expected one*/
		DBG_ERR("[7816_4] _7816_4_verifyPin: PIN has invalid state %1 expected %1\n", umbConfigData[OFFSET_KEY_CONFIG_PIN_STATE], psExpected);
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_verifyPin() returns %1\n", PIN_SC_INVALID_STATE);
		return PIN_SC_INVALID_STATE;
	}

	/*Check if retry limit exceeds*/
	if (0x00 == umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]) { // if remaining attempts are zero

		/*No retry attempts left as per key config data*/
		DBG_ERR("[7816_4] _7816_4_verifyPin: Retry attempts for corresponding operation exceeds\n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_verifyPin() returns %1\n", PIN_SC_BLOCKED);
		return PIN_SC_BLOCKED;
	}

	DBG_FUNC("[7816_4] _7816_4_verifyPin: There are still few attempts left\n");

	/*Decrement the retry counter*/
	umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]--;

	/*update the whole record with decremented counter*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_UPDATE);

	/*Verify the presented pin*/
	if (FALSE == _7816_4_compareWithRecord(fhaKeyValuesFile, umbConfigData[OFFSET_KEY_CONFIG_VERIFY_PIN_VALUE_RECORD], OFFSET_START, pumbPin, LENGTH_PIN_VALUE)) { // if pin doesnt match

		/*PIN verification failed*/
		DBG_ERR("[7816_4] _7816_4_verifyPin: Pin verification failed. %1 attempts left\n", umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]);
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_verifyPin() returns %1\n", PIN_SC_NOT_VERIFIED + umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]);
		return (PIN_SC_NOT_VERIFIED + umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER]);
	}

	DBG_FUNC("[7816_4] _7816_4_verifyPin: Pin Verified. Reset the Verify counter\n");

	/*Reset the Verify counter*/
	umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER] = umbConfigData[OFFSET_KEY_CONFIG_VERIFY_LIMIT];

	/*update the key config file after resetting counters*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_UPDATE);
	
	/*Mark the requested PIN as verified*/
	_7816_4_markKeyAsVerified(umbKeyReference);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_verifyPin() \n");

	return PIN_SC_VERIFIED;
}

/*
* It implements the Change and Unblock Pin function
* 
* @param umbKeyReference: identifies the key reference, upon which operation to perform.
* @param poMode: Pin operation
* @param pumbPin: Stores Current pin value in case of PIN_OP_CHANGE operation
*					Stores Unblock PIN value in case of PIN_OP_UNBLOCK operation
* @param pumbNewKey: New key to set corresponding to given key reference
* 
* @return tPinStatusCode tells if operation succedded or not
* @precondition: assuming length of pumbPin and pumbNewKey is enough to read data
*/
tPinStatusCode _7816_4_changeUnblockPin(IN_ARG uMonoByte umbKeyReference, 
										IN_ARG tPinOperation poMode, 
										IN_ARG puMonoByte pumbPin, 
										IN_ARG puMonoByte pumbNewKey) {
	uMonoByte		umbConfigIndex;
	uMonoByte		umbKeyConfigRecord;
	tFileHeaderAddr fhaKeyValuesFile;
	tFileHeaderAddr fhaKeyConfigFile;
	uMonoByte		umbConfigData[LENGTH_RECORD_OF_KEY_CONFIG];

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_changeUnblockPin() \n");
	
	if (((PIN_ADM_1 <= umbKeyReference) && (PIN_ADM_5 >= umbKeyReference))
		||
		((PIN_ADM_6 <= umbKeyReference) && (PIN_ADM_10 >= umbKeyReference))) {

		/*Change/Unblock PIN command can not be applicable for ADM */
		DBG_ERR("[7816_4] _7816_4_changeUnblockPin: ADM PIN can not be changed \n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_changeUnblockPin() returns %1\n", PIN_SC_INVALID_STATE);
		return PIN_SC_INVALID_STATE;
	}

	/*Search for given key reference and Key files*/
	if (RECORD_UNDEFINED == (umbKeyConfigRecord = _7816_4_searchForKeyRef(umbKeyReference, &fhaKeyConfigFile, &fhaKeyValuesFile))) {

		/*Given Key reference is not available in Key config file*/
		DBG_ERR("[7816_4] _7816_4_changeUnblockPin: Key reference not found\n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_changeUnblockPin() returns %1\n", PIN_SC_NOT_FOUND);
		return PIN_SC_NOT_FOUND;
	}

	if (PIN_OP_CHANGE == poMode) { //Operation Change PIN

		DBG_FUNC("[7816_4] _7816_4_changeUnblockPin: Requested operation is Change PIN\n");
		umbConfigIndex = OFFSET_KEY_CONFIG_VERIFY_PIN;
	}
	else { //Operation Unblock PIN

		DBG_FUNC("[7816_4] _7816_4_changeUnblockPin: Requested operation is Unblock PIN\n");
		umbConfigIndex = OFFSET_KEY_CONFIG_UNBLOCK_PIN;
	}

	/*read the whole record*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_READ);

	if (PIN_OP_UNBLOCK_COUNTER == poMode) {

		/*Remaining attempts of Unblock PIN verification */
		DBG_ERR("[7816_4] _7816_4_changeUnblockPin: For Unblock Pin verification %1 attempts left\n", umbConfigData[OFFSET_KEY_CONFIG_UNBLOCK_COUNTER]);
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_changeUnblockPin() returns %1\n", (PIN_SC_NOT_VERIFIED + umbConfigData[OFFSET_KEY_CONFIG_UNBLOCK_COUNTER]));
		return (PIN_SC_NOT_VERIFIED + umbConfigData[OFFSET_KEY_CONFIG_UNBLOCK_COUNTER]);
	}

	if ((PIN_OP_CHANGE == poMode) && (PIN_DISABLED == umbConfigData[OFFSET_KEY_CONFIG_PIN_STATE])) { //If operation is Change PIN then check if its disable or not

		/*Pin is already disabled, it needs to be enabled before performing the operation*/

		DBG_ERR("[7816_4] _7816_4_changeUnblockPin: PIN has been disabled\n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_changeUnblockPin() returns %1\n", PIN_SC_INVALID_STATE);
		return PIN_SC_INVALID_STATE;
	}

	/*Check if retry limit exceeds*/
	if (0x00 == umbConfigData[umbConfigIndex + OFFSET_RELATIVE_COUNTER]) { // if remaining attempts are zero

		/*No retry attempts left as per key config data*/
		DBG_ERR("[7816_4] _7816_4_changeUnblockPin: Retry attempts for corresponding operation exceeds\n");
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_changeUnblockPin() returns %1\n", PIN_SC_BLOCKED);
		return PIN_SC_BLOCKED;
	}

	DBG_FUNC("[7816_4] _7816_4_changeUnblockPin: There are still few attempts left\n");

	/*Decrement the retry counter*/
	umbConfigData[umbConfigIndex + OFFSET_RELATIVE_COUNTER]--;

	/*update the whole record with decremented counter*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_UPDATE);

	/*Verify the presented pin*/
	if (FALSE == _7816_4_compareWithRecord(fhaKeyValuesFile, umbConfigData[umbConfigIndex + OFFSET_RELATIVE_PIN_VALUE_RECORD], OFFSET_START, pumbPin, LENGTH_PIN_VALUE)) { // if pin doesnt match

		/*PIN verification failed*/
		DBG_ERR("[7816_4] _7816_4_changeUnblockPin: Pin verification failed. %1 attempts left\n", umbConfigData[umbConfigIndex + OFFSET_RELATIVE_COUNTER]);
		DBG_FUNC_EXIT("<<[7816_4] _7816_4_changeUnblockPin() returns %1\n", (PIN_SC_NOT_VERIFIED + umbConfigData[umbConfigIndex + OFFSET_RELATIVE_COUNTER]));
		return (PIN_SC_NOT_VERIFIED + umbConfigData[umbConfigIndex + OFFSET_RELATIVE_COUNTER]);
		
	}

	DBG_FUNC("[7816_4] _7816_4_changeUnblockPin: Pin Verified. Update the new pin and reset the verify and unblock counters\n");

	/*Update the new pin*/
	_7816_4_readUpdateRecordFile(fhaKeyValuesFile, pumbNewKey, umbConfigData[OFFSET_KEY_CONFIG_VERIFY_PIN_VALUE_RECORD], ACC_MODE_FILE_UPDATE);

	/*Reset the Verify and unblock counters*/
	umbConfigData[OFFSET_KEY_CONFIG_VERIFY_COUNTER] = umbConfigData[OFFSET_KEY_CONFIG_VERIFY_LIMIT]; // reset the verify counter
	umbConfigData[umbConfigIndex + OFFSET_RELATIVE_COUNTER] = umbConfigData[umbConfigIndex + OFFSET_RELATIVE_LIMIT]; // reset the Unblock/verify counter
	
	
	/*Update the state of key after unblocking*/
	DBG_FUNC("[7816_4] _7816_4_changeUnblockPin: Enable the pin while unblocking the card\n");
	umbConfigData[OFFSET_KEY_CONFIG_PIN_STATE] = PIN_ENABLED;

	/*update the key config file after resetting counters*/
	_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_UPDATE);

	/*Mark the requested PIN as verified*/
	_7816_4_markKeyAsVerified(umbKeyReference);

	DBG_FUNC_EXIT("<<[7816_4] _7816_4_changeUnblockPin() \n");
	return PIN_SC_VERIFIED;
}

/*
* It implements the Enable & Disable PIN function
*
* @param umbKeyReference: identifies the key reference, upon which operation to perform.
* @param poMode: Pin operation : PIN_OP_ENABLE or PIN_OP_DISABLE
* @param pumbPin: Pin value presented
*
* @return tPinStatusCode tells if operation succedded or not
* @precondition: assuming length of array pumbPin is enough to read data, operation can only be PIN_OP_ENABLE or PIN_OP_DISABLE
*/
tPinStatusCode _7816_4_changePinState(	IN_ARG	uMonoByte		umbKeyReference,
										IN_ARG	tPinOperation	poCommand,
										IN_ARG	puMonoByte		pumbPin) {
	uMonoByte			umbKeyConfigRecord;
	tPinState			psVal;
	tPinStatusCode		pscReturned;
	tFileHeaderAddr		fhaKeyConfigFile;
	tFileHeaderAddr		fhaKeyValuesFile;
	uMonoByte			umbConfigData[LENGTH_RECORD_OF_KEY_CONFIG];

	assert((PIN_OP_ENABLE == poCommand)||(PIN_OP_DISABLE == poCommand));

	DBG_FUNC_ENTRY(">>[7816_4] _7816_4_changePinState() \n");

	/*verify the PIN first before changing the pin state*/
	if (poCommand == PIN_OP_ENABLE) { // If operation is Enable Pin command

		DBG_FUNC("[7816_4] _7816_4_changePinState: Request to Enable the Pin \n");

		/*Verify PIN, Pin state must be disabled, so that command can enable it successfully*/
		pscReturned = _7816_4_verifyPin(umbKeyReference, PIN_DISABLED, pumbPin, PIN_OP_VERIFY);

		/*new pin state*/
		psVal = PIN_ENABLED;
	}
	else { // Operation is disable pin command

		DBG_FUNC("[7816_4] _7816_4_changePinState: Request to Disable the Pin \n");

		/*Verify PIN, Pin state must be Enabled, so that command can disable it successfully*/
		pscReturned = _7816_4_verifyPin(umbKeyReference, PIN_ENABLED, pumbPin, PIN_OP_VERIFY);

		/*new pin state*/
		psVal = PIN_DISABLED;
	}

	if (PIN_SC_VERIFIED == pscReturned) {

		/*Search the key reference in key config file and retreieve the record no.*/
		umbKeyConfigRecord = _7816_4_searchForKeyRef(umbKeyReference, &fhaKeyConfigFile, &fhaKeyValuesFile);

		/*read the whole record*/
		_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_READ);

		/*Update the new PIN state*/
		umbConfigData[OFFSET_KEY_CONFIG_PIN_STATE] = psVal;

		DBG_FUNC("[7816_4] _7816_4_changePinState: Updating the Pin state to %1 \n", psVal);

		/*Update the whole record, after changing pin state*/
		_7816_4_readUpdateRecordFile(fhaKeyConfigFile, umbConfigData, umbKeyConfigRecord, ACC_MODE_FILE_UPDATE);

	}
	DBG_FUNC_EXIT("<<[7816_4] _7816_4_changePinState() \n");

	return pscReturned;
}

