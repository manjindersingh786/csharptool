#include"51_011.h"

/*
* Macro defines
*/
#define SEEK_TYPE_1												(0x00)
#define SEEK_TYPE_2												(0x01)
#define MASK_SEEK_MODE											(0x0F)
#define SEEK_P3_MAX_LENGTH										(0x10)
#define SEEK_FORWARD_FROM_BEGIN									(0x00)
#define SEEK_BACKWARD_FROM_END									(0x01)
#define SEEK_FORWARD_FROM_NEXT_RECORD							(0x02)
#define SEEK_BACKWARD_FROM_PREVIOUS_RECORD						(0x03)
#define MAX_P3_LENGTH_FOR_INCREASE_GSM							(0x03)
/*
* It implements Read/Update Record command of 51.011
*/


void _51011_readUpdateRecord(void) {

	DBG_FUNC_ENTRY(">>[51_011] _51011_readUpdateRecord() \n");

	if (P2_BYTE & MASK_FILE_SELECTION) { //If p2 denotes SFI
		//SFI is not supported in GSM/SIM
		/*Wrong P1-P2*/
		DBG_ERR("[51_011] _51011_readUpdateRecord: Wrong P1-P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_readUpdateRecord() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	_102221_readUpdateRecord();

	DBG_FUNC_EXIT("<<[51_011] _51011_readUpdateRecord() \n");
}

/*
* It implements Read/Update Binary command of 51.011
*/
void _51011_readUpdateBinary(void) {

	DBG_FUNC_ENTRY(">>[51_011] _51011_readUpdateBinary() \n");

	if (P1_BYTE & MASK_BIT(8)) { //If P1 indicates SFI
		/*Wrong P1-P2*/
		DBG_ERR("[51_011] _51011_readUpdateBinary: Wrong P1-P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_readUpdateBinary() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	_102221_readUpdateBinary();

	DBG_FUNC_EXIT("<<[51_011] _51011_readUpdateBinary() \n");
}

/*
* It implements Increase record command of 51.011
*/
void _51011_increaseRecord(void) {

	DBG_FUNC_ENTRY(">>[51_011] _51011_increaseRecord() \n");

	if (P1_BYTE) { //If P1 indicates SFI
		//SFI is not supported in GSM/SIM
		/*Wrong P1-P2*/
		DBG_ERR("[51_011] _51011_increaseRecord: Wrong P1-P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_increaseRecord() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}
	else if (P3_BYTE != MAX_P3_LENGTH_FOR_INCREASE_GSM) { //if p3 is not 03 then 
		//p3=03 is mendatory for GSM, another differents value is not supported in GSM
		/*Wrong P3*/
		DBG_ERR("[51_011] _51011_increaseRecord: Wrong p3 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_increaseRecord() \n");
		THROW2(EXCEP_WRONG_P3, MAX_P3_LENGTH_FOR_INCREASE_GSM);
	}
	else {
		_102221_increase();
	}
	
	

	DBG_FUNC_EXIT("<<[51_011] _51011_increaseRecord() \n");
}

/*
* It implements Invalidate Rehabilitate File command of 51.011
*/
void _51011_invalidateRehabilitate(void) {

	DBG_FUNC_ENTRY(">>[51_011] _51011_invalidateRehabilitate() \n");

	if (P1_BYTE) { //If P1 indicates Path
	
	/*Selection by path is not supported in GSM/SIM*/
	/*Wrong P1-P2*/
		DBG_ERR("[51_011] _51011_invalidateRehabilitate: Wrong P1-P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_invalidateRehabilitate() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	_102221_deactivateActivateFile();

	DBG_FUNC_EXIT("<<[51_011] _51011_invalidateRehabilitate() \n");
}
/*
* It implements Seek command of 51.011
*/
void _51011_Seek(void) {

	uMonoByte		umbType;
	uMonoByte		umbSearchMode;
	uMonoByte		umbRecordNo;
	uMonoByte		umbPattern[16];
	
	DBG_FUNC_ENTRY(">>[51_011] _51011_Seek() \n");

	umbType = SHIFT_RIGHT_BY_N_BITS(P2_BYTE, 4); // Get type of seek command i.e. Higher nibble of p2

	umbSearchMode = P2_BYTE & MASK_SEEK_MODE;

	if ((0x00 != P1_BYTE) //non-zero P1
		||					//OR
		(umbType > SEEK_TYPE_2) // Neither Type 1 nor Type 2
		||						//OR
		(umbSearchMode > SEEK_BACKWARD_FROM_PREVIOUS_RECORD)) {  //none of the defined seek modes

		/*Wrong P1-P2*/
		DBG_ERR("[51_011] _51011_Seek: Wrong P1-P2 \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_Seek() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE > SEEK_P3_MAX_LENGTH) {
		
		/*Wrong P3*/
		DBG_ERR("[51_011] _51011_Seek: Wrong P3 must be 1-16 bytes \n");
		DBG_FUNC_EXIT("<<[51_011] _51011_Seek() \n");
		THROW2(EXCEP_WRONG_P3, 0x00);
	}

	if (NO_FILE_PRESENT == pccCurrentChannel->fhaCurrentEf) {

		/*Command not allowed, as no EF selected*/
		DBG_ERR("[51_011] _51011_Seek: Command not allowed, as no EF selected\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_Seek() \n");
		THROW(EXCEP_NO_FILE_SELECTED);
	}

	if (!(IS(LF, _7816_4_getFileDescriptor(pccCurrentChannel->fhaCurrentEf)))) { // if file is not linear fixed

		/*throw Error as target file is not linear fixed*/
		DBG_ERR("[51_011] _51011_Seek: File is not linear fixed\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_Seek() \n");
		THROW(EXCEP_INVALID_FILE_TYPE);
	}

	/*Checking the security condition of the file*/
	if (ACC_NOT_VERIFIED == _102221_verifyAccessCondition(ACC_MODE_FILE_READ, pccCurrentChannel->fhaCurrentEf)) {
		DBG_ERR("[102_221] _102221_seek: Security conditions not satisfied \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_seek() \n");
		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}


	if (LCSI_DEACTIVATED == _7816_4_getLifeCycleStatus(pccCurrentChannel->fhaCurrentEf)) { // check if its deactivated

		DBG_WARN("[102_221] _51011_Seek: File is deactivated\n");
		DBG_FUNC_EXIT("<<[102_221] _51011_Seek() \n");
		THROW(EXCEP_REFERENCED_DATA_INVALIDATED);
	}


	/*Copy the patter in local array*/
	mem_cpy(umbPattern, COMMAND_DATA, P3_BYTE);

	/*get total no. of records of the file*/
	umbRecordNo = _7816_4_getNoOfRecords(pccCurrentChannel->fhaCurrentEf);

	if (SEEK_FORWARD_FROM_BEGIN == umbSearchMode) { // forward search from begin

		DBG_FUNC("[51_011] _51011_Seek: Forward search from beginning \n");

		/*Searching from first record in forward direction*/
		umbSearchMode = SEARCH_RECORD_FORWARD;
		umbRecordNo = RECORD_FIRST;
	}
	else if (SEEK_BACKWARD_FROM_END == umbSearchMode) { // backward searching from end

		DBG_FUNC("[51_011] _51011_Seek: Backward search from ending \n");

		/*Searching from last record in backward direction*/
		umbSearchMode = SEARCH_RECORD_BACKWARD;
	}
	else if (SEEK_FORWARD_FROM_NEXT_RECORD == umbSearchMode) { //searching forward from next record
		
		DBG_FUNC("[51_011] _51011_Seek: Searching forward from next record \n");

		/*Searching in forward direction*/
		umbSearchMode = SEARCH_RECORD_FORWARD;

		if (RECORD_UNDEFINED == pccCurrentChannel->umbCurrentRecPtr) { // if record pointer is not set, in case file is just selected.

			/*Searching will start from first record*/
			umbRecordNo = RECORD_FIRST;
		}
		else if (pccCurrentChannel->umbCurrentRecPtr < umbRecordNo) { // if record pointer is less than last record no.

			/*Searching will start from next record*/
			umbRecordNo = pccCurrentChannel->umbCurrentRecPtr + 1;
		}
		else {
			/*record pointer is already set to last record*/
			DBG_WARN("[51_011] _51011_Seek: Record pointer is already at last record no.\n");
			DBG_FUNC_EXIT("<<[51_011] _51011_Seek() \n");
			THROW(EXCEP_NORMAL_ENDING);
		}
	}
	else { //searching backward from previous record

		DBG_FUNC("[51_011] _51011_Seek: Searching backward from previous record\n");

		/*Searching in backward direction*/
		umbSearchMode = SEARCH_RECORD_BACKWARD;

		if (RECORD_FIRST == pccCurrentChannel->umbCurrentRecPtr) { // if record pointer is set on first record
			
			/*record pointer is already set to first record*/
			DBG_WARN("[51_011] _51011_Seek: Record pointer is already at first record \n");
			DBG_FUNC_EXIT("<<[51_011] _51011_Seek() \n");
			THROW(EXCEP_NORMAL_ENDING);
		}
		/*Two possibilities left either record pointer is undefined or more than 1*/
		else if (RECORD_UNDEFINED != pccCurrentChannel->umbCurrentRecPtr) { // if record pointer is not undefined, it means record ptr is more than 1

			/*Searching will start from previous record*/
			umbRecordNo = pccCurrentChannel->umbCurrentRecPtr - 1;
		}

		/*If record pointer is undefined, searching will start from last record. umbRecordNo already stores last record no. */
	}

	DBG_FUNC("[51_011] _51011_Seek: Searching from record %1\n", umbRecordNo);

	/*search the currently selected file as command instructed*/
	umbRecordNo = _7816_4_SearchRecordFile(pccCurrentChannel->fhaCurrentEf, umbPattern, P3_BYTE, (tSearchMode) umbSearchMode, SEARCH_FIRST_OCCURRENCE,  umbRecordNo, OFFSET_START, COMMAND_RESPONSE_DATA);

	if (umbRecordNo) { // if records are found in searching

		/*set current record pointer to first record found*/
		pccCurrentChannel->umbCurrentRecPtr = COMMAND_RESPONSE_DATA[0x00];

		if (SEEK_TYPE_2 == umbType) { // if command is type 2

			/*set response length*/
			gudbOutputLen = 0x01;
		}
	}
	else { //Pattern not found
		THROW(EXCEP_PATTERN_NOT_FOUND);
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_Seek() \n");
}