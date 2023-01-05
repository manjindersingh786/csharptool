
#ifndef _MEMORY_WRAPPERS_H
#define _MEMORY_WRAPPERS_H
#include "types.h"
#include "wrappersExtern.h"

#ifdef WIN32
#include "SimulExtern.h"
#else
#include "ChipConfig.h"
#endif

#ifndef SIZE_CODE
#error "define Size code first"
#endif

#ifdef _CHIP_THC80F340A
	#define ADDRESS_EEPROM_START						(ADDRESS_CHIP_START + SIZE_CODE)
	#define ADDRESS_EEPROM_END							(ADDRESS_CHIP_END)
#else
	#define ADDRESS_EEPROM_START						(ADDRESS_SIMUL_START + SIZE_CODE)
	#define ADDRESS_EEPROM_END							(ADDRESS_SIMUL_END)
#endif


#ifdef FEAT_EXHAUSTEARING
/************************************Anit-Tearing***********************************/

#define AVAILABLE_MEMORY								((ADDRESS_EEPROM_END - ADDRESS_EEPROM_START) + 0x01 )
#define AVAILABLE_TOTAL_NO_OF_PAGES						(AVAILABLE_MEMORY/PAGE_SIZE)

#if (AVAILABLE_TOTAL_NO_OF_PAGES>256)
	#define TABLE_ENTRY_SIZE							(0x02)
#else
	#define TABLE_ENTRY_SIZE							(0x01)
#endif

#define NO_OF_ENTRIES_IN_SINGLE_PAGE					(PAGE_SIZE/TABLE_ENTRY_SIZE)
#define NO_OF_PAGES_FOR_PAGE_TABLE						((AVAILABLE_TOTAL_NO_OF_PAGES/NO_OF_ENTRIES_IN_SINGLE_PAGE) + 0x01)
#define TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE				(AVAILABLE_TOTAL_NO_OF_PAGES)
#define TOTAL_SIZE_IN_PAGE_TABLE						(TOTAL_NO_OF_ENTRIES_IN_PAGE_TABLE * TABLE_ENTRY_SIZE)


#define ADDRESS_TRANSACTION_STATE						(ADDRESS_EEPROM_START) //It always should be first offset of any page
#define ADDRESS_ACTIVE_PAGE								(ADDRESS_TRANSACTION_STATE + PAGE_SIZE) //It always should be first offset of any page

#define SIZE_PAGE_TABLE									(NO_OF_PAGES_FOR_PAGE_TABLE * PAGE_SIZE)

#define ADDRESS_PAGE_TABLE								(ADDRESS_ACTIVE_PAGE + PAGE_SIZE)

//Two page tables: 1 active and other will be backup
#define ADDRESS_PAGE_TABLE_1_START						(ADDRESS_PAGE_TABLE)
#define ADDRESS_PAGE_TABLE_1_END						(ADDRESS_PAGE_TABLE_1_START + SIZE_PAGE_TABLE - 0x01)
#define ADDRESS_PAGE_TABLE_2_START						(ADDRESS_PAGE_TABLE_1_END + 0x01)
#define ADDRESS_PAGE_TABLE_2_END						(ADDRESS_PAGE_TABLE_2_START + SIZE_PAGE_TABLE - 0x01)

/*******************************************************************************************/

#define ADDRESS_NVM_START								(ADDRESS_PAGE_TABLE_2_END + 0x01)

#else
#define ADDRESS_NVM_START								(ADDRESS_EEPROM_START)
#endif

#define ADDRESS_NVM_END									(ADDRESS_EEPROM_END)

#endif // !_MEMORY_WRAPPERS_H
