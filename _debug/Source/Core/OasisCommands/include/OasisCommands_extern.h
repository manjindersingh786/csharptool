#include "types.h"
#ifndef _OASIS_COMMANDS_EXTERN_H
#define _OASIS_COMMANDS_EXTERN_H

/*
* Macro defines
*/
#define INS_SET_CARD_STATE							(0xD9)
#define INS_GET_MEM_CONFIG							(0xDA)
#define INS_SWITCH_TO_BL						    (0xF0)
extern void oasis_SetCardState(void);
extern void oasis_getMemoryConfig(void);
extern void oasis_switchToBL(void);
#endif //!_OASIS_COMMANDS_EXTERN_H