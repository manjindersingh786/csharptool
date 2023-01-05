#include "102_222_extern.h"
#include "Oasis.h"
#include "wrappersExtern.h"
#include "statuswords.h"
#include "7816_3_extern.h"
#include "7816_4_extern.h"
#include "ExceptionHandling_extern.h"
#include "BitDefines.h"
#include "102_221_extern.h"
#include "MemoryManager_extern.h"
#include "MemoryLayout.h"
#include "SystemPage.h"
#include "7816_9_extern.h"
#ifndef _102_222_H
#define _102_222_H
/*
* Macro defines
*/
#define FIX_DCB_FOR_102_222								(0x21)
#define DEFAULT_102_222_BYTE							(DEFAULT_BYTE_IN_FILE)


#define IS_HIGH_UPDATE(x)											(x & MASK_HIGH_UPDATE)
#define IS_FILE_RW_WHEN_DEACTIVATE(x)								(x & MASK_READ_UPDATE)
#define IS_FILE_OTA_UPDATABLE(x)                                    (x & MASK_OTA_UPDATE) 
#endif // !_102_222_H