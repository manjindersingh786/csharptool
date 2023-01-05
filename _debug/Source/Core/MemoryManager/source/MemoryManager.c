#include "MemoryManager.h"

/*
* SMART- SIM Memory Allocation Root Table (like FAT)
* Disk Memory (Chunks of cluster)
*/

/*
* It finds first free cluster header after traversing SMART
*
* @param maLastReturnedCluster, Last free cluster returned, this param is passed to avoid re-traversing
*					the whole table
* @return tMemAddr, first free cluster header in SMART
*/
static tMemAddr mm_getFreeCluster(IN_ARG tMemAddr maLastReturnedCluster) {

	uDiByte				udbClusterOffset; 
	uDiByte				udbNoOfClustersProcessed = 0x00;
	tMemAddr			maBlockIndex; //contains SMART index
	tClusterHeader		chCurrFreeHeader;  //stores cluster header present in SMART

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_alloc() \n");

	/*check if no memory available*/
	if (NO_FREE_CLUSTER == maLastReturnedCluster)
	{
		DBG_FUNC_EXIT("<<[MemoryManager] mm_getFreeCluster() returns NO_FREE_MEMORY %2\n", NO_FREE_MEMORY);
		return NO_FREE_MEMORY;
	}

	/*Starts traversing from given Cluster no.*/
	maBlockIndex = maLastReturnedCluster;

	do
	{
		/*calculate offset of cluster header*/
		udbClusterOffset = sizeof(tClusterHeader) * (maBlockIndex - 1);

		/*Read back cluster in SMART*/
		baseReadNBytes(ADDRESS_SMART_START + udbClusterOffset, (puMonoByte) &chCurrFreeHeader, sizeof(tClusterHeader));

		/*check if its free*/
		if (chCurrFreeHeader.umbBusy != CLUSTER_BUSY) {

			/*return free cluster index*/
			DBG_FUNC_EXIT("<<[MemoryManager] mm_getFreeCluster() returns %2\n", maBlockIndex);
			return maBlockIndex;
		}
		if (NO_OF_CLUSTERS == maBlockIndex) {

			/*reset to first cluster for further traversing*/
			maBlockIndex = SMART_START_INDEX;
		}
		else {

			/*next cluster*/
			maBlockIndex++;
		}

		/*increment the total no. of clusters processed so far*/
		udbNoOfClustersProcessed++;

	} while (NO_OF_CLUSTERS != udbNoOfClustersProcessed); //If all clusters are processed

	/*if control is here, it means no free cluster*/
	DBG_FUNC_EXIT("<<[MemoryManager] mm_getFreeCluster() returns NO_FREE_CLUSTER %2\n", NO_FREE_CLUSTER);
	return NO_FREE_CLUSTER;

}
/*
* It allocates the Cluster header and memory Clusters to the requester as per required size.
* If there are no. of clusters, it connects them by making chain.
* 
* @param udbSize: Total size of memory required by requester
* 
* @return tMemAddr index of first cluster header in allocated memory.
*/
tMemAddr mm_alloc(IN_ARG uDiByte udbSize, IN_ARG uMonoByte umbDefaultByte)
{
#ifdef FEAT_EXHAUSTEARING	
	boolean 			bCloseTheTxn=FALSE;
#endif
	uDiByte				udbNoOfClusters; //Store no. of clusters required for object
	uDiByte				udbRemainingClusters;
	tMemAddr			maBlockIndex = 0x00; //will contain next free mem block address
	tMemAddr			maPrevIndex = END_OF_CLUSTER_CHAIN; //Stores index of previous cluster
	tClusterHeader		chCurrFreeHeader;  //stores cluster header present in SMART
	tCluster			cClusterData = { 0x00 };
	uQuadByte			uqbClusterOffset; //stores relative offset of cluster header in SMART

	/*Check size*/
	assert(udbSize != 0x00);

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_alloc()\n");

	/*Count no. of clusters required*/
	udbNoOfClusters = udbSize / CLUSTER_SIZE;

	DBG_FUNC("[MemoryManager] mm_alloc: Requested size of memory %2 umbDefaultByte %1\n", udbSize, umbDefaultByte);

	/*Check if remainder is there*/
	if (udbSize % CLUSTER_SIZE){

		/*increment cluster as there is extra data which requires 1 cluster more*/
		udbNoOfClusters++;
	}

	DBG_FUNC("[MemoryManager] mm_alloc: No. of Clusters required %2\n", udbNoOfClusters);

	/*read no. of free clusters*/
	baseReadNBytes(ADDRESS_NO_OF_FREE_CLUSTERS, (puMonoByte) &udbRemainingClusters, SIZE_OF_NO_OF_CLUSTER);

	DBG_FUNC("[MemoryManager] mm_alloc: Remaining No. of Clusters %2\n", udbRemainingClusters);

	/*Check if no. of free clusters are sufficient*/
	if (udbNoOfClusters > udbRemainingClusters)
	{
		/*insufficient memory*/
		DBG_WARN("[MemoryManager] mm_alloc: Required no. of clusters is more than remaining clusters \n");
		DBG_FUNC_EXIT("<<[MemoryManager] mm_alloc() returns NO_FREE_MEMORY %2\n", NO_FREE_MEMORY);
		return NO_FREE_MEMORY;
	}

	if (umbDefaultByte) { //if non-zero default byte passed
		
		/*Fill the all elements of clutserdata with default value*/
		mem_set(cClusterData, umbDefaultByte, CLUSTER_SIZE);
	}

	/*remaining no of clusters*/
	udbRemainingClusters -= udbNoOfClusters;

	/*Read the next available cluster from system page*/
	baseReadNBytes(ADDRESS_NEXT_FREE_CLUSTER, (puMonoByte) &maBlockIndex, SIZE_OF_MEMORY_ADDRESS);

#ifdef FEAT_EXHAUSTEARING
	/*begin the transaction*/
	if (TRUE == at_beginTxn()) { 

		/*indication to close the transaction in same function*/
		bCloseTheTxn = TRUE;
	}
#endif

	do {
		/*Get free cluster*/
		maBlockIndex = mm_getFreeCluster(maBlockIndex);

		/*Set busy on that cluster*/
		chCurrFreeHeader.umbBusy = CLUSTER_BUSY;

		/*Store index to previous cluster*/
		chCurrFreeHeader.maNextCluster = maPrevIndex;

		/*Check if data less than cluster size*/
		if (udbSize < CLUSTER_SIZE) {

			/*note used data length*/
			chCurrFreeHeader.umbSize = (uMonoByte)udbSize;

			/*No more clusters required*/
			udbSize = 0x00;
		}
		else {

			/*indicates that cluster is fully used*/
			chCurrFreeHeader.umbSize = 0x00;

			/*Remaining required size of memory*/
			udbSize -= CLUSTER_SIZE; 
		}

		/*calculate offset of cluster header*/
		uqbClusterOffset = sizeof(tClusterHeader) * (maBlockIndex - 1);

		/*write back cluster header in SMART*/
		baseWriteNBytes(ADDRESS_SMART_START + uqbClusterOffset, (puMonoByte)&chCurrFreeHeader, sizeof(tClusterHeader));

		/*calculate offset of cluster data*/
		uqbClusterOffset = sizeof(tCluster) * (maBlockIndex - 1);

		/*write cluster in DISK area*/
		baseWriteNBytes(ADDRESS_CLUSTER_START + uqbClusterOffset, (puMonoByte)&cClusterData, sizeof(tCluster));

		/*Stores index of current cluster as previous*/
		maPrevIndex = maBlockIndex;

		DBG_FUNC("[MemoryManager] mm_alloc: Cluster %2 allocated\n", maBlockIndex);

		/*decrement cluster as one already located*/
		udbNoOfClusters--;

	} while (udbNoOfClusters); //if no more clusters required

	DBG_FUNC("[MemoryManager] mm_alloc: Memory data has been allocated to requestor\n");

	/*search for next free cluster*/
	maPrevIndex = mm_getFreeCluster(maBlockIndex);

	DBG_FUNC("[MemoryManager] mm_alloc: Next free cluster %2\n", maPrevIndex);

	/*Write the next available cluster on system page*/
	baseWriteNBytes(ADDRESS_NEXT_FREE_CLUSTER, (puMonoByte) &maPrevIndex, SIZE_OF_MEMORY_ADDRESS);

	DBG_FUNC("[MemoryManager] mm_alloc: Remaining No. of clusters %2\n", udbRemainingClusters);

	/*write free clusters*/
	baseWriteNBytes(ADDRESS_NO_OF_FREE_CLUSTERS, (puMonoByte) &udbRemainingClusters, SIZE_OF_NO_OF_CLUSTER);

#ifdef FEAT_EXHAUSTEARING

	if (TRUE == bCloseTheTxn) {
		
		/*commit the txn started in the beginning of function*/
		at_commitTxn();
	}
#endif

	DBG_FUNC_EXIT("<<[MemoryManager] mm_alloc() returns %2\n", maBlockIndex);

	/*return Starting allocated cluster*/
	return maBlockIndex;
}

/*
* It returns the free memory in NVM
*
* @param udbSize: Total size of memory required by requester
*
* @return tMemAddr index of first cluster header in allocated memory.
*/
uQuadByte mm_GetAvailableMemory(void) {
	/*stores remaining no. of free clusters*/
	uDiByte udbRemainingClusters;
	
	DBG_FUNC_ENTRY(">>[MemoryManager] mm_GetAvailableMemory() \n");

	DBG_FUNC("[MemoryManager] mm_GetAvailableMemory: Cluster size is %2\n", CLUSTER_SIZE);

	/*read no. of free clusters*/
	baseReadNBytes(ADDRESS_NO_OF_FREE_CLUSTERS, (puMonoByte) &udbRemainingClusters, SIZE_OF_NO_OF_CLUSTER);

	DBG_FUNC("[MemoryManager] mm_GetAvailableMemory: Free clusters are %2\n", udbRemainingClusters);

	DBG_FUNC_EXIT("<<[MemoryManager] mm_GetAvailableMemory() returns %4\n", (udbRemainingClusters * CLUSTER_SIZE));

	/*return free memory*/
	return (udbRemainingClusters * CLUSTER_SIZE);
}

/*
* It Read/writes the given data into present cluster chain
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
boolean mm_ReadUpdate(IN_ARG		tMemAddr	maBlock,
						INOUT_ARG	puMonoByte	pumbData, 
						IN_ARG		uDiByte		udbOffset, 
						IN_ARG		uDiByte		udbDataSize, 
						IN_ARG		tMemMode	mmMode) {

	uMonoByte	umbDataLength; //length of data to read/write in a cluster
	uQuadByte	uqbClusterOffset; // stores the exact offset 
#ifdef FEAT_EXHAUSTEARING
	boolean bCloseTheTxn = FALSE;
#endif

	tClusterHeader chCurrHeader; //stores cluster header
	
	/*Check address must not be NULL*/
	assert(pumbData != NULL);

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_ReadUpdate() \n");

	DBG_FUNC("[MemoryManager] mm_ReadUpdate: maBlock %2  udbOffset %2 udbDataSize %2  mmMode %1\n", maBlock, udbOffset, udbDataSize, mmMode);

	/*Check if Clusterindex is not invalid one*/
	if (EMPTY_OBJECT == maBlock)
	{
		/*invalid cluster/block index*/
		DBG_FUNC_EXIT("<<[MemoryManager] mm_ReadUpdate() returns FALSE i.e. %1\n", FALSE);
		return FALSE;
	}

	if (0x00 == udbDataSize) {
		/*when Data size is zero*/
		DBG_FUNC_EXIT("<<[MemoryManager] mm_ReadUpdate() returns TRUE i.e. %1\n", TRUE);
		return TRUE;
	}
#ifdef FEAT_EXHAUSTEARING
	if (MEM_MODE_UPDATE == mmMode) {

		if (TRUE == at_beginTxn()) {
			
			DBG_FUNC("[MemoryManager] mm_ReadUpdate: Start the transaction, as memory is going to be change\n");

			/*indication to close the transaction in same function*/
			bCloseTheTxn = TRUE;
		}
	}
#endif

	do {

		/*check if Offset is less than Cluster size*/
		if (udbOffset < CLUSTER_SIZE) //Read/writing can be start from current cluster
		{
			DBG_FUNC("[MemoryManager] mm_ReadUpdate: Offset %2 of cluster %2 \n", udbOffset, maBlock);

			/*calculate offset within cluster data*/
			uqbClusterOffset = (sizeof(tCluster) * (maBlock - 1)) + udbOffset;

			/*Check if offset + data size less than cluster size*/
			if ((udbDataSize + udbOffset) < CLUSTER_SIZE) {

				/*if control is here, it means this is last cluster to read, for fetching all data*/
				/*note used data length*/
				umbDataLength = (uMonoByte)udbDataSize;
			}
			else {
				/*if control is here, it means we will read few data from this cluster and rest of the data from 
				* remaining clusters
				*/
				/*indicates that cluster is fully used*/
				umbDataLength = CLUSTER_SIZE - udbOffset;
			}

			if (MEM_MODE_READ == mmMode){

				DBG_FUNC("[MemoryManager] mm_ReadUpdate: Reading data of length %1 \n", umbDataLength);

				/*Read cluster from memory*/
				baseReadNBytes(ADDRESS_CLUSTER_START + uqbClusterOffset, pumbData, umbDataLength);
			}
			else {//MEM_MODE_UPDATE

				DBG_FUNC("[MemoryManager] mm_ReadUpdate: Writing data of length %1 \n", umbDataLength);

				/*write data in cluster of memory*/
				baseWriteNBytes(ADDRESS_CLUSTER_START + uqbClusterOffset, pumbData, umbDataLength);
			}

			/*increase data pointer*/
			pumbData += umbDataLength;

			/*Reduce size as well*/
			udbDataSize -= umbDataLength;

			DBG_FUNC("[MemoryManager] mm_ReadUpdate: Remaining Size %2 \n", udbDataSize);

			/*Reset to 0*/
			udbOffset = 0x00;

		}
		else {// it means offset doesnt fall under this cluster

			/*Reduce cluster size from offset as current cluster is skipped*/
			udbOffset -= CLUSTER_SIZE;
		}

		/*calculate offset of cluster header*/
		uqbClusterOffset = sizeof(tClusterHeader) * (maBlock - 1);

		/*Read cluster header from SMART*/
		baseReadNBytes(ADDRESS_SMART_START + uqbClusterOffset,(puMonoByte) &chCurrHeader, sizeof(tClusterHeader));

		/*get next block index*/
		maBlock = chCurrHeader.maNextCluster;

		DBG_FUNC("[MemoryManager] mm_ReadUpdate: Fetch next cluster %2 \n", maBlock);

	} while((udbDataSize)						//still there is data to Read/update
			&&									//AND
			(END_OF_CLUSTER_CHAIN != maBlock)); //havent reached the end of chain yet

#ifdef FEAT_EXHAUSTEARING
	if (MEM_MODE_UPDATE == mmMode) {

		if (TRUE == bCloseTheTxn) {
			
			DBG_FUNC("[MemoryManager] mm_ReadUpdate: Close the transaction, Commit all changes permanent\n");
			
			/*commit the txn started in the beginning of function*/
			at_commitTxn();
		}
	}
#endif

	DBG_FUNC_EXIT("<<[MemoryManager] mm_ReadUpdate() returns TRUE i.e. %1\n", TRUE);

	/*all data has been Read/updated assuming no data left to write as caller already ensured that*/
	return TRUE;
}

/*
* It frees the given allocated memory corresponding to given memory address.
*
* @param udbSize: Total size of memory required by requester
*
* @return tMemAddr index of first cluster header in allocated memory.
*/
void mm_Free(IN_ARG tMemAddr maAllocatedAddr)
{
	uDiByte			udbRemainingClusters; //Free clusters
	uDiByte			udbBackup;
	uQuadByte		uqbClusterOffset;	//stores offset to cluster header
	tClusterHeader	chCurrHeader;		//Cluster header

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_Free() \n");

	/*read no. of free clusters*/
	baseReadNBytes(ADDRESS_NO_OF_FREE_CLUSTERS, (puMonoByte)&udbRemainingClusters, SIZE_OF_NO_OF_CLUSTER);

	/*Backup no. of free clusters*/
	udbBackup = udbRemainingClusters;

	DBG_FUNC("[MemoryManager] mm_Free: So far no. of free clusters are '%2' \n", udbRemainingClusters);

	while (END_OF_CLUSTER_CHAIN != maAllocatedAddr) {

		/*calculate offset of cluster header*/
		uqbClusterOffset = sizeof(tClusterHeader) * (maAllocatedAddr - 1);

		/*Read cluster header from SMART*/
		baseReadNBytes(ADDRESS_SMART_START + uqbClusterOffset,(puMonoByte) &chCurrHeader, sizeof(tClusterHeader));

		/*free the memory*/
		chCurrHeader.umbBusy = CLUSTER_FREE;

		DBG_FUNC("[MemoryManager] mm_Free: Releasing cluster '%2' \n", maAllocatedAddr);

		/*Write back cluster header from SMART*/
		baseWriteNBytes(ADDRESS_SMART_START + uqbClusterOffset, (puMonoByte) &chCurrHeader, sizeof(tClusterHeader));

		/*get next cluster in chain*/
		maAllocatedAddr = chCurrHeader.maNextCluster;

		/*Increment the free clusters, as clusters are released*/
		udbRemainingClusters++;

		DBG_FUNC("[MemoryManager] mm_Free: Next cluster '%2' \n", maAllocatedAddr);

	}

	if (udbBackup != udbRemainingClusters) {

		DBG_FUNC("[MemoryManager] mm_Free: After releasing them, now total no. of  free clusters are '%2' \n", udbRemainingClusters);

		/*Write no. of free clusters*/
		baseWriteNBytes(ADDRESS_NO_OF_FREE_CLUSTERS, (puMonoByte)&udbRemainingClusters, SIZE_OF_NO_OF_CLUSTER);
	}
	else {
		DBG_FUNC("[MemoryManager] mm_Free: No clusters are released \n");
	}


	DBG_FUNC_EXIT("<<[MemoryManager] mm_Free() \n");

}

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
boolean mm_cmpRam(IN_ARG		tMemAddr	maBlock,
					INOUT_ARG	puMonoByte	pumbData,
					IN_ARG		uDiByte		udbOffset,
					IN_ARG		uDiByte		udbDataSize) {

	uMonoByte	umbDataLength; //length of data to compare in a cluster
	uMonoByte	umbIndex;
	uQuadByte	uqbClusterOffset; // stores the exact offset 

	tClusterHeader chCurrHeader; //stores cluster header

	DBG_FUNC_ENTRY(">>[MemoryManager] mm_cmpRam() \n");

	/*Check address must not be NULL*/
	assert(pumbData != NULL);

	DBG_FUNC("[MemoryManager] mm_cmpRam: maBlock %2  udbOffset %2 udbDataSize %2 \n", maBlock, udbOffset, udbDataSize);

	/*Check if Clusterindex is not invalid one*/
	if (EMPTY_OBJECT == maBlock)
	{
		/*invalid cluster/block index*/
		DBG_WARN("[MemoryManager] mm_cmpRam: Invalid cluster\n");
		DBG_FUNC_EXIT("<<[MemoryManager] mm_cmpRam() returns FALSE i.e. %1\n", FALSE);
		return FALSE;
	}

	do {

		/*check if Offset is less than Cluster size*/
		if (udbOffset < CLUSTER_SIZE) //comparison can be start from current cluster
		{
			DBG_FUNC("[MemoryManager] mm_ReadUpdate: Offset %2 of cluster %2 \n", udbOffset, maBlock);

			/*calculate offset within cluster data*/
			uqbClusterOffset = (sizeof(tCluster) * (maBlock - 1)) + udbOffset;

			/*Check if offset + data size less than cluster size*/
			if ((udbDataSize + udbOffset) < CLUSTER_SIZE) {

				/*if control is here, it means this is last cluster to read, for fetching all data*/
				/*note used data length*/
				umbDataLength = (uMonoByte)udbDataSize;
			}
			else {
				/*if control is here, it means we will read few data from this cluster and rest of the data from
				* remaining clusters
				*/
				/*indicates that cluster is fully used*/
				umbDataLength = CLUSTER_SIZE - udbOffset;
			}

			DBG_FUNC("[MemoryManager] mm_cmpRam: Comparing data of length %1\n", umbDataLength);

			umbIndex = 0x00;
			while (umbIndex < umbDataLength) { // read till the end of cluster or for remianing data length

				if (*pumbData != baseReadOneByte(ADDRESS_CLUSTER_START + uqbClusterOffset + umbIndex)) { // compare bytes of memory data and array

					/*it doesnt match throw error*/
					DBG_WARN("[MemoryManager] mm_cmpRam: Invalid cluster\n");
					DBG_FUNC_EXIT("<<[MemoryManager] mm_cmpRam() returns FALSE i.e. %1\n", FALSE);
					return FALSE;
				}

				/*Increment next length of array*/
				pumbData++;
				umbIndex++;
			}

			/*Reduce size as well*/
			udbDataSize -= umbDataLength;

			DBG_FUNC("[MemoryManager] mm_cmpRam: Remaining Size %2 \n", udbDataSize);

			/*Reset to 0*/
			udbOffset = 0x00;

		}
		else {// it means offset doesnt fall under this cluster

			/*Reduce cluster size from offset as current cluster is skipped*/
			udbOffset -= CLUSTER_SIZE;
		}

		/*calculate offset of cluster header*/
		uqbClusterOffset = sizeof(tClusterHeader) * (maBlock - 1);

		/*Read cluster header from SMART*/
		baseReadNBytes(ADDRESS_SMART_START + uqbClusterOffset, (puMonoByte)&chCurrHeader, sizeof(tClusterHeader));

		/*get next block index*/
		maBlock = chCurrHeader.maNextCluster;

		DBG_FUNC("[MemoryManager] mm_cmpRam: Fetch next cluster %2 \n", maBlock);

	} while ((udbDataSize)					//still there is data to compare
		&&									//AND
		(END_OF_CLUSTER_CHAIN != maBlock)); //havent reached the end of chain yet

	DBG_FUNC_EXIT("<<[MemoryManager] mm_cmpRam() returns TRUE i.e. %1\n", TRUE);

	/*all data has been succesffully compared and are equal*/
	return TRUE;
}
/*
* It prints the Memory related information
*/
void printMemoryConfig(void) {
	DBG_HEAD("Parameter                          : Values\n");
	DBG_WARN("ADDRESS_ERASE_EEPROM_START         : %4\n", ADDRESS_ERASE_EEPROM_START);
	DBG_WARN("ADDRESS_SYSTEM_AREA_START          : %4\n", ADDRESS_SYSTEM_AREA_START);
	DBG_WARN("ADDRESS_DISK_MEMORY_START          : %4\n", ADDRESS_DISK_MEMORY_START);
	DBG_WARN("ADDRESS_DISK_MEMORY_END            : %4\n", ADDRESS_DISK_MEMORY_END);
	DBG_WARN("ADDRESS_SMART_START                : %4\n", ADDRESS_SMART_START);
	DBG_WARN("ADDRESS_SMART_END                  : %4\n", ADDRESS_SMART_END);
	DBG_WARN("ADDRESS_CLUSTER_START              : %4\n", ADDRESS_CLUSTER_START);
	DBG_WARN("ADDRESS_CLUSTER_END                : %4\n", ADDRESS_CLUSTER_END);
}
