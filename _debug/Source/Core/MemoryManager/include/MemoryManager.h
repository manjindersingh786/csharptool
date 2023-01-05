
#ifndef _MEMORY_MANAGER_H
#define _MEMORY_MANAGER_H

#include "types.h"
#include "Oasis.h"
#include "MemoryManager_extern.h"
#include "MemoryLayout.h"
#include "SystemPage.h"
#include "BitDefines.h"
#include "AntiTearing_extern.h"
/*
* Macros Defines
*/

/*
* Sim Memory Allocation Root Table
*/
#define ADDRESS_SMART_START							(ADDRESS_DISK_MEMORY_START)
#define SIZE_SMART									(NO_OF_CLUSTERS * sizeof(tClusterHeader))
#define ADDRESS_SMART_END							(ADDRESS_SMART_START + SIZE_SMART - 1)

/*
* Here all memory divided into clusters
*/
#define ADDRESS_CLUSTER_START						(ADDRESS_SMART_END + 1)
#define SIZE_CLUSTER_MEMORY							(NO_OF_CLUSTERS * CLUSTER_SIZE)
#define ADDRESS_CLUSTER_END							(ADDRESS_CLUSTER_START + SIZE_CLUSTER_MEMORY - 1)


#endif // !_MEMORY_MANAGER_H
