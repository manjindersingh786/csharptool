
#ifndef _102_221_EXTERN_H
#define _102_221_EXTERN_H
#include "types.h"
#include "31_111_extern.h"
#include "7816_3_extern.h"
#include "7816_4_extern.h"
#include "Constants.h"
#include "MemoryManager_extern.h"
#include "Oasis.h"
#include "ChannelManager_extern.h"
#include "31_115_extern.h"

GLOBAL uMonoByte gumbcommandSource;

#define SRC_TERMINAL			(0x00)
#define SRC_RFM					(0x01)

/*
* Macros Defines
*/

//Constant Macros
//Macros CLA related 
#define MASK_INS_INPUT												(0x01)
#define MASK_INS_OTA												(0x02)
#define MASK_INS_CMD_DATA											(0x04) // This mask is significant if Command is input
#define MASK_CLA_8X													(0x80)
#define INS_INPUT													(0x01)
#define INS_OTA_CMD													(0x02)
#define INS_CMD_DATA_OPTIONAL										(0x04)
#define INS_ITS_CLA_8X												(0x80)
#define INS_NO_OTA_CMD												(0x00)
#define INS_NO_INPUT												(0x00)
#define INS_CMD_DATA_MANDATORY										(0x00)
#define MASK_HIGH_UPDATE											(0x80)
#define MASK_READ_UPDATE											(0x40)
#define MASK_OTA_UPDATE											    (0x01)
#define MASK_SPECIAL_FILE_INFO_RFU_BITS								(0x3F)
#define HIGHLY_UPDATE_FILE											(0x01)
#define READABLE_UPDATABLE_WHEN_DEACTIVATED							(0x01)
#define OTA_UPDATABLE							                    (0x01)
#define OTA_NOT_UPDATABLE							                (0x00)
#define LENGTH_VALID_FOR_CHANGE_UNBLOCK_PIN							(LENGTH_PIN_VALUE + LENGTH_PIN_VALUE)
#define MASK_FILE_SELECTION											(0xF8)
#define CURRENT_SELECTED_FILE										(0x00)

//Status Words
#define COMMAND_NOT_ALLOWED											(0x6900)
#define LOGICAL_CHANNEL_NOT_SUPPORTED								(0x6881)
#define NOT_ENOUGH_MEMORY											(0x6A84)
#define FILE_ID_ALREADY_EXISTS										(0x6A89)
#define DF_NAME_ALREADY_EXISTS										(0x6A8A)
#define FILE_NOT_FOUND												(0x6A82)
#define FUNCTION_NOT_SUPPORTED										(0x6A81)
#define NO_EF_SELECTED												(0x6986)
#define COMMAND_INCOMPATIBLE_WITH_FILE								(0x6981)
#define SECURITY_NOT_SATISIFIED										(0x6982)
#define RECORD_NOT_FOUND											(0x6A83)
#define SELECTED_FILE_INVALIDATED									(0x6283)
#define MAX_VALUE_REACHED_OF_RECORD									(0x9850)
#define SEND_GET_RESPONSE											(0x6100)
#define RESEND_PREVIOUS_COMMAND										(0x6C00)
#define REFERENCED_DATA_INVALIDATED									(0x6984)
#define REFERENCED_DATA_NOT_FOUND									(0x6A88)
#define PIN_VERIFICATION_FAILED										(0x63C0)
#define AUTHENTICATION_BLOCKED										(0x6983)
#define CONDITIONS_OF_USE_NOT_SATISFIED								(0x6985)
#define AUTHENTICATION_ERROR										(0x9862)
#define OPERATION_CONTRADICTS_PIN_STATUS							(REFERENCED_DATA_INVALIDATED)
#define PIN_NOT_FOUND												(REFERENCED_DATA_NOT_FOUND)
#define NORMAL_ENDING_PROACTIVE_CMD                                 (0x9100)



//Conditional Macros
#define SIM															(CLA_BYTE == CLA_SIM)
#define _102221_IS_FILE_ACTIVATED(x)								(LCSI_ACTIVATED == x)
#define _102221_IS_FILE_RW_WHEN_DEACTIVATED(x)						(READABLE_UPDATABLE_WHEN_DEACTIVATED == x)

//Commands Instruction
#define INS_STATUS													(0xF2)
#define INS_SELECT													(0xA4)
#define INS_READ_BINARY												(0xB0)
#define INS_UPDATE_BINARY											(0xD6)
#define INS_READ_RECORD												(0xB2)
#define INS_UPDATE_RECORD											(0xDC)
#define INS_DEACTIVATE_FILE											(0x04)
#define INS_ACTIVATE_FILE											(0x44)
#define INS_INCREASE												(0x32)
#define INS_SEARCH													(0xA2)
#define INS_GET_RESPONSE											(0xC0)
#define INS_TERMINAL_PROFILE										(0x10)
#define INS_ENVELOPE												(0xC2)
#define INS_TERMINAL_RESPONSE										(0x14)
#define INS_FETCH													(0x12)
#define INS_AUTHENTICATE											(0x88)
#define INS_VERIFY_PIN												(0x20)
#define INS_CHANGE_PIN												(0x24)
#define INS_DISABLE_PIN												(0x26)
#define INS_ENABLE_PIN												(0x28)
#define INS_UNBLOCK_PIN												(0x2C)
#define INS_MANAGE_CHANNEL											(0x70)


//NAA applications
#define NAA_UICC													(0x00)
#define NAA_SIM														(0x01)
#define NAA_USIM													(0x02)

//Authenticate Command related Macros
#define P2_APPLICATION_SPECIFIC										(0x80)
#define LENGTH_CHALLENGE_DATA										(0x10)
#define LENGTH_AUTH_KEY												(0x10)

#define TAG_AUTH_SYNCH_FAILURE_TAG									(0xDC)
#define TAG_AUTH_SUCCESS_TAG										(0xDB)

#define LENGTH_MAX_ARRAY_WORKING_BUFFER								(0x100)

/*
* TypeDefs
*/
/*
* Structure declarations
*/


/*
* Variables declaration
*/

/*Stores currently selected NAA application i.e. UICC,USIM,SIM*/
GLOBAL uMonoByte gumbCurrentNaaApp;

//Points to current channel
GLOBAL tChannelContext* pccCurrentChannel;

/*Methods Declaration*/
extern void _102221_status(void);
extern void _102221_select(void);
extern void _102221_readUpdateBinary(void);
/*
* It implements the UICC Read Record and Update Record command as per ETSI 102 221
*
*/
extern void _102221_readUpdateRecord(void);

/*
* It implements the UICC Deactivate and Activate command as per ETSI 102 221
*
*/
extern void _102221_deactivateActivateFile(void);

/*
* It implements the Increase command as per ETSI 102 221
*
*/
extern void _102221_increase(void);

/*
* It implements the Search command as per ETSI 102 221
*
*/
extern void _102221_search(void);

/*
* It implements the GET RESPONSE command as per 102 221
*/
extern void _102221_getResponse(void);

/*
* It implements the Terminal Profile command
*/
extern void _102221_terminalProfile(void);

/*
* It implements the Terminal Response command
*/
extern void _102221_terminalResponse(void);

/*
* It implements the Envelope command
*/
extern void _102221_envelope(void);

/*
* It implements the Fetch command
*/
extern void _102221_fetch(void);

/*
* It implements UICC Authenticate command
*/
extern void _102221_authenticate(void);

/*
* It implements UICC Manage Channel command
*/
extern void _102221_manageChannel(void);

//****************************PIN commands related functions
/*
* It throws the error exception as per give Pin status code
* 
* @param pscVal: contains status code sent by Pin functions
*/
extern void _102221_throwStatusResponseAsPer(tPinStatusCode pscVal);
/*
* It implements the Change PIN command as per ETSI 102 221
*/
extern void _102221_changePin(void);
/*
* It implements the Disable PIN command as per ETSI 102 221
*/
extern void _102221_disablePin(void);

/*
* It implements the Enable PIN command as per ETSI 102 221
*/
extern void _102221_enablePin(void);
/*
* It implements the Verify PIN command as per ETSI 102 221
*/
extern void _102221_verifyPin(void);

/*
* It implements the Unblock PIN command as per ETSI 102 221
*/
extern void _102221_unblockPin(void);

/*
* It verifies the security condition for given operation on given file
*
* @param amOperationMode: Read/Update/Increase/Invalidate/Deactivate
* @param fhaFile: File header address of target file
*/
extern tAccessResult  _102221_verifyAccessCondition(IN_ARG tAccessMode amOperation, IN_ARG tFileHeaderAddr fhaFile);

/*It selects a file using select by path functionality

* @param parentDf: It's the address of Current directory file
@param pumbPath: It's the Current Directory File Id
@param umbPathLength: It's Length of File Id
*/
extern tFileHeaderAddr _102221_selectByPath(tFileHeaderAddr parentDf, puMonoByte pumbPath, uMonoByte umbPathLength);
/*
* It prepares length of FCP response
*
* @param fhaADF: Contains address of file.
* @param pumbAid: contains an array.
*
* @return : length of FCP response.
*/
extern uMonoByte _102221_prepareFCPResponseForFile(IN_ARG tFileHeaderAddr, OUT_ARG puMonoByte);

/**/
#endif // !_102_221_EXTERN_H
