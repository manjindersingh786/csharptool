#include "types.h"
#ifndef _UICC_COMMAND_EXTERN_DISPATCHER_H
#define _UICC_COMMAND_EXTERN_DISPATCHER_H
typedef void (*cmdFunc)(void);


extern cmdFunc _102221_fetchTheCommandFunc(OUT_ARG puMonoByte pumbInput);
#endif // !_UICC_COMMAND_EXTERN_DISPATCHER_H
