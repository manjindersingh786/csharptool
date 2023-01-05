#include "OSinit.h"

/*n'3 denotes Service of FDN i.e. b5 states Allocation & b6 states Activation*/
#define MASK_SERVICE_ADN								(0x0C)
#define SERVICE_ADN_ACTIVATED							(0x0C)
#define MASK_SERVICE_FDN								(0x30)
#define SERVICE_FDN_ACTIVATED							(0x30)

void osInit_osInit(void) {
	/*will contain next free mem block address*/
	tMemAddr			maFreeClusterAddr=0x00;
	tFileHeaderAddr		fhaDf;
	tFileHeaderAddr		fhaFile;
	tFileLCSI			flState;
	uMonoByte umbSystemPage[TOTAL_SYSTEM_DATA_LEN] = { 0x00 }; //array will be updated on System page if Card is loaded first time

	DBG_FUNC_ENTRY(">>[OSinit] osInit_osInit() \n");

	/*Memory initalization and recovery after reset*/
	baseNvmInit();

	/*Read the next available cluster from system page*/
	baseReadNBytes(ADDRESS_NEXT_FREE_CLUSTER, (puMonoByte) &maFreeClusterAddr, SIZE_OF_MEMORY_ADDRESS);

	/*check if its null*/
	if ((tMemAddr)UNINITIALIZED_ADDR == maFreeClusterAddr)
	{
		DBG_FUNC("[OSinit] osInit_osInit: First time OS loaded, so re-initializing System Page\n");

		/*first time this function is called. hence reset addresses*/
		maFreeClusterAddr = SMART_START_INDEX;

		DBG_FUNC("[OSinit] osInit_osInit: Initial value of Next free cluster %2\n", maFreeClusterAddr);

		/*copy address of first cluster*/
		mem_cpy(umbSystemPage + OFFSET_NEXT_FREE_CLUSTER, (puMonoByte) &maFreeClusterAddr, SIZE_OF_NO_OF_CLUSTER); // using mem_cpy to make it endian independent

		/*As this is first time card loaded free clusters are equal to total no. of clusters*/
		maFreeClusterAddr = (tMemAddr)NO_OF_CLUSTERS;

		DBG_FUNC("[OSinit] osInit_osInit: Initial value of No. of free clusters %2\n", maFreeClusterAddr);

		/*copying free no. of clusters*/
		mem_cpy(umbSystemPage + OFFSET_NO_OF_FREE_CLUSTERS, (puMonoByte) &maFreeClusterAddr, SIZE_OF_MEMORY_ADDRESS); // using mem_cpy to make it endian independent

		/*As this first time OS is called, Card state will be OP_READY*/
		umbSystemPage[OFFSET_CARD_STATE] = (uMonoByte)STATE_OP_READY;

		DBG_FUNC("[OSinit] osInit_osInit: Initial Master file address %2\n", EMPTY_OBJECT);
		
		/*Update all elements of system page data in one write*/
		baseWriteNBytes(ADDRESS_NEXT_FREE_CLUSTER, umbSystemPage, TOTAL_SYSTEM_DATA_LEN);
		
#ifdef _DEBUG
		printMemoryConfig();
#endif // _DEBUG


	}
	
	/*Set Basic Logical Channel contexts*/
	ccContexts[BASIC_LOGICAL_CHANNEL].fhaCurrentDf = _7816_4_getMfAddress();

	//TODO check in NUTS code if we can do it in OS_init()
	if (STATE_INITIALIZED < _7186_4_getCardState()) { // If card is in secured state

		/*Search DFgsm under MF*/
		_7816_4_ifFileExists(FID_TELECOM, ccContexts[BASIC_LOGICAL_CHANNEL].fhaCurrentDf, MODE_FIND_FID, &fhaDf);

		/*Search EFadn under Telecom*/
		_7816_4_ifFileExists(FID_ADN, fhaDf, MODE_FIND_FID, &fhaFile);

		/*get life cycle status*/
		flState = _7816_4_getLifeCycleStatus(fhaFile);

		/*Search DFgsm under MF*/
		_7816_4_ifFileExists(FID_GSM, ccContexts[BASIC_LOGICAL_CHANNEL].fhaCurrentDf, MODE_FIND_FID, &fhaDf);

		/*Search EFsst under GSM*/
		_7816_4_ifFileExists(FID_SST, fhaDf, MODE_FIND_FID, &fhaFile);

		/*check if EFsst is present, only then FDN-BDN procedure to be followed*/
		if (NO_FILE_PRESENT != fhaFile) {

			/*read first byte which contains service n'3 */
			_7816_4_readUpdateFile(fhaFile, umbSystemPage, OFFSET_START, 0x01, ACC_MODE_FILE_READ);

			/*Check if FDN is enabled or nor, Refer Figure-C.6 of 3GPP 51.011*/
			if (
				(SERVICE_FDN_ACTIVATED == (umbSystemPage[0x00] & MASK_SERVICE_FDN)) // FDN allocated and Activated
				&&																//AND
				(
					(SERVICE_ADN_ACTIVATED != (umbSystemPage[0x00] & MASK_SERVICE_ADN)) // !(ADN allocated and Activated)
					||																	// OR
					(flState == LCSI_DEACTIVATED)										// ADN is deactivated/invalidated
					)
				) {

				/*Search EF under GSM*/
				_7816_4_ifFileExists(FID_IMSI, fhaDf, MODE_FIND_FID, &fhaFile);

				/*Deactivate the file as per FDN procedure*/
				_7816_4_setLifeCycleStatus(fhaFile, LCSI_DEACTIVATED);

				/*Search EFloci under GSM*/
				_7816_4_ifFileExists(FID_LOCI, fhaDf, MODE_FIND_FID, &fhaFile);

				/*Deactivate the file as per FDN procedure*/
				_7816_4_setLifeCycleStatus(fhaFile, LCSI_DEACTIVATED);
			}
		}
	}
	/*check whether MF is created or not*/
	if (ccContexts[BASIC_LOGICAL_CHANNEL].fhaCurrentDf != NO_FILE_PRESENT)
	{
		/*Prepare Select command's response for DF*/
		gudbOutputLen = _51011_prepareFileResponse(ccContexts[BASIC_LOGICAL_CHANNEL].fhaCurrentDf, FILE_IS_DF, COMMAND_RESPONSE_DATA);
	}
	DBG_FUNC_EXIT("<<[OSinit] osInit_osInit() \n");
}