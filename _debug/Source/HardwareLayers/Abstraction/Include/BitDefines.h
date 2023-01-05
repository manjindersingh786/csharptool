#include "types.h"

#ifndef _BIT_DEFINES_H
#define _BIT_DEFINES_H

#define SHIFT_RIGHT_BY_N_BITS(x,y)				((x)>>(y))
#define SHIFT_LEFT_BY_N_BITS(x,y)				((x)<<(y))

#define MASK_BIT(x)						(0x01<<(x-1))


#endif