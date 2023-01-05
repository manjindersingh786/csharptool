#include "Algorithms.h"
#define	IV							(0x00)
#define OFFSET_PD					(0x00)
#define	INPUT_ZERO					(0x00)
#define EXECUTED					(0x01)
/*
* It initializes the block cipher context as per given input
* 
* @param bcContext:  stores all info. requried to block cipher operation
* @param bcAlgoType: Type of Algo to use
* @param cOper: Encrypt or Decrypt
* @param pumbKey: 1st element stores key length and following data is key value\
* 
* @pre-requisite: bcContext must not hold partial data of other cipher operation.
*/
uMonoByte algo_bc_init(	OUT_ARG	ptBlockCipherObject bcContext, \
					IN_ARG  tBlockCipherAlgo	bcAlgoType, \
					IN_ARG  tCipherOperation	cOper, \
					IN_ARG  puMonoByte			pumbKey) {


	DBG_FUNC_ENTRY(">>[Algorithm]algo_bc_init() \n");
	
	if (bcAlgoType != BC_DES && bcAlgoType != BC_AES) {
		/*Warning Algo Not Suported*/
		DBG_ERR("[Algorithm]algo_bc_init() Algo Not Supported\n");
		return BC_NULL;
	}

	/*Populate given information into bcContext*/
	DBG_FUNC("[Algorithm]algo_bc_init() Populate given information into bcContext\n");
	bcContext->bcAlgoType= bcAlgoType;
	bcContext->cOp=cOper;
	/*key length + key store*/
	mem_cpy(bcContext->umbKey, pumbKey, pumbKey[0x00]+0x01);
	/*for DES*/
	if (pumbKey[0x00] == 0x10 && bcAlgoType == BC_DES) {
		DBG_FUNC("[Algorithm]algo_bc_init() store keys for 3DES with 2 different keys\n");
		bcContext->umbKey[0x00] = 0x18;
		/*key store for 3DES with 2 Different keys*/
		mem_cpy(bcContext->umbKey+ BLOCK_SIZE +0x01, pumbKey+1, DES_BLOCK_SIZE);
	}	

	/*Reset Intermediate data to zero as per given algo type*/
	mem_set(bcContext->umbIntermediateResult, IV, BLOCK_SIZE);
	bcContext->umbRemainingData[0x00] = INPUT_ZERO;
	DBG_FUNC_EXIT("<<[Algorithm]algo_bc_init() Reset Intermediate data to zero as per given algo type \n");
	return EXECUTED;
}

/*
* It performs the crypto operation on given source data as per block cipher context
*
* @param bcContext:  stores all info. requried to block cipher operation
* @param pumbInput: Input data to which crypto operation is performed
* @param udbInputLen: Input data length in pumbInput 
* @param pumbOutput: result data after operation will be stored here
*
* @pre-requisite:	algo_bc_init() must be called prior to this function.
*					pumbInput and pumbOutput must be udbInputLen long array
*/
uDiByte algo_bc_update(INOUT_ARG	ptBlockCipherObject bcContext, \
					   IN_ARG		puMonoByte			pumbInput, \
					   IN_ARG		uDiByte				udbInputLen, \
					   OUT_ARG		puMonoByte			pumbOutput) {
	uMonoByte		umbIndex;
	uMonoByte		umboffset= 0x00;
	uMonoByte		umbrd;
	uMonoByte		umbBlock;
	uMonoByte		umbblocklength=0x00;
	uMonoByte		umbtempvar = 0x00;
	uMonoByte		umbOffsetOut = 0x00;
	
	if (bcContext->bcAlgoType == BC_DES) {
		umbBlock = DES_BLOCK_SIZE;
	}
	else if (bcContext->bcAlgoType == BC_AES) {
		umbBlock = AES_BLOCK_SIZE;
	}
	else {
		return BC_NULL;
	}
	DBG_FUNC_ENTRY("<<[Algorithm]algo_bc_update()\n");

	if (bcContext->umbRemainingData[0x00] != NULL) {/*if Remaining Data is Not Null*/
		DBG_FUNC("[Algorithm] algo_bc_update()If Remaining Data is Not Null\n");
		umbrd = bcContext->umbRemainingData[0x00];
		umbtempvar = (umbBlock - umbrd);
		if (umbtempvar > udbInputLen) {
			mem_cpy(bcContext->umbRemainingData + (umbrd + 0x01), pumbInput, udbInputLen);
			bcContext->umbRemainingData[0x00] = (umbrd + udbInputLen);
			return 0;
		}
		//(umbtempvar <= udbInputLen)				
			mem_cpy(bcContext->umbRemainingData + (umbrd + 0x01), pumbInput, umbtempvar);
			bcContext->umbRemainingData[0x00] = (umbrd + umbtempvar);
		
		if ((udbInputLen - umbtempvar) > umbBlock) {/*if Remaining Input Length is not blockwise*/
			umbblocklength = (udbInputLen - umbtempvar);
			umbrd = (umbblocklength%umbBlock);
			umbblocklength = umbblocklength - umbrd;
		}
		else {/*if Remaining Input Length is blockwise*/
			umbrd = (udbInputLen - umbtempvar);
		}
	}
	else {/*if Remaining Data is Null*/
		DBG_FUNC("[Algorithm] algo_bc_update()If Remaining Data is Null\n");
		umbrd = (udbInputLen%umbBlock);
		umbblocklength = (udbInputLen - umbrd);
	}
	if (bcContext->umbRemainingData[0x00] != NULL) {/*if Remainining Data is not NULL and perform encryption or decryption*/
		DBG_FUNC("[Algorithm] algo_bc_update()Calculate for Remaining Data is not Null\n");
		if (bcContext->cOp == OP_ENCRYPT) {
			/*Perform Encryption as per bcContext*/
			if (bcContext->bcAlgoType == BC_DES) {
				/*if Algo 3DES*/
				algo_descbcEncrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, bcContext->umbRemainingData + 0x01, umboffset, pumbOutput);
			}
			else {
				/*if Algo AES*/
				algo_aescbcencrypt(bcContext->umbIntermediateResult, bcContext->umbRemainingData + 0x01, AES_BLOCK_SIZE, bcContext->umbKey + 0x01, bcContext->umbKey[0x00]);
				/*store output result for AES*/
				mem_cpy(pumbOutput, bcContext->umbRemainingData+0x01, umbBlock);
			}
		}
		else {
			/*Perform decryption as per bcContext*/
			if (bcContext->bcAlgoType == BC_DES) {
				/*if Algo 3DES*/

				algo_descbcDecrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, bcContext->umbRemainingData + 0x01, umboffset, pumbOutput);
			}
			else {
				/*if Algo AES*/

				algo_aescbcdecrypt(bcContext->umbIntermediateResult, bcContext->umbRemainingData + 0x01, AES_BLOCK_SIZE, bcContext->umbKey + 1, bcContext->umbKey[0x00]);
				/*store output result for AES*/
				mem_cpy(pumbOutput, bcContext->umbRemainingData + 0x01, umbBlock);
			}
		}
		if (bcContext->bcAlgoType == BC_DES)
		{
			for (umbIndex = 0x00;umbIndex < umbBlock; umbIndex++)
			{
				if (OP_ENCRYPT == bcContext->cOp) {

					bcContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex];
				}
				else {//OP_DECRYPT
					bcContext->umbIntermediateResult[umbIndex] = bcContext->umbRemainingData[umbIndex + 0x01];
				}
			}
			umbOffsetOut = DES_BLOCK_SIZE;
		}
		else {
			if (bcContext->bcAlgoType == BC_AES) {
				for (umbIndex = 0x00;umbIndex < umbBlock;umbIndex++) {
					if (OP_ENCRYPT == bcContext->cOp) {

						bcContext->umbIntermediateResult[umbIndex] = bcContext->umbRemainingData[umbIndex + 0x01];
					}
				}
			}
			umbOffsetOut = AES_BLOCK_SIZE;
		}
	}
	while (umboffset < umbblocklength) {
		/*Perform Encryption or decryption as per bcContext*/
		if (OP_ENCRYPT == bcContext->cOp) {/*for Encryption*/
			if (bcContext->bcAlgoType == BC_DES) {/*if algo type DES*/
				DBG_FUNC("[Algorithm]algo_bc_update() Perform Encryption for 3DES in cbc mode\n");
				/*call Encrypt function in cbc mode*/
				algo_descbcEncrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, pumbInput+ umbtempvar, umboffset+umbOffsetOut, pumbOutput);
			}
			else {/*if algo type AES*/
				DBG_FUNC("[Algorithm]algo_bc_update() Perform Encryption for AES in cbc mode\n");
				algo_aescbcencrypt(bcContext->umbIntermediateResult, pumbInput + umboffset+ umbtempvar, AES_BLOCK_SIZE, bcContext->umbKey + 1, bcContext->umbKey[0x00]);
			}
		}
		else
		{/*for Decryption*/
			if (bcContext->bcAlgoType == BC_DES) {
				DBG_FUNC("[Algorithm]algo_bc_update() Perform Decryption for 3DES in cbc mode\n");
				/*call decrypt function in cbc mode*/
				algo_descbcDecrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, pumbInput+ umbtempvar, umbOffsetOut+ umboffset, pumbOutput);

			}
			else {/*if algo type AES*/
				DBG_FUNC("[Algorithm]algo_bc_update() Perform Decryption for AES in cbc mode\n");
				algo_aescbcdecrypt(bcContext->umbIntermediateResult, pumbInput+ umboffset+ umbtempvar, AES_BLOCK_SIZE, bcContext->umbKey + 1, bcContext->umbKey[0x00]);
			}
		}
		/*if Algo Type AES*/
		if (bcContext->bcAlgoType == BC_AES) {
			/*store output result for AES*/
			for (umbIndex = 0x00;umbIndex < umbBlock;umbIndex++) {
				pumbOutput[umbIndex + umboffset+ umbOffsetOut] = pumbInput[umbIndex + umboffset+ umbtempvar];
			}
		}

		DBG_FUNC("[Algorithm]algo_bc_update() store intermediate result data in bcContext->umbIntermediateResult\n");
		/*store intermediate result data in bcContext->umbIntermediateResult */

		for (umbIndex = 0x00;umbIndex < umbBlock;umbIndex++) {
			if (bcContext->bcAlgoType == BC_DES) {
				if (OP_ENCRYPT == bcContext->cOp){
				
					bcContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex + umboffset+ umbOffsetOut];
				}
				else {
					bcContext->umbIntermediateResult[umbIndex] = pumbInput[umbIndex + umboffset+ umbtempvar];
				}				
			}
			else if (bcContext->bcAlgoType == BC_AES) {
				
					if (OP_ENCRYPT == bcContext->cOp) {
						bcContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex + umboffset + umbOffsetOut];
					}
				}			
		}
		/*offset update*/
		umboffset += umbBlock;
	}	
	if (umboffset < udbInputLen) {
		//offsetrd = (udbInputLen%udBlock);
		DBG_FUNC("[Algorithm]algo_bc_update() *store remaining data of input data which is not block aligned\n");
		/*store remaining data length and data of input data which is not block aligned in bcContext->umbRemainingData */
		bcContext->umbRemainingData[0x00] = umbrd;
		for (umbIndex = 0x00;umbIndex < umbrd;umbIndex++) {
			bcContext->umbRemainingData[umbIndex+0x01] = pumbInput[umbIndex + umboffset];
		}
	}		
	else {
		bcContext->umbRemainingData[0x00] = NULL;
	}
	DBG_FUNC_EXIT("<<[Algorithm]algo_bc_update() Return Perform encryption or decryption length\n", umbblocklength);
	return umbblocklength;
}

/*
* It finalize the crypto operation on given source data as per block cipher context and reset bcContext
*
* @param bcContext:  stores all info. requried to block cipher operation
* @param pumbInput: Input data to which crypto operation is performed
* @param udbInputLen: Input data length in pumbInput
* @param pumbOutput: result data after operation will be stored here
*
* @pre-requisite:	algo_bc_init() must be called prior to this function.
*					pumbInput and pumbOutput must be udbInputLen long array
*					combined given input data and remaining data in bcContext must be block aligned in case of decryption
*/
uDiByte algo_bc_finalize(INOUT_ARG	ptBlockCipherObject bcContext, \
						 IN_ARG		puMonoByte			pumbInput, \
						 IN_ARG		uDiByte				udbInputLen, \
						 OUT_ARG	puMonoByte			pumbOutput) {

	uMonoByte	umbIndex = 0x00;
	uMonoByte	umblength=0x00;
	uMonoByte	umbBlock;
	uMonoByte	umbrlength;	
	uMonoByte   umbtempvar = 0x00;
	uMonoByte   TEMP2;
	uMonoByte	umboffset = 0x00;
	uMonoByte	umbOffsetOut = 0x00;

	if (bcContext->bcAlgoType == BC_DES)
	{
		umbBlock = DES_BLOCK_SIZE;
	}
	else if (bcContext->bcAlgoType == BC_AES) {
		umbBlock = AES_BLOCK_SIZE;
	}
	else {
		return BC_NULL;
	}
	DBG_FUNC_ENTRY(">>[Algorithm]algo_bc_finalize()\n");
	umbrlength = bcContext->umbRemainingData[0x00];
	//Case1 When input value and reamining data is null
	if (udbInputLen == NULL && umbrlength ==NULL) {/*if input length is NULL and remaining Data is Null then*/
		DBG_FUNC("[Algorithm] algo_bc_finalize()if input length is NULL and remaining Data is Null then\n");
		/*Reset bcContext as we have concluded the operation by calling finalize function*/
		bcContext->umbKey[0x00] = NULL;
		return 0;
	}
	//Case2 When input value is null and reamining data is not null
	else if (udbInputLen == NULL && umbrlength != NULL) {/*if input length is zero and remaining Data is not Null then*/
		DBG_FUNC("[Algorithm] algo_bc_finalize()if input length is zero and remaining Data is not Null then\n");
		bcContext->umbRemainingData[0x00] = umbBlock;
		/*add padding in the end of data to final encryption*/		
		mem_set(bcContext->umbRemainingData + ((umbBlock - umbrlength) + 0x01), OFFSET_PD, (umbBlock - umbrlength));
	}
	
	//case 3 and 4
	else if (udbInputLen != NULL) {
		if (umbrlength != NULL) {
			DBG_FUNC("[Algorithm] algo_bc_finalize()if input length is not zero and remaining Data is not Null then\n");
			umbtempvar = umbBlock - umbrlength;
			if (umbtempvar > udbInputLen) {
				mem_cpy(bcContext->umbRemainingData + (umbrlength + 0x01), pumbInput, udbInputLen);
				TEMP2 = umbtempvar - udbInputLen;
				mem_set(bcContext->umbRemainingData + (umbrlength + 0x01 + udbInputLen), 0x00, TEMP2);
			}
			else {//(tempvar < udbInputLen)				
				mem_cpy(bcContext->umbRemainingData + (umbrlength + 0x01), pumbInput, umbtempvar);
			}
			umblength = (udbInputLen - umbtempvar);
			umblength = (umblength - (umblength%umbBlock));
		}
		else {
			DBG_FUNC("[Algorithm] algo_bc_finalize()if input length is not zero and remaining Data is Null then\n");
			umblength = (udbInputLen - (udbInputLen%umbBlock));
		}
	}	
	//rem data

	if (bcContext->umbRemainingData[0x00] != NULL) {
		DBG_FUNC("[Algorithm] algo_bc_finalize()Calculate Remaining Data\n");
		if (bcContext->cOp == OP_ENCRYPT) {
			/*Perform Encryption as per bcContext*/
			if (bcContext->bcAlgoType == BC_DES) {
				/*if Algo 3DES*/
				algo_descbcEncrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, bcContext->umbRemainingData + 0x01, umboffset, pumbOutput);
			}
			else {
				/*if Algo AES*/

				algo_aescbcencrypt(bcContext->umbIntermediateResult, bcContext->umbRemainingData + 0x01, AES_BLOCK_SIZE, bcContext->umbKey + 0x01, bcContext->umbKey[0x00]);
				/*store output result for AES*/
				for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
				{
					pumbOutput[umbIndex] = bcContext->umbRemainingData[umbIndex + 0x01];
				}
			}
		}
		else {
			/*Perform decryption as per bcContext*/
			if (bcContext->bcAlgoType == BC_DES) {
				/*if Algo 3DES*/

				algo_descbcDecrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, bcContext->umbRemainingData + 0x01, umboffset, pumbOutput);
			}
			else {
				/*if Algo AES*/

				algo_aescbcdecrypt(bcContext->umbIntermediateResult, bcContext->umbRemainingData + 0x01, AES_BLOCK_SIZE, bcContext->umbKey + 1, bcContext->umbKey[0x00]);
				/*store output result for AES*/
				for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
				{
					pumbOutput[umbIndex] = bcContext->umbRemainingData[umbIndex + 0x01];
				}
			}
		}
		if (bcContext->bcAlgoType == BC_DES)
		{
			for (umbIndex = 0x00;umbIndex < umbBlock; umbIndex++)
			{
				if (OP_ENCRYPT == bcContext->cOp) {

					bcContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex];
				}
				else {//OP_DECRYPT
					bcContext->umbIntermediateResult[umbIndex] = bcContext->umbRemainingData[umbIndex+0x01];
				}
			}
			umbOffsetOut = DES_BLOCK_SIZE;
		}
		else {
			if (bcContext->bcAlgoType == BC_AES) {
				for (umbIndex = 0x00;umbIndex < umbBlock;umbIndex++) {
					if (OP_ENCRYPT == bcContext->cOp) {

						bcContext->umbIntermediateResult[umbIndex] = bcContext->umbRemainingData[umbIndex + 0x01];
					}

				}
			}
			umbOffsetOut = AES_BLOCK_SIZE;
		}
	}	
	//input data
	
	if (udbInputLen != NULL) {
		DBG_FUNC("[Algorithm] algo_bc_finalize()Calculate Input Data for case3 and case4\n");
		while (umboffset < umblength) {

			if (bcContext->cOp == OP_ENCRYPT) {
				/*Perform Encryption as per bcContext*/
				if (bcContext->bcAlgoType == BC_DES) {
					/*if Algo 3DES*/

					algo_descbcEncrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, pumbInput + umbtempvar, umboffset + umbOffsetOut, pumbOutput);
				}
				else {

					/*if Algo AES*/

					algo_aescbcencrypt(bcContext->umbIntermediateResult, pumbInput + umboffset + umbtempvar, AES_BLOCK_SIZE, bcContext->umbKey + 1, bcContext->umbKey[0x00]);
					/*store output result for AES*/
					for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
					{
						pumbOutput[umbIndex + umbOffsetOut + umboffset] = pumbInput[umbIndex + umboffset + umbtempvar];
					}
				}
			}
			else {
				/*Perform Decryption as per bcContext*/
				if (bcContext->bcAlgoType == BC_DES) {
					/*if Algo 3DES*/

					algo_descbcDecrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, umboffset, pumbInput + umbtempvar, umboffset+ umbOffsetOut, pumbOutput);
				}
				else {
					/*if Algo AES*/

					algo_aescbcdecrypt(bcContext->umbIntermediateResult, pumbInput + umboffset + umbtempvar, AES_BLOCK_SIZE, bcContext->umbKey + 1, bcContext->umbKey[0x00]);
					/*store output result for AES*/
					for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
					{
						pumbOutput[umbIndex + umbOffsetOut + umboffset] = pumbInput[umbIndex + umboffset + umbtempvar];
					}
				}
			}
			//IV

			if (bcContext->bcAlgoType == BC_DES)
			{
				for (umbIndex = 0x00;umbIndex < umbBlock; umbIndex++)
				{
					if (OP_ENCRYPT == bcContext->cOp) {

						bcContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex + umbOffsetOut];
					}
					else {//OP_DECRYPT
						bcContext->umbIntermediateResult[umbIndex] = pumbInput[umbIndex + umbtempvar + umboffset];
					}
				}
			}
			else if (bcContext->bcAlgoType == BC_AES) {

				if (OP_ENCRYPT == bcContext->cOp) {

					bcContext->umbIntermediateResult[umbIndex] = pumbOutput[umbIndex + umboffset];
				}
			}

			umboffset += umbBlock;
			umbOffsetOut += umbBlock;
		}

		/*add last block data in the end of data to final encryption*/
		if ((udbInputLen%umbBlock) != NULL)
		{
			bcContext->umbRemainingData[0x00] = umbBlock;
			for (umbIndex = 0x00; umbIndex <(udbInputLen - umblength - umbtempvar); umbIndex++)
			{
				bcContext->umbRemainingData[umbIndex + 0x01] = pumbInput[umblength + umbIndex + umbtempvar];
			}
			for (umbIndex=0x00; umbIndex < umbBlock; umbIndex++) {
				/*add padding*/
				bcContext->umbRemainingData[umbIndex + 0x01] = OFFSET_PD;
			}
		}
		if (bcContext->umbRemainingData[0x00] != NULL) {
			DBG_FUNC("[Algorithm] algo_bc_finalize()Calculate Remaining Data for case4\n");
			if (bcContext->cOp == OP_ENCRYPT) {
				/*Perform Encryption as per bcContext*/
				if (bcContext->bcAlgoType == BC_DES) {
					/*if Algo 3DES*/
					algo_descbcEncrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, 0x00, bcContext->umbRemainingData + 0x01, umbOffsetOut, pumbOutput );
				}
				else {
					
					/*if Algo AES*/

					algo_aescbcencrypt(bcContext->umbIntermediateResult, bcContext->umbRemainingData + 0x01, AES_BLOCK_SIZE, bcContext->umbKey + 0x01, bcContext->umbKey[0x00]);
					/*store output result for AES*/
					for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
					{
						pumbOutput[umbIndex + umbOffsetOut + umboffset] = bcContext->umbRemainingData[umbIndex + 0x01];
					}
				}
			}
			else {
				/*Perform decryption as per bcContext*/
				if (bcContext->bcAlgoType == BC_DES) {
					/*if Algo 3DES*/
					algo_descbcDecrypt(bcContext->umbKey + 1, bcContext->umbIntermediateResult, 0x00, bcContext->umbRemainingData + 0x01, umbOffsetOut, pumbOutput);
				}
				else {
					/*if Algo AES*/

					algo_aescbcdecrypt(bcContext->umbIntermediateResult, bcContext->umbRemainingData + 0x01, AES_BLOCK_SIZE, bcContext->umbKey + 1, bcContext->umbKey[0x00]);
					/*store output result for AES*/
					for (umbIndex = 0x00; umbIndex < umbBlock; umbIndex++)
					{
						pumbOutput[umbIndex + umbOffsetOut + umboffset] = bcContext->umbRemainingData[umbIndex + 0x01];
					}
				}
			}
		}
	}
	DBG_FUNC_EXIT("<<[Algorithm]algo_bc_finalize()\n");
	/*Reset bcContext as we have concluded the operation by calling finalize function*/
	bcContext->umbKey[0x00] = NULL;

	return udbInputLen;
}