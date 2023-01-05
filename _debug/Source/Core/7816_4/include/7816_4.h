#include "7816_4_extern.h"
#include "SystemPage.h"
#include "BitDefines.h"

#ifndef _7816_4
#define _7816_4

/*
* Macro defines
*/
// offsets for Record of Key config file, acc. to type tKeyConfigRecord
#define OFFSET_KEY_CONFIG_VERIFY_PIN_VALUE_RECORD							(0x00)												// same as OFFSET_KEY_CONFIG_VERIFY_PIN + OFFSET_RELATIVE_PIN_VALUE_RECORD
#define OFFSET_KEY_CONFIG_VERIFY_LIMIT										(OFFSET_KEY_CONFIG_VERIFY_PIN_VALUE_RECORD + 0x01) // same as OFFSET_KEY_CONFIG_VERIFY_PIN + OFFSET_RELATIVE_LIMIT
#define OFFSET_KEY_CONFIG_VERIFY_COUNTER									(OFFSET_KEY_CONFIG_VERIFY_LIMIT + 0x01)				// same as OFFSET_KEY_CONFIG_VERIFY_PIN + OFFSET_RELATIVE_COUNTER
#define OFFSET_KEY_CONFIG_UNBLOCK_PIN_VALUE_RECORD							(OFFSET_KEY_CONFIG_VERIFY_COUNTER + 0x01)			// same as OFFSET_KEY_CONFIG_UNBLOCK_PIN + OFFSET_RELATIVE_PIN_VALUE_RECORD
#define OFFSET_KEY_CONFIG_UNBLOCK_LIMIT										(OFFSET_KEY_CONFIG_UNBLOCK_PIN_VALUE_RECORD + 0x01) // same as OFFSET_KEY_CONFIG_UNBLOCK_PIN + OFFSET_RELATIVE_LIMIT
#define OFFSET_KEY_CONFIG_UNBLOCK_COUNTER									(OFFSET_KEY_CONFIG_UNBLOCK_LIMIT + 0x01)			// same as OFFSET_KEY_CONFIG_UNBLOCK_PIN + OFFSET_RELATIVE_COUNTER
#define OFFSET_KEY_CONFIG_PIN_STATE											(OFFSET_KEY_CONFIG_UNBLOCK_COUNTER + 0x01)
#define OFFSET_KEY_CONFIG_KEY_REFERENCE										(OFFSET_KEY_CONFIG_PIN_STATE + 0x01)
#define LENGTH_RECORD_OF_KEY_CONFIG											(OFFSET_KEY_CONFIG_KEY_REFERENCE + 0x01)

#define OFFSET_KEY_CONFIG_VERIFY_PIN										(OFFSET_KEY_CONFIG_VERIFY_PIN_VALUE_RECORD)
#define OFFSET_KEY_CONFIG_UNBLOCK_PIN										(OFFSET_KEY_CONFIG_UNBLOCK_PIN_VALUE_RECORD)

/*Ensure these relative offsets must be same for Unblock key*/
#define OFFSET_RELATIVE_PIN_VALUE_RECORD									(OFFSET_KEY_CONFIG_VERIFY_PIN_VALUE_RECORD - OFFSET_KEY_CONFIG_VERIFY_PIN)
#define OFFSET_RELATIVE_LIMIT												(OFFSET_KEY_CONFIG_VERIFY_LIMIT - OFFSET_KEY_CONFIG_VERIFY_PIN)
#define OFFSET_RELATIVE_COUNTER												(OFFSET_KEY_CONFIG_VERIFY_COUNTER - OFFSET_KEY_CONFIG_VERIFY_PIN)

//offsets for record of key value file, must be acc. to type tKeyValue
#define OFFSET_KEY_VALUE													(0x00)
#define LENGTH_RECORD_OF_KEY_VALUE											(OFFSET_KEY_VALUE + LENGTH_PIN_VALUE)

/*
* Typedefs
*/
/*Below typedefs are not created for use purposes, its only for understanding purposes.
* It explains structures of Proprietary files
*/
//Structure of each record of FID_PROP_ADF_LIST
typedef struct {
	uMonoByte		umbAidLength;
	uMonoByte		umbAidValue[LENGTH_MAX_AID];
	tFileHeaderAddr	fhaAdfFile;
}tAdfListRecord;


//Structure of each record of FID_PROP_KEYS_CONFIG
typedef struct {
	uMonoByte	umbRecordOfPinValue; // points to a record of file FID_PROP_KEYS_VALUE
	uMonoByte	umbVerifyRetryLimit;
	uMonoByte	umbVerifyRetryCounter;
	uMonoByte	umbRecordOfUnblockPinValue;  // points to a record of file FID_PROP_KEYS_VALUE
	uMonoByte	umbUnblockRetryLimit;
	uMonoByte	umbUnblockRetryCounter;
	tPinState	psPin; // Enable or Disable status of pin
	uMonoByte	umbKeyReference; //Its put in the end becuase wanna reuse searchRecordFile function for this also
} tKeyConfigRecord;

//Structure of each record of FID_PROP_KEYS_VALUE
typedef struct {
	uMonoByte	umbPinValue[LENGTH_PIN_VALUE];
}tKeyValue;


#endif // !_7816_4

