#include "wrappers.h"

/*
* Macros declaration
*/
#define DEFAULT_BYTE								(0xFF)

#ifdef _CHIP_THC80F340A
#define READ_PAGE_TABLE_ENTRY(x)					((pageTableEntry)(*(pPageTableEntry)(x)))
#define READ_SINGLE_BYTE(x)							(*((puMonoByte)(x)))
#else
#define READ_PAGE_TABLE_ENTRY(x)					((pageTableEntry)(*(pPageTableEntry)(SimulGetPhysicalAddr((uQuadByte)(x)))))
#define READ_SINGLE_BYTE(x)							(*((puMonoByte)(SimulGetPhysicalAddr((uQuadByte)(x)))))
#endif 

/*
* Variables declaration
*/
static uMonoByte gumbFlashBuff[PAGE_SIZE];

/*
* Functions declarations
*/
/*Erase the page on given address*/
static boolean  baseErasePageDirect(uQuadByte uqbAddress);

/*Write the data on erased page*/
static boolean  baseWritePageDirect(uQuadByte uqbAddress, puMonoByte pumbInput);

/*
* Erase and Write page data
*
* uqbFlashAddr: Page address (It should be starting address of any page to updated)
* pumbPageData: data buffer to update. must contain page size data.
*/
static boolean updatePage(uQuadByte uqbFlashAddr, puMonoByte pumbPageData) {

	/*Erase the physical destination page first*/
	if (TRUE == baseErasePageDirect(uqbFlashAddr))
	{
		/*Write the Page buffer*/
		if (TRUE == baseWritePageDirect(uqbFlashAddr, pumbPageData))
		{
			/*if writing passed*/
			return TRUE;
		}
	}
	/*If erase or write failed*/
	return FALSE;
}

#ifdef FEAT_EXHAUSTEARING

/*
* Search of give Virtual page value in the provided page table address
*
* ptVirtualPage: Value to search
* udbStartOffset: index from where search starts
* pptPageTable: Address of Page table
*/
static uDiByte searchVirtualPage(pageTableEntry ptVirtualPage, uDiByte udbStartOffset, pPageTableEntry pptPageTable) {

	/*move pointer to Start offset in given Page table*/
	pptPageTable += udbStartOffset;

	/*search for free page (free in both active and backup page table) and given virtual page in active page table*/
	do {
		if (READ_PAGE_TABLE_ENTRY(pptPageTable) == ptVirtualPage) {

			/*Page no. from where data will be copied*/
			break;
		}

		/*increment to next page entry*/
		udbStartOffset++;
		pptPageTable++;

	} while (udbStartOffset < TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE);

	return udbStartOffset;
}
/*
* function to update the data on another free page
*
*/
static boolean WriteNBytes(uQuadByte uqbAddress, puMonoByte pumbInput, uDiByte udbLength) {

	uMonoByte			umbActivePt;
	uMonoByte			umbTxnStateOrBackupPt;
	uMonoByte			umbNewPagesUpdated = 0x00;
	uMonoByte			umbIndex = 0x00;
	uDiByte				udbOffset;
	uDiByte				udbSrcPg = 0x00;
	uDiByte				udbDestPage = 0xFFFF; //Reset it to -1
	uDiByte 			udbRemain;
	uDiByte 			udbToDo = 0x00;
	uDiByte				udbSrcPgs[2]; //Assuming at max only two pages will be updated in single call of this function
	uDiByte				udbDestPgs[2]; //Assuming at max only two pages will be updated in single call of this function
	pageTableEntry		ptVirtualPgs[2]; //Assuming at max only two pages will be updated in single call of this function
	pageTableEntry		ptVirtualPg;
	pPageTableEntry		pptActive;
	pPageTableEntry		pptBackup;

	//DBG_FUNC_ENTRY(">>[READ_UPDATE_NVM] WriteNBytes() \n");

	/*read active page table*/
	umbActivePt = baseReadOneByteDirect(ADDRESS_ACTIVE_PAGE);

	/*Read backup page table*/
	umbTxnStateOrBackupPt = baseReadOneByteDirect(ADDRESS_TRANSACTION_STATE);

	if ((AT_NO_ACTIVE_TXN == umbTxnStateOrBackupPt) || (umbTxnStateOrBackupPt == umbActivePt)) {

		pptActive = (pPageTableEntry)(ADDRESS_PAGE_TABLE + (umbActivePt * SIZE_PAGE_TABLE));
		pptBackup = (pPageTableEntry)(ADDRESS_PAGE_TABLE + (umbActivePt * SIZE_PAGE_TABLE));
	}
	else {
		pptActive = (pPageTableEntry)(ADDRESS_PAGE_TABLE + (umbActivePt * SIZE_PAGE_TABLE));
		pptBackup = (pPageTableEntry)(ADDRESS_PAGE_TABLE + (umbTxnStateOrBackupPt * SIZE_PAGE_TABLE));
	}

	/*Extract page address*/
	ptVirtualPg = (pageTableEntry)GET_PAGE_NUMBER(uqbAddress);

	/*Offset in current virtual page*/
	udbOffset = (uDiByte)(uqbAddress & (PAGE_SIZE - 1));

	for (udbRemain = udbLength; udbRemain; udbRemain -= udbToDo)
	{
		/****************************Antitearing code*****************************************/

		/*********Searching The Given virtual page*********/

		/*Search for Virtual Page in entry*/
		udbSrcPg = searchVirtualPage(ptVirtualPg, 0x00, pptActive);

		if ((TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE > udbSrcPg)/*virtual page found in Active page table*/
			&&
			((AT_NO_ACTIVE_TXN == umbTxnStateOrBackupPt) || ((pageTableEntry)(PAGE_IS_FREE) == READ_PAGE_TABLE_ENTRY(pptBackup + udbSrcPg))))
			/*No transaction in progress
			*	or
			* otherwise check if corresponding offset in backup page is free (indicates that page is newly created in the current transaction)
			*/
		{
			/*Data will be updated in same page*/
			udbDestPage = udbSrcPg;
		}
		else {
			/*search for free page in backup page*/
			do {

				/*searching free page*/
				/*Always start from next page, first time this loop will be called, (udbDestPage + 0x01) results into 00*/
				udbDestPage = searchVirtualPage((pageTableEntry)PAGE_IS_FREE, (udbDestPage + 0x01), pptBackup);

			} while ((TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE > udbDestPage) //if dest page checked the whole page table (Though it will never happend, because it will be ensured that OS maintains minimum number of free pages for AT)
				&& \
				((pageTableEntry)(PAGE_IS_FREE) != READ_PAGE_TABLE_ENTRY(pptActive + udbDestPage))); // check until same page hasnt been found free in active page as well
		}
		/*********************Searching done*************/

		/******************Writing the data start*****************/
		/*If virtual page is found in active PT*/
		if (TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE > udbSrcPg) {

			/*Convert page number into physical page address*/
			/*Read whole page in buffer*/
			baseReadNBytesDirect((uQuadByte)(ADDRESS_NVM_START + GET_ADDRESS(udbSrcPg)), gumbFlashBuff, PAGE_SIZE);

		}
		else {
			/*Indication as source page is not found, to ensure that Page table doesnt get update for this entry*/
			udbSrcPg = INVALID_PAGE_ENTRY_VALUE;

			/*This is first time given Virtual page is updating so default data would be 'FF' */
			mem_set(gumbFlashBuff, 0xFF, PAGE_SIZE);
		}

		/*Calculate data length that we need to update in given page*/
		udbToDo = (udbOffset + udbRemain) < PAGE_SIZE ? udbRemain : (PAGE_SIZE - udbOffset);

		/*Check if data is same so please dont update, for god sake*/
		if (mem_cmp(&gumbFlashBuff[udbOffset], pumbInput, udbToDo)) {

			/*Update new data in the page buffer*/
			mem_cpy(&gumbFlashBuff[udbOffset], pumbInput, udbToDo);

			/*Check if new physical page is different from previous one*/
			if (udbSrcPg != udbDestPage) {

				//DBG_FUNC("[READ_UPDATE_NVM] WriteNBytes(): Mapping Virtual Page %2 to Physical Page %2\n", ptVirtualPg, udbDestPage);
				/*store updating source and destination physical pages for current virtual page update*/
				udbSrcPgs[umbNewPagesUpdated] = udbSrcPg * TABLE_ENTRY_SIZE;
				udbDestPgs[umbNewPagesUpdated] = udbDestPage * TABLE_ENTRY_SIZE;
				ptVirtualPgs[umbNewPagesUpdated++] = ptVirtualPg;

			}

			/*Convert dest page number into physical page address*/
			/*Update physical page*/
			if (FALSE == updatePage((uQuadByte)(ADDRESS_NVM_START + GET_ADDRESS(udbDestPage)), gumbFlashBuff)) {
				return FALSE;
			}
		}

		/*Next offset must be 00*/
		udbOffset = 0x00;//udbOffset = (uDiByte)(udbToDo + udbOffset) & (PAGE_SIZE - 1);

		/*Next address would be next page*/
		ptVirtualPg++;

		/*Increase the offset for input from where next block of data will be read*/
		pumbInput += udbToDo;

		/******************Write data done***/
	}

	/*******************Update Page table*************/
	if (pptActive == pptBackup) {
		/*If first time page table is updating in current transaction
		* Old page table will be active one and new page table will be alternative one
		*/

		/*Which table is active*/
		if ((pPageTableEntry)ADDRESS_PAGE_TABLE_1_START == pptBackup) {

			/*Old table will be PT1 */
			/*New table will be PT2 */
			pptActive = (pPageTableEntry)ADDRESS_PAGE_TABLE_2_START;
		}
		else {
			/*Old table will be PT2 */
			/*New Table will be PT1 */
			pptActive = (pPageTableEntry)ADDRESS_PAGE_TABLE_1_START;
		}

	}
	else {
		/*This isnt first update in transaction, It means backup has already been taken.
		* So old page table and new page table will be same
		*/

		/*Old table = new table*/
		pptBackup = pptActive;
	}

	/*Number of pages processed of Page Table*/
	umbActivePt = 0x00;

	while (umbActivePt < NO_OF_PAGES_FOR_PAGE_TABLE) {

		/*Read first page of PT*/
		baseReadNBytesDirect((uQuadByte)pptBackup, gumbFlashBuff, PAGE_SIZE);

		/*assume page is not updated*/
		umbTxnStateOrBackupPt = PAGE_IS_NOT_UPDATED;

		//copy from old and paste it to new after updating 
		for (umbIndex = 0x00; umbIndex < umbNewPagesUpdated; umbIndex++) {

			//Check if src page fall under current page of Page Table
			if (umbActivePt == (udbSrcPgs[umbIndex] / (uDiByte)(PAGE_SIZE))) {

				/*Update src page as free*/
				ptVirtualPg = (pageTableEntry)PAGE_IS_FREE;

				mem_cpy(&gumbFlashBuff[udbSrcPgs[umbIndex]], (puMonoByte)&ptVirtualPg, TABLE_ENTRY_SIZE);

				umbTxnStateOrBackupPt = PAGE_IS_UPDATED;
			}

			//Check if dest page fall under current page of Page Table
			if (umbActivePt == (udbDestPgs[umbIndex] / (uDiByte)(PAGE_SIZE))) {

				/*Update virtual page in free physical page index of current page*/
				mem_cpy(&gumbFlashBuff[udbDestPgs[umbIndex]], (puMonoByte)&ptVirtualPgs[umbIndex], TABLE_ENTRY_SIZE);

				umbTxnStateOrBackupPt = PAGE_IS_UPDATED;
			}
		}
				
		/*Check if data needs to updated in page table*/
		if ((pptBackup != pptActive)  //Old Page table and new page tables are different
			||  // OR
			(PAGE_IS_UPDATED == umbTxnStateOrBackupPt)) { //If data is updated in current page of Page Tables

			/*Update physical page*/
			if (FALSE == updatePage((uQuadByte)pptActive, gumbFlashBuff)) {
				return FALSE;
			}

		}

		/*Get next page in Old page table*/
		pptBackup += NO_OF_ENTRIES_IN_SINGLE_PAGE;

		/*Get next page in new page table*/
		pptActive += NO_OF_ENTRIES_IN_SINGLE_PAGE;

		/*increment to next page*/
		umbActivePt++;

	}
	/******************* Update Page table done*************/

	/*******************Update Active Page table number************************/
	if (pptBackup != pptActive) {

		if ((pPageTableEntry)(ADDRESS_PAGE_TABLE_1_END + 0x01) == pptActive) {

			/*New active page table is Page Table 1*/
			umbActivePt = AT_PAGE_TABLE_1;
		}
		else {

			/*New active page table is Page Table 2*/
			umbActivePt = AT_PAGE_TABLE_2;
		}

		/*Read page containing active PT*/
		baseReadNBytesDirect(ADDRESS_ACTIVE_PAGE, gumbFlashBuff, PAGE_SIZE);

		/*update active PT in buffer*/
		gumbFlashBuff[0x00] = umbActivePt;

		/*Update buffer in NVM*/
		updatePage(ADDRESS_ACTIVE_PAGE, gumbFlashBuff);
	}
	/******************Update Active Page table number done*****************/

	//DBG_FUNC_EXIT("<<[READ_UPDATE_NVM] WriteNBytes()\n");
	return TRUE;
}

/*
* baseReadOneByte (uQuadByte uqbAddress)
* 
* @param : uqbAddress	: Memory Address To Read From
* @ret: read byte value
*/
uMonoByte baseReadOneByte(uQuadByte uqbAddress){

	uMonoByte		umbActivePt;
	uDiByte			udbPhysicalPg;
	pPageTableEntry pptActive;

	//DBG_FUNC_ENTRY(">>[READ_UPDATE_NVM] baseReadOneByte()\n");
	/*read active page table number*/
	umbActivePt = baseReadOneByteDirect(ADDRESS_ACTIVE_PAGE);

	/*Get starting address of active page table*/
	pptActive = (pPageTableEntry)(ADDRESS_PAGE_TABLE + (umbActivePt * SIZE_PAGE_TABLE));

	/*search for given Virtual Page number*/
	udbPhysicalPg = searchVirtualPage((pageTableEntry)GET_PAGE_NUMBER(uqbAddress), 0x00, pptActive);

	//DBG_FUNC("[READ_UPDATE_NVM] baseReadOneByte():  Reading Virtual Page %2 data from Physical Page %2\n", (pageTableEntry)GET_PAGE_NUMBER(uqbAddress), udbPhysicalPg);

	/*Check if virtual page not found*/
	if (TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE == udbPhysicalPg) {
		
		/*if physical page number is equal to total no. of entries it means virtual page is not found*/
		/*as no prior data available for given address it means return default data which is 'FF'*/
		return DEFAULT_BYTE;

	}

	/*Virtual Page found*/
	/*Get address of physical page and add offset of page in it*/
	uqbAddress = ((uQuadByte)(ADDRESS_NVM_START + GET_ADDRESS(udbPhysicalPg))) | (uqbAddress & (PAGE_SIZE - 1));

	//DBG_FUNC_EXIT("<<[READ_UPDATE_NVM] baseReadOneByte()\n");
	/*Read a byte and return it*/
	return READ_SINGLE_BYTE(uqbAddress);
}



/*
* baseReadNBytes (uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength)
* 
* @param : uqbAddress	: Memory Address To Read From
* @param : pumbOutput	: Pointer To The Buffer Where data Will Be Stored
* @param : udbLength	: How Much Data
*/
void baseReadNBytes(uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength){
	
	uMonoByte		umbActivePt;
	uDiByte			udbPhysicalPg;
	uDiByte			udbOffset;
	uDiByte			udbToDo;
	pageTableEntry	ptVirtualPg;
	pPageTableEntry pptActive;

	//DBG_FUNC_ENTRY(">>[READ_UPDATE_NVM] baseReadNBytes()\n");
	/*read active page table number*/
	umbActivePt = baseReadOneByteDirect(ADDRESS_ACTIVE_PAGE);

	/*Get starting address of active page table*/
	pptActive = (pPageTableEntry)(ADDRESS_PAGE_TABLE + (umbActivePt * SIZE_PAGE_TABLE));

	/*Extract page address*/
	ptVirtualPg = (pageTableEntry)GET_PAGE_NUMBER(uqbAddress);

	/*Offset in current virtual page*/
	udbOffset = (uDiByte)(uqbAddress & (PAGE_SIZE - 1));

	/*Read until no data left*/
	while(udbLength > 0x00)
	{
		/*search for given Virtual Page number*/
		udbPhysicalPg = searchVirtualPage(ptVirtualPg, 0x00, pptActive);

		/*Calculate data length that we can read from given page*/
		udbToDo = (udbOffset + udbLength) < PAGE_SIZE ? udbLength : (PAGE_SIZE - udbOffset);

		//DBG_FUNC("[READ_UPDATE_NVM] baseReadNBytes(): Reading Virtual Page %2 data from Physical Page %2\n", ptVirtualPg, udbPhysicalPg);

		/*Check if virtual page not found*/
		if (TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE == udbPhysicalPg) {

			/*if physical page number is equal to total no. of entries it means virtual page is not found*/
			/*as no prior data available for given page address it means return default data which is 'FF'*/
			mem_set(pumbOutput, DEFAULT_BYTE, udbToDo);
		}
		else {

			/*Virtual Page found*/
			/*Read data from physical address*/
			baseReadNBytesDirect((uQuadByte)((ADDRESS_NVM_START + GET_ADDRESS(udbPhysicalPg) )| udbOffset), pumbOutput, udbToDo);
		}

		/*Next data will always be start from 0th offset*/
		udbOffset = 0x00;

		/*Next address would be next page*/
		ptVirtualPg++;

		/*Increase the offset for output to where next block of data will update*/
		pumbOutput += udbToDo;

		/*Decrease the read data length from remaining length*/
		udbLength -= udbToDo;

	}

	//DBG_FUNC_EXIT("<<[READ_UPDATE_NVM] baseReadNBytes()\n");
}

/*
* baseWriteNBytes(uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength)
* 
* @param : uqbAddress	: Memory Address write to
* @param : pumbOutput	: Pointer To The Buffer Where data Will Be Stored
* @param : udbLength	: How Much Data
*/
void baseWriteNBytes(uQuadByte uqbAddress, puMonoByte pumbInput, uDiByte udbLength){

	boolean bCloseTheTxn = FALSE;

	/*begin the transaction*/
	if (TRUE == at_beginTxn()) {

		/*indication to close the transaction in same function*/
		bCloseTheTxn = TRUE;
	}

	WriteNBytes(uqbAddress, pumbInput, udbLength);

	if (TRUE == bCloseTheTxn) {

		/*commit the txn started in the beginning of function*/
		at_commitTxn();
	}
}

#endif

/*Write direct on the given physical address
* 
* uqbAddress: Destination physical address
* pumbInput: Input buffer
* udbLength: Length to update
*/
boolean baseWriteNBytesDirect(uQuadByte uqbAddress, puMonoByte pumbInput, uDiByte udbLength) {

	boolean		bRet=FALSE;
	uDiByte		udbOffset;
	uDiByte 	udbRemain;
	uDiByte 	udbToDo;
	uDiByte 	udbCounter = 0x00;
	uQuadByte uqbFlashAddr;

	/*extract page address*/
	uqbFlashAddr = uqbAddress & (0xFFFFFE00 | PAGE_SIZE);

	/*extract offset in give page*/
	udbOffset = (uDiByte)(uqbAddress & (PAGE_SIZE - 1));

	for (udbRemain = udbLength; udbRemain; udbRemain -= udbToDo)
	{
		/*Calculate length of data to update in current page*/
		udbToDo = (udbOffset + udbRemain) < PAGE_SIZE ? udbRemain : (PAGE_SIZE - udbOffset);

		/*Read page in buffer*/
		baseReadNBytesDirect(uqbFlashAddr, gumbFlashBuff, PAGE_SIZE);

		/*Check if data is same so please dont update, for god sake*/
		if (mem_cmp(&gumbFlashBuff[udbOffset], &pumbInput[udbCounter], udbToDo)) {

			/*Data is different, now we can go for update*/
			/*copy the buffer*/
			mem_cpy(&gumbFlashBuff[udbOffset], &pumbInput[udbCounter], udbToDo);

			/*Update buffer at destn page*/
			if (FALSE == updatePage(uqbFlashAddr, gumbFlashBuff)) {

				break;
			}
		}

		/*next offset of input buffer*/
		udbCounter += udbToDo;

		/*next offset in next destination page*/
		udbOffset = (uDiByte)(udbToDo + udbOffset) & (PAGE_SIZE - 1);

		/*Next destination page*/
		uqbFlashAddr += PAGE_SIZE;
	}

	return bRet;
}

/*Read direct from the given address*/
void baseReadNBytesDirect(uQuadByte uqbAddress, puMonoByte pumbOutput, uDiByte udbLength) {

#ifdef _CHIP_THC80F340A
	mem_cpy(pumbOutput, (const puMonoByte)uqbAddress, udbLength);
#else
	Simul_ReadNBytesDirect(uqbAddress, pumbOutput, udbLength);
#endif
}


/*Write the page on given address*/
static boolean  baseWritePageDirect(uQuadByte uqbAddress, puMonoByte pumbInput) {

#ifdef _CHIP_THC80F340A
	return Chip_THC80F340A_WritePage(uqbAddress, pumbInput);
#else
	return Simul_WriteNBytes(uqbAddress, pumbInput, PAGE_SIZE);
#endif
}

/*Erase the page on given address*/
static boolean  baseErasePageDirect(uQuadByte uqbAddress) {

#ifdef _CHIP_THC80F340A
	return Chip_THC80F340A_ErasePage(uqbAddress);
#else
	return Simul_ErasePage(uqbAddress);
#endif
}
