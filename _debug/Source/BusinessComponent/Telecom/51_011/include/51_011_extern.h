#ifndef _51_011_EXTERN_H
#define _51_011_EXTERN_H
#include "types.h"
#include "7816_4_extern.h"



/*
* Macro Defines
*/
#define SIM_FILE_OR_PATTERN_NOT_FOUND					(0x9404)
#define SIM_NO_EF_SELECTED								(0x9400)
#define SIM_FILE_INCONSISTENT_WITH_COMMAND				(0x9408)
#define SIM_OUT_OF_RANGE								(0x9402)
#define SIM_ACCESS_CONDITION_NOT_FULFILLED				(0x9804)
#define SIM_SEND_GET_RESPONSE							(0x9F00)
#define SIM_RECORD_NOT_FOUND							(0x9402)
#define SIM_IN_CONTRADICTION_WITH_INVALIDATION			(0x9810)
#define SIM_AUTHENTICATION_BLOCKED						(0x9840)
#define SIM_IN_CONTRADICTION_WITH_CHV_STATUS			(0x9808)
#define SIM_PIN_NOT_INITIALIZED							(0x9802)
#define SIM_PIN_VERIFICATION_FAILED						(SIM_ACCESS_CONDITION_NOT_FULFILLED)

//Instructions
#define INS_SLEEP										(0xFA)

/*Typedefs*/
/*Type of file*/
typedef enum {
	FILE_IS_MF = 0x01,
	FILE_IS_DF = 0x02,
	FILE_IS_EF = 0x04
}tFileType;

/*
* Methods declarations
*/
/*
* It implements Seek command of 51.011
*/
extern  void _51011_Seek(void);

/*
* It implements the Select command as per 51011
*/
extern void _51011_select(void);

/*
* It implements the Status command as per 51011
*/
extern void _51011_status(void);

/*
* It implements the Terminal Profile command
*/
extern void _51011_terminalProfile(void);

/*
* It implements the Terminal Response command
*/
extern void _51011_terminalResponse(void);

/*
* It implements the Envelope command
*/
extern void _51011_envelope(void);

/*
* It implements the Fetch command
*/
extern void _51011_fetch(void);

/*
* It implements the Sleep command as per 51011
*/
extern void _51011_sleep(void);

/*
* It implements RUN GSM algorithm command as per ETSI 51.011
*/
extern void _51011_runGsmAlgorithm(void);

//***************************CHV commands
/*
* It implements the Change CHV command as per 51.011
*/
extern void _51011_changeChv(void);
/*
* It implements the Unblock CHV command as per 51.011
*/
extern void _51011_unblockChv(void);

/*
* It implements the Verify CHV command as per 51.011
*/
extern void _51011_verifyChv(void);
/*
* It implements the Enable CHV command as per 51.011
*/
extern void _51011_enableChv(void);

/*
* It implements the Disable CHV command as per 51.011
*/
extern void _51011_disableChv(void);

/*
* It implements Read/Update Record command of 51.011
*/
extern void _51011_readUpdateRecord(void);

/*
* It implements Read/Update Binary command of 51.011
*/
extern void _51011_readUpdateBinary(void);

/*
* It implements Increase record command of 51.011
*/
extern void _51011_increaseRecord(void);

/*
* It implements Invalidate Rehabilitate File command of 51.011
*/
extern void _51011_invalidateRehabilitate(void);

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
extern uMonoByte _51011_prepareFileResponse(IN_ARG tFileHeaderAddr, IN_ARG tFileType, OUT_ARG puMonoByte);
#endif // !_51_011_EXTERN_H
