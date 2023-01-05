#include "102_225.h"

//Variables definition


/*It parse the data as per 102 225
*
* @param umbUDL	contain User data length
* @param pumbSmsData SMS data
* @return record no. where TAR found
*/


uMonoByte _102225_VerifySecurityCommandHeader(IN_ARG uMonoByte umbUDL, IN_ARG puMonoByte pumbSmsData) {

	uDiByte				udbCPL;
	uDiByte				udbOutputLen; // As far as data is stored by the algo, it stores that offset and it also use for checksum calculation
	uDiByte				udbDecDataLength; //it is use for calc of decrypted data len and further it also use for secure data len calc. with and without padding
	uMonoByte		  	umbIV[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //This value will be further changed by the algo
	uMonoByte			umbCHL;
	uMonoByte			umbSPI1;
	uMonoByte			umbSPI2;
	uMonoByte			umbKIcAlgo; // this variable is used for storing KIc byte and also used for algo info.
	uMonoByte			umbKIDAlgo; // this variable is used for storing KID byte and also used for algo info.
	uMonoByte			umbKVNofKIc;
	uMonoByte			umbKVNofKID;
	uMonoByte			umbFileRecord;
	uMonoByte			umbTotalNoOfRecord;
	uMonoByte			umbCPLtoTAR[LENGTH_CPL_TO_TAR];
	uMonoByte			umbRfmPr[RECORD_LENGTH_RFMPR];
	uMonoByte			umbKeyValueKIc[FILE_LENGTH_KEY_VALUE];
	uMonoByte			umbKeyValueKID[FILE_LENGTH_KEY_VALUE];
	uMonoByte			umbIndex = 0x00;
	uMonoByte			umbAlgoKeyidKVN[OFFSET_LENGTH];
	uMonoByte			umbChecksum[0x40];
	uMonoByte			umbRSC = 0x00;
	tFileHeaderAddr		fhaFile;
	tChecksumObject		cChecksumObj;
	tSignatureObject	sSignatureObj;
	tBlockCipherObject  bcCipherObj;
		
	
	DBG_FUNC_ENTRY(">>[102_225] _102225_VerifySecurityCommandHeader() \n");
	/*think twice when use the pumbSmsData buffer else where*/
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: user data header length is %1\n", pumbSmsData[OFFSET_UDHL]);

	/*if command packet is not a single SMS PP*/
	if ((USIM_TOOLKIT_SECURITY_HEADER != pumbSmsData[OFFSET_CPI])
		||
		(0x00 != pumbSmsData[OFFSET_CPI_LEN])) {

		DBG_ERR("[102_225] _102225_VerifySecurityCommandHeader: IEIa must be set to '70' for a Single SMS-PP and its length IEIDLa must be set to '00'.\n");
	
		THROW(EXCEP_NORMAL_ENDING);
	}

	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: Information Element Identifier is %1(USIM toolkit security header)\n", pumbSmsData[OFFSET_CPI]);


	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: length of Information Element Identifier is %1\n", pumbSmsData[OFFSET_CPI_LEN]);

	udbCPL = mm_makeUdiByte(pumbSmsData, OFFSET_CPL);
	
	/*Check to compare if CPL is valid corresponding to UDL*/
	if ((umbUDL - (LENGTH_UDHL + pumbSmsData[OFFSET_UDHL] + LENGTH_CPL)) != udbCPL) {

		DBG_ERR("[102_225] _102225_VerifySecurityCommandHeader: command packet length is WRONG\n");
		THROW(EXCEP_TECHNICAL_PROBLEM);
	}

	/*calculate CHL which will further use for secure data len calculation*/
	umbCHL = pumbSmsData[OFFSET_CHL];

	/*Check to compare if CHL is more than 13 bytes. SPI1 to PADD CNTR*/
	if (MIN_CHL > umbCHL)
	{
		DBG_ERR("[102_225] _102225_VerifySecurityCommandHeader: command header length is WRONG\n");
		THROW(EXCEP_NORMAL_ENDING);
	}
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: command header length is %1\n", umbCHL);

	/*store SPI1 in umbSPI1*/
	umbSPI1 = pumbSmsData[OFFSET_SPI1];
	
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: SPI1 byte is %1\n", umbSPI1);
	
	/*store SPI2 in umbSPI2*/
	umbSPI2 = pumbSmsData[OFFSET_SPI2];
	PROACTIVE_BUFF[0x00] = umbSPI2;
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: SPI2 byte is %1\n", umbSPI2);

	/*store KIc in umbKIcAlgo*/
	umbKIcAlgo = pumbSmsData[OFFSET_KIC];
	PROACTIVE_BUFF[0x01] = umbKIcAlgo;
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: KIc byte is %1\n", umbKIcAlgo);
	umbKVNofKIc = SHIFT_RIGHT_BY_N_BITS(umbKIcAlgo, 0x04);

	/*store KID in umbKIDAlgo*/
	umbKIDAlgo = pumbSmsData[OFFSET_KID];
	PROACTIVE_BUFF[0x02] = umbKIDAlgo;
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: KID byte is %1\n", umbKIDAlgo);
	umbKVNofKID = SHIFT_RIGHT_BY_N_BITS(umbKIDAlgo, 0x04);

	
   /*TAR VALIDATION*/

	/*store TAR value in PROACTIVE_BUFF*/
	mem_cpy(PROACTIVE_BUFF + 0x03, pumbSmsData + OFFSET_TAR, LENGTH_TAR);

   /*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	/*get memory reference of RFM Parameter EF*/
	_7816_4_ifFileExists(FID_PROP_RFM_PARAMETER, fhaFile, MODE_FIND_FID, &fhaFile);

	umbTotalNoOfRecord = _7816_4_SearchRecordFile(fhaFile,					//Address of file header
												&pumbSmsData[OFFSET_TAR],	//contains search pattern
												LENGTH_TAR,			//search pattern length
												SEARCH_RECORD_FORWARD,		//Record no. from where search starts
												SEARCH_FIRST_OCCURRENCE,	//forward or backward
												RECORD_FIRST,				//Search occurrence: First or ALL
												FILE_OFFSET_TAR_IN_RFMPR,			//absoulte position in record for search
												&umbFileRecord);


	/*if pattern matched with any record*/
	if (0x00 != umbTotalNoOfRecord)
	{
		
		DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: Found in Record No. %1\n", umbFileRecord);

		_7816_4_readUpdateRecordFile(fhaFile, umbRfmPr, umbFileRecord, ACC_MODE_FILE_READ);

		DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: MSL is %1\n", umbRfmPr[OFFSET_MSL]);

		/*get memory reference of Admin use DF*/
		_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

		/*get memory reference of Secured Message EF*/
		_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFile, MODE_FIND_FID, &fhaFile);

		/*store UDHL to TAR*/
		_7816_4_readUpdateFile(fhaFile, pumbSmsData + OFFSET_UDHL, (OFFSET_SECURED_MESSAGE + LEN_SMS_TPDU_TO_UDL), LENGTH_UDHL_TO_TAR, ACC_MODE_FILE_UPDATE);

		/*validate SPI1 acc. to ts_102226 clause 8.2.1.3.2.4.2 */
		if (((MASK_ALGO_1 & umbSPI1) < (MASK_MSL_1 & umbRfmPr[OFFSET_MSL])) //if SPI1.b2b1 is less than MSLD.b2b1
			||
			((CIPHERING_SPI1 & umbSPI1) < (CIPHERING_SPI1 & umbRfmPr[OFFSET_MSL])) //if SPI1.b3 is less than MSLD.b3
			||
			((MASK_MSL_2 & umbSPI1) < (MASK_MSL_2 & umbRfmPr[OFFSET_MSL]))) //if SPI1.b5b4 is less than MSLD.b5b4

		{

			DBG_ERR("[102_225] _102225_VerifySecurityCommandHeader: insufficient security level\n");
			umbRSC = INSUFFICIENT_SECURITY_LEVEL;
			DBG_FUNC_EXIT("<<[102_225] _102225_VerifySecurityCommandHeader() \n");
			return umbRSC;
		}
	}
	else {
		DBG_ERR("[102_225] _102225_VerifySecurityCommandHeader: given TAR is not available\n");
		umbRSC = TAR_UNKNOWN;
		/*set CNTR value to 00 in PROACTIVE_BUFF*/
		//mem_set(PROACTIVE_BUFF + 0x06, 0x00, LENGTH_CNTR);
		DBG_FUNC_EXIT("<<[102_225] _102225_VerifySecurityCommandHeader() \n");
		return umbRSC;
	}
	
	/*validation of SPI2 according ts_102225 clause 5.1.1*/
	if ((NO_RC_CC_DS != (MASK_SPI2_CHECKSUM & umbSPI2))
		&&
		((MASK_ALGO_1 & umbSPI1) != SHIFT_RIGHT_BY_N_BITS((MASK_SPI2_CHECKSUM & umbSPI2), 0x02))
		||
		/*TODO : add check for successfull authentication using CC or RC*/
		(CIPHERING_SPI2 == (CIPHERING_SPI2 & umbSPI2))
		&&
		(CIPHERING_SPI1 != (CIPHERING_SPI1 & umbSPI1))) {

		DBG_ERR("[102_225] _102225_VerifySecurityCommandHeader: unidentified security error\n");
		umbRSC = UNIDENTIFIED_SECURITY_ERROR;
		DBG_FUNC_EXIT("<<[102_225] _102225_VerifySecurityCommandHeader() \n");
		return umbRSC;
	}


	/*validation of KVN acc. to	ts_102225 - Annex A - clause A.2*/
	if ((MASK_KVN_VALIDATION != umbKVNofKIc) && (MASK_KVN_VALIDATION != umbKVNofKID) && (umbKVNofKIc != umbKVNofKID)){

		DBG_ERR("[102_225] _102225_VerifySecurityCommandHeader: unidentified security error\n");
		umbRSC = UNIDENTIFIED_SECURITY_ERROR;
		DBG_FUNC_EXIT("<<[102_225] _102225_VerifySecurityCommandHeader() \n");
		return umbRSC;
	}
	
	/*check if SPI1 indicate redundancy check or cryptographic checksum or NO_RC_CC_DS */
	if ((REDUNDANCY_CHECK == (umbSPI1 & MASK_ALGO_1))
		||
		(CRYPTOGRAPHIC_CHECKSUM == (umbSPI1 & MASK_ALGO_1))
		||
		(NO_RC_CC_DS == (umbSPI1 & MASK_ALGO_1))
		) {

		/*validate KIc Byte according to TS_102225*/
		umbAlgoKeyidKVN[OFFSET_KEYID] = KIC;
		umbAlgoKeyidKVN[OFFSET_KVN] = umbKVNofKIc;

		if (TRUE != _102225_validateKIcAndKID(&umbKIcAlgo, umbAlgoKeyidKVN, umbKeyValueKIc)) {
			
			THROW(EXCEP_NORMAL_ENDING);
		}
		DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: KIC byte validated successfully\n");

		/*if SPI1 indicating cryptographic checksum is used*/
		if (CRYPTOGRAPHIC_CHECKSUM == (umbSPI1 & MASK_ALGO_1))
		{
			umbAlgoKeyidKVN[OFFSET_KEYID] = KID_CC;
		}
		else if (REDUNDANCY_CHECK == (umbSPI1 & MASK_ALGO_1))/*if SPI1 indicating redundancy check is used*/
		{
			umbAlgoKeyidKVN[OFFSET_KEYID] = KID_RC;
		}
		else {//NO_RC_CC_DS
			umbAlgoKeyidKVN[OFFSET_KEYID] = NO_RC_CC_DS_KID;
		}
		umbAlgoKeyidKVN[OFFSET_KVN] = umbKVNofKID;
	
		/*validate KID Byte according to TS_102225*/
		/*in the case of RC, only KID byte validated not kvn in _102225_validateKIcAndKID*/
		if (TRUE != _102225_validateKIcAndKID(&umbKIDAlgo, umbAlgoKeyidKVN, umbKeyValueKID)) {
			
			THROW(EXCEP_NORMAL_ENDING);
		}
		DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: KID byte validated successfully\n");

	}

	DBG_PRINT_ARRAY(umbKeyValueKIc, FILE_LENGTH_KEY_VALUE);
	DBG_PRINT_ARRAY(umbKeyValueKID, FILE_LENGTH_KEY_VALUE);
		
	/*store the bytes from CPL to TAR in a array umbCPLtoTAR before using the COMMAND_DATA array*/
	mem_cpy(umbCPLtoTAR, pumbSmsData + OFFSET_CPL, LENGTH_CPL_TO_TAR);

	/*calculate the length of data*/
	udbDecDataLength = (udbCPL - 0x08);

	if (CIPHERING_SPI1 == (CIPHERING_SPI1 & umbSPI1)) {
		/*decryption of data and use same buffer to store decrypted data*/

		algo_bc_init(&bcCipherObj, umbKIcAlgo, OP_DECRYPT, umbKeyValueKIc);

		udbOutputLen = algo_bc_update(&bcCipherObj, pumbSmsData + OFFSET_CNTR, udbDecDataLength, COMMAND_DATA);

		algo_bc_finalize(&bcCipherObj, pumbSmsData + (OFFSET_CNTR + udbDecDataLength), 0x00, COMMAND_DATA + udbOutputLen);
						
	}
	else {
		/*store the incoming data from CNTR to end of secure data in the COMMAND_DATA array*/
		mem_cpy(COMMAND_DATA, pumbSmsData + OFFSET_CNTR, udbDecDataLength);
	}
	DBG_PRINT_ARRAY(COMMAND_DATA, udbDecDataLength);
		
		/*CNTR validation*/

		/*store CNTR value in PROACTIVE_BUFF*/
		//mem_cpy(PROACTIVE_BUFF + 0x06, COMMAND_DATA + OFFSET_DEC_CNTR, LENGTH_CNTR);

		if ((CNTR_HIGHER_ONLY  == (MASK_SPI1_B5B4 & umbSPI1))
			||
			(CNTR_ONE_HIGHER   == (MASK_SPI1_B5B4 & umbSPI1))
			||
			(CNTR_NO_AVAILABLE == (MASK_SPI1_B5B4 & umbSPI1))
			||
			(CNTR_AVAILABLE    == (MASK_SPI1_B5B4 & umbSPI1))
			) {
					
		
			umbRSC = _102225_validationOfCNTR(umbSPI1, COMMAND_DATA, umbRfmPr);
			/*check if CNTR is not verified*/
			if (POR_OK != umbRSC) {

				DBG_FUNC_EXIT("<<[102_225] _102225_VerifySecurityCommandHeader() \n");
				/*store CNTR*/
				_7816_4_readUpdateFile(fhaFile, COMMAND_DATA + OFFSET_DEC_CNTR, (OFFSET_SECURED_MESSAGE + LENGTH_UDHL_TO_TAR + LEN_SMS_TPDU_TO_UDL), LENGTH_CNTR, ACC_MODE_FILE_UPDATE);
				return umbRSC;
			}
			/*store CNTR*/
			_7816_4_readUpdateFile(fhaFile, COMMAND_DATA + OFFSET_DEC_CNTR, (OFFSET_SECURED_MESSAGE + LENGTH_UDHL_TO_TAR + LEN_SMS_TPDU_TO_UDL), LENGTH_CNTR, ACC_MODE_FILE_UPDATE);
			DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: CNTR verified successfully\n");

		}

	/*check padding CNTR*/
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: No. of Padding octets in command packet is %1\n", COMMAND_DATA[OFFSET_DEC_PADD_CNTR]);

	/*checksum calculation and validation*/
	
	/*calculate the checksum length*/
	udbOutputLen = (umbCHL - MIN_CHL);
	
	/*calculate the length of secured data with padding*/
	udbDecDataLength = (udbDecDataLength - (0x06 + udbOutputLen));	
	
	/*calling algo*/

	/*check if SPI1 indicate redundancy check*/
	if (REDUNDANCY_CHECK == (umbSPI1 & MASK_ALGO_1)) {
		
		algo_chksm_init(&cChecksumObj, umbKIDAlgo);

		algo_chksm_update(&cChecksumObj, umbCPLtoTAR, LENGTH_CPL_TO_TAR);

		algo_chksm_update(&cChecksumObj, COMMAND_DATA + OFFSET_DEC_CNTR, 0x06);

		algo_chksm_finalize(&cChecksumObj, COMMAND_DATA + (OFFSET_DEC_CHECKSUM + udbOutputLen), udbDecDataLength, umbChecksum);

	}
	else if (NO_RC_CC_DS == (umbSPI1 & MASK_ALGO_1)) {
		DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader:NO_RC_CC_DS for CHECKSUM  \n");
	}
	else {/*if SPI1 indicate cryptographic checksum*/

		  /*if algo defined is 3DES*/
		if (ALGO_3DES == umbAlgoKeyidKVN[OFFSET_ALGO]) {
			umbKIDAlgo = SIGN_DES_MAC;
		}
		else {/*if algo defined is AES*/
			umbKIDAlgo = SIGN_AES_CMAC;
		}
		algo_sign_init(&sSignatureObj, umbKIDAlgo, (uMonoByte)udbOutputLen, umbKeyValueKID);

		algo_sign_update(&sSignatureObj, umbCPLtoTAR, LENGTH_CPL_TO_TAR);

		algo_sign_update(&sSignatureObj, COMMAND_DATA + OFFSET_DEC_CNTR, 0x06);

		algo_sign_finalize(&sSignatureObj, COMMAND_DATA + (OFFSET_DEC_CHECKSUM + udbOutputLen), udbDecDataLength, umbChecksum);
	}
	
	DBG_PRINT_ARRAY(umbChecksum, udbOutputLen);
	
	/*verifying checksum*/
	if (mem_cmp(COMMAND_DATA + OFFSET_DEC_CHECKSUM, umbChecksum, udbOutputLen)) {

	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: 'Unidentified security error'\n");
	umbRSC = RC_CC_DS_FAILED;
	DBG_FUNC_EXIT("<<[102_225] _102225_VerifySecurityCommandHeader() \n");
	return umbRSC;
	}

	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: CHECKSUM verified successfully\n");
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: All the security pararmeters verified successfully\n");

	/*update the CNTR value in the RE acc. to ts_102225 clause 5.1.4*/
	mem_cpy(umbRfmPr + OFFSET_START_CNTR, COMMAND_DATA + OFFSET_DEC_CNTR, LENGTH_CNTR);
	
	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	/*get memory reference of RFM Parameter EF*/
	_7816_4_ifFileExists(FID_PROP_RFM_PARAMETER, fhaFile, MODE_FIND_FID, &fhaFile);

	/*update the record of EF RFMPR*/
	_7816_4_readUpdateRecordFile(fhaFile, umbRfmPr, umbFileRecord, ACC_MODE_FILE_UPDATE);

	/*calculate the length of secured data without padding*/
	udbDecDataLength = (udbDecDataLength - COMMAND_DATA[OFFSET_DEC_PADD_CNTR]);
	DBG_FUNC("[102_225] _102225_VerifySecurityCommandHeader: Length of secure message without padding is %1\n", udbDecDataLength);

	/*store the END index of secure msg in prop. file secured message*/
	udbDecDataLength = (udbDecDataLength + 0x29 + udbOutputLen);
	umbCPLtoTAR[0x00] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(udbDecDataLength, 0x08);
	umbCPLtoTAR[0x01] = (uMonoByte)udbDecDataLength;

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	/*get memory reference of Secured Message EF*/
	_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFile, MODE_FIND_FID, &fhaFile);

	/*update secure message length in prop. file*/
	_7816_4_readUpdateFile(fhaFile, umbCPLtoTAR, OFFSET_SECURED_MESSAGE_LENGTH, 0x02, ACC_MODE_FILE_UPDATE);

	/*storing Padding CNTR + Checksum + secure message without padding*/
	_7816_4_readUpdateFile(fhaFile, (COMMAND_DATA + OFFSET_DEC_PADD_CNTR), OFFSET_PADD_CNTR_IN_FILE, (udbDecDataLength + udbOutputLen + 0x01), ACC_MODE_FILE_UPDATE);
	
	/*store rec. no. where TAR found in same buffer*/
	pumbSmsData[0x00] = umbFileRecord;
	/*store offset from where secure message start in prop file, in same buffer*/
	pumbSmsData[0x01] = ((OFFSET_PADD_CNTR_IN_FILE + 0x01) + udbOutputLen);
	DBG_FUNC_EXIT("<<[102_225] _102225_VerifySecurityCommandHeader() \n");
	return umbRSC;
}

/*it validates the KIc and KID byte
* @param umbData contains KIc or KID byte
* @param pumbAlgoKeyidKVN contains algo, keyid and kvn
* @param umbKeyValueLength store length of key
* @param pumbKeyValue store key value
*/
boolean _102225_validateKIcAndKID(INOUT_ARG puMonoByte umbData, INOUT_ARG puMonoByte pumbAlgoKeyidKVN, OUT_ARG puMonoByte pumbKeyValue)
{	
	uMonoByte			umbKeyValueLength;

	DBG_FUNC_ENTRY(">>[102_225] _102225_validateKIcAndKID() \n");

	/*KID for Redundancy Check*/
	if (KID_RC == pumbAlgoKeyidKVN[OFFSET_KEYID]){

		if (CRC_ALGO != (*umbData & MASK_ALGO_1)) {

			DBG_ERR("[102_225] _102225_validateKIcAndKID: Algo not Defined in SPI1\n");

			/*TODO: return RSC or SW*/
			return FALSE;
		}
		if (CRC_16 == (*umbData & MASK_ALGO)) {

			*umbData = CHECKSUM_CRC16;
			return TRUE;
		}
			
		if (CRC_32 == (*umbData & MASK_ALGO)) {

			*umbData = CHECKSUM_CRC32;
			return TRUE;
			
		}
		DBG_ERR("[102_225] _102225_validateKIcAndKID: Reserved\n");
		return FALSE;
	}

	if (DES_ALGO == (*umbData & MASK_ALGO_1))		// if DES algorithm is used
	{
		if ((0x00 == (*umbData & MASK_ALGO))		//check if Reserved for DES as defined in previous releases
			||
			(0x0C == (*umbData & MASK_ALGO))) 
		{
		
		DBG_ERR("[102_225] _102225_validateKIcAndKID: Reserved\n");

		/*TODO : sending RSC or SC*/
		DBG_FUNC_EXIT("<<[102_225] _102225_validateKIcAndKID() \n");
		return FALSE;
		}

		pumbAlgoKeyidKVN[OFFSET_ALGO] = ALGO_3DES;
		*umbData = BC_DES;
		 
	}

	else if (AES_ALGO == (*umbData & MASK_ALGO_1))	  //if AES algorithm is used			
	{
		if (0x00 != (*umbData & MASK_ALGO)) //if Reserved 
		{
			DBG_ERR("[102_225] _102225_validateKIcAndKID: Reserved\n");

			/*TODO : sending RSC or SC*/
			DBG_FUNC_EXIT("<<[102_225] _102225_validateKIcAndKID() \n");
			return FALSE;
		}
		pumbAlgoKeyidKVN[OFFSET_ALGO] = ALGO_AES;
		*umbData = BC_AES;
	}
	else if (NO_ALGOKIC == (*umbData & MASK_ALGO_1)) {//if No AES or DES
		pumbAlgoKeyidKVN[OFFSET_ALGO] = NO_ALGOKIC;
		*umbData = BC_NULL;
		return TRUE;
	}
	else {
		DBG_FUNC_EXIT("<<[102_225] _102225_validateKIcAndKID() \n");
		return FALSE;
	}
		 
	umbKeyValueLength = _102225_KeyManagement(pumbAlgoKeyidKVN, pumbKeyValue + 0x01);

	pumbKeyValue[OFFSET_KEY_VALUE_LENGTH] = umbKeyValueLength;
	
	/*check if KVN not found (keyvalue length is 00)*/
	if (0x00 == umbKeyValueLength) {

		return FALSE;
	}
			
	DBG_FUNC_EXIT("<<[102_225] _102225_validateKIcAndKID() \n");
	 return TRUE;
}

/*it validates the counter
* @param uobCNTR contains incoming counter
* @param uobPreviousCNTR contains contains previous counter which stored in prop. file
* @return response status code for counter
*/
uMonoByte _102225_validationOfCNTR(IN_ARG uMonoByte umbSPI1, IN_ARG puMonoByte pumbCNTR, IN_ARG puMonoByte pumbPreviousCNTR) {

	uOctaByte           uobCNTR = 0x00;
	uOctaByte			uobPreviousCNTR = 0x00;

	/*Store the incoming CNTR value in uobCNTR*/
	uobCNTR = (SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbCNTR[OFFSET_DEC_CNTR], 32)) |
		(SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbCNTR[OFFSET_DEC_CNTR + 1], 24)) |
		(SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbCNTR[OFFSET_DEC_CNTR + 2], 16)) |
		(SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbCNTR[OFFSET_DEC_CNTR + 3], 8)) |
		((uOctaByte)pumbCNTR[OFFSET_DEC_CNTR + 4]);

	/*get the stored previous CNTR value from EF RFMPR in uobPreviousCNTR*/
	uobPreviousCNTR = (SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbPreviousCNTR[OFFSET_START_CNTR], 32)) |
		(SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbPreviousCNTR[OFFSET_START_CNTR + 1], 24)) |
		(SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbPreviousCNTR[OFFSET_START_CNTR + 2], 16)) |
		(SHIFT_LEFT_BY_N_BITS((uOctaByte)pumbPreviousCNTR[OFFSET_START_CNTR + 3], 8)) |
		((uOctaByte)pumbPreviousCNTR[OFFSET_START_CNTR + 4]);

	umbSPI1 = (MASK_SPI1_B5B4 & umbSPI1);	
	if ((CNTR_NO_AVAILABLE == umbSPI1) /*check if SPI1 indicates that No counter available*/
		||
		(CNTR_AVAILABLE    == umbSPI1))/*check if SPI1 indicates that Counter available No replay or sequence checking*/
	{
		DBG_FUNC("[102_225] _102225_validationOfCNTR:  No counter available OR  Counter available No replay or sequence checking \n");
		return POR_OK;
	}
	else {
		/*check if CNTR is blocked*/
		if (MAX_CNTR == uobPreviousCNTR) {
			DBG_FUNC("[102_225] _102225_validationOfCNTR: CNTR BLOCKED\n");
			return CNTR_BLOCKED;
		}
		/*if incoming CNTR is less than CNTR stored in RE*/
		if (uobCNTR < uobPreviousCNTR) {
			DBG_FUNC("[102_225] _102225_validationOfCNTR: CNTR LOW\n");
			return CNTR_LOW;
		}
		/*if incoming CNTR is greater than CNTR stored in RE*/
		if (uobCNTR > uobPreviousCNTR) {
			/*check if SPI1 indicates that Process if and only if counter value is higher than the value in the RE*/
			if ((CNTR_ONE_HIGHER == umbSPI1)
				&&
				((uobPreviousCNTR + 0x01) != uobCNTR)) {

				DBG_FUNC("[102_225] _102225_validationOfCNTR: CNTR HIGH\n");
				return CNTR_HIGH;
			}
			DBG_FUNC("[102_225] _102225_validationOfCNTR: CNTR verified successfully\n");
			return POR_OK;
		}
	}
	DBG_FUNC("[102_225] _102225_validationOfCNTR: CNTR not verified\n");
	return UNIDENTIFIED_SECURITY_ERROR;
}	

/*It stores key value as per 102 225
*
* @param pumbData : Algo, KeyID and KVN
* @param pumbKeyValue : Key value
*/
uMonoByte _102225_KeyManagement(IN_ARG puMonoByte pumbData, OUT_ARG puMonoByte pumbKeyValue) {

	uMonoByte			umbRecordNo;
	uMonoByte			umbFileRecord;
	uMonoByte			umbKeyManagementRecord[RECORD_LENGTH_OF_KEY_MANAGEMENT];
	tFileHeaderAddr		fhaFile;
	tFileHeaderAddr		fhaDf;

	DBG_FUNC_ENTRY(">>[102_225] _102225_KeyManagement() \n");

	/*Get file address of Prop. DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaDf);

	/*Get file address of Prop. Key Management file*/
	_7816_4_ifFileExists(FID_PROP_KEY_MANAGEMENT, fhaDf, MODE_FIND_FID, &fhaFile);

	umbRecordNo = _7816_4_SearchRecordFile(fhaFile,                            //Address of file header
											pumbData,                          //contains search pattern
											LENGTH_OF_KEY_MANAGEMENT_PATTERN,  //search pattern length
											SEARCH_RECORD_FORWARD,             //Record no. from where search starts
											SEARCH_FIRST_OCCURRENCE,           //forward or backward
											RECORD_FIRST,                      //Search occurrence: First or ALL
											INDEX_OF_ALGO,                     //absoulte position in record for search
											&umbFileRecord);

	/*chech the Algo, KeyID and KVN match or not*/
	if (0x00 != umbRecordNo)
	{
		DBG_FUNC("[102_225] _102225_KeyManagement(): Algo, KeyID and KVN Found in Record No. %1\n", umbFileRecord);
		/*raed the all record*/
		_7816_4_readUpdateRecordFile(fhaFile, umbKeyManagementRecord, umbFileRecord, ACC_MODE_FILE_READ);

		/*Get file address of Prop. Key Value file*/
		_7816_4_ifFileExists(FID_PROP_KEY_VALUE, fhaDf, MODE_FIND_FID, &fhaFile);

		/*read the key value*/
		_7816_4_readUpdateFile(fhaFile, pumbKeyValue, (umbKeyManagementRecord[INDEX_OF_BLOCK_OFFSET] *
			LENGTH_OF_BLOCK_SIZE_OF_KEY), umbKeyManagementRecord[INDEX_OF_KEY_SIZE],
			ACC_MODE_FILE_READ);

		DBG_FUNC_EXIT("<<[102_225] _102225_KeyManagement()\n");

		/*return the length of key value*/
		return umbKeyManagementRecord[INDEX_OF_KEY_SIZE];
	}
	
		DBG_FUNC("[102_225] _102225_KeyManagement(): Algo, KeyID and KVN not Found\n");
		DBG_FUNC_EXIT("<<[102_225] _102225_KeyManagement()\n");

		return FALSE;
	}


