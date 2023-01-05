#include "OasisCommands.h"

#define PIN_CARD_ISSUER_LENGTH						(0x10)

const uMonoByte CARD_ISSUER_PIN[PIN_CARD_ISSUER_LENGTH] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08 };

#ifdef _DEBUG
/*This is debug function to check the presence of all propreitary mandatory files*/
static boolean IsAllMandatoryFilesPresent(void) {
	uMonoByte		umbIndex;
	tFileHeaderAddr fhaDf;
	tFileHeaderAddr fhaEf;
	const uDiByte udbFileList[NO_OF_PROP_EF] = { FID_PROP_ADF_LIST ,\
													FID_PROP_KEYS_CONFIG,\
													FID_PROP_KEYS_VALUE,\
													FID_PROP_AUTH_KI,\
													FID_PROP_AUTH_OPC,\
													FID_PROP_AUTH_SEQ_ARRAY,\
													FID_PROP_RFM_PARAMETER,\
													FID_PROP_TERMINAL_PROFILE,\
													FID_PROP_KEY_MANAGEMENT,\
													FID_PROP_KEY_VALUE,\
													FID_PROP_SECURED_MESSAGE };

	/*Search DF under MF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaDf);

	/*Check if df available*/
	if (NO_FILE_PRESENT == fhaDf) {

		/*As DF is not present*/
		return FALSE;
	}
	/*Traverse throuugh each prop. file ID*/
	for (umbIndex = 0x00; umbIndex < NO_OF_PROP_EF; umbIndex++) {

		/*Search Prop. EF file */
		_7816_4_ifFileExists(udbFileList[umbIndex], fhaDf, MODE_FIND_FID, &fhaEf);

		if (NO_FILE_PRESENT == fhaEf) {

			/*File is not found*/
			return FALSE;
		}
	}

	/*All files found*/
	return TRUE;
}
#endif

/*
* It sets new Life cycle state of card
*/
void oasis_SetCardState(void) {

	DBG_FUNC_ENTRY(">>[OASIS_COMMANDS] oasis_SetCardState()\n");

	if( (0x00 != P1_BYTE)  // P1 is not zero
		||					//OR
		((STATE_INITIALIZED != P2_BYTE) && (STATE_SECURED != P2_BYTE))){ // P2 is neither STATE_INITIALIZED nor STATE_SECURED
		/*P1 is incorrect must be '00'*/
		DBG_ERR("[OASIS_COMMANDS] oasis_SetCardState: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[OASIS_COMMANDS] oasis_SetCardState() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}
	
	DBG_FUNC("[OASIS_COMMANDS] oasis_SetCardState: New state would be %1\n", P2_BYTE);

#ifdef _DEBUG
	if (FALSE == IsAllMandatoryFilesPresent()) {

		/*It means there is a file which is not available under Prop DF*/
		THROW(EXCEP_CONDITIONS_OF_USE_NOT_SATISFIED);
	}
#endif

	/*Change Card state*/
	_7186_4_changeCardState(P2_BYTE);

	DBG_FUNC_EXIT("<<[OASIS_COMMANDS] oasis_SetCardState()\n");
}

/*
* It extract the Memory related information
*/
/*
	{//DBGCODE
		extern uMonoByte umbDBG[];
		extern uMonoByte	umbDbgLen;
		mm_storeUdiByte(umbDBG,umbDbgLen,fhaFile);
	}
*/
/*DBGCODE*/
uMonoByte umbDBG[0x80];
uMonoByte	umbDbgLen = 0x00;
void oasis_getMemoryConfig(void) {
	
	if(P3_BYTE != (0x20 + TOTAL_SYSTEM_DATA_LEN + umbDbgLen)){
		THROW2(EXCEP_WRONG_P3, (0x20 + TOTAL_SYSTEM_DATA_LEN + umbDbgLen));
	}
	mm_storeWord(COMMAND_RESPONSE_DATA,0x00,ADDRESS_ERASE_EEPROM_START);
	mm_storeWord(COMMAND_RESPONSE_DATA,0x04,ADDRESS_SYSTEM_AREA_START);
	mm_storeWord(COMMAND_RESPONSE_DATA,0x08,ADDRESS_DISK_MEMORY_START);
	mm_storeWord(COMMAND_RESPONSE_DATA,0x0C,ADDRESS_DISK_MEMORY_END);
	mm_storeWord(COMMAND_RESPONSE_DATA,0x10,ADDRESS_SMART_START);
	mm_storeWord(COMMAND_RESPONSE_DATA,0x14,ADDRESS_SMART_END);
	mm_storeWord(COMMAND_RESPONSE_DATA,0x18,ADDRESS_CLUSTER_START);
	mm_storeWord(COMMAND_RESPONSE_DATA,0x1C,ADDRESS_CLUSTER_END);
	/*Read all elements of system page data in one write*/
	baseReadNBytes(ADDRESS_NEXT_FREE_CLUSTER, (COMMAND_RESPONSE_DATA + 0x20), TOTAL_SYSTEM_DATA_LEN);
	mem_cpy(COMMAND_RESPONSE_DATA+0x27, umbDBG, umbDbgLen);
	mem_set(umbDBG, 0x00, umbDbgLen);
	gudbOutputLen = 0x27 + umbDbgLen;
	umbDbgLen=0x00;
}

/*
* It returns to bootloader state
*/
void oasis_switchToBL(void) {
	
	if(P1_BYTE|P2_BYTE){
		THROW(EXCEP_WRONG_P1_P2);
	}

	/*Check if Pin length is valid*/
	if (PIN_CARD_ISSUER_LENGTH != P3_BYTE) {

		/*Return status words 6710*/
		THROW2(EXCEP_WRONG_P3, PIN_CARD_ISSUER_LENGTH);
	}

	/*Verify security PIN*/
	if (EQUAL != mem_cmp(COMMAND_DATA, CARD_ISSUER_PIN, PIN_CARD_ISSUER_LENGTH)) {

		/*Card Issuer PIN is not validated*/
		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}

	
#ifndef WIN32
	/*It switches to bootloader*/
	if(switchToBootLoader()){
		THROW(EXCEP_TECHNICAL_PROBLEM);
	}
#endif
}