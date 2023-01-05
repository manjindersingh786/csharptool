#include "102_222.h"
/*Macro
* Defines
*/
#define AN_ADF								(0xFF)

//All Possible values of valid LCSI
#define LCSI_CREATION_STATE										(0x01)
#define LCSI_INITALIZATION_STATE								(0x03)
#define LCSI_ACTIVATION_STATE_1									(0x05)
#define LCSI_ACTIVATION_STATE_2									(0x07)
#define LCSI_DEACTIVATION_STATE_1								(0x04)
#define LCSI_DEACTIVATION_STATE_2								(0x06)
#define LCSI_TERMINATION_STATE_1								(0x0C)
#define LCSI_TERMINATION_STATE_2								(0x0D)
#define LCSI_TERMINATION_STATE_3								(0x0E)
#define LCSI_TERMINATION_STATE_4								(0x0F)

/*
* Compare both values
* 
* @param: umbVal1 left operand
* @param: umbVal2 right operand
* 
* @throw: INCORRECT_DATAFIELD
*/
static void _102222_matchValues(IN_ARG uMonoByte umbVal1, IN_ARG uMonoByte umbVal2)
{
	DBG_FUNC_ENTRY(">> _102222_matchValues()\n");
	/*Compare both values*/
	if (umbVal1 != umbVal2)
	{
		/*If they dont match throw exception of INCORRECT_DATA_FIELD*/
		DBG_ERR("[102_222] _102222_matchValues: values mismatch: %1 %1\n", umbVal1, umbVal2);
		DBG_FUNC_EXIT("<< _102222_matchValues()\n");
		THROW(EXCEP_INCORRECT_DATAFIELD);
	}
	DBG_FUNC("[102_222] _102222_matchValues: both values same i.e. %1 \n", umbVal1);
	DBG_FUNC_EXIT("<< _102222_matchValues()\n");
}
/*
* It implements the Create File command acc. to ETSI 102 222
* 
* @param void
* 
* @return void
*/
//TODO: code of below function can be improved if we use while(true) loop to traverse Command elements
void _102222_createFile(void) {
	/*Variables declaration*/
	uMonoByte			umbFcpIndex = 0x00;
	uMonoByte			umbFileDesc = 0x00;
	uMonoByte			umbTemp = 0x00;
	uMonoByte			umbKey;
	uMonoByte			umbAdfOffset = 0x00;
	uMonoByte			umbIsDf = 0x00;
	uMonoByte			umbFileRecord = RECORD_UNDEFINED;
	uDiByte				udbFileSize;
	tFileHeader			fhFile = {0}; //reset it to 0x00
	tFileHeaderAddr		fhaFile = EMPTY_OBJECT;
	tFileHeaderAddr		fhaPropFile = EMPTY_OBJECT;
	tMemAddr			maFileData = EMPTY_OBJECT; // only stores file data memory address
	
	DBG_FUNC_ENTRY(">>[102_222] _102222_createFile()\n");

	/*Check P1-P2 must be '0000'*/
	if (P1_BYTE|P2_BYTE)
	{
		/*Wrong P1-P2 parameters*/
		DBG_ERR("[102_222] _102222_createFile: Invalid p1-p2: %1 %1\n", P1_BYTE, P2_BYTE);
		DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	DBG_FUNC("[102_222] _102222_createFile: Valid p1-p2: %1 %1\n", P1_BYTE, P2_BYTE);

	DBG_FUNC("[102_222] _102222_createFile: Checking FCP TLV\n");
	/*Check FCP tag first*/
	_102222_matchValues(TAG_FCP_TEMPLATE, COMMAND_DATA[umbFcpIndex++]);

	/*Check FCP Length is correct as per P3 i.e. 2 bytes less where 1 for tag and 1 for length byte*/
	/*Assuming length of FCP template comes in 1 byte only*/
	_102222_matchValues(P3_BYTE - 0x02, COMMAND_DATA[umbFcpIndex++]);

	DBG_FUNC("[102_222] _102222_createFile: Checking File descriptor TLV\n");

	/*Check File descriptor tag*/
	_102222_matchValues(TAG_FILE_DESC, COMMAND_DATA[umbFcpIndex++]);

	/*Read File descriptor Length*/
	umbTemp = COMMAND_DATA[umbFcpIndex++];

	/*Check length of File Descriptor Byte*/
	if (
		(LENGTH_FILE_DESC != umbTemp)		//	Not '02'
		&&									//	and
		(LENGTH_REC_FILE_DESC != umbTemp)	//	Not '04'
		)
	{
		/*Invalid length of File Descriptor Tag*/
		DBG_ERR("[102_222] _102222_createFile: Invalid File desc length: %1\n", umbTemp);
		DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
		THROW(EXCEP_INCORRECT_DATAFIELD);
	}

	/*Get File descriptor */
	umbFileDesc = COMMAND_DATA[umbFcpIndex++];

	/* Check MSbit of File Descriptor byte
	* OR
	* Data coding byte '21'
	*/
	if ((SHIFT_RIGHT_BY_N_BITS(umbFileDesc,7))    //Invalid File Desc byte
		||					//OR
		(FIX_DCB_FOR_102_222 != COMMAND_DATA[umbFcpIndex++]) //invalid Data coding byte
		)
	{
		/*Invalid Data*/
		DBG_ERR("[102_222] _102222_createFile: Invalid File desc byte '%1' or data coding byte \n", umbFileDesc);
		DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
		THROW(EXCEP_INCORRECT_DATAFIELD);
	}

	/*flag to check if its DF, as need to check it multiple times down in the function*/
	umbIsDf = IS(DF, umbFileDesc);

	/*Validate File Descriptor along with length of File Descriptor */
	if (
		(umbIsDf || IS(TF, umbFileDesc))	// Dedicated File or Transparent File
		&&												// AND
		(LENGTH_FILE_DESC == umbTemp)					// Length is '02'
		)
	{
		DBG_FUNC("[102_222] _102222_createFile: Incoming file is transparent\n");
		/*Store descriptor only*/
		fhFile.umbFileDesc = umbFileDesc;
	}
	else if (
		(IS(LF, umbFileDesc) || IS(CF, umbFileDesc))	// Linear File or Cyclic File i.e. Record structure file
		&&												// AND
		(LENGTH_REC_FILE_DESC == umbTemp)				// Length is '04'
		)
	{
		DBG_FUNC("[102_222] _102222_createFile: Incoming file is Record structured\n");
		/*Store descriptor*/
		fhFile.umbFileDesc = umbFileDesc;

		/*Stores record size*/
		/*Assuming Record size is always less than or equal to 0xFF ,
		* Hence, ignoring first byte of Record size in command */
		fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize = COMMAND_DATA[umbFcpIndex + 1];

		/*Move index to next TLV File-ID*/
		umbFcpIndex += 2;
	}
	else 
	{
		/*inconsistency of File Desc Len with File Desc byte*/
		DBG_ERR("[102_222] _102222_createFile: Wrong File Desc Len as per File Desc byte\n");
		DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
		THROW(EXCEP_INCORRECT_DATAFIELD);
	}
	
	DBG_FUNC("[102_222] _102222_createFile: Checking File ID TLV\n");

	/*Check File ID tag*/
	_102222_matchValues(TAG_FILE_ID, COMMAND_DATA[umbFcpIndex++]);

	/*Check File ID tag's Length*/
	_102222_matchValues(LENGTH_FILE_ID, COMMAND_DATA[umbFcpIndex++]);

	/*Store File ID*/
	fhFile.udbFileId = mm_makeUdiByte(COMMAND_DATA, umbFcpIndex);

	DBG_FUNC("[102_222] _102222_createFile: Checking if MF is created prior or not\n");

	/*Read the MF header address from system page*/
	baseReadNBytes(ADDRESS_MASTER_FILE, (puMonoByte) &fhaFile, SIZE_OF_FILE_HEADER_ADDR);

	/*Check if no MF created, ensure MF Create File command received*/
	if (
		(NO_FILE_PRESENT == fhaFile)	//If MF address is not defined i.e. MF not created
		&&													//AND
		(FID_MF != fhFile.udbFileId)						//Incoming command is not for MF
		)
	{
		/*Other create file Commands are not allowed if MF is not yet created*/
		DBG_ERR("[102_222] _102222_createFile: First create file command must be a MF\n");
		DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
		THROW(EXCEP_COMMAND_NOT_ALLOWED);
	}

	/*Skip to LCSI or DF Name TLV*/
	umbFcpIndex += 0x02;

	/*Check if incoming create file command is for DF
	* AND
	* Tag is DF NAME
	*/
	if (umbIsDf && (TAG_DF_NAME == COMMAND_DATA[umbFcpIndex]))
	{
		DBG_FUNC("[102_222] _102222_createFile:DF Name TLV present\n");

		/*points to length of DF name tlv, Indication that this is an ADF*/
		umbAdfOffset = umbFcpIndex + 1;

		if ((COMMAND_DATA[umbAdfOffset] < LENGTH_MIN_AID)
			||
			(COMMAND_DATA[umbAdfOffset] > LENGTH_MAX_AID)) {
			/*Invalid Length of DF name*/
			DBG_ERR("[102_222] _102222_createFile: Invalid Length of DF name: 5-16 bytes only\n");
			DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
			THROW(EXCEP_INCORRECT_DATAFIELD);
		}

		/*Skip to LCSI TLV*/
		umbFcpIndex += COMMAND_DATA[umbAdfOffset] + 0x02; //Add 2 bytes of Tag and Length byte and length of 
															//value part
	}

	DBG_FUNC("[102_222] _102222_createFile:Checking LCSI tlv\n");

	/*check LCSI tag*/
	_102222_matchValues(TAG_LCSI, COMMAND_DATA[umbFcpIndex++]);

	/*Check LCSI length*/
	_102222_matchValues(LENGTH_LCSI, COMMAND_DATA[umbFcpIndex++]);

	/*Check valid values of LCSI byte*/
	switch (COMMAND_DATA[umbFcpIndex++]) {

	case LCSI_CREATION_STATE:
		DBG_FUNC("[102_222] _102222_createFile: File is created in Creation state\n");
		/*If LCSI state is creation*/
		fhFile.flState = LCSI_CREATED;
		break;

	case LCSI_INITALIZATION_STATE:
		DBG_FUNC("[102_222] _102222_createFile: File is created in Initialization state\n");
		/*If LCSI state is Initialization*/
		fhFile.flState = LCSI_INIT;
		break;

	case LCSI_ACTIVATION_STATE_1:
	case LCSI_ACTIVATION_STATE_2:
		DBG_FUNC("[102_222] _102222_createFile: File is created in Activation state\n");
		/*If LCSI state is Operational state - activated*/
		fhFile.flState = LCSI_ACTIVATED;
		break;

	case LCSI_DEACTIVATION_STATE_1:
	case LCSI_DEACTIVATION_STATE_2:
		DBG_FUNC("[102_222] _102222_createFile: File is created in Deactivation state\n");
		/*If LCSI state is Operational state - deactivated*/
		fhFile.flState = LCSI_DEACTIVATED;
		break;

	case LCSI_TERMINATION_STATE_1:
	case LCSI_TERMINATION_STATE_2:
	case LCSI_TERMINATION_STATE_3:
	case LCSI_TERMINATION_STATE_4:
		DBG_FUNC("[102_222] _102222_createFile: File is created in Termination state\n");
		/*If LCSI state is termination*/
		fhFile.flState = LCSI_TERMINATED;
		break;

	default:
		DBG_ERR("[102_222] _102222_createFile: Invalid LCSI value\n");
		THROW(EXCEP_INCORRECT_DATAFIELD); // Invalid LCSI value
	}

	DBG_FUNC("[102_222] _102222_createFile:Checking Security Attribute TLV, must be Referenced\n");

	/*Check Referenced Security Attribute '8B'. This is only supported Security Attribute*/
	_102222_matchValues(TAG_SEC_ATTR_REFERENCED, COMMAND_DATA[umbFcpIndex++]);

	/*Check Length of Security Attribute*/
	_102222_matchValues(LENGTH_SEC_ATTR_REFERENCED, COMMAND_DATA[umbFcpIndex++]);

	/*Save EFarr file ID*/
	fhFile.udbEFarrId = mm_makeUdiByte(COMMAND_DATA, umbFcpIndex);
	umbFcpIndex += LENGTH_FILE_ID;		//Skip to EFarr record

	/*Save EFarr record*/
	fhFile.umbEFarrRec = COMMAND_DATA[umbFcpIndex++];
	
	/*Check if its DF*/
	if (umbIsDf){

		DBG_FUNC("[102_222] _102222_createFile: Checking DF specific TLVs\n");
		DBG_FUNC("[102_222] _102222_createFile: Checking Total File size TLV\n");
		/*Check Total File Size tag*/
		_102222_matchValues(TAG_TOTAL_FILE_SIZE, COMMAND_DATA[umbFcpIndex++]);

		/*Check Total Fil Size length*/
		_102222_matchValues(LENGTH_TOTAL_FILE_SIZE, COMMAND_DATA[umbFcpIndex++]);

		/*Value of Total File size will be ignored, as card supports Dynamic memory allocation for UICC*/
		umbFcpIndex += LENGTH_TOTAL_FILE_SIZE;

		DBG_FUNC("[102_222] _102222_createFile: Checking Pin Status Templated DO TLV\n");

		/*Check Pin Status template DO tag*/
		_102222_matchValues(TAG_PIN_STATUS_TEMP_DO, COMMAND_DATA[umbFcpIndex++]);

		/*get index of next TLV after PSTDO*/
		umbTemp = umbFcpIndex + COMMAND_DATA[umbFcpIndex] + 1;
		
		umbFcpIndex++;
		
		DBG_FUNC("[102_222] _102222_createFile: Parsing each tag of Pin Status Templated DO TLV\n");

		/*Traverse each TLV withing PSTDO*/
		while (umbFcpIndex < umbTemp)
		{
			/*Check Usage Qualifier tag*/
			if (TAG_USAGE_QUALIFIER == COMMAND_DATA[umbFcpIndex])
			{
				DBG_FUNC("[102_222] _102222_createFile: Usage Qualifier TLV present corresponding to following Key reference\n");

				/*Check Length of UQ*/
				_102222_matchValues(LENGTH_USAGE_QUALIFIER, COMMAND_DATA[umbFcpIndex+1]);

				/*Ignore Usage Quailifier byte, Right now we dont have any use case
				* So we are ignoring its value
				*/
				/*Skip to Key Reference TLV*/
				umbFcpIndex += 0x03; //3 = 1(Tag) + 1(length) + 1(value)
			}

			DBG_FUNC("[102_222] _102222_createFile: Checking Key reference TLV\n");

			/*Check Key Reference Tag*/
			_102222_matchValues(TAG_KEY_REFERENCE, COMMAND_DATA[umbFcpIndex++]);

			/*Check Key Reference Length*/
			_102222_matchValues(LENGTH_KEY_REFERENCE, COMMAND_DATA[umbFcpIndex++]);

			/*get key reference*/
			umbKey = COMMAND_DATA[umbFcpIndex++];

			if (_7816_4_isInvalidReferenceData(umbKey)) {

				/*key references are Invalid*/
				DBG_ERR("[102_222] _102222_createFile: Invalid Key Reference %1\n", umbKey);
				DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
				THROW(EXCEP_INCORRECT_DATAFIELD);
			}

			/*Set corresponding bit*/
			_7816_4_setKeyRef(&(fhFile.FileSpecific.DF.pdPstdo), umbKey);
		}
	}// End of DF handling
	else { //Its EF!!

		DBG_FUNC("[102_222] _102222_createFile: Checking EF specific TLVs\n");
		DBG_FUNC("[102_222] _102222_createFile: Checking File size TLV\n");

		/*Check File Size tag*/
		_102222_matchValues(TAG_FILE_SIZE, COMMAND_DATA[umbFcpIndex++]);

		/*Read file Size length*/
		umbTemp = COMMAND_DATA[umbFcpIndex++];

		/*Reset file size*/
		udbFileSize = 0x00; 

		/*If file size comes in 1 byte*/
		if (umbTemp == 1)
		{
			/*store File size*/
			udbFileSize = COMMAND_DATA[umbFcpIndex++];
		}
		else if (umbTemp == 2)
		{
			/*store File size*/
			udbFileSize = ((uDiByte)(SHIFT_LEFT_BY_N_BITS(COMMAND_DATA[umbFcpIndex], 0x08)) | COMMAND_DATA[umbFcpIndex + 1]);

			/*skip to next TLV*/
			umbFcpIndex += 2;
		}

		/*Check if File size is 0*/
		if(0 == udbFileSize)
		{
			/*Here control will come due two reasons
			* 1. if invalid length of file size TLV i.e current supports not more than 2 bytes
			* 2. if genuinely received file size value is 0
			*/
			DBG_ERR("[102_222] _102222_createFile:  Invalid file size %2\n", udbFileSize);
			DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
			THROW(EXCEP_INCORRECT_DATAFIELD);
		}

		/*Check type of EF*/
		if (IS(TF, umbFileDesc)) {
			
			DBG_FUNC("[102_222] _102222_createFile: Transparent File size '%2' \n", udbFileSize);

			/*saves file size*/
			fhFile.FileSpecific.EF.fileData.udbFileSize = udbFileSize;
		}
		/*if control here it means file is record structured
		* Now check if its divisible by record length */
		else if (
					(0x00 != fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize)
					&&
					(0x00 == (udbFileSize % fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize))
				) { 

			/*Save record nos.*/
			fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec = (uMonoByte)(udbFileSize / fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize);

			/*It stores last updated record, which is useful for cyclic file
			* For linear fixed file, records are fixed unlike Cyclic file where records depend upon last updated record
			*/
			fhFile.FileSpecific.EF.fileData.ofRecordFile.umbLastUpdatedRecord = RECORD_FIRST;

			DBG_FUNC("[102_222] _102222_createFile: Record sturcture file with Record length: '%1' No. of records '%1' \n", fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize, fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec);

		}
		else
		{
			/*Either Record length is 0 or File size is not divisible by record length*/
			/*Incorrect Data field*/
			DBG_ERR("[102_222] _102222_createFile: Either Record length is 0 or File size is not divisible by record length '%1' File size '%2'\n", fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize, udbFileSize);
			DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
			THROW(EXCEP_INCORRECT_DATAFIELD);
		}

		DBG_FUNC("[102_222] _102222_createFile: Checking Short File Identifier TLV\n");

		/*Check SFI tag*/
		if (TAG_SFI == COMMAND_DATA[umbFcpIndex]) { // SFI tag is present

			DBG_FUNC("[102_222] _102222_createFile:SFI TLV present \n");

			/*Get length*/
			umbTemp = COMMAND_DATA[umbFcpIndex + 0x01];

			/*Skip to value*/
			umbFcpIndex += 0x02;

			/*Get length*/
			if (LENGTH_SFI == umbTemp){ // SFI value is present 

				DBG_FUNC("[102_222] _102222_createFile:SFI value present \n");

				/*b8-b4 indicates SFI, b3-b1=000*/
				fhFile.FileSpecific.EF.umbSfi = SHIFT_RIGHT_BY_N_BITS( COMMAND_DATA[umbFcpIndex++], 0x03);
			}
			else if (0x00 == umbTemp) { // SFI value is missing

				DBG_FUNC("[102_222] _102222_createFile:SFI value empty, hence SFI is not supported for this file \n");
				/*SFI is not supported*/
				umbTemp = SFI_NOT_SUPPORTED;
			}
			else {
				/*Invalid length of SFI tag*/
				DBG_ERR("[102_222] _102222_createFile:  Invalid SFI length %1\n", umbTemp);
				DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
				THROW(EXCEP_INCORRECT_DATAFIELD);
			}
		}
		else { // SFID tag is not present

			/*lower 5 bits of file id is SFID*/
			/*Save it exactly in same way as done if SFID is present*/
			/**/
			fhFile.FileSpecific.EF.umbSfi = (uMonoByte)(fhFile.udbFileId);

			umbTemp = ~SFI_NOT_SUPPORTED; // just to ensure that below condition for checking existence of SFI under current df must be called

			DBG_FUNC("[102_222] _102222_createFile: Last 5 bits of File ID act as SFI\n");
		}

		/*Check if SFID exists under current DF previously*/
		if (SFI_NOT_SUPPORTED != umbTemp) {

			/*Check invalid values of SFI
			* Valid Range of SFI: '01' to '1E'
			*/
			if ((0x00 == fhFile.FileSpecific.EF.umbSfi)
				||
				(0x1F == fhFile.FileSpecific.EF.umbSfi)) {

				/*Invalid Params in datafield*/
				DBG_ERR("[102_222] _102222_createFile:  Invalid SFI i.e. '00'\n");
				DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
				THROW(EXCEP_INCORRECT_DATAFIELD);
			}

			DBG_FUNC("[102_222] _102222_createFile: Checking if SFID already exists \n");

			/*Check if SFID exists*/
			_7816_4_ifFileExists(fhFile.FileSpecific.EF.umbSfi, pccCurrentChannel->fhaCurrentDf, MODE_FIND_SFID, &fhaFile);

			if (NO_FILE_PRESENT != fhaFile) {
				/*SFI alread exists*/
				DBG_FUNC("[102_222] _102222_createFile: SFID already exists \n");
				THROW(EXCEP_FILE_ID_ALREADY_EXISTS);
			}

		}
		else {
			/*SFI is not supported for current EF */
			fhFile.FileSpecific.EF.umbSfi = SFI_NOT_SUPPORTED;
		}

		/*Check for Prop. TLV A5*/
		if (TAG_PROP_TLV == COMMAND_DATA[umbFcpIndex]){

			DBG_FUNC("[102_222] _102222_createFile: Prop. TLV 'A5' present\n");

			/*Skip to length of the TLV*/
			umbFcpIndex++;

			/*check length
			* Assuming only C0 TLV is supported in OS
			*/
			_102222_matchValues(LENGTH_PROP_TLV, COMMAND_DATA[umbFcpIndex++]);
			/*Check for special file TLV C0 */
			if ( TAG_SPECIAL_FILE_INFO==COMMAND_DATA[umbFcpIndex]) {


				DBG_FUNC("[102_222] _102222_createFile: Checking Special File Info. TLV \n");

				/*Check Special File Info Tag*/
				_102222_matchValues(TAG_SPECIAL_FILE_INFO, COMMAND_DATA[umbFcpIndex++]);

				/*Check Special File Info Length */
				_102222_matchValues(LENGTH_SPECIAL_FILE_INFO, COMMAND_DATA[umbFcpIndex++]);

				/*Check if value is valid one*/
				if (COMMAND_DATA[umbFcpIndex] & MASK_SPECIAL_FILE_INFO_RFU_BITS)
				{
					/*Invalid datafield incorrect error*/
					DBG_ERR("[102_222] _102222_createFile:  Invalid Special file infor '%1'\n", COMMAND_DATA[umbFcpIndex]);
					DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
					THROW(EXCEP_INCORRECT_DATAFIELD);
				}

				/*check high update activity*/
				if (IS_HIGH_UPDATE(COMMAND_DATA[umbFcpIndex])) {

					DBG_FUNC("[102_222] _102222_createFile: Given File is highly updatable file \n");
					/*set high update flag*/
					fhFile.FileSpecific.EF.umbUpdAct = HIGHLY_UPDATE_FILE;
				}

				/*check Read/Update when deactivate*/
				if (IS_FILE_RW_WHEN_DEACTIVATE(COMMAND_DATA[umbFcpIndex])) {

					DBG_FUNC("[102_222] _102222_createFile: File can be read/update when deactivated \n");
					/*Set Read/Update when deactivated flag*/
					fhFile.FileSpecific.EF.umbReadUpdateWhenDeactivated = READABLE_UPDATABLE_WHEN_DEACTIVATED;
				}

			}
			/*Check for OTA FILE TLV CC*/
			if (TAG_OTA_FILE_INFO == COMMAND_DATA[umbFcpIndex]) {
				/*This is the last TLV in CREATE command and other TLVs occurring after this shall be ignored*/
				

				/*Check OTA File Info Tag*/
				_102222_matchValues(TAG_OTA_FILE_INFO, COMMAND_DATA[umbFcpIndex++]);

				/*Check OTA File Info Length */
				_102222_matchValues(LENGTH_OTA_FILE_INFO, COMMAND_DATA[umbFcpIndex++]);
				     
				
				/*check OTA updatable */
				if (IS_FILE_OTA_UPDATABLE(COMMAND_DATA[umbFcpIndex])) {

					DBG_FUNC("[102_222] _102222_createFile: OTA can be updatable  \n");
					/*Set OTA updatable  flag*/
					fhFile.FileSpecific.EF.umbOtaUpdateable = OTA_UPDATABLE;
				}
				else {
					DBG_FUNC("[102_222] _102222_createFile: OTA can Not be updatable  \n");
					/*Set OTA NOT updatable  flag*/
					fhFile.FileSpecific.EF.umbOtaUpdateable = OTA_NOT_UPDATABLE;
				}
			}


		}// End of Prop. TLV handling

	}// End of EF

	/*Save Current DF inside parent of incoming file creation header*/
	fhFile.fhaParent = pccCurrentChannel->fhaCurrentDf;

	DBG_FUNC("[102_222] _102222_createFile: Check if incoming file id exists or not\n");

	/*Search for file ID under DF*/
	_7816_4_ifFileExists(fhFile.udbFileId, pccCurrentChannel->fhaCurrentDf, MODE_FIND_FID, &fhaFile);

	/*if File exists*/
	if (NO_FILE_PRESENT != fhaFile)
	{
		/*File already exists*/
		DBG_ERR("[102_222] _102222_createFile: File Already exists \n");
		DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
		THROW(EXCEP_FILE_ID_ALREADY_EXISTS);
	}

	DBG_FUNC("[102_222] _102222_createFile: File doesnt exist\n");

	/*Check for Type of file to create*/
	if (umbAdfOffset) { //Incoming file creation is for ADF

		/*ensure current DF is MF*/
		if (_7816_4_getMfAddress() != pccCurrentChannel->fhaCurrentDf) {

			/*Command not allowed in case of ADF creation*/
			DBG_ERR("[102_222] _102222_createFile: MF must be current DF while creating ADF \n");
			DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
			THROW(EXCEP_COMMAND_NOT_ALLOWED);
		}
		DBG_FUNC("[102_222] _102222_createFile: Checking if DF name exists or not\n");

		/*search for incoming ADF using DF Name*/
		umbFileRecord = _7816_4_ifAdfExists(&COMMAND_DATA[umbAdfOffset], &fhaFile);

		/*if ADF exists*/
		if (NO_FILE_PRESENT != fhaFile)
		{
			/*ADF already exists*/
			DBG_ERR("[102_222] _102222_createFile: ADF Already exists \n");
			DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
			THROW(EXCEP_DF_NAME_ALREADY_EXISTS);
		}
		DBG_FUNC("[102_222] _102222_createFile:DF name doesnt exist\n");

		if (RECORD_UNDEFINED == umbFileRecord) //check if returned record no. is undefined
		{
			/*if control is here it means, there is no space in ADF list file to add 1 more record*/
			DBG_ERR("[102_222] _102222_createFile: No empty records available in ADF List file: 6F10 \n");
			DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
			THROW(EXCEP_NOT_ENOUGH_MEMORY);
		}

	}

	if(0x00 == umbIsDf){ //if its EF

		DBG_FUNC("[102_222] _102222_createFile: Allocate File data of size '%2'\n", udbFileSize);

		/*Create data of EF*/
		maFileData = mm_alloc(udbFileSize, DEFAULT_102_222_BYTE);

		if (NO_FREE_MEMORY == maFileData) //if no object returned
		{
			/*No free memory available*/
			DBG_ERR("[102_222] _102222_createFile: No enough memory for File data Required: '%2' Available: '%4' \n", udbFileSize, mm_GetAvailableMemory());
			DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
			THROW(EXCEP_NOT_ENOUGH_MEMORY);
		}

		/*mamory address of file data*/
		fhFile.FileSpecific.EF.maFileData = maFileData;

	}

	DBG_FUNC("[102_222] _102222_createFile: Creating file header\n");

	/*Create File header*/
	fhaFile = _7816_9_createFileHeader(pccCurrentChannel->fhaCurrentDf, fhFile);

	if (EMPTY_OBJECT == fhaFile) //Check null/empty object returned, it means not enough memory
	{
		DBG_ERR("[102_222] _102222_createFile: Not enough memory for file header.  Required: %2 Available: '%4'\n", sizeof(fhFile), mm_GetAvailableMemory());
		/*Free file data memory if allocated*/
		mm_Free(maFileData);

		DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
		/*No free memory available*/
		THROW(EXCEP_NOT_ENOUGH_MEMORY);
	}
	DBG_FUNC("[102_222] _102222_createFile: File header created and added in File system\n");

	if (umbIsDf) { // Check if its DF

		if (FID_MF == fhFile.udbFileId) {//check if incoming file is MF update the system page

			DBG_FUNC("[102_222] _102222_createFile: This is MF, so updating its address on system page \n");
			/*Write the MF header address from system page*/
			baseWriteNBytes(ADDRESS_MASTER_FILE, (puMonoByte)&fhaFile, SIZE_OF_FILE_HEADER_ADDR);
		}
		/*Set Current DF*/
		pccCurrentChannel->fhaCurrentDf = fhaFile;
		fhaFile = NO_FILE_PRESENT;
	}

	/*Set Current File*/
	pccCurrentChannel->fhaCurrentEf = fhaFile;
	
	/*As file selection happened, record is undefined now*/
	pccCurrentChannel->umbCurrentRecPtr = RECORD_UNDEFINED;

	/*If its ADF*/
	if (umbAdfOffset) {

		DBG_FUNC("[102_222] _102222_createFile: Adding DF name in ADF list file as we have successfully created ADF\n");

		/*Assuming All prop. files must be present with intended file attributes,
		* so no extra checks are added for validation of prop file*/
		/*get memory reference of Admin use DF*/
		_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

		/*get memory reference of ADF list ef*/
		_7816_4_ifFileExists(FID_PROP_ADF_LIST, fhaFile, MODE_FIND_FID, &fhaFile);

		/*Read file header*/
		mm_ReadUpdate(fhaFile, (puMonoByte) &fhFile, OFFSET_START, sizeof(fhFile), MEM_MODE_READ);

		/*Read memory address of file data*/
		maFileData = fhFile.FileSpecific.EF.maFileData;

		/*Update File Header Address*/
		mem_cpy(COMMAND_DATA + umbAdfOffset + OFFSET_ADDR_FILE_HEADER, (puMonoByte)&(pccCurrentChannel->fhaCurrentDf), SIZE_OF_FILE_HEADER_ADDR);

		/*Read file data*/
		mm_ReadUpdate(maFileData, COMMAND_DATA + umbAdfOffset, (umbFileRecord - 1) * LENGTH_RECORD_ADF_LIST, \
			LENGTH_RECORD_ADF_LIST, MEM_MODE_UPDATE);
	}

	/*exit from the function*/
	DBG_FUNC_EXIT("<<[102_222] _102222_createFile()\n");
}