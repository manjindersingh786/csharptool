#include "Algorithms.h"

//Variables definition


/*-------------------------------------------------------------------
*algo_crc()- Takes Some Input and Length sizeof(Input), and creates 2-byte(16-bit) or 4-byte(32-bit) CHECKSUM
*@CryptoObject- It's store all
*@pumbalgo-	    It's algo type
*@udLength-		It's data Length
*@pumbCheckSum- IT's return 2-bytes or 4-byte CheckSum
*-------------------------------------------------------------------*/
uQuadByte algo_crc(INOUT_ARG uQuadByte uqbIntermediateResult, IN_ARG tChecksumAlgo algo, IN_ARG puMonoByte pumbInput, IN_ARG uDiByte udLength) {
	
	uDiByte		udIndexby;
	uDiByte		udIndexbt;
	uDiByte		udCarry;
	uQuadByte   uQdCRC;

	if (algo == CHECKSUM_CRC16) {  /*Reset Intermediate data to default value as per given algo type*/
		uQdCRC = CRC_16_POLYNOMIAL;
	}
	else {
		uQdCRC = CRC_32_POLYNOMIAL;
	}

	DBG_FUNC(">>[Algorithm]algo_crc() Calculate CheckSum for CRC Algo \n");

	for (udIndexby = 0;udIndexby < udLength;udIndexby++) {
		for (udIndexbt = 0;udIndexbt < 8;udIndexbt++) {
			udCarry = uqbIntermediateResult & 1;
			uqbIntermediateResult >>= 1;
			if (udCarry ^ ((pumbInput[udIndexby] >> udIndexbt) & 1))
				uqbIntermediateResult ^= uQdCRC;
		}
	}
	DBG_FUNC_EXIT("<<[Algorithm]algo_crc() Calculate CheckSum for CRC Algo \n");

	uqbIntermediateResult = ~uqbIntermediateResult; /* invert CRC */
	
	return uqbIntermediateResult;
}
