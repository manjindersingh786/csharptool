#ifndef _CHANNEL_MANAGER_EXTERN_H
#define _CHANNEL_MANAGER_EXTERN_H

#include "types.h"
#include "7816_4_extern.h"
#include "Oasis.h"

/*Macros*/
#define MASK_LOGICAL_CHANNEL_1					(0x02)
#define MASK_LOGICAL_CHANNEL_2					(0x04)
#define MASK_LOGICAL_CHANNEL_3					(0x08)

#define  STATE_ONLY_BASIC_CHANNEL_OPENED		(0x0E)
#define	 NO_OF_RFM_CHANNELS						(0x01)

/*
* Structure declarations
*/
typedef struct {

	/*Current record pointer for selected record file*/
	uMonoByte	umbCurrentRecPtr;

	/*Access modes of Current EF*/
	uMonoByte	umbAccessModes[NO_OF_FILE_OPERATIONS];

	/*Stores current DF*/
	tFileHeaderAddr fhaCurrentDf;

	/*Stores current EF*/
	tFileHeaderAddr fhaCurrentEf;

	/*Stores current ADF*/
	tFileHeaderAddr fhaCurrentAdf;

}tChannelContext;

/*
* Variables declaration
*/
// All channles context to be maintained parallely, hence created below array
GLOBAL tChannelContext	ccContexts[NO_OF_LOGICAL_CHANNELS + NO_OF_RFM_CHANNELS];

//stores the availability of Logical Channels
extern uMonoByte gumbChannelsState;

#endif // !_CHANNEL_MANAGER_EXTERN_H
