
#ifndef _102_221_H
#define _102_221_H
#include "types.h"
#include "102_221_extern.h"
#include "Oasis.h"
#include "BitDefines.h"
#include "ExceptionHandling_extern.h"
#include "channelManager_extern.h"
#include "31_111_extern.h"

#define MASK_VALID_SFI								(0xE0)
#define MASK_SFI									(0x1F)
#define P1_VALID_SFI								(0x80)
#define P1_VALID_OFFSET								(0x00)
#define MASK_OP_MODE								(0x07)
#define MODE_NEXT_RECORD							(0x02)
#define MODE_PREVIOUS_RECORD						(0x03)
#define MODE_ABSOLUTE_RECORD						(0x04)
#define MODE_CURRENT_RECORD							(0x00)
#endif // !_102_221_H
