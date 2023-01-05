
#ifndef _MEMORY_LAYOUT_H
#define _MEMORY_LAYOUT_H
#include "types.h"
#include "MemoryWrappers.h"
#include "AntiTearing_extern.h"

/*It stores System related information such as 
* Master file address
*/


 
#define ADDRESS_SYSTEM_AREA_START			(ADDRESS_NVM_START)
#define SYSTEM_AREA							(PAGE_SIZE)
#define ADDRESS_DISK_MEMORY_START			(ADDRESS_SYSTEM_AREA_START + SYSTEM_AREA)
#define ADDRESS_DISK_MEMORY_END				(ADDRESS_NVM_END)

#define SIZE_HEAP_AREA						((ADDRESS_DISK_MEMORY_END - ADDRESS_DISK_MEMORY_START + 1)- (SIZE_EXHAUSTEARING_AREA))
#endif // !_MEMORY_LAYOUT_H
