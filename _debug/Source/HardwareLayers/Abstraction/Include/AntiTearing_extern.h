#ifndef _ANTI_TEARING_EXTERN_H
#define _ANTI_TEARING_EXTERN_H
#include "types.h"
#include "MemoryWrappers.h"
#include "BitDefines.h"


/*
* Macros
*/
#define AT_NO_ACTIVE_TXN												(0xFF)
#define AT_NO_ACTIVE_PAGE												(0xFF)
#define AT_PAGE_TABLE_1													(0x00)
#define AT_PAGE_TABLE_2													(0x01)

#define PAGE_IS_FREE													(0xFFFF)

#if (PAGE_SIZE == 256)
#define EXTRACT_PAGE_NUMBER			(0x08)
#elif  (PAGE_SIZE == 512)
#define EXTRACT_PAGE_NUMBER			(0x09)
#endif



#define INVALID_PAGE_ENTRY_VALUE										(0xFFFF/TABLE_ENTRY_SIZE)
#define FLAG_FREE_PAGE_FOUND											(0x01)
#define FLAG_GIVEN_PAGE_FOUND											(0x02)
#define FREE_AND_GIVEN_PAGE_BOTH_FOUND									(FLAG_FREE_PAGE_FOUND|FLAG_GIVEN_PAGE_FOUND)
#define GET_PAGE_NUMBER(addr)											(SHIFT_RIGHT_BY_N_BITS(addr, EXTRACT_PAGE_NUMBER))
#define GET_ADDRESS(pgnbr)												(SHIFT_LEFT_BY_N_BITS(pgnbr, EXTRACT_PAGE_NUMBER))
#define PAGE_IS_NOT_UPDATED												(0x00)
#define PAGE_IS_UPDATED													(0x01)

/*There is no fix area for Exhaustearing pages, below macro will ensure atleast few pages are spared from NVM area for correctness of Exhaustearing mechanism*/
#ifdef FEAT_EXHAUSTEARING
	#ifndef NO_OF_SPARE_PAGES_FOR_EXHASUTEARING
		#error "What are you doing MAN, Please first define NO_OF_SPARE_PAGES_FOR_EXHASUTEARING to be reserved for Exhaustearing, an anti-tearing mechanism"
	#endif
	
	#define SIZE_EXHAUSTEARING_AREA											(NO_OF_SPARE_PAGES_FOR_EXHASUTEARING * PAGE_SIZE)
#else
	#ifdef NO_OF_SPARE_PAGES_FOR_EXHASUTEARING
		#error "Enable feature FEAT_EXHAUSTEARING as well if you really want to define NO_OF_SPARE_PAGES_FOR_EXHASUTEARING"
	#endif
	#define SIZE_EXHAUSTEARING_AREA											(0x00)
#endif

#ifdef FEAT_EXHAUSTEARING
/*
* It checks if transaction active
*/
extern boolean at_isTxnActive(void);

/*
* Begin Transaction
*/
extern  boolean at_beginTxn(void);

/*
* Commit Transaction
*/
extern void at_commitTxn(void);

/*
* Abort Transaction
*/
extern void at_abortTxn(void);

/*
* OS reset or Initialization
*/
extern void at_osInit(void);
#endif

#endif
