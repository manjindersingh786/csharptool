#ifndef _31_111_EXTERN_H
#define _31_111_EXTERN_H
#include "types.h"
/*
* Macro Defines
*/
#define TAG_ENV_SMS_PP_DATA_DOWNLOAD			(0xD1)
#define LENGTH_OF_TWO_BYTES						(0x81)
#define LENGTH_OF_THREE_BYTES					(0x82)
#define TAG_OF_DEVICE_IDENTITIES				(0x82)
#define LENGTH_OF_DEVICE_IDENTITIES				(0x02)
#define TAG_OF_ADDRESS							(0x86)
#define TAG_OF_SMS_TPDU							(0x8B)
#define PROACTIVE_BUFF                          (gumbProactive)

GLOBAL uMonoByte	gumbProactiveSession;//it is a indication for proactive session is ongoing or not gumbProactiveSession == '00' means not any proactive sess is ongoing and '01' means proact sess is ongoing
GLOBAL uMonoByte	gumbProactive[];
GLOBAL uDiByte		gudbProactiveLen;

/*
* It handles Envelope Command
*
* @param pumbEnvelopeData: Envelope Data
*/
extern void _31111_Envelope(IN_ARG puMonoByte pumbEnvelopeData);

/*
* It checks the profile(CAT facilities) that terminal supports
*
* @param byteIndex: byte that contain the facility.
* @param bitNo: bit which indicate that facility supported or not.
*
* @return TRUE if facility supported by terminal, otherwise FALSE.
*/
extern boolean _31111_checkTerminalProfile(IN_ARG uMonoByte byteIndex, IN_ARG uMonoByte bitNo);

#endif // !_31_111_EXTERN_H
