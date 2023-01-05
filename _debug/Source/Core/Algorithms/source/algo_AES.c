#include "Algorithms.h"

/*This is AES algorithm, only encryption implemented*/

/*-------------------------------------------------------------------
 * Rijndael key schedule function. Takes 16-byte key and creates
 * all Rijndael's internal subkeys ready for encryption.
 *-----------------------------------------------------------------*/
void RijndaelKeySchedule(uMonoByte key[16])
{
	KeyExpansion(AES_NK128, AES_NR128, key);
	return;
	
} /* end of function RijndaelKeySchedule */

/*-------------------------------------------------------------------
 * Rijndael encryption function. Takes 16-byte input and creates
 * 16-byte output (using round keys already derived from 16-byte
 * key).
 *-----------------------------------------------------------------*/
void RijndaelEncrypt(uMonoByte input[16], uMonoByte output[16])
{
	/*Need to optimize temp. change*/ 
	mem_cpy(output, input, 0x10);
	algo_aes_encrypt((state_t*)output, AES_NR128);
	
	return;
} /* end of function RijndaelEncrypt */