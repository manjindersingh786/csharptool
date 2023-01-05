
#ifndef _UICC_COMMAND_DISPATCHER_H
#define _UICC_COMMAND_DISPATCHER_H
#include "types.h"
#include "UICC_CommandDispatcher_extern.h"
#include "102_221_extern.h"
#include "102_222_extern.h"
#include "51_011_extern.h"
#include "OasisCommands_extern.h"
#include "Debug.h"
#include "31_102_extern.h"
typedef struct {
	/*Function pointer of UICC command*/
	cmdFunc UiccCommandFunction;

	/*Function pointer of SIM command*/
	cmdFunc SimCommandFunction;

	/*Instruction byte of given command*/
	uMonoByte umbInsByte;

	/*Type of Command i.e. Input/Output/OTA*/
	uMonoByte umbTypeOfCommand;

}tCommand;

#endif // !_UICC_COMMAND_DISPATCHER_H
