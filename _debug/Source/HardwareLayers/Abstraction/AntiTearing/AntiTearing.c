#include "AntiTearing.h"

#ifdef FEAT_EXHAUSTEARING
/*
* Check if Transaction still in progress
*/
boolean at_isTxnActive(void) {
	
	uMonoByte umbActivePt;

	/*read Txn state*/
	umbActivePt = baseReadOneByteDirect(ADDRESS_TRANSACTION_STATE);

	return ((umbActivePt == AT_NO_ACTIVE_TXN) ? FALSE: TRUE);
}

/*
* Begin Transaction
* 
* Precondition: Txn must not be active before
*/
boolean at_beginTxn(void) {
	
	uMonoByte umbActivePt;

	/*If transaction is already active*/
	if (TRUE == at_isTxnActive()) {

		/*no need backup active PT*/
		return FALSE;
	}
	
	/*read Active Page table number*/
	umbActivePt = baseReadOneByteDirect(ADDRESS_ACTIVE_PAGE);

	/*update active page in txn state, to indicate that txn is in progress and store working page table as well*/
	baseWriteNBytesDirect(ADDRESS_TRANSACTION_STATE, &umbActivePt, 0x01);

	return TRUE;

}

/*
* Commit Transaction
*/
void at_commitTxn(void) {

	uMonoByte umbActivePt = AT_NO_ACTIVE_TXN;

	/*Set TXN inactive*/
	baseWriteNBytesDirect(ADDRESS_TRANSACTION_STATE, &umbActivePt, 0x01);
}

/*
* Abort Transaction
*/
void at_abortTxn(void) {

	uMonoByte umbPrevActivePt;

	/*Previously stable page table number*/
	umbPrevActivePt = baseReadOneByteDirect(ADDRESS_TRANSACTION_STATE);

	/*Update Previously stable Page table number*/
	baseWriteNBytesDirect(ADDRESS_ACTIVE_PAGE, &umbPrevActivePt, 0x01);

	umbPrevActivePt = AT_NO_ACTIVE_TXN;

	/*Set TXN inactive*/
	baseWriteNBytesDirect(ADDRESS_TRANSACTION_STATE, &umbPrevActivePt, 0x01);
}

/*
* OS reset or Initialization
*/
void at_osInit(void) {

	uMonoByte umbActivePt;

	/*Previously stable page table number*/
	umbActivePt = baseReadOneByteDirect(ADDRESS_TRANSACTION_STATE);

	/*check txn state*/
	if (AT_NO_ACTIVE_TXN != umbActivePt) {

		/*There was an active txn prior to the last Reset*/
		
		/*Update Previously stable Page table number*/
		baseWriteNBytesDirect(ADDRESS_ACTIVE_PAGE, &umbActivePt, 0x01);

		umbActivePt = AT_NO_ACTIVE_TXN;

		/*Set TXN inactive*/
		baseWriteNBytesDirect(ADDRESS_TRANSACTION_STATE, &umbActivePt, 0x01);

	}
	else if (AT_NO_ACTIVE_PAGE == baseReadOneByteDirect(ADDRESS_ACTIVE_PAGE)) {

		/*It means this is first time POWER ON in card's life cycle*/
		/*Default active Page table is 1*/
		umbActivePt = AT_PAGE_TABLE_1;

		/*Update first time active Page table number*/
		baseWriteNBytesDirect(ADDRESS_ACTIVE_PAGE, &umbActivePt, 0x01);
	}
}

#endif

