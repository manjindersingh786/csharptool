#include "CommandRouter.h"

/*
* Holds information if given command is first after ATR
* TRUE: If this is first command
* FALSE: False
*/
boolean bFirstCommandAfterATR=TRUE; 

static const cmdFunc sourceCommApis[0x02][0x05] = {
											{_7816_3_receiveCommandHeader,
											 _7816_3_receiveCommandData,
											 _7816_3_sendResponseData,
											 _7816_3_setProcedureByte,
											 _7816_3_sendStatusWords },

											{_102226_receiveCommandHeader,
											 _102226_receiveCommandData,
											 _102226_sendResponseData,
											 _7816_3_setProcedureByte, //functionality of setProcedureByte function is same for both the sources
											 _102226_sendStatusWords }
											};

void cmdRout_commandRouter(void) {

	cmdFunc		commandToCall; //stores command function
	volatile	uMonoByte	umbTypeOfCmd = 0x00; // stores type of command
	uMonoByte	umbChannel = 0x00;

	DBG_FUNC_ENTRY(">>[CommandRouter] cmdRout_commandRouter() \n");
	
	/*Receive header of Command*/
	sourceCommApis[gumbcommandSource][0x00]();
	
	/*Check if this is first command after ATR*/
	if (TRUE == bFirstCommandAfterATR) {
		
		/*Requesting Chip communication module to keep sending NULL procedure byte for additional Processing time*/
		enableNullProcedure();
		
		/*Initalize configuration of OS (if any)*/
		osInit_osInit();
		
		/*Disable the NULL procedure byte*/
		disableNullProcedure();
		
		/* Reset the First command flag*/
		bFirstCommandAfterATR = FALSE;
	}
	
	TRY{

		if (NAA_UICC == gumbCurrentNaaApp) {

			if (CLA_SIM == CLA_BYTE) { //If CLA byte is 0xA0 initially

				/*GSM session activated*/
				gumbCurrentNaaApp = NAA_SIM;
			}
			else {
				/*clear the response prepared for SIM application in OS_Init*/
				gudbOutputLen = 0x00;

				/*USIM session activated*/
				gumbCurrentNaaApp = NAA_USIM;
			}
		}
		/*Check if CLA is valid*/
		if (gumbCurrentNaaApp == NAA_SIM)
		{
			if (CLA_SIM != CLA_BYTE) {//check if class byte is not 'A0'
				gudbOutputLen = 0x00;
				DBG_ERR("[CommandRouter] cmdRout_commandRouter: Invalid Class byte not 'A0'  \n");
				THROW(EXCEP_INVALID_CLASS);
			}
		}
		//If USIM/UICC
		else if(CLA_BYTE & MASK_CLA) { //check if class byte is not '0X' or '8X'
			gudbOutputLen = 0x00;
			DBG_ERR("[CommandRouter] cmdRout_commandRouter: Invalid Class byte neither '0X' nore '8X' \n");
			THROW(EXCEP_INVALID_CLASS);
		}
		
		DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Valid Class Byte\n");

		//Call Command dispatcher to call command function
		commandToCall = _102221_fetchTheCommandFunc((puMonoByte)&umbTypeOfCmd);

		/*get current logical channel*/
		if (SRC_RFM == gumbcommandSource) {

			//Set current channel context
			umbChannel = RFM_LOGICAL_CHANNEL;
		}
		else {
			umbChannel = CLA_BYTE & MASK_LOGICAL_CHANNEL;
		}


		if (gumbChannelsState & (SHIFT_LEFT_BY_N_BITS(0x01, umbChannel))) { //check if channel closed

			//Channel is closed
			DBG_ERR("[CommandRouter] cmdRout_commandRouter: Channel %1 is closed\n", umbChannel);
			THROW(EXCEP_LOGICAL_CHANNEL_NOT_SUPPORTED);
		}

		//Set current channel context
		pccCurrentChannel = &ccContexts[umbChannel];

		//Check if it is NULL, i,e. command not supported by dispatcher
		if ((commandToCall == NULL)
			||
			(	(NO_FILE_PRESENT == pccCurrentChannel->fhaCurrentDf)
				&&
				(INS_BYTE != INS_CREATE_FILE) 
			)/*If No file is created , so first command must be a Create File Command*/
			||
			( 
				(SRC_RFM == gumbcommandSource) 
				&& 
				(INS_OTA_CMD != (umbTypeOfCmd & MASK_INS_OTA)) ) 
			)//If source is RFM and command is not OTA applicable
		{
			//Invalid instruction
			DBG_ERR("[CommandRouter] cmdRout_commandRouter: There is no such instruction '%1' in current application\n", INS_BYTE);
			gudbOutputLen = 0x00;
			THROW(EXCEP_INVALID_INS);
		}

		DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Restore context of channel %1\n", CLA_BYTE & MASK_LOGICAL_CHANNEL);

		/*check if command need rest of the input command data*/
		if (INS_INPUT == (umbTypeOfCmd & MASK_INS_INPUT))
		{
			DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Command of type Case 3 or Case 4 \n");

			if (P3_BYTE) {// As this is input command P3 cant be 00

				DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Fetching Command Data of length %1\n", P3_BYTE);

				/*Receive remaining data of input command*/
				sourceCommApis[gumbcommandSource][0x01]();

				/*input has been taken*/
				umbTypeOfCmd = INS_INPUT;
			}
			else if(INS_CMD_DATA_MANDATORY == (umbTypeOfCmd & MASK_INS_CMD_DATA)) { // p3 is 00 and Its mandatory to have input

				DBG_ERR("[CommandRouter] cmdRout_commandRouter: No command data %1\n", P3_BYTE);
				gudbOutputLen = 0x00;
				/*Wrong p3*/
				THROW2(EXCEP_WRONG_P3,0x00);
			}
			else {
				/*No input*/
				umbTypeOfCmd = INS_NO_INPUT;
			}
		}

		DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Calling command pointer function\n");
		
		if (SRC_TERMINAL == gumbcommandSource) { /*source is terminal*/
			/*Requesting Chip communication module to keep sending NULL procedure byte for additional Processing time*/
			enableNullProcedure();

			//Call the command function
			commandToCall();

			/*Disable the NULL procedure byte*/
			disableNullProcedure();
		}
		else { /*source is RFM*/
			//Call the command function
			commandToCall();
		}

		DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Normal exeuction of Command function\n");

		/*Ideally control shall not reach here as command function must
		* throw exception. But to just play safe side send normal ending
		*/ 
		THROW(EXCEP_NORMAL_ENDING);
	}
	CATCH{

		DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Exception catched here\n");

		if ((INS_INPUT != umbTypeOfCmd) && gudbOutputLen) // if command is not input one and response is prepared
		{
			DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Sending response data of length '%2' prepared by the Command \n", gudbOutputLen);

			//Sending prepared response data
			sourceCommApis[gumbcommandSource][0x02]();
		}

		if (0x00 != gudbOutputLen) { // command is input one or still there is some response to fetch

			/*Set procedure byte*/
			sourceCommApis[gumbcommandSource][0x03]();
		}

		/*convert status word to 91XX store value of XX in P3*/
		/*Check if Exception thrown is Normal ending and Proactive command is prepared*/
		if ((IS_EXCEPTION(EXCEP_NORMAL_ENDING))
			&&
			(gudbProactiveLen > 0x00)) {

			/*It means Proactive command has been already prepared by SIM, and Terminal has to FETCH it*/
			/*Set status words to 91XX, where XX is proactive command length*/
			UPDATE_STATUS_WORDS(EXCEP_NORMAL_END_PENDING_PROACTIVE_CMD, gudbProactiveLen);
		}

		DBG_FUNC("[CommandRouter] cmdRout_commandRouter: Sending Status words\n");
		DBG_FUNC_EXIT("<<[CommandRouter] cmdRout_commandRouter() \n");

		//Sending Status Words
		sourceCommApis[gumbcommandSource][0x04]();
	}
}