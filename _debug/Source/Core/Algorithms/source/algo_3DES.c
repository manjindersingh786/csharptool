#include "Algorithms.h"

/*algo_descbcEncrypt()-for 3DES cbc mode Encryption
**@umbKey					key address
**@umbIntermediateResult	for DES IV
**@offset					offset for operation index
**@ALGO_IN-					It's pass plain text for perform cbc des encryption algo
**@ALGO_OUT-				It's return encrypted text
*/
void algo_descbcEncrypt(puMonoByte umbKey,\
						puMonoByte umbIntermediateResult,\
						uMonoByte  umbInOffset,\
						puMonoByte ALGO_IN,\
						uMonoByte  umbOutOffset, \
						puMonoByte ALGO_OUT) {

	uMonoByte	umbIndex;
	uDiByte		udbBlock = 0x08;
	uMonoByte	buffer[0x08];

	
	DBG_FUNC_ENTRY(">>[Algorithm]algo_descbcEncrypt() \n");
	/*for Offset Block*/
	for (umbIndex = 0x00;umbIndex<udbBlock; umbIndex++)
	{
		//XOR input block with IV contents
		buffer[umbIndex] = ALGO_IN[umbIndex + umbInOffset] ^ umbIntermediateResult[umbIndex];
	}
	DBG_FUNC("[Algorithm]algo_descbcEncrypt() Call Encrypt Function for 3DES\n");
	/*Call Encrypt Function*/
	algo_3des(umbKey, buffer, udbBlock, buffer, OP_ENCRYPT);

	for (umbIndex = 0x00;umbIndex<udbBlock; umbIndex++)
	{
		//store final Encrypted data in ALGO_OUT
		ALGO_OUT[umbOutOffset + umbIndex] = buffer[umbIndex];
	}
	DBG_FUNC_EXIT("<<[Algorithm]algo_descbcEncrypt() \n");
}
/*algo_descbcDecrypt()-for 3DES cbc mode Decryption
**@umbKey					key address
**@umbIntermediateResult	for DES IV
**@offset					offset for operation index
**@ALGO_IN-					It's pass  ciphertext for perform cbc des decryption
**@ALGO_OUT-				It's return decrypted text
*/
void algo_descbcDecrypt(puMonoByte	umbKey,\
						puMonoByte	umbIntermediateResult,\
						uMonoByte  umbInOffset, \
						puMonoByte ALGO_IN, \
						uMonoByte  umbOutOffset, \
						puMonoByte ALGO_OUT) {

	uMonoByte	umbIndex;
	uDiByte		udbBlock = 0x08;
	uMonoByte	buffer[0x08];

	DBG_FUNC_ENTRY(">>[Algorithm]algo_descbcDecrypt() \n");

	for (umbIndex = 0x00;umbIndex<udbBlock; umbIndex++)
	{
		//store data in buffer block wise
		buffer[umbIndex] = ALGO_IN[umbIndex + umbInOffset];
	}
	DBG_FUNC("[Algorithm]algo_descbcEncrypt() Call 3des Decrypt Function\n");
	/*Call 3des Decrypt Function*/
	algo_3des(umbKey, buffer, udbBlock, buffer, OP_DECRYPT);

	for (umbIndex = 0x00;umbIndex<udbBlock;umbIndex++)
	{
		//XOR input block with IV contents
		buffer[umbIndex] = buffer[umbIndex] ^ umbIntermediateResult[umbIndex];
		//store final decrpted data in cbc mode
		ALGO_OUT[umbOutOffset + umbIndex] = buffer[umbIndex];
	}

	DBG_FUNC_EXIT("<<[Algorithm]algo_descbcDecrypt() \n");
}