#include "102_226.h"

#define FEAT_EXPANDED_FORMAT

static uDiByte		udbResponseLength;
static uMonoByte	umbStatusWords[0x02];
static tPinData		pdPinStatusRFM;
/*
* Recieve command header from RFM Secured Message
*/
void _102226_receiveCommandHeader(void)
{
	tFileHeaderAddr		fhaFile;
	
	DBG_FUNC_ENTRY(">>[102226] _102226_receiveCommandHeader\n");

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	//get memory reference of Secured Message EF
	_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFile, MODE_FIND_FID, &fhaFile);

	/*storing command header of 5 bytes*/
	_7816_4_readUpdateFile(fhaFile, &CLA_BYTE, gudbCommandIndex, SIZE_CMD_HEADER, ACC_MODE_FILE_READ); 

	DBG_FUNC("[102226] _102226_receiveCommandHeader : \nCLA = %1\nINS = %1\nP1  = %1\nP2  = %1\nP3  = %1\n", CLA_BYTE, INS_BYTE, P1_BYTE, P2_BYTE, P3_BYTE);

	gudbCommandIndex += SIZE_CMD_HEADER; //adding the size of command header to recieve command data

	DBG_FUNC_EXIT("<<[102226] _102226_receiveCommandHeader\n");
}

/*
* Receive command data
*
* @pre command supposed to be of Input type i.e. it must have command data
*/
void _102226_receiveCommandData(void)
{
	tFileHeaderAddr		fhaFile;
	
	DBG_FUNC_ENTRY(">>[102226] _102226_receiveCommandData\n");

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	//get memory reference of Secured Message EF
	_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFile, MODE_FIND_FID, &fhaFile);
	
	_7816_4_readUpdateFile(fhaFile, COMMAND_DATA, gudbCommandIndex, P3_BYTE, ACC_MODE_FILE_READ);
	//storing Command Data from Prop. File '6F1A'

	gudbCommandIndex += P3_BYTE; //adding the size of command data to recieve next command

	DBG_FUNC_EXIT("<<[102226] _102226_receiveCommandData\n");
}

/*
* Updates the response data to Prop. File '6F1A'
*/
void _102226_sendResponseData(void)
{	
	DBG_FUNC_ENTRY(">>[102226] _102226_sendResponseData\n");

	if (0x00 == P3_BYTE) { // check if p3 is 00

		/*Max requested length will be total prepared response*/
		udbResponseLength = gudbOutputLen;
	}
	else { // if p3 is non-zero

		/*keep same data*/
		udbResponseLength = P3_BYTE;
	}

	/*Reset the response length*/
	gudbOutputLen -= udbResponseLength;

	/*Increasing commandIndex to maximum to break while loop of commands execution*/
	gudbCommandIndex = MAX_VALUE;

	DBG_FUNC_EXIT("<<[102226] _102226_sendResponseData\n");
}

void _102226_sendStatusWords(void) {

	DBG_FUNC_ENTRY(">>[102226] _102226_sendStatusWords\n");

	/*Get status words*/
	uDiByte udbSw = SIM ? SW[excepRet - 1].udbSimSw : \
		SW[excepRet - 1].udbUiccSw;

	if ((EXCEP_WRONG_P3 == excepRet)
		||
		(EXCEP_SEND_GET_RESPONSE == excepRet)
		||
		(EXCEP_RESEND_COMMAND_WITH_CORRECT_P3 == excepRet)
		||
		(EXCEP_PIN_VERIFICATION_FAILED == excepRet)) { // if its 67xx or 61xx or 6Cxx or 63Cx
 	    /*P3 will help to provide SW2 in this case*/
		udbSw |= ((uMonoByte)P3_BYTE);
	}
	/*Taking SW1 & SW2 for using in Additional Response Data*/
	umbStatusWords[0x00] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(udbSw, 0x08); //SW1
	umbStatusWords[0x01] = (uMonoByte)(udbSw);							//SW2

	DBG_FUNC_EXIT("<<[102226] _102226_sendStatusWords\n");
}

/*
* It processes (execute) the RFM commands.
*/
uMonoByte _102226_ProcessRfm(IN_ARG uMonoByte umbRecordNo) {

	tFileHeaderAddr		fhaFileDF;
	tFileHeaderAddr		fhaFileEF;
	uMonoByte			umbTemp[SIZE_CMD_HEADER];
	uMonoByte			umbNoOfExCmd = 0;
	uMonoByte			umbSmSizeBytes[0x05];
	uMonoByte			umbCurrentNaaAppTemp;
	uDiByte				udbTotalResponseLen = 0;
	uMonoByte			umbRecordData[RECORD_LENGTH_RFMPR];
	uMonoByte			umbLengthAID;
	tPinData			pdPinStatusTEMP;
	uDiByte				udbRange;		/*range of secured message to execute*/
	uMonoByte			umbFirstByte;	/*first byte of secured message*/

#ifdef FEAT_EXPANDED_FORMAT
	uMonoByte			umbTagLength[0x03];
	uMonoByte			umbCmdLength;
	uMonoByte			umbTemp2;
	uMonoByte			umbExpLength;
	uDiByte				udbNextTLV;
	uMonoByte			umbBadFormat = 0x00;
	uDiByte				udbResponseIndex = 0x0A; 
	//2 bytes for total response length + 2 bytes for expanded tag and length + 3 for no. of executed commands TLV
#endif
	DBG_FUNC_ENTRY(">>[102226] _102226_ProcessRfm()\n");

	/*setting to default after taking backup of CurrentNaaApp*/
	umbCurrentNaaAppTemp = gumbCurrentNaaApp;
	gumbCurrentNaaApp = NAA_UICC;

	pdPinStatusTEMP = pdPinStatus;  // storing backup of PinStatuse

	/*backup of Command Header*/
	umbTemp[0x00] = CLA_BYTE;
	umbTemp[0x01] = INS_BYTE;
	umbTemp[0x02] = P1_BYTE;
	umbTemp[0x03] = P2_BYTE;
	umbTemp[0x04] = P3_BYTE;

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFileDF);

	//get memory reference of Secured Message EF
	_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFileDF, MODE_FIND_FID, &fhaFileEF);

	_7816_4_readUpdateFile(fhaFileEF, umbSmSizeBytes, OFFSET_SECURED_MESSAGE_LENGTH, 0x02, ACC_MODE_FILE_READ);
	/*reading end index of secured message*/

	udbRange = mm_makeUdiByte(umbSmSizeBytes, 0x00); //taking end index of secured message

	DBG_FUNC("[102226] _102226_ProcessRfm : END index of secured message = %2\n", udbRange);
	
	if (0x00 != umbRecordNo) {/*if umbRecordNo is not equal to '00' means control coming from envelop*/

		//get memory reference of RFM Parameter EF
		_7816_4_ifFileExists(FID_PROP_RFM_PARAMETER, fhaFileDF, MODE_FIND_FID, &fhaFileEF);

		//reading record of corresponding TAR
		_7816_4_readUpdateRecordFile(fhaFileEF, umbRecordData, umbRecordNo, ACC_MODE_FILE_READ);

		pdPinStatus = 0;// setting pdPinStatus to zero for mapping accordingly

		/*
		* Mapping of key references acoording to prop. file Access Domain description
		*
		* Byte 1:
		* Bits-> |  08  |  07  |  06  |  05  |  04  |  03  |  02  |  01  |
		* Keys-> |  08  |  07  |  06  |  05  |  04  |  03  |  02  |  01  |
		*
		* Byte 2:
		* Bits-> |  16  |  15  |  14  |  13  |  12  |  11  |  10  |  09  |
		* Keys-> |  8C  |  8B  |  8A  |  0E  |  0D  |  0C  |  0B  |  0A  |
		*
		* Byte 3:
		* Bits-> |  24  |  23  |  22  |  21  |  20  |  19  |  18  |  17  |
		* Keys-> |  XX  |  XX  |  XX  |  XX  |  LOC |  ALW |  8E  |  8D  |


		* Mapping of key references to below type variable
		* Bits-> |  08  |  07  |  06  |  05  |  04  |  03  |  02  |  01  |
		* Keys-> |  08  |  07  |  06  |  05  |  04  |  03  |  02  |  01  |
		*
		* Bits-> |  16  |  15  |  14  |  13  |  12  |  11  |  10  |  09  |
		* Keys-> |  XX  |  XX  |  0E  |  0D  |  0C  |  0B  |  0A  |  XX  |
		*
		* Bits-> |  24  |  23  |  22  |  21  |  20  |  19  |  18  |  17  |
		* Keys-> |  88  |  87  |  86  |  85  |  84  |  83  |  82  |  81  |
		*
		* Bits-> |  32  |  31  |  30  |  29  |  28  |  27  |  26  |  25  |
		* Keys-> |  ALW |  XX  |  8E  |  8D  |  8C  |  8B  |  8A  |  XX  |
		*/

		/*Mapping access domain as per tPinData*/
		pdPinStatus |= (umbRecordData[0x00])

			| (SHIFT_LEFT_BY_N_BITS((tPinData)(umbRecordData[0x01] & 0x1F), 0x09))

			| (SHIFT_LEFT_BY_N_BITS((tPinData)(umbRecordData[0x01] & 0xE0), 0x14))

			| (SHIFT_LEFT_BY_N_BITS((tPinData)(umbRecordData[0x02] & 0x03), 0x1C))

			| (SHIFT_LEFT_BY_N_BITS((tPinData)(umbRecordData[0x02] & 0x04), 0x1D));

		if (umbRecordData[0x02] & 0x08) {
			pdPinStatus |= 0x00FF0000;
		}
		/*store pin status for RFM*/
//		pdPinStatusRFM = pdPinStatus;

		DBG_FUNC("[102226] _102226_ProcessRfm : PIN STATUS for Access Domain = %4\n", pdPinStatus);

		umbLengthAID = umbRecordData[OFFSET_AID_LENGTH]; //storing length of AID from offset

		if (0x00 != umbLengthAID) { //when length of AID is non-zero

			//AID found, this means Respective ADF should be Implicitly selceted
			_7816_4_ifAdfExists(&umbRecordData[OFFSET_AID_LENGTH], &fhaFileDF);

			DBG_FUNC("[102226] _102226_ProcessRfm : ADF is implicitly selected.\n");

			/*Set the found ADF as current ADF*/
			ccContexts[RFM_LOGICAL_CHANNEL].fhaCurrentAdf = fhaFileDF;

			/*Set found file as current DF in current channel context*/
			ccContexts[RFM_LOGICAL_CHANNEL].fhaCurrentDf = fhaFileDF;
		}
		else { //when length of AID is zero
			DBG_FUNC("[102226] _102226_ProcessRfm : MF is implicitly selected.\n");

			/*Set MF as current DF in current channel context*/
			ccContexts[RFM_LOGICAL_CHANNEL].fhaCurrentDf = _7816_4_getMfAddress(); //No AID found, this means MF should be Implicitly selceted
		}

		/*Reset the current EF to undefined*/
		ccContexts[RFM_LOGICAL_CHANNEL].fhaCurrentEf = NO_FILE_PRESENT;

		/*Current Record pointer is undefined on selection*/
		ccContexts[RFM_LOGICAL_CHANNEL].umbCurrentRecPtr = RECORD_UNDEFINED;

		/*get memory reference of Admin use DF*/
		_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFileDF);

		//get memory reference of Secured Message EF
		_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFileDF, MODE_FIND_FID, &fhaFileEF);

		/*reading one first byte + two bytes for length*/
		_7816_4_readUpdateFile(fhaFileEF, umbSmSizeBytes + 0x02, gudbCommandIndex, 0x03, ACC_MODE_FILE_READ);

		/*storing first byte of secured message to check if it is expanded format tag*/
		umbFirstByte = umbSmSizeBytes[0x02];

		DBG_FUNC("[102226] _102226_ProcessRfm : First Byte of secured message = %1\n", umbFirstByte);


		//	udbRange = gudbCommandIndex + udbSmSize; //calculating range to execute secured message
#ifdef FEAT_EXPANDED_FORMAT
		if (B2_B1_FOR_COMPACT != (umbFirstByte & MASK_B2_B1)) { /*expanded format*/


			if (TAG_EXPANDED_COMMAND_DEFINITE == umbFirstByte) { /*if first byte is for definite length*/

				DBG_FUNC("[102226] _102226_ProcessRfm : Tag 'AA' found of Expanded Format.\n");

				if (LENGTH_OF_TWO_BYTES == umbSmSizeBytes[0x03]) { /*when length is with '81' tag*/

					umbExpLength = umbSmSizeBytes[0x04];
					umbTemp2 = 0x03; //1 tag + 2 length bytes
				}
				else {
					umbExpLength = umbSmSizeBytes[0x03];
					umbTemp2 = 0x02; //1 tag + 1 length bytes
				}

				/*length validation*/
				if ((udbRange - gudbCommandIndex - umbTemp2) != umbExpLength) { /*validatoin of length of AA tag*/
					//BAD format
					umbBadFormat = BAD_FORMAT_WRONG_LENGTH;


					DBG_ERR("[102226] _102226_ProcessRfm : Not Executing Script as length of Command scripting template is incorrect.\n");
					gudbCommandIndex = MAX_VALUE; /*storing max value to index to stop execution*/
				}
				/*increasing 2/3 for AA tag + length byte(s)*/
				gudbCommandIndex += umbTemp2;

			}
			else if (TAG_EXPANDED_COMMAND_INDEFINITE == umbFirstByte) { /*for indefinite length*/

				DBG_FUNC("[102226] _102226_ProcessRfm : Tag 'AE' found of Expanded Format.\n");

				gudbCommandIndex += 0x02; /*increasing 2 for AE tag + length byte*/

				if (INDEFINITE_LENGTH_INDICATOR != umbSmSizeBytes[0x03]) { /*when idefinite length and next byte is not length indicator*/
					//BAD format
					umbBadFormat = BAD_FORMAT_UNKNOWN_TAG;

					DBG_ERR("[102226] _102226_ProcessRfm : Not Executing Script as Indefinite length indicator not Found.\n");
					gudbCommandIndex = MAX_VALUE; /*storing max value to index to stop execution*/
				}
				udbRange = MAX_VALUE;
			}
			else { /*tag except AA/AE*/
				//BAD format 
				umbBadFormat = BAD_FORMAT_UNKNOWN_TAG;

				DBG_ERR("[102226] _102226_ProcessRfm : Not Executing Script as TAG not Found.\n");
				gudbCommandIndex = MAX_VALUE; /*storing max value to index to stop execution*/
			}
		}
	}
	

	else {

		udbResponseIndex = udbResponseLength; //recover response index

		umbFirstByte = umbStatusWords[0x00]; //recover first byte for make difference between definite and indefinite.

		umbNoOfExCmd = umbStatusWords[0x01]; //recover no of total executed cmd

		pdPinStatus = pdPinStatusRFM; //recover RFM pin status
	}
	udbNextTLV = gudbCommandIndex; 

#endif	
	gumbcommandSource = SRC_RFM; //Source is 1 for RFM secured message commands

	while (gudbCommandIndex < udbRange) {

#ifdef FEAT_EXPANDED_FORMAT
		if (B2_B1_FOR_COMPACT != (umbFirstByte & MASK_B2_B1)) { /*expanded format*/

			/*Reading Tag & Length bytes of current TLV*/
			_7816_4_readUpdateFile(fhaFileEF, umbTagLength, udbNextTLV, 0x03, ACC_MODE_FILE_READ);

			DBG_FUNC("[102226] _102226_ProcessRfm : TAg of TLV = %1\n", umbTagLength[0]);

			/*check if it is exit indicator of indefinite length expanded format*/
			if ((TAG_EXPANDED_COMMAND_INDEFINITE == umbFirstByte)
				&&
				(INDEFINITE_LENGTH_EXIT_INDICATOR == umbTagLength[0])
				&&
				(INDEFINITE_LENGTH_EXIT_INDICATOR == umbTagLength[1])) {

				DBG_ERR("[102226] _102226_ProcessRfm : Breaking execution of script as End of content indicator '00 00' found.\n");
				break;
			}

			if (LENGTH_OF_TWO_BYTES == umbTagLength[0x01]) { /*checking if length of Two bytes*/

				umbCmdLength = umbTagLength[0x02];
				DBG_FUNC("[102226] _102226_ProcessRfm : Command Length with '81' tag = %1\n", umbCmdLength);

				/*use of local index for reaching next command TLV*/
				udbNextTLV += 0x03 + umbCmdLength;
				gudbCommandIndex = gudbCommandIndex + 0x03; /*increasing gudbCommandIndex index to reach value part of TLV*/
			}
			else {
				umbCmdLength = umbTagLength[0x01];
				DBG_FUNC("[102226] _102226_ProcessRfm : Command length = %1\n", umbCmdLength);

				/*use of local index for reaching next command TLV*/
				udbNextTLV += 0x02 + umbCmdLength;
				gudbCommandIndex = gudbCommandIndex + 0x02; /*increasing gudbCommandIndex index to reach value part of TLV*/
			}

			/*check of length if it is greater than total length*/
			if ((umbCmdLength > umbExpLength)
				||
				(umbCmdLength < MINIMUM_COMMAND_LENGTH))
			{
				//BAD format
				umbBadFormat = BAD_FORMAT_WRONG_LENGTH;

				DBG_ERR("[102226] _102226_ProcessRfm : Breaking execution of script as length '%1' is not correct.\n", umbCmdLength);
				break; /*break to stop execution*/
			}

			if (TAG_C_APDU == umbTagLength[0]) { /*C-APDU TAG*/

				/*calling Command Router for secured messages commands execution*/
				cmdRout_commandRouter();

				udbResponseIndex = _102226_ResponseTLV(udbResponseIndex);
			}

			else if (TAG_IMMEDIATE_ACTION == umbTagLength[0]) { /*Immediate action TAG*/
					
			/*performing immediate action*/
				if (0x00 == gumbProactiveSession) { /*check any proactive session is ongoing or not*/
					
					if (0x01 < umbTagLength[1]) { /*check length is greater than 1 acc. to ts_102226 if referenced format*/

						/*send incoming proactive cmd in proactive buffer*/
						_7816_4_readUpdateFile(fhaFileEF, PROACTIVE_BUFF, gudbCommandIndex, umbTagLength[1], ACC_MODE_FILE_READ);
						
						gudbProactiveLen = umbTagLength[1];
						
						/*now proactive session starts*/
						gumbProactiveSession = 0x01;
						
						/*reset the RESP_BUFF*/
						gudbOutputLen = 0x00;
						
						/*increament the no. of executed cmd considering proactive cmd is executed*/
						umbNoOfExCmd++;
						
						/*store the index of next TLV in gudbCommandIndex*/
						gudbCommandIndex = udbNextTLV;
						
						gumbCurrentNaaApp = umbCurrentNaaAppTemp; //restoring of CurrentNaaApp
						
						pdPinStatusRFM = pdPinStatus;//backup of RFM pinstatus

						pdPinStatus = pdPinStatusTEMP; //restoring of PinStatus
						
						/*reset the source*/
						gumbcommandSource = SRC_TERMINAL;
						
						/*backup the response index*/
						udbResponseLength = udbResponseIndex;

						/*backup first byte*/
						umbStatusWords[0x00] = umbFirstByte;

						/*backup no. of executed cmd*/
						umbStatusWords[0x01] = umbNoOfExCmd;

						return 0x01;
					}
					else if (0x01 == umbTagLength[1]) {

						/*TODO : 'Proactive session indication'*/
					}
				}
				else {
					/*TODO : 'SUSPENSION ERROR'*/
				}

			}
			else if (TAG_ERROR_ACTION == umbTagLength[0]) { /*Error action TAG*/
				 /*TODO: performing error action*/
			}
			else if (TAG_SCRIPT_CHAINING == umbTagLength[0]) { /*Script chaining TAG*/
				 /*TODO: performing script chaining action, will be done in http*/
			}
			else { /*if tag is encountered other than 22, 81, 82, 83*/
				
				if (TAG_EXPANDED_COMMAND_DEFINITE == umbFirstByte) { /*if definite and tag other than 22, 81, 82, 83*/

					//Bad format
					umbBadFormat = BAD_FORMAT_UNKNOWN_TAG;

					DBG_ERR("[102226] _102226_ProcessRfm : Breaking execution of script as Tag '%1' is unknown.\n", umbTagLength[0]);
					break; /*break to stop execution*/
				}
				else { /*if indefinite and tag other than 22, 81, 82, 83*/
					
					//Bad format
					umbBadFormat = BAD_FORMAT_UNKNOWN_TAG;

					DBG_ERR("[102226] _102226_ProcessRfm : Breaking execution of script as Tag '%1' is unknown.\n", umbTagLength[0]);
					break; /*break to stop execution*/
				}
			}
			gudbCommandIndex = udbNextTLV; //storing value of next TLV to Command index
		}

		else
#endif
		{ /*compact format*/
			/*calling Command Router for secured messages commands execution*/
			cmdRout_commandRouter();
		}
		umbNoOfExCmd++; //number of executed commands

		if ((umbStatusWords[0x00] != 0x61)
			&&
			(umbStatusWords[0x00] != 0x90))
			/*if Status Word 1 is not '61' or '90' i.e, unsuccessful command*/
		{
			gudbCommandIndex = MAX_VALUE;  // for breaking execution of commands 
		}
	}
	
	gumbcommandSource = SRC_TERMINAL; //setting back source as terminal
	
	if (B2_B1_FOR_COMPACT == (umbFirstByte & MASK_B2_B1)) { /*Response storage - compact format*/

		/*Taking 'No. of executed commands' before Status Words in COMMAND APDU*/
		COMMAND_APDU[0x02] = umbNoOfExCmd;
		DBG_FUNC("[102226] _102226_ProcessRfm : Total number of Executed Commands = %1\n", umbNoOfExCmd);

		if (MAX_VALUE == gudbCommandIndex) {
			udbTotalResponseLen = udbResponseLength; //when OUTPUT command, ResponseLen will be Total OutputLen
		}

		/*storing Status Words before Response Data for Additional Response preparation*/
		COMMAND_APDU[0x03] = umbStatusWords[0x00];
		COMMAND_APDU[0x04] = umbStatusWords[0x01]; 

		/*Reset the response length*/
		udbResponseLength = 0; //when no OUTPUT command, ResponseLen will be 0
		gudbOutputLen = 0;

		udbTotalResponseLen += 0x03; //adding 3 bytes for 'status words' and 'no. of executed commands' byte

		DBG_FUNC("[102226] _102226_ProcessRfm : Total response length will be %2\n", udbTotalResponseLen);

		/*total additional response length*/
		COMMAND_APDU[0x01] = (uMonoByte)(udbTotalResponseLen);
		COMMAND_APDU[0x00] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(udbTotalResponseLen, 0x08);

		/*Updating response to response message EF at required index*/
		_7816_4_readUpdateFile(fhaFileEF, COMMAND_APDU, OFFSET_RESPONSE_MESSAGE + 0x01, udbTotalResponseLen + 0x02, ACC_MODE_FILE_UPDATE); /*adding 2 for udbTotalResponseLen bytes*/
		/**COMMAND_APDU is used instead of RESPONSE_BUFF because both ae pointing same array
		*Also we need to store Status words & No of executed commands before Response in RESPONSE_BUFF*/

		COMMAND_APDU[0x00] = 0x03; /*storing offset to start additional response */
		_7816_4_readUpdateFile(fhaFileEF, COMMAND_APDU, OFFSET_RESPONSE_MESSAGE, 0x01, ACC_MODE_FILE_UPDATE);
	}
#ifdef FEAT_EXPANDED_FORMAT
	else { /*Response storage - expanded format*/

		if (umbBadFormat != 0x00) { //when there is bad format
			
			/*preparing Bad Format TLV in an array*/
			umbSmSizeBytes[0x00] = TAG_BAD_FORMAT;
			umbSmSizeBytes[0x01] = 0x01; //length of Bad Forat Value
			umbSmSizeBytes[0x02] = umbBadFormat; //Value of Bad Format

			/*storing Bad Format TLV to prop. file '6F1A' at current index as last TLV*/
			_7816_4_readUpdateFile(fhaFileEF, umbSmSizeBytes, OFFSET_RESPONSE_MESSAGE + udbResponseIndex, 0x03, ACC_MODE_FILE_UPDATE);

			//adding length of BAD Format TLV to response index
			udbResponseIndex += 0x03; 
		}

		if (TAG_EXPANDED_COMMAND_DEFINITE == umbFirstByte) { /*for definite length response*/

			if (0x80 > (udbResponseIndex - 0x07)) {
			/*when length lies between '00' to '7F'*/

				COMMAND_APDU[0x05] = TAG_EXPANDED_RESPONSE_DEFINITE;

				//difference of 'starting index and ending index' for calculating ResponseLen of definite
				COMMAND_APDU[0x06] = udbResponseIndex - 0x07;

				umbTemp2 = 0x05; //offset to start additional response
			}

			//handling of '81' tag for larger length with 'AB' definite tag
			else if (0x100 > (udbResponseIndex - 0x07)) {
			/*when length lies between '80' to 'FF'*/

				COMMAND_APDU[0x04] = TAG_EXPANDED_RESPONSE_DEFINITE;

				COMMAND_APDU[0x05] = LENGTH_OF_TWO_BYTES;

				//difference of 'starting index and ending index' for calculating ResponseLen of definite
				COMMAND_APDU[0x06] = udbResponseIndex - 0x07;

				umbTemp2 = 0x04; //offset to start additional response
			}

			//handling of '82' tag for larger length with 'AB' definite tag
			else {
			/*when length is greater than 'FF'*/

				COMMAND_APDU[0x03] = TAG_EXPANDED_RESPONSE_DEFINITE;

				COMMAND_APDU[0x04] = LENGTH_OF_THREE_BYTES;

				COMMAND_APDU[0x05] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS((udbResponseIndex - 0x07), 0x08);

				//difference of 'starting index and ending index' for calculating ResponseLen of definite
				COMMAND_APDU[0x06] = (uMonoByte)(udbResponseIndex - 0x07);

				umbTemp2 = 0x03; //offset to start additional response
			}

			//difference of 'starting index and ending index' for calculating TotalResponseLen
			udbTotalResponseLen = udbResponseIndex - umbTemp2;

			COMMAND_APDU[0x00] = umbTemp2; //index of additional response
		}
		else { /*for indefinite length response*/

			COMMAND_APDU[0x05] = TAG_EXPANDED_RESPONSE_INDEFINITE;
			COMMAND_APDU[0x06] = INDEFINITE_LENGTH_INDICATOR;

			COMMAND_APDU[0x03] = INDEFINITE_LENGTH_EXIT_INDICATOR;
			COMMAND_APDU[0x04] = INDEFINITE_LENGTH_EXIT_INDICATOR;

			COMMAND_APDU[0x00] = 0x05; //index to start additional response

			//storing 'exit indicator of indefinite format' at the 'end of response' to prop. file '6F1A'
			_7816_4_readUpdateFile(fhaFileEF, COMMAND_APDU + 0x03, OFFSET_RESPONSE_MESSAGE + udbResponseIndex, 0x02, ACC_MODE_FILE_UPDATE);
			udbResponseIndex += 0x02; //adding 2 for last indicator bytes '00 00'

			//difference of 'starting index and ending index' for calculating TotalResponseLen'
			udbTotalResponseLen = udbResponseIndex - COMMAND_APDU[0x00]; /*2 for 'AF' tag length + 2 for '00 00'*/
		}
		DBG_FUNC("[102226] _102226_ProcessRfm : Total response length will be %2\n", udbTotalResponseLen);

		/*storing additional response length for Additional Response preparation*/
		COMMAND_APDU[0x02] = (uMonoByte)(udbTotalResponseLen);
		COMMAND_APDU[0x01] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(udbTotalResponseLen, 0x08);

		/*storing 'no. of executed commands TLV' for Additional Response preparation*/
		COMMAND_APDU[0x07] = TAG_NO_OF_EXECUTED_COMMAND_TLV;
		COMMAND_APDU[0x08] = LEN_NO_OF_EXECUTED_COMMAND_TLV;
		COMMAND_APDU[0x09] = umbNoOfExCmd;
		DBG_FUNC("[102226] _102226_ProcessRfm : Total number of Executed Commands = %1\n", umbNoOfExCmd);

		/*updating HEADER of additional response to prop. file '6F1A' at response index*/
		/*contains:
		* total response length
		* definite/ indefinite tag & length
		* no. of executed commands TLV
		*/
		_7816_4_readUpdateFile(fhaFileEF, COMMAND_APDU, OFFSET_RESPONSE_MESSAGE, 0x0A, ACC_MODE_FILE_UPDATE);

		/*reset of lengths*/
		udbResponseLength = 0;
		gudbOutputLen = 0;
	}
#endif

	gumbCurrentNaaApp = umbCurrentNaaAppTemp; //restoring of CurrentNaaApp

	pdPinStatus = pdPinStatusTEMP; //restoring of PinStatuse

	/*recovery of Command Header*/
	CLA_BYTE = umbTemp[0x00];
	INS_BYTE = umbTemp[0x01];
	P1_BYTE  = umbTemp[0x02];
	P2_BYTE  = umbTemp[0x03];
	P3_BYTE  = umbTemp[0x04];

	DBG_FUNC("[102226] _102226_ProcessRfm :\nRecovered : \nCLA = %1\nINS = %1\nP1  = %1\nP2  = %1\nP3  = %1\n", CLA_BYTE, INS_BYTE, P1_BYTE, P2_BYTE, P3_BYTE);

	DBG_FUNC_EXIT("<<[102226] _102226_ProcessRfm()\n");
return 0x00;
}

#ifdef FEAT_EXPANDED_FORMAT
uDiByte _102226_ResponseTLV(IN_ARG uDiByte udbResponseIndex) {

	DBG_FUNC_ENTRY(">>[102226] _102226_ResponseTLV()\n");

	tFileHeaderAddr		fhaFile;
	uMonoByte			umbSizeOfTagLen;

	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	//get memory reference of RFM Parameter EF
	_7816_4_ifFileExists(FID_PROP_SECURED_MESSAGE, fhaFile, MODE_FIND_FID, &fhaFile);

	if (0x00 != gudbOutputLen) {

		udbResponseLength = gudbOutputLen;

		//changing status words to 9000
		umbStatusWords[0x00] = 0x90;
		umbStatusWords[0x01] = 0x00;
	}
	/*copying status words after response for preparing R-APDU*/
	mem_cpy((COMMAND_APDU + SIZE_CMD_HEADER + udbResponseLength), umbStatusWords, 0x02);

	if (0x7F < (udbResponseLength + 0x02)) {
		//handling of '81' tag for larger length with '23' R - APDU tag
		/*'82' tag is not done for now*/

		COMMAND_APDU[0x02] = TAG_R_APDU;
		COMMAND_APDU[0x03] = LENGTH_OF_TWO_BYTES;
		COMMAND_APDU[0x04] = (uMonoByte)udbResponseLength + 0x02; //storing Tag and length before response

		umbSizeOfTagLen = 0x03;
	}
	else {
		COMMAND_APDU[0x03] = TAG_R_APDU;
		COMMAND_APDU[0x04] = (uMonoByte)udbResponseLength + 0x02; //storing Tag and length before response

		umbSizeOfTagLen = 0x02;
	}
	
	/*updating R-APDU TLV to prop. file '6F1A' at current response index*/
	_7816_4_readUpdateFile(fhaFile, COMMAND_APDU + (SIZE_CMD_HEADER - umbSizeOfTagLen), OFFSET_RESPONSE_MESSAGE + udbResponseIndex, udbResponseLength + umbSizeOfTagLen + 0x02, ACC_MODE_FILE_UPDATE);
	/*adding 2/3(umbSizeOfTagLen) for '23' tag lngth + 2 for status words*/

	udbResponseIndex += udbResponseLength + umbSizeOfTagLen + 0x02;
	/*adding 2/3(umbSizeOfTagLen) for '23' tag lngth + 2 for status words*/

	//reset of lengths
	gudbOutputLen = 0x00;
	udbResponseLength = 0x00;

	DBG_FUNC_EXIT("<<[102226] _102226_ResponseTLV()\n");
	return udbResponseIndex;
}
#endif