#include "31_115.h"

//Variables definition

/*
* It parses the data acc. to 31.115
* 
* @param pumbSmsData: SMS data received from Network
*/
void _31115_SmsppImplementation(IN_ARG puMonoByte pumbSmsData, IN_ARG uMonoByte umbLenSmsData) {

	uMonoByte	umbTpMTI;
	uMonoByte	umbTpUDHI;
	uMonoByte	umbTpPID;
	uMonoByte	umbTpDCS;
	uMonoByte   umbIndex;
	uMonoByte	umbLengthOA;
	uMonoByte	umbRSC = 0x00;

	DBG_FUNC_ENTRY(">>[31_115] _31115_SmsppImplementation() \n");
	/*parsing of SMS-TPDU as per SMS-DELIVER type 23.040*/
	
	umbTpMTI = pumbSmsData[0x00] & MASK_TP_MTI; //mask for b0b1 of first TP byte

	umbTpUDHI = pumbSmsData[0x00] & MASK_TP_UDHI; //mask for b6 of first TP byte

	if ((TP_MTI_SMS_DELIVER != umbTpMTI)
		||
		(TP_UDHI_FOR_HEADER != umbTpUDHI))
	{ /*Check for invalid MTI and UDHI*/

		DBG_ERR("[31_115] _31115_SmsppImplementation: First byte of SMS TPDU Data contains %1.\nMessage type should be 'SMS-DELIVER'\nAnd, TP-UD field must contain a Header in addition to the short message\n", pumbSmsData[0x00]);
		DBG_FUNC_EXIT("<<[31_115] _31115_SmsppImplementation() \n");
		THROW(EXCEP_TECHNICAL_PROBLEM);
	}

	DBG_FUNC("[31_115] _31115_SmsppImplementation: The b0b1 indicates message type as 'SMS-DELIVER'\n");
	DBG_FUNC("[31_115] _31115_SmsppImplementation: The b6 indicates that the beginning of the TP-UD field contains a Header in addition to the short message\n");


	//umbLengthOA = (pumbSmsData[0x01] % 2) ? ((pumbSmsData[0x01] / 2) + 0x01) : (pumbSmsData[0x01] / 2); 
	umbLengthOA = (pumbSmsData[0x01] + 0x01) / 2;/*calcuating length of originating address*/

	/*adding 'length of originating address' to current index, also adding 1 to index for 'type of address' byte*/
	umbIndex = 0x02 + umbLengthOA + 0x01; //02 is current index + length of originating address + 01 for type of address

	mem_cpy(PROACTIVE_BUFF + OFFSET_TP_DA, (pumbSmsData + 0x01), 0x02 + umbLengthOA); /*storing originating address to destination adrress offset of POR*/

	umbTpPID = pumbSmsData[umbIndex]; //next byte is TP Protocol Identifier
	umbIndex++;

	umbTpDCS = pumbSmsData[umbIndex]; //next byte is TP Data Coding Scheme

	if ((TP_PID_USIM_DATA_DOWNLOAD != umbTpPID)
		||
		(TP_DCS_CLASS2_USIM != umbTpDCS))
	{ /*Check for invalid PID and DCS of USIM data download and class 2 USIM */

		DBG_ERR("[31_115] _31115_SmsppImplementation: The TP-Protocol-Identifier is %1, should be (U)SIM Data download\nAnd, The TP-Data-Coding-Scheme is %1, should be Class 2 (U)SIM-specific message\n", umbTpPID, umbTpDCS);
		DBG_FUNC_EXIT("<<[31_115] _31115_SmsppImplementation() \n");
		THROW(EXCEP_TECHNICAL_PROBLEM);
	}

	DBG_FUNC("[31_115] _31115_SmsppImplementation: The TP-Protocol-Identifier is (U)SIM Data download\n");
	DBG_FUNC("[31_115] _31115_SmsppImplementation: The TP-Data-Coding-Scheme is Class 2 (U)SIM-specific message.\n");
	umbIndex++;

	/*adding 'length of TP Time Stamp' to current index*/
	umbIndex = umbIndex + LENGTH_OF_TP_TIME_STAMP;
 
	//next byte is TP User Data Length

	if ((umbLenSmsData - (umbIndex + 0x01)) != pumbSmsData[umbIndex]) {
		/*Check for invalid TP User Data Length*/

		DBG_ERR("[31_115] _31115_SmsppImplementation: Wrong TP User Data Length, Correct will be %1\n", umbLenSmsData - (umbIndex + 0x01));
		DBG_FUNC_EXIT("<<[31_115] _31115_SmsppImplementation() \n");
		THROW(EXCEP_TECHNICAL_PROBLEM);
	}

	DBG_FUNC("[31_115] _31115_SmsppImplementation: The TP-User-Data-Length is %1 bytes\n", pumbSmsData[umbIndex]);

	//now index is at TP User Data

	/*function calling for parsing of security header data as per 102 225*/

	umbRSC = _102225_VerifySecurityCommandHeader(pumbSmsData[umbIndex], pumbSmsData + (umbIndex + 1));

	/*Offset from where SM- data starts*/
	gudbCommandIndex = pumbSmsData[umbIndex + 2];

	if (0x00 == umbRSC) {
		/*Dispatch plain data to RFM application*/
		//passed argument will store the returned record number for AID corresponding to TAR, from _102225_VerifySecurityCommandHeader
		_102226_ProcessRfm(pumbSmsData[umbIndex + 1]);
	}
	
	_31115_preparePOR(umbRSC);

	DBG_FUNC_EXIT("<<[31_115] _31115_SmsppImplementation() \n");
}

void _31115_preparePOR(IN_ARG uMonoByte umbRSC)
{
	uMonoByte			umbIndex=0x00;
	uMonoByte           udbsdbuf[0x06];
	uMonoByte           umbChecksumLen=0x00;
	uMonoByte           umbAlgo_Block;//this variable is used for store algo block size and further it again used for algo indication in encryption.
	uMonoByte           umbPadding = 0x00;
	uMonoByte			umbAlgo;
	uMonoByte			umbAlgoKeyidKVN[0x03];
	uMonoByte			umbKeyValue[FILE_LENGTH_KEY_VALUE];//it is used for key value of both kic and kid
	uMonoByte			umbChecksum[0x8C];//it is also used for storing encrypted data after encryption till DES optimization is not completed.
	uMonoByte			umbOffsetCC;
	uMonoByte			indexRPL;
	uMonoByte			indexUDL=0x00;
	uMonoByte			indexUDHL;
	uMonoByte			umbLengthOA=0x00;//it will be used for storing OA length after that it will used for storing return length of algo_bc_update.
	uMonoByte			umbOffsetCNTR;
	uDiByte				udbResSize;
	uDiByte				udbRPL;
	tFileHeaderAddr		fhaFile;
	tChecksumObject		cChecksumObj;
	tSignatureObject	sSignatureObj;
	tBlockCipherObject  bcCipherObj;

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	//get memory reference of Secured Message EF
	_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFile, MODE_FIND_FID, &fhaFile);


	/*read the SPI1,SPI2,KIC,KID for POR*/
	_7816_4_readUpdateFile(fhaFile, udbsdbuf, 0x04, 0x04, ACC_MODE_FILE_READ);

	/*if SMS SUBMIT*/
	if (udbsdbuf[0x01] & 0x20) {

		//PROACTIVE_BUFF[umbIndex++] = TAG_PROACTIVE_COMMAND;  //increment of index by 1 or 2
		PROACTIVE_BUFF[OFFSET_PROACT_TAG] = TAG_PROACTIVE_COMMAND;  //increment of index by 1 or 2

		//indexProLength = umbIndex; /*length*/
		//umbIndex++;

		//PROACTIVE_BUFF[umbIndex++] = TAG_COMMAND_DEATAILS;
		PROACTIVE_BUFF[OFFSET_CMD_DETAILS_TAG] = TAG_COMMAND_DEATAILS;

		//PROACTIVE_BUFF[umbIndex++] = LEN_COMMAND_DEATAILS;
		PROACTIVE_BUFF[OFFSET_CMD_DETAILS_LEN] = LEN_COMMAND_DEATAILS;

		//PROACTIVE_BUFF[umbIndex++] = COMMAND_NUMBER;
		PROACTIVE_BUFF[OFFSET_CMD_NO] = COMMAND_NUMBER;

		//PROACTIVE_BUFF[umbIndex++] = COMMAND_LINE;
		PROACTIVE_BUFF[OFFSET_TOC] = CMD_TYPE_SEND_SMS;

		//PROACTIVE_BUFF[umbIndex++] = COMMAND_VALUE;
		PROACTIVE_BUFF[OFFSET_CMD_QUAL] = QUALIFIER_SEND_SM_PACKAGE_NOT_REQUIRED;

		//PROACTIVE_BUFF[umbIndex++] = TAG_DEVICE_IDENTITES;
		PROACTIVE_BUFF[OFFSET_DEVICE_ID_TAG] = TAG_DEVICE_IDENTITES;

		//PROACTIVE_BUFF[umbIndex++] = LEN_DEVICE_IDENTITES;
		PROACTIVE_BUFF[OFFSET_DEVICE_ID_LEN] = LEN_DEVICE_IDENTITES;

		//PROACTIVE_BUFF[umbIndex++] = SOURCE_UICC;
		PROACTIVE_BUFF[OFFSET_DEVICE_ID_SRC] = SOURCE_UICC;

		//PROACTIVE_BUFF[umbIndex++] = SOURCE_NETWORK;
		PROACTIVE_BUFF[OFFSET_DEVICE_ID_DST] = SOURCE_NETWORK;

		/*POR of SMSPP from '8B' tag to DCS*/
		//PROACTIVE_BUFF[umbIndex] = TAG_OF_SMS_TPDU;
		PROACTIVE_BUFF[OFFSET_SMS_TPDU_TAG] = TAG_OF_SMS_TPDU;

		//umbIndex++; //now index at '8B' length byte
		/*TODO : length of 1 or 2 bytes*/
		//umbIndex++; //currently increasing 1 for length byte, now index at first octet

		//PROACTIVE_BUFF[umbIndex] = 0x00;
		//PROACTIVE_BUFF[0x0E] = 0x00;
		PROACTIVE_BUFF[OFFSET_SMS_TPDU_FIRST_BYTE] =	TP_MTI_SMS_SUBMIT /*setting MTI bits*/ \
														| TP_UDHI_FOR_HEADER /*setting UDHI bit*/ \
														| TP_RD_REJECT_DUPLICATE /*setting bit 2 as 0 for TP Reject Duplicates*/\
														| TP_VPF_NOT_PRESENT /*b3-b4 set 1-0 for TP Validity Period Format, TP Validity Period NOT present */\
														| TP_SRR_NOT_REQUESTED /*b5 setting 0 (A status report is not requested) */\
														| TP_RP_NOT_SET; /*b7 setting 0 (TP Reply Path parameter is not set in this SMS-SUBMIT) */
		//umbIndex++; //index at TP Message Reference 

		PROACTIVE_BUFF[OFFSTE_TP_MR] = TP_MR_ZERO; /*setting TP Message Reference as 00*/
		//umbIndex++; //index at destination address

		umbIndex = OFFSET_TP_DA;
		/*DESTINATION ADDRESS*/
		umbLengthOA = (PROACTIVE_BUFF[OFFSET_TP_DA] + 1) / 2;/*calcuating length of destination address*/

		umbIndex += umbLengthOA + 0x02; //now index at PID

		PROACTIVE_BUFF[umbIndex] = TP_PID_USIM_DATA_DOWNLOAD;
		umbIndex++; //now index at DCS

		PROACTIVE_BUFF[umbIndex] = TP_DCS_CLASS2_USIM;
		umbIndex++;

		indexUDL = umbIndex;

		umbIndex++;
	}
	indexUDHL = umbIndex;

	PROACTIVE_BUFF[umbIndex] = 0x02; //UDHL

	PROACTIVE_BUFF[++umbIndex] = 0x71; //RPI or IEIa

	PROACTIVE_BUFF[++umbIndex] = 0x00; //IEIDLa
							   //skip RPL and RHL
	umbIndex += 0x02;
	indexRPL = umbIndex;

	umbIndex+= 0x02; //now index at TAR

	//include TAR and CNTR
	_7816_4_readUpdateFile(fhaFile, PROACTIVE_BUFF + umbIndex, 0x08, 0x03, ACC_MODE_FILE_READ);

	umbIndex = umbIndex + 0x03; //now index at counter 
	umbOffsetCNTR = umbIndex;
	//rsc check
	if ((CNTR_LOW == umbRSC)
		||
		(CNTR_HIGH == umbRSC)
		||
		(POR_OK == umbRSC)) {

		_7816_4_readUpdateFile(fhaFile, PROACTIVE_BUFF + umbIndex, 0x0B, 0x05, ACC_MODE_FILE_READ);

	}
	else {
		mem_set(PROACTIVE_BUFF + umbIndex, 0x00, 0x05);
	}

	umbIndex = umbIndex + 0x05;

	if (POR_OK != umbRSC) {

		/*calculating RHL*/
		PROACTIVE_BUFF[indexRPL+1] = 0x0A;

		/*calculating RPL*/
		PROACTIVE_BUFF[indexRPL - 0x01] = 0x00;
		PROACTIVE_BUFF[indexRPL] = 0x0B;

		PROACTIVE_BUFF[umbIndex++] = 0x00;//PADD CNTR

		PROACTIVE_BUFF[umbIndex++] = umbRSC;


		if (udbsdbuf[0x01] & 0x20) {

			/*calculating UDL*/
			PROACTIVE_BUFF[indexUDL] = PROACTIVE_BUFF[indexRPL] + 0x05;

			/*calculating length of '8B' tag*/
			PROACTIVE_BUFF[OFFSET_SMS_TPDU_LEN] = 0x07 + umbLengthOA + PROACTIVE_BUFF[indexUDL];

			PROACTIVE_BUFF[OFFSET_PROACT_LEN] = 0x0B + PROACTIVE_BUFF[OFFSET_SMS_TPDU_LEN];
			
			gudbProactiveLen = umbIndex;
		}
		else {
			mem_cpy(RESPONSE_BUFF, PROACTIVE_BUFF, umbIndex);
			gudbOutputLen = umbIndex;
		}
		return;
	}

	//PADDING
	umbAlgoKeyidKVN[0x02] = SHIFT_RIGHT_BY_N_BITS(udbsdbuf[0x03], 0x04);

	/*Check SPI2 for checksum */
	if (POR_REDUNDANCY_CHECK == (udbsdbuf[0x01] & MASK_SPI2_CHECKSUM)) {/*for CRC16 or CRC32*/
																		  /*store CRC from KID*/
		if (CRC_16 == (udbsdbuf[0x03] & MASK_ALGO)) {
			umbChecksumLen = CRC_16_For_CheckSum;
			umbAlgo = CHECKSUM_CRC16;
		}
		else {/*if CRC 32*/
			umbChecksumLen = CRC_32_For_CheckSum;
			umbAlgo = CHECKSUM_CRC32;
		}
	}
	else if (POR_CRYPTOGRAPHIC_CHECKSUM == (MASK_SPI2_CHECKSUM & udbsdbuf[0x01]))  {/*for AES or DES checksum blocksize*/

		umbChecksumLen = AES_DES_For_CheckSum;
		/*check KID for algo*/
		umbAlgoKeyidKVN[0x01] = KID_CC;

		if (DES_ALGO == (udbsdbuf[0x03] & MASK_ALGO_1)) {

			umbAlgoKeyidKVN[0x00] = ALGO_3DES;
			umbAlgo = SIGN_DES_MAC;
		}
		else {//AES
			umbAlgoKeyidKVN[0x00] = ALGO_AES;
			umbAlgo = SIGN_AES_CMAC;
		}
	}

	DBG_FUNC("Get CheckSum Length \n", umbChecksumLen);
	/*check KIC for algo blocksize*/
	if (DES_ALGO == (udbsdbuf[0x02] & MASK_ALGO_1)) {
		umbAlgo_Block = 0x08;
	}
	else {//AES
		umbAlgo_Block = 0x10;
	}
	DBG_FUNC("Get Algo BlockSize Length \n", umbAlgo_Block);

	/*read the Secure Data Length*/
	_7816_4_readUpdateFile(fhaFile, udbsdbuf + 4, OFFSET_RESPONSE_MESSAGE, 0x02, ACC_MODE_FILE_READ);

	udbResSize = mm_makeUdiByte(udbsdbuf + 4, 0x00);

	DBG_FUNC("Store Secure Data Length \n", udbResSize);

	if (POR_CIPHERING == (udbsdbuf[0x01] & POR_CIPHERING))
	{//if SPI2 bit 5 indicate CIPHERING

	   /*Calculate padding length for create blockwise data*/
		umbPadding = (0x07 + umbChecksumLen + udbResSize) % umbAlgo_Block;
		if (umbPadding != 0x00) {/*if padding non Zero*/
			umbPadding = (umbAlgo_Block - umbPadding);
		}
		DBG_FUNC("padding length for create blockwise data \n", umbPadding);
	}
	PROACTIVE_BUFF[umbIndex++] = umbPadding;//PADD CNTR

	PROACTIVE_BUFF[umbIndex++] = umbRSC;
	umbOffsetCC = umbIndex;

	umbIndex += umbChecksumLen; /*adding cehcksum length to get index at 'additional response data*/

	//length calculation
	DBG_ERR("udbResSize = %2", udbResSize);

	/*calculating RPL*/

	udbRPL = 0x0B + umbChecksumLen + udbResSize + umbPadding;

	PROACTIVE_BUFF[indexRPL] = (uMonoByte)udbRPL;
	PROACTIVE_BUFF[indexRPL - 0x01] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(udbRPL, 0x08);


	/*calculating RHL*/
	PROACTIVE_BUFF[indexRPL+1] = 0x0A + umbChecksumLen;

	/*calculating length of '8B' tag*/
	if (udbsdbuf[0x01] & 0x20) {

		/*calculating UDL*/
		PROACTIVE_BUFF[indexUDL] = PROACTIVE_BUFF[indexRPL] + 0x05;

		PROACTIVE_BUFF[OFFSET_SMS_TPDU_LEN] = 0x07 + umbLengthOA + PROACTIVE_BUFF[indexUDL];

		PROACTIVE_BUFF[OFFSET_PROACT_LEN] = 0x0B + PROACTIVE_BUFF[OFFSET_SMS_TPDU_LEN];
	}
								//mem_cpy(PROACTIVE_BUFF + umbIndex, COMMAND_APDU + 0x02, udbResSize);
	_7816_4_readUpdateFile(fhaFile, PROACTIVE_BUFF + umbIndex, OFFSET_RESPONSE_MESSAGE + 0x02, udbResSize, ACC_MODE_FILE_READ);

	umbIndex += udbResSize;		/*adding additional response length to get index at padding*/

	mem_set(PROACTIVE_BUFF + umbIndex, 0x00, umbPadding);

	umbIndex += umbPadding;		/*adding padding to get complete POR length*/

	//_7816_4_readUpdateFile(fhaFile, PROACTIVE_BUFF + umbIndex, 0x0B, 0x03, ACC_MODE_FILE_READ);

	//checksum calc
	if (0x00 != umbChecksumLen) {//if RC/CC/DS available

		if (POR_REDUNDANCY_CHECK == (udbsdbuf[0x01] & MASK_SPI2_CHECKSUM)) {

			algo_chksm_init(&cChecksumObj, umbAlgo);

			algo_chksm_update(&cChecksumObj, (PROACTIVE_BUFF + indexUDHL), 0x10);

			algo_chksm_finalize(&cChecksumObj, (PROACTIVE_BUFF + indexUDHL + 0x10 + umbChecksumLen), udbResSize, umbChecksum);
		}
		else {
			umbKeyValue[OFFSET_KEY_VALUE_LENGTH] = _102225_KeyManagement(umbAlgoKeyidKVN, umbKeyValue + 0x01);

			algo_sign_init(&sSignatureObj, umbAlgo, umbChecksumLen, umbKeyValue);

			algo_sign_update(&sSignatureObj, (PROACTIVE_BUFF + indexUDHL), 0x10);

			algo_sign_finalize(&sSignatureObj, (PROACTIVE_BUFF + indexUDHL + 0x10 + umbChecksumLen), (udbResSize + umbPadding), umbChecksum);
		}
		
		DBG_PRINT_ARRAY(umbChecksum, umbChecksumLen);
		mem_cpy(PROACTIVE_BUFF + umbOffsetCC, umbChecksum, umbChecksumLen);
	}

	if (POR_CIPHERING == (udbsdbuf[0x01] & MASK_SPI2_CIPHERING)) {//if SPI2 bit 5 indicate CIPHERING
		if (0x08 == umbAlgo_Block) {//if block size is 0x08 means it is a DES algo

			umbAlgo_Block = BC_DES;
			umbAlgoKeyidKVN[0x00] = ALGO_3DES;
		}
		else {//if block size is 0x10 means it is a AES algo
			umbAlgo_Block = BC_AES;
			umbAlgoKeyidKVN[0x00] = ALGO_AES;
		}
		umbAlgoKeyidKVN[0x01] = KIC;

		//udbResSize is used for length from CNTR to Additional resp data including Checksum and PADDING
		udbResSize = (0x07 + umbChecksumLen + udbResSize + umbPadding);

		umbKeyValue[OFFSET_KEY_VALUE_LENGTH] = _102225_KeyManagement(umbAlgoKeyidKVN, umbKeyValue + 0x01);

		algo_bc_init(&bcCipherObj, umbAlgo_Block, OP_ENCRYPT, umbKeyValue);

		umbLengthOA = (uMonoByte)algo_bc_update(&bcCipherObj, PROACTIVE_BUFF + umbOffsetCNTR, udbResSize, umbChecksum);

		algo_bc_finalize(&bcCipherObj, (PROACTIVE_BUFF + umbOffsetCNTR + umbLengthOA), 0x00, (umbChecksum + umbLengthOA));
	}
	DBG_PRINT_ARRAY(umbChecksum, udbResSize);
	mem_cpy((PROACTIVE_BUFF + umbOffsetCNTR), umbChecksum, udbResSize);

	/*adding of 81 tag to length*/
	if (udbsdbuf[0x01] & 0x20) {

		if (0x7F < PROACTIVE_BUFF[OFFSET_SMS_TPDU_LEN]) {

			//RESPONSE_BUFF[0x00] = PROACTIVE_BUFF[0x00];

			PROACTIVE_BUFF[OFFSET_PROACT_LEN]++;

			/*Shift SMS-TPDU length to end of the data by 1*/
			mem_cpyBwd(PROACTIVE_BUFF + OFFSET_SMS_TPDU_LEN + 0x02, PROACTIVE_BUFF + OFFSET_SMS_TPDU_LEN, PROACTIVE_BUFF[OFFSET_SMS_TPDU_LEN] + 0x01);
			PROACTIVE_BUFF[OFFSET_SMS_TPDU_LEN + 0x01] = 0x81;

			/*Shift data from Proactive length to SMS-TPDU tag by 1*/
			mem_cpyBwd(PROACTIVE_BUFF + OFFSET_PROACT_LEN + 0x01, PROACTIVE_BUFF + OFFSET_PROACT_LEN, 0x0B); 
			RESPONSE_BUFF[OFFSET_PROACT_LEN] = 0x81;

			umbIndex+=2;
		}
		else {
			mem_cpy(RESPONSE_BUFF, PROACTIVE_BUFF, umbIndex);
		}
		gudbProactiveLen = umbIndex;
	}
	else {
		mem_cpy(RESPONSE_BUFF, PROACTIVE_BUFF, umbIndex);
		gudbOutputLen = umbIndex;
	}

}