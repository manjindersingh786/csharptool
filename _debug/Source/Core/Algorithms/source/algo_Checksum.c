#include "Algorithms.h"

/*
* It initializes the Checksum context as per given input
*
* @param csContext:  stores all info. requried to perform CRC
* @param rcAlgo: Redundancy checksum type
*
* @pre-requisite: csContext must not hold partial data of other checksum operation.
*/
void algo_chksm_init(OUT_ARG ptChecksumObject csContext, \
	IN_ARG	 tChecksumAlgo		rcAlgo) {

	
	/*Populate given information into csContext*/
	DBG_FUNC("[Algorithm]algo_chksm_init() Populate given information into csContext\n");
	csContext->crcType = rcAlgo;
	
	/*Reset Intermediate data to default value as per given algo type*/
	if (rcAlgo == CHECKSUM_CRC16) { 
	csContext->uqbIntermediateResult = ~CRC_16_INIT_VALUE;
	}
	else {
	csContext->uqbIntermediateResult = ~CRC_32_INIT_VALUE;
	}

}

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
void algo_chksm_update(INOUT_ARG	ptChecksumObject csContext, \
						IN_ARG puMonoByte			pumbInput, \
						IN_ARG uDiByte				udbInputLen) {

	
	/*Calculate checksum as per csContext*/

	/*store intermediate result data(partial checksum) in csContext->uqbIntermediateResult */
	csContext->uqbIntermediateResult = algo_crc(~(csContext->uqbIntermediateResult), csContext->crcType, pumbInput, udbInputLen);
	
}

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
void algo_chksm_finalize(INOUT_ARG	ptChecksumObject csContext, \
						IN_ARG puMonoByte			pumbInput, \
						IN_ARG uDiByte				udbInputLen, \
						OUT_ARG puMonoByte			pumbOutput) {


	/*Calculate checksum as per csContext*/

	/* if check inputLen is not null */
	if (NULL!= udbInputLen) {

		csContext->uqbIntermediateResult = algo_crc(~(csContext->uqbIntermediateResult), csContext->crcType, pumbInput, udbInputLen);

	}
	/*Store final checksum in pumbOutput*/
	if (csContext->crcType == CHECKSUM_CRC16) {      
		DBG_FUNC(">>[Algorithm]algo_crc() Store CheckSum Value for CRC 16 \n");
		pumbOutput[0x00] = (uMonoByte)(csContext->uqbIntermediateResult >> 8);
		pumbOutput[0x01] = (uMonoByte)csContext->uqbIntermediateResult;
	}
	else {
		DBG_FUNC(">>[Algorithm]algo_crc() Store CheckSum Value for CRC 32 \n");
		pumbOutput[0x00] = (uMonoByte)(csContext->uqbIntermediateResult >> 24);
		pumbOutput[0x01] = (uMonoByte)(csContext->uqbIntermediateResult >> 16);
		pumbOutput[0x02] = (uMonoByte)(csContext->uqbIntermediateResult >> 8);
		pumbOutput[0x03] = (uMonoByte)csContext->uqbIntermediateResult;
	}

	/*Reset csContext as we have concluded the operation by calling finalize function*/
	csContext->crcType = NULL;
	csContext->uqbIntermediateResult = NULL;

}