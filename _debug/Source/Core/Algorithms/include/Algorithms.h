#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H
#include "types.h"
#include "Oasis.h"
#include "Algorithms_extern.h"
#include "algo_blockCipher_extern.h"
#include "algo_signature_extern.h"
#include "algo_checksum_extern.h"
#include "MemoryManager_extern.h"

#define KEY_LEN						(0x08)
#define DES_BLOCK_SIZE				(0x08)
#define AES_BLOCK_SIZE				(0x10)
#define BLOCK_SIZE					0x10
#define LAST_INDEX					(BLOCK_SIZE - 1)

#define LB32_MASK					0x00000001
#define LB64_MASK					0x0000000000000001
#define L64_MASK					0x00000000ffffffff
#define H64_MASK					0xffffffff00000000
#define AES128						1
#define AES192						1
#define AES256						1
/**
* @brief Cipher operation modes
**/
//#define AES_keyExpSize (AES_check_fix+aes_check)

/* Common interface for encryption algorithms
*/
/*for DES key process*/
typedef uMonoByte key_t[KEY_LEN];
/*for AES Round key process*/
/*****************************************************************************/
/* Private variables:                                                        */
/*****************************************************************************/
// state - array holding the intermediate results during decryption.
typedef uMonoByte state_t[4][4];

/*AES encryption*/
extern void algo_aes_encrypt(	INOUT_ARG state_t* state,	IN_ARG uMonoByte Nr);

/*Declaration of function which calculate Round Keys*/
extern void KeyExpansion(uMonoByte Nk,uMonoByte Nr, const puMonoByte Key);

/*-------------------------------------------------------------------
*						Algorithm 3DES or DES
*-------------------------------------------------------------------*/


/*algo_3des()-for 3DES		Encryption or Decryption
**@key						key address
**@umbIntermediateResult	for DES IV
**@len						offset for operation index
**@message-					It's pass plain text for perform cbc des encryption algo
**@buffer-					It's return encrypted or decrypted text
**@kmode-					insert operation mode
*/

extern void algo_3des(  puMonoByte key, \
						puMonoByte message, \
						uDiByte len, \
						puMonoByte buffer, \
						tCipherOperation kmode);

/*algo_descbcEncrypt()-for 3DES cbc mode Encryption
**@umbKey					key address
**@umbIntermediateResult	for DES IV
**@umbInOffset				input offset for operation index
**@ALGO_IN-					It's pass plain text for perform cbc des encryption algo
**@ALGO_OUT-				It's return encrypted text
**@umbOutOffset				Output offset for operation index
*/
extern void algo_descbcEncrypt(puMonoByte umbKey, \
								puMonoByte umbIntermediateResult, \
								uMonoByte  umbInOffset, \
								puMonoByte ALGO_IN, \
								uMonoByte  umbOutOffset, \
								puMonoByte ALGO_OUT);

/*algo_descbcDecrypt()-for 3DES cbc mode Decryption
**@umbKey					key address
**@umbIntermediateResult	for DES IV
**@umbInOffset				input offset for operation index
**@ALGO_IN-					It's pass  ciphertext for perform cbc des decryption
**@ALGO_OUT-				It's return decrypted text
**@umbOutOffset				Output offset for operation index
*/
extern void algo_descbcDecrypt(puMonoByte umbKey, \
								puMonoByte umbIntermediateResult, \
								uMonoByte  umbInOffset, \
								puMonoByte ALGO_IN, \
								uMonoByte  umbOutOffset, \
								puMonoByte ALGO_OUT);
/*-------------------------------------------------------------------
*algo_crc()- Takes Some Input and Length sizeof(Input), and creates 2-byte(16-bit) or 4-byte(32-bit) CHECKSUM
*@CryptoObject- It's store all
*@pumbalgo-	    It's algo type
*@udLength-		It's data Length
*@pumbCheckSum- IT's return 2-bytes or 4-byte CheckSum
*-------------------------------------------------------------------*/
extern uQuadByte algo_crc(INOUT_ARG uQuadByte uqbIntermediateResult,\
	                        IN_ARG tChecksumAlgo algo,\
							IN_ARG puMonoByte pumbInput,\
							IN_ARG uDiByte udLength);

/*-------------------------------------------------------------------
* Algorithm cbc AES(Encryption)*
*-------------------------------------------------------------------
*@Iv			- It's containes IV
*@pumbInput		- It's containes Input plane text or Output cipher test
*@length		- It's containes length of the blocksize data
*@key			- It's containes key of the algo
*@keylength		- It's containes key length
*-----------------------------------------------------------------*/
extern void algo_aescbcencrypt(	puMonoByte Iv,\
								puMonoByte pumbInput,\
								uDiByte length,\
								const puMonoByte key,\
								const uMonoByte keylength);
/*-------------------------------------------------------------------
* Algorithm cbc AES(Decryption)*
*-------------------------------------------------------------------
*@Iv			- It's containes IV
*@pumbInput		- It's containes input cipher text or Output plane text
*@length		- It's containes length of the blocksize data
*@key			- It's containes key of the algo
*@keylength		- It's containes key length
*-----------------------------------------------------------------*/
extern void algo_aescbcdecrypt( puMonoByte Iv,\
								puMonoByte pumbInput,\
								uDiByte length,\
								const puMonoByte key,\
								const uMonoByte keylength);

/*-------------------------------------------------------------------
* Rijndael key schedule function. Takes 16-byte key and creates
* all Rijndael's internal subkeys ready for encryption.
*-----------------------------------------------------------------*/
extern void RijndaelKeySchedule(uMonoByte key[16]);


/*-------------------------------------------------------------------
* Rijndael encryption function. Takes 16-byte input and creates
* 16-byte output (using round keys already derived from 16-byte
* key).
*-----------------------------------------------------------------*/
extern void RijndaelEncrypt(uMonoByte input[16], uMonoByte output[16]);


/*-------------------------------------------------------------------
* Algorithm AES CBS Encryption 
*-------------------------------------------------------------------
*
* Takes Initial Vector, key, msg input, inputlength, output array and outputLength,
* and returns ciphered data
*
*-----------------------------------------------------------------*/
extern uQuadByte AES_CBC_ENC(const puMonoByte IV, const puMonoByte key, const puMonoByte input, uQuadByte inputLength, puMonoByte output, uQuadByte outputLength);

/*-------------------------------------------------------------------
* Algorithm AES CBS decryption
*-------------------------------------------------------------------
*
* Takes Initial Vector, key, msg input, inputlength, output array and outputLength,
* and returns ciphered data
*
*-----------------------------------------------------------------*/
extern uQuadByte AES_CBC_DEC(const puMonoByte IV, const puMonoByte key, const puMonoByte input, uQuadByte inputLength, puMonoByte output, uQuadByte outputLength);

/*-------------------------------------------------------------------
* padding_AES function
*-------------------------------------------------------------------
*
* Takes input lastb, pad and length,
* and check for padding
*
*-----------------------------------------------------------------*/
static void padding_AES(const puMonoByte lastb, puMonoByte pad, uQuadByte length);

/*-------------------------------------------------------------------
* AES-CMAC with AES-128, 192, 256 bit.
*@pumbkey- It's key data
*@udinput- It's input Length
*@udLength- It's data Length
*@udinput- It's input keyLength
*@pumbmac- IT's return 8-byte CheckSum
*-------------------------------------------------------------------*/
extern void AES_CMAC(puMonoByte key, puMonoByte input, uDiByte length, uMonoByte keylen, puMonoByte Iv, puMonoByte mac, uMonoByte umbflag);

/*-------------------------------------------------------------------
* xor function
*-------------------------------------------------------------------
*
* Takes input a and b,
* and returns out : xor of a and b
*
*-----------------------------------------------------------------*/
extern void xor(const puMonoByte a, const puMonoByte b, puMonoByte out);
/*------------------------------------------------------------------ -*/
/*generate the subkey * /
*------------------------------------------------------------------ - */
extern void generate_subkey(puMonoByte key, uMonoByte keylen,  puMonoByte K1, puMonoByte K2);

#endif // !_ALGORITHMS_H

