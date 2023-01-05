
#ifndef _MEMORY_MANAGER_EXTERN_H
#define _MEMORY_MANAGER_EXTERN_H

#include "types.h"
#include "MemoryLayout.h"
#include "Oasis.h"

/*
* Macro Defines
*/

/*	|---Avail EEP------|	|---Disk area---|	|----SMART table--------|
*		SIZE_HEAP_AREA	 =	 X*CLUSTER_SIZE   + X*sizeof(tClusterHeader)
*	X*(CLUSTER_SIZE + sizeof(tClusterHeader))	= SIZE_HEAP_AREA
*					SIZE_HEAP_AREA
*	X =	--------------------------------------
*		(CLUSTER_SIZE + sizeof(tClusterHeader))
* 
* where, X is No. of clusters
*/
#define NO_OF_CLUSTERS								(SIZE_HEAP_AREA/(CLUSTER_SIZE + sizeof(tClusterHeader)))

/*assuming no. of clusters can be stored in 2 bytes*/
#define SIZE_OF_NO_OF_CLUSTER						(0x02) 

#define SIZE_OF_MEMORY_ADDRESS						(sizeof(tMemAddr))

#define SMART_START_INDEX							(0x0001)
#define DISK_START_INDEX							(0x0001)

#define CLUSTER_BUSY								(0x00)
#define CLUSTER_FREE								(0xFF)
#define NO_FREE_CLUSTER								(0x0000)
#define END_OF_CLUSTER_CHAIN						((tMemAddr)0xFFFFFFFF)

#define NO_FREE_MEMORY								(0x0000)

#define EMPTY_OBJECT								(0x0000)

/*Typedefs */

typedef uDiByte	tMemAddr; // address type of memory area

typedef struct {

	/*Checks if data is busy or not*/
	uMonoByte	umbBusy;

	/*assuming cluster size cannot be more than 255 bytes: 0 indicates that full cluster consumed
	* TODO: No idea if we have any use of storing size in header, as size have been checked at application layer too
	* for time being size is stored, in future we can consider to remove it.
	*/
	uMonoByte	umbSize;

	/*next address of next cluster connected to this one*/
	tMemAddr	maNextCluster;

} tClusterHeader, * ptClusterHeader;

typedef enum{
	MEM_MODE_READ,
	MEM_MODE_UPDATE
} tMemMode;

typedef uMonoByte tCluster[CLUSTER_SIZE];
typedef tCluster* ptCluster;

/*Methods declaration*/
extern uDiByte		mm_makeUdiByte(uMonoByte* umbSource, uMonoByte umbIndex);

/*
* store word data into array elements
*
* @param: pumbDest Destination Array
* @param: umbIndex Index where elements to store
*
* @return umbIndex + 2
*/
extern uMonoByte mm_storeUdiByte(OUT_ARG puMonoByte pumbDest, IN_ARG uMonoByte umbIndex, IN_ARG uDiByte udbWord);

/*
* store word data into array elements
*
* @param: pumbDest Destination Array
* @param: umbIndex Index where elements to store
* @param: word data
* 
* @return umbIndex + 2
*/
extern uMonoByte mm_storeWord(OUT_ARG puMonoByte pumbDest, IN_ARG uMonoByte umbIndex, IN_ARG uQuadByte uqbWord);

extern tMemAddr		mm_alloc(uDiByte udbSize, uMonoByte umbDefaultByte);
extern uQuadByte	mm_GetAvailableMemory(void);

/*
* It writes the given data into present cluster chain
*
* @param maBlock: starting block of cluster chain
* @param pumbData: data will be read/write from/to this array
* @param udbOffset: offset of memory from where to start writing/reading data
* @param udbDataSize: Length of data to read/write in memory
* @param mmMode: Read or Write
*
* @return FALSE if maBlock is empty, otherwise TRUE
* @precondition: Caller ensure Data size to update must not exceed the allocated size
*/
extern boolean	mm_ReadUpdate(IN_ARG tMemAddr	maBlock, INOUT_ARG puMonoByte	pumbData, IN_ARG uDiByte udbOffset, IN_ARG uDiByte udbDataSize, IN_ARG tMemMode	mmMode);
extern void			mm_Free(tMemAddr maAllocatedAddr);

/*
* It compares the given data (RAM) with given cluster chain
*
* @param maBlock: starting block of cluster chain
* @param pumbData: data to compare
* @param udbOffset: offset of memory from where to start comparing data
* @param udbDataSize: Length of data to compare in memory
*
* @return FALSE if maBlock is empty or data doesnt match, otherwise TRUE
* @precondition: Caller ensure Data size to compare must not exceed the allocated size
*/
extern uMonoByte mm_cmpRam(	IN_ARG		tMemAddr	maBlock,
							INOUT_ARG	puMonoByte	pumbData,
							IN_ARG		uDiByte		udbOffset,
							IN_ARG		uDiByte		udbDataSize);

/*
* It prints the Memory related information
*/
extern void printMemoryConfig(void);
#endif // !_MEMORY_MANAGER_EXTERN_H
