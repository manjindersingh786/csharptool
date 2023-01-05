
#ifndef _ALGO_SIGNATURE_EXTERN_H 
#define _ALGO_SIGNATURE_EXTERN_H
#include "types.h"
#include "Oasis.h"
#include "Algorithms_extern.h"

/*
* Typedefs
*/

typedef enum {
	SIGN_NULL,
	SIGN_AES_CMAC,
	SIGN_DES_MAC
}tSignAlgo;

typedef struct {

	tSignAlgo	sgAlgoType;
	uMonoByte	umbSignatureLen;
	uMonoByte	umbIntermediateResult[MAX_BLOCK_SIZE_FOR_ANY_CIPHER_ALGO]; // Stores intermediate result for given operation
	// It stores remaining data of input buffer which is not block aligned, it will be clear in doFinal() only. First byte stores length of remaining data and following remaining data
	uMonoByte	umbRemainingData[MAX_BLOCK_SIZE_FOR_ANY_CIPHER_ALGO];
	//Stores key length and value as well
	uMonoByte	umbKey[LENGTH_KEY_OBJECT];

}tSignatureObject;

typedef tSignatureObject* ptSignatureObject;

/*
* Functions declaration
*/

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
extern uDiByte algo_sign_init(OUT_ARG	ptSignatureObject sgContext, \
	IN_ARG tSignAlgo		bcAlgoType, \
	IN_ARG uMonoByte		umbSignLength, \
	IN_ARG puMonoByte			pumbKey);


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
extern uDiByte algo_sign_update(OUT_ARG	ptSignatureObject sgContext, \
	IN_ARG puMonoByte			pumbInput, \
	IN_ARG uDiByte				udbInputLen);


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
extern uDiByte algo_sign_finalize(OUT_ARG	ptSignatureObject sgContext, \
	IN_ARG puMonoByte			pumbInput, \
	IN_ARG uDiByte				udbInputLen, \
	IN_ARG puMonoByte			pumbOutput);


#endif // !_ALGO_SIGNATURE_EXTERN_H
