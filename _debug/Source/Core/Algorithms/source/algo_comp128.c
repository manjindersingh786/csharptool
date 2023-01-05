#include "Algorithms.h"

/*function definitions */
const uMonoByte L1[256] =
{
 197, 235, 60, 151, 98, 96, 3, 100, 248, 118, 42, 117, 172, 211, 181, 203, 61, 126, 156, 87, 149, 224, 55, 132, 186, 63, 238, 255, 85, 83, 152, 33, 160, 184, 210, 219, 159, 11, 180, 194, 130, 212, 147, 5, 215, 92, 27, 46, 113, 187, 52, 25, 185, 79, 221, 48, 70, 31, 101, 15, 195, 201, 50, 222, 137, 233, 229, 106, 122, 183, 178, 177, 144, 207, 234, 182, 37, 254, 227, 231, 54, 209, 133, 65, 202, 69, 237, 220, 189, 146, 120, 68, 21, 125, 38, 30, 2, 155, 53, 196, 174, 176, 51, 246, 167, 76, 110, 20, 82, 121, 103, 112, 56, 173, 49, 217, 252, 0, 114, 228, 123, 12, 93, 161, 253, 232, 240, 175, 67, 128, 22, 158, 89, 18, 77, 109, 190, 17, 62, 4, 153, 163, 59, 145, 138, 7, 74, 205, 10, 162, 80, 45, 104, 111, 150, 214, 154, 28, 191, 169, 213, 88, 193, 198, 200, 245, 39, 164, 124, 84, 78, 1, 188, 170, 23, 86, 226, 141, 32, 6, 131, 127, 199, 40, 135, 16, 57, 71, 91, 225, 168, 242, 206, 97, 166, 44, 14, 90, 236, 239, 230, 244, 223, 108, 102, 119, 148, 251, 29, 216, 8, 9, 249, 208, 24, 105, 94, 34, 64, 95, 115, 72, 134, 204, 43, 247, 243, 218, 47, 58, 73, 107, 241, 179, 116, 66, 36, 143, 81, 250, 139, 19, 13, 142, 140, 129, 192, 99, 171, 157, 136, 41, 75, 35, 165, 26
};
		
const uMonoByte M11[256] =
{
 170, 42, 95, 141, 109, 30, 71, 89, 26, 147, 231, 205, 239, 212, 124, 129, 216, 79, 15, 185, 153, 14, 251, 162, 0, 241, 172, 197, 43, 10, 194, 235, 6, 20, 72, 45, 143, 104, 161, 119, 41, 136, 38, 189, 135, 25, 93, 18, 224, 171, 252, 195, 63, 19, 58, 165, 23, 55, 133, 254, 214, 144, 220, 178, 156, 52, 110, 225, 97, 183, 140, 39, 53, 88, 219, 167, 16, 198, 62, 222, 76, 139, 175, 94, 51, 134, 115, 22, 67, 1, 249, 217, 3, 5, 232, 138, 31, 56, 116, 163, 70, 128, 234, 132, 229, 184, 244, 13, 34, 73, 233, 154, 179, 131, 215, 236, 142, 223, 27, 57, 246, 108, 211, 8, 253, 85, 66, 245, 193, 78, 190, 4, 17, 7, 150, 127, 152, 213, 37, 186, 2, 243, 46, 169, 68, 101, 60, 174, 208, 158, 176, 69, 238, 191, 90, 83, 166, 125, 77, 59, 21, 92, 49, 151, 168, 99, 9, 50, 146, 113, 117, 228, 65, 230, 40, 82, 54, 237, 227, 102, 28, 36, 107, 24, 44, 126, 206, 201, 61, 114, 164, 207, 181, 29, 91, 64, 221, 255, 48, 155, 192, 111, 180, 210, 182, 247, 203, 148, 209, 98, 173, 11, 75, 123, 250, 118, 32, 47, 240, 202, 74, 177, 100, 80, 196, 33, 248, 86, 157, 137, 120, 130, 84, 204, 122, 81, 242, 188, 200, 149, 226, 218, 160, 187, 106, 35, 87, 105, 96, 145, 199, 159, 12, 121, 103, 112
};


/*
 * Compression function for Comp 128 Two and three algorithm.
 *
 * @param          A     array contains A parameter
 * <BR>
 * @param          B     array contains B parameter
 * <BR>
 * @param          C     array contains C parameters
 */
static void CompF( uMonoByte *A, uMonoByte *B, uMonoByte *C ) 
{
	uMonoByte Y[ 32 ], Z[ 16 ];
	uMonoByte i, j, k;

	mem_cpy(Y, B, (uMonoByte) 16);
	mem_cpy( Y+16,A, (uMonoByte) 16);
	  
	for ( k = 0; k < 5; k++ ) 
	{
	   	for ( j = 0; j < 16; j++ ) 
	   	{
	   		Z[ j ] = L1[ Y[ j ] ^ ( M11[ ( Y[ j + 16 ] ) ] ) ];
	   	}
			
	   	for ( i = 0; i < ( 1 << k ); i++ ) 
	    {
	    	for ( j = 0; j < ( 1 << ( 4-k ) ); j++ ) 
	    	{
	     		Y[ ( ( 2 * j + 1 ) << k ) + i ] = L1[ ( ( Y[ ( j << k ) + i + 16 ] ) ^ ( M11[ Z[ ( j << k ) + i] ] ) ) ];
				Y[ ( j << ( k + 1 ) ) + i ] = Z[ ( j << k ) + i ];
	      	}
	    }
	}

//	halInitBuffer(C, (uDiByte) 16);
	mem_set(C,0x00,(uDiByte) 16);
	// now compute the 16- byte output, C
	for ( i = 0; i < 16; i++ ) 
	{
		uMonoByte temp = i<<3;
	   	for ( j = 0; j < 8; j++ )  
		{
			C[ i ] = (uMonoByte)(C[ i ] ^ ( ( ( Y[ ( ( 19 *( temp + j + 1 ) ) & 0xFF ) >> 3 ] >> ( ( 3 *j + 3 ) & 0x07 ) ) & 1 ) << j ) );
	    }
	}  

}/* End of CompF */


/*
 * Perform A3A8-algorithm Comp128 Three.
 *
 * @pre Random data must be coppied to first 16 bytes of common buffer.
 * @pre 16 bytes of Ki value must be copied in common buffer starting 16th offset .
 *
 * @post SRES valued coppied to first 4 bytes of common buffer.
 * @post 8 bytes ok KC key valued coppied in common buffer starting 4th offset
* @param  pumbRand: Random data
* @param  pumbArray: Ki
* @param  pumbOutput: 4 bytes SRES and last 8 bytes are Kc
 * @return void
 */
void fvComp128Three(puMonoByte pumbRandData, puMonoByte pumbKiArray, puMonoByte pumbOutput)
{
    uMonoByte umbRandomNo[ 0x10 ], umbKeyArray[0x10];
	uMonoByte umbLoop;
	// the initial value of R is the input challenge RAND
	for ( umbLoop = 0x00; umbLoop < 0x10; umbLoop++ )
	{
	    /*Copy 16 bytes of R Parameter form Input buffer
		First 16 bytes of input buffer contains Randam data */
	    umbRandomNo[ 0x0F - umbLoop ] = pumbRandData[ umbLoop ];
		
		/*Calculate 16 bytes of K parameter
		InputBuffer[16-31] contains 16 bytes of Ki data */
	  	umbKeyArray[  0x0F - umbLoop ] = (uMonoByte)( (*(pumbKiArray + (umbLoop))) ^ pumbRandData[ umbLoop ]);
	}

	/*compute eight rounds of the compression function */
	for ( umbLoop = 0x00; umbLoop < 0x08; umbLoop++ )
	{
	    CompF( umbKeyArray, umbRandomNo, umbRandomNo );
	}
	
	/*Copy 4 bytes of SRES value in input buffer
	SRES[umbLoop]=R[15-umbLoop] */
	for ( umbLoop = 0x00; umbLoop < 0x04; umbLoop++ )
	{
		pumbOutput[umbLoop] = umbRandomNo[ 0x0F - umbLoop ];
	}
	
	/*Copy 8 bytes of KC data in input buffer from 4th offset
	KC[umbLoop] = R[0x07-umbLoop] */
	for ( umbLoop = 0x00; umbLoop < 0x08; umbLoop++ )
	{
		pumbOutput[umbLoop + 0x04] = umbRandomNo[ 0x07 - umbLoop ];
	}

}/* End of fvComp128Three */