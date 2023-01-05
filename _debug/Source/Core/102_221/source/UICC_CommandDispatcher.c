#include "UICC_CommandDispatcher.h"

static tCommand commandList[] = {
//	UICC func---------------------------------------SIM Func----------------------------Instruction byte------------I/O and OTA flag-----//
	{_102222_createFile,							NULL,								INS_CREATE_FILE,			INS_INPUT|INS_NO_OTA_CMD						},
	{_102221_status,								_51011_status,						INS_STATUS,					INS_NO_INPUT|INS_NO_OTA_CMD|INS_ITS_CLA_8X		},
	{_102221_select,								_51011_select,						INS_SELECT,					INS_INPUT|INS_CMD_DATA_OPTIONAL|INS_OTA_CMD		},
	{_102221_readUpdateBinary,						_51011_readUpdateBinary,			INS_READ_BINARY,			INS_NO_INPUT|INS_OTA_CMD						},
	{_102221_readUpdateBinary,						_51011_readUpdateBinary,			INS_UPDATE_BINARY,			INS_INPUT|INS_OTA_CMD							},
	{_102221_readUpdateRecord,						_51011_readUpdateRecord,			INS_READ_RECORD,			INS_NO_INPUT|INS_OTA_CMD						},
	{_102221_readUpdateRecord,						_51011_readUpdateRecord,			INS_UPDATE_RECORD,			INS_INPUT|INS_OTA_CMD							},
	{_102221_deactivateActivateFile,				_51011_invalidateRehabilitate,		INS_DEACTIVATE_FILE,		INS_INPUT|INS_CMD_DATA_OPTIONAL|INS_OTA_CMD		},
	{_102221_deactivateActivateFile,				_51011_invalidateRehabilitate,		INS_ACTIVATE_FILE,			INS_INPUT|INS_CMD_DATA_OPTIONAL|INS_OTA_CMD		},
	{_102221_increase,								_51011_increaseRecord,				INS_INCREASE,				INS_INPUT|INS_OTA_CMD|INS_ITS_CLA_8X			},
	{_102221_search,								_51011_Seek,						INS_SEARCH,					INS_INPUT|INS_OTA_CMD							},
	{_102221_terminalProfile,						_51011_terminalProfile,				INS_TERMINAL_PROFILE,		INS_INPUT|INS_NO_OTA_CMD|INS_ITS_CLA_8X			},
	{_102221_terminalResponse,						_51011_terminalResponse,			INS_TERMINAL_RESPONSE,		INS_INPUT|INS_NO_OTA_CMD|INS_ITS_CLA_8X			},
	{_102221_envelope,								_51011_envelope,					INS_ENVELOPE,				INS_INPUT|INS_NO_OTA_CMD|INS_ITS_CLA_8X			},
	{_102221_fetch,									_51011_fetch,						INS_FETCH,					INS_NO_INPUT|INS_NO_OTA_CMD|INS_ITS_CLA_8X		},
	{_102221_getResponse,							_102221_getResponse,				INS_GET_RESPONSE,			INS_NO_INPUT|INS_OTA_CMD						},
	{NULL,											_51011_sleep,						INS_SLEEP,					INS_NO_INPUT|INS_NO_OTA_CMD						},
	{_31102_authenticate,							_51011_runGsmAlgorithm,				INS_AUTHENTICATE,			INS_INPUT|INS_NO_OTA_CMD						},
	{_102221_changePin,								_51011_changeChv,					INS_CHANGE_PIN,				INS_INPUT|INS_OTA_CMD							},
	{_102221_verifyPin,								_51011_verifyChv,					INS_VERIFY_PIN,				INS_INPUT|INS_CMD_DATA_OPTIONAL|INS_OTA_CMD		},
	{_102221_unblockPin,							_51011_unblockChv,					INS_UNBLOCK_PIN,			INS_INPUT|INS_CMD_DATA_OPTIONAL|INS_OTA_CMD		},
	{_102221_enablePin,								_51011_enableChv,					INS_ENABLE_PIN,				INS_INPUT|INS_OTA_CMD							},
	{_102221_disablePin,							_51011_disableChv,					INS_DISABLE_PIN,			INS_INPUT|INS_OTA_CMD							},
	{_102221_manageChannel,							NULL,								INS_MANAGE_CHANNEL,			INS_NO_INPUT|INS_NO_OTA_CMD						},
	{oasis_SetCardState,							oasis_SetCardState,					INS_SET_CARD_STATE,			INS_NO_INPUT|INS_NO_OTA_CMD						},
	{oasis_switchToBL,								oasis_switchToBL,					INS_SWITCH_TO_BL,			INS_INPUT|INS_NO_OTA_CMD						},
	{oasis_getMemoryConfig,							oasis_getMemoryConfig,				INS_GET_MEM_CONFIG,			INS_NO_INPUT|INS_NO_OTA_CMD						}
};


/*
* Check the presence of incoming instruction byte and return the command function along with type of command
* 
* @param pumbInput: stores type of command info of intended command
* 
* @return command function
*/
cmdFunc _102221_fetchTheCommandFunc(OUT_ARG puMonoByte pumbInput){

	//TODO: We can try BST here to improve the performance of searching
	uMonoByte idx; //index variable

	DBG_FUNC_ENTRY(">>[102_221] _102221_fetchTheCommandFunc() \n");

	*pumbInput = 0x00; //default

	DBG_FUNC("[102_221] _102221_fetchTheCommandFunc: Parse each element of Uicc Command dispatcher to find the instruction \n");

	//Loop from 0 to last element of commandList
	for (idx = 0;idx < sizeof(commandList)/sizeof(tCommand);idx++)
	{
		//Check if incoming instruction byte matches with
		if (commandList[idx].umbInsByte == INS_BYTE)
		{
			DBG_FUNC("[102_221] _102221_fetchTheCommandFunc: Instruction '%1' found\n", INS_BYTE);

			/*Save input status*/
			*pumbInput = commandList[idx].umbTypeOfCommand;

			if (INS_GET_RESPONSE != INS_BYTE) { // if command is not get response
				gudbOutputLen = 0x00;
			}

			//Returning the command function SIM or UICC one 
			if (NAA_SIM == gumbCurrentNaaApp) {//GSM application is active

				DBG_FUNC("[102_221] _102221_fetchTheCommandFunc: SIM function\n");
				DBG_FUNC_EXIT("<<[102_221] _102221_fetchTheCommandFunc() \n");
				return commandList[idx].SimCommandFunction;
			}
			/*USIM/UICC application is active*/
			else if((CLA_BYTE & MASK_CLA_8X) == (commandList[idx].umbTypeOfCommand & MASK_CLA_8X)) {
				
				DBG_FUNC("[102_221] _102221_fetchTheCommandFunc: UICC function\n");
				DBG_FUNC_EXIT("<<[102_221] _102221_fetchTheCommandFunc() \n");
				return commandList[idx].UiccCommandFunction;
			}
			else { //Instuction has different CLA byte than incoming one

				DBG_ERR("[102_221] _102221_fetchTheCommandFunc: Given instruction doesnt fall under any CLA byte\n");
				DBG_FUNC_EXIT("<<[102_221] _102221_fetchTheCommandFunc() \n");
				return NULL;
			}

		}
	}

	DBG_WARN("[102_221] _102221_fetchTheCommandFunc: No instruction found\n");
	DBG_FUNC_EXIT("<<[102_221] _102221_fetchTheCommandFunc() \n");
	// No instruction matched
	return NULL;
}