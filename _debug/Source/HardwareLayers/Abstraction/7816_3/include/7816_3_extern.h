#include"types.h"
#include "Constants.h"
#ifndef _7816_3_EXTERN_H
#define _7816_3_EXTERN_H


/*
* Macro Defines
*/
#define CLA_BYTE											(gumbCmdApdu[INDEX_CLA])
#define INS_BYTE											(gumbCmdApdu[INDEX_INS])
#define P1_BYTE												(gumbCmdApdu[INDEX_P1])
#define P2_BYTE												(gumbCmdApdu[INDEX_P2])
#define P3_BYTE												(gumbCmdApdu[INDEX_P3])

/*Lc: Length of incoming data*/
#define LC_BYTE												(P3_BYTE)	

/*Le: Length of expected data*/
#define LE_BYTE												(P3_BYTE)
#define MAX_RESPONSE_LEN									(0x100)
#define COMMAND_APDU										(gumbCmdApdu)
#define COMMAND_DATA										(gumbCmdApdu + INDEX_CMD_DATA)
#define RESPONSE_BUFF										(COMMAND_DATA)
#define COMMAND_RESPONSE_DATA								(COMMAND_DATA)

/*
* TypeDefs
*/
/*Disclaimer: Think twice, thrice... before changing the values of below enums*/
typedef enum {
	PROCDEDURE_NULL_BYTE, // Null Procedure byte will never be used in code, as its managed by Chip itself. Written just for completeness purposes
	PROCEDURE_SEND_GET_RESPONSE,
	PROCEDURE_RESEND_SAME_COMMAND
}tProcedureByte;

/*
* Exported Variables
*/
GLOBAL uMonoByte	gumbCmdApdu[];
GLOBAL uDiByte		gudbOutputLen;



/*
* Exported functions
*/
extern void _7816_3_receiveCommandHeader(void);
extern void _7816_3_transmitAtr(void);
extern void _7816_3_ppsExchange(void);
extern void _7816_3_sendStatusWords(void);
extern void _7816_3_sendResponseData(void);
extern void _7816_3_receiveCommandData(void);

/*
* It sets status words acc. to requested procedure bytes
*
* @param pbByte: contains requested procedure byte
*
* @precondition: Requestor ensure that Procedure byte can never be PROCDEDURE_NULL_BYTE
*/
extern void _7816_3_setProcedureByte(void);

#endif // !_7816_3_EXTERN_H
