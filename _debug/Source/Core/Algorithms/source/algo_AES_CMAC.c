#include "Algorithms.h"
/****************************************************************/
/* AES-CMAC with AES-128,192 AND 256 bit                        */
/****************************************************************/

/* AES-CMAC Generation Function */

 static void leftshift_onebit(puMonoByte input, puMonoByte output) {

	 sDiByte i;
	uMonoByte overflow = 0;

	for (i = LAST_INDEX; i >= 0; i--) {
		output[i] = input[i] << 1;
		output[i] |= overflow;
		overflow = (input[i] & 0x80) ? 1 : 0;
	}
	return;
}


 void xor(const puMonoByte a, const puMonoByte b, puMonoByte out) {
	 uDiByte i;
	 for (i = 0; i < BLOCK_SIZE; i++) {
		 out[i] = a[i] ^ b[i];
	 }
 }

/*generate the subkey */
 void generate_subkey(puMonoByte key, uMonoByte keylen, puMonoByte K1, puMonoByte K2) {

	 /* For CMAC Calculation */
	 uMonoByte Iv[BLOCK_SIZE];
	 uMonoByte const_Rb[BLOCK_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87 };

	uMonoByte const_Zero[BLOCK_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	uMonoByte tmp[BLOCK_SIZE];
	mem_set(Iv, 0, BLOCK_SIZE);
	algo_aescbcencrypt(Iv,const_Zero, BLOCK_SIZE, key, keylen);

	if ((const_Zero[0] & 0x80) == 0) { /* If MSB(L) = 0, then K1 = L << 1 */
		leftshift_onebit(const_Zero, K1);
	}
	else { /* Else K1 = ( L << 1 ) (+) Rb */

		leftshift_onebit(const_Zero, tmp);
		xor(tmp, const_Rb,  K1);
	}

	if ((K1[0] & 0x80) == 0) {
		leftshift_onebit(K1, K2);
	}
	else {
		leftshift_onebit(K1, tmp);
		xor(tmp, const_Rb, K2);
	}
	return;
}

static void padding(puMonoByte lastb, puMonoByte pad, uDiByte length) {

	uDiByte j;

	/* original last block */
	for (j = 0; j < BLOCK_SIZE; j++) {
		if (j < length) {
			pad[j] = lastb[j];
		}
		else if (j == length) {
			pad[j] = 0x80;
		}
		else {
			pad[j] = 0x00;
		}
	}
}
/*AES-CMAC with AES-128,192,256 bit*/

void AES_CMAC(puMonoByte key, puMonoByte input, uDiByte length, uMonoByte keylen, puMonoByte Iv, puMonoByte mac, uMonoByte umbflag) {

	uMonoByte Y[BLOCK_SIZE], M_last[BLOCK_SIZE], padded[BLOCK_SIZE];
	
	uMonoByte K1[BLOCK_SIZE], K2[BLOCK_SIZE];
	uDiByte n, i, flag;

	
	n = (length + LAST_INDEX) / BLOCK_SIZE; /* n is number of rounds */

	if (n == 0) {
		n = 1;
		flag = 0;
	}
	else {
		if ((length % BLOCK_SIZE) == 0) { /* last block is a complete block */
			flag = 1;
		}
		else { /* last block is not complete block */
			flag = 0;
		}
	}
	mem_cpy(Y, input, BLOCK_SIZE);
	//mem_set(X, 0, BLOCK_SIZE);
	if (umbflag==0x00) {
		for (i = 0; i < n; i++) {

			//xor (Iv, &input[BLOCK_SIZE * i], Y); /* Y := Mi (+) X  */

			algo_aescbcencrypt(Iv, Y, BLOCK_SIZE, key, keylen);

			mem_cpy(Iv, Y, BLOCK_SIZE);

		}
	}
	if (umbflag) {

		generate_subkey(key, keylen, K1, K2);

		if (flag) { /* last block is complete block */
			xor (&input[BLOCK_SIZE * (n - 1)], K1, M_last);
		}
		else {
			padding(&input[BLOCK_SIZE * (n - 1)], padded, length % BLOCK_SIZE);
			xor (padded, K2, M_last);
		}
		//xor (Iv, M_last, Y);
		algo_aescbcencrypt(Iv, M_last, BLOCK_SIZE, key, keylen);

		mem_cpy(mac, M_last, BLOCK_SIZE);
	}
	mem_cpy(Iv, Y, BLOCK_SIZE);
	
}
