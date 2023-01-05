#ifndef _COMMAND_ROUTER_EXTERN_H
#define _COMMAND_ROUTER_EXTERN_H
#include "types.h"

/*It holds info. if received command is FIRST after ATR ???*/
GLOBAL boolean bFirstCommandAfterATR; 

extern void cmdRout_commandRouter(void);
#endif // !_COMMAND_ROUTER_EXTERN_H
