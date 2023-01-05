#include"102_221.h"

/*
* It implements the Terminal Profile command 
*/
void _102221_terminalProfile(void) {

	tFileHeaderAddr   fhaFile;
	tFileHeader		  fhFile;

	
	DBG_FUNC_ENTRY(">>[102221] _102221_terminalProfile() \n");

	if (P1_BYTE || P2_BYTE) {
		/*Parameters are incorrect must be '0000'*/
		DBG_ERR("[102221] _102221_terminalProfile: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[102221] _102221_terminalProfile() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}
	/*get memory reference of Admin use DF*/
	_7816_4_ifFileExists(FID_PROP_ADMIN_USE, _7816_4_getMfAddress(), MODE_FIND_FID, &fhaFile);

	/*get memory reference of Terminal profile ef*/
	_7816_4_ifFileExists(FID_PROP_TERMINAL_PROFILE, fhaFile, MODE_FIND_FID, &fhaFile);

	/*Read file header*/
	mm_ReadUpdate(fhaFile, (puMonoByte)&fhFile, OFFSET_START, sizeof(tFileHeader), MEM_MODE_READ);

	DBG_FUNC("[102221] _102221_terminalProfile(): comparing prop file %1 with command data\n", fhFile.udbFileId);

	/*Copy the data in local array*/
	mem_cpy(gumbWorkingBuffer, COMMAND_DATA, P3_BYTE);

	/*Compare command data with data of prop file terminal profile*/
	if (TRUE == mm_cmpRam(fhFile.FileSpecific.EF.maFileData, gumbWorkingBuffer, OFFSET_START, P3_BYTE)) {

		DBG_FUNC("[102221] _102221_terminalProfile: Data found inside the prop file terminal profile is same as data recieve in cmd\n");
		}
	
	else{
		DBG_FUNC("[102221] _102221_terminalProfile: data recieve in command is different from data stored in prop file terminal profile \n");
		/*update prop file terminal profile*/
		_7816_4_readUpdateFile(fhaFile, gumbWorkingBuffer, OFFSET_START, P3_BYTE, ACC_MODE_FILE_UPDATE);
	}

	
	DBG_FUNC_EXIT("<<[102221] _102221_terminalProfile() \n");

}




/*
* It implements the Terminal Response command
*/
void  _102221_terminalResponse(void) {

//	tFileHeaderAddr   fhaFile;
//	tFileHeader		  fhFile;
	
	DBG_FUNC_ENTRY(">>[102221] _102221_terminalResponse() \n");

	if (P1_BYTE || P2_BYTE) {
		/*Parameters are incorrect must be '0000'*/
		DBG_ERR("[102221] _102221_terminalResponse: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[102221] _102221_terminalResponse() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	/* Check if all mandatory TLVs are present in the command data we have fixed the P3 part as 0C*/
	if (P3_BYTE < LEN_P3_TERMINAL_RESPONSE)	 {
		
		THROW(EXCEP_TECHNICAL_PROBLEM);

	}
	/* Check the TR Tags*/
	if ((COMMAND_DATA[0] != TAG_COMMAND_DEATAILS) || (COMMAND_DATA[5] != TAG_DEVICE_IDENTITES) || (COMMAND_DATA[9] != TAG_RESULT)) {

		THROW(EXCEP_TECHNICAL_PROBLEM);

	}
	/*check if there is not any more proactive commands or it is normal ending so set gumbProactiveSession = 00*/
	if (0x0000 == gudbProactiveLen) {

		gumbProactiveSession = 0x00;

		if (0x01 != _102226_ProcessRfm(0x00)) {

			_31115_preparePOR(POR_OK);
		}
	}	
}
	


/*
* It implements the Envelope command
*/
void _102221_envelope(void) {

	DBG_FUNC_ENTRY(">>[102221] _102221_envelope() \n");

	if (P1_BYTE || P2_BYTE) {
		/*Parameters are incorrect, must be '0000'*/
		DBG_ERR("[102221] _102221_envelope: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[102221] _102221_envelope() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}

	/*Command will be dispatched to USIM Envelope handler*/
	DBG_FUNC("[102221] _102221_envelope: Command is dispatched to uSIM Envelope handler \n");
	_31111_Envelope(COMMAND_DATA);

	DBG_FUNC_EXIT("<<[102221] _102221_envelope() \n");
}

/*
* It implements the Fetch command
*/
void _102221_fetch(void) {

	
	DBG_FUNC_ENTRY(">>[102221] _102221_fetch() \n");

	if (P1_BYTE || P2_BYTE) {
		/*Parameters are incorrect must be '0000'*/
		DBG_ERR("[102221] _102221_fetch: Wrong P1-P2\n");
		DBG_FUNC_EXIT("<<[102221] _102221_fetch() \n");
		THROW(EXCEP_WRONG_P1_P2);
	}
	

	if (0x00 == gudbProactiveLen) {  //check gudbProactiveLen is zero.

		DBG_ERR("[102221] _102221_fetch: TECHNICAL PROBLEM \n");
		DBG_FUNC_EXIT("<<[102221] _102221_fetch() \n");
		THROW(EXCEP_TECHNICAL_PROBLEM);

	}
	 if (P3_BYTE != gudbProactiveLen) {  //incoming Le is not equal to gudbProactiveLen.

		DBG_ERR("[102221] _102221_fetch: Wrong P3\n");
		DBG_FUNC_EXIT("<<[102221] _102221_fetch() \n");
		THROW2(EXCEP_WRONG_P3, (uMonoByte)gudbProactiveLen);
	}

	gudbOutputLen = gudbProactiveLen;
	/*store the proactive cmd */
	mem_cpy(RESPONSE_BUFF, PROACTIVE_BUFF, gudbProactiveLen);
	/*Reset the remaining fetch*/
	gudbProactiveLen = 0x00;
	DBG_FUNC_EXIT("<<[102221] _102221_fetch() \n");
}
