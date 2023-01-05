#include "Algorithms.h"
#define	IV							(0x00)
/*
* It initializes the Signature context as per given input
*
* @param sgContext:  stores all info. requried to Signature  operation
* @param bcAlgoType: Type of Algo to use
* @param umbSignLength: Length of resultant signature output
* @param pumbKey: 1st element stores key length and following data is key value\
*
* @pre-requisite: sgContext must not hold partial data of other signature operation.
*/
uDiByte algo_sign_init( OUT_ARG	ptSignatureObject sgContext, \
						IN_ARG  tSignAlgo		  sgAlgoType, \
						IN_ARG  uMonoByte		  umbSignLength, \
						IN_ARG  puMonoByte			pumbKey) {

	DBG_FUNC_ENTRY(">>[Algorithm]algo_sign_init() \n");
	if (sgAlgoType != SIGN_DES_MAC && sgAlgoType != SIGN_AES_CMAC) {
		/*Warning Algo Not Suported*/
		DBG_ERR("[Algorithm]algo_sign_init() Algo Not Supported\n");
		return SIGN_NULL;
	}
	/*Populate given information into sgContext*/
	DBG_FUNC("[Algorithm]algo_sign_init() Populate given information into bcContext\n");
	sgContext->sgAlgoType = sgAlgoType;
	sgContext->umbSignatureLen = umbSignLength;
	/*key store*/
	mem_cpy(sgContext->umbKey, pumbKey, pumbKey[0x00]+0x01);
	/*Reset Intermediate data to zero as per given algo type*/
	/*for DES*/
	if (pumbKey[0x00] == 0x10 && sgAlgoType == SIGN_DES_MAC) {
		DBG_FUNC("[Algorithm]algo_bc_init() store keys for 3DES with 2 different keys\n");
		sgContext->umbKey[0x00] = 0x18;
		/*key store for 3DES with 2 Different keys*/
		mem_cpy(sgContext->umbKey + BLOCK_SIZE + 0x01, pumbKey + 1, DES_BLOCK_SIZE);
	}
	mem_set(sgContext->umbIntermediateResult, IV, BLOCK_SIZE);
	sgContext->umbRemainingData[0x00] = NULL;
	DBG_FUNC_EXIT("<<[Algorithm]algo_sign_init() Reset Intermediate data to zero as per given algo type \n");
	return umbSignLength;
}


/*
* It accumulates a signature of the source data as per Signature context
*
* @param sgContext:  stores all info. requried to Signature operation
* @param pumbInput: Input data to which signature algo is performed
* @param udbInputLen: Input data length in pumbInput
*
* @pre-requisite:	algo_sign_init() must be called prior to this function.
*					pumbInput must be udbInputLen long array
*/
uDiByte algo_sign_update(	OUT_ARG	ptSignatureObject sgContext, \
							IN_ARG puMonoByte			pumbInput, \
							IN_ARG uDiByte				udbInputLen) {


	uMonoByte	umbIndex = 0x00;
	uMonoByte	umblength = 0X00;
	uMonoByte	umblen;
	uMonoByte	umbBlock;
	uMonoByte	umbrlength;
	uMonoByte   tempvar = 0x00;
	uMonoByte   TEMP2;
	uMonoByte	umboffset = 0x00;
	uMonoByte	umbOffsetOut = 0x00;
	uMonoByte	umbOutput[0x10]; // max block size

	DBG_FUNC_ENTRY(">>[Algorithm]algo_sign_update() \n");
	if (sgContext->sgAlgoType == SIGN_DES_MAC)
	{
		umbBlock = DES_BLOCK_SIZE;
	}
	else if (sgContext->sgAlgoType == SIGN_AES_CMAC) {
		umbBlock = AES_BLOCK_SIZE;
	}
	else {
		return SIGN_NULL;
	}
	umbrlength = sgContext->umbRemainingData[0x00];
	//Case1 When input value and reamining data is null
	if (udbInputLen == NULL && umbrlength == NULL) {/*if input length is NULL and remaining Data is Null then*/
	
		return udbInputLen;

	}
	//Case2 When input value is null and reamining data is not null
	else if (udbInputLen == NULL && umbrlength != NULL) {/*if input length is zero and remaining Data is not Null then*/

		sgContext->umbRemainingData[0x00] = umbBlock;
		/*add padding in the end of data to final encryption*/
		mem_set(sgContext->umbRemainingData + ((umbBlock - umbrlength) + 0x01), 0x00, (umbBlock - umbrlength));
	}

	//case 3 and 4
	else if (udbInputLen != NULL) {

		if (umbrlength != NULL) {

			tempvar = umbBlock - umbrlength;
			if (tempvar > udbInputLen) {
				mem_cpy(sgContext->umbRemainingData + (umbrlength + 0x01), pumbInput, udbInputLen);
				TEMP2 = tempvar - udbInputLen;
				mem_set(sgContext->umbRemainingData + (umbrlength + 0x01 + udbInputLen), 0x00, TEMP2);
			}

			else {//(tempvar <= udbInputLen)
				sgContext->umbRemainingData[0] += tempvar;
				mem_cpy(sgContext->umbRemainingData + (umbrlength + 0x01), pumbInput, tempvar);
			}
			umblen = (udbInputLen - tempvar);
			umblength = (umblen - (umblen%umbBlock));
		}
		else {
			if (umbBlock > udbInputLen) {//	if incoming input is less than blocksize store input in remaining data

				sgContext->umbRemainingData[0x00] = (uMonoByte)udbInputLen;
				for (umbIndex = 0x00; umbIndex < udbInputLen; umbIndex++)
				{
					sgContext->umbRemainingData[umbIndex + 0x01] = pumbInput[umbIndex];
				}
				umblength = (udbInputLen - (udbInputLen%umbBlock));
				return umblength;
			}
			umblength = (udbInputLen - (udbInputLen%umbBlock)); //block aligned length in umblength
			
		}

	}

	/*if any remaining data present*/

	if (sgContext->umbRemainingData[0x00] != NULL) {


		/*Perform Encryption as per bcContext*/
		if (sgContext->sgAlgoType == SIGN_DES_MAC) {
			/*if Algo 3DES*/
			algo_descbcEncrypt(sgContext->umbKey + 1, sgContext->umbIntermediateResult, umboffset, sgContext->umbRemainingData + 0x01, 0x00, umbOutput);
		}
		else {
			/*if Algo AES*/
			AES_CMAC(sgContext->umbKey + 1, sgContext->umbRemainingData + 0x01, AES_BLOCK_SIZE, sgContext->umbKey[0x00], sgContext->umbIntermediateResult, umbOutput, 0x00);

		}

		if (sgContext->sgAlgoType == SIGN_DES_MAC)
		{
			/*store IV in sgContext->umbIntermediateResult*/
			for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
			{
				sgContext->umbIntermediateResult[umbIndex] = umbOutput[umbIndex];
			}
		}
	}

	/*if input data is present*/

	if (udbInputLen != NULL)
	{

		while (umboffset < umblength) {


			/*Perform Encryption as per bcContext*/

			if (sgContext->sgAlgoType == SIGN_DES_MAC) {

				/*if Algo 3DES*/
				algo_descbcEncrypt(sgContext->umbKey + 1, sgContext->umbIntermediateResult, umboffset, pumbInput + tempvar, 0x00, umbOutput);
			}
			else
			{
				/*if Algo AES*/
				
				AES_CMAC(sgContext->umbKey + 1, pumbInput + umboffset + tempvar, AES_BLOCK_SIZE, sgContext->umbKey[0x00], sgContext->umbIntermediateResult, umbOutput, 0x00);

			}

			//IV

			if (sgContext->sgAlgoType == SIGN_DES_MAC)
			{
				/*store IV in sgContext->umbIntermediateResult*/
				for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
				{
					sgContext->umbIntermediateResult[umbIndex] = umbOutput[umbIndex];
				}
			}

			umboffset += umbBlock;
		}

		/*if any remaining data is still present*/
		if ((udbInputLen%umbBlock) != NULL)
		{
			/*add last block data in the end of data to final encryption*/
			for (umbIndex = 0x00; umbIndex < (udbInputLen - umblength - tempvar); umbIndex++)
			{
				sgContext->umbRemainingData[umbIndex + 0x01] = pumbInput[umblength + umbIndex + tempvar];
			}
			sgContext->umbRemainingData[0x00] = umbIndex;
		}

	}
	DBG_FUNC_EXIT("<<[Algorithm]algo_sign_update() \n");
	return umblength;
}
/*
* It finalize the signature calculation on given last source data as per Signature context and reset sgContext
*
* @param sgContext:  stores all info. requried to Signature  operation
* @param pumbInput: Input data to which signature algo is performed
* @param udbInputLen: Input data length in pumbInput
* @param pumbOutput: result data after operation will be stored here
*
* @pre-requisite:	algo_sign_init() must be called prior to this function.
*					pumbInput must be udbInputLen long array
*					pumbOutput must be long enough to store signature of sgContext->umbSignatureLen
*/
uDiByte algo_sign_finalize(	OUT_ARG	ptSignatureObject	sgContext, \
							IN_ARG	puMonoByte			pumbInput, \
							IN_ARG	uDiByte				udbInputLen, \
							IN_ARG	puMonoByte			pumbOutput) {

	uMonoByte	umbIndex = 0x00;
	uMonoByte	umblength = 0X00;
	uMonoByte	umbBlock;
	uMonoByte	umbrlength;
	uMonoByte   tempvar = 0x00;
	uMonoByte   TEMP2;
	uMonoByte	umboffset = 0x00;
	uMonoByte	umbOffsetOut = 0x00;
	uMonoByte	umbLastBlock = 0x00;
	uMonoByte	umbNewlen;

	DBG_FUNC_ENTRY(">>[Algorithm]algo_sign_finalize() \n");
	umbNewlen = (uMonoByte)udbInputLen;

	if (sgContext->sgAlgoType == SIGN_DES_MAC)
	{
		umbBlock = DES_BLOCK_SIZE;
	}
	else if (sgContext->sgAlgoType == SIGN_AES_CMAC) {
		umbBlock = AES_BLOCK_SIZE;
	}
	else {
		return SIGN_NULL;
	}
	umbrlength = sgContext->umbRemainingData[0x00];
	//Case1 When input value and reamining data is null
	if (udbInputLen == NULL && umbrlength == NULL) {/*if input length is NULL and remaining Data is Null then*/
		/*Reset bcContext as we have concluded the operation by calling finalize function*/
		sgContext->umbKey[0x00] = NULL;
		return udbInputLen;
	}
	//Case2 When input value is null and reamining data is not null
	else if (udbInputLen == NULL && umbrlength != NULL) {/*if input length is zero and remaining Data is not Null then*/

		
		/*add padding in the end of data to final encryption*/\
		umbLastBlock = 0x01;
		if (sgContext->sgAlgoType == SIGN_DES_MAC) //add padding only if algo is DES
		{
			sgContext->umbRemainingData[0x00] = umbBlock;
			mem_set(sgContext->umbRemainingData + (umbrlength + 0x01), 0x00, (umbBlock - umbrlength));
		}
		
	}

	//case 3 and 4
	else if (udbInputLen != NULL) {

		if (umbrlength != NULL) {

			tempvar = umbBlock - umbrlength;
			if (tempvar > udbInputLen) {
				mem_cpy(sgContext->umbRemainingData + (umbrlength + 0x01), pumbInput, udbInputLen);
				TEMP2 = tempvar - udbInputLen;
				if (sgContext->sgAlgoType == SIGN_DES_MAC) //add padding only if algo is DES
				{
					mem_set(sgContext->umbRemainingData + (umbrlength + 0x01 + udbInputLen), 0x00, TEMP2);
				}
				else {//if algo is AES
					sgContext->umbRemainingData[0x00] = (umbrlength + udbInputLen);
				}
				
			}
			else {//(tempvar < udbInputLen)

				mem_cpy(sgContext->umbRemainingData + (umbrlength + 0x01), pumbInput, tempvar);
			}
			umbNewlen = (udbInputLen - tempvar);
			umblength = (umbNewlen - (umbNewlen%umbBlock));
		}
		else {
			umblength = (udbInputLen - (udbInputLen%umbBlock));
		}

	}

	//rem data

	if (sgContext->umbRemainingData[0x00] != NULL) {

		
			/*Perform Encryption as per bcContext*/
			if (sgContext->sgAlgoType == SIGN_DES_MAC) {
				/*if Algo 3DES*/
				algo_descbcEncrypt(sgContext->umbKey + 1, sgContext->umbIntermediateResult, umboffset, sgContext->umbRemainingData + 0x01, 0x00, pumbOutput);
			}
			else {
				/*if Algo AES*/
				AES_CMAC(sgContext->umbKey + 1, sgContext->umbRemainingData + 0x01, sgContext->umbRemainingData[0x00], sgContext->umbKey[0x00], sgContext->umbIntermediateResult, pumbOutput, umbLastBlock);
							
			}
		
		if (sgContext->sgAlgoType == SIGN_DES_MAC)
		{
			for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
			{
				sgContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex];
			}
		}
	}

	//input data

	if (udbInputLen != NULL) {

		while (umboffset < umblength) {
			
				/*Perform Encryption as per bcContext*/
				if (sgContext->sgAlgoType == SIGN_DES_MAC) {
					
					/*if Algo 3DES*/
					algo_descbcEncrypt(sgContext->umbKey + 1, sgContext->umbIntermediateResult, umboffset, pumbInput + tempvar, 0x00, pumbOutput);
				}
				else 
				{
					/*if Algo AES*/
					if (((umbNewlen%umbBlock) == NULL)
						&&
						(umboffset == (umblength - umbBlock))) {//if it is last block

						umbLastBlock = 0x01;
					}
					AES_CMAC(sgContext->umbKey + 1, pumbInput + umboffset + tempvar, AES_BLOCK_SIZE, sgContext->umbKey[0x00], sgContext->umbIntermediateResult, pumbOutput, umbLastBlock);
				}

			//IV

			if (sgContext->sgAlgoType == SIGN_DES_MAC)
			{
				for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
				{
					sgContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex];
				}
			}
		
			umboffset += umbBlock;
		}

		/*add last block data in the end of data to final encryption*/
		if ((udbInputLen%umbBlock) != NULL)
		{
			sgContext->umbRemainingData[0x00] = umbBlock;
			umbLastBlock = 0x01;
			for (umbIndex = 0x00; umbIndex <(udbInputLen - umblength - tempvar); umbIndex++)
			{
				sgContext->umbRemainingData[umbIndex + 0x01] = pumbInput[umblength + umbIndex + tempvar];
			}
			if (sgContext->sgAlgoType == SIGN_DES_MAC) {//add padding only if algo is DES
				/*add padding*/
				mem_set((sgContext->umbRemainingData + umbIndex + 0x01), 0x00, (umbBlock - umbIndex));
				
			}
			
		}
		if (sgContext->umbRemainingData[0x00] != NULL) {

				/*Perform Encryption as per bcContext*/
				if (sgContext->sgAlgoType == SIGN_DES_MAC) {
					
					/*if Algo 3DES*/
					algo_descbcEncrypt(sgContext->umbKey + 1, sgContext->umbIntermediateResult, 0x00, sgContext->umbRemainingData + 0x01, 0x00, pumbOutput);
				}
				else {
					
					/*if Algo AES*/
					AES_CMAC(sgContext->umbKey + 1, sgContext->umbRemainingData + 0x01, (udbInputLen%umbBlock), sgContext->umbKey[0x00], sgContext->umbIntermediateResult, pumbOutput, 0X01);
				}
			}
	}

	/*Reset sgContext as we have concluded the operation by calling finalize function*/
	sgContext->umbKey[0x00] = NULL;
	DBG_FUNC_EXIT("<<[Algorithm]algo_sign_finalize() \n");
	return udbInputLen;
	
}