#include "types.h"
#include "Constants.h"

#include "Debug.h"
#ifndef _OASIS_H
#define _OASIS_H



/*5 bytes of command header + 255 bytes of command data + 1 more byte
* as it is also used for response buffer of 256 max
*/
#define MAX_CMD_LEN								(0x105)

/*Total No. of logical channels supported*/
#define NO_OF_LOGICAL_CHANNELS					(0x04)

/*Cluster size used for SMART: Sim Memory Allocation Root Table mechanism*/
#define CLUSTER_SIZE							(0x20)

#endif // !_OASIS_H

