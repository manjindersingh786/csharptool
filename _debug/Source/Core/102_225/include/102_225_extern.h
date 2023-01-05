
#ifndef _102_225_EXTERN_H
#define _102_225_EXTERN_H
#include "types.h"
#include "MemoryManager.h"
#include "7816_3_extern.h"
#include "7816_4_extern.h"
#include "Algorithms_extern.h"
#include "Algorithms.h"
#include "102_226_extern.h"




/*
* Macros Defines
*/

#define LENGTH_UDHL												(0x01)
#define LENGTH_CPL												(LENGTH_UDHL + 0x01)
#define LENGTH_TAR												(LENGTH_CPL + 0x01)
#define LENGTH_CNTR												(LENGTH_TAR + 0x02)


#define OFFSET_UDHL												(0x00)
#define OFFSET_CPI												(OFFSET_UDHL + 0x01)
#define OFFSET_CPI_LEN											(OFFSET_CPI + 0x01)
#define OFFSET_CPL												(OFFSET_CPI_LEN + 0x01)
#define OFFSET_CHL												(OFFSET_CPL + LENGTH_CPL)
#define OFFSET_SPI1												(OFFSET_CHL + 0x01)
#define OFFSET_SPI2 											(OFFSET_SPI1 + 0x01)
#define OFFSET_KIC												(OFFSET_SPI2 + 0x01)
#define OFFSET_KID												(OFFSET_KIC + 0x01)
#define OFFSET_TAR												(OFFSET_KID + 0x01)
#define OFFSET_CNTR												(OFFSET_TAR + 0x03)

#define	LEN_SMS_TPDU_TO_UDL										(0x12)
#define	LENGTH_UDHL_TO_TAR										(OFFSET_TAR + 0x03)
#define	OFFSET_PADD_CNTR_IN_FILE								(OFFSET_SECURED_MESSAGE + LEN_SMS_TPDU_TO_UDL + LENGTH_UDHL_TO_TAR + LENGTH_CNTR)


#define	FILE_OFFSET_UDHL										(OFFSET_SECURED_MESSAGE + LEN_SMS_TPDU_TO_UDL)
#define	FILE_OFFSET_CPI											(FILE_OFFSET_UDHL + 0x01)
#define	FILE_OFFSET_CPI_LEN										(FILE_OFFSET_CPI + 0x01)
#define	FILE_OFFSET_CPL											(FILE_OFFSET_CPI_LEN + 0x01)
#define	FILE_OFFSET_CHL											(FILE_OFFSET_CPL + LENGTH_CPL)
#define	FILE_OFFSET_SPI1										(FILE_OFFSET_CHL + 0x01)
#define	FILE_OFFSET_SPI2										(FILE_OFFSET_SPI1 + 0x01)
#define	FILE_OFFSET_KIC 										(FILE_OFFSET_SPI2 + 0x01)
#define	FILE_OFFSET_KID 										(FILE_OFFSET_KIC + 0x01)
#define	FILE_OFFSET_TAR 										(FILE_OFFSET_KID + 0x01)
#define	FILE_OFFSET_CNTR 										(FILE_OFFSET_TAR + LENGTH_TAR)

#define OFFSET_DEC_CNTR											(0x00)
#define	OFFSET_DEC_PADD_CNTR									(OFFSET_DEC_CNTR + LENGTH_CNTR)
#define	OFFSET_DEC_CHECKSUM										(OFFSET_DEC_PADD_CNTR + 0x01)


#define OFFSET_ALGO												(0x00)
#define OFFSET_KEYID											(OFFSET_ALGO + 0x01)
#define OFFSET_KVN												(OFFSET_KEYID + 0x01)
#define OFFSET_LENGTH											(OFFSET_KVN + 0x01)

#define USIM_TOOLKIT_SECURITY_HEADER							(0x70)
#define MASK_ALGO_1												(0x03)
#define MASK_ALGO												(0x0C)
#define MASK_MSL_1												(0x03)
#define MASK_MSL_2												(0x18)
#define MASK_SPI2_CHECKSUM										(0x0C)
#define CIPHERING_SPI1											(0x04)
#define CIPHERING_SPI2											(0x10)
#define MASK_KVN_VALIDATION										(0x00)
#define MASK_SPI1_B5B4											(0x18)

#define CRYPTOGRAPHIC_CHECKSUM									(0x02)
#define	REDUNDANCY_CHECK										(0x01)
#define	NO_RC_CC_DS												(0x00)

#define FILE_OFFSET_TAR_IN_RFMPR								(0x1A)
#define OFFSET_START_CNTR										(0x15)
#define OFFSET_MSL												(0x03)
#define OFFSET_KEY_VALUE_LENGTH									(0x00)

#define MIN_CHL													(0x0D)
#define	CNTR_NO_AVAILABLE										(0x00)
#define	CNTR_AVAILABLE											(0x08)
#define CNTR_HIGHER_ONLY										(0x10)
#define	CNTR_ONE_HIGHER											(0x18)

/*RESPONSE STATUS CODE*/
#define POR_OK													(0x00)
#define RC_CC_DS_FAILED											(0x01)
#define	CNTR_LOW												(0x02)
#define	CNTR_HIGH												(0x03)
#define	CNTR_BLOCKED											(0x04)
#define	CIPHERING_ERROR											(0x05)
#define	UNIDENTIFIED_SECURITY_ERROR								(0x06)
#define	TAR_UNKNOWN												(0x09)
#define	INSUFFICIENT_SECURITY_LEVEL								(0x0A)
#define	MAX_CNTR												(0x000000FFFFFFFFFF)

#define NO_ALGOKIC												(0x00)
#define DES_ALGO												(0x01)
#define AES_ALGO												(0x02)
#define CRC_ALGO												(0x01)
#define CRC_16													(0x00)
#define CRC_32													(0x04)

#define ALGO_3DES												(0x10)
#define ALGO_AES												(0x11)
	
#define KIC														(0x00)
#define KID_CC													(0x01)
#define KID_RC													(0x02)
#define NO_RC_CC_DS_KID											(0x00)
#define RECORD_LENGTH_RFMPR										(0x1D)
#define FILE_LENGTH_KEY_VALUE									(0x21)
#define	LENGTH_CPL_TO_TAR										(0x0A)


#define INDEX_OF_KEY_SIZE										(0x00)
#define INDEX_OF_BLOCK_OFFSET								    (INDEX_OF_KEY_SIZE + 0x01)
#define INDEX_OF_ALGO								            (INDEX_OF_BLOCK_OFFSET + 0x01)
#define INDEX_OF_KEY_ID								            (INDEX_OF_ALGO + 0x01)
#define INDEX_OF_KVN								            (INDEX_OF_KEY_ID + 0x01)
#define RECORD_LENGTH_OF_KEY_MANAGEMENT						    (INDEX_OF_KVN + 0x01)
#define LENGTH_OF_BLOCK_SIZE_OF_KEY								(0x08)
#define LENGTH_OF_KEY_MANAGEMENT_PATTERN						(0x03)
#define LENGTH_MAC												(0x8C)
#define	LENGTH_BLOCK											(0x10)



/*
* Functions Declaration
*/

/*It parse the data as per 102 225
*
* @param umbUDL	contain User data length
* @param pumbSmsData SMS data
* @return record no. where TAR found
*/
extern uMonoByte _102225_VerifySecurityCommandHeader(IN_ARG uMonoByte umbUDL, IN_ARG puMonoByte pumbSmsData);

/*it validates the KIc and KID byte
* @param umbData contains KIc or KID byte
* @param pumbAlgoKeyidKVN contains algo, keyid and kvn
* @param umbKeyValueLength store length of key
* @param pumbKeyValue store key value
* @param pumbAlgo store algo for decryption and checksum calc.
*/
extern boolean _102225_validateKIcAndKID(INOUT_ARG puMonoByte umbData, INOUT_ARG puMonoByte pumbAlgoKeyidKVN, OUT_ARG puMonoByte pumbKeyValue);

/*it validates the counter
* @param uobCNTR contains incoming counter
* @param uobPreviousCNTR contains previous counter which stored in prop. file
* @return response status code for counter
*/
extern uMonoByte _102225_validationOfCNTR(IN_ARG uMonoByte umbSPI1, IN_ARG puMonoByte pumbCNTR, IN_ARG puMonoByte pumbPreviousCNTR);

/*It stores key value as per 102 225
*
* @param pumbData : Algo, KeyID and KVN
* @param pumbKeyValue : Key value
*/
extern uMonoByte _102225_KeyManagement(IN_ARG puMonoByte pumbData, OUT_ARG puMonoByte pumbKeyValue);

#endif // !_102_225_EXTERN_H
