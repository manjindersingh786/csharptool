
#ifndef _ALGO_BLOCKCIPHER_EXTERN_H
#define _ALGO_BLOCKCIPHER_EXTERN_H

#include "types.h"
#include "Oasis.h"
#include "Algorithms_extern.h"
/*
* Macros
*/

/*
* Typedefs
*/
typedef enum {
	BC_NULL,
	BC_DES,
	BC_AES
}tBlockCipherAlgo;

/*Cipher operations*/
typedef enum {
	OP_ENCRYPT,
	OP_DECRYPT
}tCipherOperation;


typedef struct {

	tBlockCipherAlgo	bcAlgoType;
	tCipherOperation	cOp;
	uMonoByte			umbIntermediateResult[MAX_BLOCK_SIZE_FOR_ANY_CIPHER_ALGO]; // Stores intermediate result for given operation
	// It stores remaining data of input buffer which is not block aligned, it will be clear in doFinal() only. First byte stores length of remaining data and following remaining data
	uMonoByte			umbRemainingData[MAX_BLOCK_SIZE_FOR_ANY_CIPHER_ALGO];
	//Stores key length and value as well
	uMonoByte			umbKey[LENGTH_KEY_OBJECT];

}tBlockCipherObject;

typedef tBlockCipherObject* ptBlockCipherObject;

/*
* Functions declaration
*/
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
extern uMonoByte algo_bc_init(	OUT_ARG	ptBlockCipherObject bcContext, \
							IN_ARG	tBlockCipherAlgo	bcAlgoType, \
							IN_ARG	tCipherOperation	cOper, \
							IN_ARG	puMonoByte			pumbKey);
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
extern uDiByte algo_bc_update(INOUT_ARG	ptBlockCipherObject bcContext, \
							IN_ARG		puMonoByte			pumbInput, \
							IN_ARG		uDiByte				udbInputLen, \
							OUT_ARG		puMonoByte			pumbOutput);
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
extern uDiByte algo_bc_finalize(INOUT_ARG	ptBlockCipherObject bcContext, \
								IN_ARG		puMonoByte			pumbInput, \
								IN_ARG		uDiByte				udbInputLen, \
								OUT_ARG		puMonoByte			pumbOutput);

#endif // !_ALGO_BLOCKCIPHER_EXTERN_H
