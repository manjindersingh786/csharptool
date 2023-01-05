
#ifndef _ALGO_CHECKSUM_EXTERN_H 
#define _ALGO_CHECKSUM_EXTERN_H
#include "types.h"
#include "Oasis.h"

/* ---------- Defines for 16-bit CCITT CRC calculation -------------------------------------------------------------- */
#define CRC_16_POLYNOMIAL                   (0x8408)/*revese polynomial value as per implementation(normal value {0x1021} https://crccalc.com/?crc=01020304&method=crc32&datatype=hex)*/
#define CRC_16_INIT_VALUE                   (0xFFFF)

/* ---------- Defines for 32-bit CCITT CRC calculation -------------------------------------------------------------- */
#define CRC_32_POLYNOMIAL                   (0xEDB88320)/*/*revese polynomial value as per implementation(normal value {0x04C11DB7} https://crccalc.com/?crc=01020304&method=crc32&datatype=hex)*/
#define CRC_32_INIT_VALUE                   (0xFFFFFFFF)


/*
* Typedefs
*/
typedef enum {
	CHECKSUM_CRC16,
	CHECKSUM_CRC32
}tChecksumAlgo;


typedef struct {
	tChecksumAlgo	crcType; //Type of CRC
	uQuadByte		uqbIntermediateResult; // stores 2 byte or 4 byte intermediate result
}tChecksumObject;

typedef tChecksumObject* ptChecksumObject;


/*
* It initializes the Checksum context as per given input
*
* @param csContext:  stores all info. requried to perform CRC
* @param rcAlgo: Redundancy checksum type
*
* @pre-requisite: csContext must not hold partial data of other checksum operation.
*/
extern void algo_chksm_init(OUT_ARG ptChecksumObject csContext, \
	IN_ARG	 tChecksumAlgo		rcAlgo);


/*
* It performs the checksum calculation on given input data as per csContext
*
* @param csContext:  stores all info. requried to perform CRC
* @param pumbInput: Input data to which checksum calculation is done
* @param udbInputLen: Input data length in pumbInput
*
* @pre-requisite:	algo_chksm_init() must be called prior to this function.
*					pumbInput  must be udbInputLen long array
*/
extern void algo_chksm_update(INOUT_ARG	ptChecksumObject csContext, \
	IN_ARG puMonoByte			pumbInput, \
	IN_ARG uDiByte				udbInputLen);


/*
* It finalizes the CRC checksum of last input data, It Resets csContext
*
* @param csContext:  stores all info. requried to perform CRC
* @param pumbInput: Input data to which checksum calculation is done
* @param udbInputLen: Input data length in pumbInput
* @param pumbOutput: result data after operation will be stored here
*
* @pre-requisite:	algo_chksm_init() must be called prior to this function.
*					pumbInput and pumbOutput must be long enough
*/
extern void algo_chksm_finalize(INOUT_ARG	ptChecksumObject csContext, \
	IN_ARG puMonoByte			pumbInput, \
	IN_ARG uDiByte				udbInputLen, \
	OUT_ARG puMonoByte			pumbOutput);

#endif // !_ALGO_CHECKSUM_EXTERN_H
