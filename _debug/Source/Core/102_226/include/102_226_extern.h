#ifndef _102_226_EXTERN_H
#define _102_226_EXTERN_H
#include "types.h"

/*global variable for index to recieve commands from secured message*/
extern uDiByte gudbCommandIndex;

/*
* Macros Defines
*/
#define SIZE_CMD_HEADER						(0x05)
#define OFFSET_SECURED_MESSAGE_LENGTH		(0x0002)
#define OFFSET_SECURED_MESSAGE				(0x0004)
#define MAX_CONCATED_PACKETS				(0x04)
#define OFFSET_RESPONSE_MESSAGE				( ( MAX_CONCATED_PACKETS * 0x96 ) + OFFSET_SECURED_MESSAGE ) /*taking 150 bytes for 1 concatenated packet*/
#define MAX_VALUE							(0xFFFF)
#define RECORD_LENGTH_RFMPR					(0x1D)
#define OFFSET_AID_LENGTH					(0x04)
#define MINIMUM_COMMAND_LENGTH				(0x04)
#define MASK_B2_B1							(0x03)
#define B2_B1_FOR_COMPACT					(0x00)

#define TAG_EXPANDED_COMMAND_DEFINITE		(0xAA)
#define TAG_EXPANDED_COMMAND_INDEFINITE		(0xAE)
#define TAG_C_APDU							(0x22)
#define TAG_IMMEDIATE_ACTION				(0x81)
#define TAG_ERROR_ACTION					(0x82)
#define TAG_SCRIPT_CHAINING					(0x83)
#define TAG_BAD_FORMAT						(0x90)
#define INDEFINITE_LENGTH_INDICATOR			(0x80)
#define INDEFINITE_LENGTH_EXIT_INDICATOR	(0x00)

#define TAG_EXPANDED_RESPONSE_DEFINITE		(0xAB)
#define TAG_EXPANDED_RESPONSE_INDEFINITE	(0xAF)
#define TAG_R_APDU							(0x23)

#define BAD_FORMAT_WRONG_LENGTH				(0x02)
#define BAD_FORMAT_UNKNOWN_TAG				(0x01)


#define TAG_NO_OF_EXECUTED_COMMAND_TLV		(0x80)
#define LEN_NO_OF_EXECUTED_COMMAND_TLV		(0x01)


/*
* Functions Declaration
*/

/*
*It search the given TAR with Different TAR's and return Acess_Domain and MSL
*@fhaTAR It's Containes the TAR
*@pumbrfmpr It's Containes the Access_Domain, MSL
*/
extern boolean _102226_RfmParameters(IN_ARG puMonoByte fhaTAR, OUT_ARG puMonoByte pumbrfmpr);

/*
* It processes (execute) the RFM commands.
*/
extern uMonoByte _102226_ProcessRfm(IN_ARG uMonoByte umbRecordNo);

extern uDiByte _102226_ResponseTLV(IN_ARG uDiByte udbResponseIndex);

extern void _102226_receiveCommandHeader(void);
extern void _102226_sendStatusWords(void);
extern void _102226_sendResponseData(void);
extern void _102226_receiveCommandData(void);

#endif // !_102_226_EXTERN_H
