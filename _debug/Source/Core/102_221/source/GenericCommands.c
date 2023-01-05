#include "102_221.h"

/*
* Macros
*/
#define P1_CLOSE_CHANNEL											(0x80)
#define P1_OPEN_CHANNEL												(0x00)
#define P1_IS_INVALID												(0x7F)
#define P3_MANAGE_CHANNEL_RESPONSE_LENGTH_FOR_OPEN					(0x01)
#define P3_MANAGE_CHANNEL_RESPONSE_LENGTH_FOR_CLOSE					(0x00)
#define MAX_LOGICAL_CHANNEL_NUMBER									(0x13)
#define P1_STATUS_APP_TERMINATION_INITIATED							(0x02)
#define	P2_STATUS_RETURN_NO_DATA									(0x0C)
#define P2_STATUS_RETURN_DF_NAME_TLV								(0x01)
#define P2_STATUS_RETURN_FCP_TLV									(0x00)
#define LENGTH_PS_DO												(0x01)				
#define LENGTH_TAG_KEY_REFERENCE									(0x01)	
#define PIN_ENABLE_STATUS											(0x00)
#define LENGTH_FIX													(0x00)
#define INDEX_TAG_PIN_STATUS_TEMP_DO								(0x00)
#define INDEX_LENGTH_PSTDO											(0x01)
#define INDEX_TAG_SECURITY_SCDO_ALWAYS								(0x02)
#define INDEX_LENGTH_PS												(0x03)
#define INDEX_PIN_ENABLE_STATUS										(0x04)

#define LENGTH_FCP_PROP_TLV											(0x0E)
/*Definition the function for FCP Response PIN Status Template(PS_DO)
@param pdPstdo: It's the identify the set pin of current DF
@pumbResponse: It's prepare the FCP Response
*/
/*In this Code Pin counter limit 8, So We Only 8 Pin Set in Perticular DF*/

uMonoByte _102221_getPSTDO(tPinData pdPstdo, puMonoByte pumbResponse) {
	uMonoByte	umbIndex = LENGTH_FIX;
	uMonoByte	umbIndexId = LENGTH_FIX;
	uMonoByte	umPinCount = LENGTH_FIX;
	uMonoByte	umDoStatus = LENGTH_FIX;

	DBG_FUNC_ENTRY(">>[102221]_102221_getPSTDO() \n");

	/*Set Tag PIN status template DO */
	pumbResponse[INDEX_TAG_PIN_STATUS_TEMP_DO] = TAG_PIN_STATUS_TEMP_DO;
	/*Set Tag Security Do Always & Tag Length*/
	pumbResponse[INDEX_TAG_SECURITY_SCDO_ALWAYS] = TAG_SECURITY_SCDO_ALWAYS;
	pumbResponse[INDEX_LENGTH_PS] = LENGTH_PS_DO;

	umbIndexId = 0x05;
	/*In First 2 Bytes Pin Check 01-08, 81-88*/
	for (umbIndex = 0; umbIndex < 14;umbIndex++) {
		if (pdPstdo & (0x01 << umbIndex)) {

			/*Set PIN status template DO Key reference tag & Key reference length */
			pumbResponse[umbIndexId++] = TAG_KEY_REFERENCE;
			pumbResponse[umbIndexId++] = LENGTH_TAG_KEY_REFERENCE;
			pumbResponse[umbIndexId++] = PIN_APP_1 + umbIndex;
			DBG_FUNC("<<[102221]_102221_getPSTDO() Set Pin '%2 \n", (PIN_APP_1 + umbIndex));
			umPinCount++;
			/*Check Pin Status, If Pin is Enable*/
			if (PIN_ENABLE_STATUS == (_7816_4_isPinActive(PIN_APP_1 + umbIndex))) {
				DBG_FUNC("<<[102221]_102221_getPSTDO() Pin is Enable \n");
				umDoStatus = umDoStatus | (0x01 << (8 - umPinCount));
			}

		}
	}
	/*In Last 2 Bytes Pin Check 0A-0E and 8A-8E*/
	for (umbIndex = 0; umbIndex < 14;umbIndex++) {
		if (pdPstdo & (0x010000 << umbIndex)) {

			/*Set PIN status template DO Key reference tag & Key reference length */
			pumbResponse[umbIndexId++] = TAG_KEY_REFERENCE;
			pumbResponse[umbIndexId++] = LENGTH_TAG_KEY_REFERENCE;
			pumbResponse[umbIndexId++] = PIN_SECOND_APP_1 + umbIndex;
			DBG_FUNC("<<[102221]_102221_getPSTDO() Set Pin '%2 \n", (PIN_APP_1 + umbIndex));
			umPinCount++;
			/*Check Pin Status, If Pin is Enable*/
			if (PIN_ENABLE_STATUS == (_7816_4_isPinActive(PIN_SECOND_APP_1 + umbIndex))) {
				DBG_FUNC("<<[102221]_102221_getPSTDO() Pin is Enable \n");
				umDoStatus = umDoStatus | (0x01 << (8 - umPinCount));
			}

		}
	}
	DBG_FUNC("<<[102221]_102221_getPSTDO() Set Pin Status '%2' \n", umDoStatus);
	/*Set DoStatus */
	pumbResponse[INDEX_PIN_ENABLE_STATUS] = umDoStatus;
	DBG_FUNC("<<[102221]_102221_getPSTDO() Set Pin Length \n");
	/*Set DoTag Length */
	pumbResponse[INDEX_LENGTH_PSTDO] = umbIndexId - 0x02;
	DBG_FUNC_EXIT("<<[102221]_102221_getPSTDO() \n");
	return umbIndexId;

}
/*
* UICC Status Command
*/

//enum {
	//FILE_IS_MF = 0x01,
	//FILE_IS_DF = 0x02,
	//FILE_IS_EF = 0x04
//}tFileType;


void _102221_status(void) {

	uMonoByte preparedLen = 0x00;

	DBG_FUNC_ENTRY(">>[102_221] _102221_status() \n");
	
	if (P1_BYTE > P1_STATUS_APP_TERMINATION_INITIATED) { 
		//P1 must either be 01 02 or 00
		DBG_ERR("[102_221] _102221_status: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_status() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	//check if the value of P1 is valid or not
	DBG_FUNC(">>[102_221] _102221_status: P1 is valid\n");

	if (P2_BYTE == P2_STATUS_RETURN_FCP_TLV) {
		// Prepare FCP tlv of current DF
		DBG_FUNC(">>[102_221] _102221_status: Return FCP template of current DF\n");
		gudbOutputLen = _102221_prepareFCPResponseForFile(pccCurrentChannel->fhaCurrentDf, RESPONSE_BUFF); //getting the prepared FCP response
		preparedLen = (uMonoByte)gudbOutputLen;
		//fcp Response 
		if (P3_BYTE != preparedLen) {
			gudbOutputLen = 0x00;
		}

	}
	else if (P2_BYTE == P2_STATUS_RETURN_DF_NAME_TLV) { // if p2 is to check TLV_OBJ of current selected Application

		DBG_FUNC(">>[102_221] _102221_status: Return DF Name TLV of current ADF\n");

		if (pccCurrentChannel->fhaCurrentAdf == NO_FILE_PRESENT) {

			//to check AID DF NAME TLV P2 must be 01
			DBG_ERR("[102_221] _102221_status: No Application selected\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_status() \n");
			THROW(EXCEP_WRONG_P1_P2);
		}

		DBG_FUNC(">>[102_221] _102221_status: Preparing DF Name TLV of current ADF\n");

		//setting the first index as the Tag for DF
		RESPONSE_BUFF[0] = TAG_DF_NAME;

		//getting the prepared response from index 1
		_7816_4_getAidOfAdf(pccCurrentChannel->fhaCurrentAdf, &RESPONSE_BUFF[1]);

		gudbOutputLen = RESPONSE_BUFF[1] + 2;
		preparedLen =(uMonoByte) gudbOutputLen;
			if (P3_BYTE != gudbOutputLen) {
				
				gudbOutputLen = 0x00;

				DBG_ERR("[102_221] _102221_status: Invalid P3 Byte as per Luicc i.e. prepared length %2\n", gudbOutputLen);
				DBG_FUNC_EXIT("<<[102_221] _102221_status() \n");
				
				/*raise exception of Wrong P3*/
				THROW2(EXCEP_WRONG_P3, preparedLen);
		}
	}
	else if (P2_BYTE == P2_STATUS_RETURN_NO_DATA)
	{	
		DBG_FUNC(">>[102_221] _102221_status: No response to return\n");
		gudbOutputLen = 0x00;
		THROW(EXCEP_NORMAL_ENDING);
	}
	else {
		//p2 must either be 00 , 01 or 0C
		DBG_ERR("[102_221] _102221_status: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_status() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE != preparedLen) {

		DBG_ERR("[102_221] _102221_status: Invalid P3 Byte as per Luicc i.e. prepared length %2\n", gudbOutputLen);

		DBG_FUNC_EXIT("<<[102_221] _102221_status() \n");
		/*raise exception of Wrong P3*/
		THROW2(EXCEP_WRONG_P3,preparedLen);
	}

	DBG_FUNC_EXIT("<<[102_221] _102221_status() \n");
}

/*Definition the function selectByPath
* @param parentDf: It's the address of Current directory file
@param pumbPath: It's the Current Directory File Id
@param umbPathLength: It's Length of File Id
*/

tFileHeaderAddr _102221_selectByPath(tFileHeaderAddr parentDf, puMonoByte pumbPath, uMonoByte umbPathLength) {
	/*Variables declaration/definition*/
	uMonoByte	umbIndex;
	uDiByte		udbFileId;

	DBG_FUNC_ENTRY(">>_102221_selectByPath\n"); 

	//Try to Select File Id, Select By Path
	for (umbIndex = 0x00; umbIndex < umbPathLength; umbIndex  += LENGTH_FILE_ID) {

		//Get Next File ID threw mm_makeUdiByte() function			
		udbFileId = mm_makeUdiByte(pumbPath, umbIndex);

		DBG_FUNC("[102_221]_102221_selectByPath:Get File Id '%2' \n", udbFileId);

		if ((0x00 == umbIndex) && (FID_CURRENT_ADF == udbFileId) && (parentDf == _7816_4_getMfAddress())) { // Check if its Reserved ADF file ID

			DBG_FUNC("[102_221]_102221_selectByPath: Requesting Current ADF using Reserved ADF file id 7FFF\n");

			/*store current ADF even if its not set*/
			parentDf = pccCurrentChannel->fhaCurrentAdf;

			DBG_FUNC("[102_221]_102221_selectByPath: Current ADF '%2'\n", parentDf);

		}
		else {
			DBG_FUNC("[102_221] _102221_selectByPath: Searching given File ID '%2' under Current DF\n", udbFileId);

			/*Check given File ID under current DF*/
			_7816_4_ifFileExists(udbFileId, parentDf, MODE_FIND_FID, &parentDf);

		}

		if (
				(NO_FILE_PRESENT == parentDf) //File not found
				||								//OR
				(!(IS(DF, _7816_4_getFileDescriptor(parentDf)))) //Selected file is an EF
			) {
			DBG_FUNC("[102_221] _102221_selectByPath: File ID '%2' not found under Current DF\n", udbFileId);
			break;
		}
	}

	if (umbIndex < (umbPathLength - LENGTH_FILE_ID)) {
		/*An EF found in the middle of given path
		* or
		* File not found*/
		return NO_FILE_PRESENT;
	}// if (umbIndex < (umbPathLength - LENGTH_FILE_ID))

	DBG_FUNC_EXIT("<<[102_221] _102221_selectByPath() \n");

	return parentDf;
}

/*Definition the function selectByFileId
* @param fhaFile: Current directory file
*/
tFileHeaderAddr _102221_selectByFileId(tFileHeaderAddr fhaFile, uDiByte udbFileID) {
	/*Variables declaration/definition*/
	tFileHeader			fhFile;
	tFileHeaderAddr	fhaRequestedFile = fhaFile; // Default initalized with current directory

	DBG_FUNC_ENTRY(">>_102221_selectByFileId\n");

	if (udbFileID == FID_MF) //if its MF
	{
		DBG_FUNC("[102_221] _102221_selectByFileId: Requesting MF 3F00\n");
		/*Set MF as current DF*/
		fhaRequestedFile = _7816_4_getMfAddress();
	}
	else if (FID_CURRENT_ADF == udbFileID) { // Check if its Reserved ADF file ID

		DBG_FUNC("[102_221] _102221_selectByFileId: Requesting Current ADF using Reserved ADF file id 7FFF\n");

		/*If application is selected fhaRequestedFile contains its address otherwise NO_FILE_PRESENT*/
		/*Set ADF as current DF*/
		fhaRequestedFile = pccCurrentChannel->fhaCurrentAdf;

	}
	/*Check the File ID of current DF with incoming File id*/
	else if (udbFileID != _7816_4_getFileId(fhaFile))  {
		
		DBG_FUNC("[102_221] _102221_selectByFileId: Search given File ID '%2' under Current DF\n", udbFileID);

		/*Check given File ID under current DF*/
		_7816_4_ifFileExists(udbFileID, fhaFile, MODE_FIND_FID, &fhaRequestedFile);

		if (NO_FILE_PRESENT == fhaRequestedFile) { //File not found

			DBG_FUNC("[102_221] _102221_selectByFileId: File ID '%2' not found under Current DF\n", udbFileID);

			/*read header of current DF*/
			_7816_4_getFileHeader(fhaFile, &fhFile);

			/*fetch the parent of current DF*/
			fhaFile = fhFile.fhaParent;
			
			/*Check if there is a parent, or current directory is root one*/
			if (fhaFile != NO_FILE_PRESENT) {

				DBG_FUNC("[102_221] _102221_selectByFileId: Check if parent of current directory is given file\n");

				/*Check File ID of Current DF's parent */
				if (udbFileID == _7816_4_getFileId(fhaFile)) {

					DBG_FUNC("[102_221] _102221_selectByFileId: Parent of current DF is the requested file\n");

					/*return parent file*/
					fhaRequestedFile = fhaFile;
				}
				else { //any DF which is an immediate child of the parent of the current DF

					DBG_FUNC("[102_221] _102221_selectByFileId: Its not even parent of current DF\n");
					DBG_FUNC("[102_221] _102221_selectByFileId: Search same file Id '%2' under parent of Current DF\n", udbFileID);

					/*search same file in parent of current DF*/
					_7816_4_ifFileExists(udbFileID, fhaFile, MODE_FIND_FID, &fhaRequestedFile);


					if ((NO_FILE_PRESENT != fhaRequestedFile) // File found
						&&								//AND
						(!(IS(DF, _7816_4_getFileDescriptor(fhaRequestedFile))))) {	// if its EF

						/*raise exception of file not found*/
						DBG_ERR("[102_221] _102221_selectByFileId: File id '%2' not found \n", udbFileID);
						fhaRequestedFile = NO_FILE_PRESENT;
					}
				}
			}

		}
	}

	DBG_FUNC("[102_221] _102221_selectByFileId: File ID '%2' found\n", udbFileID);
	DBG_FUNC_EXIT("<<[102_221]_102221_selectByFileId() \n");
	return fhaRequestedFile;
}


/*
* UICC select command
*/

void _102221_select(void) {

	/*Variables declaration/definition*/
	uDiByte			udbFileID;
	tFileHeaderAddr fhaFile;
	tFileHeader		fhFile;
	DBG_FUNC_ENTRY(">>[102_221] _102221_select()\n");

	/*get fileID from command data*/
	udbFileID = mm_makeUdiByte(COMMAND_DATA, 0x00);

	/*get Current DF*/
	fhaFile = pccCurrentChannel->fhaCurrentDf;

	/*validation of P2 byte*/
	if (P2_BYTE_VALIDATION_PARAMETER != (MASK_P2_BYTE & P2_BYTE))
	{
		DBG_ERR("[102_221] _102221_select : Invalid P2 Byte \n");
		DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
		/*raise exception of Wrong P2*/
		THROW(EXCEP_WRONG_P1_P2);
	}

	/*Select a File by Different Methods*/
	switch (P1_BYTE)
	{
	case P1_SELECT_BY_FILE_ID:
		if (P3_BYTE == LENGTH_FILE_ID) {
			/* Select the ID by _102221_selectByFileId*/
			fhaFile = _102221_selectByFileId(fhaFile, udbFileID);
		}
		else
		{
			DBG_ERR("[102_221] _102221_select : Invalid P3 Byte \n");
			DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
			/*raise exception of Wrong P3*/
			THROW2(EXCEP_WRONG_P3, LENGTH_FILE_ID);
		}

		break;
	case P1_SELECT_BY_CHILD:
		if (P3_BYTE == LENGTH_FILE_ID) {
			DBG_FUNC("[102_221] _102221_select : Select child DF of the current DF.\n");
			/*Select child DF of the current DF*/
			_7816_4_ifFileExists(udbFileID, fhaFile, MODE_FIND_FID, &fhaFile);

			/*EF check*/
			if (
				(NO_FILE_PRESENT != fhaFile) // File found
				&&								//AND
				(!(IS(DF, _7816_4_getFileDescriptor(fhaFile))))
				)
			{	// if its EF
				/*raise exception of file not found*/
				DBG_ERR("[102_221]_102221_select() Found File id '%2' is a EF \n", udbFileID);
				fhaFile = NO_FILE_PRESENT;
			}
		}
		else
		{
			DBG_ERR("[102_221] _102221_select : Invalid P3 Byte \n");
			DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
			/*raise exception of Wrong P3*/
			THROW2(EXCEP_WRONG_P3, 0x00);
		}
		break;

	case P1_SELECT_BY_PARENT:
		/*p3 Bytes Depends on FCP Response*/
		DBG_FUNC("[102_221] _102221_select : Select parent DF of the current DF.\n");
		/* Select parent DF of the current DF */
		fhaFile = _7816_4_getParentFile(fhaFile);
		break;

	case P1_SELECT_BY_AID:

		DBG_FUNC("[102_221] _102221_select : P1=04 Select by AID.\n");
		//Support Invalid selection of Select by AID (P1=04) for RFM.
		if (SRC_RFM == gumbcommandSource) {

			THROW(EXCEP_COMMAND_NOT_ALLOWED);
		}

		/*check AID length*/
		if (
			(P3_BYTE > LENGTH_MAX_AID)
			||
			(P3_BYTE < LENGTH_MIN_AID)
			) /*P3 for SelectByAID should be valid i.e. 5-16 bytes*/
		{
			DBG_ERR("[102_221] _102221_select : Invalid P3 Byte \n");

			DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");

			/*raise exception of Wrong P3*/
			THROW2(EXCEP_WRONG_P3, 0x00);
		}

		/*search for ADF using DF Name*/
		_7816_4_ifAdfExists(&COMMAND_APDU[INDEX_P3], &fhaFile);

		if (NO_FILE_PRESENT == fhaFile) {  // if search doesnt succeed
			DBG_ERR("[102_221] _102221_select: File not found \n");
			/*file is not found then*/
			break;
		}

		DBG_FUNC("[102_221] _102221_select : Found File as Current ADF.\n");
		/*Set the found ADF as current ADF*/
		pccCurrentChannel->fhaCurrentAdf = fhaFile;
		break;

	case P1_SELECT_BY_PATH_MF:				//Select by path from MF

		if ((P3_BYTE!=0) && (P3_BYTE % LENGTH_FILE_ID == 0)) {
			DBG_FUNC("[102_221] _102221_select: Requesting MF 3F00\n");
			/*Current DF would be MF*/
			fhaFile = _102221_selectByPath(_7816_4_getMfAddress(), COMMAND_DATA, P3_BYTE);
		}
		else
		{
			/*If p3 value is not correct then*/
			DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
			THROW(EXCEP_WRONG_P3);
		}
		break;

	case P1_SELECT_BY_PATH_CURRENT_DF:		//Select by path from current DF 
		if ((P3_BYTE != 0) && (P3_BYTE % LENGTH_FILE_ID == 0)) {
			/* Store the Select file address in fhaFile using _102221_selectByPath function */
			fhaFile = _102221_selectByPath(fhaFile, COMMAND_DATA, P3_BYTE);
		}
		else {
			/*If p3 value is not correct then*/
			DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
			THROW(EXCEP_WRONG_P3);
		}
		break;

	default:
		/*If p1 value is not correct then*/
		DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
		THROW(EXCEP_WRONG_P1_P2);

	}

	if (NO_FILE_PRESENT == fhaFile) { // if search doesnt exceed
		DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
		THROW(EXCEP_FILE_NOT_FOUND);
	}
	/*check P2_BYTE for FCP template to be returned or not*/
	if (RETURN_FCP_TEMPLATE == (P2_BYTE & MASK_FCP_RETURN_BYTES))
	{
		/*get length of prepared response*/
		gudbOutputLen =  _102221_prepareFCPResponseForFile(fhaFile, RESPONSE_BUFF);
	}
	

	/*read header of current file*/
	_7816_4_getFileHeader(fhaFile, &fhFile);

	if (IS(DF, fhFile.umbFileDesc)) //if its DF
	{
		DBG_FUNC("[102_221]_102221_select() :Found File ID is a DF\n");

		/*Set found file as current DF in current channel context*/
		pccCurrentChannel->fhaCurrentDf = fhaFile;

		/*And reset the current EF to undefined*/
		pccCurrentChannel->fhaCurrentEf = NO_FILE_PRESENT;
	}
	else { // if its EF

		DBG_FUNC("[102_221] _102221_select():Found File ID is an EF\n");

		/*set found file's parent DF as current DF*/
		pccCurrentChannel->fhaCurrentDf = fhFile.fhaParent;

		/*Set current EF as presented file*/
		pccCurrentChannel->fhaCurrentEf = fhaFile;
	}

	DBG_FUNC("[102_221]_102221_select: Reset the Record pointer\n");

	/*Current Record pointer is undefined on successful selection*/
	pccCurrentChannel->umbCurrentRecPtr = RECORD_UNDEFINED;

	DBG_FUNC_EXIT("<<[102_221] _102221_select() \n");
}
/*
* It prepares length of FCP response.
*/
//uMonoByte _102221_prepFCPforFile(tFileHeaderAddr fhaADF, puMonoByte pumbAid)
//{
	/*TO_DO....*/
	//return 0;
//}

/*
* It implements UICC Authenticate command
*/
void _102221_authenticate(void) {

	DBG_FUNC_ENTRY(">>[102_221] _102221_authenticate() \n");

	DBG_FUNC_EXIT("<<[102_221] _102221_authenticate() \n");
}

/*
* It implements UICC Manage Channel command
*/
void _102221_manageChannel(void) {

	uMonoByte umbChannelNo; //which channel to open

	DBG_FUNC_ENTRY(">>[102_221] _102221_manageChannel() \n");

	if ((P1_OPEN_CHANNEL == P1_BYTE) && (0x00 == P2_BYTE)) { // P1 states Open function and P2 is '00'

		DBG_FUNC("[102_221] _102221_manageChannel: Request to Open a channel\n");

		if (P3_MANAGE_CHANNEL_RESPONSE_LENGTH_FOR_OPEN != P3_BYTE) //check if p3 is not equal to expected response length
		{
			/*Expected length of response must be 01*/
			DBG_ERR("[102_221] _102221_manageChannel: Le/P3 must be '01'\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_manageChannel()\n");
			THROW2(EXCEP_WRONG_P3, P3_MANAGE_CHANNEL_RESPONSE_LENGTH_FOR_OPEN);
		}

		if (gumbChannelsState) //Check if atleast 1 logical channel must be available
		{
			/*Checking availability of each logical channel*/
			if (gumbChannelsState & MASK_LOGICAL_CHANNEL_1) {

				DBG_FUNC("[102_221] _102221_manageChannel: Opening channel no '01'\n");
				/*Open logical channel no. 1*/
				umbChannelNo = 0x01;
			}
			else if (gumbChannelsState & MASK_LOGICAL_CHANNEL_2) {

				DBG_FUNC("[102_221] _102221_manageChannel: Opening channel no '02'\n");
				/*Open logical channel no. 2*/
				umbChannelNo = 0x02;

			}
			else { //it means logical channel 3 available

				DBG_FUNC("[102_221] _102221_manageChannel: Opening channel no '03'\n");
				umbChannelNo = 0x03; //open logical channel no. 3
			}

			/*clear the corresponding bit*/
			gumbChannelsState &= (uMonoByte)(~(SHIFT_LEFT_BY_N_BITS(0x01, umbChannelNo)));

			/*Store channel no. in response buffer*/
			COMMAND_RESPONSE_DATA[0x00] = umbChannelNo;

			/*Set Response length*/
			gudbOutputLen = 0x01;

			/*default File would be MF*/
			ccContexts[umbChannelNo].fhaCurrentDf = _7816_4_getMfAddress();

		}
		else {
			/*No channel available to open*/
			DBG_ERR("[102_221] _102221_manageChannel: All channels are already opened, so no new channels left to open\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_manageChannel()\n");
			THROW(EXCEP_FUNCTION_NOT_SUPPORTED);
		}
	}
	else if ((P1_CLOSE_CHANNEL == P1_BYTE) && (P2_BYTE <= MAX_LOGICAL_CHANNEL_NUMBER)) {

		if (P3_MANAGE_CHANNEL_RESPONSE_LENGTH_FOR_CLOSE != P3_BYTE) //check if p3 is not equal to expected response length
		{
			/*Expected length of response must be 01*/
			DBG_ERR("[102_221] _102221_manageChannel: Le/P3 must be '00'\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_manageChannel()\n");
			THROW2(EXCEP_WRONG_P3, 0x00);
		}

		if ((BASIC_LOGICAL_CHANNEL < P2_BYTE) && (P2_BYTE < 0x04)) { // Valid range for Logical channels are '01','02' and '03'

			DBG_FUNC("[102_221] _102221_manageChannel: Closing the channel %1\n", P2_BYTE);

			/*Set the corresponding bit of incoming logical channel*/
			gumbChannelsState |= (uMonoByte)(SHIFT_LEFT_BY_N_BITS(0x01, P2_BYTE));

			/*clear the Channel context*/
			mem_set((puMonoByte)& ccContexts[P2_BYTE], 0x00, sizeof(tChannelContext));
		}
		else {

			/*Closing the unsupported logical channel*/
			DBG_ERR("[102_221] _102221_manageChannel: Wrong Logical channel in P2 as its not supported\n");
			DBG_FUNC_EXIT("<<[102_221] _102221_manageChannel()\n");
			THROW(EXCEP_FUNCTION_NOT_SUPPORTED);
		}
	}
	else {
		/*Invalid P1 or P2*/
		DBG_ERR("[102_221] _102221_manageChannel: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[102_221] _102221_manageChannel()\n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	DBG_FUNC_EXIT("<<[102_221] _102221_manageChannel() \n");
}
/*fhaFile : whose FCP response to return, DF or ADF or EF
pumbFcpResponse : where response to be copied
return total fcp response length
*/

uMonoByte _102221_prepareFCPResponseForFile(IN_ARG tFileHeaderAddr fhaFile, OUT_ARG puMonoByte pumbFcpResponse) {

	uMonoByte			umbIndex = 0x02;
	uDiByte				udbTemp;
	tFileHeader			fhFile;

	


	
	pumbFcpResponse[0] = TAG_FCP_TEMPLATE;

	/*read header of file*/
	_7816_4_getFileHeader(fhaFile, &fhFile);

	DBG_FUNC_ENTRY(">>_102221_prepareFCPResponseForFile() \n");

	/* TAG File Descriptor Byte 0x82  */
	pumbFcpResponse[umbIndex++] = TAG_FILE_DESC;

	/* Check Type of File*/
	if ((IS(LF, fhFile.umbFileDesc))
		||
		(IS(CF, fhFile.umbFileDesc))) {
		/* Length of file descriptor byte for LF and CF */

		pumbFcpResponse[umbIndex++] = 0x05;

		/* File descriptor byte */

		pumbFcpResponse[umbIndex++] = fhFile.umbFileDesc;

		/* Data Coding Byte of 1 byte */
		
		pumbFcpResponse[umbIndex++] = DATA_CODE_BYTE;

		/* LF or CF Record Size */

		pumbFcpResponse[umbIndex++] = 0x00;

		pumbFcpResponse[umbIndex++] = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize;


		/* LF or CF no. of records */

		pumbFcpResponse[umbIndex++] = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec;

	}
	else {

		/* Length of file descriptor byte for MF,DF and TF */

		pumbFcpResponse[umbIndex++] = 0x02;

		pumbFcpResponse[umbIndex++] = fhFile.umbFileDesc;

		//DCB of 1 byte

		pumbFcpResponse[umbIndex++] = DATA_CODE_BYTE;
	}

	pumbFcpResponse[umbIndex++] = TAG_FILE_ID;

	pumbFcpResponse[umbIndex++] = LENGTH_FILE_ID;

	umbIndex = mm_storeUdiByte(pumbFcpResponse, umbIndex, fhFile.udbFileId);

	if (IS(DF, fhFile.umbFileDesc)) {

		//Call getAdfAid()

		if (TRUE == _7816_4_getAidOfAdf(fhaFile, (pumbFcpResponse + umbIndex + 0x01))) {
			pumbFcpResponse[umbIndex] = TAG_DF_NAME;
			umbIndex += pumbFcpResponse[umbIndex + 0x01] + 0x02;
		}

		//If its adf append it in fcp response after ADF tag

		
		// Pro Tag A5 data.


		pumbFcpResponse[umbIndex++] = TAG_PROP_TLV;
		pumbFcpResponse[umbIndex++] = LENGTH_TAG_PROP_TLV;

		// UICC Characterstics Tag
		pumbFcpResponse[umbIndex++] = TAG_UICC_CHAR;
		pumbFcpResponse[umbIndex++] = LENGTH_UICC_CHAR;
		pumbFcpResponse[umbIndex++] = DATA_UICC_CHAR;
		pumbFcpResponse[umbIndex++] = TAG_AVAIL_MEM;
		pumbFcpResponse[umbIndex++] = LENGTH_AVAVIL_MEM;



	   // Call Function to get available memory.
		umbIndex = mm_storeWord(pumbFcpResponse, umbIndex, mm_GetAvailableMemory());
		pumbFcpResponse[umbIndex++] = TAG_SUPPORTED_COMMMANDS;
		pumbFcpResponse[umbIndex++] = LENGTH_SUPPORTED_COMMMANDS;
		pumbFcpResponse[umbIndex++] = DATA_SUPPORTED_COMMMANDS;


	}

	pumbFcpResponse[umbIndex++] = TAG_LCSI;

	pumbFcpResponse[umbIndex++] = LENGTH_LCSI;

	pumbFcpResponse[umbIndex++] = fhFile.flState;

	pumbFcpResponse[umbIndex++] = TAG_SEC_ATTR_REFERENCED;

	pumbFcpResponse[umbIndex++] = LENGTH_SEC_ATTR_REFERENCED;

	umbIndex = mm_storeUdiByte(pumbFcpResponse, umbIndex, fhFile.udbEFarrId);

	pumbFcpResponse[umbIndex++] = fhFile.umbEFarrRec;


	if (IS(DF, fhFile.umbFileDesc)) { //PTSDO

		//Call getPsdo()

		

		umbIndex += _102221_getPSTDO(fhFile.FileSpecific.DF.pdPstdo, (pumbFcpResponse + umbIndex));


	}
	else { // File Size
		pumbFcpResponse[umbIndex++] = TAG_FILE_SIZE;

		pumbFcpResponse[umbIndex++] = LENGTH_TOTAL_FILE_SIZE;

		if (IS(TF, fhFile.umbFileDesc)) {
			udbTemp = fhFile.FileSpecific.EF.fileData.udbFileSize;
		}
		else {
			udbTemp = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec * \
				fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize;
		}

		umbIndex = mm_storeUdiByte(pumbFcpResponse, umbIndex, udbTemp);

		
		/* SFI TAG */
		pumbFcpResponse[umbIndex++] = TAG_SFI;

		if (SFI_NOT_SUPPORTED != fhFile.FileSpecific.EF.umbSfi)
		{
			pumbFcpResponse[umbIndex++] = LENGTH_SFI;

            pumbFcpResponse[umbIndex++] = SHIFT_LEFT_BY_N_BITS(fhFile.FileSpecific.EF.umbSfi,3);


		}
		else {
			/* IF SFI NOT SUPPORTED */
			pumbFcpResponse[umbIndex++] = 0x00;

		}
	}


	pumbFcpResponse[0x01] = umbIndex - 0x02;

	return umbIndex;
}