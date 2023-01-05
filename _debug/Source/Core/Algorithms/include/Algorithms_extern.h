
#ifndef _ALGORITHMS_EXTERN_H
#define _ALGORITHMS_EXTERN_H
#include "types.h"
#include "Oasis.h"


#define MAX_BLOCK_SIZE_FOR_ANY_CIPHER_ALGO						(0x20) // DES Block size is 8 and AES Max block size is 256bit i.e. 32 bytes
#define MAX_LENGTH_OF_KEY_IN_ANY_ALGO							(0x20) // 3DES max key length is 24 bytes and AES max key length is 32 bytes, hence we choose 32 bytes

#define OFFSET_ALGO_KEY_LENGTH									(0x00)
#define OFFSET_ALGO_KEY_VALUE									(OFFSET_ALGO_KEY_LENGTH + 0x01)
#define LENGTH_KEY_OBJECT										(OFFSET_ALGO_KEY_VALUE + MAX_LENGTH_OF_KEY_IN_ANY_ALGO)


#define AES_NR128														(0x0a)
#define AES_NR192														(0x0c)
#define AES_NR256														(0x0e)
#define AES_NK128														(0x04)
#define AES_NK192														(0x06)
#define AES_NK256														(0x08)


/* Perform COMP128v3 algorithm
 *  @param [in] pumbRand pumbRandom Challenge
 *  @param [in] pumbKi Secret Key K(i) of subscriber
 *  @param [out] pumbOutput user-supplied buffer for storing computed pumbSres value and storing computed pumbKc value
 *
 */

extern void fvComp128Three(puMonoByte pumbRandData, puMonoByte pumbArray, puMonoByte pumbOutput);


/*-------------------------------------------------------------------
 * Algorithm f1
 *-------------------------------------------------------------------
 *
 * Computes network authentication code MAC-A from key K, random
 * challenge RAND, sequence number SQN and authentication management
 * field AMF.
 *
 *-----------------------------------------------------------------*/
extern void f1(puMonoByte k, puMonoByte rand, puMonoByte sqn, puMonoByte amf, puMonoByte op_c, puMonoByte mac_a);


/*-------------------------------------------------------------------
 * Algorithms f2-f5
 *-------------------------------------------------------------------
 *
 * Takes key K and random challenge RAND, and returns response RES,
 * confidentiality key CK, integrity key IK and anonymity key AK.
 *
 *-----------------------------------------------------------------*/
extern void f2345(puMonoByte k, puMonoByte rand, puMonoByte op_c, puMonoByte res, puMonoByte ck, puMonoByte ik, puMonoByte ak);

/*-------------------------------------------------------------------
 * Algorithm f1*
 *-------------------------------------------------------------------
 *
 * Computes resynch authentication code MAC-S from key K, random
 * challenge RAND, sequence number SQN and authentication management
 * field AMF.
 *
 *-----------------------------------------------------------------*/
extern void f1star(puMonoByte k, puMonoByte rand, puMonoByte sqn, puMonoByte amf, puMonoByte op_c, puMonoByte mac_s);

/*-------------------------------------------------------------------
 * Algorithm f5*
 *-------------------------------------------------------------------
 *
 * Takes key K and random challenge RAND, and returns resynch
 * anonymity key AK.
 *
 *-----------------------------------------------------------------*/
extern void f5star(puMonoByte k, puMonoByte rand, puMonoByte op_c, puMonoByte ak);

#endif // !_ALGORITHMS_EXTERN_H
