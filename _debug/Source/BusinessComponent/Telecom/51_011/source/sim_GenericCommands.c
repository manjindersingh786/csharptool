#include "51_011.h"


/*File characteristics*/
/*For clock infor b1-b3-b4 is used. 
* current value is Clock stop not allowed i.e. b1 = 0, b3 = 0 and b4 = 0
*/
#define FC_CLOCK_STOP_NOT_ALLOWED							(0x00)

/*For running the authentication algorithm, or the ENVELOPE command for SIM Data Download, a frequency is required of at least 13/8 MHz if b2=0 and 13/4 MHz if b2=1
* Current Value: frequency is required of at least 13/8 MHz i.e. b2 = 0
*/
#define FC_LOW_FREQUENCY									(0x00)

/*supply voltage indication
* Current value : 1.8V Technology SIM i.e b7-b6-b5 = 0-1-1
*/
#define FC_SIM_TECH_1_8_V									(0x30)

/* CHV1 Enable/disable i.e. b8 = 0/1 respectively*/
#define FC_CHV1_ENABLED										(0x00)
#define FC_CHV1_DISABLED									(0x80)

/*No of PINs*/
#define NO_OF_CHV											(0x02)
#define NO_OF_UNBLOCK_CHV									(NO_OF_CHV)
#define NO_OF_ADM											(0x0A) //right now UICC has 10 ADMs so we are are using same in SIM also.

/*Status Byte of Secret code*/
#define SECRET_CODE_INITIALISED								(0x80)

/*Detail 3*/
#define	INCREASE_COMMAND_ALLOWED							(0x40)
#define	INCREASE_COMMAND_NOT_ALLOWED						(0x00)

/*File Status*/
#define FILE_STATUS_INVALIDATED								(0x00)
#define FILE_STATUS_NOT_INVALIDATED							(0x01)

#define FILE_STATUS_RW_WHEN_INVALIDATED						(0x04)
#define FILE_STATUS_NOT_RW_WHEN_INVALIDATED					(0x00)


// RUN GSM Algo
#define LENGTH_CIPHERING_KEY								(0x08)
#define LENGTH_SRES											(0x04)

//GSM file response
#define MAX_SIM_RESPONSE_DF									(0X16)
#define MAX_SIM_RESPONSE_EF									(0X0F)


/*Structure of file*/
typedef enum {
	EF_STRUCT_TRANSPARENT = 0x00,
	EF_STRUCT_LINEAR = 0x01,
	EF_STRUCT_CYCLIC = 0x03
}tEfStructure;

/*Access condition as per Table 10 of 51.011*/
typedef enum {					//		-	UICC Key Ref
	SIM_ACC_ALW,				// '0'	-	---
	SIM_ACC_CHV1,				// '1'	-	0x
	SIM_ACC_CHV2,				// '2'	-	8x
	SIM_ACC_ADM1 = 0x04,		// '4'	-	0A
	SIM_ACC_ADM2,				// '5'	-	0B
	SIM_ACC_ADM3,				// '6'	-	0C
	SIM_ACC_ADM4,				// '7'	-	0D
	SIM_ACC_ADM5,				// '8'	-	0E
	SIM_ACC_ADM6,				// '9'	-	8A
	SIM_ACC_ADM7,				// 'A'	-	8B
	SIM_ACC_ADM8,				// 'B'	-	8C
	SIM_ACC_ADM9,				// 'C'	-	8D
	SIM_ACC_ADM10,				// 'D'	-	8E
	SIM_ACC_ADM11,				// 'E'	-	8E
	SIM_ACC_NEV,				// 'F'
}tSimAccessCondition;

/*
* It returns access condition of given operation on present file header
* 
* @param fhaFile: Target file header
* @param amOperation: File Operation
* 
* @return tSimAccessCondition Corresponding Access condition
*/
static tSimAccessCondition _51011_getAccessCondition(tFileHeaderAddr fhaFile, tAccessMode amOperation) {

	uMonoByte		umbStatus;

	DBG_FUNC_ENTRY(">>[51_011] _51011_getAccessCondition() \n");

	/*if file not found*/
	assert(NO_FILE_PRESENT != fhaFile);

	/*get security condition i.e. PIN_** */
	umbStatus = _7816_4_getSecurityCondition(fhaFile, amOperation);

	if (PIN_NEVER == umbStatus) { // If security condition is NEVER
		umbStatus = SIM_ACC_NEV;
	}
	else if (PIN_ALWAYS == umbStatus) { // If security condition is ALWAYS
		umbStatus = SIM_ACC_ALW;
	}
	else if ((PIN_APP_1 <= umbStatus) && (umbStatus <= PIN_APP_8)) { // If its Application/Global PIN
		umbStatus = SIM_ACC_CHV1;
	}
	else if ((PIN_SECOND_APP_1 <= umbStatus) && (umbStatus <= PIN_SECOND_APP_8)) { // If its second Application PIN
		umbStatus = SIM_ACC_CHV2;
	}
	else { // ADM left 0A-0E and 8A-8E

		if (umbStatus & 0x80) { // 8A-8E
			umbStatus = (uMonoByte)(SIM_ACC_ADM6 + (umbStatus - PIN_ADM_6));
		}
		else { // 0A-0E
			umbStatus = (uMonoByte)(SIM_ACC_ADM1 + (umbStatus - PIN_ADM_1));
		}
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_getAccessCondition() returns %1\n", umbStatus);

	return umbStatus;
}

/*
* It prepare the Response of Select command for given file
* 
* @param fhaFile: File header address of the target file (DF/EF)
* @param ftFile: Type of file DF or EF
* @param pumbResponse: prepare response will be copied here
* 
* @return length of prepared response
* @precondition: fhaFile must not be empty
*/
uMonoByte _51011_prepareFileResponse(IN_ARG tFileHeaderAddr fhaFile, IN_ARG tFileType ftFile, OUT_ARG puMonoByte pumbResponse) {

	uMonoByte			umbIndex = 0x10;
	uDiByte				udbTemp;
	tFileHeader			fhFile;

	assert(fhaFile != NO_FILE_PRESENT);
	assert(pumbResponse != NULL);

	DBG_FUNC_ENTRY(">>[51_011] _51011_prepareFileResponse() \n");

	/*read header of file*/
	_7816_4_getFileHeader(fhaFile, &fhFile);

	/*1-2:RFU First two bytes RFU*/
	pumbResponse[0x00] = RFU_BYTE;
	pumbResponse[0x01] = RFU_BYTE;

	/*3-4: File size*/
	if (FILE_IS_DF == ftFile) { //if its DF

		/*Zero memory allocated to DF, as we support dynamic memory allocation*/
		pumbResponse[0x02] = 0x00;
		pumbResponse[0x03] = 0x00;

	}
	else {

		/*Extract file size of EF*/
		if (IS(TF, fhFile.umbFileDesc)) { //transparent file
			
			/*get file size*/
			udbTemp = fhFile.FileSpecific.EF.fileData.udbFileSize;

			/*14: Structure of EF (clause 9.3 of 51.011)*/
			pumbResponse[0x0D] = EF_STRUCT_TRANSPARENT;

			/*15: RFU*/
			pumbResponse[0x0E] = 0x00;
		}
		else { // Record file

			/*calculate file size of file*/
			udbTemp = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbNoOfRec * fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize;

			/*14: Structure of EF (clause 9.3 of 51.011)*/
			pumbResponse[0x0D] = (IS(LF, fhFile.umbFileDesc) ? EF_STRUCT_LINEAR : EF_STRUCT_CYCLIC);

			/*15: Record length*/
			pumbResponse[0x0E] = fhFile.FileSpecific.EF.fileData.ofRecordFile.umbRecSize;
		}

		/*copy file size in response*/
		mm_storeUdiByte(pumbResponse, 0x02, udbTemp);
	}

	/*5-6: File ID*/
	mm_storeUdiByte(pumbResponse, 0x04, fhFile.udbFileId);

	/*7: Type of file*/
	if (FID_MF == fhFile.udbFileId) { // if its MF file id
		pumbResponse[0x06] = FILE_IS_MF;
	}
	else { // DF or EF: other than MF
		pumbResponse[0x06] = ftFile;
	}

	if (FILE_IS_DF == ftFile) { //if file is DF

		DBG_FUNC("[51_011] _51011_prepareFileResponse: Requested file is a DF \n");

		/*8-12: RFU*/
		mem_set(pumbResponse + 0x07, RFU_BYTE, 0x05);

		/*13: Length of subsequent data of DF response from File Characterstics(Byte 14) to Unblock CHV2 status(Byte 22) */
		pumbResponse[0x0C] = 0x09;

		/*14: File Characterstics: CHV1*/
		if (PIN_ENABLED == _7816_4_isPinActive(MAPPED_KEY_REF_TO_CHV_1))
		{
			/*Chv1 enabled*/
			pumbResponse[0x0D] = FC_CHV1_ENABLED;
		}
		else { //CHV1 disabled

			/*Mark key as verified if its still disabled*/
			_7816_4_markKeyAsVerified(MAPPED_KEY_REF_TO_CHV_1);

			/*CHV1 disabled*/
			pumbResponse[0x0D] = FC_CHV1_DISABLED;
		}

		/*14: File Characterstics: Electrical Params*/
		pumbResponse[0x0D] |= FC_CLOCK_STOP_NOT_ALLOWED\
								| FC_LOW_FREQUENCY\
								| FC_SIM_TECH_1_8_V;


		/*15 : Count of immediate DFs */
		pumbResponse[0x0E] = 0x00;

		/*16 : Count of immediate EFs */
		pumbResponse[0x0F] = 0x00;

		/*get child of file*/
		fhaFile = fhFile.FileSpecific.DF.fhaChild;

		/*get count of no.of DFs and EFs */
		while (NO_FILE_PRESENT != fhaFile) { // continue till the last sibling
			
			if (IS(DF, _7816_4_getFileDescriptor(fhaFile))) { // if its DF

				/*15: increment the count of DFs*/
				pumbResponse[0x0E]++;
			}
			else {

				/*16: increment the count of EFs*/
				pumbResponse[0x0F]++;
			}

			/*get next file*/
			fhaFile = _7816_4_getSiblingFile(fhaFile);
		}

		DBG_FUNC("[51_011] _51011_prepareFileResponse: No. of DF and EFs: %1 %1 respectively \n", pumbResponse[0x0E], pumbResponse[0x0F]);

		/*17: No. of CHVs + Unblock CHVs + ADM codes */
		pumbResponse[0x10] = NO_OF_CHV + NO_OF_UNBLOCK_CHV + NO_OF_ADM;

		/*18: RFU*/
		pumbResponse[0x11] = RFU_BYTE;

		DBG_FUNC("[51_011] _51011_prepareFileResponse: Retreive remaining attempts of applicable PINs\n");

		/*19: CHV1 status*/
		pumbResponse[0x12] = SECRET_CODE_INITIALISED | _7816_4_remainingAttemptsOfPin(MAPPED_KEY_REF_TO_CHV_1, PIN_OP_VERIFY);
		
		/*20: UNBLOCK CHV1 status*/
		pumbResponse[0x13] = SECRET_CODE_INITIALISED | _7816_4_remainingAttemptsOfPin(MAPPED_KEY_REF_TO_CHV_1, PIN_OP_UNBLOCK);

		/*21: CHV2 status*/
		pumbResponse[0x14] = SECRET_CODE_INITIALISED | _7816_4_remainingAttemptsOfPin(MAPPED_KEY_REF_TO_CHV_2, PIN_OP_VERIFY);

		/*22: UNBLOCK CHV2 status*/
		pumbResponse[0x15] = SECRET_CODE_INITIALISED | _7816_4_remainingAttemptsOfPin(MAPPED_KEY_REF_TO_CHV_2, PIN_OP_UNBLOCK);

		/*final length to prepared response*/
		umbIndex = MAX_SIM_RESPONSE_DF;

	}
	else { //Its EF

		DBG_FUNC("[51_011] _51011_prepareFileResponse: Requested file is an EF \n");

		/*8: Detail 3 in Clause 9.2.1 of 51.011*/
		if (IS(CF, fhFile.umbFileDesc)) {
			
			DBG_FUNC("[51_011] _51011_prepareFileResponse: Its Cyclic EF \n");

			/*8: Applicablity of Increase command on Cyclic file*/
			/*As there is no separate info. OS stores at the time of cyclic file creation that Increase is allowed or not. 
			* Its assumption, that Increase is always allowed rest will be controlled from security conditions
			*/
			pumbResponse[0x07] = INCREASE_COMMAND_ALLOWED;
		}
		else { //Transparent or Linear fixed file

			DBG_FUNC("[51_011] _51011_prepareFileResponse: Its Transparent or Linear fixed EF \n");
			/*8: RFU*/
			pumbResponse[0x07] = RFU_BYTE;
		}

		DBG_FUNC("[51_011] _51011_prepareFileResponse:Retreive access conditions of different operations on EF \n");

		/*9-11: Access conditions (clause 9.3 of 51.011)*/
		/*OPTimization: instead of reading ARR record multiple times, we can read once. will improve performance*/
		/*9: Read/Seek and Update Access condition*/
		pumbResponse[0x08] = (uMonoByte)SHIFT_LEFT_BY_N_BITS(((uMonoByte)_51011_getAccessCondition(fhaFile, ACC_MODE_FILE_READ)), 0x04)\
							| _51011_getAccessCondition(fhaFile, ACC_MODE_FILE_UPDATE);
		
		/*10: Increase condition*/
		pumbResponse[0x09] = SHIFT_LEFT_BY_N_BITS(((uMonoByte)_51011_getAccessCondition(fhaFile, ACC_MODE_FILE_INCREASE)), 0x04);

		/*11: Rehabilitate and Invalidate*/
		pumbResponse[0x0A] = (uMonoByte)SHIFT_LEFT_BY_N_BITS(((uMonoByte)_51011_getAccessCondition(fhaFile, ACC_MODE_FILE_ACTIVATE)), 0x04)\
							| _51011_getAccessCondition(fhaFile, ACC_MODE_FILE_DEACTIVATE);

		/*11: Read/Seek and Update Access condition*/

		/*12: File status (clause 9.3 of 51.011)*/
		pumbResponse[0x0B] =(_102221_IS_FILE_ACTIVATED(fhFile.flState) ? FILE_STATUS_NOT_INVALIDATED : FILE_STATUS_INVALIDATED) | \
							(_102221_IS_FILE_RW_WHEN_DEACTIVATED(fhFile.FileSpecific.EF.umbReadUpdateWhenDeactivated) ? FILE_STATUS_RW_WHEN_INVALIDATED: FILE_STATUS_NOT_RW_WHEN_INVALIDATED);

		/*13: Length of subsequent data field: 2 bytes : EF structure and Record length */
		pumbResponse[0x0C] = 0x02;

		/*14,15: Already filled above in function*/

		/*final length of prepared response*/
		umbIndex = MAX_SIM_RESPONSE_EF;
	}

	DBG_FUNC_EXIT("<<[51_011] _51011_prepareFileResponse() returns %1\n", umbIndex);

	/*Return prepared length of response*/
	return umbIndex;
}

/*
* It implements the Select command as per 51011
*/
void _51011_select(void) {

	DBG_FUNC_ENTRY(">>[51_011] _51011_select() \n");
	
	if (P1_BYTE | P2_BYTE) { // check P1-P2

		/*Parameters are incorrect must be '0000'*/
		DBG_ERR("[51_011] _51011_select: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_select() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	/*check File_ID length*/
	if (P3_BYTE != LENGTH_FILE_ID) {

		DBG_ERR("[CommandRouter] cmdRout_commandRouter: Invalid P3 Byte \n");
		THROW2(EXCEP_WRONG_P3, LENGTH_FILE_ID);
	}
	/*Set P2 BYTE value to 0C before calling "_102221_select" because in UICC P2 BYTE will not be 00*/
	P2_BYTE = NO_DATA_RETURN; 
	_102221_select();

	/*
	* UICC select command either found the file or doesnt: 
	* If it doesnt found the file, exception will be sent and control will never come here
	* If it does found the file, control will come here.
	* Hence at this point of code we can assume that file has been found.
	* Now check if its EF or DF
	*/
	if (NO_FILE_PRESENT != pccCurrentChannel->fhaCurrentEf) { // If EF is selected

		/*Prepare Select command's response for EF*/
		gudbOutputLen = _51011_prepareFileResponse(pccCurrentChannel->fhaCurrentEf, FILE_IS_EF, COMMAND_RESPONSE_DATA);
	}
	else { // else DF is selected

		/*Prepare Select command's response for DF*/
		gudbOutputLen = _51011_prepareFileResponse(pccCurrentChannel->fhaCurrentDf, FILE_IS_DF, COMMAND_RESPONSE_DATA);
	}
	
	DBG_FUNC_EXIT("<<[51_011] _51011_select() \n");
}

/*
* It implements the Status command as per 51011
*/
void _51011_status(void) {

	DBG_FUNC_ENTRY(">>[51_011] _51011_status() \n");

	if (P1_BYTE || P2_BYTE) { // check P1-P2

		/*Parameters are incorrect must be '0000'*/
		DBG_ERR("[51_011] _51011_status: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_status() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}
	if (P3_BYTE > MAX_SIM_RESPONSE_DF ) {// check if P3 is more than DF's response length
		/* Wrong P3*/
		DBG_ERR("[51_011] _51011_status: Wrong P3\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_status() \n");
		THROW2(EXCEP_WRONG_P3, MAX_SIM_RESPONSE_DF);
	}

	_102221_status();



	/*Prepare Select command's response for DF*/
	gudbOutputLen = _51011_prepareFileResponse(pccCurrentChannel->fhaCurrentDf, FILE_IS_DF, COMMAND_RESPONSE_DATA);


	DBG_FUNC_EXIT("<<[51_011] _51011_status() \n");

}

/*
* It implements the Sleep command as per 51011
*/
void _51011_sleep(void) {

	DBG_FUNC_ENTRY(">>[51_011] _51011_sleep() \n");

	if (P1_BYTE || P2_BYTE) {
		/*Parameters are incorrect must be '0000'*/
		DBG_ERR("[51_011] _51011_sleep: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_sleep() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE) {
		/*P3 must be 00*/
		DBG_ERR("[51_011] _51011_sleep: Wrong P3\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_sleep() \n");
		THROW2(EXCEP_WRONG_P3,0x00);
	}
	/*Though this command is obsolete, as it is only used by Phase 1 MEs
	* Just for backward compatibility, phase 2 or later MEs always return "Normal ending"
	*/

	DBG_FUNC_EXIT("<<[51_011] _51011_sleep() \n");
}

/*
* It implements RUN GSM algorithm command as per ETSI 51.011
*/
void _51011_runGsmAlgorithm(void) {

	uMonoByte umbKi[LENGTH_AUTH_KEY];
	tFileHeaderAddr fhaAuthKi;

	DBG_FUNC_ENTRY(">>[51_011] _51011_runGsmAlgorithm() \n");

	if (P1_BYTE || P2_BYTE) {
		/*Parameters are incorrect must be '0000'*/
		DBG_ERR("[51_011] _51011_runGsmAlgorithm: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_runGsmAlgorithm() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	if (P3_BYTE != LENGTH_CHALLENGE_DATA) {

		/*Random data or challenge must be present of 16 bytes*/
		DBG_ERR("[51_011] _51011_runGsmAlgorithm: Wrong P3 %1\n", P3_BYTE);
		DBG_FUNC_EXIT("<<[51_011] _51011_runGsmAlgorithm() \n");
		THROW2(EXCEP_WRONG_P3, LENGTH_CHALLENGE_DATA);
	}

	if (FID_GSM != _7816_4_getFileId(pccCurrentChannel->fhaCurrentDf)) { // if current DF is not 7F20

		/*Its mandatory to select DFgsm for RUN GSM ALGORITHM*/
		DBG_ERR("[51_011] _51011_runGsmAlgorithm: Current DF must be GSM\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_runGsmAlgorithm() \n");
		THROW(EXCEP_CONDITIONS_OF_USE_NOT_SATISFIED);
	}

	if (ACC_NOT_VERIFIED == _7816_4_isKeyRefVerified(MAPPED_KEY_REF_TO_CHV_1)) { // If CHV1 is not verified

		/*CHV1 mandatory to be verified for Network Authentication*/
		DBG_ERR("[51_011] _51011_runGsmAlgorithm: CHV1 is not verified\n");
		DBG_FUNC_EXIT("<<[51_011] _51011_runGsmAlgorithm() \n");
		THROW(EXCEP_SECURITY_NOT_SATISFIED);
	}

	/*Get file address of Prop. DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaAuthKi);

	assert(fhaAuthKi != NO_FILE_PRESENT);

	/*Get file address of Prop. Auth Ki file*/
	_7816_4_ifFileExists(FID_PROP_AUTH_KI, fhaAuthKi, MODE_FIND_FID, &fhaAuthKi);

	assert((fhaAuthKi != NO_FILE_PRESENT) && (LENGTH_AUTH_KEY == _7816_4_getFileSize(fhaAuthKi)));

	/*Read the auth Ki from prop. file*/
	_7816_4_readUpdateFile(fhaAuthKi, umbKi, OFFSET_START, LENGTH_AUTH_KEY, ACC_MODE_FILE_READ);

	DBG_FUNC("[51_011] _51011_runGsmAlgorithm: Ki been read from file AUTH FILE '6F13'\n");

	/*Call comp-128 algorithm*/
	fvComp128Three(COMMAND_DATA, umbKi, COMMAND_RESPONSE_DATA);

	/*Send prepared Response*/
	gudbOutputLen = LENGTH_SRES + LENGTH_CIPHERING_KEY;

	DBG_FUNC_EXIT("<<[51_011] _51011_runGsmAlgorithm() \n");
}