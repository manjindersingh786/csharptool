
#ifndef _SYSTEM_PAGE_H
#define _SYSTEM_PAGE_H
#include "types.h"
//#include "MemoryManager_extern.h"

#define OFFSET_NEXT_FREE_CLUSTER					(0x00) //relative to 
#define OFFSET_NO_OF_FREE_CLUSTERS				(OFFSET_NEXT_FREE_CLUSTER + SIZE_OF_NO_OF_CLUSTER)
#define OFFSET_MASTER_FILE								(OFFSET_NO_OF_FREE_CLUSTERS + SIZE_OF_MEMORY_ADDRESS)
#define OFFSET_CARD_STATE									(OFFSET_MASTER_FILE + SIZE_OF_FILE_HEADER_ADDR)
#define TOTAL_SYSTEM_DATA_LEN							(OFFSET_CARD_STATE + SIZE_OF_CARD_STATE)

#define ADDRESS_NEXT_FREE_CLUSTER					(ADDRESS_SYSTEM_AREA_START + OFFSET_NEXT_FREE_CLUSTER)
#define ADDRESS_NO_OF_FREE_CLUSTERS				(ADDRESS_SYSTEM_AREA_START + OFFSET_NO_OF_FREE_CLUSTERS)
#define ADDRESS_MASTER_FILE								(ADDRESS_SYSTEM_AREA_START + OFFSET_MASTER_FILE)
#define ADDRESS_CARD_STATE								(ADDRESS_SYSTEM_AREA_START + OFFSET_CARD_STATE)


#endif //!_SYSTEM_PAGE_H