#include "MemoryManager.h"


/*
* Convert array elements into half Word
* 
* @param: pumbSource Source Array
* @param: umbIndex Index from which elements to pick
*/
uDiByte mm_makeUdiByte(IN_ARG puMonoByte pumbSource, IN_ARG uMonoByte umbIndex) {

	/*Ensure source cant be null*/
	assert(pumbSource != NULL);

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_makeUdiByte() \n");

	/*Return uDiByte word: AA,BB : 0xAABB*/
	DBG_FUNC_EXIT("<<[MemoryManager] mm_makeUdiByte() \n");
	return ((uDiByte)(((uDiByte)(pumbSource[umbIndex] << 8)) | (pumbSource[umbIndex + 1])));
}

/*
* store half word data into array elements
*
* @param: pumbDest Destination Array
* @param: umbIndex Index where elements to store
* @param: udbWord	: Half word to store
* 
* @return umbIndex + 4
*/
uMonoByte mm_storeUdiByte(OUT_ARG puMonoByte pumbDest, IN_ARG uMonoByte umbIndex, IN_ARG uDiByte udbWord) {

	/*Ensure dest cant be null*/
	assert(pumbDest != NULL);

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_storeUdiByte() \n");

	pumbDest[umbIndex++] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(udbWord, 0x08);
	pumbDest[umbIndex++] = (uMonoByte)udbWord;

	/*Return uDiByte word: AA,BB : 0xAABB*/
	DBG_FUNC_EXIT("<<[MemoryManager] mm_storeUdiByte() returns %1\n", umbIndex);
	return umbIndex;
}

/*
* store word data into array elements
*
* @param: pumbDest Destination Array
* @param: umbIndex Index where elements to store
* @param: word data
* 
* @return umbIndex + 2
*/
uMonoByte mm_storeWord(OUT_ARG puMonoByte pumbDest, IN_ARG uMonoByte umbIndex, IN_ARG uQuadByte uqbWord) {

	/*Ensure dest cant be null*/
	assert(pumbDest != NULL);

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_storeWord() \n");

	pumbDest[umbIndex++] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(uqbWord, 0x18);
	pumbDest[umbIndex++] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(uqbWord, 0x10);
	pumbDest[umbIndex++] = (uMonoByte)SHIFT_RIGHT_BY_N_BITS(uqbWord, 0x08);
	pumbDest[umbIndex++] = (uMonoByte)uqbWord;

	/*Return uDiByte word: AA,BB : 0xAABB*/
	DBG_FUNC_EXIT("<<[MemoryManager] mm_storeUdiByte() returns %1\n", umbIndex);
	return umbIndex;
}