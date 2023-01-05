#include "102_221.h"

/*
* Macro defines
*/
#define MAX_CMD_LENGTH_FOR_INCREASE										(0x7f)

#define SEARCH_INDEX_OF_OFFSET_OR_VALUE									(0x01)
#define SEARCH_INDEX_OF_INDICATION										(0x00)
#define LENGTH_SEARCH_INDICATION										(0x02)

/*
* It verifies the security condition for given operation on given file
* 
* @param amOperationMode: Read/Update/Increase/Invalidate/Deactivate 
* @param fhaFile: File header address of target file
*/
 tAccessResult  _102221_verifyAccessCondition(IN_ARG tAccessMode amOperation, IN_ARG tFileHeaderAddr fhaFile) {

	uMonoByte		umbStatus = ACC_NOT_VERIFIED;
	DBG_FUNC_ENTRY(">>[102_221] _102221_verifyAccessCondition()\n");

	if (STATE_SECURED > _7186_4_getCardState()) { // If card is in Production state: OP_READY and INITIALIZED

		/*Card is in personalization state*/
		DBG_FUNC("[102_221] _102221_verifyAccessCondition: Card in Production state\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_verifyAccessCondition() returns ACC_VERIFIED %1\n", ACC_VERIFIED);
		return ACC_VERIFIED;
	}
	
	DBG_FUNC("[102_221] _102221_verifyAccessCondition: Card in Secured state\n");

	DBG_FUNC("[102_221] _102221_verifyAccessCondition: Extract security condition\n");

	/*get security condition i.e. PIN_** */
	umbStatus = _7816_4_getSecurityCondition(fhaFile, amOperation);

	if (PIN_NEVER != umbStatus) { // if security condition is not NEVER
		umbStatus = _7816_4_isKeyRefVerified(umbStatus);
	}
	else { // if security condition is never
		DBG_FUNC("[102_221] _102221_verifyAccessCondition: Security condition is never\n");
		umbStatus = ACC_NOT_VERIFIED;
	}

	DBG_FUNC_EXIT("<<[102_221] _102221_verifyAccessCondition() returns %1\n", umbStatus);
	return umbStatus;
}
/*
* It implements the UICC Read Binary and Update Binary command as per ETSI 102 221
* 
*/
void _102221_readUpdateBinary(void) {

	uMonoByte		umbSfi;
	tAccessMode		amOperation = ACC_MODE_FILE_READ;
	tFileHeaderAddr	fhaFile = NO_FILE_PRESENT;
	uDiByte			udbOffset;
	uDiByte			udbRequestedSize = P3_BYTE;
	uDiByte			udbFileSize = 0x00;

	DBG_FUNC_ENTRY(">>[102_221] _102221_readUpdateBinary() \n");

	DBG_FUNC("[102_221] _102221_readUpdateBinary: P1-P2 validation \n");

	/*P1-P2 validation*/
	if (P1_VALID_OFFSET == (P1_BYTE & MASK_BIT(8))){
		 
		DBG_FUNC("[102_221] _102221_readUpdateBinary: P1-P2 both work as offset\n");

		/*P1+p2 work as offset*/
		udbOffset = mm_makeUdiByte(&P1_BYTE, 0x00);

		/*use current file for intended operation*/
		fhaFile = pccCurrentChannel->fhaCurrentEf;

		DBG_FUNC("[102_221] _102221_readUpdateBinary: Check if File selected or not\n");
	}
	else if (P1_VALID_SFI == (P1_BYTE & MASK_VALID_SFI)) {
		
		DBG_FUNC("[102_221] _102221_readUpdateBinary: P1 is SFI and P2 work as offset\n");

		/*p1 contains SFI*/
		umbSfi = P1_BYTE & MASK_SFI;

		DBG_FUNC("[102_221] _102221_readUpdateBinary: Check if SFI exists or not\n");

		/*search for given SFI under current directory*/
		_7816_4_ifFileExists(umbSfi, pccCurrentChannel->fhaCurrentDf, MODE_FIND_SFID, &fhaFile);

		/*P2 only contains offset*/
		udbOffset = P2_BYTE;

	}
	else { // Incorrect P1 value

		/*Invalid P1-p2*/
		DBG_ERR("[102_221] _102221_readUpdateBinary: Invalid P1 value '%1'\n", P1_BYTE);
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");
		THROW(EXCEP_INCORRECT_DATAFIELD);
	}

	if (NO_FILE_PRESENT == fhaFile) { // if no file found with matched SFI

		/*Command not allowed, as no EF selected*/
		DBG_ERR("[102_221] _102221_readUpdateBinary: No file exists or selected\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");
		THROW(EXCEP_NO_FILE_SELECTED);
	}

	if (!(IS(TF, _7816_4_getFileDescriptor(fhaFile)))) // if its not transparent file
	{
		/*Invalid type of file, it must be transparent*/
		DBG_ERR("[102_221] _102221_readUpdateBinary: Invalid file type: File is record structured\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");
		THROW(EXCEP_INVALID_FILE_TYPE);
	}

	if (LCSI_DEACTIVATED == _7816_4_getLifeCycleStatus(fhaFile)) { // check if its deactivated

		DBG_WARN("[102_221] _102221_readUpdateBinary: File is deactivated\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");
		THROW(EXCEP_REFERENCED_DATA_INVALIDATED);
	}

	if (INS_UPDATE_BINARY == INS_BYTE) { // check if its Update Binary command

		DBG_FUNC("[102_221] _102221_readUpdateBinary: Its update operation\n");
		/*Mode will be Update*/
		amOperation = ACC_MODE_FILE_UPDATE;
	}
	else {
		DBG_FUNC("[102_221] _102221_readUpdateBinary: Its Read operation\n");
	}

	/*Get total size of file*/
	udbFileSize = _7816_4_getFileSize(fhaFile);

	if (0x00 == P3_BYTE)  { //If p3 is zero
		
		DBG_FUNC("[102_221] _102221_readUpdateBinary: P3 is '00' hence it will be considered as 256 bytes\n");

		/*If control is here it means command received is Read Binary, because Update Command on p3 00
		* has already been handled in command router*/
		/* Max response length in case p3 = 00 is 256 bytes as per ETSI 102221 Rel 15 Clause 10.1.6	*/
		udbRequestedSize = MAX_RESPONSE_LEN;

		if (SRC_RFM == gumbcommandSource) { //when source is RFM

			DBG_FUNC("[102_221] _102221_readUpdateBinary: Command source is RFM, Send as much response as it has in file (or max 256 bytes)\n");

			if (((uDiByte)MAX_RESPONSE_LEN) > (udbFileSize - udbOffset)) { /*if total response is smaller than 256, go for file size*/

				udbRequestedSize = (udbFileSize - udbOffset); /*storing all the response*/
			}
		}
	}

	if (udbOffset >= udbFileSize) { //wrong parameter if the offset size is greater then the filesize
		/*Invalid offset*/
		DBG_ERR("[102_221] _102221_readUpdateBinary: Offset cannot be more than the file data limit/boundaries\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");
		THROW(EXCEP_WRONG_P1_P2); 
	}

	/*Check if offset and requested size together are beyond file data*/
	if ((udbOffset + udbRequestedSize) > udbFileSize) {

		/*Invalid offset or P3*/
		DBG_ERR("[102_221] _102221_readUpdateBinary: Offset and Requested length might be beyond the File data limit/boundaries\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");
		THROW2(EXCEP_WRONG_P3, (udbFileSize - udbOffset) );
	}
	
	DBG_FUNC("[102_221] _102221_readUpdateBinary: Checking access condition of File for given operation\n");

	/*Check security condition*/
	if (ACC_NOT_VERIFIED == _102221_verifyAccessCondition(amOperation, fhaFile))
	{
		/*Security Condition not satisfied*/
		DBG_ERR("[102_221] _102221_readUpdateBinary: Security conditions not satisfied\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");
		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}

	/*Read/update file*/
	_7816_4_readUpdateFile(fhaFile, COMMAND_RESPONSE_DATA, udbOffset, udbRequestedSize, amOperation);

	if (INS_READ_BINARY == INS_BYTE) { // check if its Read Binary command
		
		DBG_FUNC("[102_221] _102221_readUpdateBinary: Setting response lenght '%2'\n", udbRequestedSize);

		/*Set response length*/
		gudbOutputLen = udbRequestedSize;
	}

	DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateBinary() \n");


}

/*
* It implements the UICC Read Record and Update Record command as per ETSI 102 221
*
*/
void _102221_readUpdateRecord(void) {

	uMonoByte		umbMode = SHIFT_RIGHT_BY_N_BITS((P2_BYTE & MASK_FILE_SELECTION),0x03);
	uMonoByte		umbRecLength;
	uMonoByte		umbTemp;
	uMonoByte		umbNoOfRecords;
	uMonoByte		umbRecordNo;
	tAccessMode		amMode = ACC_MODE_FILE_READ;
	tFileHeaderAddr fhaFile;
	
	DBG_FUNC_ENTRY(">>[102_221] _102221_readUpdateRecord() \n");

	DBG_FUNC("[102_221] _102221_readUpdateRecord: Checking file selection mode from P2\n");

	if (0x00 == umbMode) { // Currently selected EF

		DBG_FUNC("[102_221] _102221_readUpdateRecord: Command targetting currrently selected file\n");

		DBG_FUNC("[102_221] _102221_readUpdateRecord: Check if EF selected or not\n");
		/*Set current file as target file*/
		fhaFile = pccCurrentChannel->fhaCurrentEf;
	}
	else if (umbMode < 0x1F) {// range of SFI 01-30

		DBG_FUNC("[102_221] _102221_readUpdateRecord: Command targetting SFI present in P2\n");

		DBG_FUNC("[102_221] _102221_readUpdateRecord: Checking if SFI exists or not\n");

		/*search for given SFI under current directory*/
		_7816_4_ifFileExists(umbMode, pccCurrentChannel->fhaCurrentDf, MODE_FIND_SFID, &fhaFile);

	}
	else { //invalid P2 value

		DBG_ERR("[102_221] _102221_readUpdateRecord: Invalid P2 '%1'\n", P2_BYTE);
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (NO_FILE_PRESENT == fhaFile) { //If no file is there

		/*No file selected*/
		DBG_ERR("[102_221] _102221_readUpdateRecord: No EF selected\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW(EXCEP_NO_FILE_SELECTED);
	}

	DBG_FUNC("[102_221] _102221_readUpdateRecord: Checking Mode of operation \n");

	/*Read/update operation mode*/
	umbMode = P2_BYTE & MASK_OP_MODE;

	if ((MODE_NEXT_RECORD > umbMode) || (umbMode > MODE_ABSOLUTE_RECORD)) { // invalid mode

		/*Wrong p1 p2*/
		DBG_ERR("[102_221] _102221_readUpdateRecord: Invalid P2 '%1'\n", P2_BYTE);
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	/*store file descriptor byte*/
	umbTemp = _7816_4_getFileDescriptor(fhaFile);

	DBG_FUNC("[102_221] _102221_readUpdateRecord: Checking EF type\n");

	if (IS(TF, umbTemp)) { // If file is Transparent, not Linear or Cyclic 

		/*Invalid file type*/
		DBG_ERR("[102_221] _102221_readUpdateRecord: Invalid File type, it is Transparent\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW(EXCEP_INVALID_FILE_TYPE);
	}

	if (LCSI_DEACTIVATED == _7816_4_getLifeCycleStatus(fhaFile)) { // check if its deactivated

		DBG_WARN("[102_221] _102221_readUpdateRecord: File is deactivated\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW(EXCEP_REFERENCED_DATA_INVALIDATED);
	}
	
	DBG_FUNC("[102_221] _102221_readUpdateRecord: Its record file\n");

	/*Save if its Cyclic*/
	umbTemp = IS(CF, umbTemp);

	DBG_FUNC("[102_221] _102221_readUpdateRecord: Checking P3\n");

	/*get record length*/
	umbRecLength = _7816_4_getRecordLength(fhaFile);

	if ((SRC_RFM == gumbcommandSource)//when source is RFM
		&& //AND
		(0x00 == P3_BYTE)) {  // Le(p3) is 00

		DBG_FUNC("[102_221] _102221_readUpdateRecord: Command source is RFM and P3 is 00, so whole record will be returned\n");

		P3_BYTE = umbRecLength; // Record length will be all RFM need
	}

	if (P3_BYTE != umbRecLength) { // if p3 is not equals to File's record length

		/*Invalid P3*/
		DBG_ERR("[102_221] _102221_readUpdateRecord: P3 '%1' is not equal to Record length '%1'\n", P3_BYTE, umbRecLength);
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW2(EXCEP_WRONG_P3, umbRecLength);
	}

	if (INS_BYTE == INS_UPDATE_RECORD) { // If command is Update Record

		/*Access operation is Update*/
		amMode = ACC_MODE_FILE_UPDATE;

		DBG_FUNC("[102_221] _102221_readUpdateRecord: Its update operation\n");
	}
	else {
		DBG_FUNC("[102_221] _102221_readUpdateRecord: Its Read operation\n");
	}

	if(	(umbTemp)							// Cyclic file
		&&									// AND
		(ACC_MODE_FILE_UPDATE == amMode)	// Update Record
		&&									// AND
		(MODE_PREVIOUS_RECORD != umbMode)){ //If its not Previous mode
		
		/*Cyclic file can only be updated in previous mode*/
		DBG_ERR("[102_221] _102221_readUpdateRecord: Cyclic file can only be updated in Previous mode\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	DBG_FUNC("[102_221] _102221_readUpdateRecord: Check security conditions\n");

	/*Check security condition*/
	if (ACC_NOT_VERIFIED == _102221_verifyAccessCondition(amMode, fhaFile))
	{
		/*Security Condition not satisfied*/
		DBG_ERR("[102_221] _102221_readUpdateRecord: Security Condition not satisfied\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}

	/*Get No. of records*/
	umbNoOfRecords = _7816_4_getNoOfRecords(fhaFile);

	if (MODE_ABSOLUTE_RECORD == umbMode) { // Read/update absolute/current record

		if (MODE_CURRENT_RECORD == P1_BYTE) { // Read/update current record

			DBG_FUNC("[102_221] _102221_readUpdateRecord: Mode is Current record\n");

			if (pccCurrentChannel->umbCurrentRecPtr == RECORD_UNDEFINED) { // Record pointer is undefined
				
				DBG_ERR("[102_221] _102221_readUpdateRecord: Record cannot be undefined in current mode\n");
				DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");

				THROW(EXCEP_RECORD_NOT_FOUND);
			}

			/*Store record no*/
			umbRecordNo = pccCurrentChannel->umbCurrentRecPtr;

		}
		else if(umbNoOfRecords < P1_BYTE) { // record no. in command is incorrect

			DBG_ERR("[102_221] _102221_readUpdateRecord: Record no. '%1' is more than total no. of records '%1'\n", P1_BYTE, umbNoOfRecords);
			DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
			THROW(EXCEP_RECORD_NOT_FOUND);
		}
		else {
			DBG_FUNC("[102_221] _102221_readUpdateRecord: Mode is Absolute record\n");

			/*Store record no*/
			umbRecordNo = P1_BYTE;
		}

		/*Read/Update record*/
		_7816_4_readUpdateRecordFile(fhaFile, COMMAND_RESPONSE_DATA, umbRecordNo, amMode);
	}
	else { // Next/previous
		if (MODE_NEXT_RECORD == umbMode) { // Read/Update  Next record

			DBG_FUNC("[102_221] _102221_readUpdateRecord: Mode is Next record\n");

			if (pccCurrentChannel->umbCurrentRecPtr == RECORD_UNDEFINED) { // Record is undefined
				/*Record no.*/
				DBG_FUNC("[102_221] _102221_readUpdateRecord: Record undefined, so operation will be done on first record\n");
				umbRecordNo = RECORD_FIRST;
			}
			else if (pccCurrentChannel->umbCurrentRecPtr == umbNoOfRecords) {

				if (umbTemp) { //Cyclic file

					DBG_FUNC("[102_221] _102221_readUpdateRecord: Cyclic File: Record pointer is set on Last record, so operation will be done on first record\n");
					/*in cyclic file last record logically connected to first record, hence next record is first*/
					umbRecordNo = RECORD_FIRST;
				}
				else {// Linear file
					DBG_ERR("[102_221] _102221_readUpdateRecord: Linear fixed: It cannot proceed to next record, because Record pointer is at the last record\n");
					DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
					THROW(EXCEP_RECORD_NOT_FOUND);
				}
			}
			else {
				/*Increment Record no*/
				umbRecordNo = pccCurrentChannel->umbCurrentRecPtr + 1;
				DBG_FUNC("[102_221] _102221_readUpdateRecord: Next record to current record i.e. '%1'\n", umbRecordNo);
			}

		}
		else { //Read/update previous record

			DBG_FUNC("[102_221] _102221_readUpdateRecord: Mode is Previous record\n");

			if (pccCurrentChannel->umbCurrentRecPtr == RECORD_UNDEFINED) {

				DBG_FUNC("[102_221] _102221_readUpdateRecord: Record is undefined, so operation will be done Last record\n");
				/*Set last Record no.*/
				umbRecordNo = umbNoOfRecords;
			}
			else if (pccCurrentChannel->umbCurrentRecPtr == RECORD_FIRST) {

				if (umbTemp) {//cyclic

					DBG_FUNC("[102_221] _102221_readUpdateRecord: Cyclic File: Record pointer is at first record, so operation will be done on Last record\n");
					/*in cyclic file first record logically connected to last record, hence previous record is last*/
					umbRecordNo = umbNoOfRecords;
				}
				else { //Linear
					DBG_ERR("[102_221] _102221_readUpdateRecord: Linear Fixed: It cannot proceed to previous record, because Record pointer is at the first record\n");
					DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
					THROW(EXCEP_RECORD_NOT_FOUND);
				}
			}
			else {

				/*Decrement Record no*/
				umbRecordNo = pccCurrentChannel->umbCurrentRecPtr - 1;

				DBG_FUNC("[102_221] _102221_readUpdateRecord: Previous record to current one i.e. '%1'\n", umbRecordNo);
			}
		}

		/*Read/update file and save last*/
		pccCurrentChannel->umbCurrentRecPtr = _7816_4_readUpdateRecordFile(fhaFile, COMMAND_RESPONSE_DATA, umbRecordNo, amMode);

	}

	if (ACC_MODE_FILE_READ == amMode) { //Check if its read operation

		DBG_FUNC("[102_221] _102221_readUpdateRecord: Set Luicc i.e. Prepared response length '%1'\n", umbRecLength);
		/*set luicc i.e. prepared response length*/
		gudbOutputLen = umbRecLength;
	}


	DBG_FUNC_EXIT("<<[102_221] _102221_readUpdateRecord() \n");
}

/*
* It implements the UICC Deactivate and Activate command as per ETSI 102 221
*
*/
void _102221_deactivateActivateFile(void) {

	tFileHeaderAddr fhaFile;
	tFileLCSI		flState;
	tAccessMode		amMode;
	
	
	DBG_FUNC_ENTRY(">>[102_221] _102221_deactivateActivateFile() \n");

	/*Validation of P2*/
	if (P2_BYTE != 0x00)
	{
		/*P2 must be 00*/
		DBG_ERR("[102_221] _102221_deactivateActivateFile: Invalid value of P2 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	/*Validation of P1*/
	if (P1_SELECT_BY_FILE_ID == P1_BYTE)
	{

		if (LENGTH_FILE_ID == P3_BYTE) {

			DBG_FUNC("[102_221] _102221_deactivateActivateFile: Target file is in Command data\n");
			
			/*Search File ID give in command data*/
			_7816_4_ifFileExists(mm_makeUdiByte(COMMAND_DATA, 0x00), pccCurrentChannel->fhaCurrentDf, MODE_FIND_FID, &fhaFile);
		}
		else if (0x00 == P3_BYTE) {

			DBG_FUNC("[102_221] _102221_deactivateActivateFile: Target file is Currently selected\n");
			fhaFile = pccCurrentChannel->fhaCurrentEf;

			if (NO_FILE_PRESENT == fhaFile) { // no file
											  /*EF needs to be selected*/
				DBG_ERR("[102_221] _102221_deactivateActivateFile: EF not selected \n");
				DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
				THROW(EXCEP_NO_FILE_SELECTED);
			}
		}
		else { // P3 other than 00 or 02
			
			/*Wrong P3*/
			DBG_ERR("[102_221] _102221_deactivateActivateFile: Invalid P3\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
			THROW(EXCEP_WRONG_P3);
		}
		
	}
	/*Validation of P1*/
	/*if P1 is 08 or 09*/
	else if (	(P1_BYTE == P1_SELECT_BY_PATH_MF)
				||
				(P1_BYTE == P1_SELECT_BY_PATH_CURRENT_DF))
	{
		/*P3 byte would not be 00 in case of select by path*/

		if ((0x00 == P3_BYTE)
			||
			(0x00 != (P3_BYTE % LENGTH_FILE_ID)))
		{
			DBG_ERR("[102_221] _102221_deactivateActivateFile: Invalid P3\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
			THROW(EXCEP_WRONG_P3);
		}

		DBG_FUNC("[102_221] _102221_deactivateActivateFile: Valid path length\n");
		//Try to Select File Id, Select By path
		if (P1_BYTE == P1_SELECT_BY_PATH_MF)
		{
			DBG_FUNC("[102_221] _102221_deactivateActivateFile: Selecting requested file from MF 3F00\n");
			/*Current DF would be MF*/
			fhaFile = _102221_selectByPath(_7816_4_getMfAddress(), COMMAND_DATA, P3_BYTE);
		}
		else
		{
			DBG_FUNC("[102_221] _102221_deactivateActivateFile: Selecting requested file from currently selected DF\n");
			/* Select the fileID from currently selected DF*/
			fhaFile = _102221_selectByPath(pccCurrentChannel->fhaCurrentDf, COMMAND_DATA, P3_BYTE);
		}


	}
	if (NO_FILE_PRESENT == fhaFile) { // no file
									  /*File doesnt exist under current DF*/
		DBG_ERR("[102_221] _102221_deactivateActivateFile: File not found \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
		THROW(EXCEP_FILE_NOT_FOUND);
	}
	//check if the file is DF//
	/*if MF or DF is selected for activation or deactivation Send errors*/
	 else if (IS(DF, _7816_4_getFileDescriptor(fhaFile))) 
	 { 
		DBG_ERR("[102_221] _102221_deactivateActivateFile: EF needs to be selected/targeted to Deactivate or Activate \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
		THROW(EXCEP_NO_FILE_SELECTED);
	}
	

	if (INS_BYTE == INS_DEACTIVATE_FILE) { // Deactivate file command

		DBG_FUNC("[102_221] _102221_deactivateActivateFile: Command for deactivating the file\n");

		/*Store Deactivate related info. for further operation*/
		flState = LCSI_DEACTIVATED;
		amMode = ACC_MODE_FILE_DEACTIVATE;

		if (LCSI_DEACTIVATED == _7816_4_getLifeCycleStatus(fhaFile)) { // check if its already deactivated

			DBG_WARN("[102_221] _102221_deactivateActivateFile: File is already in deactivated state\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
			THROW(EXCEP_FILE_ALREADY_DEACTIVATED); 
		}
	}
	else { //Activate file command

		DBG_FUNC("[102_221] _102221_deactivateActivateFile: Command for Activating the file\n");

		/*Store Activate related info. for further operation*/
		flState = LCSI_ACTIVATED;
		amMode = ACC_MODE_FILE_ACTIVATE;
	}

	/*Check for security conditions*/
	if (ACC_NOT_VERIFIED == _102221_verifyAccessCondition(amMode,fhaFile)) {
		/*Security Condition not satisfied*/
		DBG_ERR("[102_221] _102221_deactivateActivateFile: Security Condition not satisfied\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
		THROW(EXCEP_SECURITY_NOT_SATISFIED); // Security conditions not verified
	}

	/*Set life cycle status*/
	_7816_4_setLifeCycleStatus(fhaFile, flState);

	/*set file as currently selected*/
	pccCurrentChannel->fhaCurrentEf = fhaFile;

	/*Set selected file's parent as current DF*/
	pccCurrentChannel->fhaCurrentDf = _7816_4_getParentFile(fhaFile);

	/*Reset the record pointer*/
	pccCurrentChannel->umbCurrentRecPtr = RECORD_UNDEFINED;
	
	DBG_FUNC_EXIT("<<[102_221] _102221_deactivateActivateFile() \n");
}
/*
* It implements the Increase command as per ETSI 102 221
*
*/
void _102221_increase(void) {

	uMonoByte			umbSfi = P1_BYTE & MASK_SFI;
	uMonoByte			umbRecLength;
	uMonoByte			umbIndex;
	uMonoByte			umbMinValue;
	uDiByte				udbResult = 0;
	tFileHeaderAddr		fhaFile;

	DBG_FUNC_ENTRY(">>[102_221] _102221_increase() \n");

	if (0x00 != P2_BYTE) { // invalid P2 value

		DBG_ERR("[102_221] _102221_increase: Invalid P1\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE > MAX_CMD_LENGTH_FOR_INCREASE) { // check maximum allowed value of p3

		DBG_ERR("[102_221] _102221_increase: 00 < P3 < 128 \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
		THROW2(EXCEP_WRONG_P3, 0x00);
	}
	if (0x00 == P1_BYTE) { // Currently selected EF

		DBG_FUNC("[102_221] _102221_increase: Command targetting currrently selected file\n");

		DBG_FUNC("[102_221] _102221_increase: Check if EF selected or not\n");

		/*Set current file as target file*/
		fhaFile = pccCurrentChannel->fhaCurrentEf;
	}
	else if (P1_VALID_SFI == (P1_BYTE & MASK_VALID_SFI)) { //If its for SFI

		DBG_FUNC("[102_221] _102221_increase: Command targetting SFI present in P1\n");

		DBG_FUNC("[102_221] _102221_increase: Checking if SFI exists or not\n");

		/*search for given SFI under current directory*/
		_7816_4_ifFileExists((P1_BYTE & MASK_SFI), pccCurrentChannel->fhaCurrentDf, MODE_FIND_SFID, &fhaFile);

	}
	else { // invalid P1 value RFU values
		DBG_ERR("[102_221] _102221_increase: Invalid P1\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (NO_FILE_PRESENT == fhaFile) { // if no file found neither Currently selected nor SFI 

		/*Command not allowed, as no EF selected*/
		DBG_ERR("[102_221] _102221_increase: No file exists or selected\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
		THROW(EXCEP_NO_FILE_SELECTED);
	}

	/*Get record length of file*/
	umbRecLength = _7816_4_getRecordLength(fhaFile);

	if (!IS(CF, _7816_4_getFileDescriptor(fhaFile)) // if its not Cyclic file 
		||							// OR
		(umbRecLength > 0x7F)) {  //Record length is restricted to 127 bytes

		/*Command is allowed only on cyclic file*/
		DBG_ERR("[102_221] _102221_increase: File is not Cyclic\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
		THROW(EXCEP_INVALID_FILE_TYPE);
	}

	if (LCSI_DEACTIVATED == _7816_4_getLifeCycleStatus(fhaFile)) { // check if its deactivated

		DBG_WARN("[102_221] _102221_increase: File is deactivated\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
		THROW(EXCEP_REFERENCED_DATA_INVALIDATED);
	}

	if (ACC_NOT_VERIFIED == _102221_verifyAccessCondition(ACC_MODE_FILE_INCREASE, fhaFile)) { // check if security conditions are verified

		/*Command is allowed only on cyclic file*/
		DBG_ERR("[102_221] _102221_increase: Security conditions not satisfied\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}

	if (P3_BYTE > umbRecLength) { // if command data is more than Record length

		/*If length of incoming command data i.e. value to add is more than record length
		* Now we have two possibilities:
		* 1. All preceding bytes of Command data are zero which leads to valid result value
		* 2. One of the preceding bytes of command data is non zero it definitely leads to more than max value
		*/
		for (umbIndex = 0x00; umbIndex < (P3_BYTE - umbRecLength); umbIndex++) {

			/*if there is any byte non-zero*/
			if (COMMAND_DATA[umbIndex] != 0x00) {

				DBG_ERR("[102_221] _102221_increase: Result value will exceed the maximum value of record\n");
				DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");

				THROW(EXCEP_MAX_VALUE_REACHED);
			}
		}

		/*Condition of adding incoming data values with record*/
		umbMinValue = 0x00;
	}
	else {

		/*Condition of adding incoming data values with record*/
		umbMinValue = umbRecLength - P3_BYTE;
	}

	DBG_FUNC("[102_221] _102221_increase: Value to add:\n");

	DBG_PRINT_ARRAY(COMMAND_RESPONSE_DATA, P3_BYTE);

	/*Move command data after length "umbRecLength"*/
	mem_cpyBwd(COMMAND_RESPONSE_DATA + umbRecLength, COMMAND_DATA, P3_BYTE);

	/*Reading the Record in Response array, storing it from beginning*/
	_7816_4_readUpdateRecordFile(fhaFile, COMMAND_RESPONSE_DATA, RECORD_FIRST, ACC_MODE_FILE_READ);

	DBG_FUNC("[102_221] _102221_increase: Last updated/increased Record Data before adding:\n");

	DBG_PRINT_ARRAY(COMMAND_RESPONSE_DATA, umbRecLength);

	/*Arithmetic addition of incoming data on oldest record*/
	for (umbIndex = umbRecLength - 0x01; umbIndex != 0xFF; umbIndex--) {

		if (umbIndex >= umbMinValue) {

			/*add corresponding elements of Record and incoming data*/
			udbResult += COMMAND_RESPONSE_DATA[umbIndex] + COMMAND_RESPONSE_DATA[P3_BYTE + umbIndex];
		}
		else {

			/*just add carry with remaining data of record*/
			udbResult += COMMAND_RESPONSE_DATA[umbIndex] ;
		}

		/*store result value*/
		COMMAND_RESPONSE_DATA[umbIndex] = (uMonoByte)udbResult;

		/*Store carry (if any)*/
		udbResult = SHIFT_RIGHT_BY_N_BITS(udbResult, 8); 
	}

	if (udbResult) { // if still there is a carry

		/*result value is more than max allowed*/
		DBG_ERR("[102_221] _102221_increase: Result value will exceed the maximum value of record\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");

		THROW(EXCEP_MAX_VALUE_REACHED);
	}
	DBG_FUNC("[102_221] _102221_increase: Result value after increasing the record:\n");

	DBG_PRINT_ARRAY(COMMAND_RESPONSE_DATA, umbRecLength);

	DBG_FUNC("[102_221] _102221_increase: Update the result value of record in Cyclic file\n");

	/*updating the result record data in cyclic file*/
	_7816_4_readUpdateRecordFile(fhaFile, COMMAND_RESPONSE_DATA, RECORD_FIRST, ACC_MODE_FILE_UPDATE);

	/*Response length*/
	gudbOutputLen = P3_BYTE + umbRecLength;

	DBG_FUNC_EXIT("<<[102_221] _102221_increase() \n");
}

/*
* It implements the Search command as per ETSI 102 221
*/
void _102221_search(void) {
	tSearchMode		umbSearchMode;
	uMonoByte		umbMode;
	uMonoByte		umbRecordNo = P1_BYTE; //'record number to start' or '00'
	uMonoByte		umbRecLength;
	uMonoByte		umbNoOfRecords;
	uMonoByte		umbTypeOfEnhanced;
	tFileHeaderAddr fhaFile;

	DBG_FUNC_ENTRY(">>[102_221] _102221_search() \n");

	umbMode = SHIFT_RIGHT_BY_N_BITS((P2_BYTE), 0x03); //first 5 bits of P2

	if (0x00 == umbMode) { // Currently selected EF 

		DBG_FUNC("[102_221] _102221_search: Command targetting currrently selected file\n");

		/*Set current file as target file*/
		fhaFile = pccCurrentChannel->fhaCurrentEf;

		if (NO_FILE_PRESENT == fhaFile) {

			/*Command not allowed, as no EF selected*/
			DBG_ERR("[102_221] _102221_search: Command not allowed, as no EF selected\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

			THROW(EXCEP_NO_FILE_SELECTED);
		}
	}
	else if (umbMode < 0x1F) {// Range of SFI 01-30

		DBG_FUNC("[102_221] _102221_search: Command targetting SFI present in P2\n");

		/*Search for given SFI under current directory*/
		_7816_4_ifFileExists(umbMode, pccCurrentChannel->fhaCurrentDf, MODE_FIND_SFID, &fhaFile);

		if (NO_FILE_PRESENT == fhaFile) {

			/*Command not allowed, as file not found*/
			DBG_ERR("[102_221] _102221_search: Command not allowed, as File not found\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

			THROW(EXCEP_FILE_NOT_FOUND);
		}
	}
	else { //Invalid first 5 bits of P2

		DBG_ERR("[102_221] _102221_search: Invalid P2 '%1'\n", P2_BYTE);
		DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	/*last 3 bits of P2 for mode*/
	umbMode = P2_BYTE & MASK_OP_MODE;

	if ((umbMode != SEARCH_FORWARD_MODE)
		&&
		(umbMode != SEARCH_BACKWARD_MODE)
		&&
		(umbMode != SEARCH_ENHANCED_MODE)) {
		//Invalid last 3 bits of P2

		DBG_ERR("[102_221] _102221_search: Invalid P2 '%1'\n", P2_BYTE);
		DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if ((IS(TF, _7816_4_getFileDescriptor(fhaFile)))) { // if file is not linear fixed or cyclic

		/*Throw Error as target file is not linear fixed or cyclic*/
		DBG_ERR("[102_221] _102221_search: File is neither linear fixed nor cyclic\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

		THROW(EXCEP_INVALID_FILE_TYPE);
	}

	/*Checking the security condition of the file*/
	if (ACC_NOT_VERIFIED == _102221_verifyAccessCondition(ACC_MODE_FILE_READ, fhaFile)) {

		/*throw Error as the secutiry condition of target file is not verified*/
		DBG_ERR("[102_221] _102221_search: Security conditions not satisfied \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}

	/*get total no. of records of the file*/
	umbNoOfRecords = _7816_4_getNoOfRecords(fhaFile);

	/*get the length of records of the file*/
	umbRecLength = _7816_4_getRecordLength(fhaFile);

	/*Copy the pattern in local array*/
	mem_cpy(gumbWorkingBuffer, COMMAND_DATA, P3_BYTE);

	if ((umbMode == SEARCH_FORWARD_MODE) || (umbMode == SEARCH_BACKWARD_MODE)) 
	{
		/*Simple search mode : forward(04) or backward(05)  starting from record indicated in P1*/

		if (MODE_CURRENT_RECORD == P1_BYTE) { //Search current record

			DBG_FUNC("[102_221] _102221_search: Search Mode is Current record\n");

			if (pccCurrentChannel->umbCurrentRecPtr == RECORD_UNDEFINED) { // Record pointer is undefined

				DBG_ERR("[102_221] _102221_search: Record cannot be undefined in current mode\n");
				DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

				THROW(EXCEP_RECORD_NOT_FOUND);
			}
			/*Store record no*/
			umbRecordNo = pccCurrentChannel->umbCurrentRecPtr;
		}
		else if (P1_BYTE > umbNoOfRecords) {

			DBG_FUNC("[102_221] _102221_search: Search Mode is Indication of record number in P1\n");

			/*P1 is not a valid record number for given EF*/
			DBG_ERR("[102_221] _102221_search: Wrong P1, must be an existing record number\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

			THROW(EXCEP_RECORD_NOT_FOUND);
		}

		if (P3_BYTE > umbRecLength) {
			/*Wrong P3*/
			DBG_ERR("[102_221] _102221_search: Wrong P3, must not be greater than record length \n");
			DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

			THROW2(EXCEP_WRONG_P3, 0x00);
		}

		if (umbMode == SEARCH_FORWARD_MODE) {

			/*Simple search with forward mode*/
			DBG_FUNC("[102_221] _102221_search: SIMPLE FORWARD SEARCH \n");
			umbSearchMode = SEARCH_RECORD_FORWARD;
		}
		else {

			/*Simple search with backward mode*/
			DBG_FUNC("[102_221] _102221_search: SIMPLE BACKWARD SEARCH \n");
			umbSearchMode = SEARCH_RECORD_BACKWARD;
		}	

		DBG_FUNC("[102_221] _102221_search: Searching from record %1\n", umbRecordNo);
		umbRecordNo = _7816_4_SearchRecordFile(fhaFile, gumbWorkingBuffer, P3_BYTE, umbSearchMode, SEARCH_ALL_OCCURRENCES, umbRecordNo, OFFSET_START, COMMAND_RESPONSE_DATA);
	
	}	//simple search checks end

	else {
		/*Enhanced search record*/
		/*All Enhanced search modes*/
		DBG_FUNC("[102_221] _102221_search: ENHANCED SEARCH RECORD\n");

		if ((COMMAND_DATA[SEARCH_INDEX_OF_INDICATION] & 0xF4) != 0x04) { //Incorrect input in first byte of search indication

			DBG_ERR("[102_221] _102221_search: Incorrect Data in First Byte of search indication \n");
			DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

			THROW(EXCEP_INCORRECT_DATAFIELD);
		}

		//first 5 bits of search indication's first byte to find type of enhanced search (offset or value)
		umbTypeOfEnhanced = SHIFT_RIGHT_BY_N_BITS(COMMAND_DATA[SEARCH_INDEX_OF_INDICATION], 0x03);

		if ((P3_BYTE <= LENGTH_SEARCH_INDICATION)
			||
			((P3_BYTE - LENGTH_SEARCH_INDICATION)  > umbRecLength) ) {
			/*Wrong P3*/
			DBG_ERR("[102_221] _102221_search: Wrong P3 \n");
			DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

			THROW2(EXCEP_WRONG_P3, 0x00);
		}

		/*Copy the pattern from '3rd byte of command data' in local array*/
		mem_cpy(gumbWorkingBuffer, (COMMAND_DATA + LENGTH_SEARCH_INDICATION), (P3_BYTE - LENGTH_SEARCH_INDICATION));

		/*search mode from last 3 bits of search indication's first byte*/
		umbMode = COMMAND_DATA[SEARCH_INDEX_OF_INDICATION] & MASK_OP_MODE;

		if ((umbMode == SEARCH_FORWARD_MODE) || (umbMode == SEARCH_BACKWARD_MODE)) {

			umbSearchMode = umbMode - SEARCH_FORWARD_MODE;

			if (MODE_CURRENT_RECORD == P1_BYTE) { //Search current record

				DBG_FUNC("[102_221] _102221_search: Search Mode is Current record\n");

				if (pccCurrentChannel->umbCurrentRecPtr == RECORD_UNDEFINED) { // Record pointer is undefined

					DBG_ERR("[102_221] _102221_search: Record cannot be undefined in current mode\n");
					DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

					THROW(EXCEP_RECORD_NOT_FOUND);
				}
				/*Store record no*/
				umbRecordNo = pccCurrentChannel->umbCurrentRecPtr;
			}
			else if (P1_BYTE > umbNoOfRecords) {

				DBG_FUNC("[102_221] _102221_search: Search Mode is Indication of record number in P1\n");

				/*P1 is not a valid record number for given EF*/
				DBG_ERR("[102_221] _102221_search: Wrong P1, must be an existing record number\n");
				DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

				THROW(EXCEP_RECORD_NOT_FOUND);
			}
		}
		else if (umbMode == SEARCH_FORWARD_FROM_NEXT_MODE) {

			DBG_FUNC("[102_221] _102221_search: FORWARD ENHANCED SEARCH FROM NEXT RECORD\n");
			umbSearchMode = SEARCH_RECORD_FORWARD;

			if(P1_BYTE != 0x00) { //P1 should be 00 for 'forward search from next record' mode
 
				DBG_ERR("[102_221] _102221_search: Invalid P1 '%1', must be '00' for Forward Next mode\n", P1_BYTE);
				DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");
				THROW(EXCEP_WRONG_P1_P2);
			}

			if (pccCurrentChannel->umbCurrentRecPtr == RECORD_UNDEFINED) { // Record is undefined
				/*Record no.*/
				DBG_FUNC("[102_221] _102221_search: Record is undefined, so search operation will start from first record\n");
				umbRecordNo = RECORD_FIRST;
			}
			else if (pccCurrentChannel->umbCurrentRecPtr == umbNoOfRecords) {

				if ((IS(CF, _7816_4_getFileDescriptor(fhaFile)))) { //Cyclic file

					DBG_FUNC("[102_221] _102221_search: Cyclic File: Record pointer is set on Last record, so operation will be done on first record\n");
					/*in cyclic file last record logically connected to first record, hence next record is first*/
					umbRecordNo = RECORD_FIRST;
				}
				else {// Linear file
					DBG_ERR("[102_221] _102221_search: Incorrect Data in First Byte of search indication \n");
					DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");

					THROW(EXCEP_INCORRECT_DATAFIELD);
				}
			}
			else {
				/*Increment Record no*/
				umbRecordNo = pccCurrentChannel->umbCurrentRecPtr + 0x01;
				DBG_FUNC("[102_221] _102221_search: Next record to current record i.e. '%1'\n", umbRecordNo);
			}
		}

		else if (umbMode == SEARCH_BACKWARD_FROM_PREVIOUS_MODE) {

			DBG_FUNC("[102_221] _102221_search: BACKWARD ENHANCED SEARCH FROM PREVIOUS RECORD\n");
			umbSearchMode = SEARCH_RECORD_BACKWARD;

			if (P1_BYTE != 0x00) { //P1 should be 00 for 'backward search from previous record' mode

				DBG_ERR("[102_221] _102221_search: Invalid P1 '%1', must be '00' for Backward Previous mode\n", P1_BYTE);
				DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");
				THROW(EXCEP_WRONG_P1_P2);
			}

			if (pccCurrentChannel->umbCurrentRecPtr == RECORD_UNDEFINED) {

				DBG_FUNC("[102_221] _102221_search: Record is undefined, so search operation will start from Last record\n");
				/*Set last Record no.*/
				umbRecordNo = umbNoOfRecords;
			}
			else if (pccCurrentChannel->umbCurrentRecPtr == RECORD_FIRST) {

				if ((IS(CF, _7816_4_getFileDescriptor(fhaFile)))) {//cyclic

					DBG_FUNC("[102_221] _102221_search: Cyclic File: Record pointer is at first record, so operation will be done on Last record\n");
					/*in cyclic file first record logically connected to last record, hence previous record is last*/
					umbRecordNo = umbNoOfRecords;
				}
				else { //Linear
					DBG_ERR("[102_221] _102221_search: Linear Fixed: It cannot proceed to previous record, because Record pointer is at the first record\n");
					DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");
					THROW(EXCEP_INCORRECT_DATAFIELD);
				}
			}
			else {

				/*Decrement Record no*/
				umbRecordNo = pccCurrentChannel->umbCurrentRecPtr - 0x01;

				DBG_FUNC("[102_221] _102221_search: Previous record to current one i.e. '%1'\n", umbRecordNo);
			}
		}

		DBG_FUNC("[102_221] _102221_search: Searching from record %1\n", umbRecordNo);

		umbRecordNo = _7816_4_EnhancedSearchRecordFile(fhaFile, gumbWorkingBuffer, P3_BYTE - LENGTH_SEARCH_INDICATION, umbSearchMode, umbTypeOfEnhanced, umbRecordNo, COMMAND_DATA[SEARCH_INDEX_OF_OFFSET_OR_VALUE], COMMAND_RESPONSE_DATA);
	}

	if (umbRecordNo) { // if records are found in searching

		DBG_FUNC("[102_221] _102221_search: Pattern is found \n");

		pccCurrentChannel->umbCurrentRecPtr = COMMAND_RESPONSE_DATA[0x00];
		gudbOutputLen = umbRecordNo;
	}
	else { //Pattern not found
		
		DBG_ERR("[102_221] _102221_search: Pattern is not found \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");
		THROW(EXCEP_PATTERN_NOT_FOUND);
	}

	DBG_FUNC_EXIT("<<[102_221] _102221_search() \n");
}

